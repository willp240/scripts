////////////////////////////////////////////////////////////////////////
/// \class RAT::Optimisers::Shell
///
/// \brief Evaluates FOM surface
///
/// \author Eric Marzec <marzece@sas.upenn.edu> -- contact person
///
/// REVISION HISTORY:\n
/// 02/2014 New File. \n
///
/// \details This optimiser evaluates the figure of merit at a constant
///         number of divisions over a range of the parameter space.
///         It is in many ways a modified version of the surface
///         optimiser, the difference being this does not write to a
///         file and instead returns the optimal FOM.
////////////////////////////////////////////////////////////////////////

#ifndef __RAT_Optimiser_Shell_
#define __RAT_Optimiser_Shell_

#include <string>

#include <RAT/Optimiser.hh>

namespace RAT
{

namespace Optimisers
{

class Shell : public Optimiser
{
public:
  virtual std::string GetName() const { return Shell::Name(); }

  static std::string Name() { return std::string( "Shell" ); }

  /// Initialise the Optimiser
  void Initialise( const std::string& param );

  void BeginOfRun( DS::Run& ) { }

  void EndOfRun( DS::Run& ) { }

  virtual double Minimise();

  virtual double Maximise();

protected:
  /// Sets up the variables needed for the scan and begins scanning.
  virtual double
  Evaluate();
  ///Recursive function that steps through parameter space evaluating, saving optimal values when found.
  /// @param[in] coords to scan over
  /// @param[in] dimension to scan over
  virtual void
  Scan( std::vector<double> coords, int dimension );
  std::vector<double> fStartPos; /// The position at which the scanning will begin.
  std::vector<double> fIncrements; /// Contains how much large each step in a given dimension should be.
  double fOptimalFom; /// Best value found in optimisation.
  int fMinFactor; /// This is -1 if the optimiser is maximising and 1 if minimising.
  std::vector<int> fDiv; /// How many steps should be taken in any of the dimensions.
  std::vector<double> fMax;
  std::vector<double> fMin;
  std::vector<double> fScanParams;
  int fTotalDim; ///Number of dimensions to be scanned.
  int fScannedDim;
  double fShellRadius;

};

} //::Optimiser

} //::RAT

#endif
