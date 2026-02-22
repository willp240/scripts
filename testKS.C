// compare_hit_times.C
#include <iostream>
#include <iomanip>
#include <string>

#include "TFile.h"
#include "TKey.h"
#include "TH1.h"
#include "TClass.h"

static TH1* get_hist(TFile* f, const char* name) {
  if (!f || f->IsZombie()) return nullptr;

  TObject* obj = f->Get(name);
  if (!obj) return nullptr;

  TH1* h = dynamic_cast<TH1*>(obj);
  if (!h) {
    std::cerr << "Object '" << name << "' exists but is not a TH1 (it is a "
              << obj->ClassName() << ")\n";
    return nullptr;
  }
  // Detach from file so it survives file close (optional but nice)
  h->SetDirectory(nullptr);
  return h;
}

void compare_hit_times(const char* file1,
                       const char* file2,
                       const char* histname = "hit_times",
                       bool include_under_over = true,
                       double print_eps = 0.0 // set e.g. 1e-12 to print only bins that differ
) {
  TFile f1(file1, "READ");
  TFile f2(file2, "READ");

  if (f1.IsZombie()) { std::cerr << "Failed to open " << file1 << "\n"; return; }
  if (f2.IsZombie()) { std::cerr << "Failed to open " << file2 << "\n"; return; }

  TH1* h1 = get_hist(&f1, histname);
  TH1* h2 = get_hist(&f2, histname);

  if (!h1) { std::cerr << "Could not find histogram '" << histname << "' in " << file1 << "\n"; return; }
  if (!h2) { std::cerr << "Could not find histogram '" << histname << "' in " << file2 << "\n"; return; }

  h1->Scale(10000/h1->Integral());
  h2->Scale(10000/h2->Integral());
  
  // Basic checks
  std::cout << "h1: " << h1->ClassName() << "  nbins=" << h1->GetNbinsX()
            << "  range=[" << h1->GetXaxis()->GetXmin() << ", " << h1->GetXaxis()->GetXmax() << "]\n";
  std::cout << "h2: " << h2->ClassName() << "  nbins=" << h2->GetNbinsX()
            << "  range=[" << h2->GetXaxis()->GetXmin() << ", " << h2->GetXaxis()->GetXmax() << "]\n";

  std::cout << "\nSummary:\n";
  std::cout << "  h1: entries=" << h1->GetEntries()
            << " integral=" << h1->Integral(0, h1->GetNbinsX()+1)
            << " mean=" << h1->GetMean()
            << " rms=" << h1->GetRMS() << "\n";
  std::cout << "  h2: entries=" << h2->GetEntries()
            << " integral=" << h2->Integral(0, h2->GetNbinsX()+1)
            << " mean=" << h2->GetMean()
            << " rms=" << h2->GetRMS() << "\n";

  // Compare binning in detail
  bool same_binning = true;
  if (h1->GetNbinsX() != h2->GetNbinsX()) same_binning = false;
  if (h1->GetXaxis()->GetXmin() != h2->GetXaxis()->GetXmin()) same_binning = false;
  if (h1->GetXaxis()->GetXmax() != h2->GetXaxis()->GetXmax()) same_binning = false;

  const int nb1 = h1->GetNbinsX();
  const int nb2 = h2->GetNbinsX();
  const int nb = std::min(nb1, nb2);

  // If variable binning, compare edges bin-by-bin
  for (int i = 1; i <= nb; ++i) {
    double lo1 = h1->GetXaxis()->GetBinLowEdge(i);
    double lo2 = h2->GetXaxis()->GetBinLowEdge(i);
    double up1 = h1->GetXaxis()->GetBinUpEdge(i);
    double up2 = h2->GetXaxis()->GetBinUpEdge(i);
    if (lo1 != lo2 || up1 != up2) { same_binning = false; break; }
  }

  std::cout << "\nBinning identical? " << (same_binning ? "YES" : "NO") << "\n\n";

  // KS test (a couple of modes)
  std::cout << "KolmogorovTest default: " << h1->KolmogorovTest(h2) << "\n";
  std::cout << "KolmogorovTest option \"M\" (normalize to same integral): " << h1->KolmogorovTest(h2, "M") << "\n";
  std::cout << "KolmogorovTest option \"D\" (print distance only): " << h1->KolmogorovTest(h2, "D") << "\n\n";

  // Print per-bin comparison
  const int first = include_under_over ? 0 : 1;
  const int last1  = include_under_over ? nb1 + 1 : nb1;
  const int last2  = include_under_over ? nb2 + 1 : nb2;
  const int last   = std::min(last1, last2);

  std::cout << std::fixed << std::setprecision(6);
  std::cout << "bin  [low, up]        c1           c2           diff         e1           e2\n";
  std::cout << "--------------------------------------------------------------------------------\n";

  for (int i = first; i <= last; ++i) {
    double low = (i==0) ? -INFINITY : h1->GetXaxis()->GetBinLowEdge(i);
    double up  = (i==nb1+1) ?  INFINITY : h1->GetXaxis()->GetBinUpEdge(i);

    double c1 = h1->GetBinContent(i);
    double c2 = h2->GetBinContent(i);
    double d  = c1 - c2;
    double e1 = h1->GetBinError(i);
    double e2 = h2->GetBinError(i);

    if (print_eps > 0.0 && std::abs(d) <= print_eps) continue;

    std::cout << std::setw(3) << i << "  ["
              << std::setw(10) << low << ", " << std::setw(10) << up << "]  "
              << std::setw(12) << c1 << "  "
              << std::setw(12) << c2 << "  "
              << std::setw(12) << d  << "  "
              << std::setw(12) << e1 << "  "
              << std::setw(12) << e2 << "\n";
  }

  // If nbins differ, warn about the tail
  if (nb1 != nb2) {
    std::cout << "\nWARNING: Different nbinsX (" << nb1 << " vs " << nb2
              << "). Printed up to the smaller bin range.\n";
  }
}
