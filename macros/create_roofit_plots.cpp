
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
#include "TGraph.h"
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


// Creates pValue plot

bool createPValuePlotAndSaveAsPdf(
  string year,
  string categoryName,
  string backgroundType,
  TH1F  *dataHistogram
  );


// Saves pValues and GEVs to pdf

bool savePValuePlotAsPdf(
  string year,
  string categoryName,
  string backgroundType,
  double GEVs[],
  double pValues[]
  );


// Creates RooPlot fit and saves as PDF file for specific range

bool createRooFitPlotForRangeAndSaveAsPdf(
  string year,
  string categoryName,
  string backgroundType,
  float  range[],
  TH1F  *dataHistogram
  );


// MyRooFitResult

class MyRooFitResult {
public:

  RooPlot *frame;
  double   signalEventsCount;
  double   fitError;

  MyRooFitResult(
    RooPlot *frame,
    double   signalEventsCount,
    double   fitError);

  double getPull();
  double getPValue();
};


// Creates RooPlot from TH1F fitting

MyRooFitResult* createRooFit(
  TH1F  *histogram,
  string year,
  string categoryName,
  float  peak,
  float  minPeakWidth,
  float  maxPeakWidth,
  float  xStart,
  float  xEnd,
  string backgroundType,
  string peakType
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


  // Generate plots

  for (string year : years) {
    for (string categoryName : categoryNames) {
      for (auto backgroundType : backgroundTypes) {
        cout << "Current category: " << categoryName << "\n";


        // Load histogram from analysis

        TFile *rootFile      = loadRootFile(year);
        TH1F  *dataHistogram = loadTH1F(rootFile, year, categoryName);


        // Generate pValue plot

        createPValuePlotAndSaveAsPdf(
          year,
          categoryName,
          backgroundType,
          dataHistogram
          );


        // Iterate over interesting areas

        for (auto range : ranges) {
          createRooFitPlotForRangeAndSaveAsPdf(
            year,
            categoryName,
            backgroundType,
            range,
            dataHistogram
            );
        }


        // clear reserved memory

        delete dataHistogram;
        delete rootFile;
      }
    }
  }


  return true;
}

//

bool createPValuePlotAndSaveAsPdf(
  string year,
  string categoryName,
  string backgroundType,
  TH1F  *dataHistogram
  )
{
  auto GEVs    = new double[120]();
  auto pValues = new double[120]();

  for (int i = 1; i < 121; i++) {
    double currentGEV = 1.0 * i;

    // Create fit
    MyRooFitResult *myRooFitResult = createRooFit(
      dataHistogram,
      year,
      categoryName,
      currentGEV,
      0.1,
      20.0,
      0,
      140,
      backgroundType,
      "peakIsConstant"
      );

    // Calculate p value
    double pValue = myRooFitResult->getPValue();

    cout << "pValuePlotInfo:"
         << "\tGev: " << currentGEV
         << "\tpValue: " << pValue
         << "\tpull: " << myRooFitResult->getPull()
         << "\tsignalEventsCount: " << myRooFitResult->signalEventsCount
         << "\tfitError: " << myRooFitResult->fitError
         << "\n";

    // Store pValue and GEV info
    GEVs[i]    = currentGEV;
    pValues[i] = pValue;
  }

  savePValuePlotAsPdf(
    year,
    categoryName,
    backgroundType,
    GEVs,
    pValues
    );

  return true;
}

//

bool savePValuePlotAsPdf(
  string year,
  string categoryName,
  string backgroundType,
  double GEVs[],
  double pValues[]
  )
{
  TGraph  *graph  = new TGraph(120, GEVs, pValues);
  TCanvas *canvas = new TCanvas();

  canvas->SetLogy();
  string pdfPath = string("/home/margusp/roofits/") +
                   year +
                   "_" +
                   categoryName +
                   "_background-" +
                   backgroundType +
                   ".pdf";

  cout << "pValue pdfPath is: " << pdfPath << "\n";

  graph->Draw("AC*");
  canvas->Print(pdfPath.data(), "pdf");
  return true;
}

//

bool createRooFitPlotForRangeAndSaveAsPdf(
  string year,
  string categoryName,
  string backgroundType,
  float  range[],
  TH1F  *dataHistogram
  )
{
  // Create rebinned histogram

  TH1F *rebinnedHistogram = rebinHistogram(
    dataHistogram,
    0.1,
    range[5]
    );

  // Generate roofit plot

  MyRooFitResult *myRooFitResult = createRooFit(
    rebinnedHistogram,
    year,
    categoryName,
    range[0],
    range[1],
    range[2],
    range[3],
    range[4],
    backgroundType,
    "peakIsVariable"
    );


  // Save rooplot to pdf file

  saveRooPlot(
    myRooFitResult->frame,
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
  delete myRooFitResult;

  cout << "createRooFitPlotForRangeAndSaveAsPdf() end\n";

  return true;
}

//

MyRooFitResult::MyRooFitResult(
  RooPlot *frame,
  double   signalEventsCount,
  double   fitError
  )
{
  this->frame             = frame;
  this->signalEventsCount = signalEventsCount;
  this->fitError          = fitError;
}

double MyRooFitResult::getPull() {
  double pull = this->signalEventsCount / this->fitError;

  return pull;
}

double MyRooFitResult::getPValue() {
  double pull   = this->getPull();
  double pValue = 2. * (1. - TMath::Erf(pull));

  return pValue;
}

MyRooFitResult* createRooFit(
  TH1F  *histogram,
  string year,
  string categoryName,
  float  peak,
  float  minPeakWidth,
  float  maxPeakWidth,
  float  xStart,
  float  xEnd,
  string backgroundType,
  string peakType
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

  if (peakType.compare("peakIsConstant") == 0) {
    breitWignerMean.setConstant(true);
  }

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

  MyRooFitResult *myRooFitResult = new MyRooFitResult(
    frame,
    signalEventsCount.getVal(),
    signalEventsCount.getError()
    );

  // return myRooFitResult

  return myRooFitResult;
}

//

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

//

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

//

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
  string pdfPath = string("/home/margusp/roofits/") +
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
