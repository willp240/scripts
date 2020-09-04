#include <RAT/ScintFitter.hh>
#include <RAT/PartialTestFitter_2ZSplit.hh>
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
#include <RAT/OptimisedClassifier.hh>
#include <RAT/PDFMethod.hh>
#include <RAT/SeededMethod.hh>
#include <RAT/SelectorMethod.hh>
#include <RAT/Optimiser.hh>
#include <RAT/SeededClassifier.hh>
#include <RAT/MetaInformation.hh>
#include <RAT/Log.hh>
#include <RAT/DB.hh>
#include <RAT/ListHelp.hh>
#include <RAT/MetaInformation.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/DataNotFound.hh>
#include <RAT/DS/DataQCFlags.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/DataCleaningBits.hh>
using namespace RAT;
using namespace RAT::Methods;
using namespace RAT::PDFs;
using namespace RAT::Optimisers;
using namespace RAT::PMTSelectors;
using namespace RAT::DS;
using namespace RAT::Classifiers;

#include <sstream>
using namespace std;

#include <TStopwatch.h>
using namespace ROOT;

PartialTestFitter_2ZSplit::PartialTestFitter_2ZSplit()
  : Processor("partialTestFitter_2ZSplit")
{
  fQuadSeed = MethodFactory::Get()->GetMethod( "quad" );
  fPositionTime = MethodFactory::Get()->GetMethod( "positionTimeLikelihood" );
  fMuonWater = MethodFactory::Get()->GetMethod( "MuonWater" );
  fMuonScintillator = MethodFactory::Get()->GetMethod( "MuonScintillator" );
  fPositionANN = MethodFactory::Get()->GetMethod( "positionANN" );
  fPowell = OptimiserFactory::Get()->GetOptimiser( "powell" );
  fNullSelector = PMTSelectorFactory::Get()->GetPMTSelector( "null" );
  fPMTCalSelector = PMTSelectorFactory::Get()->GetPMTSelector( "PMTCalSelector" );
  fET1D = PDFFactory::Get()->GetPDF( "et1d" );
  fPartialTest = PDFFactory::Get()->GetPDF( "partialtest2zsplit" );
  fEnergyMethod = MethodFactory::Get()->GetMethod( "energyRThetaFunctional" );
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
  fBiPo214Classifier = ClassifierFactory::Get()->GetClassifier("AlphaBetaClassifier-214_wPSD");
  fBerkeleyAlphaBeta = ClassifierFactory::Get()->GetClassifier("BerkeleyAlphaBeta");
  fGridOptimiser = OptimiserFactory::Get()->GetOptimiser("grid-80");
  fMuonClassifier = ClassifierFactory::Get()->GetClassifier( "muon" );
  fNearAVAngular = ClassifierFactory::Get()->GetClassifier( "nearAVAngular" );
  fExt0NuCosThetaTl208AV = ClassifierFactory::Get()->GetClassifier("Ext0NuCosTheta-Tl208AV");
  fExt0NuCosThetaTl208Exwater = ClassifierFactory::Get()->GetClassifier("Ext0NuCosTheta-Tl208Exwater");
  fExt0NuCosThetaTl208Hdropes = ClassifierFactory::Get()->GetClassifier("Ext0NuCosTheta-Tl208Hdropes");
  fExt0NuCosThetaPMTbg = ClassifierFactory::Get()->GetClassifier("Ext0NuCosTheta-PMTbetagamma");
  fExt0NuTimeResTl208AV = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-Tl208AV");
  fExt0NuTimeResTl208Exwater = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-Tl208Exwater");
  fExt0NuTimeResTl208Hdropes = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-Tl208Hdropes");
  fExt0NuTimeResPMTbg = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-PMTbetagamma");
  fExt0NuTimeResNaiveTl208AV = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-Tl208AV_NAIVE");
  fExt0NuTimeResNaiveTl208Hdropes = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-Tl208Hdropes_NAIVE");
  fExt0NuTimeResNaiveTl208Exwater = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-Tl208Exwater_NAIVE");
  fExt0NuTimeResNaivePMTbg = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-PMTbetagamma_NAIVE");
  fExt0NuTimeResOffsetTl208AV = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-Tl208AV_OFFSET10");
  fExt0NuTimeResOffsetTl208Hdropes = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-Tl208Hdropes_OFFSET10");
  fExt0NuTimeResOffsetTl208Exwater = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-Tl208Exwater_OFFSET10");
  fExt0NuTimeResOffsetPMTbg = ClassifierFactory::Get()->GetClassifier("Ext0NuTimeRes-PMTbetagamma_OFFSET10");
  // Now combine the components where appropriate
  dynamic_cast< OptimisedMethod* >( fPositionTime )->SetOptimiser( fPowell );
  dynamic_cast< SelectorMethod* >( fPositionTime )->AddPMTSelector( fNullSelector );
  dynamic_cast< PDFMethod* >( fPositionTime )->SetPDF( fPartialTest );
  //dynamic_cast< PDFMethod* >( fPositionTime )->SetPDF( fET1D );
  dynamic_cast< OptimisedClassifier* >( fBiPo212LikelihoodDiff )->SetOptimiser( fBiPoLikelihoodDiffOptimiser );
  dynamic_cast< OptimisedClassifier* >( fBiPo214LikelihoodDiff )->SetOptimiser( fBiPoLikelihoodDiffOptimiser );
  dynamic_cast< OptimisedClassifier* >( fBiPo212Classifier )->SetOptimiser(fGridOptimiser);
  dynamic_cast< OptimisedClassifier* >( fBiPo214Classifier )->SetOptimiser(fGridOptimiser);

  fCutOff = 3.0; // Maximum nhit for which quad cannot run
}

PartialTestFitter_2ZSplit::~PartialTestFitter_2ZSplit()
{
  delete fQuadSeed;
  delete fPositionTime;
  delete fPowell;
  delete fNullSelector;
  delete fET1D;
  delete fPartialTest;
  delete fEnergyMethod;
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
  delete fGridOptimiser;
  delete fMuonClassifier;
  delete fMuonWater;
  delete fMuonScintillator;
  delete fExt0NuCosThetaTl208AV;
  delete fExt0NuCosThetaTl208Hdropes;
  delete fExt0NuCosThetaTl208Exwater;
  delete fExt0NuCosThetaPMTbg;
  delete fExt0NuTimeResTl208AV;
  delete fExt0NuTimeResTl208Hdropes;
  delete fExt0NuTimeResTl208Exwater;
  delete fExt0NuTimeResPMTbg;
  delete fExt0NuTimeResNaiveTl208AV;
  delete fExt0NuTimeResNaiveTl208Hdropes;
  delete fExt0NuTimeResNaiveTl208Exwater;
  delete fExt0NuTimeResNaivePMTbg;
  delete fExt0NuTimeResOffsetTl208AV;
  delete fExt0NuTimeResOffsetTl208Hdropes;
  delete fExt0NuTimeResOffsetTl208Exwater;
  delete fExt0NuTimeResOffsetPMTbg;
}

void
PartialTestFitter_2ZSplit::BeginOfRun( DS::Run& run )
{
  // First, call the individual begin-of-run functions
  fQuadSeed->BeginOfRun( run );
  fPositionTime->BeginOfRun( run );
  fPositionANN->BeginOfRun( run );
  fPowell->BeginOfRun( run );
  fNullSelector->BeginOfRun( run );
  fET1D->BeginOfRun( run );
  fPartialTest->BeginOfRun( run );
  fEnergyMethod->BeginOfRun( run );
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
  fMuonClassifier->BeginOfRun( run );
  fMuonWater->BeginOfRun( run );
  fMuonScintillator->BeginOfRun( run );
  fNearAVAngular->BeginOfRun( run );
  fExt0NuCosThetaTl208AV->BeginOfRun( run );
  fExt0NuCosThetaTl208Hdropes->BeginOfRun( run );
  fExt0NuCosThetaTl208Exwater->BeginOfRun( run );
  fExt0NuCosThetaPMTbg->BeginOfRun( run );
  fExt0NuTimeResTl208AV->BeginOfRun( run );
  fExt0NuTimeResTl208Hdropes->BeginOfRun( run );
  fExt0NuTimeResTl208Exwater->BeginOfRun( run );
  fExt0NuTimeResPMTbg->BeginOfRun( run );
  fExt0NuTimeResNaiveTl208AV->BeginOfRun( run );
  fExt0NuTimeResNaiveTl208Hdropes->BeginOfRun( run );
  fExt0NuTimeResNaiveTl208Exwater->BeginOfRun( run );
  fExt0NuTimeResNaivePMTbg->BeginOfRun( run );
  fExt0NuTimeResOffsetTl208AV->BeginOfRun( run );
  fExt0NuTimeResOffsetTl208Hdropes->BeginOfRun( run );
  fExt0NuTimeResOffsetTl208Exwater->BeginOfRun( run );
  fExt0NuTimeResOffsetPMTbg->BeginOfRun( run );
}

Processor::Result
PartialTestFitter_2ZSplit::DSEvent( DS::Run& run,
                      DS::Entry& ds )
{
  for( size_t iEV = 0; iEV < ds.GetEVCount(); iEV++ )
    Event( run, ds.GetEV( iEV ) );
  return OK;
}

void
PartialTestFitter_2ZSplit::EndOfRun( DS::Run& run )
{
  // Finally ,call the individual end-of-run functions
  fQuadSeed->EndOfRun( run );
  fPositionTime->EndOfRun( run );
  fPositionANN->EndOfRun( run );
  fPowell->EndOfRun( run );
  fNullSelector->EndOfRun( run );
  fET1D->EndOfRun( run );
  fPartialTest->EndOfRun( run );
  fEnergyMethod->EndOfRun( run );
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
  fMuonClassifier->EndOfRun( run );
  fMuonClassifier->EndOfRun( run );
  fMuonWater->EndOfRun( run );
  fMuonScintillator->EndOfRun( run );
  fNearAVAngular->EndOfRun( run );
  fExt0NuCosThetaTl208AV->EndOfRun( run );
  fExt0NuCosThetaTl208Hdropes->EndOfRun( run );
  fExt0NuCosThetaTl208Exwater->EndOfRun( run );
  fExt0NuCosThetaPMTbg->EndOfRun( run );
  fExt0NuTimeResTl208AV->EndOfRun( run );
  fExt0NuTimeResTl208Hdropes->EndOfRun( run );
  fExt0NuTimeResTl208Exwater->EndOfRun( run );
  fExt0NuTimeResPMTbg->EndOfRun( run );
  fExt0NuTimeResNaiveTl208AV->EndOfRun( run );
  fExt0NuTimeResNaiveTl208Hdropes->EndOfRun( run );
  fExt0NuTimeResNaiveTl208Exwater->EndOfRun( run );
  fExt0NuTimeResNaivePMTbg->EndOfRun( run );
  fExt0NuTimeResOffsetTl208AV->EndOfRun( run );
  fExt0NuTimeResOffsetTl208Hdropes->EndOfRun( run );
  fExt0NuTimeResOffsetTl208Exwater->EndOfRun( run );
  fExt0NuTimeResOffsetPMTbg->EndOfRun( run );
}

Processor::Result
PartialTestFitter_2ZSplit::Event( DS::Run& run,
                    DS::EV& ev )
{

  // PartialTestFitter Logic:
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
  ///  5. Run seeded clasiffiers with scintResult as seed:
  ///     - ITR
  ///     - QPDFT
  ///     - meanTime
  ///     - isoRegions
  ///     - earlyTime
  ///     - BiPo212CumulTimeResid, BiPo214CumulTimeResid
  ///     - BiPo212LikelihoodDiff, BiPo214LikelihoodDiff. Use grid-55 as optimiser for both 212BiPo and 214BiPo PDFs
  ///     - AlphaBetaClassifier. Use grid-80 as optimiser for both 212_wPSD and 214_wPSD PDFs
  ///     - BerkeleyAlphaBeta
  ///     - Ext0NuCosTheta classifiers
  ///     - Ext0NuTimeRes classifiers
  ///  6. Fit the energy, using the scintResult as seed (or seedResult if the
  ///     positionTimeLikelihood result is invalid). If this fails then abort,
  ///     run classifiers for retriggers and return FAIL. \n


  TStopwatch timer;
  timer.Start( true );

  const size_t currentPass = MetaInformation::Get()->GetCurrentPass();
  ev.AddFitterPass( currentPass ); // Ensure the EV knows fitters were run for this pass
  ev.AddClassifierPass( currentPass ); // Ensure the EV knows classifiers were run for this pass

  vector<FitterPMT> pmtData;
  for( size_t iPMTCal =0; iPMTCal < ev.GetCalPMTs().GetCount(); iPMTCal++ )
    pmtData.push_back( FitterPMT( ev.GetCalPMTs().GetPMT( iPMTCal ) ) );
  /// Additionally select pmtData so it *only* contains PMTs which pass the PMTCal groups recommended selector cuts.
  DS::FitVertex dummyVertex;
  pmtData = fPMTCalSelector->GetSelectedPMTs( pmtData, dummyVertex );

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
            fMuonScintillator->SetEventData( pmtData, &ev, &run );
            muonResult = fMuonScintillator->GetBestFit();
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
        warn << "PartialTestFitter_2ZSplit::Event: Muon classifier failed " << error.what() << ", quiting." << newline;
        return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
      }
    catch( Method::MethodFailError& error )
      {
        warn << "PartialTestFitter_2ZSplit::Event: Muon classification failed: "<< error.what() << ", quitting." << newline;
        return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
      }
  }

  // Run unseeded classifiers

  // Initialise and run the PositionANN fitter
  // (note, this is stores a fit result, not classifier result)
  fPositionANN->SetEventData( pmtData, &ev, &run );
  try{ ev.SetFitResult( currentPass, fPositionANN->GetName() + ":partialTestFitter_2ZSplit", fPositionANN->GetBestFit() ); }
  catch( Method::MethodFailError& error ) { warn << error.what() << newline; }
  // Initialise and run the NearAVAngular classifier
  fNearAVAngular->SetEventData( pmtData, &ev, &run );
  try{ ev.SetClassifierResult( currentPass, fNearAVAngular->GetName() + ":partialTestFitter_2ZSplit",
                               fNearAVAngular->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the TimingPeaks classifier
  fTimingPeaks->SetEventData( pmtData, &ev, &run );
  try { ev.SetClassifierResult( currentPass, fTimingPeaks->GetName() + ":partialTestFitter_2ZSplit", fTimingPeaks->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the PreTriggerHits classifier
  fPreTriggerHits->SetEventData( pmtData, &ev, &run );
  try { ev.SetClassifierResult( currentPass, fPreTriggerHits->GetName() + ":partialTestFitter_2ZSplit", fPreTriggerHits->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }


  // Initialise the seed
  fQuadSeed->SetEventData( pmtData, &ev, &run );

  // Run the seed method
  FitResult seedResult;
  try
    {
      if( ev.GetCalPMTs().GetCount() > fCutOff )
        seedResult = fQuadSeed->GetBestFit();
      else
        {
          warn << "PartialTestFitter_2ZSplit::Event: insufficient points for quad to run, exiting" << newline;
          return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
        }
    }
  catch( Method::MethodFailError& error ) { warn << "PartialTestFitter_2ZSplit::Event: Seed failed " << error.what() << ", continuing." << newline; }

  // Now initialise the position time method
  fPositionTime->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededMethod* >( fPositionTime )->DefaultSeed();;
  dynamic_cast< SeededMethod* >( fPositionTime )->SetSeed( seedResult );
  
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
        warn << "PartialTestFitter_2ZSplit::Event: Main method failed " << error.what() << ", exiting" << newline;
      return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
    }
  
  // Run seeded classifiers

  // Now initialise and run the ITR classifier
  fITR->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fITR )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fITR )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fITR->GetName() + ":partialTestFitter_2ZSplit", fITR->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the XSite classifier
  fXSite->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fXSite )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fXSite )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fXSite->GetName() + ":partialTestFitter_2ZSplit", fXSite->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the QPDT classifier
  fQPDT->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fQPDT )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fQPDT )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fQPDT->GetName() + ":partialTestFitter_2ZSplit", fQPDT->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the MeanTime classifier
  fMeanTime->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fMeanTime )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fMeanTime )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fMeanTime->GetName() + ":partialTestFitter_2ZSplit", fMeanTime->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the IsoRegions classifier
  fIsoRegions->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fIsoRegions )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fIsoRegions )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fIsoRegions->GetName() + ":partialTestFitter_2ZSplit", fIsoRegions->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the EarlyTime classifier
  fEarlyTime->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fEarlyTime )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fEarlyTime )->SetSeed( scintResult );
  try { ev.SetClassifierResult( currentPass, fEarlyTime->GetName() + ":partialTestFitter_2ZSplit", fEarlyTime->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  // Now initialise and run the BiPoCumulTimeResid classifier
  fBiPoCumulTimeResid->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBiPoCumulTimeResid )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPoCumulTimeResid )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fBiPoCumulTimeResid->GetName() + ":partialTestFitter_2ZSplit", fBiPoCumulTimeResid->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the BiPoLikelihoodDiff classifiers
  fBiPo212LikelihoodDiff->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBiPo212LikelihoodDiff )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo212LikelihoodDiff )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fBiPo212LikelihoodDiff->GetName() + ":212:partialTestFitter_2ZSplit", fBiPo212LikelihoodDiff->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fBiPo214LikelihoodDiff->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBiPo214LikelihoodDiff )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo214LikelihoodDiff )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fBiPo214LikelihoodDiff->GetName() + ":214:partialTestFitter_2ZSplit", fBiPo214LikelihoodDiff->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fBiPo212Classifier->SetEventData(pmtData,&ev,&run);
  dynamic_cast< SeededClassifier* >( fBiPo212Classifier )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo212Classifier )->SetSeed( scintResult );
  try{ ev.SetClassifierResult( currentPass, fBiPo212Classifier->GetName()+":212:partialTestFitter_2ZSplit",fBiPo212Classifier->GetClassification() ); }
  catch ( Classifier::ClassifierFailError & error ) { warn<<error.what()<<newline; }
  fBiPo214Classifier->SetEventData(pmtData,&ev,&run);
  dynamic_cast< SeededClassifier* >( fBiPo214Classifier )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBiPo214Classifier )->SetSeed( scintResult );
  try{ ev.SetClassifierResult( currentPass, fBiPo214Classifier->GetName()+":214:partialTestFitter_2ZSplit",fBiPo214Classifier->GetClassification() ); }
  catch ( Classifier::ClassifierFailError & error ) { warn<<error.what()<<newline; }
  // Now initialise and run the BerkeleyAlphaBeta classifier
  fBerkeleyAlphaBeta->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBeta )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBeta )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fBerkeleyAlphaBeta->GetName() + ":partialTestFitter_2ZSplit", fBerkeleyAlphaBeta->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the Ext0NuCosTheta classifiers
  fExt0NuCosThetaTl208AV->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208AV )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208AV )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuCosThetaTl208AV->GetName() + ":Tl208AV:partialTestFitter_2ZSplit", fExt0NuCosThetaTl208AV->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuCosThetaTl208Hdropes->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208Hdropes )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208Hdropes )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuCosThetaTl208Hdropes->GetName() + ":Tl208Hdropes:partialTestFitter_2ZSplit", fExt0NuCosThetaTl208Hdropes->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuCosThetaTl208Exwater->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208Exwater )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaTl208Exwater )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuCosThetaTl208Exwater->GetName() + ":Tl208Exwater:partialTestFitter_2ZSplit", fExt0NuCosThetaTl208Exwater->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuCosThetaPMTbg->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaPMTbg )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuCosThetaPMTbg )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuCosThetaPMTbg->GetName() + ":PMTbetagamma:partialTestFitter_2ZSplit", fExt0NuCosThetaPMTbg->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  // Now initialise and run the Ext0NuTimeRes classifiers
  fExt0NuTimeResTl208AV->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResTl208AV )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResTl208AV )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResTl208AV->GetName() + ":Tl208AV:partialTestFitter_2ZSplit", fExt0NuTimeResTl208AV->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResTl208Hdropes->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResTl208Hdropes )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResTl208Hdropes )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResTl208Hdropes->GetName() + ":Tl208Hdropes:partialTestFitter_2ZSplit", fExt0NuTimeResTl208Hdropes->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResTl208Exwater->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResTl208Exwater )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResTl208Exwater )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResTl208Exwater->GetName() + ":Tl208Exwater:partialTestFitter_2ZSplit", fExt0NuTimeResTl208Exwater->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResPMTbg->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResPMTbg )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResPMTbg )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResPMTbg->GetName() + ":PMTbetagamma:partialTestFitter_2ZSplit", fExt0NuTimeResPMTbg->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResNaiveTl208AV->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResNaiveTl208AV )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResNaiveTl208AV )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResNaiveTl208AV->GetName() + ":Tl208AV_NAIVE:partialTestFitter_2ZSplit", fExt0NuTimeResNaiveTl208AV->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResNaiveTl208Hdropes->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResNaiveTl208Hdropes )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResNaiveTl208Hdropes )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResNaiveTl208Hdropes->GetName() + ":Tl208Hdropes_NAIVE:partialTestFitter_2ZSplit", fExt0NuTimeResNaiveTl208Hdropes->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResNaiveTl208Exwater->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResNaiveTl208Exwater )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResNaiveTl208Exwater )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResNaiveTl208Exwater->GetName() + ":Tl208Exwater_NAIVE:partialTestFitter_2ZSplit", fExt0NuTimeResNaiveTl208Exwater->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResNaivePMTbg->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResNaivePMTbg )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResNaivePMTbg )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResNaivePMTbg->GetName() + ":PMTbetagamma_NAIVE:partialTestFitter_2ZSplit", fExt0NuTimeResNaivePMTbg->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResOffsetTl208AV->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResOffsetTl208AV )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResOffsetTl208AV )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResOffsetTl208AV->GetName() + ":Tl208AV_OFFSET10:partialTestFitter_2ZSplit", fExt0NuTimeResOffsetTl208AV->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResOffsetTl208Hdropes->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResOffsetTl208Hdropes )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResOffsetTl208Hdropes )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResOffsetTl208Hdropes->GetName() + ":Tl208Hdropes_OFFSET10:partialTestFitter_2ZSplit", fExt0NuTimeResOffsetTl208Hdropes->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResOffsetTl208Exwater->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResOffsetTl208Exwater )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResOffsetTl208Exwater )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResOffsetTl208Exwater->GetName() + ":Tl208Exwater_OFFSET10:partialTestFitter_2ZSplit", fExt0NuTimeResOffsetTl208Exwater->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  fExt0NuTimeResOffsetPMTbg->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResOffsetPMTbg )->DefaultSeed();
  dynamic_cast< SeededClassifier* >( fExt0NuTimeResOffsetPMTbg )->SetSeed( scintResult );
  try { ev.SetClassifierResult ( currentPass, fExt0NuTimeResOffsetPMTbg->GetName() + ":PMTbetagamma_OFFSET10:partialTestFitter_2ZSplit", fExt0NuTimeResOffsetPMTbg->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }
  
  // Now initialise the energy method
  fEnergyMethod->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededMethod* >( fEnergyMethod )->DefaultSeed();
  if( scintResult.GetValid() )
    dynamic_cast< SeededMethod* >( fEnergyMethod )->SetSeed( scintResult );
  else if( seedResult.GetValid() )
    dynamic_cast< SeededMethod* >( fEnergyMethod )->SetSeed( seedResult );
  else
    warn << "PartialTestFitter_2ZSplit::Event: No seed for the energy method." << newline;
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
      warn << "PartialTestFitter_2ZSplit::Event: Energy method failed " << error.what() << ", exiting" << newline;
      return RunClassifiersForRetriggersAndDie(pmtData, ev, run, currentPass);
    }

  timer.Stop();
  scintResult.SetExecutionTime( timer.RealTime() );
  
  return SetResult(scintResult, ev, currentPass);
}

Processor::Result
PartialTestFitter_2ZSplit::SetResult( DS::FitResult& scintResult,
                        DS::EV& ev,
                        size_t currentPass)
{
  ev.SetFitResult( currentPass, "partialTestFitter_2ZSplit", scintResult );
  // Now set the 0 vertex as the default fit vertex, if the fit is valid
  DS::FitVertex& defaultVertex = scintResult.GetVertex(0);
  // Only the position, energy and time are calculated by the partialTestFitter_2ZSplit, so the DS::INVALID flag must be set for direction
  defaultVertex.SetDirection(TVector3(DS::INVALID, DS::INVALID, DS::INVALID),false);
  defaultVertex.SetDirectionErrors(TVector3(DS::INVALID, DS::INVALID, DS::INVALID),false);
  ev.SetDefaultFitVertex( "partialTestFitter_2ZSplit", defaultVertex );

  return Processor::OK;
}


Processor::Result
PartialTestFitter_2ZSplit::SetMuonResult( DS::FitResult& scintResult,
                            DS::EV& ev,
                            size_t currentPass)
{
  ev.SetFitResult( currentPass, "partialTestFitter_2ZSplit", scintResult );
  // Now set the 0 vertex as the default fit vertex, if the fit is valid
  ev.SetDefaultFitVertex( "partialTestFitter_2ZSplit", scintResult.GetVertex(0) );

  return Processor::OK;
}


void
PartialTestFitter_2ZSplit::SetFOMs( DS::FitResult& fitResult,
                      const DS::FitResult& partialResult,
                      const string& prefix )
{
  vector<string> fomNames = partialResult.GetFOMNames();
  for(vector<string>::const_iterator iter = fomNames.begin(); iter != fomNames.end(); ++iter)
    fitResult.SetFOM( (prefix+*iter), partialResult.GetFOM(*iter) );
}

Processor::Result
PartialTestFitter_2ZSplit::RunClassifiersForRetriggersAndDie(vector<FitterPMT>& pmtData,
                                               DS::EV& ev,
                                               DS::Run& run,
                                               size_t currentPass){
  /// Run BerkeleyAlphaBeta classifier and exit
  fBerkeleyAlphaBeta->SetEventData( pmtData, &ev, &run );
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBeta )->DefaultSeed();
  const DS::FitResult *seed_null = NULL;
  dynamic_cast< SeededClassifier* >( fBerkeleyAlphaBeta )->SetSeed( *seed_null );
  try { ev.SetClassifierResult ( currentPass, fBerkeleyAlphaBeta->GetName() + ":default", fBerkeleyAlphaBeta->GetClassification() ); }
  catch( Classifier::ClassifierFailError& error ) { warn << error.what() << newline; }

  return Processor::FAIL;

}
