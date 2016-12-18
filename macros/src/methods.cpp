#include "string.h"
#include "methods.hpp"
#include "RooDataHist.h"
#include "RooGenericPdf.h"
#include "RooAddPdf.h"
#include "RooCmdArg.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"


bool createPValuePlotAndSaveAsPdf(
  string          year,
  string          categoryName,
  string          backgroundType,
  MyRooFitSetting settings,
  TH1F           *dataHistogram
  )
{
  int fitBinsCount = (settings.fitEnd - settings.fitStart) / settings.fitBinWidth;


  TH1F *rebinnedHistogram = rebinHistogram(
    dataHistogram,
    0.1, // original binning
    settings.binWidth
    );

  auto GEVs    = new double[fitBinsCount]();
  auto pValues = new double[fitBinsCount]();

  for (int i = 0; i < fitBinsCount; i++) {
    double currentGEV = settings.fitStart + (i * settings.fitBinWidth);

    // Create fit

    MyRooFitResult *myRooFitResult = createRooFit(
      rebinnedHistogram,
      year,
      categoryName,
      currentGEV,            // peak
      settings.minPeakWidth, // minPeakWidth
      settings.maxPeakWidth, // maxPeakWidth
      settings.xStart,       // xStart
      settings.xEnd,         // xEnd
      backgroundType,        // backgroundType
      "peakIsConstant"       // peakType
      );


    // Store information about the fit in plot

    saveRooPlot(
      myRooFitResult->frame, // frame
      year + "_pValue_",     // year
      categoryName,          // categoryName
      currentGEV,            // peak
      settings.minPeakWidth, // minPeakWidth
      settings.maxPeakWidth, // maxPeakWidth
      settings.xStart,       // xStart
      settings.xEnd,         // xEnd
      settings.fitBinWidth,  // binning
      backgroundType         // backgroundType
      );


    // Calculate p value

    double pValue = myRooFitResult->getPValue();

    cout << "pValuePlotInfo:"
         << "\tGev: " << currentGEV
         << myRooFitResult->getInfo()
         << "\n";


    // Store pValue and GEV info

    GEVs[i]    = currentGEV;
    pValues[i] = pValue;

    delete myRooFitResult;
  }

  savePValuePlotAsPdf(
    year,                  // year
    categoryName,          // categoryName
    settings.minPeakWidth, // minPeakWidth
    settings.maxPeakWidth, // maxPeakWidth
    settings.xStart,       // xStart
    settings.xEnd,         // xEnd
    settings.binWidth,     // binWidth
    backgroundType,        // backgroundType
    GEVs,                  // GEVs
    pValues                // pValues
    );

  delete[] GEVs;
  delete[] pValues;

  return true;
}

//

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
  )
{
  TGraph  *graph  = new TGraph(xEnd - xStart, GEVs, pValues);
  TCanvas *canvas = new TCanvas();

  canvas->SetLogy();
  string pdfPath = string("/home/margusp/roofits/") +
                   year +
                   "_" +
                   categoryName +
                   "_peakWidth-" +
                   to_string(minPeakWidth) +
                   "-" +
                   to_string(maxPeakWidth) +
                   "_range-" +
                   to_string(xStart) +
                   "-" +
                   to_string(xEnd) +
                   "_binning-" +
                   to_string(binWidth) +
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
  string          year,
  string          categoryName,
  string          backgroundType,
  MyRooFitSetting settings,
  TH1F           *dataHistogram
  )
{
  // Create rebinned histogram

  TH1F *rebinnedHistogram = rebinHistogram(
    dataHistogram,
    0.1,
    settings.binWidth
    );

  // Generate roofit plot

  MyRooFitResult *myRooFitResult = createRooFit(
    rebinnedHistogram,
    year,
    categoryName,
    settings.peak,         // peak
    settings.minPeakWidth, // minPeakWidth
    settings.maxPeakWidth, // maxPeakWidth
    settings.xStart,       // xStart
    settings.xEnd,         // xEnd
    backgroundType,        // backgroundType
    "peakIsVariable"       // peakType
    );


  // Save rooplot to pdf file

  saveRooPlot(
    myRooFitResult->frame, // frame
    year,                  // year
    categoryName,          // categoryName
    settings.peak,
    settings.minPeakWidth,
    settings.maxPeakWidth,
    settings.xStart,
    settings.xEnd,
    settings.binWidth,
    backgroundType // backgroundType
    );

  // clear reserved memory

  delete rebinnedHistogram;
  delete myRooFitResult;

  cout << "createRooFitPlotForRangeAndSaveAsPdf() end\n";

  return true;
}

//


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
    -100.0,
    -10000.0,
    10000.0
    );

  RooRealVar backgroundB(
    "backgroundB",
    "backgroundB",
    100.0,
    -10000.0,
    10000.0
    );

  RooRealVar backgroundC(
    "backgroundC",
    "backgroundC",
    500.0,
    -10000.0,
    10000.0
    );

  RooArgList backgroundDependents(
    backgroundA,
    backgroundB,
    backgroundC,
    x
    );

  RooGenericPdf *background = NULL;

  if (backgroundType.compare("polynomial") == 0) {
    // This is old version:
    //
    // background = new RooGenericPdf(
    // "background",
    // "(a * x * x) + (b * x) + c",
    // "(backgroundA * x * x) + (backgroundB * x) + backgroundC",
    // backgroundDependents
    // );

    // This is new version (more effective):

    double meanOfTheDistribution = (xStart + xEnd) / 2;

    string xMinusMeanOfTheDistribution = string("(x - ")
                                         + to_string(meanOfTheDistribution)
                                         + ")";

    string formula = string("backgroundA + backgroundB * ")
                     + xMinusMeanOfTheDistribution
                     + " + (0.5 * backgroundC * (3 * ("
                     + xMinusMeanOfTheDistribution
                     + " * "
                     + xMinusMeanOfTheDistribution
                     + ") - 1))";

    cout << "Background is polynomial: " << formula << "\n";

    background = new RooGenericPdf(
      "background",
      "a + b * (x - meanOfTheDistribution) + (0.5 * c * (3 * ((x - meanOfTheDistribution) ^ 2) - 1))",
      formula.data(),
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

  double entriesCount = dataHist.sumEntries();
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
  signalAndBackground.fitTo(dataHist, RooFit::Extended());


  // Draw fitted result onto RooPlot

  RooPlot *frame = x.frame();
  dataHist.plotOn(frame);
  signalAndBackground.plotOn(frame);
  signalAndBackground.plotOn(frame, RooFit::Components(*background), RooFit::LineStyle(kDashed));


  // clean reserved memory

  delete background;


  // return myRooFitResult

  MyRooFitResult *myRooFitResult = new MyRooFitResult();

  myRooFitResult->frame                 = frame;
  myRooFitResult->x                     = x.getVal();
  myRooFitResult->signalEventsCount     = signalEventsCount.getVal();
  myRooFitResult->fitError              = signalEventsCount.getError();
  myRooFitResult->backgroundEventsCount = backgroundEventsCount.getVal();
  myRooFitResult->breitWignerMean       = breitWignerMean.getVal();
  myRooFitResult->breitWignerWidth      = breitWignerWidth.getVal();
  myRooFitResult->gaussMean             = gaussMean.getVal();
  myRooFitResult->gaussWidth            = gaussWidth.getVal();
  myRooFitResult->backgroundA           = backgroundA.getVal();
  myRooFitResult->backgroundB           = backgroundB.getVal();
  myRooFitResult->backgroundC           = backgroundC.getVal();

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
  double   peak,
  double   minPeakWidth,
  double   maxPeakWidth,
  double   xStart,
  double   xEnd,
  double   binning,
  string   backgroundType
  )
{
  // Print it to .pdf file

  TCanvas *canvas = new TCanvas();

  frame->Draw();
  string pdfPath = string("/home/margusp/roofits/") +
                   year +
                   "_peak-" +
                   to_string(peak) +
                   "_range-" +
                   to_string(xStart) +
                   "-" +
                   to_string(xEnd) +
                   "_category-" +
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
  double originalBinning,
  double newBinning
  )
{
  double binningMultiplier = (1.0 / originalBinning) * (newBinning);

  cout << "originalBinning: " << originalBinning
       << ", newBinning: " << newBinning
       << ", binningMultiplier: " << binningMultiplier << "\n";

  return (TH1F *)histogram->Rebin(binningMultiplier, "suva");
}
