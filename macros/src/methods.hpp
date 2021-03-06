#ifndef METHODS_H
#define METHODS_H


#include "string.h"

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooNumConvPdf.h"
#include "RooBreitWigner.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "RooDataHist.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TH1.h"
#include "string.h"


#include "MyRooFitSetting.hpp"
#include "MyRooFitResult.hpp"

using namespace std;


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
  string          year,
  string          categoryName,
  string          backgroundType,
  MyRooFitSetting myRooFitSetting,
  TH1F           *dataHistogram
  );


// Saves pValues and GEVs to pdf

bool savePValuePlotAsPdf(
  string year,
  string categoryName,
  double minPeakWidth,
  double maxPeakWidth,
  double xStart,
  double xEnd,
  double binWidth,
  string backgroundType,
  double GEVs[],
  double pValues[]
  );


// Creates RooPlot fit and saves as PDF file for specific range

bool createRooFitPlotForRangeAndSaveAsPdf(
  string          year,
  string          categoryName,
  string          backgroundType,
  MyRooFitSetting myRooFitSetting,
  TH1F           *dataHistogram
  );


// Creates RooPlot from TH1F fitting

MyRooFitResult* createRooFit(
  TH1F  *histogram,
  string year,
  string categoryName,
  double peak,
  double minPeakWidth,
  double maxPeakWidth,
  double xStart,
  double xEnd,
  string backgroundType,
  string peakType
  );


// Saves RooPlot into .pdf file

bool saveRooPlot(
  RooPlot *frame,
  string   year,
  string   categoryName,
  double   peak,
  double   minPeakWidth,
  double   maxPeakWidth,
  double   xStart,
  double   xEnd,
  double   binning,
  string   backgroundType
  );


// Original TH1F files are 10 datapoints per GeV, but you might want to have it different some times
// so rebinned clone is created

TH1F* rebinHistogram(
  TH1F  *histogram,
  double originalBinning,
  double newBinning
  );

#endif // ifndef METHODS_H
