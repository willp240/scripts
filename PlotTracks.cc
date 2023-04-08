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
TH1D* plotTrackLength( const std::string& fileName )
{
  gStyle->SetOptStat(111111); // to show overflow / underflow stats
  // The process summary flag:
  RAT::DS::MCTrack::ESummaryFlag flag = RAT::DS::MCTrack::OpReemission;
  TH1D* hTrackLengths = new TH1D( "hTrackLengths", "MC track length", 600, 0, 6000);

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
          if( mcTrack.GetSummaryFlag( flag ) )
            hTrackLengths->Fill( mcTrack.GetLength() );
        }
    }
  hTrackLengths->SetXTitle( "Track Length (mm)" );
  hTrackLengths->SetYTitle( "Counts per 10 mm" );
  hTrackLengths->Draw();
  return hTrackLengths;
}


/// Plot the energy deposited for neutron capture track steps.
///
/// See RAT/include/MCTrackStep.hh for other possible track step processes.
///
/// @param[in] fileName of the RAT::DS root file to analyse
/// @return the histogram plot
TH1D* plotEdepStep( const std::string& fileName )
{
  // The process to cut on:
  std::string process = "nCapture";

  TH1D* hEdepStep = new TH1D( "hEdepStep", "Energy deposited", 500, 0, 5);
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
          const RAT::DS::MCTrack& mcTrack = mc.GetMCTrack( mcTrackIDs.at(iTrack) );
          for( size_t iStep = 0; iStep < mcTrack.GetMCTrackStepCount(); iStep++ )
            {
              const RAT::DS::MCTrackStep& mcTrackStep = mcTrack.GetMCTrackStep( iStep );
              // Note: it is possible to get processes either by name (string) or by enum.
              if( mcTrackStep.GetProcess() == process )
                hEdepStep->Fill( mcTrackStep.GetDepositedEnergy() );
            }
        }
    }
  hEdepStep->SetXTitle( "Energy deposited (MeV)" );
  hEdepStep->SetYTitle( "Counts per 10 keV" );
  hEdepStep->Draw();
  return hEdepStep;
}


/// Plot the final kinetic energy for Compton scattering track steps.
///
/// See RAT/include/MCTrackStep.hh for other possible track step processes.
///
/// @param[in] fileName of the RAT::DS root file to analyse
/// @return the histogram plot
TH1D* plotKEStep( const std::string& fileName )
{
  // The process to cut on:
  RAT::DS::MCTrackStep::EProcess process = RAT::DS::MCTrackStep::compt;

  TH1D* hKEStep = new TH1D( "hKEStep", "Kinetic energy", 500, 0, 0.5);
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
          const RAT::DS::MCTrack& mcTrack = mc.GetMCTrack( mcTrackIDs.at(iTrack) );
          for( size_t iStep = 0; iStep < mcTrack.GetMCTrackStepCount(); iStep++ )
            {
              const RAT::DS::MCTrackStep& mcTrackStep = mcTrack.GetMCTrackStep( iStep );
              // Note: it is possible to get processes either by name (string) or by enum.
              if( mcTrackStep.GetProcessEnum() == process )
                hKEStep->Fill( mcTrackStep.GetKineticEnergy() );
            }
        }
    }
  hKEStep->SetXTitle( "Energy (MeV)" );
  hKEStep->SetYTitle( "Counts per 1 keV" );
  hKEStep->Draw();
  return hKEStep;
}


TH1D* plotEndPos( const std::string& fileName )
{
  gStyle->SetOptStat(111111); // to show overflow / underflow stats
  // The process summary flag:
  RAT::DS::MCTrack::ESummaryFlag flag = RAT::DS::MCTrack::OpReemission;
  TH1D* hTrackEndX = new TH1D( "hTrackEndX", "MC track end point X", 12000, -6000, 6000);
  TH1D* hTrackEndY = new TH1D( "hTrackEndY", "MC track end point Y", 12000, -6000, 6000);
  TH1D* hTrackEndZ = new TH1D( "hTrackEndZ", "MC track end point Z", 12000, -6000, 6000);

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
          //if( mcTrack.GetSummaryFlag( flag ) ){
	    if( mcTrack.GetPDGCode() == 13 ){
	      hTrackEndX->Fill( mcTrack.GetLastMCTrackStep().GetPosition().x() );
	      hTrackEndY->Fill( mcTrack.GetLastMCTrackStep().GetPosition().y() );
	      hTrackEndZ->Fill( mcTrack.GetLastMCTrackStep().GetPosition().z() );
	      std::cout << mcTrack.GetLastMCTrackStep().GetPosition().x() << " " << mcTrack.GetLastMCTrackStep().GetPosition().y() << " " << mcTrack.GetLastMCTrackStep().GetPosition().z() << " " << mcTrack.GetLastMCTrackStep().GetDepositedEnergy() << std::endl;
	    }
	    //}
        }
    }
  hTrackEndX->SetXTitle( "Track End X (mm)" );
  hTrackEndX->SetYTitle( "Counts" );
  hTrackEndX->Draw();
  hTrackEndY->SetXTitle( "Track End Y (mm)" );
  hTrackEndY->SetYTitle( "Counts" );
  hTrackEndY->Draw();
  hTrackEndZ->SetXTitle( "Track End Z (mm)" );
  hTrackEndZ->SetYTitle( "Counts" );
  //  hTrackEndZ->Draw();
  return hTrackEndX;
}
