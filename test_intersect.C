#include <string>
#include <iostream>

#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/EV.hh>

void test_intersect()
{

  TVector3 pos(-859.956, 5305.9, 6584.67);
  TVector3 dir(-0.210306, -0.170792, -0.962601);

  double udDotVtx = pos.X()*dir.X() + pos.Y()*dir.Y() + pos.Z()*dir.Z(); // (P-X0)/|P-X0| * (X0 - oAV)
  double vtx2 = pos.X()*pos.X() + pos.Y()*pos.Y() + pos.Z()*pos.Z(); // (X0 - oAV)*(X0 - oAV)
  double rVertex = sqrt( vtx2 ); // Roffset = |X0 - oAV|
  double sqrVal = udDotVtx*udDotVtx - vtx2 + 6000*6000;
  
  if( sqrVal>0 ) { // line passes AV sphere
    /// find the line-sphere intersect points; a1, a2 are the path lengths btw vertex and intersection points
    double a1 = 0, a2 = 0, aplus = 0, abig = 0, asmall = 0, scintpathInAV = 0;
    a1 = -udDotVtx + sqrt(sqrVal);
    a2 = -udDotVtx - sqrt(sqrVal); // a2<a1
    
    if ( a1*a2<0 ) {
      aplus = a1; // a1>0>a2
      scintpathInAV = aplus;
    }
    
    else { // rVertex>=fAVRadiusOuter, vertex in external
      if( a1>0 && a2>0) {
	abig = a1; // far intersection point
	asmall = a2; // near intersection point
	scintpathInAV = abig - asmall;
      } // ensure a1 and a2 are positive
    }
    std::cout << "dist in scint " << scintpathInAV << std::endl;
  }
}
