#include <RAT/AdaptiveGrid.hh>
#include <RAT/OptimisedComponent.hh>
#include <iostream>
#include <math.h>
#include <sstream>
using namespace RAT::Optimisers;

void AdaptiveGrid::Initialise( const std::string& param )
{
  // Convert input string to fDiv, else make fDiv take default val
  if ( param.empty() ){
    // this sets up number of steps in each dimension. Eventually could be read from a ratdb file
    fDiv.push_back(12);//12
    fDiv.push_back(12);//12
    fDiv.push_back(12);//12
    fDiv.push_back(0);
    fDiv.push_back(20);//20
    fDiv.push_back(20);//20
  } else {
    //    std::stringstream strStream(param);
    //strStream >> fDiv;
  }

}

double
AdaptiveGrid::Minimise()
{
  fMinFactor = 1.0;
  return fMinFactor*Evaluate();
}

double
AdaptiveGrid::Maximise()
{
  fMinFactor = -1.0;
  return fMinFactor*Evaluate();
}

double AdaptiveGrid::Evaluate()
{

  // Get initial seed parameters
  fParams = fComponent->GetParams();
  fPositiveErrors = fComponent->GetPositiveErrors();
  fNegativeErrors = fComponent->GetNegativeErrors();
 
  // step arounnd seet result, but around 0 for directions
  //fParams[0] = 0;
  //fParams[1] = 0;
  //  fParams[2] = 0;
  // fParams[3] = 225;
  fParams[4] = 0;
  fParams[5] = 0;
  //  fParams[6] = 0;

  //max and min points of search in each dimension
  fPositiveErrors[0] = 600;
  fPositiveErrors[1] = 600;
  fPositiveErrors[2] = 600;
  fPositiveErrors[3] = 0;
  fPositiveErrors[4] = 1.0;
  fPositiveErrors[5] = M_PI;

  fNegativeErrors[0] = 600;
  fNegativeErrors[1] = 600;
  fNegativeErrors[2] = 600;
  fNegativeErrors[3] = 0;
  fNegativeErrors[4] = 1;
  fNegativeErrors[5] = M_PI;

  fTotalDim=fParams.size();
  // Initialise starting point of mixed space grid and increments
  fStartPos = fParams;
  fIncrements = fParams;
  // Range to be evaluated is from (seed-NegativeError) to (seed+PositiveError)
  for (int ii=0; ii<fTotalDim; ii++) {
    fStartPos[ii] = fParams[ii] - fNegativeErrors[ii];
    if(fDiv.at(ii)>0)
      fIncrements[ii] = ( fPositiveErrors[ii]+fNegativeErrors[ii] ) / (double) fDiv.at(ii) ;
    else
      fIncrements[ii] = 0;
  }

  //want to step in cos theta, but llh is evaluated with non cos
  std::vector<double> facosPos;
  facosPos = fParams;
  facosPos[4] = acos(fParams[4]);
  
  fOptimalFom = fMinFactor * (*fComponent)(facosPos);
  Scan( fParams, 0, 5 );
  fStartPos = fParams;
  
  //doing second scan over smaller range. Eventually this would all be done in a while loop so can have many iterations if needed 
  for (int ii=0; ii<fTotalDim; ii++) {
    if(fDiv.at(ii)>0){
      //step round smaller area
      fPositiveErrors[ii] = 2*fPositiveErrors[ii]/(double) fDiv.at(ii);
      fNegativeErrors[ii] = 2*fNegativeErrors[ii]/(double) fDiv.at(ii);
      fStartPos[ii] = fParams[ii] - fNegativeErrors[ii];
      fIncrements[ii] = ( fPositiveErrors[ii]+fNegativeErrors[ii] ) / (double) fDiv.at(ii) ;
    }
    else{
      fIncrements[ii] = 0;
      fPositiveErrors[ii] = 0;
      fNegativeErrors[ii] = 0;
      fStartPos[ii] = fParams[ii];
    }
  }
  
  facosPos = fParams;
  facosPos[4] = acos(fParams[4]);
  
  fOptimalFom = fMinFactor * (*fComponent)(facosPos);
  Scan( fParams, 0, 5 );
    
  facosPos = fParams;
  facosPos[4] = acos(fParams[4]);
  fParams = facosPos;

  fValid = true;
  return fOptimalFom;
}

void AdaptiveGrid::Scan( std::vector<double> coords, int dimension, int maxdim )
{
  //Scan( coords, dimension + 1 );
  for( int spacePosition=0 ; spacePosition<fDiv.at(dimension)+1; spacePosition++  )
  {
    coords[dimension] = fStartPos[dimension] + (spacePosition * fIncrements[dimension]);
    if(dimension == 5)//phi between -pi and pi
      {
      if(fParams[dimension] > M_PI)
	fParams[dimension] -= 2*M_PI;
      if(fParams[dimension] < -M_PI)
	fParams[dimension] += 2*M_PI;
    }
    if( dimension != maxdim )
    {
      Scan( coords, dimension + 1, maxdim );
    }
    else
    {
      //      std::cout << coords[0] << " " << coords[1] << " " << coords[2] <<" " << coords[3] << " "<< coords[4] <<" " << coords[5] << std::endl;

      coords[4] = acos(coords[4]);
      double fomHere = fMinFactor * (*fComponent)( coords );
      coords[4] = cos(coords[4]);

      if ( fomHere <= fOptimalFom)
      {
        fOptimalFom = fomHere;
        fParams = coords;
      }
    }
  }
}
