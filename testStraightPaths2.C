
using namespace RAT;
using namespace RAT::DS;

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
double fAV_offset_z;
TVector3 fFillLevelCentre;//center of fill level

fFillLevel = 747.48;
fAV_offset_z = 131.8;
fFillLevel+=fAV_offset_z;
fFillLevelCentre.SetXYZ(0,0,fFillLevel);//Centre of fill level
fAVRadius = 6005.3; // AV inner radius
fAVRadiusOuter = fAVRadius+55.118; // AV outer radius
fNeckRadius = 728.98; // inner radius of neck, mm
fNeckRadiusOuter = fNeckRadius+64.77; // outer radius of neck, mm
fZneckLo = sqrt( fAVRadiusOuter*fAVRadiusOuter - fNeckRadiusOuter*fNeckRadiusOuter ) + fAV_offset_z; // bottom of the neck

void testStraightPaths2(std::string filename)
{

  RAT::DU::DSReader dsreader(filename);

  double LPExecTime = 0;
  double GPExecTime = 0;
  double SWExecTime = 0;
  double eventTime = 250;

  // Loop through events
  for(int i=0; i<1; i++){
    
    std::cout << "event " << i << std::endl;
    //TVector3 mcPos(2806.35,-3247.95,695.305);
    //TVector3 pmtPos(3436.44,-7189.03,2675.36);
    
    //    TVector3 mcPos(-3403.46,-1831.13,2137.19);
    // TVector3 pmtPos(3660.6,-2473.09,-7150.71);
    //-5420.39,-1125.46,6347.53,
    TVector3 mcPos(0,0,4000);
    TVector3 pmtPos(0,0,-8000);
    TVector3 diff = mcPos-pmtPos;

    double pmtTime = 250;
    double LPdistInAV = 0.0;
    double LPdistInWater = 0.0;
    double LPdistInUpperTarget = 0.0;
    double LPdistInLowerTarget = 0.0;
    double LPdistInNeck = 0.0;
    double GPdistInAV = 0.0;
    double GPdistInWater = 0.0;
    double GPdistInUpperTarget = 0.0;
    double GPdistInLowerTarget = 0.0;
    double SWdistInAV = 0.0;
    double SWdistInWater = 0.0;
    double SWdistInUpperTarget = 0.0;
    double SWdistInLowerTarget = 0.0;
    double SWdistInNeck = 0.0;

    TStopwatch LPtimer;
    LPtimer.Start();

    /// Use LPC to get straight path + time
    ///// MultiPath Straight Line Fitter
    RAT::DU::Point3D pStart(RAT::DU::Point3D::GetSystemId("av"), mcPos.X(), mcPos.Y(), mcPos.Z() + fAV_offset_z );
    RAT::DU::Point3D pEnd(RAT::DU::Point3D::GetSystemId("av"), pmtPos.X(), pmtPos.Y(), pmtPos.Z() + fAV_offset_z );
    RAT::DU::LightPath path(3.0, pStart, pEnd, RAT::DU::LightPath::UpperInnerAV, RAT::DU::LightPath::PMT);
    RAT::DU::LightPathStraightFitter fit;
    fit.Fit(path);
    LPdistInUpperTarget=path.GetDistByRegionType(RAT::DU::LightPath::UpperInnerAV);
    LPdistInLowerTarget=path.GetDistByRegionType(RAT::DU::LightPath::LowerInnerAV);
    LPdistInWater=path.GetDistByRegionType(RAT::DU::LightPath::Water);
    LPdistInAV=path.GetDistByRegionType(RAT::DU::LightPath::AV);
    LPdistInNeck=path.GetDistByRegionType(RAT::DU::LightPath::Neck);
    LPtimer.Stop();

    TStopwatch SWtimer;
    SWtimer.Start();

    RAT::DU::Point3D pStart2(RAT::DU::Point3D::GetSystemId("av"), mcPos.X(), mcPos.Y(), mcPos.Z()-131.8 );
    RAT::DU::Point3D pEnd2(RAT::DU::Point3D::GetSystemId("av"), pmtPos.X(), pmtPos.Y(), pmtPos.Z()-131.8 );

    RAT::DU::LightPath path2(3.0, pStart2, pEnd2, RAT::DU::LightPath::UpperInnerAV, RAT::DU::LightPath::PMT);

    RAT::DU::LightPathStraightScintWater fit2;
    fit2.Fit(path2);
    SWdistInUpperTarget=path2.GetDistByRegionType(RAT::DU::LightPath::UpperInnerAV);
    SWdistInLowerTarget=path2.GetDistByRegionType(RAT::DU::LightPath::LowerInnerAV);
    SWdistInWater=path2.GetDistByRegionType(RAT::DU::LightPath::Water);
    SWdistInAV=path2.GetDistByRegionType(RAT::DU::LightPath::AV);
    SWdistInNeck=path2.GetDistByRegionType(RAT::DU::LightPath::Neck);
    SWtimer.Stop();


    TStopwatch GPtimer;
    GPtimer.Start();
    /// Use GetPath to get straight path + time
    GetPath(pmtPos, mcPos, GPdistInUpperTarget, GPdistInWater);
    GPtimer.Stop();    

    GPExecTime += GPtimer.RealTime();
    LPExecTime += LPtimer.RealTime();
    SWExecTime += SWtimer.RealTime();

    float LPtransitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( LPdistInUpperTarget, LPdistInAV+LPdistInNeck, LPdistInWater+LPdistInLowerTarget );
    float LPcorrectedTime = pmtTime - LPtransitTime - eventTime;
    
    float GPtransitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( GPdistInUpperTarget, GPdistInAV, GPdistInWater );
    float GPcorrectedTime = pmtTime - GPtransitTime - eventTime;

    float SWtransitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( SWdistInUpperTarget, SWdistInAV, SWdistInWater );
    float SWcorrectedTime = pmtTime - SWtransitTime - eventTime;
    
    std::cout << "event " << i << std::endl;
    std::cout << "vertexPos\t\t" << mcPos.X() << "\t" << mcPos.Y() << "\t" << mcPos.Z() << std::endl;
    std::cout << "pmtPos\t\t\t" << pmtPos.X() << "\t" << pmtPos.Y() << "\t" << pmtPos.Z() << " " << std::endl;
    std::cout << "distInAV\t\t" << LPdistInAV << " " << GPdistInAV << " " << SWdistInAV << std::endl;
    std::cout << "distInWater\t\t" << LPdistInWater+LPdistInLowerTarget << " " << GPdistInWater+GPdistInLowerTarget << " " << SWdistInWater+SWdistInLowerTarget << std::endl;
    std::cout << "distInUpperTarget\t" << LPdistInUpperTarget << " " << GPdistInUpperTarget << " " << SWdistInUpperTarget << std::endl;
    std::cout << "distInLowerTarget\t" << LPdistInLowerTarget << " " << GPdistInLowerTarget << " " << SWdistInLowerTarget << std::endl;
    std::cout << "distInNeck\t" << LPdistInNeck << " " << SWdistInNeck << std::endl;
    std::cout << "transitTime\t\t" << LPtransitTime << " " << GPtransitTime << " " << SWtransitTime << std::endl;
    std::cout << "correctedTime\t\t" << LPcorrectedTime << " " << GPcorrectedTime << " " << SWcorrectedTime << std::endl;
    std::cout << "totalDistance\t\t" << LPdistInAV+LPdistInWater+LPdistInLowerTarget+LPdistInUpperTarget+LPdistInNeck << " " << GPdistInAV+GPdistInWater+GPdistInLowerTarget+GPdistInUpperTarget << " " << path.GetTotalDist() << " " << " " << SWdistInAV+SWdistInWater+SWdistInLowerTarget+SWdistInUpperTarget+SWdistInNeck << " " << diff.Mag() <<  std::endl << std::endl;
    
  }
  
  std::cout << "LP time per event " << LPExecTime << std::endl;
  std::cout << "GP time per event " << GPExecTime << std::endl;
  std::cout << "SW time per event " << SWExecTime << std::endl;
}


void GetPath (TVector3 fPMTpos, TVector3 fVertex, double &distInUpperTarget, double &distInWater){


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
    double udDotVtx = fx*dx/dr + fy*dy/dr + (fz-fAV_offset_z)*dz/dr; // (P-X0)/|P-X0| * (X0 - oAV)
    double vtx2 = fx*fx + fy*fy + (fz-fAV_offset_z)*(fz-fAV_offset_z); // (X0 - oAV)*(X0 - oAV)
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
    std::cout << fFillLevel << " " << fAVRadius << std::endl;
    Log::Die( "Not a valid water height " );
  }

}

