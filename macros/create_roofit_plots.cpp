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

TH1F* loadTH1F();


double XFULLMIN = 0.;
double XFULLMAX = 200000.;
double XFITMIN  = 60000.;
double XFITMAX  = 120000.;


void create_roofit_plots()
{
  TH1F *h1 = loadTH1F();

  RooRealVar  x("x", "x", 60, 120);
  RooDataHist dataHist("dataHist", "dataHist", x, h1);

  RooRealVar breitWignerMean("breitWignerMean", "breitWignerMean", 0);
  RooRealVar breitWignerSigma("breitWignerSigma", "breitWignerSigma", 3, 0.1, 5.0);
  RooBreitWigner breitWigner("breitWigner", "breitWigner", x, breitWignerMean, breitWignerSigma);

  RooRealVar  gaussMean("gaussMean", "gaussMean", 0);
  RooRealVar  gaussSigma("gaussSigma", "gaussSigma", 3, 0.1, 5.0);
  RooGaussian gauss("gauss", "gauss", x, gaussMean, gaussSigma);

  x.setBins(10000, "cache");

  RooFFTConvPdf pdf("Breit-Wigner (X) Gauss", "Breit-Wigner (X) Gauss", x, breitWigner, gauss);

  RooPlot *frame = x.frame(Title("Breit-Wigner (x) Gauss convolution"));
  frame->GetYaxis()->SetTitleOffset(1.4);
  frame->Draw();

  dataHist.plotOn(frame);
  breitWigner.plotOn(frame);
  gauss.plotOn(frame, LineStyle(kDashed));

  TCanvas *c = new TCanvas("Breit-Wigner (x) Gauss convolution", "Breit-Wigner (x) Gauss convolution", 60, 120);
  gPad->SetLeftMargin(0.15);
}

TH1F* loadTH1F() {
  // set configuration params

  char rootFile[] =
    "/home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/histograms/histograms_harvested_stage1_2mu1b1j.root";
  TFile *f = new TFile(rootFile);

  char histDir[]  = "2mu1b1jCategoryA_Tight/sel/evt/data_obs";
  char histName[] = "massOfOppositeChargeMuons1PinPerGeV";


  // show TH1F histogram

  std::cout << "Contents of current directory: \n";
  f->ls();

  bool cdSuccessful = f->Cd(histDir);

  if (cdSuccessful) {
    std::cout << "Success: CD to " << histDir << "\n";
  } else {
    std::cout << "Failed: CD to " << histDir << "\n";
    return NULL;
  }

  std::cout << "Contents of current directory: \n";
  f->ls();

  TH1F *h1 = (TH1F *)gDirectory->Get(histName);

  if (h1) {
    std::cout << "Success: Histogram loaded. " << histName << "\n";
  } else {
    std::cout << "Failed: Histogram not found. " << histName << "\n";
  }

  return h1;
}
