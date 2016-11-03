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


// interface

TH1F *loadTH1F(
  char  categoryName[]
  );

bool createRooFit(
  TH1F * histogram,
  char  categoryName[]
  );


// entry-point

bool create_roofit_plots()
{
  char categoryNames[20][20] =  {
    "CategoryA",
    "CategoryB"
  };

  for (char categoryName[20] : categoryNames) {
    TH1F *histogram = loadTH1F(categoryName);
    return createRooFit(histogram, categoryName);
  }
}

// methods

bool createRooFit(
  TH1F *histogram,
  char  categoryName[]
  )
{
  // create roofit

  TCanvas *canvas = new TCanvas();

  RooRealVar x("x", "x", 80, 100);

  std::cout << "x (initial).\n";
  x.Print();

  RooDataHist dataHist("dataHist", "dataHist", x, Import(*histogram));

  std::cout << "x (after data load from TH1F).\n";
  x.Print();


  // RooRealVar breitWignerMean("breitWignerMean", "breitWignerMean", 90);
  // RooRealVar breitWignerSigma("breitWignerSigma", "breitWignerSigma", 3, 0.1, 5.0);
  // RooBreitWigner breitWigner("breitWigner", "breitWigner", x, breitWignerMean, breitWignerSigma);

  RooRealVar  mean("mean", "mean", 90);
  RooRealVar  sigma("sigma", "sigma", 3, 0.01, 5);
  RooGaussian model("gauss", "gauss", x, mean, sigma);


  std::cout << "Before fit. \n";
  mean.Print();
  sigma.Print();
  model.Print();

  model.fitTo(dataHist);

  std::cout << "After fit. \n";
  mean.Print();
  sigma.Print();
  model.Print();


  RooPlot *xframe = x.frame();

  dataHist.plotOn(xframe);
  model.plotOn(xframe);
  xframe->Draw();

  char pdfPath[100];
  sprintf(pdfPath, "/home/margusp/roofits/%s.pdf", categoryName);

  cout << "pdfPath is: " << pdfPath << "\n";

  canvas->Print(pdfPath, "pdf");

  // pinned
  // RooDataSet *data = model.generateBinned(x, 10000);

  return true;

  //
  // RooFFTConvPdf pdf("Breit-Wigner (X) Gauss", "Breit-Wigner (X) Gauss", x, breitWigner, gauss);
  //
  // RooPlot *frame = x.frame(Title("Breit-Wigner (x) Gauss convolution"));
  // dataHist.plotOn(frame);
  // breitWigner.plotOn(frame);
  // gauss.plotOn(frame, LineStyle(kDashed));
  //
  // TCanvas *c = new TCanvas("Breit-Wigner (x) Gauss convolution", "Breit-Wigner (x) Gauss convolution", 600, 600);
  // c->SetLeftMargin(0.15);
  // c->Divide(2);
  //
  // frame->GetYaxis()->SetTitleOffset(1.4);
  // frame->Draw();
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

  TH1F *histogram = (TH1F *)gDirectory->Get(histName);

  if (histogram) {
    std::cout << "Success: Histogram loaded. " << histName << "\n";
  } else {
    std::cout << "Failed: Histogram not found. " << histName << "\n";
  }

  return histogram;
}
