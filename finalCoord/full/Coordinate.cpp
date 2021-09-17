#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/LightPathCalculator.hh>
#include <RAT/LightPathStraightScint.hh>
#include <RAT/DU/GroupVelocity.hh>
#include <RAT/DU/EffectiveVelocity.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/MCEV.hh>
#include <RAT/DS/MCParticle.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/Version.hh>


using namespace RAT;

void FillFullTimeResidualsPlot(std::string material, std::vector <TH1D*> histogram, std::string coordinate, std::vector<double> boundaries, int numPDFs, double velocity=-999);

void GetFullPDF(std::string material, unsigned int numberOfRuns, double velocity=-999, std::string coordinate = "r", std::string boundaries = "" )
{

  const float binWidth = 1.0;      // ns - width of bins in PDF
  const float lowLim = -100.0;     // ns - lower end of PDF
  const float upLim  =  300.0;     // ns - upper end of PDF
  const float lowLimFlat = -6.0;   // ns - time below which PDF is extrapolated flat
  const float lowLimVal  = -90.0;  // ns - time of lower value used in extrapolation
  const float upLimFlat = 234.0;   // ns - time above which PDF is extrapolated flat

  const unsigned int numBins = (upLim-lowLim)/binWidth + 1;
  const unsigned int lowBinFlat = (lowLimFlat-lowLim)/binWidth + 1;
  const unsigned int lowBinVal  = (lowLimVal -lowLim)/binWidth + 1;
  const unsigned int upBinFlat  = (upLimFlat -lowLim)/binWidth + 1;

  std::size_t pos = 0;
  std::vector< double > bounds;
  bounds.push_back(-40000);
  double r = 0.0;

  // convert ',' to ' '
  while (pos < boundaries.size ())
    if ((pos = boundaries.find_first_of (',',pos)) != std::string::npos)
      boundaries[pos] = ' ';

  std::cout << boundaries << std::endl;
  std::stringstream ss(boundaries);
  
  while(ss >> r)
    bounds.push_back(r);

  bounds.push_back(40000);
  
  std::string coordname;
  if(coordinate == "r")
    coordname = "vertex_r_range";
  else if(coordinate== "d")
    coordname ="distance_range";
  else if(coordinate== "z")
    coordname ="pmt_z_range";

  int numPDFs = bounds.size()-1;

  std::vector <TH1D*> histogram1;
  std::vector <TH1D*> histogram2;

  for(int ihist=0; ihist<numPDFs; ihist++){
    
    TString hname1 = Form("histogram1%d", ihist);
    TString hname2 = Form("histogram2%d", ihist);

    TH1D* h1 = new TH1D(hname1, "", numBins, lowLim-binWidth/2., upLim+binWidth/2.);
    TH1D* h2 = new TH1D(hname2, "", numBins, lowLim-binWidth/2., upLim+binWidth/2.);

    histogram1.push_back(h1);
    histogram2.push_back(h2);
  }

  std::cout << "running with velocity " << velocity << endl;

  for(unsigned int ihist = 0; ihist < numberOfRuns; ihist++)
    {
      std::stringstream fileNameStream;
      fileNameStream << material + "_DataForPDF_part" << (ihist + 1) << ".root";
      FillFullTimeResidualsPlot(fileNameStream.str(), histogram1, coordinate, bounds, numPDFs, velocity);
    }
  
  for(int ihist=0; ihist < numPDFs; ihist++) {

    //    histogram1.at(ihist)->Scale(100000/histogram1.at(ihist)->Integral());

    // Print coordinated values to screen in RATDB format
    std::cout << std::endl;
    std::cout << "Please place the text below into the database file: ET1D_MultiPDF.ratdb located in rat/data, replacing any existing entry with the same index." << std::endl;
    std::cout << std::endl;
    std::cout << "{" << std::endl;
    std::cout << "type : \"ET1D_MultiPDF\"," << std::endl;
    std::cout << "version: 1," << std::endl;
    std::cout << "index: \"" << material << "_" << numPDFs << "PDFNum" << ihist << "\"," << std::endl;
    std::cout <<  coordname << ":  [" << bounds.at(ihist) << ", " << bounds.at(ihist+1) << " ]," << std::endl;
    std::cout << "run_range: [0, 0]," << std::endl;
    std::cout << "pass: 0," << std::endl;
    std::cout << "production: false," << std::endl;
    std::cout << "timestamp: \"\"," << std::endl;
    std::cout << "comment: \"Generated with RAT " << RAT::GetRATVersion() << "\"," << std::endl;
    
    std::cout << "time: [";
    for(unsigned int j = 1; j <= numBins; j++)
      {
	std::cout << std::fixed << std::setprecision(2)<< float(histogram1.at(ihist)->GetBinCenter(j)) << ", ";
      }
    std::cout << "]," << std::endl;
    
    std::cout << "probability: [";
    for(unsigned int j = 1; j <= numBins; j++)
      {
    // Approximate early hits to a flat distribution
    if (j < lowBinFlat)
    {
      histogram2.at(ihist)->Fill(histogram1.at(ihist)->GetBinCenter(j), histogram1.at(ihist)->GetBinContent(lowBinVal));
      std::cout << std::fixed << std::setprecision(2)<< float(histogram1.at(ihist)->GetBinContent(lowBinVal)) << ", ";
    }
    // Approximate late hits to a flat distribution
    else if (j > upBinFlat)
    {
      histogram2.at(ihist)->Fill(histogram1.at(ihist)->GetBinCenter(j), histogram1.at(ihist)->GetBinContent(upBinFlat));
      std::cout << std::fixed << std::setprecision(2)<< float(histogram1.at(ihist)->GetBinContent(upBinFlat)) << ", ";
    }
    else
    {
      histogram2[ihist]->Fill(histogram1[ihist]->GetBinCenter(j), histogram1[ihist]->GetBinContent(j));
      std::cout << std::fixed << std::setprecision(2)<< float(histogram1[ihist]->GetBinContent(j)) << ", ";
    }
  }
  std::cout << "]," << endl;  
  
  std::cout << "}" << std::endl;
  std::cout << std::endl;
  
  // histogram1 shows MC data in black
  // histogram2 shows PDF (including approximations) in red
  histogram1.at(ihist)->Draw();
  histogram1.at(ihist)->GetXaxis()->SetTitle("Time Residual, ns");
  histogram2.at(ihist)->SetLineColor(2);
  histogram2.at(ihist)->Draw("SAMES");
  }
  std::stringstream outname;
  outname << "MultiPDF_Plots.root";
  TFile *outfile = TFile::Open(outname.str().c_str(),"RECREATE");
  for(int ihist=0; ihist<numPDFs; ihist++){
    TString hname1 = Form("MC_%d",ihist);
    TString hname2 = Form("PDF_%d",ihist);
    histogram1.at(ihist)->Write(hname1);
    histogram2.at(ihist)->Write(hname2);
  }
}

// Update the Scintillator Time Residual distribution from a single rootfile
void FillFullTimeResidualsPlot(std::string infile, std::vector <TH1D*> histogram, std::string coordinate, std::vector<double> boundaries, int numPDFs, double velocity)
{
  RAT::DU::DSReader dsReader(infile);
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator();
  const RAT::DU::EffectiveVelocity& effectiveVelocity = RAT::DU::Utility::Get()->GetEffectiveVelocity();
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();

  DB *db = DB::Get();

  for(size_t i = 0; i < dsReader.GetEntryCount(); i++) 
  {
    const RAT::DS::Entry& dsEntry = dsReader.GetEntry(i);
    if(dsEntry.GetEVCount() == 0) continue;

    // Get MC info
    const RAT::DS::MC& mcEvent = dsEntry.GetMC();
    const RAT::DS::MCParticle& mcParticle = mcEvent.GetMCParticle(0);
    const TVector3 mcPosition = mcParticle.GetPosition();
    if(mcPosition.Z()>6000) continue;

    // Get triggered PMT information
    const RAT::DS::EV& triggeredEvent = dsEntry.GetEV(0);
    const RAT::DS::CalPMTs& calibratedPMTs = triggeredEvent.GetCalPMTs();
    // Get event time
    const double eventTime = 390 - dsEntry.GetMCEV(0).GetGTTime();

    // Loop over each triggered PMT and get the time residual
    for(size_t j = 0; j < calibratedPMTs.GetCount(); j++)
    {
      const RAT::DS::PMTCal& pmt = calibratedPMTs.GetPMT(j);
      const TVector3 pmtPos = pmtInfo.GetPosition( pmt.GetID() );

      double distInAV = 0.0;
      double distInWater = 0.0;
      double distInTarget = 0.0;
      double distInInnerAV = 0.0;

      //      GetPath(pmtPos, mcPosition, distInTarget, distInWater);
      RAT::LP::LightPathStraightScint::GetPath(pmtPos, mcPosition, distInTarget, distInWater);      

      //double transitTime = effectiveVelocity.CalcByDistance( distInTarget, distInAV, distInWater );
      float transitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );

      if(velocity>0)
        // We are overriding the velocity in EffectiveVelocity for faster processing
        transitTime = ( distInInnerAV / velocity + distInAV / effectiveVelocity.GetAVVelocity() +
                        distInWater / effectiveVelocity.GetWaterVelocity() ) + effectiveVelocity.GetOffset();
      const double timeResid = pmt.GetTime() - transitTime - eventTime;
     
      double dist = (pmtPos-mcPosition).Mag();

      double coord = 0; //coordinate which the PDFs are split in
      if(coordinate == "r")
	coord = mcPosition.Mag();
      else if(coordinate == "d")
        coord = dist;
      else if(coordinate == "z")
	coord = pmtPos.Z();

      for(int ipdf = 0; ipdf < numPDFs; ipdf++){
	if( coord > boundaries.at(ipdf) && coord < boundaries.at(ipdf+1) )
	  histogram.at(ipdf)->Fill(timeResid);
      }
      DB::Get()->Clear();
    }
  }
}
