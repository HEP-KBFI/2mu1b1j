#ifndef tthAnalysis_HiggsToTauTau_EvtHistManager_2mu1b1j_h
#define tthAnalysis_HiggsToTauTau_EvtHistManager_2mu1b1j_h

/** \class EvtHistManager_2mu1b1j
 *
 * Book and fill histograms for event-level quantities in ttH, H->tautau analysis
 * in 2mu1b1j category
 *
 * \author Christian Veelken, Tallin
 *
 */

#include "tthAnalysis/HiggsToTauTau/interface/HistManagerBase.h" // HistManagerBase

class EvtHistManager_2mu1b1j
  : public HistManagerBase
{
 public:
  EvtHistManager_2mu1b1j(edm::ParameterSet const& cfg);
  ~EvtHistManager_2mu1b1j() {}

  /// book and fill histograms
  void bookHistograms(TFileDirectory& dir);
  void fillHistograms(int numElectrons, int numMuons, int numHadTaus, int numJets, int numBJets_loose, int numBJets_medium,
		      double mvaOutput_3l_ttV, double mvaOutput_3l_ttbar, double mvaDiscr_3l,
		      double mTauTauVis1, double mTauTauVis2, double evtWeight);

 private:
  TH1* histogram_numElectrons_;
  TH1* histogram_numMuons_;
  TH1* histogram_numHadTaus_;
  TH1* histogram_numJets_;
  TH1* histogram_numBJets_loose_;
  TH1* histogram_numBJets_medium_;

  TH1* histogram_mvaOutput_3l_ttV_;
  TH1* histogram_mvaOutput_3l_ttbar_;
  TH1* histogram_mvaDiscr_3l_;

  TH1* histogram_mTauTauVis_;

  TH1* histogram_EventCounter_;

  std::vector<TH1*> histograms_;
};

#endif
