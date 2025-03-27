 #include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"

#include "AliCFContainer.h"
#include "AliPID.h"

#include <iostream>

#include "THnSparseDefinitions.h"

enum type { kTrackPt = 0, kZ = 1, kXi = 2, kR = 3, kjT = 4, kNtypes};

Int_t createMCClosureCheckCanvas(TString pathWithGenYields, TString pathCorrectedData, Bool_t setLogX = kTRUE, Bool_t setLogY = kTRUE)
{
  TString yieldNamesCorrected[AliPID::kSPECIES] = { "hYieldElectrons_corrected", "hYieldMuons_corrected", "hYieldPions_corrected", "hYieldKaons_corrected", "hYieldProtons_corrected" };
  TString yieldNamesDivided[AliPID::kSPECIES] = { "hComparisonElectrons", "hComparisonMuons", "hComparisonPions", "hComparisonKaons", "hComparisonProtons" };
  TString genYieldNames[AliPID::kSPECIES] = { "hgenYieldElectrons", "hgenYieldMuons", "hgenYieldPions", "hgenYieldKaons", "hgenYieldProtons" };

  Bool_t useHistos[AliPID::kSPECIES] = {kFALSE, kFALSE, kTRUE, kTRUE, kTRUE};

  TFile* fileGen = TFile::Open(pathWithGenYields.Data(), "UPDATE");
  if (!fileGen) {
    printf("Failed to open file \"%s\"\n", pathWithGenYields.Data());
    return -1;
  }

  TFile* fileDataCorrected = TFile::Open(pathCorrectedData.Data());
  if (!fileDataCorrected) {
    printf("Failed to open file \"%s\"\n", pathCorrectedData.Data());
    return -1;
  }

  TH1D* yieldsCorrected[5] = {0x0, };
  TH1D* yieldsGen[5] = {0x0, };

  for (Int_t species=0;species<AliPID::kSPECIES;species++) {
    if (!useHistos[species])
      continue;

    yieldsCorrected[species] = (TH1D*)fileDataCorrected->FindObjectAny(yieldNamesCorrected[species].Data());
    yieldsGen[species] = (TH1D*)fileGen->FindObjectAny(genYieldNames[species].Data());
    if (!yieldsCorrected[species] || !yieldsGen[species]) {
      printf("Failed to get histo \"%s\" or \"%s\" \"\n", yieldNamesCorrected[species].Data(), genYieldNames[species].Data());
      return -1;
    }
  }

  TCanvas* c = new TCanvas("cClosureCheck", "cClosureCheck");
  c->Divide(1,2);
  c->cd(1)->SetLogx(setLogX);
  c->cd(1)->SetLogy(setLogY);
  c->cd(2)->SetLogx(setLogX);

  Bool_t first = kTRUE;

  for (Int_t species=0;species<AliPID::kSPECIES;species++) {
    if (!useHistos[species])
      continue;

    c->cd(1);
    yieldsCorrected[species]->SetMarkerStyle(kOpenCircle);
    yieldsCorrected[species]->Draw(first ? "" : "same");
    yieldsGen[species]->SetMarkerStyle(kOpenCross);
    yieldsGen[species]->Draw("same");

    TH1* yieldsDivided = (TH1*)yieldsCorrected[species]->Clone(yieldNamesDivided[species]);
    yieldsDivided->Divide(yieldsGen[species]);
    yieldsDivided->SetMarkerStyle(kFullCross);
    c->cd(2);
    yieldsDivided->Draw(first ? "" : "same");
    first = kFALSE;
  }
  fileGen->cd();
  c->Write();

  fileGen->Close();
  fileDataCorrected->Close();

  return 0;
}
