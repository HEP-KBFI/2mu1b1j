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

TH1F* loadTH1F(
  string categoryName
  );

bool createRooFit(
  TH1F  *histogram,
  string categoryName
  );


// entry-point

bool create_roofit_plots()
{
  string categoryNames[2] =  {
    "CategoryA",
    "CategoryB"
  };

  for (string categoryName : categoryNames) {
    cout << "Current category: " << categoryName << "\n";
    TH1F *histogram = loadTH1F(categoryName);
    createRooFit(histogram, categoryName);
  }

  return true;
}

// methods

bool createRooFit(
  TH1F  *histogram,
  string categoryName
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

  RooRealVar  mean("mean", "mean", 91);
  RooRealVar  sigma("sigma", "sigma", 2, 0.01, 5);
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

  string pdfPath = string("/home/margusp/roofits/") + categoryName + string(".pdf");

  cout << "pdfPath is: " << pdfPath << "\n";

  canvas->Print(pdfPath.data(), "pdf");

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

TH1F* loadTH1F(string categoryName) {
  // set configuration params

  string rootFile =
    "/home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/histograms/histograms_harvested_stage1_2mu1b1j.root";
  TFile *f = new TFile(rootFile.data());

  string histDir  = string("2mu1b1j") + categoryName + string("_Tight/sel/evt/data_obs");
  string histName = "massOfOppositeChargeMuons1PinPerGeV";


  // show TH1F histogram

  std::cout << "Contents of current directory: \n";
  f->ls();

  bool cdSuccessful = f->Cd(histDir.data());

  if (cdSuccessful) {
    std::cout << "Success: CD to " << histDir << "\n";
  } else {
    std::cout << "Failed: CD to " << histDir << "\n";
    return NULL;
  }

  std::cout << "Contents of current directory: \n";
  f->ls();

  TH1F *histogram = (TH1F *)gDirectory->Get(histName.data());

  if (histogram) {
    std::cout << "Success: Histogram loaded. " << histName << "\n";
  } else {
    std::cout << "Failed: Histogram not found. " << histName << "\n";
  }

  return histogram;
}
