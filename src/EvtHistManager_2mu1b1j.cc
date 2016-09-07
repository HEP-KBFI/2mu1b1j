#include "tthAnalysis/HiggsToTauTau/interface/EvtHistManager_2mu1b1j.h"

#include "tthAnalysis/HiggsToTauTau/interface/histogramAuxFunctions.h"

#include <TMath.h>

EvtHistManager_2mu1b1j::EvtHistManager_2mu1b1j(const edm::ParameterSet& cfg)
        : HistManagerBase(cfg)
{
}

void EvtHistManager_2mu1b1j::bookHistograms(TFileDirectory& dir)
{
        histogram_numMuons_ = book1D(dir, "numMuons", "numMuons", 5, -0.5, +4.5);
        histogram_numJets_ = book1D(dir, "numJets", "numJets", 20, -0.5, +19.5);
        histogram_numBJets_medium_ = book1D(dir, "numBJets_medium", "numBJets_medium", 10, -0.5, +9.5);
        histogram_massOfOppositeChargeMuons_ = book1D(dir, "massOfOppositeChargeMuons", "massOfOppositeChargeMuons", 200, 0., 200.);
        histogram_EventCounter_ = book1D(dir, "EventCounter", "EventCounter", 1, -0.5, +0.5);
}

void EvtHistManager_2mu1b1j::fillHistograms(int numMuons,
                                            int numJets,
                                            int numBJets_medium,
                                            double massOfOppositeChargeMuons,
                                            double evtWeight)
{
        double evtWeightErr = 0.;

        fillWithOverFlow(histogram_numMuons_, numMuons, evtWeight, evtWeightErr);
        fillWithOverFlow(histogram_numJets_, numJets, evtWeight, evtWeightErr);
        fillWithOverFlow(histogram_numBJets_medium_, numBJets_medium, evtWeight, evtWeightErr);
        fillWithOverFlow(histogram_massOfOppositeChargeMuons_, massOfOppositeChargeMuons, evtWeight, evtWeightErr);
        fillWithOverFlow(histogram_EventCounter_, 0., evtWeight, evtWeightErr);
}
