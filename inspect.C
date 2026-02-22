void inspect()
{
    TFile *f = TFile::Open("photontruthntuple.root");
    TTree *t = (TTree *)f->Get("output");

    std::vector<std::vector<double>> *emit = nullptr;
    std::vector<double> *emitt = nullptr;
    std::vector<double> *hittimes = nullptr;
    t->SetBranchAddress("emitposs", &emit);
    t->SetBranchAddress("emittimes", &emitt);
    t->SetBranchAddress("hittimes", &hittimes);

    for (int i = 0; i < 10; i++)
    { // first 5 entries
        t->GetEntry(i);
        std::cout << "Entry " << i << " " << emitt->size() << "\n";

        for (size_t p = 0; p < emitt->size(); ++p)
        {
            std::cout << "  Photon " << p << ": " << emitt->at(p) << " ";
            for (double v : emit->at(p))
                std::cout << v << " ";
            std::cout << std::endl;
        }
    }
}
