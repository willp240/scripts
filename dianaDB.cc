#include <RAT/DU/DSReader.hh>
#include <RAT/TrackNav.hh>
#include <RAT/TrackCursor.hh>
#include <RAT/TrackNode.hh>
#include <RAT/DB.hh>
#include <TH1D.h>
#include <TFile.h>
#include <string>

//A function to plot the final displacement of the 1st Primary Particle
void mag(const std::string& filename){
  double disp,xdisp,ydisp,zdisp,fx,fy,fz,ix,iy,iz;

  TH1D* hist = new TH1D("Primary1Displacement","",10,0,100);
  TTree *tree = new TTree("T","events");
  tree->Branch("disp",&disp);
  tree->Branch("xdisp",&xdisp);
  tree->Branch("ydisp",&ydisp);
  tree->Branch("zdisp",&zdisp);

  // If this is being done on data that does not require remote database connection
  // eg.: a simple simulation with default run number (0)
  // We can disable the remote connections:
  //
  // NOTE: Don't do this if you are using real data!!!
  RAT::DB::Get()->SetAirplaneModeStatus(true);
  //  RAT::DB::Get()->LoadDefaults();
  RAT::DU::DSReader reader(filename);

  //Event Loop
  for(size_t iEv =0; iEv<reader.GetEntryCount(); iEv++){
    const RAT::DS::Entry& ds = reader.GetEntry(iEv);
    RAT::TrackNav nav(&ds);
    RAT::TrackCursor cursor =nav.Cursor(false);

    if(cursor.ChildCount()){
      cursor.GoChild(0);
      RAT::TrackNode* node = cursor.Here();
      TVector3 init_pos  = node->GetPosition();
      ix = init_pos.X();
      iy = init_pos.Y();
      iz = init_pos.Z();

      node = cursor.TrackEnd();
      TVector3 fin_pos   = node->GetPosition();
      fx = fin_pos.X();
      fy = fin_pos.Y();
      fz = fin_pos.Z();

      xdisp = ix-fx;
      ydisp = iy - fy;
      zdisp = iz - fz;

      disp = (fin_pos - init_pos).Mag();
      //   hist->Fill(disp);
      tree->Fill();
    }
  }

  //return hist;

  TFile *f = TFile::Open("mag.root","RECREATE");
  tree->Write();

  // out->Write();
}
