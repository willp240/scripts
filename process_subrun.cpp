#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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
// Read text file: each line "run,gtid"
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
// Extract run number from filename
// ------------------------------------------------------------
long long parseRunFromFilename(const std::string& filename) {
    size_t pos = filename.find("_r");
    if (pos == std::string::npos) {
        std::cerr << "Warning: could not find \"_r\" in filename " << filename << std::endl;
        return -1;
    }
    pos += 2;

    std::string digits;
    while (pos < filename.size() && std::isdigit(filename[pos])) {
        digits.push_back(filename[pos]);
        pos++;
    }

    if (digits.empty()) {
        std::cerr << "Warning: no digits after _r in filename " << filename << std::endl;
        return -1;
    }

    return atoll(digits.c_str());
}

// ------------------------------------------------------------
// List only files whose run appears in neededRuns
// ------------------------------------------------------------
std::vector<std::string> listOnlyNeededRootFiles(
        const std::string& dir,
        const std::unordered_set<long long>& neededRuns)
{
    std::vector<std::string> files;

    TSystemDirectory sysDir("sysDir", dir.c_str());
    TList* flist = sysDir.GetListOfFiles();
    if (!flist) return files;

    TIter next(flist);
    while (TObject* obj = next()) {
        TSystemFile* file = dynamic_cast<TSystemFile*>(obj);
        if (!file || file->IsDirectory()) continue;

        std::string fname = file->GetName();
        if (fname.size() < 5 || fname.substr(fname.size() - 5) != ".root")
            continue;

        size_t pos = fname.find("_r");
        if (pos == std::string::npos) continue;
        pos += 2;

        std::string digits;
        while (pos < fname.size() && std::isdigit(fname[pos])) {
            digits.push_back(fname[pos]);
            pos++;
        }
        if (digits.empty()) continue;

        long long run = atoll(digits.c_str());
        if (neededRuns.count(run)) {
            files.push_back(dir + "/" + fname);
        }
    }

    std::cout << "Directory " << dir
              << " → using " << files.size()
              << " files for " << neededRuns.size() << " delayed runs." << std::endl;

    return files;
}


// ------------------------------------------------------------
// Fill fit arrays for matching GTIDs inside a set of files
// ------------------------------------------------------------
void fillFitsFromFiles(const std::vector<std::string>& files,
                       const std::unordered_map<long long, size_t>& gtidToIndex,
                       std::vector<double>& fitx,
                       std::vector<double>& fity,
                       std::vector<double>& fitz,
                       std::vector<double>& fitr,
                       const std::string& label)
{
    size_t matched = 0;

    for (const auto& filename : files) {
        std::cout << "[" << label << "] Opening file " << filename << std::endl;

        RAT::DU::DSReader ds(filename);

        for (size_t iEntry = 0; iEntry < ds.GetEntryCount(); iEntry++) {
            RAT::DS::Entry entry;
            try { entry = ds.GetEntry(iEntry); }
            catch (...) { continue; }

            for (size_t iEv = 0; iEv < entry.GetEVCount(); iEv++) {
                const RAT::DS::EV& ev = entry.GetEV(iEv);
                long long gtid = ev.GetGTID();

                auto it = gtidToIndex.find(gtid);
                if (it == gtidToIndex.end())
                    continue;

                size_t idx = it->second;

                RAT::DS::FitVertex vtx;
                try {
                    vtx = ev.GetFitResult("scintFitter").GetVertex(0);
                }
                catch (...) { continue; }

                if (!vtx.ValidPosition() || !vtx.ValidTime())
                    continue;

                fitx[idx] = vtx.GetPosition().X();
                fity[idx] = vtx.GetPosition().Y();
                fitz[idx] = vtx.GetPosition().Z();
                fitr[idx] = vtx.GetPosition().Mag();

                matched++;
            }
        }
    }

    std::cout << "[" << label << "] Matched " << matched << " events.\n";
}



// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main(int argc, char** argv)
{
    if (argc != 8) {
        std::cerr << "Usage: " << argv[0]
                  << " <prompt1> <prompt2>"
                  << " <promptList.txt> <delayedList.txt>"
                  << " <delayed1_dir> <delayed2_dir>"
                  << " <output.root>"
                  << std::endl;
        return 1;
    }

    std::string prompt1File = argv[1];
    std::string prompt2File = argv[2];
    std::string promptList  = argv[3];
    std::string delayedList = argv[4];
    std::string delayed1_dir = argv[5];
    std::string delayed2_dir = argv[6];
    std::string outfilename = argv[7];

    std::cout << "prompt1: " << prompt1File << "\n"
              << "prompt2: " << prompt2File << "\n"
              << "lists:   " << promptList << " and " << delayedList << "\n"
              << "delayed dirs: " << delayed1_dir << "  " << delayed2_dir << "\n";

    // 1. Load event lists
    auto promptEvents  = readEventList(promptList);
    auto delayedEvents = readEventList(delayedList);

    if (promptEvents.size() != delayedEvents.size()) {
        std::cerr << "WARNING: prompt/delayed list sizes differ\n";
    }

    size_t N = std::min(promptEvents.size(), delayedEvents.size());
    if (N == 0) return 1;

    // 2. Which run does this job correspond to?
    long long thisRun = parseRunFromFilename(prompt1File);
    if (thisRun < 0) return 1;

    std::cout << "This job is for run " << thisRun << "\n";

    // 3. Build global GTID → index map
    std::unordered_map<long long, size_t> promptIndexByGTID;
    promptIndexByGTID.reserve(N * 2);

    for (size_t i = 0; i < N; i++) {
        promptIndexByGTID[promptEvents[i].gtid] = i;
    }

    // 4. Identify GTIDs actually present in this subrun (prompt1)
    std::unordered_set<long long> promptGTIDsInThisFile;
    {
        std::cout << "Scanning prompt1 file for GTIDs...\n";
        RAT::DU::DSReader ds(prompt1File);

        for (size_t iEntry = 0; iEntry < ds.GetEntryCount(); iEntry++) {
            RAT::DS::Entry entry;
            try { entry = ds.GetEntry(iEntry); }
            catch (...) { continue; }

            for (size_t iEv = 0; iEv < entry.GetEVCount(); iEv++) {
                promptGTIDsInThisFile.insert(entry.GetEV(iEv).GetGTID());
            }
        }
    }

    // 5. indicesThisRun = only those coincidences whose prompt GTID is in this file
    std::vector<size_t> indicesThisRun;
    indicesThisRun.reserve(promptGTIDsInThisFile.size());

    for (long long gtid : promptGTIDsInThisFile) {
        auto it = promptIndexByGTID.find(gtid);
        if (it != promptIndexByGTID.end()) {
            indicesThisRun.push_back(it->second);
        }
    }

    std::cout << "This prompt subrun contains "
              << indicesThisRun.size() << " coincidences.\n";

    if (indicesThisRun.empty()) {
        std::cout << "Nothing to do for this subrun.\n";
        return 0;
    }

    // 6. Determine delayed runs used by these indices
    std::unordered_set<long long> delayedRunsThisJob;
    std::unordered_map<long long, size_t> delayedIndexByGTID_Job;
    delayedRunsThisJob.reserve(indicesThisRun.size());
    delayedIndexByGTID_Job.reserve(indicesThisRun.size());

    for (size_t idx : indicesThisRun) {
        long long gr = delayedEvents[idx].run;
        long long gg = delayedEvents[idx].gtid;
        delayedRunsThisJob.insert(gr);
        delayedIndexByGTID_Job[gg] = idx;
    }

    std::cout << "This job needs delayed info for "
              << delayedRunsThisJob.size() << " runs.\n";

    // 7. Allocate arrays (size N)
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

    // 8. Fill prompt1 and prompt2 fits (only one file each)
    fillFitsFromFiles({prompt1File}, promptIndexByGTID,
                      fitxprompt1, fityprompt1, fitzprompt1, fitrprompt1,
                      "prompt1");

    fillFitsFromFiles({prompt2File}, promptIndexByGTID,
                      fitxprompt2, fityprompt2, fitzprompt2, fitrprompt2,
                      "prompt2");

    // 9. Fill delayed fits
    auto delayed1_files = listOnlyNeededRootFiles(delayed1_dir, delayedRunsThisJob);
    auto delayed2_files = listOnlyNeededRootFiles(delayed2_dir, delayedRunsThisJob);

    fillFitsFromFiles(delayed1_files, delayedIndexByGTID_Job,
                      fitxdelayed1, fitydelayed1, fitzdelayed1, fitrdelayed1,
                      "delayed1");

    fillFitsFromFiles(delayed2_files, delayedIndexByGTID_Job,
                      fitxdelayed2, fitydelayed2, fitzdelayed2, fitrdelayed2,
                      "delayed2");

    // 10. Write output ROOT file (only entries for this subrun)
    TFile* fout = TFile::Open(outfilename.c_str(), "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cerr << "Error creating " << outfilename << std::endl;
        return 1;
    }

    TTree* tree = new TTree("T", "coincidences for this subrun");

    long long rp, gp, rd, gd;
    double xp1, xp2, yp1, yp2, zp1, zp2, rp1, rp2;
    double xd1, xd2, yd1, yd2, zd1, zd2, rd1, rd2;

    tree->Branch("runPrompt", &rp);
    tree->Branch("gtidPrompt", &gp);
    tree->Branch("runDelayed", &rd);
    tree->Branch("gtidDelayed", &gd);

    tree->Branch("fitxprompt1", &xp1);
    tree->Branch("fitxprompt2", &xp2);
    tree->Branch("fityprompt1", &yp1);
    tree->Branch("fityprompt2", &yp2);
    tree->Branch("fitzprompt1", &zp1);
    tree->Branch("fitzprompt2", &zp2);
    tree->Branch("fitrprompt1", &rp1);
    tree->Branch("fitrprompt2", &rp2);

    tree->Branch("fitxdelayed1", &xd1);
    tree->Branch("fitxdelayed2", &xd2);
    tree->Branch("fitydelayed1", &yd1);
    tree->Branch("fitydelayed2", &yd2);
    tree->Branch("fitzdelayed1", &zd1);
    tree->Branch("fitzdelayed2", &zd2);
    tree->Branch("fitrdelayed1", &rd1);
    tree->Branch("fitrdelayed2", &rd2);

    size_t filled = 0;

    for (size_t idx : indicesThisRun) {
        rp = promptEvents[idx].run;
        gp = promptEvents[idx].gtid;
        rd = delayedEvents[idx].run;
        gd = delayedEvents[idx].gtid;

        xp1 = fitxprompt1[idx];
        xp2 = fitxprompt2[idx];
        yp1 = fityprompt1[idx];
        yp2 = fityprompt2[idx];
        zp1 = fitzprompt1[idx];
        zp2 = fitzprompt2[idx];
        rp1 = fitrprompt1[idx];
        rp2 = fitrprompt2[idx];

        xd1 = fitxdelayed1[idx];
        xd2 = fitxdelayed2[idx];
        yd1 = fitydelayed1[idx];
        yd2 = fitydelayed2[idx];
        zd1 = fitzdelayed1[idx];
        zd2 = fitzdelayed2[idx];
        rd1 = fitrdelayed1[idx];
        rd2 = fitrdelayed2[idx];

        tree->Fill();
        filled++;
    }

    fout->cd();
    tree->Write("", TObject::kOverwrite);
    fout->Close();

    std::cout << "Finished. Wrote " << filled << " entries to "
              << outfilename << std::endl;

    return 0;
}
