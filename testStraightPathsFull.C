
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
double fAV_offset_z;
TVector3 fFillLevelCentre;//center of fill level

fAV_offset_z = 108;
fAVRadius = 6005.3; // AV inner radius
fAVRadiusOuter = fAVRadius+55.118; // AV outer radius
fNeckRadius = 728.98; // inner radius of neck, mm
fNeckRadiusOuter = fNeckRadius+64.77; // outer radius of neck, mm
fZneckLo = sqrt( fAVRadiusOuter*fAVRadiusOuter - fNeckRadiusOuter*fNeckRadiusOuter ) + fAV_offset_z; // bottom of the neck

void testStraightPathsFull(std::string filename)
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
    TVector3 mcPos(0,0,-4000);
    TVector3 pmtPos(0,0,-8000);
    TVector3 diff = mcPos-pmtPos;

    double pmtTime = 250;
    double LPdistInAV = 0.0;
    double LPdistInWater = 0.0;
    double LPdistInTarget = 0.0;
    double LPdistInNeck = 0.0;
    double GPdistInAV = 0.0;
    double GPdistInWater = 0.0;
    double GPdistInTarget = 0.0;
    double SWdistInAV = 0.0;
    double SWdistInWater = 0.0;
    double SWdistInTarget = 0.0;
    double SWdistInNeck = 0.0;

    TStopwatch LPtimer;
    LPtimer.Start();

    /// Use LPC to get straight path + time
    ///// MultiPath Straight Line Fitter
    RAT::DU::Point3D pStart(RAT::DU::Point3D::GetSystemId("av"), mcPos.X(), mcPos.Y(), mcPos.Z() );
    RAT::DU::Point3D pEnd(RAT::DU::Point3D::GetSystemId("av"), pmtPos.X(), pmtPos.Y(), pmtPos.Z() );
    RAT::DU::LightPath path(3.0, pStart, pEnd, RAT::DU::LightPath::InnerAV, RAT::DU::LightPath::PMT);
    RAT::DU::LightPathStraightFitter fit;
    fit.Fit(path);
    LPdistInTarget=path.GetDistByRegionType(RAT::DU::LightPath::InnerAV);
    LPdistInWater=path.GetDistByRegionType(RAT::DU::LightPath::Water);
    LPdistInAV=path.GetDistByRegionType(RAT::DU::LightPath::AV);
    LPdistInNeck=path.GetDistByRegionType(RAT::DU::LightPath::Neck);
    LPtimer.Stop();

    TStopwatch SWtimer;
    SWtimer.Start();

    RAT::DU::Point3D pStart2(RAT::DU::Point3D::GetSystemId("av"), mcPos.X(), mcPos.Y(), mcPos.Z());
    RAT::DU::Point3D pEnd2(RAT::DU::Point3D::GetSystemId("av"), pmtPos.X(), pmtPos.Y(), pmtPos.Z());

    RAT::DU::LightPath path2(3.0, pStart2, pEnd2, RAT::DU::LightPath::InnerAV, RAT::DU::LightPath::PMT);
    RAT::DU::Point3D pMid1 = pStart2.GetNewPoint( (0.5*(pmtPos-mcPos).Mag()) * (pmtPos-mcPos));
    path2.InsertPoint(path2.GetPointCount()-1, pMid1, RAT::DU::LightPath::Water );

    RAT::DU::LightPathStraightScint fit2;
    fit2.Fit(path2);
    SWdistInTarget=path2.GetDistByRegionType(RAT::DU::LightPath::InnerAV);
    SWdistInWater=path2.GetDistByRegionType(RAT::DU::LightPath::Water);
    SWdistInAV=path2.GetDistByRegionType(RAT::DU::LightPath::AV);
    SWdistInNeck=path2.GetDistByRegionType(RAT::DU::LightPath::Neck);
    SWtimer.Stop();


    TStopwatch GPtimer;
    GPtimer.Start();
    /// Use GetPath to get straight path + time
    RAT::DU::LightPathStraightScint::GetPath(pmtPos, mcPos, GPdistInTarget, GPdistInWater);

    GPtimer.Stop();    

    GPExecTime += GPtimer.RealTime();
    LPExecTime += LPtimer.RealTime();
    SWExecTime += SWtimer.RealTime();

    float LPtransitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( LPdistInTarget, LPdistInAV+LPdistInNeck, LPdistInWater );
    float LPcorrectedTime = pmtTime - LPtransitTime - eventTime;
    
    float GPtransitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( GPdistInTarget, GPdistInAV, GPdistInWater );
    float GPcorrectedTime = pmtTime - GPtransitTime - eventTime;

    float SWtransitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( SWdistInTarget, SWdistInAV, SWdistInWater );
    float SWcorrectedTime = pmtTime - SWtransitTime - eventTime;
    
    std::cout << "event " << i << std::endl;
    std::cout << "vertexPos\t\t" << mcPos.X() << "\t" << mcPos.Y() << "\t" << mcPos.Z() << std::endl;
    std::cout << "pmtPos\t\t\t" << pmtPos.X() << "\t" << pmtPos.Y() << "\t" << pmtPos.Z() << " " << std::endl;
    std::cout << "distInAV\t\t" << LPdistInAV << " " << GPdistInAV << " " << SWdistInAV << std::endl;
    std::cout << "distInWater\t\t" << LPdistInWater << " " << GPdistInWater << " " << SWdistInWater << std::endl;
    std::cout << "distInTarget\t" << LPdistInTarget << " " << GPdistInTarget << " " << SWdistInTarget << std::endl;
    std::cout << "distInNeck\t" << LPdistInNeck << " " << SWdistInNeck << std::endl;
    std::cout << "transitTime\t\t" << LPtransitTime << " " << GPtransitTime << " " << SWtransitTime << std::endl;
    std::cout << "correctedTime\t\t" << LPcorrectedTime << " " << GPcorrectedTime << " " << SWcorrectedTime << std::endl;
    std::cout << "totalDistance\t\t" << LPdistInAV+LPdistInWater+LPdistInTarget+LPdistInNeck << " " << GPdistInAV+GPdistInWater+GPdistInTarget << " " << path.GetTotalDist() << " " << " " << SWdistInAV+SWdistInWater+SWdistInTarget+SWdistInNeck << " " << diff.Mag() <<  std::endl << std::endl;
    
  }
  
  std::cout << "LP time per event " << LPExecTime << std::endl;
  std::cout << "GP time per event " << GPExecTime << std::endl;
  std::cout << "SW time per event " << SWExecTime << std::endl;
}


void GetPath (TVector3 fPMTpos, TVector3 fVertex, double &distInTarget, double &distInWater){

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
