////////////////////////////////////////////////////////////////////////
/// \class RAT::PDFs::DirPosPDF_Binned
///
/// \brief   2D PDFs in Cos Angle and Time Residual
///
/// \author name Josie Paton <josephine.paton@physics.ox.ac.uk>
///
///
/// REVISION HISTORY:\n
///     15/09/2021 : J Paton - New file \n
///
///
/// \details Returns probabilities for 2D PDFs using the effective velocity
///          transit time routine, and the cos angle between particle direction 
///          and photon direction. The 2D PDF is split into 10 slices in cos angle.
///          
////////////////////////////////////////////////////////////////////////

#ifndef __RAT_PDF_DirPosPDF_Binned__
#define __RAT_PDF_DirPosPDF_Binned__

#include <RAT/PDF.hh>
#include <TFile.h>
#include <TH2D.h>
#include <string>

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

class DirPosPDF_Binned : public PDF
{
public:
  double GetProbability( const FitterPMT& pmt, const DS::FitVertex& vertex );

  std::string GetName() const { return Name(); }
  static std::string Name() { return std::string( "DirPosPDF_Binned" ); }

  /// Initialise the PDF
  void Initialise( const std::string& param );

  void BeginOfRun( DS::Run& run );

  void EndOfRun( DS::Run& run );

  void SetI( const std::string& param, const int val );

  void SetS( const std::string& param, const std::string& value );

protected:
  std::string fIndex; ///Database index
  std::string fTable; ///Tabls name
  std::vector <G4PhysicsOrderedFreeVector*> fProbability; ///< PDF in a convenient interpolating structure

  unsigned int fNumPDFs; // Number of pdf slices
  std::vector < std::vector <double> > fCosAngleRange;// range of cos angle each pdf slice applies to
  std::vector <double> fAV_offset;
  double fAV_offset_z;
  TH2D* fPDF;
 };

} //::PDFs

} //::RAT

#endif
