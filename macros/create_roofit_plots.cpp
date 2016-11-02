#ifndef __CINT__
# include "RooGlobalFunc.h"
#endif // ifndef __CINT__
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"

using namespace RooFit;


void create_roofit_plots()
{
  char rootFile[] =
    "/home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/histograms/histograms_harvested_stage1_2mu1b1j.root";
  TFile *f = new TFile(rootFile);

  char histDir[]  = "2mu1b1jCategoryA_Tight/sel/evt/data_obs";
  char histName[] = "massOfOppositeChargeMuons1PinPerGeV";

  f->ls();

  std::cout << "CD to " << histDir << "\n";
  bool cdSuccessful = f->Cd(histDir);

  if (!cdSuccessful) {
    std::cout << "CD FAILED \n";
  }

  f->ls();

  std::cout << "Load histogram " << histName << "\n";
  TH1F *h1 = (TH1F *)gDirectory->Get(histName);

  if (!h1) {
    std::cout << "Histogram not found" << "\n";
  }

  std::cout << "Draw histogram " << histName << "\n";
  h1->Draw();


  RooRealVar  range("range", "range", 25, 35);
  RooDataHist data("data", "data", range, h1);

  RooRealVar breitWignerMean("breitWignerMean", "breitWignerMean", 0);
  RooRealVar breitWignerSigma("breitWignerSigma", "breitWignerSigma", 3, 0.1, 5.0);
  RooBreitWigner breitWigner("breitWigner", "breitWigner", data, breitWignerMean, breitWignerSigma);

  RooRealVar  gaussMean("gaussMean", "gaussMean", 0);
  RooRealVar  gaussSigma("gaussSigma", "gaussSigma", 3, 0.1, 5.0);
  RooGaussian gauss("gauss", "gauss", data, gaussMean, gaussSigma);

  range.setBins(10000, "cache");

  RooFFTConvPdf pdf("Breit-Wigner (X) Gauss", "Breit-Wigner (X) Gauss", data, breitWigner, gauss);

  RooPlot *frame = t.frame(Title("Breit-Wigner (x) Gauss convolution"));
  data.plotOn(frame);
  breitWigner.plotOn(frame);
  gauss.plotOn(frame, LineStyle(kDashed));

  new TCanvas("rf208_convolution", "rf208_convolution", 600, 600);
  gPad->SetLeftMargin(0.15);
  frame->GetYaxis()->SetTitleOffset(1.4);
  frame->Draw();
}
