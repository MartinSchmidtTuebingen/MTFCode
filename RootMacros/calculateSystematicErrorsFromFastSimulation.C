// Calculates the systematic errors for the mc correction. Needs input files from writeOutCorrectionFiles.C

// TODO: catch files not present
// TODO: Loop through variations in this file! Best load json info directly 

#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TLine.h"
#include "TF1.h"
#include "TProfile.h"
#include "TPaveText.h"
#include "TMath.h"
#include "TPaletteAxis.h"
#include "TSystem.h"
#include "TMath.h"
#include "THnSparseDefinitions.h"

#include <iostream>
#include <fstream>

const Int_t nModes = 5;

const TString modeString[nModes] = {"TrackPt", "Z", "Xi", "R", "jT"};
Bool_t useModes[nModes] = {kTRUE, kTRUE, kTRUE, kTRUE, kTRUE};

const Bool_t useLogX[nModes] = {kTRUE, kFALSE, kFALSE, kFALSE, kTRUE};
const TString xAxeTitles[nModes] = {"#it{p}_{T} (GeV/#it{c})", "#it{z}", "#it{#xi}", "R", "j_{T} (GeV/#it{c})"};

Int_t nJetPtBins = 0;
Int_t* jetPtLimits = 0x0;

const Int_t nSpecies = 5;
const TString strTitSp[nSpecies] = {"h^{+} + h^{-}","#pi^{+} + #pi^{-}","K^{+} + K^{-}","p + #bar{p}","e^{+} + e^{-}"};//,"#mu^{+} + #mu^{-}"}; 
const TString strSpeciesNames[nSpecies] = {"","_pi","_K","_p","_e"};//,"_mu"}; 
  
// --------------------------------------------------

void setHistoStyleColor(TH1* hist, Bool_t isSpectrum = kFALSE, Int_t color=2) {

  hist->SetLineColor(1);
  hist->SetMarkerStyle(20);
  hist->SetMarkerSize(0.7);
  hist->SetMarkerColor(color);
  hist->SetTitleSize(0.08);
  
  hist->GetXaxis()->SetTitleSize(0.06);
  hist->GetYaxis()->SetTitleSize(0.06); 
  hist->GetXaxis()->SetLabelSize(0.05);
  hist->GetYaxis()->SetLabelSize(0.05);
  hist->GetXaxis()->SetTitleOffset(1.0); 
	hist->GetYaxis()->SetTitleOffset(isSpectrum ? 0.8 : 0.7); 

  hist->GetXaxis()->SetTitleFont(62); 
  hist->GetYaxis()->SetTitleFont(62); 
  hist->GetXaxis()->SetLabelFont(62); 
  hist->GetYaxis()->SetLabelFont(62); 


  /*
  hist->GetXaxis()->SetTitleSize(0.04);
  hist->GetYaxis()->SetTitleSize(0.04); 
  hist->GetYaxis()->SetTitleOffset(1.0); 
  */
}

TLegend* createLegend(Int_t type)
{
  TLegend* leg = 0x0;
  if (type == 0)
    leg = new TLegend(0.55,0.20,0.96,0.41);
  else if (type == 1)
    leg = new TLegend(0.49,0.63,0.83,0.84);
  else
    leg = new TLegend(0.55,0.61,0.96,0.83);
  
  leg->SetTextSize(0.05);
  leg->SetFillColor(0);
  leg->SetBorderSize(1);
  
  return leg;
}

// ---------------------------------------------------


TCanvas* createCanvas(TString name, Bool_t isSpectrum = kFALSE)
{
  TCanvas* c = new TCanvas(name.Data(),"",760,420); 
  c->Divide(2,2);
  
  for (Int_t i = 1; i <= 4; i++) {
    c->GetPad(i)->SetLeftMargin(isSpectrum ? 0.1 : 0.081);
    c->GetPad(i)->SetRightMargin(0.01);
    c->GetPad(i)->SetBottomMargin(0.13);
    c->GetPad(i)->SetTopMargin(0.12);
  }
  
  return c;
}

// ---------------------------------------------------

Int_t getColor(Int_t col){
  
  Int_t color = kBlack;

  switch(col)
    {
    case 0 : color = kBlue;    break;
    case 1 : color = kCyan;    break;
    case 2 : color = kGreen;   break;
    case 3 : color = kOrange;  break;
    case 4 : color = kRed;     break;
    case 5 : color = kPink;    break;
    case 6 : color = kMagenta; break;
    case 7 : color = kViolet;  break;
    case 8 : color = kBlue-2;  break;
    case 9 : color = kGreen-2;  break;
    case 10: color = kRed+2;  break;
    }

  return color;
  
}

// -------------------------------------------------------------------------
TList* getList(TString name = "jets_noGenJets_trackTypeUndef_jetTypeUndef"){

  TList* list = 0x0; 

  if(gDirectory->Get("obusch_list")){
    list = (TList*) gDirectory->Get("obusch_list");
  }
  else{
    
    TString dirName  = "PWGJE_FragmentationFunction_" + name;
    TString listName = "fracfunc_" + name;
    
    gDirectory->cd(dirName);
    list = (TList*) gDirectory->Get(listName);
        //gDirectory->cd("PWG4_FragmentationFunction_jets_noGenJets_KINEb_KINEb");
    //list = (TList*) gDirectory->Get("fracfunc_jets_noGenJets_KINEb_KINEb");
  }

  return list;
}

void createSystematicErrorsFromFiles(TString filePathDown, TString filePathUp, TString modeNameDown, TString modeNameUp, TH1F** corrFacOriginalMC, TH1F** corrFacGenericFastSimulation, TH1F** fh1SpectraGenericFastSimulation, TString systematicHistogramIdentifier, TString shortVariationDescription, Bool_t simpleCalculationSysError, TString fileSysErrorsPath);

Int_t index(Int_t jetPtBin, Int_t mode, Int_t species) {
  return jetPtBin * (nModes * nSpecies) + mode * nSpecies + species;
}

// ----------------------------------------------------------------------------

void calculateSystematicErrorsFromFastSimulation(TString fileDir = "files", TString saveDir = "files", TString originalMCFile = "corrections_LHC13b2_efix_p1.root", TString genericFastSimulationFile = "outCorrections_FastJetSimulation_Eff100_Res100.root", TString variationShortName = "Eff", TString variationLegendEntry = "Efficiency +/- 5%", Bool_t simpleCalculationSysError = kTRUE, TString variationDownFile = "outCorrections_FastJetSimulation_Eff095_Res100.root", TString variationDownName = "Eff095", TString variationUpFile = "outCorrections_FastJetSimulation_Eff105_Res100.root", TString variationUpName = "Eff105", TString jetPtStepsString = "", TString modesInputString = "") {
  
  if (variationShortName == "") {
      std::cout << "Variation entry needed" << std::endl;
  }
  
  if (jetPtStepsString != "") {
    TObjArray* jetPtBins = jetPtStepsString.Tokenize(";");
    if (!TMath::Even(jetPtBins->GetEntriesFast())) {
      cout << "Attention: jetPtStepsString has to have even number of entries!" << endl;
    } else {
      nJetPtBins = jetPtBins->GetEntriesFast()/2;
      delete jetPtLimits;
      jetPtLimits = new Int_t[2*nJetPtBins];
      for (Int_t i=0;i<2*nJetPtBins;i++) {
        jetPtLimits[i] = getStringFromTObjStrArray(jetPtBins, i).Atoi();
      }
    }
  }
  
  if (!nJetPtBins) {
    // Standard jet Bins
    nJetPtBins = 5;
    jetPtLimits = new Int_t[2*nJetPtBins];
    jetPtLimits[0] = 5;
    jetPtLimits[1] = 10;
    jetPtLimits[2] = 10;
    jetPtLimits[3] = 15;
    jetPtLimits[4] = 15;
    jetPtLimits[5] = 20;
    jetPtLimits[6] = 20;
    jetPtLimits[7] = 30;
    jetPtLimits[8] = 30;
    jetPtLimits[9] = 80;
  }
  
  if (modesInputString != "") {
    TObjArray* modeArray = modesInputString.Tokenize(";");
    Int_t nActivatedModes = modeArray->GetEntriesFast();

    for (Int_t j=0;j<nModes;j++) {
      useModes[j] = kFALSE;
      for (Int_t i=0;i<nActivatedModes;i++) {
        TString activatedMode = getStringFromTObjStrArray(modeArray, i);
        activatedMode = activatedMode.CompareTo("pt", TString::kIgnoreCase) == 0 ? "TrackPt" : activatedMode;
        
        if (activatedMode.CompareTo(modeString[j], TString::kIgnoreCase) == 0) {
          useModes[j] = kTRUE;
        }
      }
    }
  }
  
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleY(1.0);
  gStyle->SetTitleH(0.06);//0.054);
  
  TH1F** corrFacOriginalMC = new TH1F*[nJetPtBins * nModes * nSpecies];
  
  TH1F** fh1SpectraGenericFastSimulation = new TH1F*[nJetPtBins * nModes * nSpecies];
  TH1F** corrFacGenericFastSimulation = new TH1F*[nJetPtBins * nModes * nSpecies];
 
  // Load original MC results
  TFile f1(Form("%s/%s",fileDir.Data(), originalMCFile.Data()),"READ");
  if (f1.IsZombie()) {
    cout << "Could not open full simulation file " << Form("%s/%s",fileDir.Data(), originalMCFile.Data()) << ", no comparison shown." << endl;
  }
  else {
    for (Int_t sp=0; sp<nSpecies; sp++) {
      for (Int_t i=0; i<nJetPtBins; i++) {
        for (Int_t mode=0;mode<nModes;++mode) {
          if (!useModes[mode])
            continue;
          
          TString strTitle(Form("hBbBCorr%s%s_%02d_%02d",modeString[mode].Data(),strSpeciesNames[sp].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));        
          corrFacOriginalMC[index(i, mode, sp)] = (TH1F*) gDirectory->Get(strTitle);
          corrFacOriginalMC[index(i, mode, sp)]->SetDirectory(0);
        }
      }
    }   
    f1.Close();  
  }
  
  // Load generic fast simulation results
  TFile f2(Form("%s/%s", fileDir.Data(), genericFastSimulationFile.Data()),"READ");
  // Load fast MC results particle level
  for (Int_t sp=0; sp<nSpecies; sp++) {
    gDirectory->cd(Form("Gen%s",strSpeciesNames[sp].Data()));

    for(Int_t i=0; i<nJetPtBins; i++){
    
      // for genLevel doesn't matter which histos we take - eff == 1 
      for (Int_t mode=0;mode<nModes;++mode) {
        if (!useModes[mode])
          continue;
        
        TString strTitle(Form("fh1FF%sGen%s_%02d_%02d",modeString[mode].Data(),strSpeciesNames[sp].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));
        
        fh1SpectraGenericFastSimulation[index(i, mode, sp)] = (TH1F*) gDirectory->Get(strTitle);
        
        fh1SpectraGenericFastSimulation[index(i, mode, sp)]->SetDirectory(0);
      }
    }
    gDirectory->cd("..");
  }
  
  // Load fast MC results detector level. Calculate generic correction factors. Format histogram

  for (Int_t sp=0; sp<nSpecies; sp++) {
    
    gDirectory->cd(Form("RecCuts%s",strSpeciesNames[sp].Data()));
    
    for(Int_t i=0; i<nJetPtBins; i++){
      for (Int_t mode=0;mode<nModes;++mode) {
        if (!useModes[mode])
          continue;
        
        TString strTitleRec(Form("fh1FF%sRecCuts%s_%02d_%02d",modeString[mode].Data(),strSpeciesNames[sp].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));
        
        TH1F* fInput = (TH1F*) gDirectory->Get(strTitleRec);
        
        // -- corr factors
        
        TString strTit(Form("corrFac%s_GenericFastSimulation_%s_%d",modeString[mode].Data(), strSpeciesNames[sp].Data(), i));
        
        corrFacGenericFastSimulation[index(i, mode, sp)] = (TH1F*) fh1SpectraGenericFastSimulation[index(i, mode, sp)]->Clone(strTit);
        corrFacGenericFastSimulation[index(i, mode, sp)]->Divide(fh1SpectraGenericFastSimulation[index(i, mode, sp)],fInput,1,1,"B");
        
        /*********/
        
        corrFacGenericFastSimulation[index(i, mode, sp)]->SetDirectory(0); 
        
        delete fInput;
        fInput = 0x0;
        
        // Format histogram
        if (i!=0)
          continue;
        TString strPlotTitle(Form("%s, #it{p}_{T}^{jet, ch} = %d-%d GeV/#it{c}",strTitSp[sp].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));
        
        corrFacGenericFastSimulation[index(i, mode, sp)]->SetTitle(strPlotTitle.Data());
        corrFacGenericFastSimulation[index(i, mode, sp)]->SetXTitle(xAxeTitles[mode].Data());
        corrFacGenericFastSimulation[index(i, mode, sp)]->SetYTitle("Correction Factor");
        setHistoStyleColor(corrFacGenericFastSimulation[index(i, mode, sp)],4);
        corrFacGenericFastSimulation[index(i, mode, sp)]->GetXaxis()->SetRangeUser(0.0, corrFacOriginalMC[index(i, mode, sp)]->GetBinLowEdge(corrFacOriginalMC[index(i, mode, sp)]->FindLastBinAbove() + 1));
        
        Double_t minY = TMath::Min(corrFacOriginalMC[index(i, mode, sp)]->GetBinContent(corrFacOriginalMC[index(i, mode, sp)]->GetMinimumBin()), corrFacGenericFastSimulation[index(i, mode, sp)]->GetBinContent(corrFacGenericFastSimulation[index(i, mode, sp)]->GetMinimumBin()));
        Double_t maxY = TMath::Max(corrFacOriginalMC[index(i, mode, sp)]->GetBinContent(corrFacOriginalMC[index(i, mode, sp)]->GetMaximumBin()), corrFacGenericFastSimulation[index(i, mode, sp)]->GetBinContent(corrFacGenericFastSimulation[index(i, mode, sp)]->GetMaximumBin()));
       
        corrFacGenericFastSimulation[index(i, mode, sp)]->GetYaxis()->SetRangeUser(0.7 * minY, TMath::Min(3.0, 1.3 * maxY));
        
        corrFacGenericFastSimulation[index(i, mode, sp)]->SetLineColor(4);
      }
      
    }
    gDirectory->cd("..");
  }               
  f2.Close();
  
  // Do variation
  createSystematicErrorsFromFiles(Form("%s/%s",fileDir.Data(), variationDownFile.Data()), Form("%s/%s",fileDir.Data(), variationUpFile.Data()), variationDownName, variationUpName, corrFacOriginalMC, corrFacGenericFastSimulation, fh1SpectraGenericFastSimulation, variationShortName, variationLegendEntry, simpleCalculationSysError, saveDir);
  
//   // Efficiency
//   createSystematicErrorsFromFiles(Form("%s/outCorrections_FastJetSimulation_Eff095_Res100.root",fileDir.Data()), Form("%s/outCorrections_FastJetSimulation_Eff105_Res100.root",fileDir.Data()), "Eff095", "Eff105", corrFacOriginalMC, corrFacGenericFastSimulation, fh1SpectraGenericFastSimulation, "Eff", "Efficiency +/- 5%", kTRUE, saveDir);
//   
//   // Resolution
//   createSystematicErrorsFromFiles(Form("%s/outCorrections_FastJetSimulation_Eff100_Res080.root",fileDir.Data()), Form("%s/outCorrections_FastJetSimulation_Eff100_Res120.root",fileDir.Data()), "Res080", "Res120", corrFacOriginalMC, corrFacGenericFastSimulation, fh1SpectraGenericFastSimulation, "Res", "Resolution +/- 20%", kTRUE, saveDir);
//   
//   // Jet Shape
//   createSystematicErrorsFromFiles(Form("%s/outCorrections_PythiaFastJet_LowPtEnhancement.root",fileDir.Data()), Form("%s/outCorrections_FastJetSimulation_LowPtDepletion.root",fileDir.Data()), "LowPtEnhancement", "LowPtDepletion", corrFacOriginalMC, corrFacGenericFastSimulation, fh1SpectraGenericFastSimulation, "BbB", "Low-pt enhanced/depleted", kFALSE, saveDir);
}

void createSystematicErrorsFromFiles(TString filePathDown, TString filePathUp, TString modeNameDown, TString modeNameUp, TH1F** corrFacOriginalMC, TH1F** corrFacGenericFastSimulation, TH1F** fh1SpectraGenericFastSimulation, TString systematicHistogramIdentifier, TString shortVariationDescription, Bool_t simpleCalculationSysError, TString fileSysErrorsPath)
{
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleY(1.0);
  gStyle->SetTitleH(0.06);//0.054);
  
  const Int_t nSysVariations = 2;
	
	TString sysVariationFiles[nSysVariations] = {filePathDown, filePathUp};
	TString namesSysVariations[nSysVariations] = {modeNameDown, modeNameUp};
  
  TH1F** fh1SysVariationGenPrim = new TH1F*[nJetPtBins * nModes * nSpecies * nSysVariations];
	TH1F** fh1SysVariationRecPrim = new TH1F*[nJetPtBins * nModes * nSpecies * nSysVariations];
	TH1F** corrFactorsVariations = new TH1F*[nJetPtBins * nModes * nSpecies * nSysVariations];
	TH1F** systematicVariationCorrFactor = new TH1F*[nJetPtBins * nModes * nSpecies];
	TH1F** hSystematicError = new TH1F*[nJetPtBins * nModes * nSpecies];
	
	//TODO: catch files not present
  // Load fast MC results particle level
  
	for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;++sysVariationMode) {
		TFile f(sysVariationFiles[sysVariationMode],"READ");
    // Load particle level spectra
		for (Int_t sp=0; sp<nSpecies; sp++) {
			gDirectory->cd(Form("Gen%s",strSpeciesNames[sp].Data()));

      for(Int_t i=0; i<nJetPtBins; i++){
      
        // for genLevel doesn't matter which histos we take - eff == 1 
        for (Int_t mode=0;mode<nModes;++mode) {
          if (!useModes[mode])
            continue;
          
          TString strTitle(Form("fh1FF%sGen%s_%02d_%02d",modeString[mode].Data(),strSpeciesNames[sp].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));
          
          fh1SysVariationGenPrim[index(i, mode, sp) * nSysVariations + sysVariationMode] = (TH1F*) gDirectory->Get(strTitle);
          
          fh1SysVariationGenPrim[index(i, mode, sp) * nSysVariations + sysVariationMode]->SetDirectory(0);
        }
      }
      gDirectory->cd("..");
    }
    // Load detector level spectra
    for (Int_t sp=0; sp<nSpecies; sp++) {
      gDirectory->cd(Form("RecCuts%s",strSpeciesNames[sp].Data()));

      for(Int_t i=0; i<nJetPtBins; i++){
        for (Int_t mode=0;mode<nModes;++mode) {
          if (!useModes[mode])
              continue;
          
          TString strTitle(Form("fh1FF%sRecCuts%s_%02d_%02d",modeString[mode].Data(),strSpeciesNames[sp].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));
          
          fh1SysVariationRecPrim[index(i, mode, sp) * nSysVariations + sysVariationMode] = (TH1F*) gDirectory->Get(strTitle);
          
          fh1SysVariationRecPrim[index(i, mode, sp) * nSysVariations + sysVariationMode]->SetDirectory(0);
        }
      }
      gDirectory->cd("..");
    }
    f.Close();
		
    
		for (Int_t sp=0; sp<nSpecies; sp++) {
			for(Int_t i=0; i<nJetPtBins; i++){
				for (Int_t mode=0;mode<nModes;++mode) {
					if (!useModes[mode])
						continue;
					
					TString strTit(Form("corrFac%s_%s_%s_%d", modeString[mode].Data(), namesSysVariations[sysVariationMode].Data(), strSpeciesNames[sp].Data(), i));
					
					corrFactorsVariations[index(i, mode, sp) * nSysVariations + sysVariationMode] = (TH1F*) fh1SysVariationGenPrim[index(i, mode, sp) * nSysVariations + sysVariationMode]->Clone(strTit);
					corrFactorsVariations[index(i, mode, sp) * nSysVariations + sysVariationMode]->Divide(fh1SysVariationGenPrim[index(i, mode, sp) * nSysVariations + sysVariationMode],fh1SysVariationRecPrim[index(i, mode, sp) * nSysVariations + sysVariationMode],1,1,"B");
				}
			}
		}
	}

	// Calculate systematic errors
	for (Int_t sp=0; sp<nSpecies; sp++) {
		for(Int_t i=0; i<nJetPtBins; i++) {
			for (Int_t mode=0;mode<nModes;++mode) {
				if (!useModes[mode])
					continue;
				
				TString strNameCorrSysErrBbB(Form("corrFac%sSysEff_%02d_%02d%s", modeString[mode].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1], strSpeciesNames[sp].Data()));
				TString strNamehSystematicError(Form("hSysErr%s%s_%02d_%02d%s", systematicHistogramIdentifier.Data(), modeString[mode].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1], strSpeciesNames[sp].Data()));
				
				systematicVariationCorrFactor[index(i, mode, sp)] = (TH1F*) corrFacGenericFastSimulation[index(i, mode, sp)]->Clone(strNameCorrSysErrBbB);
				hSystematicError[index(i, mode, sp)] =  (TH1F*) corrFacGenericFastSimulation[index(i, mode, sp)]->Clone(strNamehSystematicError);
				hSystematicError[index(i, mode, sp)]->Reset();
				
				for (Int_t bin=1;bin<=corrFacGenericFastSimulation[index(i, mode, sp)]->GetNbinsX();++bin) {
          Double_t sysErr = 0.0; 
          Double_t sysErrScaled = 0.0;
          systematicVariationCorrFactor[index(i, mode, sp)]->SetBinError(bin, 0);
          Double_t cont = systematicVariationCorrFactor[index(i, mode, sp)]->GetBinContent(bin);
          if (simpleCalculationSysError) {
            sysErr = 0.5 * TMath::Abs(corrFactorsVariations[index(i, mode, sp) * nSysVariations + 0]->GetBinContent(bin) - 
              corrFactorsVariations[index(i, mode, sp) * nSysVariations + 1]->GetBinContent(bin));
            
            if (cont)
              sysErrScaled = sysErr/cont;
          } else {
            if (fh1SpectraGenericFastSimulation[index(i, mode, sp)]->GetBinContent(bin) == 0)
              continue;
            
            Double_t minEl = cont;
            Double_t maxEl = minEl;
            
            for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;sysVariationMode++) {
              Double_t binContent = corrFactorsVariations[index(i, mode, sp) * nSysVariations + sysVariationMode]->GetBinContent(bin);
              minEl = TMath::Min(minEl, binContent);
              maxEl = TMath::Max(maxEl, binContent);
            }
            
            if (minEl) 
              sysErr = 0.5*fabs((maxEl/minEl) - 1);
            
            sysErrScaled = sysErr;
          }
          
          systematicVariationCorrFactor[index(i, mode, sp)]->SetBinError(bin, sysErr);
          
          if (corrFacOriginalMC[index(i, mode, sp)] && corrFacOriginalMC[index(i, mode, sp)]->GetBinContent(bin) == 0) 
            continue; 
          
          hSystematicError[index(i, mode, sp)]->SetBinContent(bin, sysErrScaled);
          hSystematicError[index(i, mode, sp)]->SetBinError(bin, 0);
        }
      }
    }
  }
  
  TFile* fFileSysErrors = new TFile(Form("%s/outSysErr_%s.root", fileSysErrorsPath.Data(), systematicHistogramIdentifier.Data()), "RECREATE");
  
  for (Int_t sp=0; sp<nSpecies; sp++) {
    for (Int_t i=0; i<nJetPtBins; i++) {
      for (Int_t mode=0;mode<nModes;mode++) {  
        if (!useModes[mode] || !hSystematicError[index(i, mode, sp)])
          continue;  
        
        hSystematicError[index(i, mode, sp)]->Write();
      }
    }
  }
	
	fFileSysErrors->Close();
	
	gStyle->SetOptStat(0);
  
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleY(1.0);
  gStyle->SetTitleH(0.06);
	
	const Int_t sysVariationModesColor[nSysVariations] = {51, 8};
	const TString sysVariationModesNames[nSysVariations] = {"Low-pt enhanced", "Low-pt depleted"};
  const Int_t nSpeciesPlot = 4; //Do not plot electron
	
	TFile* qaPlotsFile = new TFile(Form("%s/qaPlots_%s.root", fileSysErrorsPath.Data(), systematicHistogramIdentifier.Data()),"RECREATE");
  for(Int_t i=0; i<nJetPtBins; i++) {
    for (Int_t mode=0;mode<nModes;mode++) {
      if (!useModes[mode])
        continue;
      
      TCanvas* c1 = createCanvas(Form("c%d", mode), kTRUE);
      TLegend* leg1 = createLegend(0);
      
      TCanvas* c2 = createCanvas(Form("c%d", mode + nModes));
      TLegend* leg2 = createLegend(2);

      for(Int_t sp=0; sp<nSpeciesPlot; sp++) {
				c1->cd(sp+1)->SetLogx(useLogX[mode]);

				TString strPlotTitle(Form("%s, #it{p}_{T}^{jet, ch} = %d-%d GeV/#it{c}", strTitSp[sp].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));
				
				fh1SpectraGenericFastSimulation[index(i, mode, sp)]->SetTitle(strPlotTitle);
				fh1SpectraGenericFastSimulation[index(i, mode, sp)]->SetXTitle(xAxeTitles[mode].Data());
				fh1SpectraGenericFastSimulation[index(i, mode, sp)]->SetYTitle(Form("1/#it{N}_{Jets} d#it{N}/d#it{%s}", xAxeTitles[mode].Data()));
				setHistoStyleColor(fh1SpectraGenericFastSimulation[index(i, mode, sp)], kTRUE, 2);
				Int_t lastBinWithContent = fh1SpectraGenericFastSimulation[index(i, mode, sp)]->FindLastBinAbove(0.0);
				fh1SpectraGenericFastSimulation[index(i, mode, sp)]->GetXaxis()->SetRange(1,lastBinWithContent);

				Double_t maxValue = fh1SpectraGenericFastSimulation[index(i, mode, sp)]->GetBinContent(fh1SpectraGenericFastSimulation[index(i, mode, sp)]->GetMaximumBin());

				for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;++sysVariationMode) {
					maxValue = TMath::Max(maxValue, fh1SysVariationGenPrim[index(i, mode, sp) * nSysVariations + sysVariationMode]->GetBinContent(fh1SysVariationGenPrim[index(i, mode, sp) * nSysVariations + sysVariationMode]->GetMaximumBin()));
				}
				
				fh1SpectraGenericFastSimulation[index(i, mode, sp)]->GetYaxis()->SetRangeUser(0,maxValue *1.5);
				fh1SpectraGenericFastSimulation[index(i, mode, sp)]->DrawCopy();  
				for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;++sysVariationMode) {
					setHistoStyleColor(fh1SysVariationGenPrim[index(i, mode, sp) * nSysVariations + sysVariationMode],kTRUE, sysVariationModesColor[sysVariationMode]);
					fh1SysVariationGenPrim[index(i, mode, sp) * nSysVariations + sysVariationMode]->DrawCopy("same");
				}
				fh1SpectraGenericFastSimulation[index(i, mode, sp)]->DrawCopy("same"); //redraw on top  
   						
				if(sp==0){
					leg1->AddEntry(fh1SpectraGenericFastSimulation[index(i, mode, sp)],"Reference, gen level","P");
					for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;++sysVariationMode) {
						leg1->AddEntry(fh1SysVariationGenPrim[index(i, mode, sp) * nSysVariations + sysVariationMode],sysVariationModesNames[sysVariationMode].Data(),"P");
					}
					
					leg1->Draw();
				}
				
				gPad->RedrawAxis("");
				gPad->RedrawAxis("G");
				
				c2->cd(sp+1)->SetLogx(useLogX[mode]);

				corrFacGenericFastSimulation[index(i, mode, sp)]->SetTitle(strPlotTitle);
				corrFacGenericFastSimulation[index(i, mode, sp)]->SetXTitle(xAxeTitles[mode].Data());
				corrFacGenericFastSimulation[index(i, mode, sp)]->SetYTitle("Correction Factor");
				setHistoStyleColor(corrFacGenericFastSimulation[index(i, mode, sp)],kFALSE,4);
				lastBinWithContent = corrFacGenericFastSimulation[index(i, mode, sp)]->FindLastBinAbove(0.0);
				corrFacGenericFastSimulation[index(i, mode, sp)]->GetXaxis()->SetRange(1,lastBinWithContent);
				corrFacGenericFastSimulation[index(i, mode, sp)]->SetLineColor(4); 
				maxValue = corrFacGenericFastSimulation[index(i, mode, sp)]->GetBinContent(corrFacGenericFastSimulation[index(i, mode, sp)]->GetMaximumBin());
				
				setHistoStyleColor(systematicVariationCorrFactor[index(i, mode, sp)],kFALSE,4);
				systematicVariationCorrFactor[index(i, mode, sp)]->SetFillColor(7);
				maxValue = TMath::Max(maxValue, systematicVariationCorrFactor[index(i, mode, sp)]->GetBinContent(systematicVariationCorrFactor[index(i, mode, sp)]->GetMaximumBin()));
				
				setHistoStyleColor(corrFacOriginalMC[index(i, mode, sp)],kFALSE,2);
				corrFacOriginalMC[index(i, mode, sp)]->SetMarkerStyle(24);
				maxValue = TMath::Max(maxValue, corrFacOriginalMC[index(i, mode, sp)]->GetBinContent(corrFacOriginalMC[index(i, mode, sp)]->GetMaximumBin()));
				
				corrFacGenericFastSimulation[index(i, mode, sp)]->GetYaxis()->SetRangeUser(0.0,TMath::Min(3.0, maxValue * 1.2));
				corrFacGenericFastSimulation[index(i, mode, sp)]->DrawCopy();  
				systematicVariationCorrFactor[index(i, mode, sp)]->DrawCopy("same,E2");
				corrFacOriginalMC[index(i, mode, sp)]->DrawCopy("same");

        for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;sysVariationMode++) {
          corrFactorsVariations[index(i, mode, sp) * nSysVariations + sysVariationMode]->DrawCopy("same");
        }
				
				if(sp==0){
					leg2->AddEntry(corrFacOriginalMC[index(i, mode, sp)], "Full simulation", "P");
					leg2->AddEntry(corrFacGenericFastSimulation[index(i, mode, sp)], "Fast simulation", "P");
					leg2->AddEntry(systematicVariationCorrFactor[index(i, mode, sp)], shortVariationDescription.Data(),"F");
					leg2->Draw();
				}
				
				gPad->RedrawAxis("");                              
				gPad->RedrawAxis("G");                             
			}
      c1->SetName(Form("spectraMod%s_dNd%s_%02d_%02d", systematicHistogramIdentifier.Data(), modeString[mode].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));                               
      c2->SetName(Form("corrFacMod%s_dNd%s_%02d_%02d", systematicHistogramIdentifier.Data(), modeString[mode].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));
      qaPlotsFile->cd();                        
      if (systematicHistogramIdentifier == "BbB") // Only save spectra for LowPtEnhancement/depletion
        c1->Write();
      
      c2->Write();
      
      if (i==0) {
        // Only save lowest jetPt-bin directly as PDF
        if (systematicHistogramIdentifier == "BbB")
          c1->SaveAs(Form("%s/spectraMod%s_dNd%s_%02d_%02d.pdf", fileSysErrorsPath.Data(), systematicHistogramIdentifier.Data(), modeString[mode].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));
        c2->SaveAs(Form("%s/corrFacMod%s_dNd%s_%02d_%02d.pdf", fileSysErrorsPath.Data(), systematicHistogramIdentifier.Data(), modeString[mode].Data(), jetPtLimits[2*i], jetPtLimits[2*i+1]));
      }
		}
	}
	qaPlotsFile->Close();
}
