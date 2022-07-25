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

#include <iostream>
#include <fstream>

Bool_t  savePlots = kTRUE;
Bool_t  writeSysErrors = kTRUE;


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

void createSystematicErrorsFromFiles(TString filePathDown, TString filePathUp, TString modeNameDown, TString modeNameUp, TH1F* corrFacOriginalMC[5][5][5], TH1F* corrFacGenericFastSimulation[5][5][5], TH1F* fh1FFGenPrim[5][5][5], TString systematicHistogramIdentifier, Bool_t simpleCalculationSysError, TString fileSysErrorsPath);

// ----------------------------------------------------------------------------

void calculateSystematicErrorsFromFastSimulation(TString fileDir = "files", TString saveDir = "files") {
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleY(1.0);
  gStyle->SetTitleH(0.06);//0.054);
  
  const Int_t nModes = 5;
  
  TString modeString[nModes] = {"TrackPt", "Z", "Xi", "R", "jT"};
  Bool_t useModes[nModes] = {kTRUE, kTRUE, kFALSE, kFALSE, kFALSE};
  
	const Bool_t useLogX[nModes] = {kTRUE, kFALSE, kFALSE, kFALSE, kTRUE};
  TString xAxeTitles[nModes] = {"#it{p}_{T} (GeV/#it{c})", "#it{z}", "#it{#xi}", "R", "j_{T} (GeV/#it{c})"};

  const Int_t nJetPtBins = 5;
  Double_t jetPtLim[nJetPtBins+1] = {5,10,15,20,30,80}; // nBins+1 entries
  
  TString strSp[] = {"","_pi","_K","_p","_e","_mu"}; 
  const Int_t nSpecies   = 5;

  TString strTitSp[] = {"h^{+} + h^{-}","#pi^{+} + #pi^{-}","K^{+} + K^{-}","p + #bar{p}","e^{+} + e^{-}"};//,"#mu^{+} + #mu^{-}"}; 

  TString strSp_10f6a[] = {"","_pi","_K","_p","_e"};//,"_mu"}; 
  
  TH1F* fh1FFGenPrim[nModes][nSpecies][nJetPtBins];
  
  const Int_t nVar = 2;
  const Bool_t useVariations[nVar] = {kTRUE, kTRUE};
  
  TString legendEntry[nVar] = {"Efficiency +/- 5%", "Resolution +/- 20%"};
  TString nameVar[nVar] = {"Eff", "Res"};
  
  TH1F* corrFacOriginalMC[nModes][nSpecies][nJetPtBins] = {0x0};
  
  TH1F* corrFacGenericFastSimulation[nModes][nSpecies][nJetPtBins];

  //TString strInFile10f6a = "files/outCorrections_10f6a_tpcCut.root";
  TString strOriginalMCResults = fileDir + "/corrections_LHC13b2_efix_p1.root";
  
  TString strResDir = fileDir;

  TString strInFileGen(Form("%s/outCorrections_FastJetSimulation_Eff100_Res100.root",strResDir.Data()));
 
  // Load original MC results

  TFile f1(strOriginalMCResults,"READ");
  if (f1.IsZombie()) {
    cout << "Could not open full simulation file " << strOriginalMCResults << ", no comparison shown." << endl;
  }
  else {
    for (Int_t sp=0; sp<nSpecies; sp++) {
      for (Int_t i=0; i<nJetPtBins; i++) {
        for (Int_t mode=0;mode<nModes;++mode) {
          if (!useModes[mode])
            continue;
          
          TString strTitle(Form("hBbBCorr%s%s_%02d_%02d",modeString[mode].Data(),strSp_10f6a[sp].Data(),(int)jetPtLim[i],(int)jetPtLim[i+1]));        
          corrFacOriginalMC[mode][sp][i] = (TH1F*) gDirectory->Get(strTitle);
          corrFacOriginalMC[mode][sp][i]->SetDirectory(0);
        }
      }
    }   
    f1.Close();  
  }
  
  //TODO: catch files not present

  TFile f2(strInFileGen,"READ");
  // Load fast MC results particle level
  for (Int_t sp=0; sp<nSpecies; sp++) {
    gDirectory->cd(Form("Gen%s",strSp[sp].Data()));

    for(Int_t i=0; i<nJetPtBins; i++){
    
      // for genLevel doesn't matter which histos we take - eff == 1 
      for (Int_t mode=0;mode<nModes;++mode) {
        if (!useModes[mode])
          continue;
        
        TString strTitle(Form("fh1FF%sGen%s_%02d_%02d",modeString[mode].Data(),strSp_10f6a[sp].Data(),(int)jetPtLim[i],(int)jetPtLim[i+1]));
        
        fh1FFGenPrim[mode][sp][i] = (TH1F*) gDirectory->Get(strTitle);
        
        fh1FFGenPrim[mode][sp][i]->SetDirectory(0);
      }
    }
    gDirectory->cd("..");
  }
  
  // Load fast MC results detector level. Calculate generic correction factors. Format histogram

  for (Int_t sp=0; sp<nSpecies; sp++) {
    
    gDirectory->cd(Form("RecCuts%s",strSp[sp].Data()));
    
    for(Int_t i=0; i<nJetPtBins; i++){
      for (Int_t mode=0;mode<nModes;++mode) {
        if (!useModes[mode])
          continue;
        
        TString strTitleRec(Form("fh1FF%sRecCuts%s_%02d_%02d",modeString[mode].Data(),strSp[sp].Data(),(int)jetPtLim[i],(int)jetPtLim[i+1]));
        
        TH1F* fInput = (TH1F*) gDirectory->Get(strTitleRec);
        
        // -- corr factors
        
        TString strTit(Form("corrFac%s_GenericFastSimulation_%s_%d",modeString[mode].Data(), strSp[sp].Data(), i));
        
        corrFacGenericFastSimulation[mode][sp][i] = (TH1F*) fh1FFGenPrim[mode][sp][i]->Clone(strTit);
        corrFacGenericFastSimulation[mode][sp][i]->Divide(fh1FFGenPrim[mode][sp][i],fInput,1,1,"B");
        
        /*********/
        
        corrFacGenericFastSimulation[mode][sp][i]->SetDirectory(0); 
        
        delete fInput;
        fInput = 0x0;
        
        // Format histogram
        if (i!=0)
          continue;
        TString strPlotTitle(Form("%s, #it{p}_{T}^{jet, ch} = %d-%d GeV/#it{c}",strTitSp[sp].Data(),(int)jetPtLim[i],(int)jetPtLim[i+1]));
        
        corrFacGenericFastSimulation[mode][sp][i]->SetTitle(strPlotTitle.Data());
        corrFacGenericFastSimulation[mode][sp][i]->SetXTitle(xAxeTitles[mode].Data());
        corrFacGenericFastSimulation[mode][sp][i]->SetYTitle("Correction Factor");
        setHistoStyleColor(corrFacGenericFastSimulation[mode][sp][i],4);
        corrFacGenericFastSimulation[mode][sp][i]->GetXaxis()->SetRangeUser(0.0, corrFacOriginalMC[mode][sp][i]->GetBinLowEdge(corrFacOriginalMC[mode][sp][i]->FindLastBinAbove() + 1));
        
        Double_t minY = TMath::Min(corrFacOriginalMC[mode][sp][i]->GetBinContent(corrFacOriginalMC[mode][sp][i]->GetMinimumBin()), corrFacGenericFastSimulation[mode][sp][i]->GetBinContent(corrFacGenericFastSimulation[mode][sp][i]->GetMinimumBin()));
        Double_t maxY = TMath::Max(corrFacOriginalMC[mode][sp][i]->GetBinContent(corrFacOriginalMC[mode][sp][i]->GetMaximumBin()), corrFacGenericFastSimulation[mode][sp][i]->GetBinContent(corrFacGenericFastSimulation[mode][sp][i]->GetMaximumBin()));
       
        corrFacGenericFastSimulation[mode][sp][i]->GetYaxis()->SetRangeUser(0.7 * minY, TMath::Min(3.0, 1.3 * maxY));
        
        corrFacGenericFastSimulation[mode][sp][i]->SetLineColor(4);
      }
      
    }
    gDirectory->cd("..");
  }               
  f2.Close();
  
  // Efficiency
  createSystematicErrorsFromFiles(Form("%s/outCorrections_FastJetSimulation_Eff095_Res100.root",strResDir.Data()), Form("%s/outCorrections_FastJetSimulation_Eff105_Res100.root",strResDir.Data()), "Eff095", "Eff105", corrFacOriginalMC, corrFacGenericFastSimulation, fh1FFGenPrim, "Eff", kTRUE, saveDir);
  
  // Resolution
  createSystematicErrorsFromFiles(Form("%s/outCorrections_FastJetSimulation_Eff100_Res080.root",strResDir.Data()), Form("%s/outCorrections_FastJetSimulation_Eff100_Res120.root",strResDir.Data()), "Res080", "Res120", corrFacOriginalMC, corrFacGenericFastSimulation, fh1FFGenPrim, "Res", kTRUE, saveDir);
  
  // Jet Shape
  createSystematicErrorsFromFiles(Form("%s/outCorrections_PythiaFastJet_LowPtEnhancement.root",strResDir.Data()), Form("%s/outCorrections_FastJetSimulation_LowPtDepletion.root",strResDir.Data()), "LowPtEnhancement", "LowPtDepletion", corrFacOriginalMC, corrFacGenericFastSimulation, fh1FFGenPrim, "BbB", kFALSE, saveDir);
}

void createSystematicErrorsFromFiles(TString filePathDown, TString filePathUp, TString modeNameDown, TString modeNameUp, TH1F* corrFacOriginalMC[5][5][5], TH1F* corrFacGenericFastSimulation[5][5][5], TH1F* fh1FFGenPrim[5][5][5], TString systematicHistogramIdentifier, Bool_t simpleCalculationSysError, TString fileSysErrorsPath)
{
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleY(1.0);
  gStyle->SetTitleH(0.06);//0.054);
  
  const Int_t nModes = 5;
  
  TString modeString[nModes] = {"TrackPt", "Z", "Xi", "R", "jT"};
  Bool_t useModes[nModes] = {kTRUE, kTRUE, kFALSE, kFALSE, kFALSE};
  
	const Bool_t useLogX[nModes] = {kTRUE, kFALSE, kFALSE, kFALSE, kTRUE};
  TString xAxeTitles[nModes] = {"#it{p}_{T} (GeV/#it{c})", "#it{z}", "#it{#xi}", "R", "j_{T} (GeV/#it{c})"};

  const Int_t nJetPtBins = 5;
  Double_t jetPtLim[nJetPtBins+1] = {5,10,15,20,30,80}; // nBins+1 entries
  
  TString strSp[] = {"","_pi","_K","_p","_e","_mu"}; 
  const Int_t nSpecies   = 5;

  TString strTitSp[] = {"h^{+} + h^{-}","#pi^{+} + #pi^{-}","K^{+} + K^{-}","p + #bar{p}","e^{+} + e^{-}"};//,"#mu^{+} + #mu^{-}"}; 

  TString strSp_10f6a[] = {"","_pi","_K","_p","_e"};//,"_mu"}; 
  
  const Int_t nSysVariations = 2;
	
	TString sysVariationFiles[nSysVariations] = {filePathDown, filePathUp};
	TString namesSysVariations[nSysVariations] = {modeNameDown, modeNameUp};
  
  TH1F* fh1SysVariationGenPrim[nSysVariations][nModes][nSpecies][nJetPtBins];
	TH1F* fh1SysVariationRecPrim[nSysVariations][nModes][nSpecies][nJetPtBins];
	TH1F* corrFactorsVariations[nSysVariations][nModes][nSpecies][nJetPtBins];
	TH1F* systematicVariationCorrFactor[nModes][nSpecies][nJetPtBins];
	TH1F* hSystematicError[nModes][nSpecies][nJetPtBins];
	
	//TODO: catch files not present
  // Load fast MC results particle level
  
	for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;++sysVariationMode) {
		TFile f(sysVariationFiles[sysVariationMode],"READ");
    // Load particle level spectra
		for (Int_t sp=0; sp<nSpecies; sp++) {
			gDirectory->cd(Form("Gen%s",strSp[sp].Data()));

      for(Int_t i=0; i<nJetPtBins; i++){
      
        // for genLevel doesn't matter which histos we take - eff == 1 
        for (Int_t mode=0;mode<nModes;++mode) {
          if (!useModes[mode])
            continue;
          
          TString strTitle(Form("fh1FF%sGen%s_%02d_%02d",modeString[mode].Data(),strSp_10f6a[sp].Data(),(int)jetPtLim[i],(int)jetPtLim[i+1]));
          
          fh1SysVariationGenPrim[sysVariationMode][mode][sp][i] = (TH1F*) gDirectory->Get(strTitle);
          
          fh1SysVariationGenPrim[sysVariationMode][mode][sp][i]->SetDirectory(0);
        }
      }
      gDirectory->cd("..");
    }
    // Load detector level spectra
    for (Int_t sp=0; sp<nSpecies; sp++) {
      gDirectory->cd(Form("RecCuts%s",strSp[sp].Data()));

      for(Int_t i=0; i<nJetPtBins; i++){
        for (Int_t mode=0;mode<nModes;++mode) {
          if (!useModes[mode])
              continue;
          
          TString strTitle(Form("fh1FF%sRecCuts%s_%02d_%02d",modeString[mode].Data(),strSp_10f6a[sp].Data(),(int)jetPtLim[i],(int)jetPtLim[i+1]));
          
          fh1SysVariationRecPrim[sysVariationMode][mode][sp][i] = (TH1F*) gDirectory->Get(strTitle);
          
          fh1SysVariationRecPrim[sysVariationMode][mode][sp][i]->SetDirectory(0);
        }
      }
      gDirectory->cd("..");
    }
    f.Close();
    
    TFile* checkGraphsFile = new TFile(Form("%s/checkGraphs.root", fileSysErrorsPath.Data()), "RECREATE");
    fh1SysVariationGenPrim[0][0][0][0]->Write();
    fh1SysVariationRecPrim[1][0][0][0]->Write();
    checkGraphsFile->Close();
		
    
		for (Int_t sp=0; sp<nSpecies; sp++) {
			for(Int_t i=0; i<nJetPtBins; i++){
				for (Int_t mode=0;mode<nModes;++mode) {
					if (!useModes[mode])
						continue;
					
					TString strTit(Form("corrFac%s_%s_%s_%d", modeString[mode].Data(), namesSysVariations[sysVariationMode].Data(), strSp[sp].Data(), i));
					
					corrFactorsVariations[sysVariationMode][mode][sp][i] = (TH1F*) fh1SysVariationGenPrim[sysVariationMode][mode][sp][i]->Clone(strTit);
					corrFactorsVariations[sysVariationMode][mode][sp][i]->Divide(fh1SysVariationGenPrim[sysVariationMode][mode][sp][i],fh1SysVariationRecPrim[sysVariationMode][mode][sp][i],1,1,"B");
				}
			}
		}
	}
  TFile* checkGraphsFile = new TFile(Form("%s/checkGraphs.root", fileSysErrorsPath.Data()), "UPDATE");
  corrFactorsVariations[0][0][0][0]->Write();
  corrFactorsVariations[1][0][0][0]->Write();
  checkGraphsFile->Close();

	// Calculate systematic errors
	for (Int_t sp=0; sp<nSpecies; sp++) {
		for(Int_t i=0; i<nJetPtBins; i++){
			for (Int_t mode=0;mode<nModes;++mode) {
				if (!useModes[mode])
					continue;
				
				TString strNameCorrSysErrBbB(Form("corrFac%sSysEff_%02d_%02d%s", modeString[mode].Data(), (int)jetPtLim[i], (int)jetPtLim[i+1], strSp[sp].Data()));
				TString strNamehSystematicError(Form("hSysErr%s%s_%02d_%02d%s", systematicHistogramIdentifier.Data(), modeString[mode].Data(), (int)jetPtLim[i], (int)jetPtLim[i+1], strSp[sp].Data()));
				
				systematicVariationCorrFactor[mode][sp][i] = (TH1F*) corrFacGenericFastSimulation[mode][sp][i]->Clone(strNameCorrSysErrBbB);
				hSystematicError[mode][sp][i] =  (TH1F*) corrFacGenericFastSimulation[mode][sp][i]->Clone(strNamehSystematicError);
				hSystematicError[mode][sp][i]->Reset();
				
				for (Int_t bin=1;bin<=corrFacGenericFastSimulation[mode][sp][i]->GetNbinsX();++bin) {
          Double_t sysErr = 0.0; 
          Double_t sysErrScaled = 0.0;
          systematicVariationCorrFactor[mode][sp][i]->SetBinError(bin, 0);
          Double_t cont = systematicVariationCorrFactor[mode][sp][i]->GetBinContent(bin);
          if (simpleCalculationSysError) {
            sysErr = 0.5 * TMath::Abs(corrFactorsVariations[0][mode][sp][i]->GetBinContent(bin) - 
              corrFactorsVariations[1][mode][sp][i]->GetBinContent(bin));
            
            if (cont)
              sysErrScaled = sysErr/cont;
          } else {
            if (fh1FFGenPrim[mode][sp][i]->GetBinContent(bin) == 0)
              continue;
            
            Double_t minEl = cont;
            Double_t maxEl = minEl;
            
            for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;sysVariationMode++) {
              minEl = TMath::Min(minEl, corrFactorsVariations[sysVariationMode][mode][sp][i]->GetBinContent(bin));
              maxEl = TMath::Max(maxEl, corrFactorsVariations[sysVariationMode][mode][sp][i]->GetBinContent(bin));
            }
            
            if(minEl) 
              sysErr = 0.5*fabs((maxEl/minEl) - 1);
            
            sysErrScaled = sysErr;
          }
          
          systematicVariationCorrFactor[mode][sp][i]->SetBinError(bin, sysErr);
          
          if (corrFacOriginalMC[mode][sp][i] && corrFacOriginalMC[mode][sp][i]->GetBinContent(bin) == 0) 
            continue; 
          
          hSystematicError[mode][sp][i]->SetBinContent(bin, sysErrScaled);
          hSystematicError[mode][sp][i]->SetBinError(bin, 0);
        }
      }
    }
  }
  
  TFile* fFileSysErrors = new TFile(Form("%s/outSysErr_%s.root", fileSysErrorsPath.Data(), systematicHistogramIdentifier.Data()), "RECREATE");
  
  for (Int_t sp=0; sp<nSpecies; sp++) {
    for (Int_t i=0; i<nJetPtBins; i++) {
      for (Int_t mode=0;mode<nModes;mode++) {  
        if (!useModes[mode] || !hSystematicError[mode][sp][i])
          continue;  
        
        hSystematicError[mode][sp][i]->Write();
      }
    }
  }
	
	fFileSysErrors->Close();
// 	
// 	gStyle->SetOptStat(0);
//   
//   gStyle->SetTitleStyle(0);
//   gStyle->SetTitleX(0.5);
//   gStyle->SetTitleAlign(23);
//   gStyle->SetTitleY(1.0);
//   gStyle->SetTitleH(0.06);
// 	
// 	const Int_t sysVariationModesColor[nSysVariations] = {51, 8};
// 	const TString sysVariationModesNames[nSysVariations] = {"Low-pt enhanced", "Low-pt depleted"};
// 	
// 	for (Int_t mode=0;mode<nModes;mode++) {
// 		Int_t selectBin = 0;
// 
// 		TCanvas* c1 = createCanvas(Form("c%d", mode), kTRUE);
// 		TLegend* leg1 = createLegend(0);
// 		
// 		TCanvas* c2 = createCanvas(Form("c%d", mode + nModes));
// 		TLegend* leg2 = createLegend(2);
// 		
// 		const Int_t nSpeciesPlot = 4;
// 		
// 		for(Int_t sp=0; sp<nSpeciesPlot; sp++){
// 			for(Int_t i=0; i<nJetPtBins; i++){
// 
// 				if(i != selectBin) 
// 					continue;
// 			
// 				c1->cd(sp+1)->SetLogx(useLogX[mode]);
// 
// 				TString strPlotTitle(Form("%s, #it{p}_{T}^{jet, ch} = %d-%d GeV/#it{c}", strTitSp[sp].Data(), (int)jetPtLim[i], (int)jetPtLim[i+1]));
// 				
// 				fh1FFGenPrim[mode][sp][i]->SetTitle(strPlotTitle);
// 				fh1FFGenPrim[mode][sp][i]->SetXTitle(xAxeTitles[mode].Data());
// 				fh1FFGenPrim[mode][sp][i]->SetYTitle(Form("1/#it{N}_{Jets} d#it{N}/d#it{%s}", xAxeTitles[mode].Data()));
// 				setHistoStyleColor(fh1FFGenPrim[mode][sp][i], kTRUE, 2);
// 				Int_t lastBinWithContent = fh1FFGenPrim[mode][sp][i]->FindLastBinAbove(0.0);
// 				fh1FFGenPrim[mode][sp][i]->GetXaxis()->SetRange(1,lastBinWithContent);
// 
// 				Double_t maxValue = fh1FFGenPrim[mode][sp][i]->GetBinContent(fh1FFGenPrim[mode][sp][i]->GetMaximumBin());
// 
// 				for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;++sysVariationMode) {
// 					maxValue = TMath::Max(maxValue, fh1SysVariationGenPrim[sysVariationMode][mode][sp][i]->GetBinContent(fh1SysVariationGenPrim[sysVariationMode][mode][sp][i]->GetMaximumBin()));
// 				}
// 				
// 				fh1FFGenPrim[mode][sp][i]->GetYaxis()->SetRangeUser(0,maxValue *1.5);
// 				fh1FFGenPrim[mode][sp][i]->DrawCopy();  
// 				for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;++sysVariationMode) {
// 					setHistoStyleColor(fh1SysVariationGenPrim[sysVariationMode][mode][sp][i],kTRUE, sysVariationModesColor[sysVariationMode]);
// 					fh1SysVariationGenPrim[sysVariationMode][mode][sp][i]->DrawCopy("same");
// 				}
// 				fh1FFGenPrim[mode][sp][i]->DrawCopy("same"); //redraw on top   
// 						
// 				if(sp==0){
// 					leg1->AddEntry(fh1FFGenPrim[mode][sp][i],"Reference, gen level","P");
// 					for (Int_t sysVariationMode=0;sysVariationMode<nSysVariations;++sysVariationMode) {
// 						leg1->AddEntry(fh1SysVariationGenPrim[sysVariationMode][mode][sp][i],sysVariationModesNames[sysVariationMode].Data(),"P");
// 					}
// 					
// 					leg1->Draw();
// 				}
// 				
// 				gPad->RedrawAxis("");
// 				gPad->RedrawAxis("G");
// 				
// 				c2->cd(sp+1)->SetLogx(useLogX[mode]);
// 
// 				corrFacGenericFastSimulation[mode][sp][i]->SetTitle(strPlotTitle);
// 				corrFacGenericFastSimulation[mode][sp][i]->SetXTitle(xAxeTitles[mode].Data());
// 				corrFacGenericFastSimulation[mode][sp][i]->SetYTitle("Correction Factor");
// 				setHistoStyleColor(corrFacGenericFastSimulation[mode][sp][i],kFALSE,4);
// 				lastBinWithContent = corrFacGenericFastSimulation[mode][sp][i]->FindLastBinAbove(0.0);
// 				corrFacGenericFastSimulation[mode][sp][i]->GetXaxis()->SetRange(1,lastBinWithContent);
// // 				corrFacGenericFastSimulation[mode][sp][i]->GetXaxis()->SetRangeUser(0,jetPtLim[i+1]);
// 				corrFacGenericFastSimulation[mode][sp][i]->SetLineColor(4); 
// 				maxValue = corrFacGenericFastSimulation[mode][sp][i]->GetBinContent(corrFacGenericFastSimulation[mode][sp][i]->GetMaximumBin());
// 				
// 				setHistoStyleColor(systematicVariationCorrFactor[mode][sp][i],kFALSE,4);
// 				systematicVariationCorrFactor[mode][sp][i]->SetFillColor(7);
// 				maxValue = TMath::Max(maxValue, systematicVariationCorrFactor[mode][sp][i]->GetBinContent(systematicVariationCorrFactor[mode][sp][i]->GetMaximumBin()));
// 				
// 				setHistoStyleColor(corrFacOriginalMC[mode][sp][i],kFALSE,2);
// 				corrFacOriginalMC[mode][sp][i]->SetMarkerStyle(24);
// 				maxValue = TMath::Max(maxValue, corrFacOriginalMC[mode][sp][i]->GetBinContent(corrFacOriginalMC[mode][sp][i]->GetMaximumBin()));
// 				
// 				//Do not use dynamic calculation here because correction can sometimes become very big
// // 				corrFacGenericFastSimulation[mode][sp][i]->GetYaxis()->SetRangeUser(0.0,maxValue * 1.2);
// 				corrFacGenericFastSimulation[mode][sp][i]->GetYaxis()->SetRangeUser(0.0,3.0);
// 				corrFacGenericFastSimulation[mode][sp][i]->DrawCopy();  
// 				systematicVariationCorrFactor[mode][sp][i]->DrawCopy("same,E2");
// 				corrFacOriginalMC[mode][sp][i]->DrawCopy("same");
// 				
// 				if(sp==0){
// 					leg2->AddEntry(corrFacOriginalMC[mode][sp][i],"Full simulation","P");
// 					leg2->AddEntry(corrFacGenericFastSimulation[mode][sp][i],"Fast simulation","P");
// 					leg2->AddEntry(systematicVariationCorrFactor[mode][sp][i],"Low-pt enhanced/depleted","F");
// 					leg2->Draw();
// 				}
// 				
// 				gPad->RedrawAxis("");
// 				gPad->RedrawAxis("G");
// 				
// 				if (savePlots) {
// 					c1->SaveAs(Form("%sspectraModFF_dNd%s_%02d_%02d.pdf", saveDir.Data(), modeString[mode].Data(), (int) jetPtLim[i],(int) jetPtLim[i+1]));
// 					c2->SaveAs(Form("%scorrFacModFF_dNd%s_%02d_%02d.pdf", saveDir.Data(), modeString[mode].Data(), (int) jetPtLim[i],(int) jetPtLim[i+1]));
// 				}
// 					
// 			}
// 		}
// 	}
}
