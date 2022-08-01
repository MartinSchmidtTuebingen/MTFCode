#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"

void GetCanvasOutOfFile(TString filename, TString canvasName) {
  TFile* f = new TFile(filename.Data(), "READ");
  TCanvas* c = (TCanvas*)f->FindObjectAny(canvasName.Data());
  c->Draw();
}
