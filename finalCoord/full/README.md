# MultiPDF Coordinator for Full Fill
This folder contains the files needed to coordinate the MultiPDF fitter - a set of histograms of the hit PMT time residuals based on MC information.
There are two methods for running the coordinator:

-------------------------

1) The standard method, albeit slightly differently to other coordinators. As extra options are needed you can't use fitcoordinate in the directory above this. Instead, firstly directly run ProduceData.py in this directory:

   python ProduceData.py [options]

The following options exist:
- [-g]: Geometry File to use ... location relative to rat/data/ (e.g., geo/snoplusnative_water.geo)
- [-l]: Load an extra .ratdb directory
- [-s]: Scintillator Material to use (e.g., labppo_scintillator)
- [-t]: Photon thinning factor to use
- [-c]: File with defaults (defaults itself to ../defaults.ini)

- This generates 20 rootfiles containing 5000 events each at the AV centre (the energy is automatically set by the material chosen). The Template_Macro.mac file is used to generate the simulation, this could be modified if you wanted to run with a different configuration. Once the production script is complete, the analysis script will NOT begin automatically - it must be run by the user. To do this, while still in this folder, run the command:

    python AnalyseData.py [options]

The following options exist:
- [-s]: Scintillator Material to use (e.g., labppo_scintillator)
- [-v]: Use an updated group velocity for the scintillator material
- [-m]: MultiPDF splitting coordinate ("r","d",or"z" for event radius, vertex-PMT distance, or PMT height respectively)
- [-r]: Range of coordinate for which each PDF corresponds to (eg. -m "r" -r 1500,4500 would split the PDFs into three different PDFs for events inside 1500 mm, between 1500 and 4500 mm, and outside 4500 mm). Note the option "-r" is read in as a string and deliminated on "," or " " so must be in the format (a,b,c,..,n) or (a b c .. n). If either option "m" or "r" are not given, the default behaviour is to make a single PDF

- The coordination results are written to screen - there will be a complete RATDB entry that should be placed in the ET1D_MultiPDF.ratdb file located in rat/data, replacing any existing entry with the same index and number of PDFs.


-------------------------

2) batch method, which needs to be invoked differently from the standard method:
- in this folder, run the command:

    python ProduceData.py [options]

The options for this script are: [-g], [-l], [-s], [-c], and [-t] as specified above, as well as:
- [-b]: Batch configuration file ... absolute location

There already exists a basic "batch.config" file in the "FitCoordination" folder.  However, users may specify their own configuration using that file as a template, and then provide the filename of their new configuration file here.  

- once the production script is complete, the analysis script will NOT begin automatically - it must be run by the user.  To do this, while still in this folder, run the command:

    python AnalyseData.py [options]

Available options for this script are [-b] [-s], [-v], [m], and [-r] as described above.

The coordination results are written to the Batch logfile - there will be a complete RATDB entry that should be placed in the ET1D_MultiPDF.ratdb file located in rat/data, replacing any existing entry with the same index.  

-------------------------

