Ran:
`rat scintFitter.mac -n 300000`

Make time residuals:
`root -l -b -q 'example/root_ntuple/PlotHitTimeResidualsWithCalculator.cc+("online","testntup.root","time_residuals_online.root")'`
`root -l -b -q 'example/root_ntuple/PlotHitTimeResidualsWithCalculator.cc+("airplane","testntup.root","time_residuals_airplane.root")'`

Compare them:
`python3 compare_hit_time_residuals.py time_residuals_online.root time_residuals_airplane.root time_residual_comparison.png`