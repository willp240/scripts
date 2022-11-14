///////////////////////////////////////////////////////////////////////
//
// Position and time likelihood method
//
// Author: Phil G Jones <p.g.jones@qmul.ac.uk>
// Author: Matthew Mottram <m.mottram@qmul.ac.uk> -- contact person
//
// REVISION HISTORY:
//     - 12/08/2011 : P G Jones - New file
//     - 02/05/2012 : P G Jones - New method optimiser relationship.
//     - 10/09/2013 : K Majumdar - Adding in ability to do asymmetric
//        errors and errors in Spherical coordinates
//     - 2014-12-09 : P G Jones - Allow chained PMTSelectors
//     - 2015-08-03 : M Mottram - Updates from code review
//
// The position time likelihood method uses a maximum likelihood method
// to fit the position and time of an event. Its designed for use with
// a PDF to which it provides a seed vertex which is then optimised
// according to the criteria in the PDF, e.g. times residual of the PMT
// hits. It then returns an fit vertex with position, time, errors and
// a validity.
// Requires Seeding, optimising, selector and a pdf
//
///////////////////////////////////////////////////////////////////////

#ifndef __RAT_Method_PositionTimePoissonLikelihood_
#define __RAT_Method_PositionTimePoissonLikelihood_

#include <string>
#include <stdexcept>

#include <RAT/OptimisedMethod.hh>
#include <RAT/PDFMethod.hh>
#include <RAT/SeededMethod.hh>
#include <RAT/SelectorMethod.hh>
#include <RAT/FitterComponent.hh>

#include <TH1D.h>
#include <TFile.h>
#include <TGraph.h>

namespace RAT
{

namespace DS
{
  class FitResult;
}

namespace Methods
{

class PositionTimePoissonLikelihood : public SeededMethod, public OptimisedMethod, public PDFMethod, public SelectorMethod
{
public:

  PositionTimePoissonLikelihood() : fNHitCut(0) { }

  // Virtual destructor
  virtual ~PositionTimePoissonLikelihood() {}

  // Returns the name of the positionTimePoissonLikelihood method
  virtual std::string
  GetName() const { return PositionTimePoissonLikelihood::Name(); }

  // Returns the name of the positionTimePoissonLikelihood method
  static std::string
  Name() { return std::string( "positionTimePoissonLikelihood" ); }

  // Initialises the Positionlikelihood method (requires no init string)
  void Initialise( const std::string& ) { }

  // Does nothing currently
  void BeginOfRun( DS::Run& ) { }
  void EndOfRun( DS::Run& ) { }

  // Sets the seed to the default
  void DefaultSeed();

  void SetI( const std::string& name, const int value );

  virtual void LLHScan();

  double calcLLH( const std::vector<double>& params );

  // Invokes the method's algorithm
  virtual DS::FitResult
  GetBestFit();
  // Called by the optimiser to get the starting/seed params to optimise
  virtual std::vector<double>
  GetParams() const;
  // Called by the optimiser to get the starting/seed errors to optimise
  virtual std::vector<double> GetPositiveErrors() const;
  virtual std::vector<double> GetNegativeErrors() const;
  // Converts the vector of params into the fFitResult
  virtual void SetParams( const std::vector<double>& params );
  // Converts the vector of errors into the fFitResult
  virtual void SetPositiveErrors( const std::vector<double>& errors );
  virtual void SetNegativeErrors( const std::vector<double>& errors );
  // Called by the optimiser to invoke the method algorithm, optimiser will pass current parameters
  virtual double
  operator()( const std::vector<double>& params );

protected:
  size_t fNHitCut;
  TH1D* fPDFHist;
  TH1D* fTResidDist;
  bool firstTime=true;
};

} //::Methods

} //::RAT

#endif
