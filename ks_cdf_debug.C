// ks_cdf_debug.C
#include <iostream>
#include <iomanip>
#include <cmath>
#include "TFile.h"
#include "TH1.h"

static TH1* get_hist(TFile& f, const char* name) {
  TObject* obj = f.Get(name);
  if (!obj) return nullptr;
  TH1* h = dynamic_cast<TH1*>(obj);
  if (!h) return nullptr;
  h->SetDirectory(nullptr);
  return h;
}

void ks_cdf_debug(const char* file1,
                  const char* file2,
                  const char* histname = "hit_times",
                  bool normalize = true,      // normalize to 1 over bins 1..nbins (shape-only)
                  int print_window = 5        // print +/- this many bins around max
) {
  TFile f1(file1,"READ"), f2(file2,"READ");
  if (f1.IsZombie() || f2.IsZombie()) {
    std::cerr << "Error opening one of the files.\n";
    return;
  }

  TH1* h1 = get_hist(f1, histname);
  TH1* h2 = get_hist(f2, histname);
  if (!h1 || !h2) {
    std::cerr << "Missing histogram '" << histname << "' in one of the files.\n";
    return;
  }

  const int nb1 = h1->GetNbinsX();
  const int nb2 = h2->GetNbinsX();
  if (nb1 != nb2) {
    std::cerr << "Different nbins: " << nb1 << " vs " << nb2 << "\n";
    return;
  }

  // Use only in-range bins (exclude under/overflow) for CDF + KS
  const int first = 1;
  const int last  = nb1;

  double sum1 = 0.0, sum2 = 0.0;
  for (int i = first; i <= last; ++i) {
    sum1 += h1->GetBinContent(i);
    sum2 += h2->GetBinContent(i);
  }

  if (sum1 <= 0 || sum2 <= 0) {
    std::cerr << "One histogram has zero (in-range) sum: sum1=" << sum1 << " sum2=" << sum2 << "\n";
    return;
  }

  std::cout << std::fixed << std::setprecision(8);
  std::cout << "In-range sums (bins 1.." << last << "): sum1=" << sum1 << "  sum2=" << sum2 << "\n";
  std::cout << "normalize=" << (normalize ? "true (shape CDF)" : "false (raw counts CDF)") << "\n\n";

  double cdf1 = 0.0, cdf2 = 0.0;
  double maxD = -1.0;
  int    maxBin = -1;

  for (int i = first; i <= last; ++i) {
    double v1 = h1->GetBinContent(i);
    double v2 = h2->GetBinContent(i);

    if (normalize) {
      v1 /= sum1;
      v2 /= sum2;
    }

    cdf1 += v1;
    cdf2 += v2;

    const double D = std::fabs(cdf1 - cdf2);
    if (D > maxD) {
      maxD = D;
      maxBin = i;
    }
  }

  const double xlo = h1->GetXaxis()->GetBinLowEdge(maxBin);
  const double xhi = h1->GetXaxis()->GetBinUpEdge(maxBin);
  const double xct = h1->GetXaxis()->GetBinCenter(maxBin);

  std::cout << "Max |CDF1-CDF2| = " << maxD << " at bin " << maxBin
            << "  x=[" << xlo << "," << xhi << "]  center=" << xct << "\n\n";

  // Print a window around the culprit bin
  int lo = std::max(first, maxBin - print_window);
  int hi = std::min(last,  maxBin + print_window);

  std::cout << "bin   x_center     p1(or c1)     p2(or c2)     CDF1         CDF2         |diff|\n";
  std::cout << "-----------------------------------------------------------------------------------\n";

  cdf1 = 0.0; cdf2 = 0.0;
  for (int i = first; i <= last; ++i) {
    double v1 = h1->GetBinContent(i);
    double v2 = h2->GetBinContent(i);

    if (normalize) {
      v1 /= sum1;
      v2 /= sum2;
    }

    cdf1 += v1;
    cdf2 += v2;

    if (i >= lo && i <= hi) {
      std::cout << std::setw(3) << i << "  "
                << std::setw(9) << h1->GetXaxis()->GetBinCenter(i) << "  "
                << std::setw(12) << v1 << "  "
                << std::setw(12) << v2 << "  "
                << std::setw(11) << cdf1 << "  "
                << std::setw(11) << cdf2 << "  "
                << std::setw(11) << std::fabs(cdf1 - cdf2)
                << (i == maxBin ? "  <-- max" : "")
                << "\n";
    }
  }

  // Compare to ROOT's own KS distance option (for sanity)
  std::cout << "\nROOT KolmogorovTest(\"D\") = " << h1->KolmogorovTest(h2, "D") << "\n";
  std::cout << "ROOT KolmogorovTest(\"MD\")= " << h1->KolmogorovTest(h2, "MD") << "  (normalize + distance)\n";
}
