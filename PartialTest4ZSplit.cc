#include <G4PhysicsOrderedFreeVector.hh>

#include <vector>
#include <string>
using namespace std;

#include <RAT/Log.hh>
#include <RAT/DB.hh>
#include <RAT/PartialTest4ZSplit.hh>
#include <RAT/FitterPMT.hh>
#include <RAT/DS/FitVertex.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/LightPathCalculator.hh>
#include <RAT/DU/EffectiveVelocity.hh>
using namespace RAT;
using namespace RAT::PDFs;
using namespace RAT::DS;

void
PartialTest4ZSplit::Initialise( const std::string& param )
{
  fIndex = param;
}

void
PartialTest4ZSplit::BeginOfRun( DS::Run& )
{
  DB *db = DB::Get();
  string index;
  if( fIndex.empty() == true )
    {
      index = db->GetLink("GEO", "inner_av")->GetS("material");
    }
  else
    index = fIndex;

  // Set PDF to labppo_scintillator as default
  string defaultIndex2 = "labppo_scintillator";
  string defaultIndex = "internalwater_snoplus";
  
  RAT::DBLinkPtr promptTimingTable = RAT::DB::Get()->GetLink( "ET1D_Partial_PMTZBelow-1.5",defaultIndex );
  // then check for material specific PDF
  DBLinkGroup grp=db->GetLinkGroup("ET1D_Partial_PMTZBelow-1.5");
  DBLinkGroup::iterator it;
  for(it=grp.begin(); it != grp.end(); ++it){
    if(index == it->first){
      promptTimingTable = RAT::DB::Get()->GetLink("ET1D_Partial_PMTZBelow-1.5",defaultIndex);
      break;
    }
  }

  RAT::DBLinkPtr promptTimingTable2 = RAT::DB::Get()->GetLink( "ET1D_Partial_PMTZ-1.5to0",defaultIndex );
  // then check for material specific PDF
  DBLinkGroup grp2=db->GetLinkGroup("ET1D_Partial_PMTZ-1.5to0");
  DBLinkGroup::iterator it2;
  for(it2=grp2.begin(); it2 != grp2.end(); ++it2){
    if(index == it2->first){
      promptTimingTable2 = RAT::DB::Get()->GetLink("ET1D_Partial_PMTZ-1.5to0",defaultIndex);
      break;
    }
  } 

  RAT::DBLinkPtr promptTimingTable3 = RAT::DB::Get()->GetLink( "ET1D_Partial_PMTZ0to1.5",defaultIndex2 );
  // then check for material specific PDF
  DBLinkGroup grp3=db->GetLinkGroup("ET1D_Partial_PMTZ0to1.5");
  DBLinkGroup::iterator it3;
  for(it3=grp.begin(); it3 != grp.end(); ++it3){
    if(index == it3->first){
      promptTimingTable3 = RAT::DB::Get()->GetLink("ET1D_Partial_PMTZ0to1.5",defaultIndex2);
      break;
    }
  }

  RAT::DBLinkPtr promptTimingTable4 = RAT::DB::Get()->GetLink( "ET1D_Partial_PMTZAbove1.5",defaultIndex2 );
  // then check for material specific PDF
  DBLinkGroup grp4=db->GetLinkGroup("ET1D_Partial_PMTZAbove1.5");
  DBLinkGroup::iterator it4;
  for(it4=grp2.begin(); it4 != grp2.end(); ++it4){
    if(index == it4->first){
      promptTimingTable4 = RAT::DB::Get()->GetLink("ET1D_Partial_PMTZAbove1.5",defaultIndex2);
      break;
    }
  }
 
  vector<double> times, times2, times3, times4;
  vector<double> probability, probability2, probability3, probability4;
  
  try
    {
      times = promptTimingTable->GetDArray( "time" );
      probability = promptTimingTable->GetDArray( "probability" );
      times2 = promptTimingTable2->GetDArray( "time" );
      probability2 = promptTimingTable2->GetDArray( "probability" );
      times3 = promptTimingTable->GetDArray( "time" );
      probability3 = promptTimingTable->GetDArray( "probability" );
      times4 = promptTimingTable2->GetDArray( "time" );
      probability4 = promptTimingTable2->GetDArray( "probability" );
    }
  catch ( RAT::DBNotFoundError &e)
    {
      RAT::Log::Die( "PartialTest4ZSplit::Initialise: Cannot Find Prompt PDF Parameters" );
    }

  fProbability = new G4PhysicsOrderedFreeVector( &(times[0]), &(probability[0]), times.size() );
  fProbability2 = new G4PhysicsOrderedFreeVector( &(times2[0]), &(probability2[0]), times2.size() );
  fProbability3 = new G4PhysicsOrderedFreeVector( &(times3[0]), &(probability3[0]), times3.size() );
  fProbability4 = new G4PhysicsOrderedFreeVector( &(times4[0]), &(probability4[0]), times4.size() );

  fLightPath = DU::Utility::Get()->GetLightPathCalculator();
}

void
PartialTest4ZSplit::EndOfRun( DS::Run& )
{
  delete fProbability;
}

double
PartialTest4ZSplit::GetProbability( const FitterPMT& pmt,
                      const FitVertex& vertex )
{
  TVector3 startPos;
  double eventTime;
  try
    {
      startPos = vertex.GetPosition();
      eventTime = vertex.GetTime();
    }
  catch( FitVertex::NoValueError& error )
    {
      warn << "PartialTest4ZSplit::GetProbability: This PDF does nothing without a proposed position & time" << newline;
      return 0.0;
    }

  const double mag2 = startPos.Mag2();
  if( std::isinf( mag2 ) || std::isnan( mag2 ) )
    return 0.0;

  const TVector3 pmtPos = DU::Utility::Get()->GetPMTInfo().GetPosition( pmt.GetID() );
  double distInInnerAV = 0.0;
  double distInAV = 0.0;
  double distInWater = 0.0;
  double distInUpperTarget = 0.0;
  double distInLowerTarget = 0.0;

  fLightPath.CalcByPositionPartial( startPos, pmtPos );
  if( fLightPath.GetPathValid() ) {
    distInInnerAV = fLightPath.GetDistInInnerAV();
    distInAV = fLightPath.GetDistInAV();
    distInWater = fLightPath.GetDistInWater();
    distInUpperTarget = fLightPath.GetDistInUpperTarget();
    distInLowerTarget = fLightPath.GetDistInLowerTarget();
  } else  // approximation: total distance in only inner AV material
    distInInnerAV = ( startPos - pmtPos ).Mag();

  const double transitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInUpperTarget, distInAV, distInWater+distInLowerTarget );
  //const double transitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInInnerAV, distInAV,distInWater );
  const double correctedTime = pmt.GetTime() - transitTime - eventTime;

  if(pmtPos.Z()<-1500){
    if(fProbability->Value( correctedTime ) < 0.5)
      return 2.0;
    else
      return 1000*fProbability->Value( correctedTime );
  }
  else if(pmtPos.Z()<0){
    if(fProbability2->Value( correctedTime ) < 0.5)
      return 2.0;
    else
      return 1000*fProbability2->Value( correctedTime );
  }
  else if(pmtPos.Z()<1500){
    if(fProbability3->Value( correctedTime ) < 0.5)
      return 2.0;
    else
      return 1000*fProbability3->Value( correctedTime );
  }
  else{
    if(fProbability4->Value( correctedTime ) < 0.5)
      return 2.0;
    else
      return 1000*fProbability4->Value( correctedTime );
  }
   
}
