#ifndef THNSPARSEDEFINITIONS_H
#define THNSPARSEDEFINITIONS_H

#ifndef __CINT__
#include "TCanvas.h"
#include "THnSparse.h"
#include "TPaveText.h"
#include "TString.h"
#include "TGraphAsymmErrors.h"
#include "AliPID.h"
#include "TH1.h"
#include "TAxis.h"
#endif

#define NEW_AXES

#ifdef NEW_AXES
  enum axesTHnSparseEta {
    kMCpid = 0,
    kSelectSpecies,
    kPtpcInner,
    kMultiplicity,
    kDeltaPrime,
    kEta
  };
#else
  enum axesTHnSparseEta {
    kMCpid = 0,
    kSelectSpecies,
    kPtpcInner,
    kPt,
    kDeDx,
    kMultiplicity,//kDelta,
    kDeltaPrime,
    kEta
  };
#endif

enum axesTHnSparsePID {
  kPidMCpid = 0,
  kPidSelectSpecies,
  kPidPt,
  //OLD kPidDelta,
  kPidDeltaPrime,
  kPidCentrality,
  kPidJetPt,
  kPidZ,
  kPidXi
};

/*OLD with TOF, p_TPC_Inner and p_vertex
enum axesTHnSparsePID {
  kPidMCpid = 0,
  kPidSelectSpecies,
  kPidPtpcInner,
  kPidPt,
  kPidPvertex,
  kPidDelta,
  kPidDeltaPrime,
  kPidDeltaTOF
};//*/

enum axesTHnSparsePIDgen {
  kPidGenMCpid = 0,
  kPidGenSelectSpecies,
  kPidGenPt,
  //OLD kPidGenDelta,
  kPidGenDeltaPrime,
  kPidGenCentrality,
  kPidGenJetPt,
  kPidGenZ,
  kPidGenXi
};

enum axesTHnSparsePIDgenYield {
  kPidGenYieldMCpid = 0,
  kPidGenYieldPt = 1,
  kPidGenYieldCentrality = 2,
  kPidGenYieldJetPt = 3,
  kPidGenYieldZ = 4,
  kPidGenYieldXi = 5,
  kPidGenYieldNumAxes = 6 
};

enum MCpid  {
  kEl = 1,
  kKa = 2,
  kMu = 3,
  kPi = 4,
  kPr = 5,
  kMuPlusPi = 10
};

enum PIDtype { 
  kMCid = 0, 
  kTPCid = 1, 
  kV0idNoTOF = 2, 
  kTPCandTOFid = 3,
  kV0idPlusTOFaccepted = 4,
  kV0idPlusTOFrejected = 5  
};

enum efficiencyAxes {
  kEffMCID = 0, 
  kEffTrackPt = 1, 
  kEffTrackEta = 2, 
  kEffTrackCharge = 3, 
  kEffCentrality = 4, 
  kEffJetPt = 5,
  kEffNumAxes = 6 
};

enum ptResolutionAxes { 
  kPtResJetPt = 0,
  kPtResGenPt = 1,
  kPtResRecPt = 2,
  kPtResCharge = 3,
  kPtResCentrality = 4, 
  kPtResNumAxes = 5
};

enum dEdxCheckAxes {
  kDeDxCheckPID = 0,
  kDeDxCheckP = 1,
  kDeDxCheckJetPt = 2,
  kDeDxCheckEtaAbs = 3,
  kDeDxCheckDeDx = 4,
  kDeDxCheckNumAxes = 5 
};
  
enum EffSteps {
  kStepGenWithGenCuts = 0, 
  kStepRecWithGenCuts = 1, 
  kStepRecWithGenCutsMeasuredObs = 2,
  kStepRecWithRecCutsMeasuredObs = 3, 
  kStepRecWithRecCutsMeasuredObsPrimaries = 4,
  kStepRecWithRecCutsMeasuredObsStrangenessScaled = 5,
  kStepRecWithRecCutsPrimaries = 6,
  kNumSteps = 7
};

enum chargeMode {
  kNegCharge = -1,
  kAllCharged = 0,
  kPosCharge = 1
};

enum TOFpidInfo {
  kNoTOFinfo = -2,
  kNoTOFpid = -1,
  kTOFpion = 0,
  kTOFkaon = 1,
  kTOFproton = 2,
  kNumTOFspecies = 3,
  kNumTOFpidInfoBins = 5
};

const TString partShortName[9] = { "El", "Ka", "Mu", "Pi", "Pr", "V0plusTOFel", "V0el", "V0pi", "V0pr" };

enum typePtBin {
  kPtBinTypeJets = 0,
  kPtBinTypePPMultOld = 1,
  kPtBinTypePPMult = 2,
  kPtBinTypePP13TeV = 3
};


const Bool_t useNewMultiplicityRangeForMB = kTRUE;

// Coarser binning from ~5 GeV/c on to have reasonable statistics in high-mult bins
const Int_t nPtBinsPPmult = 53;
Double_t binsPtPPmult[nPtBinsPPmult + 1] = {0.,  0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45,
                                           0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95,
                                            1.,  1.1, 1.2,  1.3, 1.4,  1.5, 1.6,  1.7, 1.8,  1.9,
                                            2.,  2.1, 2.2,  2.3, 2.4,  2.5, 2.6,  2.7, 2.8,  2.9, 
                                            3.,  3.2, 3.4,  3.6, 3.8,   4., 4.5,    5., 6.,   8., 
                                           10.,  15., 20.,  50. };

// Old binning for mult (used for thesis and results before 2015_04_21). 
// Much coarser binning at high pT and in general coarser binning to get reasonable weighting for regularisation
const Int_t nPtBinsPPmultOld = 40;
const Double_t binsPtPPmultOld[nPtBinsPPmultOld+1] = {0., 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45,
             0.5, 0.55, 0.6, 0.65, 0.7, 0.8, 0.9, 1.0, 1.2, 1.4,
             1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 3.4,
             3.6, 3.8, 4.0, 4.5, 5.0, 6.0, 8.0, 10.0, 15.0, 20.0,
             50.0 };

///*
//coarser binning at high pT and in general coarser binning to get reasonable weighting for regularisation
const Int_t nPtBinsJets = 53;
const Double_t binsPtJets[nPtBinsJets+1] = {0., 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45,
             0.5, 0.55, 0.6, 0.65, 0.7, 0.8, 0.9, 1.0, 1.2, 1.4,
             1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 3.4,
             3.6, 3.8, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 8.0,
             9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 20.0,
             24.0, 30.0, 40.0, 50.0 };
//*
//Coarster binning for pp 13 TeV LF Analysis
const Int_t nPtBinsPP13TeV = 59;
const Double_t binsPtPP13TeV[nPtBinsPP13TeV+1] = {0.01, 0.1, 0.12, 0.14, 0.16, 0.18, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 20.0};
             
/*
//coarser binning at high pT and, in addition, coarser binning around crossings for PbPb 2.76 ATeV FINAL version
const Int_t nPtBins = 50;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
             0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.95, 1.1,
             1.3, 1.4, 1.8, 2.2, 2.6, 3.0, 3.2, 3.4, 3.6, 3.8, 
             4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0, 
             11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 22.0, 26.0, 35.0, 
             50.0 };
*/



/*
//coarser binning at high pT and, in addition, coarser binning around crossings for pPb 5.023 ATeV FINAL version
const Int_t nPtBins = 52;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
             0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95,
             1.0, 1.1, 1.2, 1.3, 1.4, 1.8, 2.2, 2.6, 3.0, 3.2, 
             3.4, 3.6, 3.8, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 
             8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0,
             22.0, 30.0, 50.0 };
*/

/*
// Coarser binning at high pT and, in addition, coarser binning around crossings for pp 7 TeV FINAL version
const Int_t nPtBins = 47;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
             0.5, 0.55, 0.6,  0.65, 0.7, 0.75, 0.8, 1.2, 1.4, 1.6,
             1.8, 2.0, 2.6, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.5,
             5.0, 5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 
             13.0, 14.0, 15.0, 16.0, 18.0, 22.0, 26.0, 50.0 };
*/
/*
// Coarser binning at high pT and, in addition, coarser binning around crossings for pp 7 TeV VERY NEW version
const Int_t nPtBins = 46;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
             0.5, 0.55, 0.6,  0.65, 0.7, 0.75, 0.8, 1.2, 1.6,
             2.0, 2.5, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.5, 5.0,
             5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0,
             14.0, 15.0, 16.0, 18.0, 22.0, 26.0, 35.0, 50.0 };
*/
/*
// Coarser binning at high pT and, in addition, coarser binning around crossings for pp 7 TeV NEW version
const Int_t nPtBins = 47;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
             0.5, 0.55, 0.6,  0.65, 0.7, 0.75, 0.9, 1.2, 1.4, 1.6,
             2.0, 2.5, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.5, 5.0,
             5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0,
             14.0, 15.0, 16.0, 18.0, 22.0, 26.0, 35.0, 50.0 };
*/
// Coarser binning at high pT and, in addition, coarser binning around crossings for pp 7 TeV
/*
const Int_t nPtBins = 50;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
             0.5, 0.55, 0.6,  0.65, 0.7, 0.75, 0.9, 1.1, 1.2, 1.3,
             1.4, 1.6, 1.8, 2.0, 2.9, 3.0, 3.2, 3.4, 3.6, 3.8,
             4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0,
             11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 22.0, 26.0, 35.0,
             50.0 };
*/
/*
// Coarser binning at high pT and, in addition, coarser binning around crossings for pp 2.76 TeV FINAL version
const Int_t nPtBins = 45;
const Double_t binsPt[nPtBins+1] = {0., 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45,
             0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 1.05,  
             1.2, 1.3, 1.4, 1.8, 2.9, 3.4, 3.6, 3.8, 4.0, 4.5, 
             5.0, 5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 
             13.0, 14.0, 15.0, 16.0, 20.0, 50.0 };
*/
/*
// Coarser binning at high pT and, in addition, coarser binning around crossings for pp 2.76 TeV NEW version
const Int_t nPtBins = 49;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
             0.5,  0.55, 0.6,  0.65, 0.7,  0.75, 0.8,  0.85, 0.9,
             1.05,  1.2,  1.3 , 1.4,  1.7,  2.4, 3.2 , 3.4 , 3.6,  3.8 ,
             4.0,  4.5 , 5.0,  5.5 , 6.0,  6.5 , 7.0,  8.0 , 9.0,  10.0,
             11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 22.0, 26.0, 35.0,
             50.0 };
*/
/*
// Coarser binning at high pT and, in addition, coarser binning around crossings for pp 2.76 TeV
const Int_t nPtBins = 50;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
             0.5,  0.55, 0.6,  0.65, 0.7,  0.75, 0.8,  0.85, 0.9,  0.95,
             1.0,  1.1 , 1.2,  1.3 , 1.4, 1.8,  2.4, 3.2,  3.6,  3.8 ,
             4.0,  4.5 , 5.0,  5.5 , 6.0,  6.5 , 7.0,  8.0 , 9.0,  10.0,
             11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 22.0, 26.0, 35.0,
             50.0 };
*/

/*
//coarser binning at high pT
const Int_t nPtBins = 60;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
             0.5,  0.55, 0.6,  0.65, 0.7,  0.75, 0.8,  0.85, 0.9,  0.95,
             1.0,  1.1 , 1.2,  1.3 , 1.4,  1.5 , 1.6,  1.7 , 1.8,  1.9 ,
             2.0,  2.2 , 2.4,  2.6 , 2.8,  3.0 , 3.2,  3.4 , 3.6,  3.8 ,
             4.0,  4.5 , 5.0,  5.5 , 6.0,  6.5 , 7.0,  8.0 , 9.0,  10.0,
             11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 22.0, 26.0, 35.0,
             50.0 };
*/

/* OLD default as used in PID-Task to create THnSparses
const Int_t nPtBins = 68;
const Double_t binsPt[nPtBins+1] = {0. ,  0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35, 0.4,  0.45,
			       0.5,  0.55, 0.6,  0.65, 0.7,  0.75, 0.8,  0.85, 0.9,  0.95,
			       1.0,  1.1 , 1.2,  1.3 , 1.4,  1.5 , 1.6,  1.7 , 1.8,  1.9 ,
			       2.0,  2.2 , 2.4,  2.6 , 2.8,  3.0 , 3.2,  3.4 , 3.6,  3.8 ,
			       4.0,  4.5 , 5.0,  5.5 , 6.0,  6.5 , 7.0,  8.0 , 9.0,  10.0,
			       11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 20.0, 22.0, 24.0,
			       26.0, 28.0, 30.0, 32.0, 34.0, 36.0, 40.0, 45.0, 50.0 };
*/
/* "NEW" default as used in PID-task to create THnSparses (from 2015_04_21 on) - old binning is a true subset:
const Int_t nPtBins = 73;
Double_t binsPt[nPtBins + 1] = {0.,  0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45,
                                  0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95,
                                   1.,  1.1, 1.2,  1.3, 1.4,  1.5, 1.6,  1.7, 1.8,  1.9,
                                   2.,  2.1, 2.2,  2.3, 2.4,  2.5, 2.6,  2.7, 2.8,  2.9, 
                                   3.,  3.2, 3.4,  3.6, 3.8,   4., 4.5,   5., 5.5,   6.,
                                  6.5,   7.,  8.,   9., 10.,  11., 12.,  13., 14.,  15.,
                                  16.,  18., 20.,  22., 24.,  26., 28.,  30., 32.,  34., 
                                  36.,  40., 45.,  50. };
*/

const Int_t nDeDxBins = 19;
const Double_t binsDeDx[nDeDxBins+1] = {50., 52., 54., 56., 58., 60., 65., 70., 75., 80., 
                                        85., 90., 100., 120., 160., 200., 250., 300., 400., 600. };	       
/*			       
const Int_t nDeDxBins = 35;
const Double_t binsDeDx[nDeDxBins+1] = {50., 51., 52., 53., 54., 55., 56., 57., 58., 59.,
             60., 62., 64., 66., 68., 70., 72., 74., 76., 78.,
             80., 85., 90., 95., 100., 120., 140., 160., 180., 200.,
             250., 300., 350., 400., 500, 650.};*/

//_____________________________________________________________________________________________________________________________
Bool_t CentralityHasDecimalsPlaces(Double_t cent)
{
  // Check, whether the centrality has decimals places or is an integer
  const Double_t temp1 = ((Int_t)cent)*1e6;
  const Double_t temp2 = cent*1e6;
  
  return TMath::Abs(temp1 - temp2) > 1;
}


//____________________________________________________________________________________________________________________
Int_t getLineColor(Int_t ID) {
  switch (ID) {
    case kEl:
      // El
      return kMagenta;
    case kKa:
      // Ka
      return kGreen;
    case kMu:
      // Mu
      return kOrange -3;
    case kPi:
      // Pi
      return kRed;
    case kPr:
      // Pr
      return kBlue;
    case kMuPlusPi:
      // Muons plus pions
      return kCyan;
    default:
      return 0;
  }
  
  return 0;
}


//____________________________________________________________________________________________________________________
Int_t getLineColorAliPID(Int_t ID) {
  switch (ID) {
    case AliPID::kElectron:
      // El
      return kMagenta;
    case AliPID::kKaon:
      // Ka
      return kGreen;
    case AliPID::kMuon:
      // Mu
      return kOrange - 3;
    case AliPID::kPion:
      // Pi
      return kRed;
    case AliPID::kProton:
      // Pr
      return kBlue;
    default:
      return 0;
  }
  
  return 0;
}


//____________________________________________________________________________________________________________________
void ClearTitleFromHistoInCanvas(TCanvas* c, Int_t padNum = -1)
{
  // Remove the title from a histogram plotted in the canvas without 
  // removing the title from the histogram itself.
  // If padNum is >= 0, this method will be applied to the corresponding
  // pad number
  
  c->Update();    // Update in order to have access to the title in the following

  TPaveText* paveTextTitle = (padNum >= 0) ? (TPaveText*)c->GetPad(padNum)->FindObject("title") : (TPaveText*)c->FindObject("title");
  if (paveTextTitle) 
    paveTextTitle->Clear();
  
  c->Modified();
}


//____________________________________________________________________________________________________________________
void ClearTitleFromHistoInPad(TPad* c)
{
  // Remove the title from a histogram plotted in the pad without 
  // removing the title from the histogram itself.
  
  c->Update();    // Update in order to have access to the title in the following

  TPaveText* paveTextTitle = (TPaveText*)c->FindObject("title");
  if (paveTextTitle) 
    paveTextTitle->Clear();
  
  c->Modified();
}


//____________________________________________________________________________________________________________________
Int_t GetAxisByTitle(const THnSparse* h, TString title)
{
  if (!h)
    return -1;
  
  for (Int_t iDim = 0; iDim < h->GetNdimensions(); iDim++) {
    if (!title.CompareTo(h->GetAxis(iDim)->GetTitle()))
      return iDim;
  }

  return -1;
}


//____________________________________________________________________________________________________________________
Double_t GetErrorRatioIndependent(Double_t num, Double_t numErr, Double_t den, Double_t denErr)
{
  if (num <= 0)
    return 0.;
  
  const Double_t den2 = den*den;
  return TMath::Sqrt((numErr*numErr * den2 + denErr*denErr * num*num) / (den2*den2));
}



//____________________________________________________________________________________________________________________
const Double_t* GetPtBins(Int_t type, Int_t& nBins)
{
  switch (type) {
    case kPtBinTypeJets:
      nBins = nPtBinsJets;
      return binsPtJets;
      break;
    case kPtBinTypePPMultOld:
      nBins = nPtBinsPPmultOld;
      return binsPtPPmultOld;
      break;
    case kPtBinTypePPMult:
      nBins = nPtBinsPPmult;
      return binsPtPPmult;
      break;
    case kPtBinTypePP13TeV:
      nBins = nPtBinsPP13TeV;
      return binsPtPP13TeV;
      break;
    default:
      break;
  }
  
  printf("ERROR: Requested unknown bin type!");
  nBins = -999;
  
  return 0x0;
}


//_____________________________________________________________________________________________________________________________
TH1D* GetRelErrorHisto(TH1D* h, TString name, TString yAxisTitle = "Rel. sys. error")
{
  if (!h)
    return 0x0;
  
  TH1D* hRelErr = new TH1D(*h);
  hRelErr->SetName(name.Data());
  hRelErr->GetYaxis()->SetTitle(yAxisTitle.Data());
  
  for (Int_t i = 1; i <= h->GetNbinsX(); i++) {
    const Double_t val = h->GetBinContent(i);
    const Double_t err = h->GetBinError(i);
    
    Double_t relErr = 0.;
    if (val > 0) 
      relErr = err / val;
    
    hRelErr->SetBinContent(i, relErr);
  }
  
  return hRelErr;
}


//_____________________________________________________________________________________________________________________________
TGraphAsymmErrors* GetRelErrorGraph(TGraphAsymmErrors* gr, TString name, TString yAxisTitle = "Rel. sys. error")
{
  if (!gr)
    return 0x0;
  
  TGraphAsymmErrors* grRelErr = new TGraphAsymmErrors(*gr);
  grRelErr->SetName(name.Data());
  grRelErr->GetYaxis()->SetTitle(yAxisTitle.Data());
  
  for (Int_t i = 0; i < gr->GetN(); i++) {
    const Double_t val = gr->GetY()[i];
    const Double_t errLow = gr->GetEYlow()[i];
    const Double_t errHigh = gr->GetEYhigh()[i];
    
    Double_t relErrLow = 0.;
    Double_t relErrHigh = 0.;
    if (val > 0) {
      relErrLow = errLow / val;
      relErrHigh = errHigh / val;
    }
    
    grRelErr->SetPointEYlow(i, relErrLow);
    grRelErr->SetPointEYhigh(i, relErrHigh);
  }
  
  return grRelErr;
}


//____________________________________________________________________________________________________________________
const Char_t* GetLatexNameParticleAntiParticle(Int_t species)
{
  switch (species) {
    case AliPID::kElectron:
      return "e^{+}+e^{-}";
      break;
    case AliPID::kMuon:
      return "#mu^{+}+#mu^{-}";
      break;
    case AliPID::kPion:
      return "#pi^{+}+#pi^{-}";
      break;
    case AliPID::kKaon:
      return "K^{+}+K^{-}";
      break;
    case AliPID::kProton:
      return "p+#bar{p}";
      break;
  }
  
  return "UNKNOWN SPECIES";
}


//____________________________________________________________________________________________________________________
const Char_t* GetLatexNamePosParticle(Int_t species)
{
  switch (species) {
    case AliPID::kElectron:
      return "e^{+}";
      break;
    case AliPID::kMuon:
      return "#mu^{+}";
      break;
    case AliPID::kPion:
      return "#pi^{+}";
      break;
    case AliPID::kKaon:
      return "K^{+}";
      break;
    case AliPID::kProton:
      return "p";
      break;
  }
  
  return "UNKNOWN SPECIES";
}


//____________________________________________________________________________________________________________________
const Char_t* GetLatexNameNegParticle(Int_t species)
{
  switch (species) {
    case AliPID::kElectron:
      return "e^{-}";
      break;
    case AliPID::kMuon:
      return "#mu^{-}";
      break;
    case AliPID::kPion:
      return "#pi^{-}";
      break;
    case AliPID::kKaon:
      return "K^{-}";
      break;
    case AliPID::kProton:
      return "#bar{p}";
      break;
  }
  
  return "UNKNOWN SPECIES";
}


//____________________________________________________________________________________________________________________
const Char_t* GetLatexNameParticleAndOrAntiParticle(Int_t species, chargeMode charge, Bool_t addBrackets = kTRUE)
{
  if (charge == kPosCharge)
    return GetLatexNamePosParticle(species);
  else if (charge == kNegCharge)
    return GetLatexNameNegParticle(species);
  
  return Form("%s%s%s", addBrackets ? "(" : "", GetLatexNameParticleAntiParticle(species), addBrackets ? ")" : "");
}

//____________________________________________________________________________________________________________________
TGraphAsymmErrors* HistToGraph(const TString grname, const TH1 *hh, const Double_t thres=0, const TH1 *herr=0x0, const Double_t xmin=-1e10, const Double_t xmax=1e10)
{
  if (!hh)
    return 0x0;
  
  const Int_t nbin = hh->GetNbinsX();
  Double_t xxs[nbin], yys[nbin], exs[nbin], eys[nbin];
  Int_t np=0;
  for(Int_t ii=1; ii<=nbin; ii++){
    const Double_t iyy = hh->GetBinContent(ii);
    if(iyy<=thres)
      continue;

    const Double_t iey = hh->GetBinError(ii);
    if(iey<1e-15){
      if(iyy>1e-15){
        printf("HistToGraph warning! should be fine if this is ratio %d %e %e\n", ii, iyy, iey); //exit(1);
      }
      //continue;
    }

    const Double_t ixx = hh->GetBinCenter(ii);
    if(ixx<xmin || ixx>xmax){
      //printf("test HistToGraph rejecting ixx %e xmin %e xmax %e\n", ixx, xmin, xmax);
      continue;
    }

    Double_t iex = 0;
    if(herr){
      iex = herr->GetBinContent(herr->GetXaxis()->FindBin(ixx));
    }
    else{
      iex = hh->GetBinWidth(ii)/2.;
    }

    xxs[np] = ixx;
    yys[np] = iyy;
    exs[np] = iex;
    eys[np] = iey;
    np++;
  }
  TGraphAsymmErrors * gr = new TGraphAsymmErrors(np, xxs, yys, exs, exs, eys, eys);
  gr->SetName(grname);
  gr->SetMaximum(hh->GetMaximum());
  gr->SetMinimum(hh->GetMinimum());
  gr->GetXaxis()->SetLimits(hh->GetXaxis()->GetXmin(), hh->GetXaxis()->GetXmax());
  
  gr->SetLineColor(hh->GetLineColor());
  gr->SetMarkerColor(hh->GetMarkerColor());
  gr->SetFillStyle(hh->GetFillStyle());
  gr->GetXaxis()->SetTitle(hh->GetXaxis()->GetTitle());
  gr->GetYaxis()->SetTitle(hh->GetYaxis()->GetTitle());
  return gr;
}


//____________________________________________________________________________________________________________________
void scaleGraph(TGraphAsymmErrors* g, Double_t scaleFactor, Double_t eps = 1e-5) {
  if (TMath::Abs(scaleFactor - 1.) < eps)
    return;
  
  if (!g)
    return;
  
  for (Int_t i = 0; i < g->GetN(); i++) {
    g->SetPoint(i, g->GetX()[i], g->GetY()[i] * scaleFactor);
    g->SetPointEYlow(i,  g->GetEYlow()[i] * scaleFactor);
    g->SetPointEYhigh(i, g->GetEYhigh()[i] * scaleFactor);
  }
}


//____________________________________________________________________________________________________________________
void setAxisTitlesItalic(TAxis* axis)
{
  if (!axis)
    return;
  
  TString temp = axis->GetTitle();
  temp = temp.ReplaceAll("p_", "#it{p}_");
  temp = temp.ReplaceAll("eV/c", "eV/#it{c}");
  temp = temp.ReplaceAll("z", "#it{z}^{ch}");
  temp = temp.ReplaceAll("#xi", "#it{#xi}^{ch}");
  temp = temp.ReplaceAll("N", "#it{N}");
  
  axis->SetTitle(temp.Data());
}


//____________________________________________________________________________________________________________________
void setAxisTitlesItalic(TH1* h)
{
  if (!h)
    return;
  
  TAxis* axis = 0;
  for (Int_t i = 0; i < 2; i++) {
    if (i == 0)
      axis = h->GetXaxis();
    else
      axis = h->GetYaxis();
    
    setAxisTitlesItalic(axis);
  }
}



//_____________________________________________________________________________________________________________________________
void SetErrorsGraphFromRelErrGraph(TGraphAsymmErrors* gr, TGraphAsymmErrors* grRelErrors)
{
  if (!gr || !grRelErrors) {
    printf("SetErrorsGraphFromRelErrGraph: Missing graph(s)!\n");
    return;
  }
  
  // Every point in gr needs a point in grRelErrors, but not vice-versa!
  if (gr->GetN() > grRelErrors->GetN()) {
    printf("SetErrorsGraphFromRelErrGraph: Graphs not consistent (different number of points)!\n");
    return;
  }
  
  for (Int_t i = 0; i < gr->GetN(); i++) {
    const Double_t val = gr->GetY()[i];
    
    if (val <= 0)
      continue;
    
    const Double_t x = gr->GetX()[i];
    const Double_t exLow = gr->GetEXlow()[i];
    const Double_t exHigh = gr->GetEXhigh()[i];
    
    Int_t iRelErrors = -1;
    
    for (Int_t j = 0; j < grRelErrors->GetN(); j++) {
      if (TMath::Abs(x - grRelErrors->GetX()[j]) < 1e-5 &&
          TMath::Abs(exLow - grRelErrors->GetEXlow()[j]) < 1e-5 &&
          TMath::Abs(exHigh - grRelErrors->GetEXhigh()[j]) < 1e-5) {
        iRelErrors = j;
        break;
      }
    }
    
    if (iRelErrors < 0) {
      printf("SetErrorsGraphFromRelErrGraph: Graphs not consistent (different different binning)!\n");
      return;
    }
    
    const Double_t relErrLow = grRelErrors->GetEYlow()[iRelErrors];
    const Double_t relErrHigh = grRelErrors->GetEYhigh()[iRelErrors];
    
    Double_t errLow = 0.;
    Double_t errHigh = 0.;
    if (val > 0) {
      errLow = relErrLow * val;
      errHigh = relErrHigh * val;
    }
    
    gr->SetPointEYlow(i, errLow);
    gr->SetPointEYhigh(i, errHigh);
  }
}


//_____________________________________________________________________________________________________________________________
void GetAxisRangeForMultiplicityAxisForMB(TAxis* axis, Int_t& low, Int_t& high, Bool_t noActionForOldRange = kTRUE)
{
  // Get range for the multiplicity axis for the MB range
  // Already checked: Works also for Integral(...)!
  
  // Old definition: MB = full range
  if (!useNewMultiplicityRangeForMB) {
    if (!noActionForOldRange) {
      low = 0;
      high = -1;
    }
  }
  else {
    // Get range to mult < 0 (including underflow)
    low = axis->FindFixBin(-9999);
    if (low == 0)
      low = -1; // To include underflow bin;
    high = axis->FindFixBin(-1e-5);
  }
}


//_____________________________________________________________________________________________________________________________
void PrintSettingsAxisRangeForMultiplicityAxisForMB()
{
  // Print which range is used for MB for the mult axis
  if (useNewMultiplicityRangeForMB) 
    printf("NEW range definition for the multiplicity in the MB case....\n");
  else
    printf("OLD range definition for the multiplicity in the MB case....\n");
}

#endif
