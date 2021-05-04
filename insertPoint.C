
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

void insertPoint(std::string filename)
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
    double SWdistInAV = 0.0;
    double SWdistInWater = 0.0;
    double SWdistInUpperTarget = 4000.0;
    double SWdistInLowerTarget = 0.0;
    double SWdistInNeck = 0.0;

    RAT::DU::Point3D pStart2(RAT::DU::Point3D::GetSystemId("av"), mcPos.X(), mcPos.Y(), mcPos.Z()-131.8 );
    RAT::DU::Point3D pEnd2(RAT::DU::Point3D::GetSystemId("av"), pmtPos.X(), pmtPos.Y(), pmtPos.Z()-131.8 );
    RAT::DU::Point3D pMid(RAT::DU::Point3D::GetSystemId("av"), 0, 0, -131.8);

    TVector3 dir = pEnd2.GetDirectionFrom(pStart2);
    dir = dir.Unit();
    std::cout << dir.X() << " " << dir.Y() << " " << dir.Z() << std::endl;
    RAT::DU::Point3D pMid1 = pStart2.GetNewPoint(SWdistInUpperTarget*dir);

    std::cout << pMid.X() << " " << pMid.Y() << " " << pMid.Z() << std::endl;
    std::cout << pMid1.X() << " " << pMid1.Y() << " " << pMid1.Z() << std::endl;

    RAT::DU::LightPath path2(3.0, pStart2, pEnd2, RAT::DU::LightPath::UpperInnerAV, RAT::DU::LightPath::PMT);

    std::cout << path2.GetDistByRegionType(RAT::DU::LightPath::UpperInnerAV) << " " << path2.GetDistByRegionType(RAT::DU::LightPath::Water) << " " << path2.GetDistByRegionType(RAT::DU::LightPath::LowerInnerAV) << std::endl;

    std::cout << path2.GetPointCount() << std::endl;

    path2.InsertPoint(path2.GetPointCount()-1, pMid1, RAT::DU::LightPath::Water );//pStart2.GetNewPoint(SWdistInUpperTarget*dir), RAT::DU::LightPath::Water);

    std::cout << path2.GetPointCount() << std::endl;

    //    RAT::DU::LightPathStraightScintWater fit2;
    //fit2.Fit(path2);
    SWdistInUpperTarget=path2.GetDistByRegionType(RAT::DU::LightPath::UpperInnerAV);
    SWdistInLowerTarget=path2.GetDistByRegionType(RAT::DU::LightPath::LowerInnerAV);
    SWdistInWater=path2.GetDistByRegionType(RAT::DU::LightPath::Water);
    SWdistInAV=path2.GetDistByRegionType(RAT::DU::LightPath::AV);
    SWdistInNeck=path2.GetDistByRegionType(RAT::DU::LightPath::Neck);

    std::cout << path2.GetDistByRegionType(RAT::DU::LightPath::UpperInnerAV) << " " << path2.GetDistByRegionType(RAT::DU::LightPath::Water) << " " << path2.GetDistByRegionType(RAT::DU::LightPath::LowerInnerAV) << std::endl;
    std::cout << dir.X() << " " << dir.Y() << " " << dir.Z() << std::endl;
    std::cout << SWdistInUpperTarget << " " << SWdistInWater << " " << path2.GetTotalDist() << " " <<  path2.GetPointCount() << std::endl;
   
  }
}
