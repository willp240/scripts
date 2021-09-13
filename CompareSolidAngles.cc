#include <RAT/DU/DSReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/MultiPDFFull.hh>
#include <RAT/DU/GroupVelocity.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/FitterPMT.hh>
#include <RAT/PDFFactory.hh>
#include <RAT/PDFMethod.hh>
#include <RAT/PDF.hh>
#include <RAT/DU/Point3D.hh>
#include <RAT/DU/LightPathStraightScint.hh>
#include <RAT/DU/LightPath.hh>

#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include "TFile.h"
#include "TPaveText.h"

#include <string>
using namespace std;

//double
//GetProbability( const RAT::FitterPMT& pmt,
//              const RAT::DS::FitVertex& vertex );

//double
//CalcProb( double distance,
//        double timeresid );

//TH1D* fSAngleX = new TH1D( "fSAngleX", "fSAngleX", 401, -100.5, 300.5 );
TGraph *fSolidX = new TGraph();
TGraph *fSolidY = new TGraph();
TGraph *fSolidZ = new TGraph();

void* CompareSolidAngles( const string& fileName )
{
  RAT::DB::Get()->SetAirplaneModeStatus(true);

  RAT::DU::DSReader dsReader( fileName );
  RAT::DS::Run run = dsReader.GetRun();
  RAT::DU::LightPathStraightScint::BeginOfRun();
  
  std::cout << "hi " << std::endl;

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..

  std::cout << "2 " << std::endl;

  for( size_t iEntry = 0; iEntry < 1; iEntry++ )//dsReader.GetEntryCount(); iEntry++ )
    {
      std::cout << "3 " << std::endl;
      const RAT::DS::Entry& rDS = dsReader.GetEntry( iEntry );
      const TVector3 startPos(0,0,0);//rDS.GetMC().GetMCParticle(0).GetPosition(); // At least 1 is somewhat guaranteed
      double eventTime = rDS.GetMC().GetMCParticle(0).GetTime();
      for( size_t iEV = 0; iEV < 1; iEV++)//rDS.GetEVCount(); iEV++ )
        {
          const RAT::DS::EV& rEV = rDS.GetEV( iEV );
	  RAT::DS::FitVertex rVertex = rEV.GetFitResult("scintFitter").GetVertex(0);
          const RAT::DS::CalPMTs& calibratedPMTs = rEV.GetCalPMTs();
	  int point = 0;
	  for( size_t iPMT = 0; iPMT < calibratedPMTs.GetCount(); iPMT++ )
	    { 
	      RAT::FitterPMT fitPMT = calibratedPMTs.GetPMT(iPMT);
	      const TVector3 pmtPos = RAT::DU::Utility::Get()->GetPMTInfo().GetPosition( fitPMT.GetID() );
	      RAT::DU::Point3D start(1, startPos.X(), startPos.Y(), startPos.Z() );
	      RAT::DU::Point3D stop(0, pmtPos.X(), pmtPos.Y(), pmtPos.Z());
	      RAT::DU::LightPath path(1.0, start, stop, RAT::DU::LightPath::InnerAV, RAT::DU::LightPath::PMT);

	      RAT::DU::Point3D origin(0);
	      TVector3 pmtNorm = origin.GetDirectionFrom(stop);

	      Double_t cosThetaAvg;
	      double solidang = RAT::DU::LightPathStraightScint::GetSolidAngle(path, pmtNorm, 10, cosThetaAvg);

	      fSolidX->SetPoint(point, pmtPos.X(), solidang);
	      fSolidY->SetPoint(point, pmtPos.Y(), solidang);
	      fSolidZ->SetPoint(point, pmtPos.Z(), solidang);

	      point++;
            }
        }
    }

  TFile *outfile = TFile::Open("fit5100_truePos.root","RECREATE");
  fSolidX->SetLineColor(kRed);
  fSolidX->Write("fSolidX");
  fSolidY->SetLineColor(kBlue);
  fSolidY->Write("fSolidY");
  fSolidZ->SetLineColor(kGreen);
  fSolidZ->Write("fSolidZ");
}

