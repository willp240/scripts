void inspectFitFile(const char* filename = "fit_result_tree.root") {
    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "Could not open file.\n";
        return;
    }

    std::vector<std::string>* paramNames = nullptr;
    std::vector<double>* nomVals = nullptr;
    std::vector<double>* constrMeans = nullptr;
    std::vector<double>* constrErr = nullptr;
    std::vector<std::string>* labelsVec = nullptr;
    TMatrixT<double>* covMatrix = nullptr;

    f->GetObject("param_names", paramNames);
    f->GetObject("param_asimov_values", nomVals);
    f->GetObject("constr_mean_values", constrMeans);
    f->GetObject("constr_sigma_values", constrErr);
    f->GetObject("tex_labels", labelsVec);
    f->GetObject("covMatrix", covMatrix);

    std::cout << "param_names: " << (paramNames ? paramNames->size() : -1) << "\n";
    std::cout << "nomVals: " << (nomVals ? nomVals->size() : -1) << "\n";
    std::cout << "constrMeans: " << (constrMeans ? constrMeans->size() : -1) << "\n";
    std::cout << "constrErr: " << (constrErr ? constrErr->size() : -1) << "\n";
    std::cout << "labelsVec: " << (labelsVec ? labelsVec->size() : -1) << "\n";
    std::cout << "covMatrix: " << (covMatrix ? covMatrix->GetNrows() : -1) << " x "
              << (covMatrix ? covMatrix->GetNcols() : -1) << "\n";
}
