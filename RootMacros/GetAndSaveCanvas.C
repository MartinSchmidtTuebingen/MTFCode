#include "TCanvas.h"

#include "GetObjectOutOfDirectory.C"

void GetAndSaveCanvas(TString filename, TString savename, TString canvasName) {
//   TString canvasName = "cCorrFractionsWithMC";
//   gROOT->LoadMacro("GetObjectOutOfDirectory.C");
//   gROOT->LoadMacro("changeCanvas.C");
  TCanvas* c = (TCanvas*)GetObjectOutOfDirectory(filename, canvasName)->Clone();
//   changeCanvas(c);
  c->Draw();
  c->SaveAs(savename.Data());
  delete c;
}
