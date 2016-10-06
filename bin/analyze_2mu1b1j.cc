
#include "FWCore/ParameterSet/interface/ParameterSet.h" // edm::ParameterSet
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h" // edm::readPSetsFrom()
#include "FWCore/ParameterSet/interface/FileInPath.h" // edm::FileInPath
#include "FWCore/Utilities/interface/Exception.h" // cms::Exception
#include "PhysicsTools/FWLite/interface/TFileService.h" // fwlite::TFileService
#include "DataFormats/FWLite/interface/InputSource.h" // fwlite::InputSource
#include "DataFormats/FWLite/interface/OutputFiles.h" // fwlite::OutputFiles
#include "DataFormats/Math/interface/LorentzVector.h" // math::PtEtaPhiMLorentzVector
#include "DataFormats/Math/interface/deltaR.h" // deltaR

#include <Rtypes.h> // Int_t, Long64_t, Double_t
#include <TChain.h> // TChain
#include <TTree.h> // TTree
#include <TBenchmark.h> // TBenchmark
#include <TString.h> // TString, Form

#include "tthAnalysis/HiggsToTauTau/interface/RecoLepton.h" // RecoLepton
#include "tthAnalysis/HiggsToTauTau/interface/RecoJet.h" // RecoJet
#include "tthAnalysis/HiggsToTauTau/interface/GenLepton.h" // GenLepton
#include "tthAnalysis/HiggsToTauTau/interface/GenJet.h" // GenJet
#include "tthAnalysis/HiggsToTauTau/interface/KeyTypes.h"
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonReader.h" // RecoMuonReader
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetReader.h" // RecoJetReader
#include "tthAnalysis/HiggsToTauTau/interface/GenLeptonReader.h" // GenLeptonReader
#include "tthAnalysis/HiggsToTauTau/interface/GenJetReader.h" // GenJetReader
#include "tthAnalysis/HiggsToTauTau/interface/convert_to_ptrs.h" // convert_to_ptrs
#include "tthAnalysis/HiggsToTauTau/interface/ParticleCollectionCleaner.h" // RecoElectronCollectionCleaner, RecoMuonCollectionCleaner, RecoHadTauCollectionCleaner, RecoJetCollectionCleaner
#include "tthAnalysis/HiggsToTauTau/interface/ParticleCollectionGenMatcher.h" // RecoElectronCollectionGenMatcher, RecoMuonCollectionGenMatcher, RecoHadTauCollectionGenMatcher, RecoJetCollectionGenMatcher
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonCollectionSelectorLoose.h" // RecoMuonCollectionSelectorLoose
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonCollectionSelectorFakeable.h" // RecoMuonCollectionSelectorFakeable
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonCollectionSelectorTight_2mu1b1j.h" // RecoMuonCollectionSelectorTight
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetCollectionSelector.h" // RecoJetCollectionSelector
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetCollectionSelectorBtag.h" // RecoJetCollectionSelectorBtagLoose, RecoJetCollectionSelectorBtagMedium
#include "tthAnalysis/HiggsToTauTau/interface/RunLumiEventSelector.h" // RunLumiEventSelector
#include "tthAnalysis/HiggsToTauTau/interface/MuonHistManager.h" // MuonHistManager
#include "tthAnalysis/HiggsToTauTau/interface/JetHistManager.h" // JetHistManager
#include "tthAnalysis/HiggsToTauTau/interface/MEtHistManager.h" // MEtHistManager
#include "tthAnalysis/HiggsToTauTau/interface/EvtHistManager_2mu1b1j.h" // EvtHistManager_2mu1b1j
#include "tthAnalysis/HiggsToTauTau/interface/EvtHistManager_2mu1b1jCategory.h" // EvtHistManager_2mu1b1j
#include "tthAnalysis/HiggsToTauTau/interface/leptonTypes.h" // getLeptonType, kElectron, kMuon
#include "tthAnalysis/HiggsToTauTau/interface/backgroundEstimation.h" // prob_chargeMisId
#include "tthAnalysis/HiggsToTauTau/interface/hltPath.h" // hltPath, create_hltPaths, hltPaths_setBranchAddresses, hltPaths_isTriggered, hltPaths_delete
#include "tthAnalysis/HiggsToTauTau/interface/data_to_MC_corrections.h"
#include "tthAnalysis/HiggsToTauTau/interface/lutAuxFunctions.h" // loadTH2, get_sf_from_TH2
#include "tthAnalysis/HiggsToTauTau/interface/cutFlowTable.h" // cutFlowTableType

#include <boost/range/algorithm/copy.hpp> // boost::copy()
#include <boost/range/adaptor/map.hpp> // boost::adaptors::map_keys

#include <iostream> // std::cerr, std::fixed
#include <iomanip> // std::setprecision(), std::setw()
#include <string> // std::string
#include <vector> // std::vector<>
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <algorithm> // std::sort(), std::find()
#include <fstream> // std::ofstream
#include <assert.h> // assert
#include <array> // std::array<>

#define EPS 1E-2

typedef math::PtEtaPhiMLorentzVector LV;
typedef std::vector<std::string> vstring;


/**
 * @brief Auxiliary function used for sorting leptons by decreasing pT
 * @param Given pair of leptons
 * @return True, if first lepton has higher pT; false if second lepton has higher pT
 */
bool isHigherPt(const GenParticle* particle1, const GenParticle* particle2)
{
        return (particle1->pt_ > particle2->pt_);
}

/**
 * @brief Auxiliary function used for sorting jets by decreasing btagging CSV value
 * @param Given pair of jets
 * @return True, if first jet has higher CSV; false if second jet has higher CSV
 */
bool isHigherCSV(const RecoJet* jet1, const RecoJet* jet2)
{
        return (jet1->BtagCSV_ > jet2->BtagCSV_);
}


/**
 * @brief Produce datacard and control plots for 2mu1b1j categories.
 */
int main(int argc, char* argv[])
{
        //--- parse command-line arguments
        if ( argc < 2 ) {
                std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
                return EXIT_SUCCESS;
        }

        std::cout << "<analyze_2mu1b1j>:" << std::endl;

        //--- keep track of time it takes the macro to execute
        TBenchmark clock;
        clock.Start("analyze_2mu1b1j");

        //--- read python configuration parameters
        if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") )
                throw cms::Exception("analyze_2mu1b1j")
                      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

        edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

        edm::ParameterSet cfg_analyze = cfg.getParameter<edm::ParameterSet>("analyze_2mu1b1j");

        std::string treeName = cfg_analyze.getParameter<std::string>("treeName");

        std::string process_string = cfg_analyze.getParameter<std::string>("process");
        // bool isSignal = ( process_string == "signal" ) ? true : false;

        vstring triggerNames_1mu = cfg_analyze.getParameter<vstring>("triggers_1mu");
        std::vector<hltPath*> triggers_1mu = create_hltPaths(triggerNames_1mu);
        bool use_triggers_1mu = cfg_analyze.getParameter<bool>("use_triggers_1mu");

        vstring triggerNames_2mu = cfg_analyze.getParameter<vstring>("triggers_2mu");
        std::vector<hltPath*> triggers_2mu = create_hltPaths(triggerNames_2mu);
        bool use_triggers_2mu = cfg_analyze.getParameter<bool>("use_triggers_2mu");

        enum { kLoose, kFakeable, kTight };
        std::string leptonSelection_string = "Tight";
        int leptonSelection = kTight;


        bool isMC = cfg_analyze.getParameter<bool>("isMC");
        std::string central_or_shift = cfg_analyze.getParameter<std::string>("central_or_shift");
        // double lumiScale = ( process_string != "data_obs" ) ? cfg_analyze.getParameter<double>("lumiScale") : 1.;

        std::string jet_btagWeight_branch = ( isMC ) ? "Jet_bTagWeight" : "";

        int jetPt_option = RecoJetReader::kJetPt_central;


        // currently commented out because using "central" and !isMC

        // if ( isMC && central_or_shift != "central" ) {
        //         TString central_or_shift_tstring = central_or_shift.data();
        //         std::string shiftUp_or_Down = "";
        //         if      ( central_or_shift_tstring.EndsWith("Up")   ) shiftUp_or_Down = "Up";
        //         else if ( central_or_shift_tstring.EndsWith("Down") ) shiftUp_or_Down = "Down";
        //         else throw cms::Exception("analyze_2mu1b1j")
        //                       << "Invalid Configuration parameter 'central_or_shift' = " << central_or_shift << " !!\n";
        //         if      ( central_or_shift_tstring.BeginsWith("CMS_ttHl_btag_HF")       ) jet_btagWeight_branch = "Jet_bTagWeightHF" + shiftUp_or_Down;
        //         else if ( central_or_shift_tstring.BeginsWith("CMS_ttHl_btag_HFStats1") ) jet_btagWeight_branch = "Jet_bTagWeightHFStats1" + shiftUp_or_Down;
        //         else if ( central_or_shift_tstring.BeginsWith("CMS_ttHl_btag_HFStats2") ) jet_btagWeight_branch = "Jet_bTagWeightHFStats2" + shiftUp_or_Down;
        //         else if ( central_or_shift_tstring.BeginsWith("CMS_ttHl_btag_LF")       ) jet_btagWeight_branch = "Jet_bTagWeightLF" + shiftUp_or_Down;
        //         else if ( central_or_shift_tstring.BeginsWith("CMS_ttHl_btag_LFStats1") ) jet_btagWeight_branch = "Jet_bTagWeightLFStats1" + shiftUp_or_Down;
        //         else if ( central_or_shift_tstring.BeginsWith("CMS_ttHl_btag_LFStats2") ) jet_btagWeight_branch = "Jet_bTagWeightLFStats2" + shiftUp_or_Down;
        //         else if ( central_or_shift_tstring.BeginsWith("CMS_ttHl_btag_cErr1")    ) jet_btagWeight_branch = "Jet_bTagWeightcErr1" + shiftUp_or_Down;
        //         else if ( central_or_shift_tstring.BeginsWith("CMS_ttHl_btag_cErr2")    ) jet_btagWeight_branch = "Jet_bTagWeightcErr2" + shiftUp_or_Down;
        //         else if ( central_or_shift_tstring.BeginsWith("CMS_ttHl_JES") ) {
        //                 jet_btagWeight_branch = "Jet_bTagWeightJES" + shiftUp_or_Down;
        //                 if      ( shiftUp_or_Down == "Up"   ) jetPt_option = RecoJetReader::kJetPt_jecUp;
        //                 else if ( shiftUp_or_Down == "Down" ) jetPt_option = RecoJetReader::kJetPt_jecDown;
        //                 else assert(0);
        //         } else throw cms::Exception("analyze_2mu1b1j")
        //                       << "Invalid Configuration parameter 'central_or_shift' = " << central_or_shift << " !!\n";
        // }


        // currently commented out because selEventsFileName_input == ""

        // std::string selEventsFileName_input = cfg_analyze.getParameter<std::string>("selEventsFileName_input");
        // std::cout << "selEventsFileName_input = " << selEventsFileName_input << std::endl;

        RunLumiEventSelector* run_lumi_eventSelector = 0;
        // if ( selEventsFileName_input != "" ) {
        //         edm::ParameterSet cfgRunLumiEventSelector;
        //         cfgRunLumiEventSelector.addParameter<std::string>("inputFileName", selEventsFileName_input);
        //         cfgRunLumiEventSelector.addParameter<std::string>("separator", ":");
        //         run_lumi_eventSelector = new RunLumiEventSelector(cfgRunLumiEventSelector);
        // }


        // # Start reading events
        //
        // FWLite (pronounced "framework-light") is just that -- a ROOT session with CMS data format libraries loaded
        // https://cmssdt.cern.ch/SDT/doxygen/CMSSW_6_2_9/doc/html/d2/d77/classfwlite_1_1InputSource.html#af3867def6afcc5a7b976e0182f3d339f

        fwlite::InputSource inputFiles(cfg);
        int maxEvents = inputFiles.maxEvents();
        std::cout << " maxEvents = " << maxEvents << std::endl;
        unsigned reportEvery = inputFiles.reportAfter();

        fwlite::OutputFiles outputFile(cfg);
        fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

        // A chain is a collection of files containg TTree objects.
        // https://root.cern.ch/doc/master/classTChain.html

        TChain* inputTree = new TChain(treeName.data());
        for ( std::vector<std::string>::const_iterator inputFileName = inputFiles.files().begin();
              inputFileName != inputFiles.files().end(); ++inputFileName ) {
                std::cout << "input Tree: adding file = " << (*inputFileName) << std::endl;
                inputTree->AddFile(inputFileName->data());
        }

        // if there are no entries, throw exception (why not just "== 0"?)

        if ( inputTree->GetListOfFiles()->GetEntries() == 1 ) {
                throw cms::Exception("analyze_2mu1b1j")
                      << "Failed to identify input Tree !!\n";
        }

        // CV: need to call TChain::LoadTree before processing first event
        //     in order to prevent ROOT causing a segmentation violation,
        //     cf. http://root.cern.ch/phpBB3/viewtopic.php?t=10062
        inputTree->LoadTree(0);

        std::cout << "input Tree contains " << inputTree->GetEntries()
                  << " Entries in " << inputTree->GetListOfFiles()->GetEntries()
                  << " files." << std::endl;

        // # Declare event-level variables

        // _TYPE and _KEY are defined in KeyTypes.h and are (UInt_t, UInt_t, ULong64_t, "run", "lumi", "evt")

        RUN_TYPE run;
        inputTree->SetBranchAddress(RUN_KEY, &run);
        LUMI_TYPE lumi;
        inputTree->SetBranchAddress(LUMI_KEY, &lumi);
        EVT_TYPE event;
        inputTree->SetBranchAddress(EVT_KEY, &event);

        hltPaths_setBranchAddresses(inputTree, triggers_1mu);
        hltPaths_setBranchAddresses(inputTree, triggers_2mu);

        MET_PT_TYPE met_pt;
        inputTree->SetBranchAddress(MET_PT_KEY, &met_pt);
        MET_ETA_TYPE met_eta;
        inputTree->SetBranchAddress(MET_ETA_KEY, &met_eta);
        MET_PHI_TYPE met_phi;
        inputTree->SetBranchAddress(MET_PHI_KEY, &met_phi);


        // # Declare particle collections

        RecoMuonReader* muonReader = new RecoMuonReader("nselLeptons", "selLeptons");
        muonReader->setBranchAddresses(inputTree);
        RecoMuonCollectionGenMatcher muonGenMatcher;
        RecoMuonCollectionSelectorLoose preselMuonSelector;
        RecoMuonCollectionSelectorFakeable fakeableMuonSelector;
        RecoMuonCollectionSelectorTight_2mu1b1j tightMuonSelector(-1, run_lumi_eventSelector != 0);

        RecoJetReader* jetReader = new RecoJetReader("nJet", "Jet");
        jetReader->setJetPt_central_or_shift(jetPt_option);
        jetReader->setBranchName_BtagWeight(jet_btagWeight_branch);
        jetReader->setBranchAddresses(inputTree);
        RecoJetCollectionGenMatcher jetGenMatcher;
        RecoJetCollectionCleaner jetCleaner(0.5);
        RecoJetCollectionSelector jetSelector;
        RecoJetCollectionSelectorBtagLoose jetSelectorBtagLoose;
        RecoJetCollectionSelectorBtagMedium jetSelectorBtagMedium;

        // GenLeptonReader* genLeptonReader = 0;
        // GenJetReader* genJetReader = 0;
        // if ( isMC ) {
        //         genLeptonReader = new GenLeptonReader("nGenLep", "GenLep");
        //         genLeptonReader->setBranchAddresses(inputTree);
        //         genJetReader = new GenJetReader("nGenJet", "GenJet");
        //         genJetReader->setBranchAddresses(inputTree);
        // }


        //--- open output file containing run:lumi:event numbers of events passing final event selection criteria
        std::string selEventsFileName_output = cfg_analyze.getParameter<std::string>("selEventsFileName_output");
        std::ostream* selEventsFile = new std::ofstream(selEventsFileName_output.data(), std::ios::out);

        //--- declare histograms
        MuonHistManager preselMuonHistManager(makeHistManager_cfg(process_string,
                                                                  Form("2mu1b1j_%s/presel/muons", leptonSelection_string.data()),
                                                                  central_or_shift));
        preselMuonHistManager.bookHistograms(fs);
        JetHistManager preselJetHistManager(makeHistManager_cfg(process_string,
                                                                Form("2mu1b1j_%s/presel/jets", leptonSelection_string.data()),
                                                                central_or_shift));
        preselJetHistManager.bookHistograms(fs);
        JetHistManager preselBJet_looseHistManager(makeHistManager_cfg(process_string,
                                                                       Form("2mu1b1j_%s/presel/BJets_loose", leptonSelection_string.data()),
                                                                       central_or_shift));
        preselBJet_looseHistManager.bookHistograms(fs);
        JetHistManager preselBJet_mediumHistManager(makeHistManager_cfg(process_string,
                                                                        Form("2mu1b1j_%s/presel/BJets_medium", leptonSelection_string.data()),
                                                                        central_or_shift));
        preselBJet_mediumHistManager.bookHistograms(fs);
        MEtHistManager preselMEtHistManager(makeHistManager_cfg(process_string,
                                                                Form("2mu1b1j_%s/presel/met", leptonSelection_string.data()),
                                                                central_or_shift));
        preselMEtHistManager.bookHistograms(fs);
        EvtHistManager_2mu1b1j preselEvtHistManager(makeHistManager_cfg(process_string,
                                                                        Form("2mu1b1j_%s/presel/evt", leptonSelection_string.data()),
                                                                        central_or_shift));
        preselEvtHistManager.bookHistograms(fs);

        MuonHistManager selMuonHistManager(makeHistManager_cfg(process_string,
                                                               Form("2mu1b1j_%s/sel/muons", leptonSelection_string.data()),
                                                               central_or_shift));
        selMuonHistManager.bookHistograms(fs);

        JetHistManager selJetHistManager(makeHistManager_cfg(process_string,
                                                             Form("2mu1b1j_%s/sel/jets", leptonSelection_string.data()),
                                                             central_or_shift));
        selJetHistManager.bookHistograms(fs);
        JetHistManager selJetHistManager_lead(makeHistManager_cfg(process_string,
                                                                  Form("2mu1b1j_%s/sel/leadJet", leptonSelection_string.data()),
                                                                  central_or_shift,
                                                                  0));
        selJetHistManager_lead.bookHistograms(fs);
        JetHistManager selJetHistManager_sublead(makeHistManager_cfg(process_string,
                                                                     Form("2mu1b1j_%s/sel/subleadJet", leptonSelection_string.data()),
                                                                     central_or_shift,
                                                                     1));
        selJetHistManager_sublead.bookHistograms(fs);

        JetHistManager selBJet_looseHistManager(makeHistManager_cfg(process_string,
                                                                    Form("2mu1b1j_%s/sel/BJets_loose", leptonSelection_string.data()),
                                                                    central_or_shift));
        selBJet_looseHistManager.bookHistograms(fs);
        JetHistManager selBJet_looseHistManager_lead(makeHistManager_cfg(process_string,
                                                                         Form("2mu1b1j_%s/sel/leadBJet_loose", leptonSelection_string.data()),
                                                                         central_or_shift,
                                                                         0));
        selBJet_looseHistManager_lead.bookHistograms(fs);
        JetHistManager selBJet_looseHistManager_sublead(makeHistManager_cfg(process_string,
                                                                            Form("2mu1b1j_%s/sel/subleadBJet_loose", leptonSelection_string.data()),
                                                                            central_or_shift,
                                                                            1));
        selBJet_looseHistManager_sublead.bookHistograms(fs);
        JetHistManager selBJet_mediumHistManager(makeHistManager_cfg(process_string,
                                                                     Form("2mu1b1j_%s/sel/BJets_medium", leptonSelection_string.data()),
                                                                     central_or_shift));
        selBJet_mediumHistManager.bookHistograms(fs);

        MEtHistManager selMEtHistManager(makeHistManager_cfg(process_string,
                                                             Form("2mu1b1j_%s/sel/met", leptonSelection_string.data()),
                                                             central_or_shift));
        selMEtHistManager.bookHistograms(fs);

        EvtHistManager_2mu1b1j selEvtHistManager(makeHistManager_cfg(process_string,
                                                                     Form("2mu1b1j_%s/sel/evt", leptonSelection_string.data()),
                                                                     central_or_shift));
        selEvtHistManager.bookHistograms(fs);

        EvtHistManager_2mu1b1jCategory categoryAHistManager(makeHistManager_cfg(process_string,
                                                                     Form("2mu1b1jCategoryA_%s/sel/evt", leptonSelection_string.data()),
                                                                     central_or_shift));
        categoryAHistManager.bookHistograms(fs);

        EvtHistManager_2mu1b1jCategory categoryBHistManager(makeHistManager_cfg(process_string,
                                                                     Form("2mu1b1jCategoryB_%s/sel/evt", leptonSelection_string.data()),
                                                                     central_or_shift));
        categoryBHistManager.bookHistograms(fs);

        EvtHistManager_2mu1b1jCategory categoryBHistWithoutCriteria5Manager(makeHistManager_cfg(process_string,
                                                                     Form("2mu1b1jCategoryBWithoutCriteria5_%s/sel/evt", leptonSelection_string.data()),
                                                                     central_or_shift));
        categoryBHistManager.bookHistograms(fs);

        int numEntries = inputTree->GetEntries();
        int analyzedEntries = 0;
        int selectedEntries = 0;
        double selectedEntries_weighted = 0.;
        cutFlowTableType cutFlowTable;

        // loop over all the events in inputTree

        for ( int idxEntry = 0; idxEntry < numEntries && (maxEvents == -1 || idxEntry < maxEvents); ++idxEntry ) {
                if ( idxEntry > 0 && (idxEntry % reportEvery) == 0 ) {
                        std::cout << "processing Entry " << idxEntry << " (" << selectedEntries << " Entries selected)" << std::endl;
                }
                ++analyzedEntries;

                // load entry with ID

                inputTree->GetEntry(idxEntry);

                // check if run number matches any of the runs containing events to be selected

                if ( run_lumi_eventSelector && !(*run_lumi_eventSelector)(run, lumi, event) )
                        continue;
                cutFlowTable.update("run:ls:event selection");

                bool isTriggered_1mu = hltPaths_isTriggered(triggers_1mu);
                bool isTriggered_2mu = hltPaths_isTriggered(triggers_2mu);

                bool selTrigger_1mu = use_triggers_1mu && isTriggered_1mu;
                bool selTrigger_2mu = use_triggers_2mu && isTriggered_2mu;

                if ( (!selTrigger_1mu) && (!selTrigger_2mu) ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS trigger selection." << std::endl;
                                std::cout << ", selTrigger_1mu = " << selTrigger_1mu
                                          << ", selTrigger_2mu = " << selTrigger_2mu;
                        }
                        continue;
                }

                //--- rank triggers by priority and ignore triggers of lower priority if a trigger of higher priority has fired for given event;
                //    the ranking of the triggers is as follows: 2mu, 1e1mu, 2e, 1mu, 1e
                // CV: this logic is necessary to avoid that the same event is selected multiple times when processing different primary datasets
                if ( !isMC ) {

                        if ( selTrigger_1mu && isTriggered_2mu ) {
                                if ( run_lumi_eventSelector ) {
                                        std::cout << "event FAILS trigger selection." << std::endl;
                                        std::cout << " (selTrigger_1mu = " << selTrigger_1mu
                                                  << ", isTriggered_2mu = " << isTriggered_2mu
                                                  << std::endl;
                                }
                                continue;
                        }
                }
                cutFlowTable.update("trigger");


                // # Build collections of muons and jets

                std::vector<RecoMuon> muons = muonReader->read();
                std::vector<const RecoMuon*> muon_ptrs = convert_to_ptrs(muons);
                std::vector<const RecoMuon*> cleanedMuons = muon_ptrs;   // CV: no cleaning needed for muons, as they have the highest priority in the overlap removal
                std::vector<const RecoMuon*> preselMuons = preselMuonSelector(cleanedMuons);
                std::vector<const RecoMuon*> fakeableMuons = fakeableMuonSelector(preselMuons);
                std::vector<const RecoMuon*> tightMuons = tightMuonSelector(preselMuons);

                // setting selMuons tightMuons, because leptonSelection == 'tight'

                std::vector<const RecoMuon*> selMuons;
                if      ( leptonSelection == kLoose    ) selMuons = preselMuons;
                else if ( leptonSelection == kFakeable ) selMuons = fakeableMuons;
                else if ( leptonSelection == kTight    ) selMuons = tightMuons;
                else assert(0);
                //for ( size_t idxPreselMuon = 0; idxPreselMuon < preselMuons.size(); ++idxPreselMuon ) {
                //  std::cout << "preselMuon #" << idxPreselMuon << ":" << std::endl;
                //  std::cout << (*preselMuons[idxPreselMuon]);
                //}

                //--- build collections of jets and select subset of jets passing b-tagging criteria
                std::vector<RecoJet> jets = jetReader->read();
                std::vector<const RecoJet*> jet_ptrs = convert_to_ptrs(jets);
                std::vector<const RecoJet*> cleanedJets = jetCleaner(jet_ptrs, selMuons);
                std::vector<const RecoJet*> selJets = jetSelector(cleanedJets);
                std::vector<const RecoJet*> selBJets_loose = jetSelectorBtagLoose(cleanedJets);
                std::vector<const RecoJet*> selBJets_medium = jetSelectorBtagMedium(cleanedJets);

                //--- build collections of generator level particles
                std::vector<GenLepton> genMuons;
                std::vector<GenJet> genJets;

                // if ( isMC ) {
                //         genLeptons = genLeptonReader->read();
                //         for ( std::vector<GenLepton>::const_iterator genLepton = genLeptons.begin();
                //               genLepton != genLeptons.end(); ++genLepton ) {
                //                 int abs_pdgId = std::abs(genLepton->pdgId_);
                //                 if      ( abs_pdgId == 11 ) genElectrons.push_back(*genLepton);
                //                 else if ( abs_pdgId == 13 ) genMuons.push_back(*genLepton);
                //         }
                //         genHadTaus = genHadTauReader->read();
                //         genJets = genJetReader->read();
                // }

                //--- match reconstructed to generator level particles
                // if ( isMC ) {
                //         muonGenMatcher.addGenLeptonMatch(preselMuons, genLeptons, 0.3);
                //         muonGenMatcher.addGenHadTauMatch(preselMuons, genHadTaus, 0.3);
                //         muonGenMatcher.addGenJetMatch(preselMuons, genJets, 0.5);
                //
                //         jetGenMatcher.addGenLeptonMatch(selJets, genLeptons, 0.3);
                //         jetGenMatcher.addGenHadTauMatch(selJets, genHadTaus, 0.3);
                //         jetGenMatcher.addGenJetMatch(selJets, genJets, 0.5);
                // }

                //--- apply preselection
                std::sort(preselMuons.begin(), preselMuons.end(), isHigherPt);

                // require exactly two leptons passing loose preselection criteria to avoid overlap with 3l category
                if ( preselMuons.size() != 2 ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS preselMuons selection." << std::endl;
                                std::cout << " (#preselMuons = " << preselMuons.size() << ")" << std::endl;
                                for ( size_t idxPreselMuon = 0; idxPreselMuon < preselMuons.size(); ++idxPreselMuon ) {
                                        std::cout << "preselMuons #" << idxPreselMuon << ":" << std::endl;
                                        std::cout << (*preselMuons[idxPreselMuon]);
                                }
                        }
                        continue;
                }
                cutFlowTable.update("2 presel muons");

                const RecoMuon* preselMuon_lead = preselMuons[0];
                const RecoMuon* preselMuon_sublead = preselMuons[1];

                // apply requirement on jets (incl. b-tagged jets)

                std::sort(selJets.begin(), selJets.end(), isHigherPt);

                if ( !(selJets.size() >= 2) ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS selJets selection." << std::endl;
                                std::cout << " (#selJets = " << selJets.size() << ")" << std::endl;
                        }
                        continue;
                }

                const RecoJet* selJet_lead = selJets[0];
                const RecoJet* selJet_sublead = selJets[1];

                cutFlowTable.update(">= 2 jets (1)");


                if ( !(selBJets_medium.size() >= 1) ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS selBJets selection." << std::endl;
                                std::cout << "(#selBJets_medium = " << selBJets_medium.size() << ")" << std::endl;
                        }
                        continue;
                }
                cutFlowTable.update("1 medium b-jet (1)");


                //--- compute event-level weight for data/MC correction of b-tagging efficiency and mistag rate
                //   (using the method "Event reweighting using scale factors calculated with a tag and probe method",
                //    described on the BTV POG twiki https://twiki.cern.ch/twiki/bin/view/CMS/BTagShapeCalibration )
                double evtWeight = 1.;
                // if ( isMC ) {
                //         evtWeight *= lumiScale;
                //         evtWeight *= pileupWeight;
                //         for ( std::vector<const RecoJet*>::const_iterator jet = selJets.begin();
                //               jet != selJets.end(); ++jet ) {
                //                 evtWeight *= (*jet)->BtagWeight_;
                //         }
                // }

                //--- apply data/MC corrections for trigger efficiency,
                //    and efficiencies for lepton to pass loose identification and isolation criteria
                // if ( isMC ) {
                //         evtWeight *= sf_triggerEff(2,
                //                                    preselMuon_lead_type, preselMuon_lead->pt_, preselMuon_lead->eta_,
                //                                    preselMuon_sublead_type, preselMuon_sublead->pt_, preselMuon_sublead->eta_);
                //
                //         evtWeight *= sf_leptonID_and_Iso_loose(2,
                //                                                preselMuon_lead_type, preselMuon_lead->pt_, preselMuon_lead->eta_,
                //                                                preselMuon_sublead_type, preselMuon_sublead->pt_, preselMuon_sublead->eta_);
                // }


                // check that muons have opposite charge
                if ( !((preselMuon_lead->charge_ + preselMuon_sublead->charge_) == 0.0) ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS muons opposite charge criteria." << std::endl;
                        }
                        continue;
                }
                cutFlowTable.update("muons opposite charge criteria");


                double massOfOppositeChargeMuons = (preselMuon_lead->p4_ + preselMuon_sublead->p4_).mass();


                //--- fill histograms with events passing preselection
                preselMuonHistManager.fillHistograms(preselMuons, evtWeight);
                preselJetHistManager.fillHistograms(selJets, evtWeight);
                selBJet_mediumHistManager.fillHistograms(selBJets_medium, evtWeight);
                preselEvtHistManager.fillHistograms(preselMuons.size(),
                                                    selJets.size(),
                                                    selBJets_medium.size(),
                                                    massOfOppositeChargeMuons,
                                                    evtWeight);

                //--- apply final event selection

                // require exactly two leptons passing tight selection criteria of final event selection
                if ( !(selMuons.size() == 2) ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS selMuons selection." << std::endl;
                                std::cout << " (#selMuons = " << selMuons.size() << ")" << std::endl;
                                for ( size_t idxSelMuon = 0; idxSelMuon < selMuons.size(); ++idxSelMuon ) {
                                        std::cout << "selMuon #" << idxSelMuon << ":" << std::endl;
                                        std::cout << (*selMuons[idxSelMuon]);
                                }
                        }
                        continue;
                }
                cutFlowTable.update("2 sel muons", evtWeight);

                const RecoMuon* selMuon_lead = selMuons[0];
                const RecoMuon* selMuon_sublead = selMuons[1];

                // require 2 selMuons
                if ( !(selMuons.size() == 2) ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS trigger selection for given selMuons multiplicity."
                                          << std::endl;
                                std::cout << " (#selMuons = " << selMuons.size()
                                          << ")" << std::endl;
                        }
                        continue;
                }
                cutFlowTable.update("selMuons == 2");

                // apply requirement on jets (incl. b-tagged jets)
                if ( !(selJets.size() >= 2) ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS selJets selection." << std::endl;
                                std::cout << " (#selJets = " << selJets.size() << ")" << std::endl;
                                for ( size_t idxSelJet = 0; idxSelJet < selJets.size(); ++idxSelJet ) {
                                        std::cout << "selJet #" << idxSelJet << ":" << std::endl;
                                        std::cout << (*selJets[idxSelJet]);
                                }
                        }
                        continue;
                }
                cutFlowTable.update(">= 2 jets (2)", evtWeight);

                // at least 1 medium b-jet (if there is 1, there must be a second one that was not registered)
                if ( !(selBJets_medium.size() >= 1)) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS selBJets selection." << std::endl;
                                std::cout << " (#selBJets_medium = " << selBJets_medium.size() << ")" << std::endl;

                                for ( size_t idxSelBJet_medium = 0; idxSelBJet_medium < selBJets_medium.size(); ++idxSelBJet_medium ) {
                                        std::cout << "selJet #" << idxSelBJet_medium << ":" << std::endl;
                                        std::cout << (*selJets[idxSelBJet_medium]);
                                }
                        }
                        continue;
                }
                cutFlowTable.update(">= 1 medium b-jet (2)", evtWeight);

                // muon pt cuts
                double minPt = 25.;

                if ( !(selMuon_lead->pt_ > minPt && selMuon_sublead->pt_ > minPt) ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS muon pT selection." << std::endl;
                                std::cout << " (leading selMuon pT = " << selMuon_lead->pt_
                                          << ", subleading selMuon pT = " << selMuon_sublead->pt_
                                          << std::endl;
                        }
                        continue;
                }
                cutFlowTable.update("muon pt cut", evtWeight);

                // check that muon and antimuon
                bool hasTwoOppositeSignMuons = (selMuon_lead->charge_ + selMuon_sublead->charge_) == 0;

                if ( !hasTwoOppositeSignMuons ) {
                        if ( run_lumi_eventSelector ) {
                                std::cout << "event FAILS muon charge selection." << std::endl;
                                std::cout << " (leading selMuon charge = " << selMuon_lead->charge_
                                          << ", subleading selMuon charge = " << selMuon_sublead->charge_
                                          << ")" << std::endl;
                        }
                        continue;
                }
                cutFlowTable.update("sel lepton charge");

                // what does leptonSelection == kFakeable means, what is tightMuons cut here? We require == 2 muons in earlier code
                if ( leptonSelection == kFakeable ) {
                        if ( tightMuons.size() != 2 ) {
                                if ( run_lumi_eventSelector ) {
                                        std::cout << "event FAILS tightMuons selection." << std::endl;
                                        std::cout << " (#tightMuons = " << tightMuons.size() << ")" << std::endl;
                                }
                                continue;   // CV: avoid overlap with signal region
                        }
                }
                cutFlowTable.update("passed tightmuons == 2", evtWeight);



                // //--- apply data/MC corrections for efficiencies of leptons passing the loose identification and isolation criteria
                // //    to also pass the tight identification and isolation criteria
                // if ( isMC ) {
                //         double sf_tight_to_loose = 1.;
                //         if ( leptonSelection == kFakeable ) {
                //                 sf_tight_to_loose = sf_leptonID_and_Iso_fakeable_to_loose(3,
                //                                                                           preselLepton_lead_type, preselLepton_lead->pt_, preselLepton_lead->eta_,
                //                                                                           preselLepton_sublead_type, preselLepton_sublead->pt_, preselLepton_sublead->eta_,
                //                                                                           preselLepton_third_type, preselLepton_third->pt_, preselLepton_third->eta_);
                //         } else if ( leptonSelection == kTight ) {
                //                 sf_tight_to_loose = sf_leptonID_and_Iso_tight_to_loose(3,
                //                                                                        preselLepton_lead_type, preselLepton_lead->pt_, preselLepton_lead->eta_,
                //                                                                        preselLepton_sublead_type, preselLepton_sublead->pt_, preselLepton_sublead->eta_,
                //                                                                        preselLepton_third_type, preselLepton_third->pt_, preselLepton_third->eta_);
                //         }
                //         evtWeight *= sf_tight_to_loose;
                // }
                //
                // if ( leptonSelection == kFakeable ) {
                //         TH2* lutFakeRate_lead = 0;
                //         if      ( std::abs(selLepton_lead->pdgId_) == 11 ) lutFakeRate_lead = lutFakeRate_e;
                //         else if ( std::abs(selLepton_lead->pdgId_) == 13 ) lutFakeRate_lead = lutFakeRate_mu;
                //         assert(lutFakeRate_lead);
                //         double prob_fake_lead = get_sf_from_TH2(lutFakeRate_lead, selLepton_lead->pt_, selLepton_lead->eta_);
                //
                //         TH2* lutFakeRate_sublead = 0;
                //         if      ( std::abs(selLepton_sublead->pdgId_) == 11 ) lutFakeRate_sublead = lutFakeRate_e;
                //         else if ( std::abs(selLepton_sublead->pdgId_) == 13 ) lutFakeRate_sublead = lutFakeRate_mu;
                //         assert(lutFakeRate_sublead);
                //         double prob_fake_sublead = get_sf_from_TH2(lutFakeRate_sublead, selLepton_sublead->pt_, selLepton_sublead->eta_);
                //
                //         TH2* lutFakeRate_third = 0;
                //         if      ( std::abs(selLepton_third->pdgId_) == 11 ) lutFakeRate_third = lutFakeRate_e;
                //         else if ( std::abs(selLepton_third->pdgId_) == 13 ) lutFakeRate_third = lutFakeRate_mu;
                //         assert(lutFakeRate_third);
                //         double prob_fake_third = get_sf_from_TH2(lutFakeRate_third, selLepton_third->pt_, selLepton_third->eta_);
                //
                //         bool passesTight_lead = isMatched(*selLepton_lead, tightElectrons) || isMatched(*selLepton_lead, tightMuons);
                //         bool passesTight_sublead = isMatched(*selLepton_sublead, tightElectrons) || isMatched(*selLepton_sublead, tightMuons);
                //         bool passesTight_third = isMatched(*selLepton_third, tightElectrons) || isMatched(*selLepton_third, tightMuons);
                //
                //         double p1 = prob_fake_lead/(1. - prob_fake_lead);
                //         double p2 = prob_fake_sublead/(1. - prob_fake_sublead);
                //         double p3 = prob_fake_third/(1. - prob_fake_third);
                //         double evtWeight_tight_to_loose = 0.;
                //         if      ( !passesTight_lead &&  passesTight_sublead &&  passesTight_third ) evtWeight_tight_to_loose =  p1;
                //         else if (  passesTight_lead && !passesTight_sublead &&  passesTight_third ) evtWeight_tight_to_loose =  p2;
                //         else if (  passesTight_lead &&  passesTight_sublead && !passesTight_third ) evtWeight_tight_to_loose =  p3;
                //         else if ( !passesTight_lead && !passesTight_sublead &&  passesTight_third ) evtWeight_tight_to_loose = -p1*p2;
                //         else if ( !passesTight_lead &&  passesTight_sublead && !passesTight_third ) evtWeight_tight_to_loose = -p1*p3;
                //         else if (  passesTight_lead && !passesTight_sublead && !passesTight_third ) evtWeight_tight_to_loose = -p2*p3;
                //         else if ( !passesTight_lead && !passesTight_sublead && !passesTight_third ) evtWeight_tight_to_loose =  p1*p2*p3;
                //         evtWeight *= evtWeight_tight_to_loose;
                // }

                //--- fill histograms with events passing final selection
                selMuonHistManager.fillHistograms(selMuons, evtWeight);
                selJetHistManager.fillHistograms(selJets, evtWeight);
                selJetHistManager_lead.fillHistograms(selJets, evtWeight);
                selJetHistManager_sublead.fillHistograms(selJets, evtWeight);
                selBJet_mediumHistManager.fillHistograms(selBJets_medium, evtWeight);
                selEvtHistManager.fillHistograms(selMuons.size(),
                                                 selJets.size(),
                                                 selBJets_medium.size(),
                                                 massOfOppositeChargeMuons,
                                                 evtWeight);



                // category A criterias
                // ====================
                //
                // 1. two opposite sign muons with pT > 25 GeV, |η| < 2.1 with tight muon identification and loose tracker isolation

                bool hasTwoMuonsWithPtOver25 = selMuon_lead->pt_ > 25 && selMuon_sublead->pt_ > 25;
                bool hasTwoMuonsWithAbsValueOfEtaSmallerThan21 = abs(selMuon_lead->eta_) < 2.1 && abs(selMuon_sublead->eta_) < 2.1;
                bool hasCategoryACriteria1Passed = hasTwoMuonsWithPtOver25 && hasTwoMuonsWithAbsValueOfEtaSmallerThan21;


                // 2. one b–tagged jet pT > 30 GeV, |η| < 2.4 and no other jets with pT > 30 GeV, |η| < 2.4. Jet is tagged with CSV MVA algorithm and is required to have the b–tagging discriminator value greater that 0.783;

                bool bTaggedJetWithPtOver30AndEtaLessThan24Count = 0;
                for (unsigned int i = 0; i < selBJets_medium.size(); i++) {
                        const RecoJet* bJet = selBJets_medium.at(i);
                        if (bJet->pt_ > 30 && bJet->eta_ < 2.4) {
                                  bTaggedJetWithPtOver30AndEtaLessThan24Count++;
                        }
                }
                bool hasCategoryACriteria2Passed = bTaggedJetWithPtOver30AndEtaLessThan24Count == 1;


                // 3. at least one jet pT > 30 GeV, |η| > 2.4;

                bool jetCountWithPtOver30AndEtaBigger24Count = 0;
                for (unsigned int i = 0; i < selJets.size(); i++) {
                        const RecoJet* selJet = selJets.at(i);
                        if (selJet->pt_ > 30 && selJet->eta_ > 2.4) {
                                  jetCountWithPtOver30AndEtaBigger24Count++;
                        }
                }
                bool hasCategoryACriteria3Passed = jetCountWithPtOver30AndEtaBigger24Count > 0;


                // Is it category A event?

                bool isCategoryAEvent = hasCategoryACriteria1Passed && hasCategoryACriteria2Passed && hasCategoryACriteria3Passed;

                if (hasTwoMuonsWithPtOver25) {
                        cutFlowTable.update("hasTwoMuonsWithPtOver25", evtWeight);
                }

                if (hasTwoMuonsWithAbsValueOfEtaSmallerThan21) {
                        cutFlowTable.update("hasTwoMuonsWithAbsValueOfEtaSmallerThan21", evtWeight);
                }

                if (hasCategoryACriteria1Passed) {
                        cutFlowTable.update("hasCategoryACriteria1Passed", evtWeight);
                }

                if (hasCategoryACriteria2Passed) {
                        cutFlowTable.update("hasCategoryACriteria2Passed", evtWeight);
                }

                if (hasCategoryACriteria3Passed) {
                        cutFlowTable.update("hasCategoryACriteria3Passed", evtWeight);
                }

                if (isCategoryAEvent) {
                        categoryAHistManager.fillHistograms(massOfOppositeChargeMuons, evtWeight);
                        cutFlowTable.update("isCategoryAEvent", evtWeight);
                }


                // category B criterias
                // ====================
                //

                // 1. two opposite sign muons with pT > 25 GeV, |η| < 2.1 with tight muon identification and loose tracker isolation;

                bool hasCategoryBCriteria1Passed = hasCategoryACriteria1Passed;


                // 2. two jets with pT > 30 GeV, |η| < 2.4 with at least one b–tagged jet. Jet tagging criteria are the same as for the first excess observation;

                int jetsWithPtOver30AndEtaLessThan24Count = 0;
                for (unsigned int i = 0; i < selJets.size(); i++) {
                        const RecoJet* jet = selJets.at(i);
                        if (jet->pt_ > 30 && jet->eta_ < 2.4) {
                                  jetsWithPtOver30AndEtaLessThan24Count++;
                        }
                }
                bool hasCategoryBCriteria2Passed = (jetsWithPtOver30AndEtaLessThan24Count >= 2) && (bTaggedJetWithPtOver30AndEtaLessThan24Count >= 1);


                // 3. no jets with pT > 30 GeV, |η| > 2.4;

                bool hasCategoryBCriteria3Passed = jetCountWithPtOver30AndEtaBigger24Count == 0;


                // 4. missing ET < 40 GeV (against t ̄t background);

                bool hasCategoryBCriteria4Passed = met_pt < 40.0;


                // 5. di–muon and di–jet system are required to be back–to–back in the transverse plane of the detector, ∆φ(μμ − jj) > 2.5 (against t ̄t background);

                LV diMuonP4 = selMuon_lead->p4_ + selMuon_sublead->p4_;
                LV diJetP4 = selJet_lead->p4_ + selJet_sublead->p4_;
                double dPhi = reco::deltaPhi(diMuonP4.phi(), diJetP4.phi());

                bool hasCategoryBCriteria5Passed = dPhi > 2.5;


                // Is it category B event?

                bool isCategoryBEvent = hasCategoryBCriteria1Passed && hasCategoryBCriteria2Passed && hasCategoryBCriteria3Passed && hasCategoryBCriteria4Passed && hasCategoryBCriteria5Passed;
                bool isCategoryBEventWithoutCriteria5 = hasCategoryBCriteria1Passed && hasCategoryBCriteria2Passed && hasCategoryBCriteria3Passed && hasCategoryBCriteria4Passed;

                if (hasCategoryBCriteria1Passed) {
                        cutFlowTable.update("hasCategoryBCriteria1Passed", evtWeight);
                }

                if (hasCategoryBCriteria2Passed) {
                        cutFlowTable.update("hasCategoryBCriteria2Passed", evtWeight);
                }

                if (hasCategoryBCriteria3Passed) {
                        cutFlowTable.update("hasCategoryBCriteria3Passed", evtWeight);
                }

                if (hasCategoryBCriteria4Passed) {
                        cutFlowTable.update("hasCategoryBCriteria4Passed", evtWeight);
                }

                if (hasCategoryBCriteria5Passed) {
                        cutFlowTable.update("hasCategoryBCriteria5Passed", evtWeight);
                }

                if (isCategoryBEvent) {
                        categoryBHistManager.fillHistograms(massOfOppositeChargeMuons, evtWeight);
                        cutFlowTable.update("isCategoryBEvent", evtWeight);
                }

                if (isCategoryBEventWithoutCriteria5) {
                        categoryBHistWithoutCriteria5Manager.fillHistograms(massOfOppositeChargeMuons, evtWeight);
                        cutFlowTable.update("isCategoryBEventWithoutCriteria5", evtWeight);
                }

                cutFlowTable.update("control check (id: 005)", evtWeight);


                // if ( isSignal ) {
                //         for ( const auto & kv : decayMode_idString ) {
                //                 if ( std::fabs(genHiggsDecayMode - kv.second) < EPS ) {
                //                         selEvtHistManager_decayMode[kv.first]->fillHistograms(selElectrons.size(), selMuons.size(), selHadTaus.size(),
                //                                                                               selJets.size(), selBJets_loose.size(), selBJets_medium.size(),
                //                                                                               mvaOutput_3l_ttV, mvaOutput_3l_ttbar, mvaDiscr_3l,
                //                                                                               mTauTauVis1_presel, mTauTauVis2_presel, evtWeight);
                //                         break;
                //                 }
                //         }
                // }

                // if(writeSelEventsFile)
                // {
                //         // KE: unique merge loose and medium B-jets, and hadronic jets
                //         std::sort(selBJets_medium.begin(), selBJets_medium.end(), isHigherPt);
                //         std::sort(selBJets_loose.begin(), selBJets_loose.end(), isHigherPt);
                //         std::sort(selJets.begin(), selJets.end(), isHigherCSV);   // optional: sort by pT
                //         std::vector<const RecoJet *> selBJetsMerged(selBJets_medium);
                //         auto unique_push_back = [&selBJetsMerged](const std::vector<const RecoJet *> &v)->void
                //         {
                //                 for(const RecoJet * j : v)
                //                         if(std::find(selBJetsMerged.begin(), selBJetsMerged.end(), j) == selBJetsMerged.end())
                //                                 selBJetsMerged.push_back(j);
                //         };
                //         unique_push_back(selBJets_loose);
                //         unique_push_back(selJets);
                //         if(selBJetsMerged.size() < 2)
                //         {
                //                 std::cerr << "Error: merged b-jets contains less than two jets\n";
                //                 assert(0);
                //         }
                //
                //         eventSpecificsOut.run  = run;
                //         eventSpecificsOut.lumi = lumi;
                //         eventSpecificsOut.evt  = event;
                //
                //         eventSpecificsOut.met_pt  = met_pt;
                //         eventSpecificsOut.met_phi = met_phi;
                //
                //         for(std::size_t i = 0; i < 3; ++i)
                //                 leptonsOut[i].setValues(selLeptons[i]);
                //         for(std::size_t i = 0; i < 2; ++i)
                //                 jetsOut[i].setValues(selBJetsMerged[i]);
                //         htauOut.setValues(selHadTau_lead);
                //         MVAOut.setValues(mvaInputs, mvaOutput_3l_ttV, mvaOutput_3l_ttbar);
                //
                //         selEventsTTree->Fill();
                // }

                (*selEventsFile) << run << ":" << lumi << ":" << event << std::endl;

                ++selectedEntries;
                selectedEntries_weighted += evtWeight;
        }

        std::cout << "num. Entries = " << numEntries << std::endl;
        std::cout << " analyzed = " << analyzedEntries << std::endl;
        std::cout << " selected = " << selectedEntries << " (weighted = " << selectedEntries_weighted << ")" << std::endl;


        std::cout << "cut-flow table" << std::endl;
        cutFlowTable.print(std::cout);
        std::cout << std::endl;

        // if(writeSelEventsFile)
        // {
        //         selEventsTFile->Write();
        //         delete selEventsTFile;
        // }

        delete run_lumi_eventSelector;

        delete selEventsFile;

        delete muonReader;
        delete jetReader;
        // delete genLeptonReader;
        // delete genJetReader;

        hltPaths_delete(triggers_1mu);
        hltPaths_delete(triggers_2mu);

        clock.Show("analyze_2mu1b1j");

        return EXIT_SUCCESS;
}
