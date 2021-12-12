#include <RAT/Grid.hh>
#include <RAT/OptimisedComponent.hh>
#include <iostream>
#include <math.h>
#include <sstream>
using namespace RAT::Optimisers;

void Grid::Initialise( const std::string& param )
{
  // Convert input string to fDiv, else make fDiv take default val
  if ( param.empty() ){
    fDiv=13;
  } else {
    std::stringstream strStream(param);
    strStream >> fDiv;
  }

}

double
Grid::Minimise()
{
  fMinFactor = 1.0;
  return fMinFactor*Evaluate();
}

double
Grid::Maximise()
{
  fMinFactor = -1.0;
  return fMinFactor*Evaluate();
}

double Grid::Evaluate()
{
  // Get initial seed parameters
  fParams = fComponent->GetParams();
  fPositiveErrors = fComponent->GetPositiveErrors();
  fNegativeErrors = fComponent->GetNegativeErrors();
 
  fParams[0] = 0;
  fParams[1] = 0;
  fParams[2] = 0;
  fParams[3] = 0;
  fParams[4] = 0;
  fParams[5] = 0;
  fParams[6] = 0;

  fPositiveErrors[0] = 300;
  fPositiveErrors[1] = 300;
  fPositiveErrors[2] = 300;
  fPositiveErrors[3] = 250;
  fPositiveErrors[4] = 1;
  fPositiveErrors[5] = 1;
  fPositiveErrors[6] = 1;

  fNegativeErrors[0] = 300;
  fNegativeErrors[1] = 300;
  fNegativeErrors[2] = 300;
  fNegativeErrors[3] = -210;
  fNegativeErrors[4] = 1;
  fNegativeErrors[5] = 1;
  fNegativeErrors[6] = 1;

  fTotalDim=fParams.size();

  // Initialise starting point of mixed space grid and increments
  fStartPos = fParams;
  fIncrements = fParams;
  // Range to be evaluated is from (seed-NegativeError) to (seed+PositiveError)
  for (int ii=0; ii<fTotalDim; ii++) {
    fStartPos[ii] -=fNegativeErrors[ii];
    fIncrements[ii] = ( fPositiveErrors[ii]+fNegativeErrors[ii] ) / (double) fDiv ;
  }

  fOptimalFom = fMinFactor * (*fComponent)(fStartPos);
  Scan( fParams, 0 );

  //  std::cout << "Final " << std::endl;
  //std::cout << fParams[0] << " " << fParams[1] << " " << fParams[2] << " " << fParams[3] << " " << fParams[4] << " " << fParams[5] << " " << fParams[6] << " " << fOptimalFom << std::endl << std::endl;

  fValid = true;
  return fOptimalFom;
}

void Grid::Scan( std::vector<double> coords, int dimension )
{
  for( int spacePosition=0 ; spacePosition<fDiv; spacePosition++  )
  {
    coords[dimension] = fStartPos[dimension] + (spacePosition * fIncrements[dimension]);
    if( dimension != fTotalDim - 1 )
    {
      Scan( coords, dimension + 1 );
    }
    else
    {
      double fomHere = fMinFactor * (*fComponent)( coords );
      //  std::cout << coords[0] << " " << coords[1] << " " << coords[2] << " " << coords[3] << " " << coords[4] << " " << coords[5] << " " << coords[6] << " " << fomHere << std::endl;
      if ( fomHere <= fOptimalFom)
      {
        fOptimalFom = fomHere;
        fParams = coords;
      }
    }
  }
}



