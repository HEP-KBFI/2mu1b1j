
// Intro
// =====
//
// This program creates Roofit plots from TH1F histograms for 2mu1b1j analysis.
//
// If you have any questions, send an email to margus@wave.ee
//
//
// Architecture
// ============
//
// * create_roofit_plots() - entry method that generates plots for different years, GeV ranges
// * loadTH1F() - loads TH1F from .root file
// * createRooFit() - createsRooFit from loaded TH1F


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
  };

  string categoryNames[] = {
    "CategoryA",

    // "CategoryARelaxed",
    "CategoryACompare",

    // "CategoryACompareRelaxed",
    // "CategoryB",
    // "CategoryBRelaxed",
    // "CategoryBCompare",
    // "CategoryBCompareRelaxed",
    // "CategoryC",
    // "CategoryCRelaxed",
    // "CategoryCCompare",
    // "CategoryCCompareRelaxed"
  };

  // value, range
  float ranges[][3] = {
    {  3.0,  1.0,   5.0 },
    { 10.0, 8.00, 12.00 },
    { 28.5, 24.5,  32.5 },
    { 91.0, 60.0, 120.0 },
    {    0,  0.0, 120.0 }
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

bool createRooFit(
  TH1F  *histogram,
  string year,
  string categoryName,
  float  range[] // 0 - value, 1 - begin, 2 - end
  )
{
  if (histogram) {
    cout << "Histogram loaded. \n";
  } else {
    cout << "Histogram not loaded. \n";
    return false;
  }

  // create roofit

  TCanvas *canvas = new TCanvas();

  canvas->Divide(2);

  RooRealVar x("x", "x", range[1], range[2]);

  // std::cout << "x (initial).\n";
  // x.Print();
  //
  // RooDataHist dataHist("dataHist", "dataHist", x, Import(*histogram));
  //
  // std::cout << "x (after data load from TH1F).\n";
  // x.Print();


  RooRealVar mean1("breitWigner mean", "breitWigner mean", range[0], range[1], range[2]);
  RooRealVar sigma1("breitWigner sigma", "breitWigner sigma", range[0], range[1], range[2]);
  RooBreitWigner model1("breitWigner", "breitWigner", x, mean1, sigma1);

  RooRealVar  mean2("gauss mean", "gauss mean", 0);
  RooRealVar  sigma2("gauss sigma", "gauss sigma", range[0], range[1], range[2]);
  RooGaussian model2("gauss", "gauss", x, mean2, sigma2);

  RooFFTConvPdf convolution("convolution", "breitWigner (X) gauss", x, model1, model2);

  x.setBins(10000, "cache");
  RooDataSet *generatedData = convolution.generate(x, 10000);
  convolution.fitTo(*generatedData);


  RooPlot *frame = x.frame(Title("breitWigner (x) gauss convolution"));

  generatedData->plotOn(frame);
  convolution.plotOn(frame, LineColor(kRed));
  model2.plotOn(frame, LineColor(kBlue));
  model1.plotOn(frame, LineColor(kGreen));

  // generatedData->plotOn(xframe);

  frame->Draw();

  string pdfPath = string("/home/margusp/roofits/") +
                   year +
                   "_" +
                   to_string(range[0]) +
                   "_" +
                   to_string(range[1]) +
                   "-" +
                   to_string(range[2]) +
                   "_" +
                   categoryName +
                   ".pdf";

  cout << "pdfPath is: " << pdfPath << "\n";

  canvas->Print(pdfPath.data(), "pdf");


  return true;
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

  string histDir = string("2mu1b1j") + categoryName + string("_Tight/sel/evt/data_obs");

  string histName = "massOfOppositeChargeMuons1PinPerGeV";

  // string histName = "massOfOppositeChargeMuons10PinsPerGev";


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
