
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
  float  range[]
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

  // value, rangeStart, rangeEnd
  float ranges[][3] = {
    {  3.0,  1.0,   5.0 },
    { 10.0, 8.00, 12.00 },
    { 28.5, 24.5,  32.5 },
    { 91.0, 60.0, 120.0 },
    { 91.0,  0.0, 120.0 }
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


  // Create roofit variable

  RooRealVar x("x", "x", range[1], range[2]);


  // Import histogram from x range to dataHist

  // RooDataHist dataHist("dataHist", "dataHist", x, Import(*histogram));


  // Set model for signal

  RooRealVar signalMean("signalMean", "signalMean", range[0], range[1], range[2]);
  RooRealVar signalWidth("signalWidth", "signalWidth", 1, 0.1, range[0] / 5);
  RooBreitWigner signal("gauss", "gauss", x, signalMean, signalWidth);


  // Set model for background

  RooRealVar lambda("lambda", "slope", 0, 0., 120.);
  RooExponential background("expo", "exponential PDF", x, lambda);


  // Signal + background

  RooRealVar signalEventsCount("signalEventsCount", "#signal events", 200, 0., 10000);
  RooRealVar backgroundEventsCount("backgroundEventsCount", "#background events", 800, 0., 10000);

  RooAddPdf signalAndBackground(
    "signalAndBackground",
    "gauss + lambda",
    RooArgList(signal,            background),
    RooArgList(signalEventsCount, backgroundEventsCount)
    );

  // Make fitting

  RooDataSet *data = signalAndBackground.generate(x, 2000);
  signalAndBackground.fitTo(*data, Extended());


  // Draw fitted result onto RooPlot

  RooPlot *frame = x.frame();
  data->plotOn(frame);
  signalAndBackground.plotOn(frame);
  signalAndBackground.plotOn(frame, Components(background), LineStyle(kDashed));


  // Print it to .pdf file

  TCanvas *canvas = new TCanvas();
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
