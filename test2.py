import string, ROOT, Utilities, os, rat

nhitsPlot = ROOT.TH1D("Nhits", "Nhits", 900, 0, 9000)
nhitsFit = ROOT.TF1("nhitsFit", "gaus", 0, 9000)

infileName = "/data/snoplus3/parkerw/ratSimulations/Jun26_SEV_2.5Terecoord_Round4/Template_Macro_SEV_*.root"

fiducialCut = 4000

count = 0
for ds, run in rat.dsreader(infileName):
    
    if ds.GetEVCount() == 0:
        continue
    
    count += 1
    if count % 1000 == 0:
        print(count)

    ev = ds.GetEV(0)
    #print(ev.GetGTID())
    if not ev.FitResultExists("positionTimeFit"):
        continue
    if not ev.GetFitResult("positionTimeFit").GetValid():
        continue

    try:
        fitPosition = ev.GetFitResult("positionTimeFit").GetVertex(0).GetPosition()
        nhits = ev.GetNhits()
        
        if fitPosition.Mag() < fiducialCut:
            nhitsPlot.Fill(nhits)
    except:
        pass

#nhitsFit = ROOT.TF1("nhitsFit", "gaus", nhitsPlot.GetMean() - 5*nhitsPlot.GetRMS(), nhitsPlot.GetMean() + 5*nhitsPlot.GetRMS())
print(nhitsPlot.GetMean(), nhitsPlot.GetRMS())
nhitsPlot.GetXaxis().SetRangeUser(nhitsPlot.GetMean() - 5*nhitsPlot.GetRMS(), nhitsPlot.GetMean() + 5*nhitsPlot.GetRMS())
nhitsPlot.Fit(nhitsFit)
nhitsPlot.Draw()
raw_input("Press 'Enter' to exit")
    
print (nhitsFit.GetParameter(1), nhitsFit.GetParError(1))
