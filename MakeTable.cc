void MakeTable(){

  TFile* file = new TFile("/home/patonj/DirPosFitting/MakePDFs/PDFs/PDF_TimeAngle_6MeV_0p5gL_fullVolume_FullFill.root");
  TH2D * pdf = (TH2D*)file->Get("hCosVSTime6");
  //  double scale = pdf->GetEntries();
  //pdf->Scale(1/scale);

  TH2D * newPDF = new TH2D("newPDF", "PDF put into the 2D fitter",401,-100.5,300.5,5,-1.0,1.0);
  
  const float binWidth = 1.0;      // ns - width of bins in PDF                        
  const float lowLim = -100.0;     // ns - lower end of PDF                          
  const float upLim  =  300.0;     // ns - upper end of PDF                          
  const float lowLimFlat = -20.0;   // ns - time below which PDF is extrapolated flat
  const float lowLimVal  = -90.0;  // ns - time of lower value used in extrapolation
  const float upLimFlat = 234.0;   // ns - time above which PDF is extrapolated flat

  const unsigned int numBins = (upLim-lowLim)/binWidth + 1;
  const unsigned int lowBinFlat = (lowLimFlat-lowLim)/binWidth + 1;
  const unsigned int lowBinVal  = (lowLimVal -lowLim)/binWidth + 1;
  const unsigned int upBinFlat  = (upLimFlat -lowLim)/binWidth + 1;

  double lowFlat = 0.;
  double highFlat = 0.;
  
  //Find low and high flat values
  for(int ybin1 = 1;ybin1<6;ybin1++){

    lowFlat += pdf->GetBinContent(lowBinVal,ybin1)/5.;
    highFlat += pdf->GetBinContent(upBinFlat,ybin1)/5.;
  }
  
  //Make file
  ofstream myfile;
  myfile.open ("FIT_DIRECTION_POSITION_6MeV_fullVolume_10bin.ratdb");

  
  //Print first table with times  
  myfile<<"{"<<endl;
  myfile<<"type : \"FIT_DIRECTION_POSITION_6MeV_fullVolume\","<<endl;
  myfile<<"index: \"labppo_0p5_scintillator\","<<endl;
  myfile<<"version: 1,"<<endl;
  myfile<<"num_pdfs: 20,"<<endl;
  myfile<<"pass: 0,"<<endl;
  myfile<<"run_range: [0, 0],"<<endl;
  myfile<<"timestamp: \"\","<<endl;
  myfile<<"comment: \"\","<<endl;
  myfile<<"time: [";
  for(int xbin = 1;xbin<402;xbin++){
    myfile<<pdf->GetXaxis()->GetBinCenter(xbin)<<".00, ";
  }
  myfile<<"],"<<endl;
  myfile<<"}"<<endl;
  myfile<<endl;

  for(int ybin = 1;ybin<6;ybin++){
    
    myfile<<"{"<<endl;
    myfile<<"type : \"FIT_DIRECTION_POSITION_6MeV_fullVolume\","<<endl;
    myfile<<"version: 1,"<<endl;
    myfile<<"index: \"labppo_0p5_scintillator_20PDFBand"<<ybin-1<<"\","<<endl;
    double lowBin = pdf->GetYaxis()->GetBinLowEdge(ybin);
    double highBin = pdf->GetYaxis()->GetBinWidth(ybin) + lowBin;
    double midBin = (lowBin + highBin)/2.;
    myfile<<"cos_angle_range:  ["<<lowBin<<", "<<midBin<<", "<<highBin<<"],"<<endl;
    myfile<<"run_range: [0, 0],"<<endl;
    myfile<<"pass: 0,"<<endl;
    myfile<<"production: false,"<<endl;
    myfile<<"timestamp: \"\","<<endl;
    myfile<<"comment: \"Generated with RAT 6.18.13\","<<endl;
    myfile<<"probability: [";
    for(int xbin = 1;xbin<402;xbin++){

      if (xbin < lowBinFlat)
      	{
	  newPDF->Fill(pdf->GetXaxis()->GetBinCenter(xbin),pdf->GetYaxis()->GetBinCenter(ybin),lowFlat);
	  myfile << std::fixed << std::setprecision(2)<< float(lowFlat) << ", ";
      	}
      //Approximate late hits to a flat distribution 
      else if (xbin > upBinFlat)
      	{
	  newPDF->Fill(pdf->GetXaxis()->GetBinCenter(xbin),pdf->GetYaxis()->GetBinCenter(ybin),highFlat);
	  myfile << std::fixed << std::setprecision(2)<< float(highFlat) << ", ";
	}
      else
      	{
	  newPDF->Fill(pdf->GetXaxis()->GetBinCenter(xbin),pdf->GetYaxis()->GetBinCenter(ybin),pdf->GetBinContent(xbin,ybin));
	  myfile << std::fixed << std::setprecision(2)<< float(pdf->GetBinContent(xbin,ybin)) << ", ";
	}
      
      
    }
    myfile<<"],"<<endl;
    myfile<<"}"<<endl;
    myfile<<endl;
  }

  myfile.close();

  TFile* newfile = new TFile("6MeV_0p5gL_fullVolume_FullFill_PDFAltered_5bin.root","RECREATE");
  newPDF->Write();
  
}
