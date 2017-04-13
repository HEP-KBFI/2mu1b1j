#include \
  "analysis2mu1b1j/analysis2mu1b1j/interface/RecoMuonCollectionSelectorTight_2mu1b1j.h" // RecoMuonSelectorTight
#include "tthAnalysis/HiggsToTauTau/interface/analysisAuxFunctions.h"                   // kEra_2015, kEra_2016

#include <cmath>                                                                        // fabs

RecoMuonSelectorTight_2mu1b1j::RecoMuonSelectorTight_2mu1b1j(int  era,
                                                             int  index,
                                                             bool debug)
  : era_(era)
    , debug_(debug)
    , min_pt_(10.)             // 15 GeV for 2lss channel, 10 GeV for 3l channel (cf. Table 13 of AN-2015/321)
    , max_absEta_(2.4)
    , max_dxy_(0.2)            // distance from helix to vertex
    , max_dz_(0.5)             // distance from helix to vertex
    , max_relIso_(0.1)         // relative isolation compared with other particles
    , max_chargedHadRelIso03_(0.1)
    , max_sip3d_(8.)
    , apply_looseIdPOG_(false) // physics group standard agreement
    , apply_mediumIdPOG_(true)
{}

bool RecoMuonSelectorTight_2mu1b1j::operator()(const RecoMuon& muon) const
{
  if (debug_) {
    std::cout << "<RecoMuonSelectorTight_2mu1b1j::operator()>:" << std::endl;
    std::cout << " muon: pT = " << muon.pt() << ", eta = " << muon.eta() <<
    ", phi = " << muon.phi() << ", charge = " << muon.charge() << std::endl;
  }

  if (muon.pt() < min_pt_) {
    if (debug_) std::cout << "FAILS pT cut." << std::endl;
    return false;
  }

  if (muon.absEta() > max_absEta_) {
    if (debug_) std::cout << "FAILS eta cut." << std::endl;
    return false;
  }

  if (std::fabs(muon.dxy()) > max_dxy_) {
    if (debug_) std::cout << "FAILS dxy cut." << std::endl;
    return false;
  }

  if (std::fabs(muon.dz()) > max_dz_) {
    if (debug_) std::cout << "FAILS dz cut." << std::endl;
    return false;
  }

  if (muon.relIso() > max_relIso_) {
    if (debug_) std::cout << "FAILS relIso cut." << std::endl;
    return false;
  }

  if (muon.sip3d() > max_sip3d_) {
    if (debug_) std::cout << "FAILS sip3d cut." << std::endl;
    return false;
  }

  if (apply_looseIdPOG_ && !muon.passesLooseIdPOG()) {
    if (debug_) std::cout << "FAILS looseIdPOG cut." << std::endl;
    return false;
  }

  if (apply_mediumIdPOG_ && !muon.passesMediumIdPOG()) {
    if (debug_) std::cout << "FAILS mediumIdPOG cut." << std::endl;
    return false;
  }

  if (muon.chargedHadRelIso03() > max_chargedHadRelIso03_) {
    if (debug_) std::cout << "FAILS max_chargedHadRelIso03_ cut." << std::endl;
    return false;
  }

  // muon passes all cuts
  return true;
}
