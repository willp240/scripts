#include <G4PhysicsOrderedFreeVector.hh>

#include <RAT/Log.hh>
#include <RAT/DB.hh>
#include <RAT/DirPosPDF_Binned.hh>
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
DirPosPDF_Binned::SetI( const std::string& param, const int numPDFs )
{
  //Set the number of PDFs
  if( param == std::string( "fNumPDFs" ) ) {
    fNumPDFs = numPDFs;
  }
  else
    throw Processor::ParamUnknown( param );
}

void
DirPosPDF_Binned::SetS( const std::string& param, const std::string& value )
{
  //Set the material index
  if( param == std::string( "index" ) ) {
    fIndex = value;
  }
  else if( param == std::string( "tableName" ) ) {
    fTable = value;
  }
  else
    throw Processor::ParamUnknown( param );
}

void
DirPosPDF_Binned::Initialise( const std::string& param )
{
  //  fIndex = param;
  //  fTable = RAT::DB::Get()->GetLink("FIT_DIRECTION_POSITION_TABLENAME","tableName")->GetS("option");
  //fIndex = RAT::DB::Get()->GetLink("GEO", "inner_av")->GetS("material");
  //fNumPDFs = RAT::DB::Get()->GetLink(fTable ,fIndex)->GetI("num_pdfs");
}

void
DirPosPDF_Binned::BeginOfRun( DS::Run& )
{

  // get tables from ratdb
  DB *db = DB::Get();
  DBLinkPtr link = db->GetLink( "NATIVE_GEO_DIMENSIONS", "natgeo_dimensions" );
  /*  std::vector <RAT::DBLinkPtr> probabilityAngleSliceTable;
  RAT::DBLinkPtr timingTable = RAT::DB::Get()->GetLink( fTable ,fIndex);
  std::cout << fIndex << std::endl;
  std::vector<double> times;
  try
    {
      times = timingTable->GetDArray( "time" );
    }
  catch ( RAT::DBNotFoundError &e)
    {
      RAT::Log::Die( "DirPosPDF_Binned::Initialise: Cannot Find Timing Table" );
    }

  //Loop over pdf slices (different pdf slices for different bands of cos angle)
  for(unsigned int i_pdf = 0; i_pdf < fNumPDFs; i_pdf++){
    std::stringstream fullIndex;
    fullIndex << fIndex << "_" << fNumPDFs << "PDFBand" << i_pdf;
    probabilityAngleSliceTable.push_back(RAT::DB::Get()->GetLink( fTable,fullIndex.str()));
    //get cos angle range each table applies to
    fCosAngleRange.push_back(probabilityAngleSliceTable.at(i_pdf)->GetDArray("cos_angle_range"));

    std::vector<double> probability;
    try
      {
        probability = probabilityAngleSliceTable.at(i_pdf)->GetDArray( "probability" );
      }
    catch ( RAT::DBNotFoundError &e)
      {
        RAT::Log::Die( "DirPosPDF_Binned::Initialise: Cannot Find PDF Slice" );
      }

    G4PhysicsOrderedFreeVector* tempProb = new G4PhysicsOrderedFreeVector( &(times[0]), &(probability[0]), times.size() );
    fProbability.push_back(tempProb);
  }
*/
  fAV_offset = db->GetLink("GEO", "av")->GetDArray("position");
  fAV_offset_z = fAV_offset[2];

  RAT::LP::LightPathStraightScint::BeginOfRun();

  ////////////// If want PDF direct from file 
  //  TFile* pdfFile = TFile::Open("/home/parkerw/Software/6MeV_0p5gL_fullVolume_FullFill_PDFAltered_20bin.root");
  ////////////// If want PDF direct from file
  //TFile* pdfFile = TFile::Open("/home/parkerw/Software/rat_newdipo/newPDF6MeV_0.5gl_centre_fitPos_20dirbins.root");
  TFile* pdfFile = TFile::Open("/home/parkerw/Software/rat_newdipo/TimeAngle_volume_FITTED_ALTERED.root");
  //  TFile* pdfFile = TFile::Open("/data/snoplus/parkerw/ratSimulations/pdfs/dirpos/newPDF_centre_fitPos_20dirbins_tresidcalc.root");
  fPDF = (TH2D*)pdfFile->Get("newPDF")->Clone();

  //  fPDF->Scale(1/fPDF->Integral());
}

void
DirPosPDF_Binned::EndOfRun( DS::Run& )
{
  for(unsigned int i_pdf = 0; i_pdf < fNumPDFs; i_pdf++){
    //    delete fProbability.at(i_pdf);
  }
  fProbability.clear();
  fCosAngleRange.clear();
}

double
DirPosPDF_Binned::GetProbability( const FitterPMT& pmt,
                                 const FitVertex& vertex )
{
  //Get seed vertex position
  TVector3 startPos;
  TVector3 startDir;
  double eventTime;
  try
    {
      startPos = vertex.GetPosition();
      startDir = vertex.GetDirection().Unit();
      eventTime = vertex.GetTime();
    }
  catch( FitVertex::NoValueError& error )
    {
      warn << "DirPosPDF_Binned::GetProbability: This PDF does nothing without a proposed position, direction & time" << newline;
      return 0.0;
    }
  //Only really happens in grid search, but if direction is (0,0,0) normalise (1,1,1)
  if(startDir.X() == 0.0 && startDir.Y() == 0.0 && startDir.Z() == 0.0){
    startDir.SetX(1.0);
    startDir.SetY(1.0);
    startDir.SetZ(1.0);
    startDir = startDir.Unit();
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
    RAT::Log::Die( "DirPosPDF_Binned::GetProbability: Number of pdf slices for 2D PDF methods has not been set." );
  
  TVector3 photonDir = (pmtPos - startPos).Unit();
  double cosAngle = photonDir.Dot(startDir);

  if (correctedTime < -5)
    return 1;
  if (correctedTime > 15)
    return 1;

  //Get value fromm histogram bin
  int bin = fPDF->FindBin(correctedTime,cosAngle);
  double  prob = fPDF->GetBinContent(bin);

  return prob;//fPDF->GetBinContent(bin);
  /*
  //return probability from corresponding pdf
  for(unsigned int i_pdf = 0; i_pdf < fNumPDFs; i_pdf++){
    if( cosAngle >= fCosAngleRange.at(i_pdf).at(0) &&
        cosAngle <  fCosAngleRange.at(i_pdf).at(2) ) {
      return fProbability.at(i_pdf)->Value( correctedTime );
    }
  }
  //if no corresponding pdf, return from first
  return fProbability.at(0)->Value( correctedTime );
  */
}
