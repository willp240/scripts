#include <RAT/Shell.hh>
#include <RAT/OptimisedComponent.hh>
#include <iostream>
#include <math.h>
#include <sstream>
#include <TVector3.h>

using namespace RAT::Optimisers;

void Shell::Initialise( const std::string& param )
{
  // Convert input string to fDiv, else make fDiv take default val
  if ( param.empty() ){
    // this sets up number of steps in each dimension. Eventually could be read from a ratdb file
    fDiv.push_back(200);//12
    fDiv.push_back(200);//12
    fMax.push_back(M_PI);
    fMax.push_back(2*M_PI);
    fMin.push_back(0);
    fMin.push_back(0);
    fShellRadius = 225;
  } else {
    //    std::stringstream strStream(param);
    //strStream >> fDiv;
  }

}

double
Shell::Minimise()
{
  fMinFactor = 1.0;
  return fMinFactor*Evaluate();
}

double
Shell::Maximise()
{
  fMinFactor = -1.0;
  return fMinFactor*Evaluate();
}

double Shell::Evaluate()
{
  // Get initial seed parameters
  fParams = fComponent->GetParams();
  fPositiveErrors = fComponent->GetPositiveErrors();
  fNegativeErrors = fComponent->GetNegativeErrors();

  fTotalDim=fParams.size();
  fScannedDim=fDiv.size();

  // Initialise starting point of mixed space grid and increments
  fStartPos = fParams;
  fIncrements = fParams;
  fScanParams = fMin;

  for (int ii=0; ii<fScannedDim; ii++) {
    fScanParams[ii] = fMin[ii];
    fIncrements[ii] = ( fMin[ii]+fMax[ii] ) / (double) fDiv.at(ii) ;
  }

  fOptimalFom = fMinFactor * (*fComponent)(fStartPos);
  Scan( fParams, 0 );
  //  std::cout << std::endl;

  //std::cout << "After Round 1" << std::endl;
  //std::cout << fParams[0] << " " << fParams[1] << " " << fParams[2] << " " << fParams[3] << " " << fParams[4] << " " << fParams[5] << " " << fOptimalFom << std::endl << std::endl;
  
  
  fValid = true;
  return fOptimalFom;
}

void Shell::Scan( std::vector<double> coords, int dimension )
{

  //Scan( coords, dimension + 1 );
  for( int spacePosition=0 ; spacePosition<fDiv.at(dimension)+1; spacePosition++  )
  {
    fScanParams[dimension] = fMin[dimension] + (spacePosition * fIncrements[dimension]);

    if( dimension != fScannedDim - 1 )
    {
      Scan( coords, dimension + 1);
    }
    else
    {
      double r = fShellRadius;
      double theta = fScanParams[0];
      double phi = fScanParams[1];
      double x = fStartPos[0] + r*cos(phi)*sin(theta);
      double y = fStartPos[1] + r*sin(phi)*sin(theta);
      double z = fStartPos[2] + r*cos(theta);
      TVector3 direction(-r*cos(phi)*sin(theta), -r*sin(phi)*sin(theta), -r*cos(theta));
      direction.Unit();
      double dirTheta = direction.Theta();
      double dirPhi = direction.Phi();

      coords[0] = x;
      coords[1] = y;
      coords[2] = z;
      coords[4] = dirTheta;
      coords[5] = dirPhi;

      double fomHere = fMinFactor * (*fComponent)( coords );

      //std::cout << theta << " " << phi << std::endl; 

      //      std::cout << coords[0] << " " << coords[1] << " " << coords[2] << " " << coords[3] << " " << coords[4] << " " << coords[5] << " " << fomHere << std::endl;
      //      std::cout << coords[0] << " " << coords[1] << " " << coords[2] << " " << coords[3] << " " << fomHere << std::endl;
      if ( fomHere <= fOptimalFom)
      {
        fOptimalFom = fomHere;
        fParams = coords;
      }
    }
  }
}
