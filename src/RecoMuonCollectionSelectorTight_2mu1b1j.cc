#include "analysis2mu1b1j/analysis2mu1b1j/interface/RecoMuonCollectionSelectorTight_2mu1b1j.h" // RecoMuonSelectorTight
#include "tthAnalysis/HiggsToTauTau/interface/analysisAuxFunctions.h" // kEra_2015, kEra_2016

#include <cmath> // fabs

RecoMuonSelectorTight_2mu1b1j::RecoMuonSelectorTight_2mu1b1j(int era, int index, bool debug)
        : era_(era)
        , debug_(debug)
        , min_pt_(10.) // 15 GeV for 2lss channel, 10 GeV for 3l channel (cf. Table 13 of AN-2015/321)
        , max_absEta_(2.4)
        , max_dxy_(0.2) // distance from helix to vertex
        , max_dz_(0.5) // distance from helix to vertex
        , max_relIso_(0.1) // relative isolation compared with other particles
        , apply_looseIdPOG_(true)
        , apply_mediumIdPOG_(true)
{
        if      ( era_ == kEra_2015 ) max_jetBtagCSV_ = 0.89;
        else if ( era_ == kEra_2016 ) max_jetBtagCSV_ = 0.80;
        else assert(0);
}


bool RecoMuonSelectorTight_2mu1b1j::operator()(const RecoMuon& muon) const
{
        if ( debug_ ) {
                std::cout << "<RecoMuonSelectorTight_2mu1b1j::operator()>:" << std::endl;
                std::cout << " muon: pT = " << muon.pt_ << ", eta = " << muon.eta_ << ", phi = " << muon.phi_ << ", charge = " << muon.charge_ << std::endl;
        }
        if ( muon.pt_ < min_pt_ ) {
                if ( debug_ ) std::cout << "FAILS pT cut." << std::endl;
                return false;
        }
        if ( muon.absEta_ > max_absEta_ ) {
                if ( debug_ ) std::cout << "FAILS eta cut." << std::endl;
                return false;
        }
        if ( std::fabs(muon.dxy_) > max_dxy_ ) {
                if ( debug_ ) std::cout << "FAILS dxy cut." << std::endl;
                return false;
        }
        if ( std::fabs(muon.dz_) > max_dz_ ) {
                if ( debug_ ) std::cout << "FAILS dz cut." << std::endl;
                return false;
        }
        if ( muon.relIso_ > max_relIso_ ) {
                if ( debug_ ) std::cout << "FAILS relIso cut." << std::endl;
                return false;
        }
        if ( muon.sip3d_ > max_sip3d_ ) {
                if ( debug_ ) std::cout << "FAILS sip3d cut." << std::endl;
                return false;
        }
        if ( apply_looseIdPOG_ && !muon.passesLooseIdPOG_ ) {
                if ( debug_ ) std::cout << "FAILS looseIdPOG cut." << std::endl;
                return false;
        }
        if ( muon.jetBtagCSV_ > max_jetBtagCSV_ ) {
                if ( debug_ ) std::cout << "FAILS jetBtagCSV cut." << std::endl;
                return false;
        }
        if ( apply_mediumIdPOG_ && !muon.passesMediumIdPOG_ ) {
                if ( debug_ ) std::cout << "FAILS mediumIdPOG cut." << std::endl;
                return false;
        }
        if ( apply_tightCharge_ && muon.tightCharge_ < 2 ) {
                if ( debug_ ) std::cout << "FAILS tightCharge cut." << std::endl;
                return false;
        }
        if ( muon.mvaRawTTH_ < min_mvaTTH_ ) {
                if ( debug_ ) std::cout << "FAILS mvaTTH cut." << std::endl;
                return false;
        }
        // muon passes all cuts
        return true;
}
