#include "analysis2mu1b1j/analysis2mu1b1j/interface/EvtHistManager_2mu1b1jCategory.h"

#include "tthAnalysis/HiggsToTauTau/interface/histogramAuxFunctions.h"

#include <TMath.h>

EvtHistManager_2mu1b1jCategory::EvtHistManager_2mu1b1jCategory(
  const edm::ParameterSet& cfg)
  : HistManagerBase(cfg)
{}

void EvtHistManager_2mu1b1jCategory::bookHistograms(TFileDirectory& dir)
{
  histogram_massOfOppositeChargeMuons1PinPerGeV_ = book1D(dir,
                                                          "massOfOppositeChargeMuons1PinPerGeV",
                                                          "massOfOppositeChargeMuons1PinPerGeV",
                                                          200,
                                                          0.,
                                                          200.);
  histogram_massOfOppositeChargeMuons10PinsPerGev_ = book1D(dir,
                                                            "massOfOppositeChargeMuons10PinsPerGev",
                                                            "massOfOppositeChargeMuons10PinsPerGev",
                                                            2000,
                                                            0.,
                                                            200.);


  histogram_deltaROfOppositeChargeMuons_ = book1D(dir,
                                                  "deltaROfOppositeChargeMuons",
                                                  "deltaROfOppositeChargeMuons",
                                                  70,
                                                  0.,
                                                  7.);


  histogram_ptOfOppositeChargeMuons1PinPerGeV_ = book1D(dir,
                                                        "ptOfOppositeChargeMuons1PinPerGeV",
                                                        "ptOfOppositeChargeMuons1PinPerGeV",
                                                        200,
                                                        0.,
                                                        200.);
  histogram_ptOfOppositeChargeMuons10PinsPerGev_ = book1D(dir,
                                                          "ptOfOppositeChargeMuons10PinsPerGev",
                                                          "ptOfOppositeChargeMuons10PinsPerGev",
                                                          2000,
                                                          0.,
                                                          200.);


  histogram_EventCounter_ = book1D(dir,
                                   "EventCounter",
                                   "EventCounter",
                                   1,
                                   -0.5,
                                   +0.5);
}

void EvtHistManager_2mu1b1jCategory::fillHistograms(
  double massOfOppositeChargeMuons,
  double deltaROfOppositeChargeMuons,
  double ptOfOppositeChargeMuons,
  double evtWeight)
{
  double evtWeightErr = 0.;

  fillWithOverFlow(histogram_massOfOppositeChargeMuons1PinPerGeV_,
                   massOfOppositeChargeMuons,
                   evtWeight,
                   evtWeightErr);

  fillWithOverFlow(histogram_massOfOppositeChargeMuons10PinsPerGev_,
                   massOfOppositeChargeMuons,
                   evtWeight,
                   evtWeightErr);

  fillWithOverFlow(histogram_deltaROfOppositeChargeMuons_,
                   deltaROfOppositeChargeMuons,
                   evtWeight,
                   evtWeightErr);

  fillWithOverFlow(histogram_ptOfOppositeChargeMuons1PinPerGeV_,
                   ptOfOppositeChargeMuons,
                   evtWeight,
                   evtWeightErr);

  fillWithOverFlow(histogram_ptOfOppositeChargeMuons10PinsPerGev_,
                   ptOfOppositeChargeMuons,
                   evtWeight,
                   evtWeightErr);

  fillWithOverFlow(histogram_EventCounter_,
                   0.,
                   evtWeight,
                   evtWeightErr);
}
