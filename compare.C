#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
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

struct EventInfo {
    long long run;
    long long gtid;
};

// ------------------------------------------------------------
// Read text file: each line "run, gtid"
// ------------------------------------------------------------
std::vector<EventInfo> readEventList(const std::string& filename) {
    std::vector<EventInfo> events;

    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "Error: could not open " << filename << std::endl;
        return events;
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        char comma;
        EventInfo e;
        ss >> e.run >> comma >> e.gtid;
        if (!ss.fail()) {
            events.push_back(e);
        }
    }

    std::cout << "Read " << events.size() << " events from " << filename << std::endl;
    return events;
}

// ------------------------------------------------------------
// List all .root files in a directory
// ------------------------------------------------------------
std::vector<std::string> listRootFiles(const std::string& dir) {
    std::vector<std::string> files;

    TSystemDirectory sysDir("sysDir", dir.c_str());
    TList* flist = sysDir.GetListOfFiles();
    if (!flist) {
        std::cerr << "Warning: could not list files in " << dir << std::endl;
        return files;
    }

    TIter next(flist);
    while (TObject* obj = next()) {
        TSystemFile* file = dynamic_cast<TSystemFile*>(obj);
        if (!file || file->IsDirectory()) continue;

        std::string fname = file->GetName();
        if (fname.size() >= 5 && fname.substr(fname.size() - 5) == ".root") {
            files.push_back(dir + "/" + fname);
        }
    }

    std::cout << "Found " << files.size() << " ROOT files in " << dir << std::endl;
    return files;
}

// ------------------------------------------------------------
// Scan all files in 'files' and fill fit arrays where GTID matches
// gtidToIndex tells us which index in the vectors corresponds to a GTID
// ------------------------------------------------------------
void fillFitsFromFiles(const std::vector<std::string>& files,
                       const std::unordered_map<long long, size_t>& gtidToIndex,
                       std::vector<double>& fitx,
                       std::vector<double>& fity,
                       std::vector<double>& fitz,
                       std::vector<double>& fitr,
                       const std::string& label)
{
    size_t nMatched = 0;

    for (const auto& filename : files) {
        std::cout << "[" << label << "] Opening file: " << filename << std::endl;

        RAT::DU::DSReader ds(filename);

        const size_t nEntries = ds.GetEntryCount();
        for (size_t iEntry = 0; iEntry < nEntries; iEntry++) {
            RAT::DS::Entry entry;
            try {
                entry = ds.GetEntry(iEntry);
            } catch (const RAT::DS::DataNotFound&) {
                continue;
            }

            const size_t nEv = entry.GetEVCount();
            for (size_t iEv = 0; iEv < nEv; iEv++) {
                const RAT::DS::EV& ev = entry.GetEV(iEv);
                long long gtid = ev.GetGTID();

                auto it = gtidToIndex.find(gtid);
                if (it == gtidToIndex.end())
                    continue; // this event isn't in our list

                size_t idx = it->second;

                RAT::DS::FitVertex vtx;
                try {
                    vtx = ev.GetFitResult("scintFitter").GetVertex(0);
                } catch (const RAT::DS::DataNotFound&) {
                    // Event exists but no fit result – leave as NaN
                    continue;
                }

                if (!vtx.ValidPosition() || !vtx.ValidTime()) {
                    // Invalid fit – leave as NaN
                    continue;
                }

                fitx[idx] = vtx.GetPosition().X();
                fity[idx] = vtx.GetPosition().Y();
                fitz[idx] = vtx.GetPosition().Z();
                fitr[idx] = vtx.GetPosition().Mag();

                nMatched++;
            }
        }
    }

    std::cout << "[" << label << "] Matched and filled " << nMatched << " events." << std::endl;
}

// ------------------------------------------------------------
// Main driver
// ------------------------------------------------------------
int compare_reprocessing(const std::string& promptFile,
                         const std::string& delayedFile,
                         const std::string& outfilename)
{
    // 1. Read text lists
    std::vector<EventInfo> promptEvents = readEventList(promptFile);
    std::vector<EventInfo> delayedEvents = readEventList(delayedFile);

    if (promptEvents.size() != delayedEvents.size()) {
        std::cerr << "Warning: prompt list has " << promptEvents.size()
                  << " entries but delayed list has " << delayedEvents.size()
                  << " entries." << std::endl;
    }

    const size_t N = std::min(promptEvents.size(), delayedEvents.size());
    if (N == 0) {
        std::cerr << "No events to process. Exiting." << std::endl;
        return 1;
    }

    // 2. Build GTID -> index maps
    std::unordered_map<long long, size_t> promptIndexByGTID;
    std::unordered_map<long long, size_t> delayedIndexByGTID;
    promptIndexByGTID.reserve(N * 2);
    delayedIndexByGTID.reserve(N * 2);

    for (size_t i = 0; i < N; i++) {
        promptIndexByGTID[promptEvents[i].gtid] = i;
        delayedIndexByGTID[delayedEvents[i].gtid] = i;
    }

    // 3. Allocate fit arrays, initialised to NaN
    auto makeVec = [N]() {
        return std::vector<double>(N, std::numeric_limits<double>::quiet_NaN());
    };

    std::vector<double> fitxprompt1 = makeVec();
    std::vector<double> fityprompt1 = makeVec();
    std::vector<double> fitzprompt1 = makeVec();
    std::vector<double> fitrprompt1 = makeVec();

    std::vector<double> fitxprompt2 = makeVec();
    std::vector<double> fityprompt2 = makeVec();
    std::vector<double> fitzprompt2 = makeVec();
    std::vector<double> fitrprompt2 = makeVec();

    std::vector<double> fitxdelayed1 = makeVec();
    std::vector<double> fitydelayed1 = makeVec();
    std::vector<double> fitzdelayed1 = makeVec();
    std::vector<double> fitrdelayed1 = makeVec();

    std::vector<double> fitxdelayed2 = makeVec();
    std::vector<double> fitydelayed2 = makeVec();
    std::vector<double> fitzdelayed2 = makeVec();
    std::vector<double> fitrdelayed2 = makeVec();

    // 4. Directories (adjust if needed)
    std::string prompt1_dir  = "/data/snoplus3/parkerw/ratSimulations/Nov20_origSFPrompt";
    std::string delayed1_dir = "/data/snoplus3/parkerw/ratSimulations/Nov20_origSFDelay";
    std::string prompt2_dir  = "/data/snoplus3/parkerw/ratSimulations/Oct8_fastSFPrompt_copy";
    std::string delayed2_dir = "/data/snoplus3/parkerw/ratSimulations/Oct7_fastSFDelay_copy";

    // 5. List files in each directory
    std::vector<std::string> prompt1_files  = listRootFiles(prompt1_dir);
    std::vector<std::string> prompt2_files  = listRootFiles(prompt2_dir);
    std::vector<std::string> delayed1_files = listRootFiles(delayed1_dir);
    std::vector<std::string> delayed2_files = listRootFiles(delayed2_dir);

    // 6. Fill fits for each reprocessing
    fillFitsFromFiles(prompt1_files,  promptIndexByGTID,
                      fitxprompt1, fityprompt1, fitzprompt1, fitrprompt1,
                      "prompt1");

    fillFitsFromFiles(prompt2_files,  promptIndexByGTID,
                      fitxprompt2, fityprompt2, fitzprompt2, fitrprompt2,
                      "prompt2");

    fillFitsFromFiles(delayed1_files, delayedIndexByGTID,
                      fitxdelayed1, fitydelayed1, fitzdelayed1, fitrdelayed1,
                      "delayed1");

    fillFitsFromFiles(delayed2_files, delayedIndexByGTID,
                      fitxdelayed2, fitydelayed2, fitzdelayed2, fitrdelayed2,
                      "delayed2");

    // 7. Write output TTree
    TFile* fout = TFile::Open(outfilename.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cerr << "Error: could not create output file " << outfilename << std::endl;
        return 1;
    }

    TTree* tree = new TTree("T", "coincidences");

    // Scalars to bind branches
    long long runPrompt, runDelayed;
    long long gtidPrompt, gtidDelayed;

    double bxp1, bxp2, byp1, byp2, bzp1, bzp2, brp1, brp2;
    double bxd1, bxd2, byd1, byd2, bzd1, bzd2, brd1, brd2;

    tree->Branch("runPrompt",   &runPrompt);
    tree->Branch("gtidPrompt",  &gtidPrompt);
    tree->Branch("runDelayed",  &runDelayed);
    tree->Branch("gtidDelayed", &gtidDelayed);

    tree->Branch("fitxprompt1", &bxp1);
    tree->Branch("fitxprompt2", &bxp2);
    tree->Branch("fityprompt1", &byp1);
    tree->Branch("fityprompt2", &byp2);
    tree->Branch("fitzprompt1", &bzp1);
    tree->Branch("fitzprompt2", &bzp2);
    tree->Branch("fitrprompt1", &brp1);
    tree->Branch("fitrprompt2", &brp2);

    tree->Branch("fitxdelayed1", &bxd1);
    tree->Branch("fitxdelayed2", &bxd2);
    tree->Branch("fitydelayed1", &byd1);
    tree->Branch("fitydelayed2", &byd2);
    tree->Branch("fitzdelayed1", &bzd1);
    tree->Branch("fitzdelayed2", &bzd2);
    tree->Branch("fitrdelayed1", &brd1);
    tree->Branch("fitrdelayed2", &brd2);

    for (size_t i = 0; i < N; i++) {
        runPrompt   = promptEvents[i].run;
        gtidPrompt  = promptEvents[i].gtid;
        runDelayed  = delayedEvents[i].run;
        gtidDelayed = delayedEvents[i].gtid;

        bxp1 = fitxprompt1[i];
        bxp2 = fitxprompt2[i];
        byp1 = fityprompt1[i];
        byp2 = fityprompt2[i];
        bzp1 = fitzprompt1[i];
        bzp2 = fitzprompt2[i];
        brp1 = fitrprompt1[i];
        brp2 = fitrprompt2[i];

        bxd1 = fitxdelayed1[i];
        bxd2 = fitxdelayed2[i];
        byd1 = fitydelayed1[i];
        byd2 = fitydelayed2[i];
        bzd1 = fitzdelayed1[i];
        bzd2 = fitzdelayed2[i];
        brd1 = fitrdelayed1[i];
        brd2 = fitrdelayed2[i];

        tree->Fill();
    }

    fout->cd();
    tree->Write("", TObject::kOverwrite);
    fout->Write();
    fout->Close();

    std::cout << "Done. Wrote " << N << " entries to " << outfilename << std::endl;

    return 0;
}

// ROOT-friendly wrapper
void compare(const char* prompt, const char* delayed, const char* outfilename) {
    compare_reprocessing(prompt, delayed, outfilename);
}
