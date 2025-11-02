// pair_open.C
// ROOT macro to pair reprocessed files with originals and open both.
// Run: root -l -q 'pair_open.C()'
// or:  root -l -q 'pair_open.C("reproc_dir","orig_dir","T","T")'

#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TList.h>
#include <TObjString.h>
#include <TString.h>

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

struct Pair {
  long long run{0};
  int sub{0};
  int part{0};
  std::string reproc_path;
  std::string orig_path;
};

static TFile* SafeOpen(const std::string& path) {
  TFile* f = TFile::Open(path.c_str(), "READ");
  if (!f || f->IsZombie()) {
    throw std::runtime_error(std::string("Failed to open ROOT file: ") + path);
  }
  return f;
}

// Helper: returns true if s ends with suffix
static bool EndsWith(const TString& s, const char* suffix) {
  return s.EndsWith(suffix);
}

// Parse run/sub/part from a filename of form:
//   Analysis20_PPOR_r0000307969_s014_p005.root_reproc.root
// Returns true on success.
static bool ParseRSP(const char* fname, long long& run, int& sub, int& part) {
  // We don't rely on fixed padding; %lld/%d/%d will parse the digits.
  // Use two patterns (with and without _reproc) just in case.
  int n = std::sscanf(fname,
                      "Analysis20_bMR_r%lld_s%d_p%d.root_reproc.root",
                      &run, &sub, &part);
  if (n == 3) return true;
  n = std::sscanf(fname,
                  "Analysis20_bMR_r%lld_s%d_p%d.root",
                  &run, &sub, &part);
  return n == 3;
}

void pair_open_bismsb(const char* reproc_dir =
                 "/data/snoplus3/parkerw/ratSimulations/Oct6_Ana20bMR",
               const char* orig_dir   =
                 "/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/Analysis20_bMR/ratds",
               const char* tree_reproc = "T",
               const char* tree_orig   = "T") {
  // Quick directory checks
  if (gSystem->AccessPathName(reproc_dir, kReadPermission)) {
    std::cerr << "[ERROR] Cannot access reproc dir: " << reproc_dir << "\n";
    return;
  }
  if (gSystem->AccessPathName(orig_dir, kReadPermission)) {
    std::cerr << "[ERROR] Cannot access orig dir: " << orig_dir << "\n";
    return;
  }

  std::vector<Pair> pairs;
  std::vector<std::pair<std::string,std::string>> missing; // (reproc, expected orig)
  std::vector<std::string> skipped;

  // List files in reproc_dir
  TSystemDirectory d("reproc", reproc_dir);
  TList* flist = d.GetListOfFiles();
  if (!flist) {
    std::cerr << "[ERROR] No file list from: " << reproc_dir << "\n";
    return;
  }

  TIter it(flist);
  int count = 0;
  while (TSystemFile* f = (TSystemFile*)it()) {

    if (f->IsDirectory()) continue;
    TString fname = f->GetName();

    // We only care about *.root_reproc.root
    if (!EndsWith(fname, ".root_reproc.root")) continue;

    long long run = 0;
    int sub = 0, part = 0;
    if (!ParseRSP(fname.Data(), run, sub, part)) {
      skipped.emplace_back(fname.Data());
      continue;
    }

    count++;
    
    if(count > 2)
      break;

    
    // Original filename is the same without the "_reproc" before .root
    TString origName = fname;
    origName.ReplaceAll(".root_reproc.root", ".root");

    TString reprocPath = TString::Format("%s/%s", reproc_dir, fname.Data());
    TString origPath   = TString::Format("%s/%s", orig_dir,   origName.Data());

    if (gSystem->AccessPathName(origPath.Data(), kReadPermission)) {
      missing.emplace_back(reprocPath.Data(), origPath.Data());
      std::cout << reprocPath.Data() << " " << origPath.Data() << std::endl;
      continue;
    }

    pairs.push_back(Pair{
      run, sub, part,
      reprocPath.Data(),
      origPath.Data()
    });
  }

  // Sort by (run, sub, part)
  std::sort(pairs.begin(), pairs.end(),
            [](const Pair& a, const Pair& b){
              return std::tie(a.run, a.sub, a.part) < std::tie(b.run, b.sub, b.part);
            });

  std::cout << "Matched file pairs: " << pairs.size() << "\n";
  if (!missing.empty()) {
    std::cerr << "WARNING: " << missing.size()
              << " reprocessed files have no matching original. Showing up to 10:\n";
    for (size_t i = 0; i < missing.size() && i < 10; ++i) {
      const auto& kv = missing[i];
      std::cerr << "  missing orig for: "
                << kv.first.substr(kv.first.find_last_of("/\\")+1)
                << "  -> expected "
                << kv.second.substr(kv.second.find_last_of("/\\")+1)
                << "\n";
    }
    if (missing.size() > 10)
      std::cerr << "  ... and " << (missing.size() - 10) << " more\n";
  }
  if (!skipped.empty()) {
    std::cerr << "NOTE: " << skipped.size()
              << " files in reproc dir did not match expected pattern and were skipped.\n";
  }

  double fitx1, fity1, fitz1, fitr1, fitx2, fity2, fitz2, fitr2;
  int nhits;

  // Declare tree with branches: mcx mcy mcz mcr mce fitx1 fity1 fitz1 fitr1 fitx2 fity2 fitz2 fitr2
  TFile *file = TFile::Open("bismsbdatatree.root", "RECREATE");
  TTree *tree = new TTree("T","events");

  tree->SetAutoSave(0);
  tree->SetAutoFlush(-100000);
  tree->SetMaxVirtualSize(64*1024*1024);
  
  tree->Branch("fitx1", &fitx1);
  tree->Branch("fity1", &fity1);
  tree->Branch("fitz1", &fitz1);
  tree->Branch("fitr1", &fitr1);
  tree->Branch("fitx2", &fitx2);
  tree->Branch("fity2", &fity2);
  tree->Branch("fitz2", &fitz2);
  tree->Branch("fitr2", &fitr2);
  tree->Branch("nhits", &nhits);
  
  // ------- OPEN + EVENT LOOP SKELETON -------
  for (const auto& pr : pairs) {
    std::cout << "Opening r=" << pr.run << " s=" << pr.sub << " p=" << pr.part << "\n";

    RAT::DU::DSReader dsreaderOrig(pr.orig_path);
    RAT::DU::DSReader dsreaderUpdated(pr.reproc_path);

    for( size_t iEntry = 0; iEntry < dsreaderOrig.GetEntryCount(); iEntry++ )
      {
        const RAT::DS::Entry& rDS1 = dsreaderOrig.GetEntry( iEntry );
        const RAT::DS::Entry& rDS2 = dsreaderUpdated.GetEntry( iEntry );

        // Loop over events
        for(size_t iEv = 0; iEv< rDS1.GetEVCount(); iEv++){

          // Get from file 1
          const RAT::DS::EV& rev1 = rDS1.GetEV(iEv);
          // Get from file 2
          const RAT::DS::EV& rev2 = rDS2.GetEV(iEv);

          try{

            const RAT::DS::FitVertex& rVertex1 = rev1.GetFitResult("scintFitter").GetVertex(0);
	    const RAT::DS::FitVertex& rVertex2 = rev2.GetFitResult("scintFitter").GetVertex(0);

	    if (rev1.GetGTID() != rev2.GetGTID()) continue;

            if(!(rVertex1.ValidPosition() && rVertex1.ValidTime()))
              continue; // fit invalid

            fitx1 = rVertex1.GetPosition().X();
            fity1 = rVertex1.GetPosition().Y();
            fitz1 = rVertex1.GetPosition().Z();
            fitr1 = rVertex1.GetPosition().Mag();

            if(!(rVertex2.ValidPosition() && rVertex2.ValidTime()))
              continue; // fit invalid

            fitx2 = rVertex2.GetPosition().X();
            fity2 = rVertex2.GetPosition().Y();
            fitz2 = rVertex2.GetPosition().Z();
            fitr2 = rVertex2.GetPosition().Mag();

	    if(rev1.GetGTID() != rev2.GetGTID())
	      continue;
	    
          }
          catch(...){
            continue;
          }

	  // Fill tree
          tree->Fill();
	  tree->AutoSave("SaveSelf");
        }  // end loop events  
      } // end loop entries
  } // end loop files

  // make sure we’re writing to the output file
  file->cd();

  tree->Write("", TObject::kOverwrite);
  file->Write();
  file->Close();
    
  std::cout << "Done.\n";
}
