#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <regex>
#include <limits>

#include "TFile.h"
#include "TTree.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TSystem.h"

#include "RAT/DU/DSReader.hh"
#include "RAT/DS/Entry.hh"
#include "RAT/DS/EV.hh"
#include "RAT/DS/FitVertex.hh"


std::vector<std::string> getFilesForRun(const std::string& dir,
                                       long long runNumber)
{
    std::vector<std::string> files;

    // Zero-pad run to match "r0000307969"
    char runStr[64];
    sprintf(runStr, "r%010lld", runNumber);

    // Directory listing
    TSystemDirectory sysDir("sysDir", dir.c_str());
    TList* flist = sysDir.GetListOfFiles();
    if (!flist) return files;

    // Regex to match: Analysis20_PPOR_r0000307969_sXXX_*.root
    std::regex pattern(Form(".*%s.*\\.root", runStr));

    TIter next(flist);
    while (TObject* obj = next()) {
        TSystemFile* file = dynamic_cast<TSystemFile*>(obj);
        if (!file || file->IsDirectory()) continue;

        std::string fname = file->GetName();
        if (std::regex_match(fname, pattern)) {
            files.push_back(dir + "/" + fname);
        }
    }

    return files;
}

bool loadEventFit(const std::vector<std::string>& files,
                  long long gtid,
                  double& fitx, double& fity, double& fitz, double& fitr)
{
    // Default to NaNs 
    fitx = fity = fitz = fitr = std::numeric_limits<double>::quiet_NaN();

    for (const auto& filename : files) {

        RAT::DU::DSReader ds(filename);

        for (size_t iEntry = 0; iEntry < ds.GetEntryCount(); iEntry++) {

            RAT::DS::Entry entry;
            try {
                entry = ds.GetEntry(iEntry);
            }
	    catch (const RAT::DS::DataNotFound&) {
                continue;
            }

            for (size_t iEv = 0; iEv < entry.GetEVCount(); iEv++) {

                const RAT::DS::EV& ev = entry.GetEV(iEv);

                if (ev.GetGTID() != gtid)
                    continue;

                // Found it → load scintFitter result
                RAT::DS::FitVertex vtx;
                try {
                    vtx = ev.GetFitResult("scintFitter").GetVertex(0);
                }
		catch (const RAT::DS::DataNotFound&) {
                    return false;  // GTID exists but no fit
                }

                if (!vtx.ValidPosition() || !vtx.ValidTime())
                    return false;

                fitx = vtx.GetPosition().X();
                fity = vtx.GetPosition().Y();
                fitz = vtx.GetPosition().Z();
                fitr = vtx.GetPosition().Mag();

                return true;
            }
        }
    }

    // Event not found
    return false;
}


int compare_reprocessing(const std::string& promptFile,
			 const std::string& delayedFile,
			 const std::string& outfilename)
{
  std::ifstream fp(promptFile);
  std::ifstream fd(delayedFile);

  if (!fp.is_open() || !fd.is_open()) {
    std::cerr << "Error: could not open one of the input files.\n";
    return 1;
  }

  std::string pline, dline;
  long long p_run, p_evt;
  long long d_run, d_evt;

  size_t line_number = 0;

  double fitxprompt1, fitxprompt2;
  double fityprompt1, fityprompt2;
  double fitzprompt1, fitzprompt2;
  double fitrprompt1, fitrprompt2;
  
  double fitxdelayed1, fitxdelayed2;
  double fitydelayed1, fitydelayed2;
  double fitzdelayed1, fitzdelayed2;
  double fitrdelayed1, fitrdelayed2;
  
  
  TFile *file = TFile::Open(outfilename.c_str(), "RECREATE");
  TTree *tree = new TTree("T","events");

  tree->SetAutoSave(0);
  tree->SetAutoFlush(-100000);
  tree->SetMaxVirtualSize(64*1024*1024);

  tree->Branch("fitxprompt1", &fitxprompt1);
  tree->Branch("fitxprompt2", &fitxprompt2);
  tree->Branch("fityprompt1", &fityprompt1);
  tree->Branch("fityprompt2", &fityprompt2);
  tree->Branch("fitzprompt1", &fitzprompt1);
  tree->Branch("fitzprompt2", &fitzprompt2);
  tree->Branch("fitrprompt1", &fitrprompt1);
  tree->Branch("fitrprompt2", &fitrprompt2);
  tree->Branch("fitxdelayed1", &fitxdelayed1);
  tree->Branch("fitxdelayed2", &fitxdelayed2);
  tree->Branch("fitydelayed1", &fitydelayed1);
  tree->Branch("fitydelayed2", &fitydelayed2);
  tree->Branch("fitzdelayed1", &fitzdelayed1);
  tree->Branch("fitzdelayed2", &fitzdelayed2);
  tree->Branch("fitrdelayed1", &fitrdelayed1);
  tree->Branch("fitrdelayed2", &fitrdelayed2);

  
  while (std::getline(fp, pline) && std::getline(fd, dline)) {
    line_number++;

    if (pline.empty() || dline.empty()) continue;

    // Parse prompt line
    {
      std::stringstream ss(pline);
      char comma;
      ss >> p_run >> comma >> p_evt;
    }

    // Parse delayed line
    {
      std::stringstream ss(dline);
      char comma;
      ss >> d_run >> comma >> d_evt;
    }

    std::cout << "Line " << line_number << ":\n";
    std::cout << "  Prompt:  run " << p_run << ", event " << p_evt << "\n";
    std::cout << "  Delayed: run " << d_run << ", event " << d_evt << "\n";
    std::cout << "-----------------------------------------\n";

    std::string prompt1_dir = "/data/snoplus3/parkerw/ratSimulations/Nov20_origSFPrompt";
    std::string delayed1_dir = "/data/snoplus3/parkerw/ratSimulations/Nov20_origSFDelay";
    std::string prompt2_dir = "/data/snoplus3/parkerw/ratSimulations/Oct8_fastSFPrompt_copy";
    std::string delayed2_dir = "/data/snoplus3/parkerw/ratSimulations/Oct7_fastSFDelay_copy";
    
    std::vector<std::string> prompt1_files = getFilesForRun(prompt1_dir, p_run);
    std::vector<std::string> prompt2_files = getFilesForRun(prompt2_dir, p_run);
    std::vector<std::string> delayed1_files = getFilesForRun(delayed1_dir, d_run);
    std::vector<std::string> delayed2_files = getFilesForRun(delayed2_dir, d_run);
    
    bool foundp1 = loadEventFit(prompt1_files, p_evt,
                            fitxprompt1, fityprompt1, fitzprompt1, fitrprompt1);

    bool foundp2 = loadEventFit(prompt2_files, p_evt,
                            fitxprompt2, fityprompt2, fitzprompt2, fitrprompt2);

    bool foundd1 = loadEventFit(delayed1_files, d_evt,
                            fitxdelayed1, fitydelayed1, fitzdelayed1, fitrdelayed1);

    bool foundd2 = loadEventFit(delayed2_files, d_evt,
                            fitxdelayed2, fitydelayed2, fitzdelayed2, fitrdelayed2);

    tree->Fill();

  }

  file->cd();

  tree->Write("", TObject::kOverwrite);
  file->Write();
  file->Close();

  std::cout << "Done.\n";
  
  return 0;
}


void compareBiPo(const char* prompt, const char* delayed, const char* outfilename) {
    compare_reprocessing(prompt, delayed, outfilename);
}
