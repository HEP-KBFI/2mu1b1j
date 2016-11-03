#ifndef __CINT__
# include "RooGlobalFunc.h"
#endif // ifndef __CINT__
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooNumConvPdf.h"
#include "RooBreitWigner.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"

using namespace RooFit;


// interface

TH1F* loadTH1F(
  string year,
  string categoryName
  );

bool createRooFit(
  TH1F  * histogram,
  string year,
  string categoryName,
  float range[]
  );


// entry-point

bool create_roofit_plots()
{
  string years[] = {
    "2015",
    "2016"
  }

  string categoryNames[] = {
    "CategoryA",
    "CategoryB"
  };

  // value, range
  float ranges[][3] = {
    {  3.0,  1.0,   5.0 },
    { 10.0, 8.00, 12.00 },
    { 28.5, 24.5,  32.5 }
  };

  for (string year : years) {
    for (string categoryName : categoryNames) {
      cout << "Current category: " << categoryName << "\n";
      TH1F *histogram = loadTH1F(year, categoryName);

      for (auto range : ranges) {
        createRooFit(histogram, year, categoryName, range);
      }
    }
  }

  return true;
}

// methods

bool createRooFit(
  TH1F  *histogram,
  string year,
  string categoryName,
  float  range[] // 0 - value, 1 - begin, 2 - end
  )
{
  // create roofit

  TCanvas *canvas = new TCanvas();

  canvas->Divide(2);

  RooRealVar x("x", "x", range[1], range[2]);

  std::cout << "x (initial).\n";
  x.Print();

  RooDataHist dataHist("dataHist", "dataHist", x, Import(*histogram));

  std::cout << "x (after data load from TH1F).\n";
  x.Print();


  // RooRealVar mean1("breitWigner mean", "breitWigner mean", range[0], range[1], range[2]);
  // RooRealVar sigma1("breitWigner sigma", "breitWigner sigma", 0.01, 0.01, 10);
  // RooBreitWigner model1("breitWigner", "breitWigner", x, mean1, sigma1);
  //
  // RooRealVar  mean2("gauss mean", "gauss mean", 0.001);
  // RooRealVar  sigma2("gauss sigma", "gauss sigma", 1, 0.01, 10);
  // RooGaussian model2("gauss", "gauss", x, mean2, sigma2);


  // RooNumConvPdf convolution("convolution", "convolution", x, model1, model2);
  // convolution.fitTo(dataHist);
  // model1.fitTo(dataHist);
  // model2.fitTo(dataHist);

  RooPlot *xframe = x.frame();

  // convolution.plotOn(xframe);
  // model1.plotOn(xframe);
  // model2.plotOn(xframe);

  dataHist.plotOn(xframe);

  // convolution.plotOn(xframe);
  xframe->Draw();

  string pdfPath = string("/home/margusp/roofits/") + year + "_" + categoryName + "_" + to_string(range[0]) + ".pdf";

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

TH1F* loadTH1F(
  string year,
  string categoryName
  )
{
  // set configuration params

  string rootFile =
    string("/home/margusp/analysis2mu1b1j/") +
    year +
    "/2016Oct28_v1/histograms/histograms_harvested_stage1_2mu1b1j.root";

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
