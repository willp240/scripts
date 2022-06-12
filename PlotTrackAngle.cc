////////////////////////////////////////////////////////////////////
/// \file PlotTracks.cc
///
/// \brief Functions to select and plot track features
///
/// \author M Mottram <m.mottram@qmul.ac.uk>
///
/// REVISION HISTORY:\n
///     2014-04-02: M Mottram - First Revision.\n
///
/// \details Functions to plot track features
///
////////////////////////////////////////////////////////////////////

#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/MCTrack.hh>
#include <RAT/DS/MCTrackStep.hh>
#include <RAT/DB.hh>

#include <TH1D.h>
#include <TStyle.h>

#include <string>

/// Plot the length of tracks that underwent reemission.
///
/// See RAT/include/MCTrack.hh ESummaryFlag for other processes.
///
/// @param[in] fileName of the RAT::DS root file to analyse
/// @return the histogram plot
TH1D* plotTrackAngle( const std::string& fileName )
{
  gStyle->SetOptStat(111111); // to show overflow / underflow stats
  // The process summary flag:
  RAT::DS::MCTrack::ESummaryFlag flag = RAT::DS::MCTrack::OpReemission;
  TH1D* hTrackAngles = new TH1D( "hTrackLengths", "MC track cos angle", 200, -1, 1);

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );
  for( size_t iEntry = 0; iEntry < dsReader.GetEntryCount(); iEntry++ )
    {
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      const RAT::DS::MC& mc = rDS.GetMC();
      std::vector<unsigned int> mcTrackIDs = mc.GetMCTrackIDs();
      for( size_t iTrack = 0; iTrack < mcTrackIDs.size(); iTrack++ )
        {
          const RAT::DS::MCTrack& mcTrack = mc.GetMCTrack( mcTrackIDs.at(iTrack));
	  //          if( mcTrack.GetSummaryFlag( flag ) )
	  const RAT::DS::MCTrackStep& mcTrackStepFirst = mcTrack.GetFirstMCTrackStep();
	  const RAT::DS::MCTrackStep& mcTrackStepLast = mcTrack.GetLastMCTrackStep();
	  
	  TVector3 startVec = mcTrackStepFirst.GetMomentum();
	  TVector3 endVec = mcTrackStepLast.GetPosition();

	  std::cout << startVec.X() << " " << startVec.Y() << " " << startVec.Z() << std::endl;
	  std::cout << endVec.X() << " " << endVec.Y() << " " << endVec.Z() << std::endl;

	  double angle = startVec.Angle(endVec);
	  hTrackAngles->Fill(cos(angle));
	  std::cout << cos(angle) << std::endl << std::endl;
        }
    }
  hTrackAngles->SetXTitle( "cos(Track Angle)" );
  hTrackAngles->SetYTitle( "Counts " );
  hTrackAngles->Draw();
  return hTrackAngles;
}
