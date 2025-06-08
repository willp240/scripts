#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TStyle.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

void PlotLLHIters( ) {

  gStyle->SetOptStat(0);
  
  const char* filename = "log.txt";

  std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    int currentEvent = -1;
    int iter = 1;

    std::vector<TGraph*> llhGraphs;
    std::vector<TGraph*> posGraphs;
    std::vector<TGraph*> llhDiffGraphs;

    TGraph* gLLH = nullptr;
    TGraph* gPos = nullptr;
    TGraph* gLLHDiff = nullptr;

    while (std::getline(infile, line)) {
        if (line.find("CountProc::DSEvent") != std::string::npos) {
            currentEvent++;
            iter = 1;

            gLLH = new TGraph();
            gLLH->SetTitle(Form("Event %d", currentEvent));
            gLLH->SetLineColor((currentEvent % 9) + 1);
            llhGraphs.push_back(gLLH);

            gPos = new TGraph();
            gPos->SetTitle(Form("Event %d", currentEvent));
            gPos->SetLineColor((currentEvent % 9) + 1);
            posGraphs.push_back(gPos);

	    gLLHDiff = new TGraph();
            gLLHDiff->SetTitle(Form("Event %d", currentEvent));
            gLLHDiff->SetLineColor((currentEvent % 9) + 1);
            llhDiffGraphs.push_back(gLLHDiff);
        }

        else if (line.find("Seed: pos = ") != std::string::npos) {
            size_t posStart = line.find("pos = ");
            if (posStart != std::string::npos) {
                std::string posPart = line.substr(posStart + 6);
                size_t timePos = posPart.find("time =");
                if (timePos != std::string::npos)
                    posPart = posPart.substr(0, timePos);

                std::istringstream posStream(posPart);
                double x, y, z, pos4th;
                posStream >> x >> y >> z >> pos4th;
                gPos->SetPoint(0, 0, pos4th);
                //gLLH->SetPoint(0, 0, 0); // No LLH in seed line
            }
        }

        else if (line.find("Iteration") != std::string::npos) {
            double llh = 0, pos4 = 0;

            size_t llhPos = line.find("LLH = ");
            if (llhPos != std::string::npos) {
                size_t commaPos = line.find(",", llhPos);
                llh = std::stod(line.substr(llhPos + 6, commaPos - llhPos - 6));
            }

            size_t posStart = line.find("Pos = ");
            if (posStart != std::string::npos) {
                std::string posPart = line.substr(posStart + 6);
                std::replace(posPart.begin(), posPart.end(), ',', ' ');
                std::istringstream posStream(posPart);
                double x, y, z, pos4th;
                posStream >> x >> y >> z >> pos4th;
                pos4 = pos4th;
            }

            gLLH->SetPoint(iter-1, iter, llh);
            gPos->SetPoint(iter, iter, pos4);


	    if(iter > 1)
	      {

		double prevLLH = gLLH->GetPointY( iter-2 );
		double currLLH = gLLH->GetPointY( iter-1 );
		double change = (abs(currLLH-prevLLH)) / abs(prevLLH);
		gLLHDiff->SetPoint(iter-1, iter, change);
	      }
	    else if( iter == 1)
	      {
		gLLHDiff->SetPoint(iter-1, iter, 1.0);
	      }
	    
            ++iter;
        }
    }

    
    TCanvas* c1 = new TCanvas("c1", "LLH vs Iteration", 1200, 800);
    gPad->SetGrid(1);
    gPad->SetFrameLineWidth(2);
    c1->SetLeftMargin(0.12);
    
    TMultiGraph* mgLLH = new TMultiGraph();
    TLegend* leg1 = new TLegend(0.75, 0.55, 0.85, 0.8);
    for (size_t i = 0; i < llhGraphs.size(); ++i) {
        mgLLH->Add(llhGraphs[i]);
        leg1->AddEntry(llhGraphs[i], llhGraphs[i]->GetTitle(), "l");
    }
    mgLLH->SetTitle("LLH vs Iteration;Iteration;LLH");
    mgLLH->Draw("AL");
    mgLLH->GetYaxis()->SetTitleOffset(1.3);
    leg1->Draw();
    c1->SaveAs("llh_vs_iteration.pdf");

    TCanvas* c2 = new TCanvas("c2", "Radius vs Iteration", 1200, 800);
    gPad->SetGrid(1);
    gPad->SetFrameLineWidth(2);
    
    TMultiGraph* mgPos = new TMultiGraph();
    TLegend* leg2 = new TLegend(0.75, 0.55, 0.85, 0.8);
    for (size_t i = 0; i < posGraphs.size(); ++i) {
        mgPos->Add(posGraphs[i]);
        leg2->AddEntry(posGraphs[i], posGraphs[i]->GetTitle(), "l");
    }
    mgPos->SetTitle("Radius vs Iteration;Iteration;Current Best Radius, mm");
    mgPos->GetYaxis()->SetRangeUser(0, 350);
    mgPos->Draw("AL");
    leg2->Draw();
    c2->SaveAs("radius_vs_iteration.pdf");

    TCanvas* c3 = new TCanvas("c3", "#Delta LLH vs Iteration", 1200, 800);
    gPad->SetGrid(1);
    gPad->SetFrameLineWidth(2);
    gPad->SetLogy();
    
    TMultiGraph* mgLLHDiff = new TMultiGraph();
    TLegend* leg3 = new TLegend(0.75, 0.55, 0.85, 0.8);
    for (size_t i = 0; i < llhDiffGraphs.size(); ++i) {
        mgLLHDiff->Add(llhDiffGraphs[i]);
        leg3->AddEntry(llhDiffGraphs[i], llhDiffGraphs[i]->GetTitle(), "l");
    }
    mgLLHDiff->SetTitle("#Delta LLH vs Iteration;Iteration;Fractional Change in LLH");
    mgLLHDiff->GetYaxis()->SetRangeUser(1E-10, 1.1);
    mgLLHDiff->Draw("AL");
    leg3->Draw();
    c3->SaveAs("deltaLLH_vs_iteration.pdf");
}
