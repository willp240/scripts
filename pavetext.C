void pavetext() {

  TH1D *h_x = new TH1D("h_x", "Event Distribution", 100, -1000, 1000);
  TRandom3 *r3=new TRandom3(4);
  for( int i_event = 0; i_event < 1000; i_event++)
    {
      double eve_x = r3->Gaus(0, 100);
      h_x->Fill(eve_x);
    }

  TCanvas *c = new TCanvas("c");

  TPaveText *pt = new TPaveText(-100,20,100,40); 
  pt->AddText("SNO+ Preliminary");

  h_x->Draw();
  pt->Draw();
}
