#ifndef tthAnalysis_HiggsToTauTau_EvtHistManager_2mu1b1jCategory_h
#define tthAnalysis_HiggsToTauTau_EvtHistManager_2mu1b1jCategory_h

/** \class EvtHistManager_2mu1b1jCategory
 *
 * Book and fill histograms for event-level quantities
 * in 2mu1b1jCategory category
 *
 * \author mxrguspxrt, Tallinn
 *
 */

#include "tthAnalysis/HiggsToTauTau/interface/HistManagerBase.h" // HistManagerBase

class EvtHistManager_2mu1b1jCategory : public HistManagerBase
{
public:
EvtHistManager_2mu1b1jCategory(edm::ParameterSet const& cfg);
~EvtHistManager_2mu1b1jCategory() {
}

/// book and fill histograms
void bookHistograms(TFileDirectory& dir);
void fillHistograms(double massOfOppositeChargeMuons,
                    double evtWeight);

private:
TH1* histogram_massOfOppositeChargeMuons_;
TH1* histogram_EventCounter_;
std::vector<TH1*> histograms_;
};

#endif
