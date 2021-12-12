///////////////////////////////////////////////////////////////////////
//
// Dual direction and position PDF (combines result from two separate
// distributions)
//
// Author: Matt Mottram <m.mottram@qmul.ac.uk> -- contact person
//
// REVISION HISTORY:
//     - 26/04/2011 : M Mottram - New file
//     - 2014-12-09 : P G Jones - Allow chained PMTSelectors
//     - 2015-08-03 : M Mottram - Updates from code review
//
// Requires Seeding, optimising and a pdf
//
///////////////////////////////////////////////////////////////////////

#ifndef __RAT_Method_PositionTimeDirectionLikelihood__
#define __RAT_Method_PositionTimeDirectionLikelihood__

#include <string>
#include <stdexcept>

#include <RAT/OptimisedMethod.hh>
#include <RAT/PDFMethod.hh>
#include <RAT/SeededMethod.hh>
#include <RAT/SelectorMethod.hh>

#include <TFile.h>
#include <TGraph.h>
#include <TMath.h>
#include <TRandom3.h>

namespace RAT
{

namespace DS
{
  class FitResult;
}

namespace Methods
{

class PositionTimeDirectionLikelihood : public SeededMethod, public OptimisedMethod, public PDFMethod, public SelectorMethod
{
public:

  // Virtual destructor
  virtual ~PositionTimeDirectionLikelihood() {}

  // Returns the name of the positionTimeDirectionLikelihood method
  virtual std::string
  GetName() const { return PositionTimeDirectionLikelihood::Name(); }

  // Returns the name of the positionTimeDirectionLikelihood method
  static std::string
  Name() { return std::string( "positionTimeDirectionLikelihood" ); }

  // Set options for the positionTimeDirectionLikelihood method
  virtual void SetI( const std::string& param, const int value );

  // Initialises the positionTimeDirection likelihood method (requires no init string)
  void
  Initialise( const std::string& );

  // Does nothing currently
  //
  void BeginOfRun( DS::Run& ) { }

  void EndOfRun( DS::Run& ) { }

  // Sets the seed to the default
  //
  void DefaultSeed();

  // Invokes the method's algorithm
  virtual DS::FitResult
  GetBestFit();

  // Called by the optimiser to get the starting/seed params to optimise
  virtual std::vector<double>
  GetParams() const;

  // Called by the optimiser to get the starting/seed errors to optimise
  virtual std::vector<double> GetPositiveErrors() const;

  // Called by the optimiser to get the starting/seed errors to optimise
  virtual std::vector<double> GetNegativeErrors() const;

  // Converts the vector of params into the fFitResult
  virtual void SetParams( const std::vector<double>& params );

  // Converts the vector of errors into the fFitResult
  virtual void SetPositiveErrors( const std::vector<double>& errors );

  // Converts the vector of errors into the fFitResult
  virtual void SetNegativeErrors( const std::vector<double>& errors );

  virtual void DirectionLLHScan();

  virtual double calcLLH( const std::vector<double>& params );

  // Called by the optimiser to invoke the method algorithm, optimiser will pass current parameters
  virtual double
  operator()( const std::vector<double>& params );

private:
  bool fPolarDirection; // Run using cartesian (default) or polar coordinates for direction
  bool firstPass;
};

} //::Methods

} //::RAT

#endif
