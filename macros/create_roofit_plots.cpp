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


void create_roofit_plots()
{
  char rootFile[] =
    "/home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/histograms/histograms_harvested_stage1_2mu1b1j.root";
  TFile *f = new TFile(rootFile);

  char histDir[]  = "2mu1b1jCategoryA_Tight/sel/evt/";
  char histName[] = "massOfOppositeChargeMuons1PinPerGev";

  f->ls();

  std::cout << "CD to " << histDir << "\n";
  bool cdSuccessful = f->Cd(histDir);

  if (!cdSuccessful) {
    std::cout << "CD FAILED \n";
  }

  std::cout << "Draw histogram " << histName << "\n";
  TH1F *h1 = (TH1F *)f->Get(histName);
  h1->Draw();


  // S e t u p   c o m p o n e n t   p d f s
  // ---------------------------------------
  // Construct observable
  RooRealVar t("t", "t", -10, 30);

  // Construct landau(t,ml,sl) ;
  RooRealVar ml("ml", "mean landau", 5., -20, 20);
  RooRealVar sl("sl", "sigma landau", 1, 0.1, 10);
  RooLandau  landau("lx", "lx", t, ml, sl);

  // Construct gauss(t,mg,sg)
  RooRealVar  mg("mg", "mg", 0);
  RooRealVar  sg("sg", "sg", 2, 0.1, 10);
  RooGaussian gauss("gauss", "gauss", t, mg, sg);

  // C o n s t r u c t   c o n v o l u t i o n   p d f
  // ---------------------------------------
  // Set #bins to be used for FFT sampling to 10000
  t.setBins(10000, "cache");

  // Construct landau (x) gauss
  RooFFTConvPdf lxg("lxg", "landau (X) gauss", t, landau, gauss);

  // S a m p l e ,   f i t   a n d   p l o t   c o n v o l u t e d   p d f
  // ----------------------------------------------------------------------
  // Sample 1000 events in x from gxlx
  RooDataSet *data = lxg.generate(t, 10000);

  // Fit gxlx to data
  lxg.fitTo(*data);

  // Plot data, landau pdf, landau (X) gauss pdf
  RooPlot *frame = t.frame(Title("landau (x) gauss convolution"));
  data->plotOn(frame);
  lxg.plotOn(frame);
  landau.plotOn(frame, LineStyle(kDashed));

  // Draw frame on canvas
  new TCanvas("rf208_convolution", "rf208_convolution", 600, 600);
  gPad->SetLeftMargin(0.15);
  frame->GetYaxis()->SetTitleOffset(1.4);
  frame->Draw();
}
