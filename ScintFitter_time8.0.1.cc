#include <RAT/ScintFitter.hh>
#include <RAT/MethodFactory.hh>
#include <RAT/PDFFactory.hh>
#include <RAT/OptimiserFactory.hh>
#include <RAT/ClassifierFactory.hh>
#include <RAT/PMTSelectorFactory.hh>
#include <RAT/Method.hh>
#include <RAT/Classifier.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/DS/FitVertex.hh>
#include <RAT/OptimisedClassifier.hh>
#include <RAT/OptimisedMethod.hh>
#include <RAT/PDFMethod.hh>
#include <RAT/SeededMethod.hh>
#include <RAT/SelectorMethod.hh>
#include <RAT/SelectorClassifier.hh>
#include <RAT/Optimiser.hh>
#include <RAT/SeededClassifier.hh>
#include <RAT/MetaInformation.hh>
#include <RAT/Log.hh>
#include <RAT/DB.hh>
#include <RAT/ListHelp.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/DataNotFound.hh>
#include <RAT/DS/DataQCFlags.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/DataCleaningBits.hh>
#include <RAT/GLG4StringUtil.hh>
using namespace RAT;
using namespace RAT::Methods;
using namespace RAT::PDFs;
using namespace RAT::Optimisers;
using namespace RAT::PMTSelectors;
using namespace RAT::DS;
using namespace RAT::Classifiers;

#include <sstream>

#include <TStopwatch.h>
using namespace ROOT;

ScintFitter::ScintFitter()
  : Processor("scintFitter")
{
  fQuadSeed = MethodFactory::Get()->GetMethod( "quad" );
  fCutOff = 3.0; // Maximum nhit for which quad cannot run
  fPositionTime = MethodFactory::Get()->GetMethod( "multipdf-scint" );
  fTrackMethod = MethodFactory::Get()->GetMethod( "mCPTrack" );
  fTrackSeed = MethodFactory::Get()->GetMethod( "TrackSeed" );
  fMuonWater = MethodFactory::Get()->GetMethod( "MuonWater" );
  fMuonFitter = MethodFactory::Get()->GetMethod( "MuonFitter" );
  fPositionANN = MethodFactory::Get()->GetMethod( "positionANN" );
  fPowell = OptimiserFactory::Get()->GetOptimiser( "powell" );
  fNullSelector = PMTSelectorFactory::Get()->GetPMTSelector( "null" );
  fPMTCalSelector = PMTSelectorFactory::Get()->GetPMTSelector( "PMTCalSelector" );
  fET1D = PDFFactory::Get()->GetPDF( "et1d" );
  fEnergyMethod = MethodFactory::Get()->GetMethod( "energyRThetaFunctional" );
  fTimeResidualCut = PMTSelectorFactory::Get()->GetPMTSelector( "timeResidualCut" );
  fIsotropy = ClassifierFactory::Get()->GetClassifier( "isotropy" );
  fITR = ClassifierFactory::Get()->GetClassifier( "ITR" );
  fXSite = ClassifierFactory::Get()->GetClassifier( "XSite" );
  fQPDT = ClassifierFactory::Get()->GetClassifier( "QPDT" );
  fTimingPeaks = ClassifierFactory::Get()->GetClassifier( "timingPeaks" );
  fMeanTime = ClassifierFactory::Get()->GetClassifier( "meanTime" );
  fPreTriggerHits = ClassifierFactory::Get()->GetClassifier( "preTriggerHits" );
  fIsoRegions = ClassifierFactory::Get()->GetClassifier( "isoRegions" );
  fEarlyTime = ClassifierFactory::Get()->GetClassifier( "earlyTime" );
  fBiPoCumulTimeResid = ClassifierFactory::Get()->GetClassifier( "BiPoCumulTimeResid" );
  fBiPo212LikelihoodDiff = ClassifierFactory::Get()->GetClassifier( "BiPoLikelihoodDiff-212" );
  fBiPo214LikelihoodDiff = ClassifierFactory::Get()->GetClassifier( "BiPoLikelihoodDiff-214" );
  fBiPoLikelihoodDiffOptimiser = OptimiserFactory::Get()->GetOptimiser( "grid-55" );
  fBiPo212Classifier = ClassifierFactory::Get()->GetClassifier("AlphaBetaClassifier-212_wPSD");
  fBiPo212Classifier->SetI("nhit_cut", 400);
  fBiPo214Classifier = ClassifierFactory::Get()->GetClassifier("AlphaBetaClassifier-214_wPSD");
  fBiPo214Classifier->SetI("nhit_cut", 400);
  fGridOptimiser = OptimiserFactory::Get()->GetOptimiser("grid-64");
  fBerkeleyAlphaBeta = ClassifierFactory::Get()->GetClassifier("BerkeleyAlphaBeta");
  fBerkeleyAlphaBetaFlasher = ClassifierFactory::Get()->GetClassifier("BerkeleyAlphaBeta-BERKELEY_FB_PDF");
  fMuonClassifier = ClassifierFactory::Get()->GetClassifier( "muon" );
  fNearAVAngular = ClassifierFactory::Get()->GetClassifier( "nearAVAngular" );
  fExt0NuCosThetaTl208AV = ClassifierFactory::Get()->GetClassifier("Ext0NuCosTheta-Tl208AV");
  fExt0NuCosThetaTl208Exwater = ClassifierFactory::Get()->GetClassifier("Ext0NuCosTheta-Tl208Exwater");
  fExt0NuCosThetaTl208Hdropes = ClassifierFactory::Get()->GetClassifier("Ext0NuCosTheta-Tl208Hdropes");
  fExt0NuCosThetaPMTbg = ClassifierFactory::Get()->GetClassifier("Ext0NuCosTheta-PMTbetagamma");
  fMultisiteTimeResTl208AV = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208AV");
  fMultisiteTimeResTl208Exwater = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208Exwater");
  fMultisiteTimeResTl208Hdropes = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208Hdropes");
  fMultisiteTimeResPMTbg = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-PMTbetagamma");
  fMultisiteTimeResNaiveTl208AV = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208AV_NAIVE");
  fMultisiteTimeResNaiveTl208Hdropes = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208Hdropes_NAIVE");
  fMultisiteTimeResNaiveTl208Exwater = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208Exwater_NAIVE");
  fMultisiteTimeResNaivePMTbg = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-PMTbetagamma_NAIVE");
  fMultisiteTimeResOffsetTl208AV = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208AV_OFFSET10");
  fMultisiteTimeResOffsetTl208Hdropes = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208Hdropes_OFFSET10");
  fMultisiteTimeResOffsetTl208Exwater = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208Exwater_OFFSET10");
  fMultisiteTimeResOffsetPMTbg = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-PMTbetagamma_OFFSET10");
  fMultisiteTimeResBi214 = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Bi214_NAIVE");
  fMultisiteTimeResCo60 = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Co60_NAIVE");
  fMultisiteTimeResBi210Bi214 = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Bi210Bi214_NAIVE");
  fMultisiteTimeResTl208 = ClassifierFactory::Get()->GetClassifier("MultisiteTimeRes-Tl208_NAIVE");
  fAlphaNReactorIBD = ClassifierFactory::Get()->GetClassifier("AlphaNReactorIBDClassifier");
  // Now combine the components where appropriate
  dynamic_cast< SelectorClassifier* >( fIsotropy )->AddPMTSelector( fPMTCalSelector );
  dynamic_cast< SelectorClassifier* >( fIsotropy )->AddPMTSelector( fTimeResidualCut );
  dynamic_cast< OptimisedClassifier* >( fBiPo212LikelihoodDiff )->SetOptimiser( fBiPoLikelihoodDiffOptimiser );
  dynamic_cast< OptimisedClassifier* >( fBiPo214LikelihoodDiff )->SetOptimiser( fBiPoLikelihoodDiffOptimiser );
  dynamic_cast< OptimisedClassifier* >( fBiPo212Classifier )->SetOptimiser(fGridOptimiser);
  dynamic_cast< OptimisedClassifier* >( fBiPo214Classifier )->SetOptimiser(fGridOptimiser);
}

void ScintFitter::SetI(const std::string& param, const int value)
{
  std::vector<std::string> parts = util_split( param, "."  );
  if( parts.size() != 2 )
    throw Processor::ParamUnknown( param );
  if( parts[0] == std::string( "alphaBeta212" ) )
    {
      fBiPo212Classifier->SetI(parts[1], value);
    }
  else if(parts[0] == std::string( "alphaBeta214" ))
    {
      fBiPo214Classifier->SetI(parts[1], value);
    }
  else if(parts[0] == std::string( "mCP" ))
    {
      fTrackMethod->SetI(parts[1], value);
    }
  else
    throw Processor::ParamUnknown( param );
}

void ScintFitter::SetD(const std::string& param, const double value)
{
  std::vector<std::string> parts = util_split( param, "."  );
  if( parts.size() != 2 )
    throw Processor::ParamUnknown( param );
  if(parts[0] == std::string( "mCP" ))
    {
      fTrackMethod->SetD(parts[1], value);
    }
  if(parts[0] == std::string( "TrackSeed" ))
    {
      fTrackSeed->SetD(parts[1], value);
    }
  else
    throw Processor::ParamUnknown( param );
}

ScintFitter::~ScintFitter()
{
  delete fQuadSeed;
  delete fPositionTime;
  delete fTrackMethod;
  delete fTrackSeed;
  delete fPowell;
  delete fNullSelector;
  delete fET1D;
  delete fEnergyMethod;
  delete fIsotropy;
  delete fTimeResidualCut;
  delete fITR;
  delete fXSite;
  delete fQPDT;
  delete fTimingPeaks;
  delete fMeanTime;
  delete fPreTriggerHits;
  delete fIsoRegions;
  delete fEarlyTime;
  delete fBiPoCumulTimeResid;
  delete fBiPo212LikelihoodDiff;
  delete fBiPo214LikelihoodDiff;
  delete fBiPoLikelihoodDiffOptimiser;
  delete fBiPo212Classifier;
  delete fBiPo214Classifier;
  delete fBerkeleyAlphaBeta;
  delete fBerkeleyAlphaBetaFlasher;
  delete fGridOptimiser;
  delete fMuonClassifier;
  delete fMuonWater;
  delete fMuonFitter;
  delete fExt0NuCosThetaTl208AV;
  delete fExt0NuCosThetaTl208Hdropes;
  delete fExt0NuCosThetaTl208Exwater;
  delete fExt0NuCosThetaPMTbg;
  delete fMultisiteTimeResTl208AV;
  delete fMultisiteTimeResTl208Hdropes;
  delete fMultisiteTimeResTl208Exwater;
  delete fMultisiteTimeResPMTbg;
  delete fMultisiteTimeResNaiveTl208AV;
  delete fMultisiteTimeResNaiveTl208Hdropes;
  delete fMultisiteTimeResNaiveTl208Exwater;
  delete fMultisiteTimeResNaivePMTbg;
  delete fMultisiteTimeResOffsetTl208AV;
  delete fMultisiteTimeResOffsetTl208Hdropes;
  delete fMultisiteTimeResOffsetTl208Exwater;
  delete fMultisiteTimeResOffsetPMTbg;
  delete fMultisiteTimeResBi214;
  delete fMultisiteTimeResCo60;
  delete fMultisiteTimeResBi210Bi214;
  delete fMultisiteTimeResTl208;
  delete fAlphaNReactorIBD;
}

void
ScintFitter::BeginOfRun( DS::Run& run )
{
  // First, call the individual begin-of-run functions
  fQuadSeed->BeginOfRun( run );
  fPositionTime->BeginOfRun( run );
  fTrackMethod->BeginOfRun( run );
  fPositionANN->BeginOfRun( run );
  fPowell->BeginOfRun( run );
  fNullSelector->BeginOfRun( run );
  fET1D->BeginOfRun( run );
  fEnergyMethod->BeginOfRun( run );
  fTimeResidualCut->BeginOfRun( run );
  fIsotropy->BeginOfRun( run );
  fITR->BeginOfRun( run );
  fXSite->BeginOfRun( run );
  fQPDT->BeginOfRun( run );
  fTimingPeaks->BeginOfRun( run );
  fMeanTime->BeginOfRun( run );
  fPreTriggerHits->BeginOfRun( run );
  fIsoRegions->BeginOfRun( run );
  fEarlyTime->BeginOfRun( run );
  fBiPoCumulTimeResid->BeginOfRun( run );
  fBiPo212LikelihoodDiff->BeginOfRun( run );
  fBiPo214LikelihoodDiff->BeginOfRun( run );
  fBiPo212Classifier->BeginOfRun( run );
  fBiPo214Classifier->BeginOfRun( run );
  fBerkeleyAlphaBeta->BeginOfRun( run );
  fBerkeleyAlphaBetaFlasher->BeginOfRun( run );
  fMuonClassifier->BeginOfRun( run );
  fMuonWater->BeginOfRun( run );
  fMuonFitter->BeginOfRun( run );
  fNearAVAngular->BeginOfRun( run );
  fExt0NuCosThetaTl208AV->BeginOfRun( run );
  fExt0NuCosThetaTl208Hdropes->BeginOfRun( run );
  fExt0NuCosThetaTl208Exwater->BeginOfRun( run );
  fExt0NuCosThetaPMTbg->BeginOfRun( run );
  fMultisiteTimeResTl208AV->BeginOfRun( run );
  fMultisiteTimeResTl208Hdropes->BeginOfRun( run );
  fMultisiteTimeResTl208Exwater->BeginOfRun( run );
  fMultisiteTimeResPMTbg->BeginOfRun( run );
  fMultisiteTimeResNaiveTl208AV->BeginOfRun( run );
  fMultisiteTimeResNaiveTl208Hdropes->BeginOfRun( run );
  fMultisiteTimeResNaiveTl208Exwater->BeginOfRun( run );
  fMultisiteTimeResNaivePMTbg->BeginOfRun( run );
  fMultisiteTimeResOffsetTl208AV->BeginOfRun( run );
  fMultisiteTimeResOffsetTl208Hdropes->BeginOfRun( run );
  fMultisiteTimeResOffsetTl208Exwater->BeginOfRun( run );
  fMultisiteTimeResOffsetPMTbg->BeginOfRun( run );
  fMultisiteTimeResBi214->BeginOfRun( run );
  fMultisiteTimeResCo60->BeginOfRun( run );
  fMultisiteTimeResBi210Bi214->BeginOfRun( run );;
  fMultisiteTimeResTl208->BeginOfRun( run );
  fAlphaNReactorIBD->BeginOfRun( run );
}

Processor::Result
ScintFitter::DSEvent( DS::Run& run,
                      DS::Entry& ds )
{
  for( size_t iEV = 0; iEV < ds.GetEVCount(); iEV++ )
    Event( run, ds.GetEV( iEV ) );
  return OK;
}

void
ScintFitter::EndOfRun( DS::Run& run )
{
  // Finally ,call the individual end-of-run functions
  fQuadSeed->EndOfRun( run );
  fPositionTime->EndOfRun( run );
  fTrackMethod->EndOfRun( run );
  fPositionANN->EndOfRun( run );
  fPowell->EndOfRun( run );
  fNullSelector->EndOfRun( run );
  fET1D->EndOfRun( run );
  fEnergyMethod->EndOfRun( run );
  fTimeResidualCut->EndOfRun( run );
  fIsotropy->EndOfRun( run );
  fITR->EndOfRun( run );
  fXSite->EndOfRun( run );
  fQPDT->EndOfRun( run );
  fTimingPeaks->EndOfRun( run );
  fMeanTime->EndOfRun( run );
  fPreTriggerHits->EndOfRun( run );
  fIsoRegions->EndOfRun( run );
  fEarlyTime->EndOfRun( run );
  fBiPoCumulTimeResid->EndOfRun( run );
  fBiPo212LikelihoodDiff->EndOfRun( run );
  fBiPo214LikelihoodDiff->EndOfRun( run );
  fBiPo212Classifier->EndOfRun( run );
  fBiPo214Classifier->EndOfRun( run );
  fBerkeleyAlphaBeta->EndOfRun( run );
  fBerkeleyAlphaBetaFlasher->EndOfRun( run );
  fMuonClassifier->EndOfRun( run );
  fMuonClassifier->EndOfRun( run );
  fMuonWater->EndOfRun( run );
  fMuonFitter->EndOfRun( run );
  fNearAVAngular->EndOfRun( run );
  fExt0NuCosThetaTl208AV->EndOfRun( run );
  fExt0NuCosThetaTl208Hdropes->EndOfRun( run );
  fExt0NuCosThetaTl208Exwater->EndOfRun( run );
  fExt0NuCosThetaPMTbg->EndOfRun( run );
  fMultisiteTimeResTl208AV->EndOfRun( run );
  fMultisiteTimeResTl208Hdropes->EndOfRun( run );
  fMultisiteTimeResTl208Exwater->EndOfRun( run );
  fMultisiteTimeResPMTbg->EndOfRun( run );
  fMultisiteTimeResNaiveTl208AV->EndOfRun( run );
  fMultisiteTimeResNaiveTl208Hdropes->EndOfRun( run );
  fMultisiteTimeResNaiveTl208Exwater->EndOfRun( run );
  fMultisiteTimeResNaivePMTbg->EndOfRun( run );
  fMultisiteTimeResOffsetTl208AV->EndOfRun( run );
  fMultisiteTimeResOffsetTl208Hdropes->EndOfRun( run );
  fMultisiteTimeResOffsetTl208Exwater->EndOfRun( run );
  fMultisiteTimeResOffsetPMTbg->EndOfRun( run );
  fMultisiteTimeResBi214->EndOfRun( run );
  fMultisiteTimeResCo60->EndOfRun( run );
  fMultisiteTimeResBi210Bi214->EndOfRun( run );
  fMultisiteTimeResTl208->EndOfRun( run );
  fAlphaNReactorIBD->EndOfRun( run );
}

Processor::Result
ScintFitter::Event( DS::Run& run,
                    DS::EV& ev )
{

  // ScintFitter Logic:
  //  This is a simple overview of the logic that is expressed in the next 80 lines
  //
  ///  0. Use PMTCalSelector to select only well calibrated PMT hits for analysis
  ///  1. See if event was caused by a muon (muon datacleaning tag
  ///     check), if so, run the muon fitter routines, if fails, run
  ///     classifiers for retriggers and return FAIL, else continue. \n
  ///  2. Run unseeded classifiers:
  ///     - PositionANN
  ///     - NearAVAngular
  ///     - TimingPeaks
  ///     - PreTriggerHits
  ///  3. Fit the position & time as the seedResult from quad.
  ///     if nhits < fNhitCutoff then abort the fit, run classifiers for retriggers
  ///     and return FAIL. \n
  ///  4. Fit the position & time as the scintResult using the positionTimeLikelihood,
  ///     powell, null and seedResult as seed. If this fails then abort the fit, run
  ///     classifiers for retriggers and return FAIL. \n
  ///  5. Run position-seeded clasiffiers with scintResult as seed:
  ///     - isotropy
  ///     - ITR
  ///     - QPDFT
  ///     - meanTime
  ///     - isoRegions
  ///     - earlyTime
  ///     - BiPo212CumulTimeResid, BiPo214CumulTimeResid
  ///     - BiPo212LikelihoodDiff, BiPo214LikelihoodDiff. Use grid-55 as optimiser for both 212BiPo and 214BiPo PDFs
  ///     - AlphaBetaClassifier. Use grid-400 as optimiser for both 212_wPSD and 214_wPSD PDFs
  ///     - BerkeleyAlphaBeta
  ///     - Ext0NuCosTheta classifiers
  ///     - Millicharged classifiers and track reconstruction
  ///  6. Fit the energy, using the scintResult as seed (or seedResult if the
  ///     positionTimeLikelihood result is invalid). If this fails then abort,
  ///     run classifiers for retriggers and return FAIL. \n
  ///  7. Run energy-seeded clasiffiers with scintResult as seed:
  ///     - MultisiteTimeRes classifiers \n

  TStopwatch timer;
  timer.Start( true );

  TStopwatch selTimer;
  selTimer.Start( true );
  
  const size_t currentPass = MetaInformation::Get()->GetCurrentPass();
  ev.AddFitterPass( currentPass ); // Ensure the EV knows fitters were run for this pass
  ev.AddClassifierPass( currentPass ); // Ensure the EV knows classifiers were run for this pass

  std::vector<FitterPMT> pmtData;
  for( size_t iPMTCal =0; iPMTCal < ev.GetCalPMTs().GetCount(); iPMTCal++ )
    pmtData.push_back( FitterPMT( ev.GetCalPMTs().GetPMT( iPMTCal ) ) );
  /// Additionally select pmtData so it *only* contains PMTs which pass the PMTCal groups recommended selector cuts.
  DS::FitVertex dummyVertex;
  pmtData = fPMTCalSelector->GetSelectedPMTs( pmtData, dummyVertex );

  // If pmtData is empty, nothing useful can be processed.
  // Fail for the event.
  if(pmtData.empty()){
    warn << "ScintFitter::Event: No PMT hits after selection." << newline;
    return Processor::FAIL;
  }

  selTimer.Stop();
  double seltime = selTimer.RealTime();

  TStopwatch muonTimer;
  muonTimer.Start( true );
  
  // if event was tagged as muon, run muon fitting routines
  const DS::DataQCFlags& dcFlags = ev.GetDataCleaningFlags();
  const DU::DataCleaningBits& dcBits = DU::Utility::Get()->GetDataCleaningBits();
  // If the data cleaning tags have processed and the muon tag is set for this pass then run the muon fitter
  bool is_muon = false;
  Int_t dcPassNumber = dcFlags.GetLatestPass();
  if( dcFlags.ExistFlags( dcPassNumber ))
    is_muon = (dcFlags.GetApplied(dcPassNumber).Get(dcBits.GetBitIndex( "muontag" )) &&
        !dcFlags.GetFlags(dcPassNumber).Get(dcBits.GetBitIndex( "muontag" )));
  if( is_muon ) {
    fMuonClassifier->SetEventData( pmtData, &ev, &run );
    try
      {
        ClassifierResult muon = fMuonClassifier->GetClassification() ;
        ev.SetClassifierResult( currentPass, fMuonClassifier->GetName(), muon );
        const bool scintMedium = muon.GetClassification("medium");

        FitResult muonResult;
        if( scintMedium )
          {
            fMuonFitter->SetEventData( pmtData, &ev, &run );
            muonResult = fMuonFitter->GetBestFit();
          }
        else
          {
            fMuonWater->SetEventData( pmtData, &ev, &run );
            muonResult = fMuonWater->GetBestFit();
          }

        timer.Stop();
        muonResult.SetExecutionTime( timer.RealTime() );

        return SetMuonResult( muonResult, ev, currentPass );
      }
    catch( Classifier::ClassifierFailError& error )
      {
        warn << "ScintFitter::Event: Muon classifier failed " << error.what() << ", quiting." << newline;
        return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
      }
    catch( Method::MethodFailError& error )
      {
        warn << "ScintFitter::Event: Muon classification failed: "<< error.what() << ", quitting." << newline;
        return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
      }
  }

  muonTimer.Stop();
  double muontime = muonTimer.RealTime();

  TStopwatch unseedTimer;
  unseedTimer.Start( true );
  
  ////////////////////////////
  // Run unseeded classifiers
  ////////////////////////////

  // Initialise and run the PositionANN fitter
  // (note, this is stores a fit result, not classifier result)
  fPositionANN->SetEventData( pmtData, &ev, &run );
  try{ ev.SetFitResult( currentPass, fPositionANN->GetName() + ":scintFitter", fPositionANN->GetBestFit() ); }
  catch( Method::MethodFailError& error ) { warn << error.what() << newline; }
  // Initialise and run the NearAVAngular classifier
  fNearAVAngular->SetEventData( pmtData, &ev, &run );
  try{ ev.SetClassifierResult( currentPass, fNearAVAngular->GetName() + ":scintFitter",
                               fNearAVAngular->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the TimingPeaks classifier
  fTimingPeaks->SetEventData( pmtData, &ev, &run );
  try { ev.SetClassifierResult( currentPass, fTimingPeaks->GetName() + ":scintFitter", fTimingPeaks->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the PreTriggerHits classifier
  fPreTriggerHits->SetEventData( pmtData, &ev, &run );
  try { ev.SetClassifierResult( currentPass, fPreTriggerHits->GetName() + ":scintFitter", fPreTriggerHits->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  ////////////////////////////
  // Run position fit
  ////////////////////////////

  unseedTimer.Stop();
  double unseedtime = unseedTimer.RealTime();

  TStopwatch quadTimer;
  quadTimer.Start( true );
  
  // Initialise the seed
  fQuadSeed->SetEventData( pmtData, &ev, &run );

  // Run the seed method
  FitResult seedResult;
  // Set a dummy (invalid) vertex so we don't have to worry about nonexistant vertices
  seedResult.SetVertex(0, FitVertex());
  SetResult( seedResult, ev, currentPass );
  try
    {
      if( ev.GetNhitsCleaned() > fCutOff )
        seedResult = fQuadSeed->GetBestFit();
      else
        {
          warn << "ScintFitter::Event: insufficient points for quad to run, exiting" << newline;
          return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
        }
    }
  catch( Method::MethodFailError& error ) {
    warn << "ScintFitter::Event: Seed failed " << error.what() << ", continuing." << newline;
  }

  // Now initialise the position time method
  fPositionTime->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededMethod* >( fPositionTime )->DefaultSeed();
  dynamic_cast< SeededMethod* >( fPositionTime )->SetSeed( seedResult );

  quadTimer.Stop();
  double quadtime = quadTimer.RealTime();

  TStopwatch mpdfTimer;
  mpdfTimer.Start( true );
  
  FitResult scintResult;
  // Run the position time method
  try
    {
      FitResult positionResult = fPositionTime->GetBestFit();
      scintResult.SetVertex(0, positionResult.GetVertex(0));
      SetFOMs(scintResult, positionResult, "Position");
    }
  catch( Method::MethodFailError& error )
    {
      warn << "ScintFitter::Event: Main method failed " << error.what() << ", exiting" << newline;
      return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
    }

  // Set the result here so the position information is certain to be stored,
  // independent of failures from this point.
  SetResult( scintResult, ev, currentPass );

  // Check if the current scintResult vertex is valid.
  // If not, just return. No point in doing anything that requires it as seed
  if (!scintResult.GetValid()) {
    detail << "ScintFitter::Event: Invalid vertex. Bypassing seeded classifiers and energy fit." << newline;
    return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
  }

  mpdfTimer.Stop();
  double mpdftime = mpdfTimer.RealTime();

  TStopwatch seededTimer;
  seededTimer.Start( true );
  
  ///////////////////////////////////////
  // Run position-seeded classifiers
  //////////////////////////////////////

  TStopwatch isotropyTimer;
  isotropyTimer.Start( true );
  
  // Now initialise and run the Isotropy classifier
  fIsotropy->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fIsotropy )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fIsotropy )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fIsotropy->GetName() + ":scintFitter", fIsotropy->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  isotropyTimer.Stop();
  double isotropytime = isotropyTimer.RealTime();
  TStopwatch itrTimer;
  itrTimer.Start( true );
  
  // Now initialise and run the ITR classifier
  fITR->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fITR )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fITR )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fITR->GetName() + ":scintFitter", fITR->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  itrTimer.Stop();
  double itrtime = itrTimer.RealTime();
  TStopwatch xsiteTimer;
  xsiteTimer.Start( true );
  
  // Now initialise and run the XSite classifier
  fXSite->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fXSite )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fXSite )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fXSite->GetName() + ":scintFitter", fXSite->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  xsiteTimer.Stop();
  double xsitetime = xsiteTimer.RealTime();
  TStopwatch qpdtTimer;
  qpdtTimer.Start( true );
  
  // Now initialise and run the QPDT classifier
  fQPDT->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fQPDT )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fQPDT )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fQPDT->GetName() + ":scintFitter", fQPDT->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  qpdtTimer.Stop();
  double qpdttime = qpdtTimer.RealTime();
  TStopwatch meanTimer;
  meanTimer.Start( true );
  
  // Now initialise and run the MeanTime classifier
  fMeanTime->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMeanTime )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMeanTime )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fMeanTime->GetName() + ":scintFitter", fMeanTime->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  meanTimer.Stop();
  double meantime = meanTimer.RealTime();
  TStopwatch isoTimer;
  isoTimer.Start( true );
  
  // Now initialise and run the IsoRegions classifier
  fIsoRegions->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fIsoRegions )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fIsoRegions )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fIsoRegions->GetName() + ":scintFitter", fIsoRegions->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  isoTimer.Stop();
  double isotime = isoTimer.RealTime();
  TStopwatch earlyTimer;
  earlyTimer.Start( true );
  
  // Now initialise and run the EarlyTime classifier
  fEarlyTime->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fEarlyTime )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fEarlyTime )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fEarlyTime->GetName() + ":scintFitter", fEarlyTime->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  earlyTimer.Stop();
  double earlytime = earlyTimer.RealTime();
  TStopwatch bipocumulTimer;
  bipocumulTimer.Start( true );
  
  // Now initialise and run the BiPoCumulTimeResid classifier
  fBiPoCumulTimeResid->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBiPoCumulTimeResid )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPoCumulTimeResid )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fBiPoCumulTimeResid->GetName() + ":scintFitter", fBiPoCumulTimeResid->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  bipocumulTimer.Stop();
  double bipocumultime = bipocumulTimer.RealTime();
  TStopwatch BiPoLikelihoodTimer;
  BiPoLikelihoodTimer.Start( true );
  
  // Now initialise and run the BiPoLikelihoodDiff classifiers
  fBiPo212LikelihoodDiff->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBiPo212LikelihoodDiff )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo212LikelihoodDiff )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fBiPo212LikelihoodDiff->GetName() + ":212:scintFitter", fBiPo212LikelihoodDiff->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fBiPo214LikelihoodDiff->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBiPo214LikelihoodDiff )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo214LikelihoodDiff )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fBiPo214LikelihoodDiff->GetName() + ":214:scintFitter", fBiPo214LikelihoodDiff->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  BiPoLikelihoodTimer.Stop();
  double bipollhtime = BiPoLikelihoodTimer.RealTime();
  TStopwatch BiPo212Timer;
  BiPo212Timer.Start( true );
  
  //// for new AlphaBetaClassifier
  fBiPo212Classifier->SetEventData( pmtData , &ev , &run );
  dynamic_cast< SeededClassifier* >( fBiPo212Classifier )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo212Classifier )->SetSeed( scintResult );
  try{ ev.SetClassifierResult( currentPass, fBiPo212Classifier->GetName()+":212:scintFitter",fBiPo212Classifier->GetClassification() ); }
  catch ( Classifier::ClassifierFailError & error ) { warn<<error.what()<<newline; }

  BiPo212Timer.Stop();
  double bipollh212time = BiPo212Timer.RealTime();
  TStopwatch BiPo214Timer;
  BiPo214Timer.Start( true );
  
  fBiPo214Classifier->SetEventData( pmtData , &ev , &run );
  dynamic_cast< SeededClassifier* >( fBiPo214Classifier )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo214Classifier )->SetSeed( scintResult );
  try{ ev.SetClassifierResult( currentPass, fBiPo214Classifier->GetName()+":214:scintFitter",fBiPo214Classifier->GetClassification() ); }
  catch ( Classifier::ClassifierFailError & error ) { warn<<error.what()<<newline; }

  BiPo214Timer.Stop();
  double bipollh214time = BiPo214Timer.RealTime();
  TStopwatch BABTimer;
  BABTimer.Start( true );
  
  // Now initialise and run the BerkeleyAlphaBeta classifier
  fBerkeleyAlphaBeta->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBeta )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBeta )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fBerkeleyAlphaBeta->GetName() + ":scintFitter", fBerkeleyAlphaBeta->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fBerkeleyAlphaBetaFlasher->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBetaFlasher )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBetaFlasher )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fBerkeleyAlphaBetaFlasher->GetName() + ":Flasher:scintFitter", fBerkeleyAlphaBetaFlasher->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  BABTimer.Stop();
  double babtime = BABTimer.RealTime();
  TStopwatch Ext0nuTimer;
  Ext0nuTimer.Start( true );
  
  // Now initialise and run the Ext0NuCosTheta classifiers
  fExt0NuCosThetaTl208AV->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208AV )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208AV )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuCosThetaTl208AV->GetName() + ":Tl208AV:scintFitter", fExt0NuCosThetaTl208AV->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuCosThetaTl208Hdropes->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208Hdropes )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208Hdropes )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuCosThetaTl208Hdropes->GetName() + ":Tl208Hdropes:scintFitter", fExt0NuCosThetaTl208Hdropes->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuCosThetaTl208Exwater->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208Exwater )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208Exwater )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuCosThetaTl208Exwater->GetName() + ":Tl208Exwater:scintFitter", fExt0NuCosThetaTl208Exwater->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuCosThetaPMTbg->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaPMTbg )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaPMTbg )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuCosThetaPMTbg->GetName() + ":PMTbetagamma:scintFitter", fExt0NuCosThetaPMTbg->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  Ext0nuTimer.Stop();
  double ext0nutimer = Ext0nuTimer.RealTime();
  TStopwatch alphanTimer;
  alphanTimer.Start( true );
  
  // Now initialise and run the AlphaNReactor classifier
  fAlphaNReactorIBD->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fAlphaNReactorIBD )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fAlphaNReactorIBD )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fAlphaNReactorIBD->GetName() + ":scintFitter", fAlphaNReactorIBD->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  
  // Now initialise and run the ITR Millicharged particle classifier on events with > nhit cut
  fTrackMethod->SetEventData( pmtData, &ev, &run );
  fTrackMethod->SetD("PositionLogL", scintResult.GetFOM("PositionLogL"));
  //fTrackMethod->SetD("PositionLogL", 0.1);
  dynamic_cast< SeededMethod* >( fTrackMethod )->DefaultSeed();
  dynamic_cast< SeededMethod* >( fTrackMethod )->SetSeed( scintResult );
  try {
      FitResult mCPResult = fTrackMethod->GetBestFit();
      scintResult.SetVertex(1, mCPResult.GetVertex(0));
      scintResult.SetVertex(2, mCPResult.GetVertex(1));
      scintResult.SetVertex(3, mCPResult.GetVertex(2));
      scintResult.SetVertex(4, mCPResult.GetVertex(3));
      SetFOMs(scintResult, mCPResult, "mcp");
    }
  catch( Method::MethodFailError& error ) { warn << "ScintFitter::Event: Millicharged method failed " << error.what() << ", exiting" << newline; }

  alphanTimer.Stop();
  double alphantime = alphanTimer.RealTime();

  seededTimer.Stop();
  double seededtime = seededTimer.RealTime();

  TStopwatch energyTimer;
  energyTimer.Start( true );
  
  ////////////////////////////
  // Run energy fit
  ////////////////////////////

  // Now initialise the energy method
  fEnergyMethod->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededMethod* >( fEnergyMethod )->DefaultSeed();
  if( scintResult.GetValid() )
    dynamic_cast< SeededMethod* >( fEnergyMethod )->SetSeed( scintResult );
  else if( seedResult.GetValid() )
    dynamic_cast< SeededMethod* >( fEnergyMethod )->SetSeed( seedResult );
  else
    warn << "ScintFitter::Event: No seed for the energy method." << newline;
  // Run the energy method
  try
    {
      FitVertex vertex = scintResult.GetVertex(0);
      FitResult energyResult = fEnergyMethod->GetBestFit();
      FitVertex energyVertex = energyResult.GetVertex(0);
      vertex.SetEnergy( energyVertex.GetEnergy(), energyVertex.ValidEnergy() );
      vertex.SetPositiveEnergyError( energyVertex.GetPositiveEnergyError(), energyVertex.ValidEnergy() );
      vertex.SetNegativeEnergyError( energyVertex.GetNegativeEnergyError(), energyVertex.ValidEnergy() );
      scintResult.SetVertex( 0, vertex );
      SetFOMs(scintResult, energyResult, "Energy");
    }
  catch( Method::MethodFailError& error )
    {
      warn << "ScintFitter::Event: Energy method failed " << error.what() << ", exiting" << newline;
      return Processor::FAIL;
    }

  energyTimer.Stop();
  double energytime = energyTimer.RealTime();
  
  TStopwatch eseededTimer;
  eseededTimer.Start( true );

 
  //////////////////////////////////
  // Run energy-seeded classifiers
  /////////////////////////////////

  // Now initialise and run the MultisiteTimeRes classifiers
  fMultisiteTimeResTl208AV->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResTl208AV )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResTl208AV )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResTl208AV->GetName() + ":Tl208AV:scintFitter", fMultisiteTimeResTl208AV->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResTl208Hdropes->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResTl208Hdropes )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResTl208Hdropes )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResTl208Hdropes->GetName() + ":Tl208Hdropes:scintFitter", fMultisiteTimeResTl208Hdropes->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResTl208Exwater->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResTl208Exwater )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResTl208Exwater )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResTl208Exwater->GetName() + ":Tl208Exwater:scintFitter", fMultisiteTimeResTl208Exwater->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResPMTbg->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResPMTbg )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResPMTbg )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResPMTbg->GetName() + ":PMTbetagamma:scintFitter", fMultisiteTimeResPMTbg->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResNaiveTl208AV->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResNaiveTl208AV )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResNaiveTl208AV )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResNaiveTl208AV->GetName() + ":Tl208AV_NAIVE:scintFitter", fMultisiteTimeResNaiveTl208AV->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResNaiveTl208Hdropes->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResNaiveTl208Hdropes )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResNaiveTl208Hdropes )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResNaiveTl208Hdropes->GetName() + ":Tl208Hdropes_NAIVE:scintFitter", fMultisiteTimeResNaiveTl208Hdropes->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResNaiveTl208Exwater->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResNaiveTl208Exwater )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResNaiveTl208Exwater )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResNaiveTl208Exwater->GetName() + ":Tl208Exwater_NAIVE:scintFitter", fMultisiteTimeResNaiveTl208Exwater->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResNaivePMTbg->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResNaivePMTbg )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResNaivePMTbg )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResNaivePMTbg->GetName() + ":PMTbetagamma_NAIVE:scintFitter", fMultisiteTimeResNaivePMTbg->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResOffsetTl208AV->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResOffsetTl208AV )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResOffsetTl208AV )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResOffsetTl208AV->GetName() + ":Tl208AV_OFFSET10:scintFitter", fMultisiteTimeResOffsetTl208AV->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResOffsetTl208Hdropes->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResOffsetTl208Hdropes )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResOffsetTl208Hdropes )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResOffsetTl208Hdropes->GetName() + ":Tl208Hdropes_OFFSET10:scintFitter", fMultisiteTimeResOffsetTl208Hdropes->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResOffsetTl208Exwater->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResOffsetTl208Exwater )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResOffsetTl208Exwater )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResOffsetTl208Exwater->GetName() + ":Tl208Exwater_OFFSET10:scintFitter", fMultisiteTimeResOffsetTl208Exwater->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResOffsetPMTbg->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResOffsetPMTbg )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResOffsetPMTbg )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResOffsetPMTbg->GetName() + ":PMTbetagamma_OFFSET10:scintFitter", fMultisiteTimeResOffsetPMTbg->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResBi214->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResBi214 )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResBi214 )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResBi214->GetName() + ":Bi214:scintFitter", fMultisiteTimeResBi214->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResCo60->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResCo60 )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResCo60 )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResCo60->GetName() + ":Co60:scintFitter", fMultisiteTimeResCo60->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResBi210Bi214->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResBi210Bi214 )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResBi210Bi214 )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResBi210Bi214->GetName() + ":Bi210Bi214:scintFitter", fMultisiteTimeResBi210Bi214->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fMultisiteTimeResTl208->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResTl208 )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMultisiteTimeResTl208 )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fMultisiteTimeResTl208->GetName() + ":Tl208:scintFitter", fMultisiteTimeResTl208->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  
  eseededTimer.Stop();
  double eseededtime = eseededTimer.RealTime();
  
  //Finish and save
  timer.Stop();
  scintResult.SetExecutionTime( timer.RealTime() );
  double fulltime = timer.RealTime();

  std::cout << "PMT Selector: " << seltime << std::endl;
  std::cout << "Muon Check: " << muontime << std::endl;
  std::cout << "Unseeded Classifiers: " << unseedtime << std::endl;
  std::cout << "Quad Seed: " << quadtime << std::endl;
  std::cout << "MultiPDF: " << mpdftime << std::endl;
  std::cout << "Seeded Classifiers: " << seededtime << std::endl;
  std::cout << "Energy: " << energytime << std::endl;
  std::cout << "Energy Seeded Classifiers: " << eseededtime << std::endl;
  std::cout << "Total: " << fulltime << std::endl;

  std::cout << "Isotropy: " << isotropytime << std::endl;
  std::cout << "ITR: " << itrtime << std::endl;
  std::cout << "XSite: " << xsitetime << std::endl;
  std::cout << "QPDT: " << qpdttime << std::endl;
  std::cout << "Mean Time: " << meantime << std::endl;
  std::cout << "Iso Regions: " << isotime << std::endl;
  std::cout << "Early Time: " << earlytime << std::endl;
  std::cout << "BiPo Cumul: " << bipocumultime << std::endl;
  std::cout << "BiPo LLH: " << bipollhtime << std::endl;
  std::cout << "AlphaBeta 212: " << bipollh212time << std::endl;
  std::cout << "AlphaBeta 214: " << bipollh214time << std::endl;
  std::cout << "BAB: " << babtime << std::endl;
  std::cout << "Ext0Nu: " << ext0nutimer << std::endl;
  std::cout << "Alpha N IBD: " << alphantime << std::endl;

  return SetResult(scintResult, ev, currentPass);
}

Processor::Result
ScintFitter::SetResult( DS::FitResult& scintResult,
                        DS::EV& ev,
                        size_t currentPass)
{
  ev.SetFitResult( currentPass, "scintFitter", scintResult );
  DS::FitVertex defaultVertex;
  try{  // Set the 0 vertex as the default, if the fit is valid
    defaultVertex = scintResult.GetVertex(0);
  } catch (FitResult::NoVertexError&){
    defaultVertex = FitVertex();
    // Direction not calculated by the scint fitter so the DS::INVALID flag must be set
    defaultVertex.SetDirection(TVector3(DS::INVALID, DS::INVALID, DS::INVALID),false);
    defaultVertex.SetDirectionErrors(TVector3(DS::INVALID, DS::INVALID, DS::INVALID),false);
    scintResult.SetVertex(0,defaultVertex);
    ev.SetFitResult( currentPass, "scintFitter", scintResult );
    ev.SetDefaultFitVertex( "scintFitter", defaultVertex );
    return Processor::FAIL;
  }
  // Direction not calculated by the scint fitter so the DS::INVALID flag must be set
  defaultVertex.SetDirection(TVector3(DS::INVALID, DS::INVALID, DS::INVALID),false);
  defaultVertex.SetDirectionErrors(TVector3(DS::INVALID, DS::INVALID, DS::INVALID),false);
  ev.SetDefaultFitVertex( "scintFitter", defaultVertex );
  return Processor::OK;
}


Processor::Result
ScintFitter::SetMuonResult( DS::FitResult& scintResult,
                            DS::EV& ev,
                            size_t currentPass)
{
  ev.SetFitResult( currentPass, "scintFitter", scintResult );
  // Now set the 0 vertex as the default fit vertex, if the fit is valid
  ev.SetDefaultFitVertex( "scintFitter", scintResult.GetVertex(0) );

  return Processor::OK;
}


void
ScintFitter::SetFOMs( DS::FitResult& fitResult,
                      const DS::FitResult& scintResult,
                      const std::string& prefix )
{
  std::vector<std::string> fomNames = scintResult.GetFOMNames();
  for(std::vector<std::string>::const_iterator iter = fomNames.begin(); iter != fomNames.end(); ++iter)
    fitResult.SetFOM( (prefix+*iter), scintResult.GetFOM(*iter) );
}

Processor::Result
ScintFitter::RunClassifiersForRetriggersAndDie(std::vector<FitterPMT>& pmtData,
                                               DS::EV& ev,
                                               DS::Run& run,
                                               size_t currentPass){
  /// Run BerkeleyAlphaBeta classifier and exit
  fBerkeleyAlphaBeta->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBeta )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBeta )->SetSeed( DS::FitResult() );

  try {
    ev.SetClassifierResult ( currentPass, fBerkeleyAlphaBeta->GetName() + ":default", fBerkeleyAlphaBeta->GetClassification() );
  }
  catch( const Classifier::ClassifierFailError& error ) {
    warn << error.what() << newline;
  }

  fBerkeleyAlphaBetaFlasher->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBetaFlasher )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBetaFlasher )->SetSeed( DS::FitResult() );

  try {
    ev.SetClassifierResult ( currentPass, fBerkeleyAlphaBetaFlasher->GetName() + ":Flasher:default", fBerkeleyAlphaBetaFlasher->GetClassification() );
  }
  catch( const Classifier::ClassifierFailError& error ) {
    warn << error.what() << newline;
  }

  //// even if it's a retrigger event, run the AlphaBetaClassifier
  //// adding retrigger data helps distinguishing between BiPo and Te130
  //// Now initialise and run the AlphaBetaClassifier.cc BiPo212 classifier
  fBiPo212Classifier->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBiPo212Classifier )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo212Classifier )->SetSeed( DS::FitResult() );
  try{ ev.SetClassifierResult( currentPass, fBiPo212Classifier->GetName()+":212:default", fBiPo212Classifier->GetClassification() ); }
  catch ( const Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  //// even if it's a retrigger event, the AlphaBetaClassifier will be run
  //// adding retrigger data helps distinguishing between BiPo and Te130
  //// Now initialise and run the AlphaBetaClassifier.cc BiPo214 classifier
  fBiPo214Classifier->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBiPo214Classifier )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo214Classifier )->SetSeed( DS::FitResult() );
  try{ ev.SetClassifierResult( currentPass, fBiPo214Classifier->GetName()+":214:default", fBiPo214Classifier->GetClassification() ); }
  catch ( const Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  return Processor::FAIL;
}
