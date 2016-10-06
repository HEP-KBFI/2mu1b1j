#include "tthAnalysis/HiggsToTauTau/interface/RecoJetCollectionSelector.h" // RecoJetSelector

#include <cmath> // fabs

RecoJetSelector::RecoJetSelector(int index, bool debug)
  : min_pt_(25.)
  , max_absEta_(999.9) // 2.4 TODO change this back (Margus)
{}

bool RecoJetSelector::operator()(const RecoJet& jet) const
{
  if ( jet.pt_ >= min_pt_ &&
       jet.absEta_ <= max_absEta_ ) {
    return true;
  }
  return false;
}
