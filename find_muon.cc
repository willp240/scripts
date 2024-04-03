#include <string>
#include <iostream>

#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/EV.hh>

int main( int argc, char **argv )
{

  const std::string fname = argv[1];

  RAT::DU::DSReader dsReader( fname );

  RAT::DU::DataCleaningBits rDataCleaningBits = RAT::DU::Utility::Get()->GetDataCleaningBits(); // To get the data cleaning bits

  // Loop through entries in rootfile
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ ){
    const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );

    // Look at each triggered event in the entry
    for( size_t iEV = 0; iEV < rDS.GetEVCount(); iEV++ ){
      const RAT::DS::EV& ev = rDS.GetEV( iEV );
      //std::cout << rDS.GetRunID() << " " <<  ev.GetGTID() << " " << ev.GetNhits() << " " << iEntry << std::endl;
      // if event was tagged as muon, run muon fitting routines
      const RAT::DS::DataQCFlags& dcFlags = ev.GetDataCleaningFlags();
      const RAT::DU::DataCleaningBits& dcBits = RAT::DU::Utility::Get()->GetDataCleaningBits();

      bool is_muon = false;
      Int_t dcPassNumber = dcFlags.GetLatestPass();
      if( dcFlags.ExistFlags( dcPassNumber ))
	is_muon = (dcFlags.GetApplied(dcPassNumber).Get(dcBits.GetBitIndex( "muontag" )) &&
		   !dcFlags.GetFlags(dcPassNumber).Get(dcBits.GetBitIndex( "muontag" )));
      
      if(is_muon){

	RAT::DS::FitResult fitResult = ev.GetFitResult( "scintFitter" );
        RAT::DS::FitVertex fitVertex = fitResult.GetVertex(0);

	double udDotVtx = fitVertex.GetPosition().X()*fitVertex.GetDirection().X() + fitVertex.GetPosition().Y()*fitVertex.GetDirection().Y() + fitVertex.GetPosition().Z()*fitVertex.GetDirection().Z(); // (P-X0)/|P-X0| * (X0 - oAV)
	double vtx2 = fitVertex.GetPosition().X()*fitVertex.GetPosition().X() + fitVertex.GetPosition().Y()*fitVertex.GetPosition().Y() + fitVertex.GetPosition().Z()*fitVertex.GetPosition().Z(); // (X0 - oAV)*(X0 - oAV)
	double rVertex = sqrt( vtx2 ); // Roffset = |X0 - oAV|
	double sqrVal = udDotVtx*udDotVtx - vtx2 + 6000*6000;
	if( sqrVal>0 ){

	  std::cout << "intersects sphere ok " << std::endl;
	  std::cout << "************************************" << std::endl;
	  std::cout << "************************************" << std::endl;
	  std::cout << "************************************" << std::endl;
	  std::cout << "************************************" << std::endl;
	  std::cout << "************************************" << std::endl;
	  std::cout << ev.GetGTID() << " " << ev.GetNhits() << " " << iEntry << std::endl;
	  std::cout << "Pos " << fitVertex.GetPosition().X() << " " << fitVertex.GetPosition().Y() << " " << fitVertex.GetPosition().Z() << std::endl;
	  std::cout << "Dir " << fitVertex.GetDirection().X() << " " << fitVertex.GetDirection().Y() << " " << fitVertex.GetDirection().Z() << std::endl;

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
	  std::cout << "************************************" << std::endl;
	  std::cout << "************************************" << std::endl << std::endl;;
	}
      }
    }
  }
  
  return 0;
}
