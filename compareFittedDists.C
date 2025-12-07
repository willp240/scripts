// ROOT Headers
#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>

// c++ Headers
#include <sys/stat.h>

/* ///////////////////////////////////////////////////////////////////
///
/// Hacked script to compare fitted distributions from individual
/// fits. i.e fits not necessarily from full 500 fixed osc fits.
///
/////////////////////////////////////////////////////////////////// */

// Configurable parameter sets
const std::string datasetname1 = "2p2ppo";

const std::string datasetname2 = "bismsb";

// Axis ranges
double xmin = 0.9;
double xmax = 8.0;

// Depeding on your 'datasetChoice', the yrange is either the 0 (both datasets), 1 (dataset1/ppo), or 2 (dataset2/bismsb) choice of these ranges
double ymin;
double ymax;
double ymin0 = 0.0;
double ymax0 = 7.0;
double ymin1 = 0.0;
double ymax1 = 2.5;
double ymin2 = 0.0;
double ymax2 = 4.5;
double ymin0dat = 0.0;
double ymax0dat = 50.0;
double ymin1dat = 0.0;
double ymax1dat = 15.0;//20.0;
double ymin2dat = 0.0;
double ymax2dat = 35.0;

// If you are plotting real data, what bin width to use for data
double databinwidth = 0.3;

TH1D* rebinData(std::string dataDir, const int datasetChoice)
{
    std::cout << "Rebinning data from " << dataDir << std::endl;

    // Open the file
    std::string filename1 = dataDir + "/data_" + datasetname1 + ".root";
    TFile *file1 = TFile::Open(filename1.c_str(), "READ");
    if (!file1 || file1->IsZombie())
    {
        std::cerr << "Error opening file: " << filename1 << std::endl;
        throw;
    }

    // Get the TNtuple (replace "ntuple" with your actual name if different)
    TNtuple *nt1 = (TNtuple *)file1->Get("pruned");
    if (!nt1)
    {
        std::cerr << "Could not find TNtuple 'pruned' in file " << filename1 << std::endl;
        file1->Close();
        throw;
    }

    // Open the second file
    std::string filename2 = dataDir + "/data_" + datasetname2 + ".root";
    TFile *file2 = TFile::Open(filename2.c_str(), "READ");
    if (!file2 || file2->IsZombie())
    {
        std::cerr << "Error opening file: " << filename2 << std::endl;
        throw;
    }

    // Get the TNtuple (replace "ntuple" with your actual name if different)
    TNtuple *nt2 = (TNtuple *)file2->Get("pruned");
    if (!nt2)
    {
        std::cerr << "Could not find TNtuple 'pruned' in file " << filename2 << std::endl;
        file2->Close();
        throw;
    }

    //    Int_t nBins = static_cast<Int_t>((xmax - xmin) / databinwidth);
    double eps = 1e-12;
    int nBins = static_cast<int>(std::ceil((xmax - xmin)/databinwidth - eps));
    double xmax_new = xmin + nBins * databinwidth;
    
    TH1D *h1 = new TH1D("hEnergy", "Energy distribution;Energy (MeV);Events",
                           nBins, xmin, xmax_new);
    h1->SetLineWidth(2);
    h1->SetLineColor(kBlack);

    float energy = 0;

    if (datasetChoice == 0 || datasetChoice == 1)
    {

        nt1->SetBranchAddress("energy", &energy);

        // Loop over entries
        Long64_t nEntries = nt1->GetEntries();
        for (Long64_t i = 0; i < nEntries; ++i)
        {
            nt1->GetEntry(i);
            h1->Fill(energy);
        }
    }
    if (datasetChoice == 0 || datasetChoice == 2)
    {

        nt2->SetBranchAddress("energy", &energy);

        // Loop over entries
        Long64_t nEntries = nt2->GetEntries();
        for (Long64_t i = 0; i < nEntries; ++i)
        {
            nt2->GetEntry(i);
            h1->Fill(energy);
        }
    }
    return h1;
}

std::string RemoveDSName(const std::string& input) {
    std::string result = input;
    // Remove "PPO" or "bisMSB" with optional leading/trailing spaces
    result = std::regex_replace(result, std::regex("\\s*(PPO|bisMSB)\\s*"), " ");
    // Trim any extra whitespace that might be left at the ends
    result = std::regex_replace(result, std::regex("^\\s+|\\s+$"), "");
    return result;
}

void compareFittedDists(const char *dir1 = "", const char *dir2 = "", const int datasetChoice = 1, std::string dataDir = "")
{

    gROOT->SetStyle("snoplus");
    std::string outsuffix = "";

    std::vector<std::string> dsname;

    if (datasetChoice == 0)
    {
        dsname.push_back(datasetname1);
        dsname.push_back(datasetname2);
        outsuffix = "_all";
        if(dataDir == "")
        {
            ymin = ymin0;
            ymax = ymax0;
        }
        else{
            ymin = ymin0dat;
            ymax = ymax0dat;
        }
    }
    else if (datasetChoice == 1)
    {
        dsname.push_back(datasetname1);
        outsuffix = "_" + datasetname1;
        if(dataDir == "")
        {
            ymin = ymin1;
            ymax = ymax1;
        }
        else{
            ymin = ymin1dat;
            ymax = ymax1dat;
        }
    }
    else if (datasetChoice == 2)
    {
        dsname.push_back(datasetname2);
        outsuffix = "_" + datasetname2;
        if(dataDir == "")
        {
            ymin = ymin2;
            ymax = ymax2;
        }
        else{
            ymin = ymin2dat;
            ymax = ymax2dat;
        }
    }

    // Declare group histos and stacks
    TH1D *hData;
    TH1D *hMC1;
    TH1D *hMC1Clone;
    TH1D *hMC2;
    TH1D *hMC2Clone;
    hData = rebinData(dataDir, datasetChoice);

    // Directories
    std::ostringstream directory1;
    directory1 << dir1;
    std::ostringstream directory2;
    directory2 << dir2;

    TLegend *t1 = new TLegend(0.5, 0.48, 0.85, 0.85);
    t1->SetLineWidth(2);

    // Add files from the postfit_dists directory
    for (int iDS = 0; iDS < dsname.size(); iDS++)
    {

        std::string filePath1 = directory1.str() + "/asimov_" + dsname.at(iDS) + ".root";
        std::string filePath2 = directory2.str() + "/asimov_" + dsname.at(iDS) + ".root";

        TFile *file1 = TFile::Open(filePath1.c_str(), "READ");
        if (!file1 || file1->IsZombie())
        {
            std::cerr << "Error opening file: " << filePath1 << std::endl;
            delete file1;
            continue;
        }

        TFile *file2 = TFile::Open(filePath2.c_str(), "READ");
        if (!file2 || file2->IsZombie())
        {
            std::cerr << "Error opening file: " << filePath2 << std::endl;
            delete file2;
            continue;
        }

        // Get histogram
        TH1D *h1 = nullptr;
        file1->GetObject("asimov", h1);
        for (int i = 1; i <= h1->GetNbinsX(); ++i)
        {
            double binWidth = h1->GetBinWidth(i);
            double content = h1->GetBinContent(i);
            double error = h1->GetBinError(i);

            double scale = databinwidth / binWidth;
            h1->SetBinContent(i, content * scale);
            h1->SetBinError(i, error * scale);
        }

        // Get histogram
        TH1D *h2 = nullptr;
        file2->GetObject("asimov", h2);
        for (int i = 1; i <= h2->GetNbinsX(); ++i)
        {
            double binWidth = h2->GetBinWidth(i);
            double content = h2->GetBinContent(i);
            double error = h2->GetBinError(i);

            double scale = databinwidth / binWidth;
            h2->SetBinContent(i, content * scale);
            h2->SetBinError(i, error * scale);
        }

        if (iDS == 0)
        {
            hMC1 = h1;
            hMC2 = h2;
        }
        else
        {
            hMC1->Add(h1);
            hMC2->Add(h2);
        }

    }

    // Intialise group histos
    // Assumes we have a data and postfitdist (full fitted MC) but let's assume we always want to plot these
    hData->SetMarkerStyle(21);
    hMC1Clone = (TH1D *)hMC1->Clone("hMC1Clone");
    hMC1Clone->Reset("");

    for(int ibin = 1; ibin < hMC1->GetXaxis()->GetNbins(); ibin++)
    {
        double val1 = hMC1->GetBinContent(ibin);
        double val2 = hMC2->GetBinContent(ibin);
        hMC1Clone->SetBinContent(ibin, (val2-val1)/val1);
    }

    hData->SetLineColor(kBlack);
    hData->SetLineWidth(2);
    hMC1->SetLineColor(kBlue);
    hMC1->SetLineWidth(2);
    hMC2->SetLineColor(kRed);
    hMC2->SetLineWidth(2);

    t1->AddEntry(hData, "Data", "lep");
    //t1->AddEntry(hMC1, "James", "l");
    //t1->AddEntry(hMC2, "OXO", "l");
    t1->AddEntry(hMC1, "#it{a_{PPO}} =  0.00", "l");
    t1->AddEntry(hMC2, "#it{a_{PPO}} = -0.67", "l");

    // Draw stack of all event types
    TCanvas *c1 = new TCanvas("c1", "c1", 1000, 600);
    gStyle->SetOptStat(0);
    c1->SetFrameLineWidth(2);
    double uppermin = 0.3;
    TPad *lower = new TPad("lower", "pad", 0, 0, 1, uppermin);
    TPad *upper = new TPad("upper", "pad", 0, uppermin, 1, 1);
    upper->SetBottomMargin(0.04);
    lower->SetTopMargin(0.06);
    lower->SetBottomMargin(0.4);
    upper->SetFrameLineWidth(2);
    lower->SetFrameLineWidth(2);
    upper->Draw();
    lower->Draw();
    lower->SetGrid(1);
    c1->cd();

    upper->cd();
    TH1F *frame = upper->DrawFrame(xmin, ymin, xmax, ymax);
    frame->SetTitle(";Reconstructed Energy, MeV;Events");
    frame->GetXaxis()->SetLabelOffset(1.2);
    frame->GetXaxis()->SetTitleFont(42);
    frame->GetYaxis()->SetTitleFont(42);
    frame->GetXaxis()->SetLabelFont(42);
    frame->GetYaxis()->SetLabelFont(42);
    frame->GetXaxis()->SetLabelSize(0.06);
    frame->GetYaxis()->SetLabelSize(0.06);
    frame->GetXaxis()->SetTitleSize(0.06);
    frame->GetYaxis()->SetTitleSize(0.06);
    frame->GetYaxis()->SetTitleOffset(0.8);

    hMC1->Draw("histsame");
    hMC2->Draw("histsame");
    hData->SetFillStyle(0);
    hData->Draw("e1same");

    hMC1->GetXaxis()->SetLabelOffset(1.2);
    hMC1->GetXaxis()->SetTitleFont(42);
    hMC1->GetYaxis()->SetTitleFont(42);
    hMC1->GetXaxis()->SetLabelFont(42);
    hMC1->GetYaxis()->SetLabelFont(42);
    hMC1->GetXaxis()->SetLabelSize(0.06);
    hMC1->GetYaxis()->SetLabelSize(0.06);
    hMC1->GetXaxis()->SetTitleSize(0.06);
    hMC1->GetYaxis()->SetTitleSize(0.06);
    hMC1->GetYaxis()->SetTitleOffset(0.8);
    t1->SetTextFont(42);
    t1->Draw();
    upper->Modified();
    upper->Update();
    c1->Update();

    lower->cd();

    hMC1Clone->SetFillStyle(0);
    //hMC1Clone->GetYaxis()->SetRangeUser(-0.25, 0.25);
    hMC1Clone->GetYaxis()->SetRangeUser(-0.5, 0.5);
    hMC1Clone->GetXaxis()->SetRangeUser(xmin, xmax);
    hMC1Clone->GetXaxis()->SetTitleFont(42);
    hMC1Clone->GetYaxis()->SetTitleFont(42);
    hMC1Clone->GetXaxis()->SetLabelFont(42);
    hMC1Clone->GetYaxis()->SetLabelFont(42);
    hMC1Clone->GetXaxis()->SetLabelSize(0.14);
    hMC1Clone->GetYaxis()->SetLabelSize(0.14);
    hMC1Clone->GetXaxis()->SetTitleSize(0.14);
    hMC1Clone->GetYaxis()->SetTitleSize(0.14);
    hMC1Clone->GetYaxis()->SetTitle("Frac. Diff.");
    hMC1Clone->GetYaxis()->SetTitleOffset(0.33);
    hMC1Clone->GetYaxis()->SetNdivisions(404);
    hMC1Clone->GetYaxis()->SetTickLength(0.05);
    hMC1Clone->GetXaxis()->SetTickLength(0.07);
    hMC1Clone->GetYaxis()->ChangeLabel(2, -1, -1, -1, -1, -1, " ");
    hMC1Clone->GetYaxis()->ChangeLabel(4, -1, -1, -1, -1, -1, " ");
    hMC1Clone->SetLineWidth(2);
    hMC1Clone->Draw("hist");
    c1->Update();

    struct stat st = {0};
    std::filesystem::path pathObj(dir1);
    pathObj.replace_filename("plots/");
    if (stat(pathObj.string().c_str(), &st) == -1)
        mkdir(pathObj.string().c_str(), 0700);
    pathObj.replace_filename(("asimovcomp" + outsuffix + ".pdf").c_str());
    c1->SaveAs(pathObj.string().c_str());
    pathObj.replace_filename(("asimovcomp" + outsuffix + ".root").c_str());
    c1->SaveAs(pathObj.string().c_str());

}