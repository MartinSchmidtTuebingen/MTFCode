#include "TFile.h"
#include "TCollection.h"

#ifndef GETOBJECTOUTOFDIRECTORY
#define GETOBJECTOUTOFDIRECTORY
TObject* GetObjectOutOfCollection(TCollection* coll, TString objectName);
TObject* GetObjectOutOfDirectory(TDirectoryFile* d, TString objectName);
TObject* GetObjectOutOfDirectory(TString fileName, TString objectName);
#endif

TObject* GetObjectOutOfDirectory(TString fileName, TString objectName) {
  TFile* f = new TFile(fileName.Data());
  return GetObjectOutOfDirectory(f,objectName);
}

TObject* GetObjectOutOfDirectory(TDirectoryFile* d, TString objectName) {
  if (!d || d->IsZombie())
    return 0x0;

  TObject* obj = 0x0;
  d->GetObject(objectName.Data(),obj);
  if (!obj) {
    TList* list = d->GetListOfKeys();
    for (Int_t n=0;n<list->GetEntries();n++) {
      TObject* dirobj = 0x0;
      d->GetObject(list->At(n)->GetName(),dirobj);
      if (!dirobj)
        continue;

      if (dirobj->InheritsFrom("TDirectory"))
        obj = GetObjectOutOfDirectory((TDirectoryFile*)dirobj, objectName);
      else if (dirobj->InheritsFrom("TCollection"))
        obj = GetObjectOutOfCollection((TCollection*)dirobj, objectName);

      if (obj)
        break;
    }
  }

  return obj;
}

TObject *GetObjectOutOfCollection(TCollection* coll, TString objectName) {
  TObject* obj = 0x0;
  obj = coll->FindObject(objectName.Data());
  
  if (!obj) {
    TIter it = TIter(coll);
    TObject* collobj = it();
    while (collobj) {
      if (collobj->InheritsFrom("TDirectory"))
        obj = GetObjectOutOfDirectory((TDirectoryFile*)collobj, objectName);
      else if (collobj->InheritsFrom("TCollection"))
        obj = GetObjectOutOfCollection((TCollection*)collobj, objectName);
      
      if (obj)
        break;
      
      collobj = it();
    }
  }
  return obj;
}
