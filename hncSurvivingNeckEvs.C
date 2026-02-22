#include "TFile.h"
#include "TTree.h"
#include "TError.h"
#include <iostream>

void hncSurvivingNeckEvs()
{

  const char* filename = "/home/parkerw/Software/rat-tools2/FitCoordination/HighNeck/Feb19_alpha_combined.root";
  const char* treename = "output";
  TFile* f = TFile::Open(filename, "READ");
  if (!f || f->IsZombie()) {
    Error("scan_fid", "Failed to open file: %s", filename);
    return;
  }

  TTree* output = nullptr;
  f->GetObject(treename, output);
  if (!output) {
    Error("scan_fid", "Couldn't find TTree '%s' in %s", treename, filename);
    f->ls();
    f->Close();
    return;
  }

  output->SetAlias("TrueR_mm",
		   "sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4))");
  output->SetAlias("RecoR_mm",
		   "sqrt(posx*posx+posy*mcPosy+(posz-184.4)*(posz-184.4))");
  output->SetAlias("TrueE_MeV", "mcke1");
  output->SetAlias("RecoE_MeV", "energy");
  output->SetAlias("Nhits", "nhits");
  output->SetAlias("NeckHitsFrac", "necknhits/nhitsCleaned");
  output->SetAlias("QuadUnc_mm",
		   "sqrt((seedxPosError-seedxNegError)^2 + (seedyPosError-seedyNegError)^2 + (seedzPosError-seedzNegError)^2)");
  output->SetAlias("PosFOM", "posFOM/posFOM2");
  output->SetAlias("QuadMPDFDist_mm",
		   "sqrt((posx-seedx)^2+(posy-seedy)^2+(posz-seedz)^2)");
  output->SetAlias("ITR", "itr");
  output->SetAlias("MPDFUnc_mm",
		   "sqrt((posxPosError-posxNegError)^2 + (posyPosError-posyNegError)^2 + (poszPosError-poszNegError)^2)");
  
  const char* expr =
    "sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4)):"
    "sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)):"
    "mcke1:"
    "energy:"
    "nhits:"
    "necknhits / nhitsCleaned:"
    "sqrt( (seedxPosError-seedxNegError)*(seedxPosError-seedxNegError) + (seedyPosError-seedyNegError)*(seedyPosError-seedyNegError) + (seedzPosError-seedzNegError)*(seedzPosError-seedzNegError)):"
    "posFOM/posFOM2:"
    "sqrt( (posx-seedx)*(posx-seedx)+(posy-seedy)*(posy-seedy)+(posz-seedz)*(posz-seedz) ):"
    "itr:"
    "sqrt( (posxPosError-posxNegError)*(posxPosError-posxNegError) + (posyPosError-posyNegError)*(posyPosError-posyNegError) + (poszPosError-poszNegError)*(poszPosError-poszNegError))";


  const char* cut = ""
    "sqrt(posx*posx+posy*posy+(posz-184.4)*(posz-184.4)) < 6000 && "
    "sqrt(mcPosx*mcPosx+mcPosy*mcPosy+(mcPosz-184.4)*(mcPosz-184.4)) > 7000 && "
    "necknhits / nhitsCleaned < 0.004 && "
    "sqrt( (seedxPosError-seedxNegError)*(seedxPosError-seedxNegError) + (seedyPosError-seedyNegError)*(seedyPosError-seedyNegError) + (seedzPosError-seedzNegError)*(seedzPosError-seedzNegError)) < 600 && "
    "posFOM/posFOM2 > 12 && "
    "sqrt( (posx-seedx)*(posx-seedx)+(posy-seedy)*(posy-seedy)+(posz-seedz)*(posz-seedz) ) < 1000 && "
    "itr > 0.2 && "
    "sqrt( (posxPosError-posxNegError)*(posxPosError-posxNegError) + (posyPosError-posyNegError)*(posyPosError-posyNegError) + (poszPosError-poszNegError)*(poszPosError-poszNegError)) < 800 ";

  output->Scan("TrueR_mm:RecoR_mm:TrueE_MeV:RecoE_MeV:Nhits:NeckHitsFrac:QuadUnc_mm:PosFOM:QuadMPDFDist_mm:ITR:MPDFUnc_mm", cut);

  f->Close();
}
