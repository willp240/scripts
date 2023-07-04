import string, ROOT, Utilities, os

oneDimPlot = ROOT.TH1D("oneDimPlot", "Radial Bias vs. Effective Speed in 1 Dimension", 2000, -1000.0, 1000.0)

oneDimPlot.Fill(68.47069599776964, 191.0)
oneDimPlot.Fill(62.84321068826579, 190.5)
oneDimPlot.Fill(55.70483949866414, 190.0)
oneDimPlot.Fill(47.23682525811548, 189.5)
oneDimPlot.Fill(41.17466758591516, 189.0)
oneDimPlot.Fill(29.064942461997028, 188.5)
oneDimPlot.Fill(22.10748030696552, 188.0)
oneDimPlot.Fill(16.22308162087096, 187.5)
oneDimPlot.Fill(8.04567740270192, 187.0)
oneDimPlot.Fill(-0.8478276889289642, 186.5)
#oneDimPlot.Fill(-12.379859278046084, 186.0)
oneDimPlot.Fill(-12.108435709245512, 185.5)
oneDimPlot.Fill(-25.41464076226021, 185.0)

linearFit = ROOT.TF1("linearFit", "pol1", -1000.0, 1000.0)

oneDimPlot.Fit(linearFit, "R")

oneDimPlot.Draw()
raw_input("Press 'Enter' to exit")  
print(linearFit.GetParameter(0))


