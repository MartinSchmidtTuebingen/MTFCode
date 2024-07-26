#include "TFile.h"
#include "TSystem.h"
#include "TObjArray.h"

#include <iostream>

Int_t extractDirFromFile(TString fileName, TString dirToExtract) {
  TFile* f = new TFile(fileName.Data(), "READ");
  if (f->IsZombie()) {
    std::cout << "File " << fileName << " not found" << std::endl;
    return -1;
  }
  TDirectoryFile* df = (TDirectoryFile*)f->FindObjectAny(dirToExtract);
  if (!df) {
    std::cout << "Directory " << dirToExtract << " not found." << std::endl;
    return -1;
  }
  TObjArray* arr = (TObjArray*)df->FindObjectAny(dirToExtract);
  if (!arr) {
    std::cout << "Array " << dirToExtract << " not found." << std::endl;
    return -1;
  }
  TString saveFileName = TString(gSystem->DirName(fileName.Data())) + "/" + dirToExtract + ".root";
  TFile* g = new TFile(saveFileName, "RECREATE");
  g->cd();
  arr->Write(arr->GetName(), TObject::kSingleKey);
  g->Close();
  f->Close();
  return 0;
}
