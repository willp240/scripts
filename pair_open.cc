#include <TFile.h>
#include <TTree.h>

#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

namespace fs = std::filesystem;

struct Pair {
  long long run{};
  int sub{};
  int part{};
  std::string reproc_path;
  std::string orig_path;
};

static TFile* SafeOpen(const std::string& path) {
  TFile* f = TFile::Open(path.c_str(), "READ");
  if (!f || f->IsZombie()) {
    throw std::runtime_error("Failed to open ROOT file: " + path);
  }
  return f;
}

int main() {

  const fs::path reproc_dir = "/data/snoplus3/parkerw/ratSimulations/Oct6_Ana20PPO";
  const fs::path orig_dir   = "/data/snoplus3/SNOplusData/processing/fullFill/rat-8.0.1/Analysis20_PPOR/ratds";

  if (!fs::exists(reproc_dir) || !fs::is_directory(reproc_dir)) {
    std::cerr << "[ERROR] Reprocessed directory not found: " << reproc_dir << "\n";
    return 1;
  }
  if (!fs::exists(orig_dir) || !fs::is_directory(orig_dir)) {
    std::cerr << "[ERROR] Original directory not found: " << orig_dir << "\n";
    return 1;
  }

  // Reprocessed filename: Analysis20_PPOR_r0000307969_s014_p005.root_reproc.root
  // We capture the zero-padded strings (for name reconstruction) but also parse ints for sorting.
  const std::regex reproc_rx(
      R"(^(Analysis20_PPOR_r(\d+)_s(\d+)_p(\d+)\.root)_reproc\.root$)");

  std::vector<Pair> pairs;
  std::vector<std::string> skipped;
  std::vector<std::pair<std::string,std::string>> missing;

  // Walk reproc_dir for files ending with .root_reproc.root (anything else in there isn't a ROOT file)
  for (const auto& dirent : fs::directory_iterator(reproc_dir)) {
    if (!dirent.is_regular_file()) continue;

    const auto filename = dirent.path().filename().string();

    // Quick suffix filter
    const std::string path_str = dirent.path().string();
    if (path_str.size() < 17 ||
	path_str.compare(path_str.size() - 17, 17, ".root_reproc.root") != 0) {
      continue;  // not one of the reproc files we care about
    }

    std::smatch m;
    if (!std::regex_match(filename, m, reproc_rx)) {
      skipped.push_back(filename);
      continue;
    }

    // m[1] = "Analysis20_PPOR_r..._s..._p....root" (without _reproc.root)
    const std::string base_no_reproc = m[1];
    const std::string run_str = m[2];
    const std::string sub_str = m[3];
    const std::string part_str = m[4];

    // Construct expected original filename (exact zero-padding preserved)
    const std::string orig_name = base_no_reproc; // already ends with ".root"
    const fs::path orig_path = orig_dir / orig_name;

    if (!fs::exists(orig_path)) {
      missing.emplace_back(dirent.path().string(), orig_path.string());
      continue;
    }

    // Numeric keys for stable ordering
    long long run = 0;
    int sub = 0, part = 0;
    try {
      run  = std::stoll(run_str);
      sub  = std::stoi(sub_str);
      part = std::stoi(part_str);
    } catch (...) {
      skipped.push_back(filename);
      continue;
    }

    pairs.push_back(Pair{
        run, sub, part,
        dirent.path().string(),
        orig_path.string()
    });
  }

  // Sort by (run, sub, part)
  std::sort(pairs.begin(), pairs.end(),
            [](const Pair& a, const Pair& b) {
              return std::tie(a.run, a.sub, a.part) < std::tie(b.run, b.sub, b.part);
            });

  std::cout << "Matched file pairs: " << pairs.size() << "\n";
  if (!missing.empty()) {
    std::cerr << "WARNING: " << missing.size() << " reprocessed files have no matching original.\n";
    size_t shown = 0;
    for (const auto& kv : missing) {
      if (shown++ >= 10) { std::cerr << "  ... and " << (missing.size() - 10) << " more\n"; break; }
      std::cerr << "  missing orig for: "
                << fs::path(kv.first).filename().string()
                << "  -> expected "
                << fs::path(kv.second).filename().string()
                << "\n";
    }
  }
  if (!skipped.empty()) {
    std::cerr << "NOTE: " << skipped.size()
              << " files in reproc dir did not match expected pattern and were skipped.\n";
  }

  double fitx1, fity1, fitz1, fitr1, fitx2, fity2, fitz2, fitr2;
  int nhits;

  // Declare tree with branches: mcx mcy mcz mcr mce fitx1 fity1 fitz1 fitr1 fitx2 fity2 fitz2 fitr2
  TFile *file = TFile::Open("ppodatatree.root", "RECREATE");
  TTree *tree = new TTree("T","events");
  tree->Branch("fitx1", &fitx1);
  tree->Branch("fity1", &fity1);
  tree->Branch("fitz1", &fitz1);
  tree->Branch("fitr1", &fitr1);
  tree->Branch("fitx2", &fitx2);
  tree->Branch("fity2", &fity2);
  tree->Branch("fitz2", &fitz2);
  tree->Branch("fitr2", &fitr2);
  tree->Branch("nhits", &nhits);
  
  // Example: iterate pairs and open both files
  for (const auto& pr : pairs) {
    // You can early filter here if desired (e.g., only a specific run/sub/part).
    std::cout << "Opening r=" << pr.run << " s=" << pr.sub << " p=" << pr.part << "\n";

    RAT::DU::DSReader dsreaderOrig(pr.reproc_path);
    RAT::DU::DSReader dsreaderUpdated(pr.orig_path);

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
	    if(!(rVertex1.ValidPosition() && rVertex1.ValidTime()))
	      continue; // fit invalid
	    
	    fitx1 = rVertex1.GetPosition().X();
	    fity1 = rVertex1.GetPosition().Y();
	    fitz1 = rVertex1.GetPosition().Z();
	    fitr1 = rVertex1.GetPosition().Mag();
	    
	    const RAT::DS::FitVertex& rVertex2 = rev2.GetFitResult("scintFitter").GetVertex(0);
	    if(!(rVertex2.ValidPosition() && rVertex2.ValidTime()))
	      continue; // fit invalid
	    
	    fitx2 = rVertex2.GetPosition().X();
	    fity2 = rVertex2.GetPosition().Y();
	    fitz2 = rVertex2.GetPosition().Z();
	    fitr2 = rVertex2.GetPosition().Mag();
	    
	  }
	  catch(const RAT::DS::FitCollection::NoResultError&){
	    continue;
	  }
	  // Fill tree
	  tree->Fill();
	}
      }

    
  }

  tree->Write();
  
  std::cout << "Done.\n";
  return 0;
}
