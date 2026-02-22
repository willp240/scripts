

int modify()
{

  auto fin  = TFile::Open("current.root", "READ");
  auto fout = TFile::Open("current_modified.root", "RECREATE");
  
  auto hA = (TH1D*)fin->Get("hApplied")->Clone("hApplied");
  auto hS = (TH1D*)fin->Get("hSet")->Clone("hSet");
  
  hA->SetBinContent(1, 0);
  hS->SetBinContent(1, 0);
  hA->SetBinError(1, 0);
  hS->SetBinError(1, 0);
  
  fout->cd();
  hA->Write();
  hS->Write();
  
  fout->Close();
  fin->Close();
  return 0;
}
