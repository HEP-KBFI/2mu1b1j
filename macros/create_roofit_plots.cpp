
// Intro
// =====
//
// This program creates Roofit plots from TH1F histograms for 2mu1b1j analysis.
//
// If you have any questions, send an email to margus@wave.ee
//
//


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


// Interface
// =========

// Load root file

TFile* loadRootFile(
  string year
  );


// Loads TH1F from pregenerated analysis .root file

TH1F* loadTH1F(
  TFile *rootFile,
  string year,
  string categoryName
  );


// Creates RooPlot from TH1F fitting

RooPlot* createRooFit(
  TH1F  *histogram,
  string year,
  string categoryName,
  float  peak,
  float  minPeakWidth,
  float  maxPeakWidth,
  float  xStart,
  float  xEnd,
  string backgroundType
  );


// Saves RooPlot into .pdf file

bool saveRooPlot(
  RooPlot *frame,
  string   year,
  string   categoryName,
  float    peak,
  float    minPeakWidth,
  float    maxPeakWidth,
  float    xStart,
  float    xEnd,
  float    binning,
  string   backgroundType
  );


// Original TH1F files are 10 datapoints per GeV, but you might want to have it different some times
// so rebinned clone is created

TH1F* rebinHistogram(
  TH1F  *histogram,
  double originalPinning,
  double newPinning
  );


// Entry point function. Iterates over interesting years, categories and ranges and creates .pdf files for visual inspection

bool create_roofit_plots()
{
  string years[] = {
    "2015",
    "2016"
  };

  string categoryNames[] = {
    "CategoryA",
    "CategoryARelaxed",
    "CategoryACompare",
    "CategoryACompareRelaxed",
    "CategoryB",
    "CategoryBRelaxed",
    "CategoryBCompare",
    "CategoryBCompareRelaxed",
    "CategoryC",
    "CategoryCRelaxed",
    "CategoryCCompare",
    "CategoryCCompareRelaxed"
  };

  string backgroundTypes[] = {
    "polynomial",
    "exponential"
  };


  // peak, minPeakWidth, maxPeakWidth, xStart, xEnd, binning

  float ranges[][6] = {
    {     3.1,   0.1,  1.0,    2.0,    4.0,   0.1 },
    {     3.1,   0.1,  1.0,    2.0,    4.0,   0.2 },
    {    9.46,   0.1,  2.0,    8.0,  12.00,   0.2 },
    {    10.0,   0.1,  2.0,    8.0,  12.00,   0.2 },
    {   10.35,   0.1,  2.0,    8.0,  12.00,   0.2 },
    {    28.5,   1.0,  2.0,   20.0,   40.0,   0.5 },
    {    28.5,   1.0,  2.0,   20.0,   40.0,     1 },
    {    28.5,   1.0,  2.0,   20.0,   40.0,     2 },
    {    91.0,   1.0,  2.0,   80.0,  100.0,     1 },
    {    91.0,   1.0,  2.0,    0.0,  120.0,     1 }
  };


  for (string year : years) {
    for (string categoryName : categoryNames) {
      for (auto backgroundType : backgroundTypes) {
        cout << "Current category: " << categoryName << "\n";

        // Load histogram from analysis
        TFile *rootFile  = loadRootFile(year);
        TH1F  *histogram = loadTH1F(rootFile, year, categoryName);


        // Iterate over interesting areas

        for (auto range : ranges) {
          // Create rebinned histogram

          TH1F *rebinnedHistogram = rebinHistogram(
            histogram,
            0.1,
            range[5]
            );

          // Generate roofit plot

          RooPlot *frame = createRooFit(
            rebinnedHistogram,
            year,
            categoryName,
            range[0],
            range[1],
            range[2],
            range[3],
            range[4],
            backgroundType
            );


          // Save rooplot to pdf file

          saveRooPlot(
            frame,
            year,
            categoryName,
            range[0],
            range[1],
            range[2],
            range[3],
            range[4],
            range[5],
            backgroundType
            );

          // clear reserved memory

          delete rebinnedHistogram;
          delete frame;
        }

        // clear reserved memory

        delete histogram;
        delete rootFile;
      }
    }
  }


  return true;
}

RooPlot* createRooFit(
  TH1F  *histogram,
  string year,
  string categoryName,
  float  peak,
  float  minPeakWidth,
  float  maxPeakWidth,
  float  xStart,
  float  xEnd,
  string backgroundType
  )
{
  if (histogram) {
    cout << "Histogram loaded. \n";
  } else {
    cout << "Histogram not loaded. \n";
    return NULL;
  }


  // Create roofit variable

  RooRealVar x("x", "x", xStart, xEnd);
  x.setBins(10000, "fft");


  // Import histogram from x range to dataHist

  RooDataHist dataHist("dataHist", "dataHist", x, histogram);


  // Theory model (Breit-Wigner)

  RooRealVar breitWignerMean(
    "breitWignerMean",
    "breitWignerMean",
    peak,
    xStart + minPeakWidth,
    xEnd - minPeakWidth
    );

  RooRealVar breitWignerWidth(
    "breitWignerWidth",
    "breitWignerWidth",
    (minPeakWidth + maxPeakWidth) / 2,
    minPeakWidth,
    maxPeakWidth
    );

  RooBreitWigner breitWigner(
    "bw",
    "bw",
    x,
    breitWignerMean,
    breitWignerWidth
    );


  // Resolution model (Gauss)

  RooRealVar gaussMean(
    "gaussMean",
    "gaussMean",
    0
    );
  gaussMean.setConstant(true);

  RooRealVar gaussWidth(
    "gaussWidth",
    "gaussWidth",
    (minPeakWidth + maxPeakWidth) / 2,
    minPeakWidth,
    maxPeakWidth
    );

  RooGaussian gauss(
    "gauss",
    "gauss",
    x,
    gaussMean,
    gaussWidth
    );


  // Set model for signal

  RooFFTConvPdf signal(
    "signal",
    "signal",
    x,
    breitWigner,
    gauss
    );


  // Set model for background

  RooRealVar backgroundA(
    "backgroundA",
    "backgroundA",
    0.1
    );

  RooRealVar backgroundB(
    "backgroundB",
    "backgroundB",
    0.2
    );

  RooRealVar backgroundC(
    "backgroundC",
    "backgroundC",
    0.3
    );

  RooArgList backgroundDependents(
    backgroundA,
    backgroundB,
    backgroundC,
    x
    );

  RooGenericPdf *background = NULL;

  if (backgroundType.compare("polynomial") == 0) {
    cout << "Background is polynomial\n";
    background = new RooGenericPdf(
      "background",
      "(a * x * x) + (b * x) + c",
      "(backgroundA * x * x) + (backgroundB * x) + backgroundC",
      backgroundDependents
      );
  }

  if (backgroundType.compare("exponential") == 0) {
    cout << "Background is exponential\n";
    background = new RooGenericPdf(
      "background",
      "a * exp(-b * x ^ c)",
      "backgroundA * exp(-backgroundB * x ^ backgroundC)",
      backgroundDependents
      );
  }


  // Signal + background

  float entriesCount = dataHist.sumEntries();
  RooRealVar signalEventsCount("signalEventsCount", "#signal events", entriesCount, 0.0, entriesCount);
  RooRealVar backgroundEventsCount("backgroundEventsCount", "#background events", 1, 0.0, entriesCount);

  RooAddPdf signalAndBackground(
    "signalAndBackground",
    "signalAndBackground",
    RooArgList(
      signal,
      *background
      ),
    RooArgList(
      signalEventsCount,
      backgroundEventsCount
      )
    );


  // Make fitting

  // RooDataSet *data = signalAndBackground.generate(x, 2000);
  signalAndBackground.fitTo(dataHist, Extended());


  // Draw fitted result onto RooPlot

  RooPlot *frame = x.frame();
  dataHist.plotOn(frame);
  signalAndBackground.plotOn(frame);
  signalAndBackground.plotOn(frame, Components(*background), LineStyle(kDashed));


  // clean reserved memory

  delete background;


  // return frame

  return frame;
}

TFile* loadRootFile(
  string year
  )
{
  string rootFilePath = string("/home/margusp/analysis2mu1b1j/") +
                        year +
                        "/2016Oct28_v1/histograms/histograms_harvested_stage1_2mu1b1j.root";

  TFile *rootFile = new TFile(rootFilePath.data());

  if (rootFile) {
    cout << "Loaded root file: " << rootFilePath << "\n";
  } else {
    cout << "Unable to load root file: " << rootFilePath << "\n";
  }

  return rootFile;
}

TH1F* loadTH1F(
  TFile *rootFile,
  string year,
  string categoryName
  )
{
  // set configuration params

  string histDir = string("2mu1b1j") + categoryName + string("_Tight/sel/evt/data_obs");

  // string histName = "massOfOppositeChargeMuons1PinPerGeV";
  string histName = "massOfOppositeChargeMuons10PinsPerGev";


  // show TH1F histogram

  std::cout << "Contents of current directory: \n";
  rootFile->ls();

  bool cdSuccessful = rootFile->Cd(histDir.data());

  if (cdSuccessful) {
    std::cout << "Success: CD to " << histDir << "\n";
  } else {
    std::cout << "Failed: CD to " << histDir << "\n";
    return NULL;
  }

  std::cout << "Contents of current directory: \n";
  rootFile->ls();

  TH1F *histogram = (TH1F *)gDirectory->Get(histName.data());

  if (histogram) {
    std::cout << "Success: Histogram loaded. " << histName << "\n";
    return (TH1F *)histogram;
  } else {
    std::cout << "Failed: Histogram not found. " << histName << "\n";
    return NULL;
  }
}

bool saveRooPlot(
  RooPlot *frame,
  string   year,
  string   categoryName,
  float    peak,
  float    minPeakWidth,
  float    maxPeakWidth,
  float    xStart,
  float    xEnd,
  float    binning,
  string   backgroundType
  )
{
  // Print it to .pdf file

  TCanvas *canvas = new TCanvas();

  frame->Draw();
  string pdfPath =
    string("/home/margusp/roofits/") +
    year +
    "_" +
    to_string(peak) +
    "_" +
    to_string(xStart) +
    "-" +
    to_string(xEnd) +
    "_" +
    categoryName +
    "_binning-" +
    to_string(binning) +
    "_background-" +
    backgroundType +
    ".pdf";
  cout << "pdfPath is: " << pdfPath << "\n";
  canvas->Print(pdfPath.data(), "pdf");

  return true;
}

// Returns clone of histogram with new binning

TH1F* rebinHistogram(
  TH1F  *histogram,
  double originalPinning,
  double newPinning
  )
{
  double binningMultiplier = (1.0 / originalPinning) * (newPinning);

  cout << "originalPinning: " << originalPinning
       << ", newPinning: " << newPinning
       << ", binningMultiplier: " << binningMultiplier << "\n";

  return (TH1F *)histogram->Rebin(binningMultiplier, "suva");
}
