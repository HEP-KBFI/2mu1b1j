#include "analysis2mu1b1j/analysis2mu1b1j/interface/RecoJetCollectionSelectorBtag_2mu1b1j.h" // RecoJetSelector

#include <cmath> // fabs

RecoJetSelectorBtag_2mu1b1j::RecoJetSelectorBtag_2mu1b1j(int era, int index, bool debug)
  : era_(era)
  , min_pt_(25.)
  , max_absEta_(999.9) // 2.4 TODO change this back (Margus)
  , min_BtagCSV_(-1.e+3)
{}

bool RecoJetSelectorBtag_2mu1b1j::operator()(const RecoJet& jet) const
{
  if ( jet.pt_ >= min_pt_ &&
       jet.absEta_ <= max_absEta_ &&
       jet.BtagCSV_ >= min_BtagCSV_ ) {
    return true;
  }
  return false;
}
