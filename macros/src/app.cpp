
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

#include "string.h"

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
#include "TFile.h"

#include "methods.hpp"
#include "MyRooFitSetting.hpp"
#include "MyRooFitResult.hpp"


using namespace std;


// Entry point function. Iterates over interesting years, categories and ranges and creates .pdf files for visual inspection

int app()
{
  string years[] = {
    "2015",
    "2016"
  };

  string categoryNames[] = {
    "CategoryA",

    // "CategoryARelaxed",
    // "CategoryACompare",
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

  string backgroundTypes[] = {
    "polynomial",
    "exponential"
  };


  // peak, minPeakWidth, maxPeakWidth, xStart, xEnd, binWidth, fitStart, fitEnd, fitBinWidth

  MyRooFitSetting myRooFitSettings[] = {
    MyRooFitSetting(3.1,   0.1, 1.0, 2.0,  4.0,   0.1, 2.0,  4.0,   0.1),
    MyRooFitSetting(3.1,   0.1, 1.0, 2.0,  4.0,   0.2, 2.0,  4.0,   0.2),
    MyRooFitSetting(9.46,  0.1, 2.0, 8.0,  12.00, 0.2, 8.0,  12.00, 0.2),
    MyRooFitSetting(10.0,  0.1, 2.0, 8.0,  12.00, 0.2, 8.0,  12.00, 0.2),
    MyRooFitSetting(10.35, 0.1, 2.0, 8.0,  12.00, 0.2, 8.0,  12.00, 0.2),
    MyRooFitSetting(28.5,  0.5, 2.0, 20.0, 40.0,  0.5, 20.0, 40.0,  0.5),
    MyRooFitSetting(28.5,  0.5, 5.0, 12.0, 70.0,  0.5, 12.0, 70.0,  0.5),
    MyRooFitSetting(28.5,  0.5, 5.0, 12.0, 70.0,  1.0, 12.0, 70.0,  1.0),
    MyRooFitSetting(28.5,  0.5, 5.0, 12.0, 70.0,  2.0, 12.0, 70.0,  1.0),
    MyRooFitSetting(28.5,  1.0, 2.0, 20.0, 40.0,  1.0, 20.0, 40.0,  1.0),
    MyRooFitSetting(28.5,  1.0, 2.0, 20.0, 40.0,  2.0, 20.0, 40.0,  1.0),
    MyRooFitSetting(91.0,  1.0, 2.0, 80.0, 100.0, 1.0, 80.0, 100.0, 1.0),
    MyRooFitSetting(91.0,  1.0, 2.0, 0.0,  120.0, 1.0, 0.0,  120.0, 1.0)
  };


  // Generate plots

  for (string year : years) {
    for (string categoryName : categoryNames) {
      for (string backgroundType : backgroundTypes) {
        cout << "Current category: " << categoryName << "\n";


        // Load histogram from analysis

        TFile *rootFile      = loadRootFile(year);
        TH1F  *dataHistogram = loadTH1F(rootFile, year, categoryName);


        // Iterate over interesting areas

        for (MyRooFitSetting myRooFitSetting : myRooFitSettings) {
          // Generate pValue plot

          createPValuePlotAndSaveAsPdf(
            year,
            categoryName,
            backgroundType,
            myRooFitSetting,
            dataHistogram
            );

          // Generate causal RooFit plot

          createRooFitPlotForRangeAndSaveAsPdf(
            year,
            categoryName,
            backgroundType,
            myRooFitSetting,
            dataHistogram
            );
        }


        // clear reserved memory

        delete dataHistogram;
        delete rootFile;
      }
    }
  }


  return 0;
}
