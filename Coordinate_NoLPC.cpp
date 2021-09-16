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

//#include <RAT/MultiPDFPartial_1PDFs.hh>

using namespace RAT;

void FillPartialTimeResidualsPlot(std::string material, TH1D* histogram[6], double velocity=-999);

void GetPath (TVector3 fPMTpos, TVector3 fVertex, double &distInWater, double &distInScint);

void GetFullPDF(std::string filePathName, std::string material, unsigned int numberOfRuns, double velocity=-999, int numPDFs=0)
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

  TH1D* histogram1[6];// = new TH1D("histogram1", "", numBins, lowLim-binWidth/2., upLim+binWidth/2.);
  TH1D* histogram2[6];// = new TH1D("histogram2", "", numBins, lowLim-binWidth/2., upLim+binWidth/2.);

  for(int i=0; i<2; i++){
    
    TString hname1 = Form("histogram1%d", i);
    TString hname2 = Form("histogram2%d", i);

    histogram1[i] = new TH1D(hname1, "", numBins, lowLim-binWidth/2., upLim+binWidth/2.);
    histogram2[i] = new TH1D(hname2, "", numBins, lowLim-binWidth/2., upLim+binWidth/2.);
  }

  double distmax[6], distmin[6];

  if(numPDFs==1){
    for(int i = 0; i<6; i++){
      distmax[i] = 40000 ;
      distmin[i] = 0 ;
    }
  }
  else if (numPDFs==6){
    distmax[0] = 4000 ;
    distmin[0] = 0 ;   
    distmax[1] = 6000 ;
    distmin[1] = 4000 ;
    distmax[2] = 8000 ;
    distmin[2] = 6000 ;
    distmax[3] = 10000 ;
    distmin[3] = 8000 ;
    distmax[4] = 12000 ;
    distmin[4] = 10000 ;
    distmax[5] = 40000 ;
    distmin[5] = 12000 ;
  }
  else if(numPDFs==2){
    distmax[0] = 8000 ;
    distmin[0] = 0 ;
    distmax[1] = 15000 ;
    distmin[1] = 8000 ;
  }
  std::cout << "running with velocity " << velocity << endl;

  TH1D* disthist = new TH1D("h1","h1", 40,0,20);

  for(unsigned int i = 0; i < numberOfRuns; i++)
    {

      std::stringstream fileNameStream;
      fileNameStream << filePathName + "_" << i << ".root";
      std::cout << fileNameStream.str() << std::endl;
      FillPartialTimeResidualsPlot(fileNameStream.str(), histogram1, velocity);
    }
  
  for(int ihist=0; ihist < 2; ihist++) {

    histogram1[ihist]->Scale(100000/histogram1[ihist]->Integral());

    // Print coordinated values to screen in RATDB format
    std::cout << std::endl;
    std::cout << "Please place the text below into the database file: ET1D_MultiPDF.ratdb located in rat/data, replacing any existing entry with the same index." << std::endl;
    std::cout << std::endl;
    std::cout << "{" << std::endl;
    std::cout << "type : \"ET1D_MultiPDF\"," << std::endl;
    std::cout << "version: 1," << std::endl;
    std::cout << "index: \"" << material << "_" << numPDFs << "PDFNum" << ihist << "\"," << std::endl;
    std::cout << "distanc_range:  [" << distmin[ihist] << ", " << distmax[ihist] << " ]," << std::endl;
    std::cout << "run_range: [0, 0]," << std::endl;
    std::cout << "pass: 0," << std::endl;
    std::cout << "production: false," << std::endl;
    std::cout << "timestamp: \"\"," << std::endl;
    std::cout << "comment: \"Generated with RAT " << RAT::GetRATVersion() << "\"," << std::endl;
    
    std::cout << "time: [";
    for(unsigned int j = 1; j <= numBins; j++)
      {
	std::cout << std::fixed << std::setprecision(2)<< float(histogram1[ihist]->GetBinCenter(j)) << ", ";
      }
    std::cout << "]," << std::endl;
    
    std::cout << "probability: [";
    for(unsigned int j = 1; j <= numBins; j++)
      {
    // Approximate early hits to a flat distribution
    if (j < lowBinFlat)
    {
      histogram2[ihist]->Fill(histogram1[ihist]->GetBinCenter(j), histogram1[ihist]->GetBinContent(lowBinVal));
      std::cout << std::fixed << std::setprecision(2)<< float(histogram1[ihist]->GetBinContent(lowBinVal)) << ", ";
    }
    // Approximate late hits to a flat distribution
    else if (j > upBinFlat)
    {
      histogram2[ihist]->Fill(histogram1[ihist]->GetBinCenter(j), histogram1[ihist]->GetBinContent(upBinFlat));
      std::cout << std::fixed << std::setprecision(2)<< float(histogram1[ihist]->GetBinContent(upBinFlat)) << ", ";
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
  histogram1[ihist]->Draw();
  histogram1[ihist]->GetXaxis()->SetTitle("Time Residual, ns");
  histogram2[ihist]->SetLineColor(2);
  histogram2[ihist]->Draw("SAMES");
  }
  std::stringstream outname;
  outname << "May24_2PDF_DistHist.root";
  TFile *outfile = TFile::Open(outname.str().c_str(),"RECREATE");
  for(int i=0; i<2; i++){
    TString hname1 = Form("MC_%d",i);
    TString hname2 = Form("PDF_%d",i);
    histogram1[i]->Write(hname1);
    histogram2[i]->Write(hname2);
  }
  disthist->Write("disthist");
}

// Update the Scintillator Time Residual distribution from a single rootfile
void FillPartialTimeResidualsPlot(std::string infile, TH1D* histogram[6], double velocity)
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

      GetPath(pmtPos, mcPosition, distInTarget, distInWater);
      
      double transitTime = effectiveVelocity.CalcByDistance( distInTarget, distInAV, distInWater);

      if(velocity>0)
        // We are overriding the velocity in EffectiveVelocity for faster processing
        transitTime = ( distInInnerAV / velocity + distInAV / effectiveVelocity.GetAVVelocity() +
                        distInWater / effectiveVelocity.GetWaterVelocity() ) + effectiveVelocity.GetOffset();
      const double timeResid = pmt.GetTime() - transitTime - eventTime;
     
      double dist = (pmtPos-mcPosition).Mag();

      if(dist < 8000 )
        histogram[0]->Fill(timeResid);
      else
        histogram[1]->Fill(timeResid);

      /*      if(dist < 4000 ) 
	histogram[0]->Fill(timeResid);
      if(dist < 6000 && dist > 4000 )
        histogram[1]->Fill(timeResid);
      if(dist < 8000 && dist > 6000 )
        histogram[2]->Fill(timeResid);
      if(dist < 10000 && dist > 8000 )
        histogram[3]->Fill(timeResid);
      if(dist < 12000 && dist > 10000 )
        histogram[4]->Fill(timeResid);
      if(dist > 12000 )
        histogram[5]->Fill(timeResid);*/
    }
  }
}

//Algorithm originally taken from src/fit/vertexFunctions/MultiPathScint.cc
void GetPath (TVector3 fPMTpos, TVector3 fVertex, double &distInTarget, double &distInWater){

  double fAVRadiusOuter; // outer radius, 6060 mm
  double fAVRadius; // inner radius, 6005 mm
  double fNeckRadiusOuter; // outer radius of neck
  double fNeckRadius; // inner radius of neck
  double fBoundary = 0.001;
  double fPSUPRadius = 8390;
  double fPSUPRadius2; // psup_rad, squared
  double fMaxPMTtRes; // max_PMT_tRes
  std::vector <double> fAV_offset;
  double fAV_offset_z;

  DB *db = DB::Get();
  fAV_offset = db->GetLink("GEO", "av")->GetDArray("position");
  fAV_offset_z = fAV_offset[2];
   fAVRadius = db->GetLink( "SOLID", "acrylic_vessel_inner" )->GetD( "r_sphere" ); // 6005.0, AV inner radius
  fAVRadiusOuter = db->GetLink( "SOLID", "acrylic_vessel_outer" )->GetD( "r_sphere" ); // 6060, AV outer radius
  fNeckRadius = db->GetLink( "SOLID", "acrylic_vessel_inner" )->GetD( "r_neck" ); // inner radius of neck = 730 mm
  fNeckRadiusOuter = db->GetLink( "SOLID", "acrylic_vessel_outer" )->GetD( "r_neck" ); // outer radius of neck = 785 mm
 
  double dx = fPMTpos[0] - fVertex[0];
  double dy = fPMTpos[1] - fVertex[1];
  double dz = fPMTpos[2] - fVertex[2];
  double dr = sqrt(dx*dx + dy*dy + dz*dz); // | pmtpos - X0|

  double fx = fVertex[0]; // trial x0
  double fy = fVertex[1]; // trial y0
  double fz = fVertex[2]; // trial z0

  /// evaluate line-AV sphere intersection first
  double scintpathInAV = 0;
  double udDotVtx = fx*dx/dr + fy*dy/dr + (fz-fAV_offset_z)*dz/dr; // (P-X0)/|P-X0| * (X0 - oAV)
  double vtx2 = fx*fx + fy*fy + (fz-fAV_offset_z)*(fz-fAV_offset_z); // (X0 - oAV)*(X0 - oAV)
  double rVertex = sqrt( vtx2 ); // Roffset = |X0 - oAV|
  double sqrVal = udDotVtx*udDotVtx - vtx2 + fAVRadiusOuter*fAVRadiusOuter;
  double a1 = 0, a2 = 0, aplus = 0, abig = 0, asmall = 0;

  if( sqrVal>0 ) { // line passes AV sphere
    /// find the line-sphere intersect points; a1, a2 are the path lengths btw vertex and intersection points

    a1 = -udDotVtx + sqrt(sqrVal);
    a2 = -udDotVtx - sqrt(sqrVal); // a2<a1

    if( rVertex<fAVRadiusOuter ) { // vertex inside the AV, equivalent to a1*a2<0
      if ( a1*a2<0 ) {
        aplus = a1; // a1>0>a2
        scintpathInAV = aplus;
      }
    } // vertex in AV

    else { // rVertex>=fAVRadiusOuter, vertex in external
      if( a1>0 && a2>0) {
        abig = a1; // far intersection point
        asmall = a2; // near intersection point
        scintpathInAV = abig - asmall;
      } // ensure a1 and a2 are positive
    } // vertex in external
  } // pass through AV

  if( scintpathInAV<0 ) scintpathInAV = 0; // avoid negative calculated value

  distInWater =  dr - scintpathInAV;
  distInTarget = scintpathInAV;
}
