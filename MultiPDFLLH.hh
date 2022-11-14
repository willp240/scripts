////////////////////////////////////////////////////////////////////////
/// \class RAT::PDFs::MultiPDFLLH
///
/// \brief   Effective time 1d pdfs
///
/// \author name Will Parker <william.parker@physics.ox.ac.uk>
///
///
/// REVISION HISTORY:\n
///     17/03/2021 : W Parker - New file \n
///
///
/// \details Returns probabilities for 1d pdfs using the effective velocity
///         transit time routine. The pdfs can be split by distancs from
///         vertex position to pmt, but currently only 1 pdf is used.
///
////////////////////////////////////////////////////////////////////////

#ifndef __RAT_PDF_MultiPDFLLH__
#define __RAT_PDF_MultiPDFLLH__

#include <RAT/PDF.hh>

#include <string>
#include <TH1D.h>

class G4PhysicsOrderedFreeVector;

namespace RAT
{

class FitterPMT;

namespace DS
{
  class FitVertex;
}

namespace PDFs
{

class MultiPDFLLH : public PDF
{
public:
  double GetProbability( const FitterPMT& pmt, const DS::FitVertex& vertex );

  TH1D* GetPDF();
  
  std::string GetName() const { return Name(); }
  static std::string Name() { return std::string( "multiPDFLLH" ); }

  /// Initialise the PDF
  void Initialise( const std::string& param );

  void BeginOfRun( DS::Run& run );

  void EndOfRun( DS::Run& run );

  void SetI( const std::string& param, const int val );

  void SetS( const std::string& param, const std::string& value );

protected:
  std::string fIndex; ///Database index
  std::vector <G4PhysicsOrderedFreeVector*> fProbability; ///< PDF in a convenient interpolating structure

  unsigned int fNumPDFs; // Number of pdf tables
  std::vector < std::vector <double> > fDistanceRange;// range of vertex-pmt distances each pdf applies to
  std::vector <double> fAV_offset;
  double fAV_offset_z;
  TH1D* fPDF;
 };

} //::PDFs

} //::RAT

#endif
