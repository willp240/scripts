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
#include <RAT/DU/GroupVelocity.hh>
#include <RAT/DU/EffectiveVelocity.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/MCEV.hh>
#include <RAT/DS/MCParticle.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/Version.hh>

void FillPartialTimeResidualsPlot(std::string material, TH1D* histogram, double velocity=-999, int pmtBand=0);


void GetPartialPDF(std::string filePathName, std::string material, unsigned int numberOfRuns, double velocity=-999, int pmtBand=0, int numPDFs=0)
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

  TH1D* histogram1 = new TH1D("histogram1", "", numBins, lowLim-binWidth/2., upLim+binWidth/2.);
  TH1D* histogram2 = new TH1D("histogram2", "", numBins, lowLim-binWidth/2., upLim+binWidth/2.);

  std::cout << "running with velocity " << velocity << endl;

  for(unsigned int i = 0; i < numberOfRuns; i++)
    {
      std::stringstream fileNameStream;
      fileNameStream << filePathName + "_" << i << ".root";
      FillPartialTimeResidualsPlot(fileNameStream.str(), histogram1, velocity, pmtBand);
    }

  // Print coordinated values to screen in RATDB format
  std::cout << std::endl;
  std::cout << "Please place the text below into the database file: ET1D.ratdb located in rat/data, replacing any existing entry with the same index." << std::endl;
  std::cout << std::endl;
  std::cout << "{" << std::endl;
  std::cout << "type : \"ET1D_Partial_MultiPDF\"," << std::endl;
  std::cout << "version: 1," << std::endl;
  std::cout << "index: \"" << material << "_" << numPDFs << "PDFBand" << pmtBand << " \"," << std::endl;
  std::cout << "run_range: [0, 0]," << std::endl;
  std::cout << "pass: 0," << std::endl;
  std::cout << "production: false," << std::endl;
  std::cout << "timestamp: \"\"," << std::endl;
  std::cout << "comment: \"Generated with RAT " << RAT::GetRATVersion() << "\"," << std::endl;
	
  std::cout << "time: [";
  for(unsigned int j = 1; j <= numBins; j++)
  {
    std::cout << std::fixed << std::setprecision(2)<< float(histogram1->GetBinCenter(j)) << ", ";
  }
  std::cout << "]," << std::endl;
  
  std::cout << "probability: [";
  for(unsigned int j = 1; j <= numBins; j++)
  {
    // Approximate early hits to a flat distribution
    if (j < lowBinFlat)
    {
      histogram2->Fill(histogram1->GetBinCenter(j), histogram1->GetBinContent(lowBinVal));
      std::cout << std::fixed << std::setprecision(2)<< float(histogram1->GetBinContent(lowBinVal)) << ", ";
    }
    // Approximate late hits to a flat distribution
    else if (j > upBinFlat)
    {
      histogram2->Fill(histogram1->GetBinCenter(j), histogram1->GetBinContent(upBinFlat));
      std::cout << std::fixed << std::setprecision(2)<< float(histogram1->GetBinContent(upBinFlat)) << ", ";
    }
    else
    {
      histogram2->Fill(histogram1->GetBinCenter(j), histogram1->GetBinContent(j));
      std::cout << std::fixed << std::setprecision(2)<< float(histogram1->GetBinContent(j)) << ", ";
    }
  }
  std::cout << "]," << endl;  
  
  std::cout << "}" << std::endl;
  std::cout << std::endl;
  
  // histogram1 shows MC data in black
  // histogram2 shows PDF (including approximations) in red
  histogram1->Draw();
  histogram1->GetXaxis()->SetTitle("Time Residual, ns");
  histogram2->SetLineColor(2);
  histogram2->Draw("SAMES");

  std::stringstream outname;
  outname << "Dec15Scint_" << numPDFs << "PDFBand" << pmtBand << ".root";
  TFile *outfile = TFile::Open(outname.str().c_str(),"RECREATE");
  histogram1->Write("MC");
  histogram2->Write("PDF");
}

// Update the Scintillator Time Residual distribution from a single rootfile
void FillPartialTimeResidualsPlot(std::string infile, TH1D* histogram, double velocity, int pmtBand)
{
  RAT::DU::DSReader dsReader(infile);
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator();
  const RAT::DU::EffectiveVelocity& effectiveVelocity = RAT::DU::Utility::Get()->GetEffectiveVelocity();
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();

  for(size_t i = 0; i < dsReader.GetEntryCount(); i++) 
  {
    const RAT::DS::Entry& dsEntry = dsReader.GetEntry(i);
    if(dsEntry.GetEVCount() == 0) continue;

    // Get MC info
    const RAT::DS::MC& mcEvent = dsEntry.GetMC();
    const RAT::DS::MCParticle& mcParticle = mcEvent.GetMCParticle(0);
    const TVector3 mcPosition = mcParticle.GetPosition();

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

      lightPath.CalcByPositionPartial( mcPosition, pmtPos );
      if( !lightPath.GetPathValid() )  continue;  // do not save invalid path calculations

      const double distInInnerAV = lightPath.GetDistInInnerAV();
      const double distInAV = lightPath.GetDistInAV();
      const double distInWater = lightPath.GetDistInWater();
      const double distInUpperTarget = lightPath.GetDistInUpperTarget();
      const double distInLowerTarget = lightPath.GetDistInLowerTarget();

      double transitTime = effectiveVelocity.CalcByDistance( distInUpperTarget, distInAV, distInWater+distInLowerTarget );

      if(velocity>0)
        // We are overriding the velocity in EffectiveVelocity for faster processing
        transitTime = ( distInInnerAV / velocity + distInAV / effectiveVelocity.GetAVVelocity() +
                        distInWater / effectiveVelocity.GetWaterVelocity() ) + effectiveVelocity.GetOffset();
      const double timeResid = pmt.GetTime() - transitTime - eventTime;
      //Fill level is  747.48, with 108mm offset for av/psup coordinates
     

      /*if(pmtBand % 4 == 0 && pmtPos.Z() >= 1500 + 855.48)
	histogram->Fill(timeResid);
      if(pmtBand % 4 == 1 && pmtPos.Z() >= 0 + 855.48 && pmtPos.Z() < 1500 + 855.48)
        histogram->Fill(timeResid);
      if(pmtBand % 4 == 2 && pmtPos.Z() >= -1500 + 855.48 && pmtPos.Z() < 0 + 855.48)
        histogram->Fill(timeResid);
	if(pmtBand % 4 == 3 && pmtPos.Z() < -1500 + 855.48)*/
      //      if(mcParticle.GetPosition().Mag()<5500)
      histogram->Fill(timeResid);
    }
  }
}
