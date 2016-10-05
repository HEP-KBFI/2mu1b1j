#include "tthAnalysis/HiggsToTauTau/interface/EvtHistManager_2mu1b1jCategory.h"

#include "tthAnalysis/HiggsToTauTau/interface/histogramAuxFunctions.h"

#include <TMath.h>

EvtHistManager_2mu1b1jCategory::EvtHistManager_2mu1b1jCategory(const edm::ParameterSet& cfg)
        : HistManagerBase(cfg)
{
}

void EvtHistManager_2mu1b1jCategory::bookHistograms(TFileDirectory& dir)
{
        histogram_massOfOppositeChargeMuons_ = book1D(dir, "massOfOppositeChargeMuons", "massOfOppositeChargeMuons", 200, 0., 200.);
        histogram_EventCounter_ = book1D(dir, "EventCounter", "EventCounter", 1, -0.5, +0.5);
}

void EvtHistManager_2mu1b1jCategory::fillHistograms(
                                            double massOfOppositeChargeMuons,
                                            double evtWeight)
{
        double evtWeightErr = 0.;
        
        fillWithOverFlow(histogram_massOfOppositeChargeMuons_, massOfOppositeChargeMuons, evtWeight, evtWeightErr);
        fillWithOverFlow(histogram_EventCounter_, 0., evtWeight, evtWeightErr);
}
