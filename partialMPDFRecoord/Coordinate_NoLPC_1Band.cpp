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

void FillPartialTimeResidualsPlot(std::string material, TH1D* histogram, double velocity=-999, int pmtBand=0);

void GetPath (TVector3 fPMTpos, TVector3 fVertex, double &distInWater, double &distInScint);

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

  double pmtzmax, pmtzmin, vertexrmax, vertexrmin;

  

  std::cout << "running with velocity " << velocity << endl;

  for(unsigned int i = 0; i < numberOfRuns; i++)
    {

      //std::stringstream fileNameStream;
      //      fileNameStream << material + "_DataForPDF_part" << (i + 1) << ".root";

      std::stringstream fileNameStream;
      fileNameStream << filePathName + "_" << i << ".root";
      std::cout << fileNameStream.str() << std::endl;
      FillPartialTimeResidualsPlot(fileNameStream.str(), histogram1, velocity, pmtBand);
    }

  // Print coordinated values to screen in RATDB format
  std::cout << std::endl;
  std::cout << "Please place the text below into the database file: ET1D.ratdb located in rat/data, replacing any existing entry with the same index." << std::endl;
  std::cout << std::endl;
  std::cout << "{" << std::endl;
  std::cout << "type : \"ET1D_Partial_MultiPDF\"," << std::endl;
  std::cout << "version: 1," << std::endl;
  std::cout << "index: \"" << material << "_" << numPDFs << "PDFBand" << pmtBand << "\"," << std::endl;
  std::cout << "pmt_z_range: [ -40000, 40000]," << std::endl;
  std::cout << "vertex_r_range:  [-40000 ]," << std::endl;
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
  outname << "Jan12_NoLPC_" << numPDFs << "PDFBand" << pmtBand << ".root";
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

  DB *db = DB::Get();
  double fFillLevel;
  std::vector <double> fAV_offset_z;
  TVector3 fFillLevelCentre;//center of fill level
  double boundary = 1500; //boundary between different pdfs
  fFillLevel = db->GetLink("GEO", "inner_av")->GetD("split_z");
  fAV_offset_z = db->GetLink("GEO", "av")->GetDArray("position");
  fFillLevelCentre.SetXYZ(0,0,fFillLevel+fAV_offset_z[2]);//Centre of fill level

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
      double distInUpperTarget = 0.0;
      double distInLowerTarget = 0.0;
      double distInInnerAV = 0.0;

      GetPath(pmtPos, mcPosition, distInUpperTarget, distInWater);

      /*     
      lightPath.CalcByPositionPartial( mcPosition, pmtPos );
      if( !lightPath.GetPathValid() )  continue;  // do not save invalid path calculations

      const double distInInnerAV = lightPath.GetDistInInnerAV();
      const double distInAV = lightPath.GetDistInAV();
      const double distInWater = lightPath.GetDistInWater();
      const double distInUpperTarget = lightPath.GetDistInUpperTarget();
      const double distInLowerTarget = lightPath.GetDistInLowerTarget();*/
      
      double transitTime = effectiveVelocity.CalcByDistance( distInUpperTarget, distInAV, distInWater+distInLowerTarget );

      if(velocity>0)
        // We are overriding the velocity in EffectiveVelocity for faster processing
        transitTime = ( distInInnerAV / velocity + distInAV / effectiveVelocity.GetAVVelocity() +
                        distInWater / effectiveVelocity.GetWaterVelocity() ) + effectiveVelocity.GetOffset();
      const double timeResid = pmt.GetTime() - transitTime - eventTime;
      //Fill level is  747.48, with 108mm offset for av/psup coordinates
     
      histogram->Fill(timeResid);
    }
  }
}


void GetPath (TVector3 fPMTpos, TVector3 fVertex, double &distInUpperTarget, double &distInWater){

  double fAVRadiusOuter; // outer radius, 6060 mm
  double fAVRadius; // inner radius, 6005 mm
  double fNeckRadiusOuter; // outer radius of neck
  double fNeckRadius; // inner radius of neck
  double fBoundary = 0.001;
  double fPSUPRadius = 8390;
  double fPSUPRadius2; // psup_rad, squared
  double fMaxPMTtRes; // max_PMT_tRes
  double fZneckLo; // bottom of the neck in z position
  double fFillLevel;
  std::vector <double> fAV_offset_z;
  TVector3 fFillLevelCentre;//center of fill level

  DB *db = DB::Get();
  fFillLevel = db->GetLink("GEO", "inner_av")->GetD("split_z");
  fAV_offset_z = db->GetLink("GEO", "av")->GetDArray("position");
  fFillLevelCentre.SetXYZ(0,0,fFillLevel+fAV_offset_z[2]);//Centre of fill level
  fAVRadius = db->GetLink( "SOLID", "acrylic_vessel_inner" )->GetD( "r_sphere" ); // 6005.0, AV inner radius
  fAVRadiusOuter = db->GetLink( "SOLID", "acrylic_vessel_outer" )->GetD( "r_sphere" ); // 6060, AV outer radius
  fNeckRadius = db->GetLink( "SOLID", "acrylic_vessel_inner" )->GetD( "r_neck" ); // inner radius of neck = 730 mm
  fNeckRadiusOuter = db->GetLink( "SOLID", "acrylic_vessel_outer" )->GetD( "r_neck" ); // outer radius of neck = 785 mm
  fZneckLo = sqrt( fAVRadiusOuter*fAVRadiusOuter - fNeckRadiusOuter*fNeckRadiusOuter ) + fAV_offset_z[2]; // bottom of the neck



  // look up position each time - is this efficient enough?
  // const TVector3& pos = DU::Utility::Get()->GetPMTInfo().GetPosition(pmt.GetID());
  // double tpmt = pmt.GetTime();

  double dx = fPMTpos[0] - fVertex[0];
  double dy = fPMTpos[1] - fVertex[1];
  double dz = fPMTpos[2] - fVertex[2];
  double dr = sqrt(dx*dx + dy*dy + dz*dz); // | pmtpos - X0|

  //  TVector3 fVertex( startPos[0], par[1], par[2] ); // X0 = (x0,y0,z0), trial vertex
  // TVector3 fPMTpos( pmt[0], pmt[1], pmt[2] );
  /// following is to calculate time residual by: PMTtime - tof - trial time;
  // fRes = pmt[3] - tof - par[3];
  // in partial fitter, tof is calculated separately in different ways in different situations;
  // instead of fRes, (tDiff, tof) is passed to the likelihood calculation
  //double tDiff = fPMTpos[3] - fVertex[3];// tDiff = hitTime - trialTime; then fRes = tDiff - tof

  double fx = fVertex[0]; // trial x0
  double fy = fVertex[1]; // trial y0
  double fz = fVertex[2]; // trial z0
  /// water level set relative to the AV
  //double depth = fFillLevel - fz; // L - z0
  //double height = fPMTpos[2] - fFillLevel; // Zpmt - L

  if( fFillLevel >= -fAVRadius && fFillLevel <= fAVRadius ) { // valid water level, check in PSUP coordinate

    /// evaluate the distanceInScint = scintpathInNeck + scintpathInAV
    double distInScint = 0;

    /// evaluate line-AV sphere intersection first
    double scintpathInAV = 0;
    double udDotVtx = fx*dx/dr + fy*dy/dr + (fz-fAV_offset_z[2])*dz/dr; // (P-X0)/|P-X0| * (X0 - oAV)
    double vtx2 = fx*fx + fy*fy + (fz-fAV_offset_z[2])*(fz-fAV_offset_z[2]); // (X0 - oAV)*(X0 - oAV)
    double rVertex = sqrt( vtx2 ); // Roffset = |X0 - oAV|
    double sqrVal = udDotVtx*udDotVtx - vtx2 + fAVRadiusOuter*fAVRadiusOuter;
    double a1 = 0, a2 = 0, aplus = 0, abig = 0, asmall = 0;
    int scintPathType=-1;
    if( sqrVal>0 ) { // line passes AV sphere
      /// find the line-sphere intersect points; a1, a2 are the path lengths btw vertex and intersection points
      a1 = -udDotVtx + sqrt(sqrVal);
      a2 = -udDotVtx - sqrt(sqrVal); // a2<a1
      /// find the line-interface plane intersection; a3 is the path length btw vertex and intersection point
      double a3 = -9999; // a3 is not defined if lightPath is parallel to interface
      if( fabs(dz)>fBoundary ) a3 = ( fFillLevel-fz )*dr/dz; // well-defined a3

      if( rVertex<fAVRadiusOuter ) { // vertex inside the AV, equivalent to a1*a2<0
        if ( a1*a2<0 ) {
          aplus = a1; // always has a1>0>a2
          if( a3>0 ) { // a3>0, hit interface plane
            if( fz<fFillLevel ) { // vertex below
              if( a3<aplus ){
                scintpathInAV = aplus - a3;
                scintPathType=0;
              }
            }
            else { // vertex above
              if( a3<aplus ) {
                scintpathInAV = a3;
                scintPathType=1;
              }
              else{
                scintpathInAV = aplus;
                scintPathType=2;
              }
            }
          }
          else { // a3<=0, not hit interface plane
            if( fz>=fFillLevel ){
              scintpathInAV = aplus; // vertex must above
              scintPathType=4;
            }
          }
        }
      } // vertex in AV

      else { // rVertex>=fAVRadiusOuter, vertex in external
        if( a1>0 && a2>0 && a1<dr) { // always has a1>a2>0
          abig = a1; // far intersection point
          asmall = a2; // near intersection point
          double zbig = fz + abig*dz/dr; // z position of the intersection points
          double zsmall = fz + asmall*dz/dr;
          if( zsmall>=fFillLevel && zbig>=fFillLevel ){
            scintPathType=5;
            scintpathInAV = abig - asmall;
          }
          if( zsmall<fFillLevel && zbig>fFillLevel && a3>0 ){
            scintpathInAV = abig - a3;
            scintPathType=6;
          }
          if( zsmall>fFillLevel && zbig<fFillLevel && a3>0 ){
            scintpathInAV = a3 - asmall;
            scintPathType=7;
          }
        } // ensure a1 and a2 are positive
      } // vertex in external
    } // pass through AV

    /// evaluate line-neck cylinder intersection
    double scintpathInNeck = 0;
    bool fNeckpathEnable=true;
    int neckPathType=-1;
    double an2=0, an1=0;
    if( fNeckpathEnable ) { // turn off to ignore high z(neck) events

      double fZneckHi = fPSUPRadius; // top Z of the neck = Zpsup
      double bneck = fx*dx + fy*dy;
      double drPerp2 = dx*dx + dy*dy;
      double sqrValneck = bneck*bneck - ( fx*fx+fy*fy-fNeckRadiusOuter*fNeckRadiusOuter )*drPerp2;

      if( sqrValneck > 0 ) { // check the line passes through the neck
        double aneck1 = dr*( -bneck + sqrt(sqrValneck) )/drPerp2; // dr comes from direction.Unit()
        double aneck2 = dr*( -bneck - sqrt(sqrValneck) )/drPerp2;
        /// the z position of intersection point is checked, to cancel out the duplications in AV

        if( aneck1*aneck2<0 ) { // vertex inside cylinder, one hit point on cylinder
          double aneckplus = aneck1; // aneck1>0>aneck2
          double zplus = fz + aneckplus*dz/dr;
          if( zplus < fZneckHi && zplus > fZneckLo ) { // hit point in neck region
            if( rVertex>=fAVRadiusOuter ) { // vertex outside AV
              scintpathInNeck = aneckplus;
              neckPathType = 1;
            }
            else { // vertex inside AV
              if( a1*a2<0 ){
                scintpathInNeck = aneckplus - aplus; // hit AV at one point and then hit neck
                neckPathType=2;
              }
            }
          }
          if( zplus < fZneckLo ) { // hit below the neck but on cylinder
            if( rVertex>=fAVRadiusOuter && fz>fZneckLo && fz<fZneckHi ) { // this case forces vertex inside neck and outside AV
              if( a1>0 && a2>0 ) { // must hits the AV at 2 points
                scintpathInNeck = asmall;
                neckPathType=3;
              }
            }
          }
        } // vertex inside cylinder

        if( aneck1>0 && aneck2>0 && aneck1<dr && aneck2<dr) { // vertex outside cylinder, two intersection points on cylinder
          double aneckbig = aneck1>aneck2? aneck1:aneck2; // far hit point
          double anecksmall = aneck1<aneck2? aneck1:aneck2; // near hit point
          double zbig = fz + aneckbig*dz/dr; // z of far hit point
          double zsmall = fz + anecksmall*dz/dr; // z of near hit point
          /// if two intersection points are in neck region
          if( zbig<fZneckHi && zbig>fZneckLo && zsmall<fZneckHi && zsmall>fZneckLo )
            {
              an1=aneckbig;
              an2=anecksmall;
              scintpathInNeck = aneckbig - anecksmall;
              neckPathType=4;
              // an extreme condition: pass thru neck as well as virtual AV sphere inside neck
              if(a1>0 && a2>0) {
                double zAVbig = fz + abig*dz/dr, zAVsmall = fz + asmall*dz/dr; // z of AV hit points
                if(zAVbig>=fZneckLo && zAVsmall>=fZneckLo){
                  scintpathInNeck = scintpathInNeck - (abig-asmall);
                  neckPathType=5;
                }
              }
            }
          if( zbig<fZneckLo && zsmall>fZneckLo && zsmall<fZneckHi ) {
            if(a1>0 && a2>0) { // must pass the AV at 2 points and outside the AV
              scintpathInNeck = asmall - anecksmall;
              neckPathType=6;
            }
          }
          if( zsmall<fZneckLo && zbig>fZneckLo && zbig<fZneckHi) {
            if(a1*a2<0) {// inside AV
              scintpathInNeck = aneckbig - aplus;
              neckPathType=7;
            }
            if(a1>0 && a2>0) { // outside AV
              scintpathInNeck = aneckbig - abig;
              neckPathType=8;
            }
          }
        } // vertex outside the neck, aneck1>0 and aneck2>0
      } // line hits the neck
    } // enalbe neck path calculation

    if( scintpathInAV<0 ) scintpathInAV = 0; // avoid negative calculated value
    if( scintpathInNeck<0 ) scintpathInNeck = 0; // avoid negative calculated value

    //    std::cout << dr << " " << scintpathInAV << " " << scintpathInNeck << std::endl;
    distInScint = scintpathInAV + scintpathInNeck;
    distInWater =  dr - distInScint;
    distInUpperTarget = distInScint;

  } // valid water level
  else {
    //    fHeight = pmt[2];
    Log::Die( "Not a valid water height ");
  }

}
