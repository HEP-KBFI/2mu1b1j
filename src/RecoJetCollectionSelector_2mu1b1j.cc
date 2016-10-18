#include "analysis2mu1b1j/analysis2mu1b1j/interface/RecoJetCollectionSelector_2mu1b1j.h" // RecoJetSelector

#include <cmath> // fabs

RecoJetSelector_2mu1b1j::RecoJetSelector_2mu1b1j(int era, int index, bool debug)
  : min_pt_(25.)
  , max_absEta_(4.7) // 2.4 TODO change this back (Margus)
{}

bool RecoJetSelector_2mu1b1j::operator()(const RecoJet& jet) const
{
  if ( jet.pt_ >= min_pt_ &&
       jet.absEta_ <= max_absEta_ ) {
    return true;
  }
  return false;
}
