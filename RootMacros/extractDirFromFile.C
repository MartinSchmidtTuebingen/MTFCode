#include "TFile.h"
#include "TSystem.h"
#include "TObjArray.h"

#include <iostream>

void extractDirFromFile(TString fileName, TString dirToExtract) {
  TFile* f = new TFile(fileName.Data(), "READ");
  TDirectoryFile* df = (TDirectoryFile*)f->FindObjectAny(dirToExtract);
  TObjArray* arr = (TObjArray*)df->FindObjectAny(dirToExtract);
  TString saveFileName = TString(gSystem->DirName(fileName.Data())) + "/" + dirToExtract + ".root";
  TFile* g = new TFile(saveFileName, "RECREATE");
  g->cd();
  arr->Write(arr->GetName(), TObject::kSingleKey);
  g->Close();
  f->Close();
}
