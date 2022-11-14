#include <TVector3.h>
#include <CutConfigLoader.hh>
#include <IOConfigLoader.hh>
//#include <EventHistograms.hh>
//#include <CoincHistograms.hh>
#include <OutputFiles.hh>
#include <HistsCollection.hh>
#include <iostream>
#include <array>
#include <algorithm>

int main( int argc, char **argv ){

  if (argc != 2) {
    std::cout << "Syntax is $: test config.cfg" << std::endl;
    std::cout << "Where config.cfg is a valid config file" << std::endl;
    exit(-1);
  }
  // Get Config File Name
  const std::string cfgname = argv[1];

  // Read IO info
  IOConfigLoader ioConfLoader( cfgname );
  std::string inFileName = ioConfLoader.LoadInputFilePath();
  std::string outFileBaseName = ioConfLoader.LoadOutputFileName();
  bool saveHistos = ioConfLoader.LoadSaveHistosBool();
  bool debugMode = ioConfLoader.LoadDebugModeBool();

  CutConfigLoader cutConfLoader( cfgname );
  CutCollection cutCol = cutConfLoader.LoadCutCollection();

  // Open input file
  InputFile inFile( inFileName );
  int nEntries = inFile.GetNumEvents();

  // For each cut, open output files
  OutputFiles outfiles( "testout", inFile, ioConfLoader.GetString(), cutCol );

  HistsCollection histsCol( cutCol, cfgname );

  // For checking if in muon follower time
  ULong64_t currentTime;
  int currentEvent;

  // Now loop backwards over events
  for( int iEntry = nEntries-1; iEntry >= 0; iEntry-- ){

    std::cout << "Event " << iEntry << " of " << nEntries << std::endl;

    // Get the event
    AnaEvent eve = inFile.GetEvent( iEntry );

    if(debugMode){
      std::cout << "Delay Candiate " << iEntry << std::endl;
      eve.PrintEvent();
    }

    if( iEntry == nEntries-1 ) {
	currentTime = eve.GetTime();
	currentEvent = nEntries-1;
    }
    
    // Now check there hasn't been a muon event in proceeding N nanoseconds where N is set as followertime from muon cut
    // This is complicated slightly because we're looping backwards, so now have to loop over events we've not looked at yet
    // Use global "current" event and time variables so we're not repeating checking the same proceeding events
    ULong64_t cutTime = eve.GetTime() - cutCol.GetMuonCut(0).GetFollowerTime(); // This is the time we need to check events until
    while ( (currentTime & 0x7FFFFFFFFFF)  > (cutTime & 0x7FFFFFFFFFF) && currentEvent > 0 ){
      currentEvent--;
      AnaEvent currEve = inFile.GetEvent( currentEvent );
      currentTime = currEve.GetTime();
      if( currEve.GetNhit() > cutCol.GetMuonCut(0).GetNhitMax() ){
	cutCol.SetMuonCurrentVals(0, true);
	if( saveHistos )
	  histsCol.GetMuonHistsVec().at( 0 ).Fill( currEve, false );
	iEntry = currentEvent;
	break;
      }
    }

    // If we've detected a Muon in the proceeding window, move along
    if( cutCol.GetMuonCurrentVals(0) )
      continue;
    if( saveHistos )
      histsCol.GetMuonHistsVec().at( 0 ).Fill( eve, true );

    cutCol.PassesAnaCuts( eve );
    cutCol.PassesDelayCuts( eve );

    if(saveHistos) {
      histsCol.FillAnaHists( eve );
      histsCol.FillCoincAllHists( eve );
    }

    // If any delay cut is passed, let's look for prompt candidates
    if( cutCol.PassedAnyDelayCuts() ){

      // Loop over subsequent events
      int counter = 1;
      while ( iEntry-counter >= 0 ) {

	AnaEvent promptCandidate = inFile.GetEvent( iEntry - counter );
	if(debugMode){
	  std::cout << "Prompt Candiate " << iEntry - counter << std::endl;
	  promptCandidate.PrintEvent();
	}

	CoincPair coincCandidate(promptCandidate, eve );

	cutCol.PassesCoincCuts( coincCandidate );

	for( int iCoincCut = 0; iCoincCut < cutCol.GetNCoincCuts(); iCoincCut++ ){
	  if( cutCol.GetCoincCurrentVals( iCoincCut ) )
	    outfiles.WriteCoincidenceEvents( iCoincCut, iEntry - counter, iEntry, coincCandidate);
	}

	if( saveHistos )
	  histsCol.FillCoincTaggedHists( coincCandidate );
	  //coincHistsVec.at( iCoincCut ).FillTagged( coincCandidate );
	counter++;
      } // End while loop over prompt candidates
      if( cutCol.PassedAllCoincCuts() ){
	for( int iAnaCut = 0; iAnaCut < cutCol.GetNAnaCuts(); iAnaCut++ ){
	  if( cutCol.GetAnaCurrentVals( iAnaCut ) )
	    outfiles.WriteAnalysisEvents( iAnaCut, iEntry, eve );
	}
      }
    } // End if delay candidate
  } // End loop over events

  if( saveHistos )
    histsCol.Write();

  outfiles.WriteAndCloseFiles();

  return 0;
}
