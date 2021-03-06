#ifndef analysis2mu1b1j_analysis2mu1b1j_EvtHistManager_2mu1b1j_h
#define analysis2mu1b1j_analysis2mu1b1j_EvtHistManager_2mu1b1j_h

/** \class EvtHistManager_2mu1b1j
 *
 * Book and fill histograms for event-level quantities
 * in 2mu1b1j category
 *
 * \author mxrguspxrt, Tallinn
 *
 */

#include "tthAnalysis/HiggsToTauTau/interface/HistManagerBase.h" // HistManagerBase

class EvtHistManager_2mu1b1j : public HistManagerBase {
public:

  EvtHistManager_2mu1b1j(edm::ParameterSet const& cfg);
  ~EvtHistManager_2mu1b1j() {}

  /// book and fill histograms
  void bookHistograms(TFileDirectory& dir);
  void fillHistograms(int    numMuons,
                      int    numJets,
                      int    numBJets_medium,
                      double massOfOppositeChargeMuons,
                      double evtWeight);

private:

  TH1 *histogram_numMuons_;
  TH1 *histogram_numJets_;
  TH1 *histogram_numBJets_medium_;

  TH1 *histogram_massOfOppositeChargeMuons_;

  TH1 *histogram_EventCounter_;

  std::vector<TH1 *> histograms_;
};

#endif // ifndef analysis2mu1b1j_analysis2mu1b1j_EvtHistManager_2mu1b1j_h
