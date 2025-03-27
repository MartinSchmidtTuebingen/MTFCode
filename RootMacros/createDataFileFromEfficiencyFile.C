 #include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"

#include "AliCFContainer.h"
#include "AliPID.h"

#include <iostream>

#include "THnSparseDefinitions.h"

enum type { kTrackPt = 0, kZ = 1, kXi = 2, kR = 3, kjT = 4, kNtypes};

Int_t createDataFileFromEfficiencyFile(TString path, TString outPutPath, Double_t lowerJetPt = -1, Double_t upperJetPt = -1, Int_t lowerCentrality = -1, Int_t upperCentrality = -1, Int_t iObs = kTrackPt, Bool_t applyMuonCorrection = kTRUE)
{
  TString yieldNames[AliPID::kSPECIES] = { "hYieldElectrons", "hYieldMuons", "hYieldPions", "hYieldKaons", "hYieldProtons" };
  TString genYieldNames[AliPID::kSPECIES] = { "hgenYieldElectrons", "hgenYieldMuons", "hgenYieldPions", "hgenYieldKaons", "hgenYieldProtons" };
  TFile* fileData = TFile::Open(path.Data());
  if (!fileData) {
    printf("Failed to open file \"%s\"\n", path.Data());
    return -1;
  }

  cout << "Opened file successfully!" << endl;

  AliCFContainer* data = 0x0;
  TDirectoryFile* effDir = (TDirectoryFile*)(fileData->Get("PWGJE_taskMTFPID_efficiency"));
  if (!effDir) {
    data = (AliCFContainer*)(fileData->Get("containerEff"));

    if (!data) {
      printf("Failed to load efficiency container directly!\n");
      return -1;
    }
  } else {
    data = (AliCFContainer*)(effDir->Get("containerEff"));
    if (!data) {
      printf("Failed to load efficiency container after finding sub directory!\n");
      return -1;
    }
  }
  cout << "Data Container loaded" << endl;

  TDirectoryFile* dataDir = 0x0;
  TObjArray* array = 0x0;
  dataDir = (TDirectoryFile*)(fileData->Get("PWGJE_taskMTFPID"));
  if (!dataDir) {
    TString pathData = path.ReplaceAll("_efficiency","");

    TFile* fileDataOld = TFile::Open(pathData.Data());

    if (!fileDataOld)
      printf("Failed to load data directory!\n");
    else
      array = (TObjArray*)(fileDataOld->Get("PWGJE_taskMTFPID"));
  } else {
    array = (TObjArray*)(dataDir->Get("PWGJE_taskMTFPID"));
  }

  TH1* hNumEventsTriggerSel = 0x0;
  TH1* hNumEventsTriggerSelVtxCut = 0x0;
  TH1* hNumEventsTriggerSelVtxCutZ = 0x0;
  TH1* hNumEventsTriggerSelVtxCutZPileUpRejected = 0x0;

  TH2D* hNjetsGen = 0x0;
  TH2D* hNjetsRec = 0x0;

  if (array) {
    printf("Loading event arrays!\n");
    hNumEventsTriggerSel = dynamic_cast<TH1*>(array->FindObject("fhEventsTriggerSel"));
    hNumEventsTriggerSelVtxCut = dynamic_cast<TH1*>(array->FindObject("fhEventsTriggerSelVtxCut"));
    hNumEventsTriggerSelVtxCutZ = dynamic_cast<TH1*>(array->FindObject("fhEventsProcessedNoPileUpRejection"));
    hNumEventsTriggerSelVtxCutZPileUpRejected = dynamic_cast<TH1*>(array->FindObject("fhEventsProcessed"));
      hNjetsGen = (TH2D*)array->FindObject("fh2FFJetPtGen");
    hNjetsRec = (TH2D*)array->FindObject("fh2FFJetPtRec");
  } else {
    cout << "Could not find event array" << endl;
  }


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

  Int_t iPt     = data->GetVar(Form("%s_{T} (GeV/c)", momentumString.Data()));
  Int_t iMCid   = data->GetVar("MC ID");
//   iEta    = data->GetVar("#eta");
//   iCharge = data->GetVar("Charge (e_{0})");
  Int_t iMult   = data->GetVar("Centrality Percentile");
//   // Will be set later, if jet pT is restricted
  Int_t iJetPt  = 0;
//
  Int_t iObsAxis = iPt; // To be set to other values later;
//
  if (lowerJetPt >= 0 && upperJetPt >= 0) {
    iJetPt = data->GetVar(Form("%s_{T}^{jet} (GeV/c)", momentumString.Data()));

    if (iObs == kZ)
      iObsAxis = data->GetVar(Form("z = %s_{T}^{track} / %s_{T}^{jet}", momentumString.Data(), momentumString.Data()));
    else if (iObs == kXi)
      iObsAxis = data->GetVar(Form("#xi = ln(%s_{T}^{jet} / %s_{T}^{track})", momentumString.Data(), momentumString.Data()));
    else if (iObs == kR)
      iObsAxis = data->GetVar("R");
    else if (iObs == kjT)
      iObsAxis = data->GetVar("j_{T} (GeV/c)");
  }

   // If desired, restrict centrality axis
  Int_t lowerCentralityBinLimit = -1;
  Int_t upperCentralityBinLimit = -2; // Integral(lowerCentBinLimit, uppCentBinLimit) will not be restricted if these values are kept. In particular, under- and overflow bin will be used!
  Bool_t restrictCentralityAxis = kFALSE;

  if (lowerCentrality >= -1 && upperCentrality >= -1) {
    // Add subtract a very small number to avoid problems with values right on the border between two bins
    lowerCentralityBinLimit = data->GetAxis(iMult, 0)->FindFixBin(lowerCentrality + 0.001);
    upperCentralityBinLimit = data->GetAxis(iMult, 0)->FindFixBin(upperCentrality - 0.001);

    // Check if the values look reasonable
    if (lowerCentralityBinLimit <= upperCentralityBinLimit && lowerCentralityBinLimit >= 0
        && upperCentralityBinLimit <= data->GetAxis(iMult, 0)->GetNbins() + 1) {
      restrictCentralityAxis = kTRUE;
    }
    else {
      std::cout << std::endl;
      std::cout << "Requested centrality range out of limits or upper and lower limit are switched!" << std::endl;
      return -1;
    }
  }

  cout << "Centrality: " << lowerCentrality << " - " << upperCentrality << endl;

  data->SetRangeUser(iMult, lowerCentralityBinLimit, upperCentralityBinLimit, kTRUE);

  // If desired, restrict jetPt axis
  Int_t lowerJetPtBinLimit = -1;
  Int_t upperJetPtBinLimit = -2;
  Bool_t restrictJetPtAxis = kFALSE;
  Double_t actualLowerJetPt = -1.;
  Double_t actualUpperJetPt = -1.;

  if (lowerJetPt >= 0 && upperJetPt >= 0) {
    // Add subtract a very small number to avoid problems with values right on the border between to bins
    lowerJetPtBinLimit = data->GetAxis(iJetPt, 0)->FindFixBin(lowerJetPt + 0.001);
    upperJetPtBinLimit = data->GetAxis(iJetPt, 0)->FindFixBin(upperJetPt - 0.001);

    // Check if the values look reasonable
    if (lowerJetPtBinLimit <= upperJetPtBinLimit && lowerJetPtBinLimit >= 0 &&
        upperJetPtBinLimit <= data->GetAxis(iJetPt, 0)->GetNbins() + 1) {
      actualLowerJetPt = data->GetAxis(iJetPt, 0)->GetBinLowEdge(lowerJetPtBinLimit);
      actualUpperJetPt = data->GetAxis(iJetPt, 0)->GetBinUpEdge(upperJetPtBinLimit);

      restrictJetPtAxis = kTRUE;
      std::cout << "Lower jetPt: " << actualLowerJetPt << "; Upper jetPt: " << upperJetPt << std::endl;
    }
    else {
      std::cout << std::endl;
      std::cout << "Requested jet pT range out of limits or upper and lower limit are switched!" << std::endl;
      return -1;
    }
  }

  if (restrictJetPtAxis) {
    data->SetRangeUser(iJetPt, lowerJetPtBinLimit, upperJetPtBinLimit, kTRUE);
    if (!hNjetsRec || !hNjetsGen) {
      std::cout << "Failed to load number of jets histos!" << std::endl;
      return -1;
    }
  } else {
    cout << "No Jet axis pT restriction";
  }

  const Double_t nJetsGen = hNjetsGen ? hNjetsGen->Integral(lowerCentralityBinLimit, upperCentralityBinLimit, lowerJetPtBinLimit, upperJetPtBinLimit) : 1.;
  const Double_t nJetsRec = hNjetsRec ? hNjetsRec->Integral(lowerCentralityBinLimit, upperCentralityBinLimit, lowerJetPtBinLimit, upperJetPtBinLimit) : 1.;

  TH1D* yields[5] = {0x0, };
  TH1D* yieldsGen[5] = {0x0, };

  cout << "Creating data particle yields" << endl;
  for (Int_t species=0;species<AliPID::kSPECIES;species++) {
    data->SetRangeUser(iMCid,species+1,species+1,kTRUE);
    yields[species] = (TH1D*)data->Project(kStepRecWithRecCutsMeasuredObs, iObs);
    yields[species]->SetName(yieldNames[species].Data());
    yields[species]->SetTitle(AliPID::ParticleLatexName(species));
    yields[species]->SetLineColor(getLineColorAliPID(species));
    yields[species]->SetMarkerColor(getLineColorAliPID(species));
    for (Int_t bin=0;bin<yields[species]->GetXaxis()->GetNbins();++bin) {
      Double_t adjustmentFactor = 1/(yields[species]->GetXaxis()->GetBinWidth(bin) * nJetsRec);
      yields[species]->SetBinContent(bin, yields[species]->GetBinContent(bin) * adjustmentFactor);
      yields[species]->SetBinError(bin, yields[species]->GetBinError(bin) * adjustmentFactor);
    }
  }

  if (applyMuonCorrection) {
    yields[AliPID::kPion]->Add(yields[AliPID::kMuon]);
  }

  cout << "Creating generated particle yields" << endl;
  for (Int_t species=0;species<AliPID::kSPECIES;species++) {
    data->SetRangeUser(iMCid,species+1,species+1,kTRUE);
    yieldsGen[species] = (TH1D*)data->Project(kStepGenWithGenCuts, iObs);
    yieldsGen[species]->SetName(genYieldNames[species].Data());
    yieldsGen[species]->SetTitle(AliPID::ParticleLatexName(species));
    yieldsGen[species]->SetLineColor(getLineColorAliPID(species));
    yieldsGen[species]->SetMarkerColor(getLineColorAliPID(species));
    for (Int_t bin=0;bin<yieldsGen[species]->GetXaxis()->GetNbins();++bin) {
      Double_t adjustmentFactor = 1/(yieldsGen[species]->GetXaxis()->GetBinWidth(bin) * nJetsGen);
      yieldsGen[species]->SetBinContent(bin, yieldsGen[species]->GetBinContent(bin) * adjustmentFactor);
      yieldsGen[species]->SetBinError(bin, yieldsGen[species]->GetBinError(bin) * adjustmentFactor);
    }
  }

  TFile* fileOutput = new TFile(outPutPath.Data(), "RECREATE");
  fileOutput->cd();
  for (Int_t species=0;species<AliPID::kSPECIES;species++) {
    yields[species]->Write();
    yieldsGen[species]->Write();
  }

  if (hNumEventsTriggerSel) {
    hNumEventsTriggerSel->Write();
    hNumEventsTriggerSelVtxCut->Write();
    hNumEventsTriggerSelVtxCutZ->Write();
    hNumEventsTriggerSelVtxCutZPileUpRejected->Write();
  }

  fileOutput->Close();
  fileData->Close();

  return 0;
}
