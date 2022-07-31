//Write out the results from fast MC simulation, not calculating correction factors, scaled with nOfJets

#include "TFile.h"
#include "AliCFContainer.h"
#include "AliPID.h"
#include "TH2.h"
#include "TString.h"
#include "TObjArray.h"

#include "THnSparseDefinitions.h"

#include <iostream>
using namespace std;

Int_t iPt     = 0;
Int_t iMCid   = 0;
Int_t iEta    = 0;
Int_t iCharge = 0;
Int_t iMult   = 0;
Int_t iJetPt  = 0;
Int_t iZ = 0;
Int_t iXi = 0;
Int_t iDistance = 0;
Int_t ijT = 0;

Int_t iObsAxis = 0;

const Int_t nOfTrackObservables = 5;
const TString observableNames[nOfTrackObservables] = {"TrackPt", "Z", "Xi", "R", "jT"};

Int_t writeOutCorrectionFiles(TString effFile, TString outFileString, TString jetPtStepsString = "", TString usedObservablesInputString = "") {
  TFile* fileEff = new TFile(effFile.Data());
  if (!fileEff) {
    printf("Failed to open efficiency file \"%s\"\n", effFile.Data());
    return -1;
  }
  
  AliCFContainer* data = (AliCFContainer*)(fileEff->Get("containerEff"));
  if (!data) {
    printf("Failed to load efficiency container!\n");
    return -1;
  }  
  
  fileEff->Close();
  
  //Setting jet limits and getting the number of rec/gen jets from the file associated with the efficiency file
  Int_t nOfJetBins = 0;
  
  Int_t* jetPtLimits = 0x0;
  if (jetPtStepsString != "") {
    TObjArray* jetPtBins = jetPtStepsString.Tokenize(";");
    if (!TMath::Even(jetPtBins->GetEntriesFast())) {
      cout << "Attention: jetPtStepsString has to have even number of entries!" << endl;
    } else {
      nOfJetBins = jetPtBins->GetEntriesFast()/2;
      jetPtLimits = new Int_t[2*nOfJetBins];
      for (Int_t i=0;i<2*nOfJetBins;i++) {
        jetPtLimits[i] = getStringFromTObjStrArray(jetPtBins, i).Atoi();
      }
    }
  }
  
  if (!nOfJetBins) {
    nOfJetBins = 5;
    jetPtLimits = new Int_t[2*nOfJetBins];
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
  
  Int_t* nOfJets = new Int_t[2*nOfJetBins];
  Int_t* jetBinLimits = new Int_t[2*nOfJetBins];
  
  Bool_t useObservable[nOfTrackObservables] = {kTRUE, kTRUE, kTRUE, kTRUE, kTRUE};
  
  if (usedObservablesInputString != "") {
    TObjArray* modeArray = usedObservablesInputString.Tokenize(";");
    Int_t nActivatedModes = modeArray->GetEntriesFast();

    for (Int_t j=0;j<nOfTrackObservables;j++) {
      useObservable[j] = kFALSE;
      for (Int_t i=0;i<nActivatedModes;i++) {
        TString activatedMode = getStringFromTObjStrArray(modeArray, i);
        activatedMode = activatedMode.CompareTo("pt", TString::kIgnoreCase) == 0 ? "TrackPt" : activatedMode;
        
        if (activatedMode.CompareTo(observableNames[j], TString::kIgnoreCase) == 0) {
          useObservable[j] = kTRUE;
        }
      }
    }
  }
  
  const Int_t nOfEffSteps = 2;
  EffSteps usedEffSteps[nOfEffSteps] = {kStepGenWithGenCuts, kStepRecWithRecCutsMeasuredObsPrimaries};
  TString dirNameEffSteps[nOfEffSteps] = {"Gen", "RecCuts"};
  
  // For backward compatibility:
  // Check whether "P_{T}" or "p_{T}" is used
  TString momentumString = "p";
  for (Int_t i = 0; i < data->GetNVar(); i++) {
    TString temp = data->GetVarTitle(i);
    if (temp.Contains("P_{")) {
      momentumString = "P";
      break;
    }
    else if (temp.Contains("p_{")) {
      momentumString = "p";
      break;
    }
  }
  
//   AliPID::ParticleShortName(species)
  
  iPt     = data->GetVar(Form("%s_{T} (GeV/c)", momentumString.Data()));
  iMCid   = data->GetVar("MC ID");
  iEta    = data->GetVar("#eta");
  iCharge = data->GetVar("Charge (e_{0})");
  iMult   = data->GetVar("Centrality Percentile");
  iJetPt = data->GetVar(Form("%s_{T}^{jet} (GeV/c)", momentumString.Data()));
  iZ     = data->GetVar(Form("z = %s_{T}^{track} / %s_{T}^{jet}", momentumString.Data(), momentumString.Data()));
  iXi    = data->GetVar(Form("#xi = ln(%s_{T}^{jet} / %s_{T}^{track})", momentumString.Data(), momentumString.Data()));
  iDistance = data->GetVar("R");
  ijT = data->GetVar("j_{T} (GeV/c)");  
  
  Int_t trackObservableBins[nOfTrackObservables] = {iPt, iZ, iXi, iDistance, ijT};
  
  TString pathNameDataMC = effFile;
  pathNameDataMC.ReplaceAll("_efficiency", "");
  
  TFile* fDataMC = TFile::Open(pathNameDataMC.Data());
  if (!fDataMC)  {
    std::cout << std::endl;
    std::cout << "Failed to open file \"" << pathNameDataMC.Data() << "\" to obtain num of rec/gen jets!" << std::endl;   
    return -1;
  }
  
  TString listName = pathNameDataMC;
  listName.Replace(0, listName.Last('/') + 1, "");
  listName.ReplaceAll(".root", "");
    
  TObjArray* histList = (TObjArray*)(fDataMC->Get(listName.Data()));
  if (!histList) {
    std::cout << std::endl;
    std::cout << "Failed to load list \"" << listName.Data() << "\" to obtain num of rec/gen jets!" << std::endl;
    return -1;
  }    
  
  TH2* hNjetsGen = (TH2D*)histList->FindObject("fh2FFJetPtGen");
  TH2* hNjetsRec = (TH2D*)histList->FindObject("fh2FFJetPtRec");
  
  Int_t lowerCentralityBinLimit = -1;
  Int_t upperCentralityBinLimit = -1;
	
	if (hNjetsGen->GetEntries() > 0 && hNjetsRec->GetEntries() > 0) {
		for (Int_t i=0;i<nOfJetBins;++i) {
			jetBinLimits[2*i] = data->GetAxis(iJetPt, 0)->FindFixBin(jetPtLimits[2*i] + 0.001);
			jetBinLimits[2*i+1] = data->GetAxis(iJetPt, 0)->FindFixBin(jetPtLimits[2*i+1] - 0.001);   
			
			nOfJets[2*i] = hNjetsGen ? hNjetsGen->Integral(lowerCentralityBinLimit, upperCentralityBinLimit, jetBinLimits[2*i], jetBinLimits[2*i+1]) : 1.;
			nOfJets[2*i+1] = hNjetsRec ? hNjetsRec->Integral(lowerCentralityBinLimit, upperCentralityBinLimit, jetBinLimits[2*i], jetBinLimits[2*i+1]) : 1.;
		}
	}
  
  TFile* outFile = new TFile(outFileString.Data(),"RECREATE");
  
  for (Int_t species=-1;species<AliPID::kSPECIES;++species) {
    TString speciesString = species >= 0 ? (TString("_") + TString(AliPID::ParticleShortName(species))) : TString("");
    data->SetRangeUser(iMCid,species+1,species+1,kTRUE);
    for (Int_t effStep=0;effStep<nOfEffSteps;++effStep) {
      TString dirName = dirNameEffSteps[effStep] + speciesString;
      outFile->mkdir(dirName.Data());
      outFile->cd(dirName.Data());
      for (Int_t jetPtStep = 0;jetPtStep<nOfJetBins;++jetPtStep) {
        data->SetRangeUser(iJetPt,jetBinLimits[2*jetPtStep],jetBinLimits[2*jetPtStep+1],kTRUE);
        for (Int_t observable = 0;observable<nOfTrackObservables;++observable) {
          if (!useObservable[observable])
            continue;
          
          TH1* h = data->Project(usedEffSteps[effStep],trackObservableBins[observable]);
          if (!h)
            continue;
          
          h->SetNameTitle(TString::Format("fh1FF%s%s%s_%02d_%02d",observableNames[observable].Data(),dirNameEffSteps[effStep].Data(),speciesString.Data(),(Int_t)jetPtLimits[jetPtStep*2],(Int_t)jetPtLimits[jetPtStep*2+1]),"");
    
          if (nOfJets[2*jetPtStep + TMath::Min(effStep,1)])
            h->Scale(1.0/nOfJets[2*jetPtStep + TMath::Min(effStep,1)]);
          
          for (Int_t binNumber = 0;binNumber<=h->GetNbinsX();binNumber++) 
            h->SetBinContent(binNumber,h->GetBinContent(binNumber)/h->GetBinWidth(binNumber));
            
          h->Write();
        }
      }
    }
  }
  outFile->Close();
  return 0;
}
