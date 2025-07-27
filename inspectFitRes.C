void inspectFitRes(const char* filename = "fit_result_tree.root") {
    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "Could not open file.\n";
        return;
    }

    std::vector<std::string>* paramNames = nullptr;
    std::vector<double>* paramVals = nullptr;
    std::vector<double>* paramErr = nullptr;
    TMatrixT<double>* covMatrix = nullptr;

    f->GetObject("paramNames", paramNames);
    f->GetObject("paramVals", paramVals);
    f->GetObject("paramErr", paramErr);
    f->GetObject("covMatrix", covMatrix);

    std::cout << "paramNames: " << (paramNames ? paramNames->size() : -1) << "\n";
    std::cout << "paramVals: " << (paramVals ? paramVals->size() : -1) << "\n";
    std::cout << "paramErr: " << (paramErr ? paramErr->size() : -1) << "\n";
    std::cout << "covMatrix: " << (covMatrix ? covMatrix->GetNrows() : -1) << " x "
              << (covMatrix ? covMatrix->GetNcols() : -1) << "\n";
}
