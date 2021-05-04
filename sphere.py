import numpy as np
import ROOT

c1 = ROOT.TCanvas("c", "canvas", 800, 800)

#Create a sphere of radius 1.
radius = 6000
f3 = ROOT.TF3("sphere", "x*x+y*y+z*z-%f" % radius**2, -1.5*radius, 1.5*radius, -1.5*radius, 1.5*radius, -1.5*radius, 1.5*radius)
#f3.SetClippingBoxOn(0,0,0)

#Array of points for line
line_points = np.array([[-4087.87,-1451.33,4199.08], [-8005.05,-2445.74,-765.71]])
#We have to transpose this matrix to get rows of x, y, z
#Although this creates the right ordering it causes a TypeError in TGraph2D.
#line_points = np.transpose(line_points)
#We do this instead.
line_points = np.dstack(line_points)[0]

line = ROOT.TGraph2D(2, line_points[0], line_points[1], line_points[2])
line.SetLineColor(ROOT.kBlack)
line.SetMarkerSize(2)
f3.GetXaxis().SetTitle("X")
f3.GetYaxis().SetTitle("Y")
f3.GetZaxis().SetTitle("Z")
f3.SetFillColor(30)
f3.SetLineColor(15)
f3.Draw("GLSURF")
line.Draw("LINE SAME")
#c1.SetTheta(90)
#c1.SetPhi(0)
ROOT.gPad.Update()
raw_input("Press Enter to exit...")
