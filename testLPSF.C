

using namespace RAT;
using namespace RAT::DS;

void testLPSF(std::string filename)
{

  RAT::DU::DSReader dsreader(filename);

  // Loop through events
  for(int i=0; i<1; i++){
    
    std::cout << "event " << i << std::endl;
  
    TVector3 mcPos(0,0,4000);
    TVector3 pmtPos(0,0,-8000);
    TVector3 dir = mcPos-pmtPos;

    double distInUpperTarget1 = 0.0;
    double distInAV1 = 0.0;
    double distInWater1 = 0.0;

    //Full Fit
    TStopwatch FullFit;
    FullFit.Start();
    RAT::DU::Point3D pStart1(RAT::DU::Point3D::GetSystemId("av"), mcPos.X(), mcPos.Y(), mcPos.Z() );
    RAT::DU::Point3D pEnd1(RAT::DU::Point3D::GetSystemId("av"), pmtPos.X(), pmtPos.Y(), pmtPos.Z() );
    RAT::DU::LightPathStraightScintWater fit1;
    RAT::DU::LightPath startPath1(3.0, pStart1, pEnd1, RAT::DU::LightPath::UpperInnerAV, RAT::DU::LightPath::PMT);
    fit1.Fit(startPath1);
    distInAV1=startPath1.GetDistByRegionType(RAT::DU::LightPath::AV);;
    distInWater1=startPath1.GetDistByRegionType(RAT::DU::LightPath::Water);
    distInUpperTarget1=startPath1.GetDistByRegionType(RAT::DU::LightPath::UpperInnerAV);
    FullFit.Stop();

    ///Make point3ds
    TStopwatch PointTimer;
    PointTimer.Start();
    RAT::DU::Point3D pStart(RAT::DU::Point3D::GetSystemId("av"), mcPos.X(), mcPos.Y(), mcPos.Z() );
    RAT::DU::Point3D pEnd(RAT::DU::Point3D::GetSystemId("av"), pmtPos.X(), pmtPos.Y(), pmtPos.Z() );
    PointTimer.Stop();

    ///Make LightPath
    TStopwatch LightPathTimer;
    LightPathTimer.Start();
    RAT::DU::LightPath startPath(3.0, pStart, pEnd, RAT::DU::LightPath::UpperInnerAV, RAT::DU::LightPath::PMT);
    LightPathTimer.Stop();

    ///Get Vector
    TStopwatch VectorTimer;
    VectorTimer.Start();
    RAT::DU::Point3D startPoint = startPath.GetPoint(0);
    RAT::DU::Point3D endPoint = startPath.GetEndPos();
    TVector3 startVec(startPoint.X(), startPoint.Y(), startPoint.Z());
    TVector3 endVec(endPoint.X(), endPoint.Y(), endPoint.Z());
    VectorTimer.Stop();

    //Norm Diff
    TStopwatch UnitVecTimer;
    UnitVecTimer.Start();
    TVector3 dir = mcPos-pmtPos;
    dir=dir.Unit();
    UnitVecTimer.Stop();

    ///Run algorithm
    TStopwatch GetPathTimer;
    GetPathTimer.Start();
    double distInUpperTarget = 0.0;
    double distInAV = 0.0;
    double distInWater = 0.0;
    RAT::DU::LightPathStraightScintWater::GetPath(endVec, startVec, distInUpperTarget, distInWater);
    GetPathTimer.Stop();

    ///Add Point
    TStopwatch InsertTimer;
    InsertTimer.Start();
    startPath.InsertPoint(startPath.GetPointCount()-1, startPoint.GetNewPoint(distInUpperTarget*dir), RAT::DU::LightPath::Water);
    InsertTimer.Stop();

    ///Calc Distances
    TStopwatch DistancesTimer;
    DistancesTimer.Start();
    distInAV=startPath.GetDistByRegionType(RAT::DU::LightPath::AV);;
    distInWater=startPath.GetDistByRegionType(RAT::DU::LightPath::Water);
    distInUpperTarget=startPath.GetDistByRegionType(RAT::DU::LightPath::UpperInnerAV);
    DistancesTimer.Stop();

    ///Declare Fitter
    TStopwatch DeclareTimer;
    DeclareTimer.Start();
    RAT::DU::LightPathStraightScintWater fit;
    DeclareTimer.Stop();
    RAT::DU::LightPath startPath2(3.0, pStart, pEnd, RAT::DU::LightPath::UpperInnerAV, RAT::DU::LightPath::PMT);   
    
    ///Fit
    TStopwatch FitTimer;
    FitTimer.Start();
    fit.Fit(startPath2);
    FitTimer.Stop();
    std::cout << std::fixed << std::setprecision(8);
    std::cout << "FullFit\t\t" << FullFit.RealTime() << std::endl;
    std::cout << "PointTimer\t" << PointTimer.RealTime() << std::endl;
    std::cout << "LightPathTimer\t" << LightPathTimer.RealTime() << std::endl;
    std::cout << "VectorTimer\t" << VectorTimer.RealTime() << std::endl;
    std::cout << "UnitVecTimer\t" << UnitVecTimer.RealTime() << std::endl;
    std::cout << "GetPathTimer\t" << GetPathTimer.RealTime() << std::endl;
    std::cout << "InsertTimer\t" << InsertTimer.RealTime() << std::endl;
    std::cout << "DistancesTimer\t" << DistancesTimer.RealTime() << std::endl;
    std::cout << "DeclareTimer\t" << DeclareTimer.RealTime() << std::endl;
    std::cout << "FitTimer\t" << FitTimer.RealTime() << std::endl;
  }
}
