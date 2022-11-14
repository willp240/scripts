#include <G4PhysicsOrderedFreeVector.hh>

#include <RAT/Log.hh>
#include <RAT/DB.hh>
#include <RAT/MultiPDFLLH.hh>
#include <RAT/FitterPMT.hh>
#include <RAT/Processor.hh>
#include <RAT/DS/FitVertex.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/EffectiveVelocity.hh>
#include <RAT/LightPathStraightScint.hh>

using namespace RAT;
using namespace RAT::PDFs;
using namespace RAT::DS;

void
MultiPDFLLH::SetI( const std::string& param, const int numPDFs )
{
  //Set the number of PDFs
  if( param == std::string( "fNumPDFs" ) ) {
    fNumPDFs = numPDFs;
  }
  else
    throw Processor::ParamUnknown( param );
}

void
MultiPDFLLH::SetS( const std::string& param, const std::string& value )
{
  //Set the material index
  if( param == std::string( "index" ) ) {
    fIndex = value;
  }
  else
    throw Processor::ParamUnknown( param );
}

void
MultiPDFLLH::Initialise( const std::string& param )
{
  fIndex = param;
  fNumPDFs = 0;
}

void
MultiPDFLLH::BeginOfRun( DS::Run& )
{
  // get tables from ratdb
  DB *db = DB::Get();
  DBLinkPtr link = db->GetLink( "NATIVE_GEO_DIMENSIONS", "natgeo_dimensions" );
  std::vector <RAT::DBLinkPtr> promptTimingTable;
  //Loop over pdfs (different pdfs for different pmt and vertex positions)
  for(unsigned int i_pdf = 0; i_pdf < fNumPDFs; i_pdf++){
    std::stringstream fullIndex;
    fullIndex << fIndex << "_" << fNumPDFs << "PDFNum" << i_pdf;
    promptTimingTable.push_back(RAT::DB::Get()->GetLink( "ET1D_MultiPDF",fullIndex.str()));
    //get pmt-vertex distances each table applies to
    fDistanceRange.push_back(promptTimingTable.at(i_pdf)->GetDArray("distance_range"));

    std::vector<double> times;
    std::vector<double> probability;
    try
      {
        times = promptTimingTable.at(i_pdf)->GetDArray( "time" );
        probability = promptTimingTable.at(i_pdf)->GetDArray( "probability" );
      }
    catch ( RAT::DBNotFoundError &e)
      {
        RAT::Log::Die( "MultiPDFLLH::Initialise: Cannot Find Prompt PDF Parameters" );
      }

    G4PhysicsOrderedFreeVector* tempProb = new G4PhysicsOrderedFreeVector( &(times[0]), &(probability[0]), times.size() );
    fProbability.push_back(tempProb);
  }

  fAV_offset = db->GetLink("GEO", "av")->GetDArray("position");
  fAV_offset_z = fAV_offset[2];

  RAT::LP::LightPathStraightScint::BeginOfRun();

  fPDF = new TH1D("pdf", "pdf", 400, -100, 300); 
  if (fProbability.size() > 0){
    for( int i=0; i< fProbability.at(0)->GetVectorLength(); i++){
      //      std::cout << i << " " << fProbability.at(0)->Value(i-100) << std::endl;
      fPDF->SetBinContent(i, fProbability.at(0)->Value(i-100));
    }
  }
  
}

void
MultiPDFLLH::EndOfRun( DS::Run& )
{
  for(unsigned int i_pdf = 0; i_pdf < fNumPDFs; i_pdf++){
    delete fProbability.at(i_pdf);
  }
  fProbability.clear();
  fDistanceRange.clear();
}


TH1D*
MultiPDFLLH::GetPDF( )
{
  //std::cout << fPDF->Integral() << std::endl;
  return fPDF;
}


double
MultiPDFLLH::GetProbability( const FitterPMT& pmt,
                                 const FitVertex& vertex )
{
  //Get seed vertex position
  TVector3 startPos;
  double eventTime;
  try
    {
      startPos = vertex.GetPosition();
      eventTime = vertex.GetTime();
    }
  catch( FitVertex::NoValueError& error )
    {
      warn << "MultiPDFLLH::GetProbability: This PDF does nothing without a proposed position & time" << newline;
      return 0.0;
    }

  const double mag2 = startPos.Mag2();
  if( std::isinf( mag2 ) || std::isnan( mag2 ) )
    return 0.0;

  //get path from vertex to pmt
  const TVector3 pmtPos = DU::Utility::Get()->GetPMTInfo().GetPosition( pmt.GetID() );
  double distInAV = 0.0;
  double distInWater = 0.0;
  double distInTarget = 0.0;

  RAT::LP::LightPathStraightScint::GetPath(pmtPos, startPos, distInTarget, distInWater);

  float transitTime = DU::Utility::Get()->GetEffectiveVelocity().CalcByDistance( distInTarget, distInAV, distInWater );
  float correctedTime = pmt.GetTime() - transitTime - eventTime;

  if(fNumPDFs==0)
    RAT::Log::Die( "MultiPDFLLH::GetProbability: Number of pdfs for multipdf methods has not been set." );

  return correctedTime;

  /*  double distance = (pmtPos - startPos).Mag();

  //return probability from corresponding pdf
  for(unsigned int i_pdf = 0; i_pdf < fNumPDFs; i_pdf++){
    if( distance >= fDistanceRange.at(i_pdf).at(0) &&
        distance <  fDistanceRange.at(i_pdf).at(1) ) {
      return fProbability.at(i_pdf)->Value( correctedTime );
    }
  }
  //if no corresponding pdf, return from first
  return fProbability.at(0)->Value( correctedTime );*/
}
