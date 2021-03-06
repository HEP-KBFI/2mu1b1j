
#include "FWCore/ParameterSet/interface/ParameterSet.h"                                        // edm::ParameterSet
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"                             // edm::readPSetsFrom()
#include "FWCore/ParameterSet/interface/FileInPath.h"                                          // edm::FileInPath
#include "FWCore/Utilities/interface/Exception.h"                                              // cms::Exception
#include "PhysicsTools/FWLite/interface/TFileService.h"                                        // fwlite::TFileService
#include "DataFormats/FWLite/interface/InputSource.h"                                          // fwlite::InputSource
#include "DataFormats/FWLite/interface/OutputFiles.h"                                          // fwlite::OutputFiles
#include "DataFormats/Math/interface/LorentzVector.h"                                          // math::PtEtaPhiMLorentzVector
#include "DataFormats/Math/interface/deltaR.h"                                                 // deltaR

#include <Rtypes.h>                                                                            // Int_t, Long64_t, Double_t
#include <TChain.h>                                                                            // TChain
#include <TTree.h>                                                                             // TTree
#include <TBenchmark.h>                                                                        // TBenchmark
#include <TString.h>                                                                           // TString, Form

#include "tthAnalysis/HiggsToTauTau/interface/RecoLepton.h"                                    // RecoLepton
#include "tthAnalysis/HiggsToTauTau/interface/RecoJet.h"                                       // RecoJet
#include "tthAnalysis/HiggsToTauTau/interface/GenLepton.h"                                     // GenLepton
#include "tthAnalysis/HiggsToTauTau/interface/GenJet.h"                                        // GenJet
#include "tthAnalysis/HiggsToTauTau/interface/KeyTypes.h"
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonReader.h"                                // RecoMuonReader
#include "tthAnalysis/HiggsToTauTau/interface/RecoJetReader.h"                                 // RecoJetReader
#include "tthAnalysis/HiggsToTauTau/interface/GenLeptonReader.h"                               // GenLeptonReader
#include "tthAnalysis/HiggsToTauTau/interface/GenJetReader.h"                                  // GenJetReader
#include "tthAnalysis/HiggsToTauTau/interface/convert_to_ptrs.h"                               // convert_to_ptrs
#include "tthAnalysis/HiggsToTauTau/interface/ParticleCollectionCleaner.h"                     // RecoElectronCollectionCleaner, RecoMuonCollectionCleaner, RecoHadTauCollectionCleaner, RecoJetCollectionCleaner
#include "tthAnalysis/HiggsToTauTau/interface/ParticleCollectionGenMatcher.h"                  // RecoElectronCollectionGenMatcher, RecoMuonCollectionGenMatcher, RecoHadTauCollectionGenMatcher, RecoJetCollectionGenMatcher
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonCollectionSelectorLoose.h"               // RecoMuonCollectionSelectorLoose
#include "tthAnalysis/HiggsToTauTau/interface/RecoMuonCollectionSelectorFakeable.h"            // RecoMuonCollectionSelectorFakeable
#include "analysis2mu1b1j/analysis2mu1b1j/interface/RecoMuonCollectionSelectorTight_2mu1b1j.h" // RecoMuonCollectionSelectorTight
#include "analysis2mu1b1j/analysis2mu1b1j/interface/RecoJetCollectionSelector_2mu1b1j.h"       // RecoJetCollectionSelector
#include "analysis2mu1b1j/analysis2mu1b1j/interface/RecoJetCollectionSelectorBtag_2mu1b1j.h"   // RecoJetCollectionSelectorBtagLoose, RecoJetCollectionSelectorBtagMedium
#include "tthAnalysis/HiggsToTauTau/interface/RunLumiEventSelector.h"                          // RunLumiEventSelector
#include "tthAnalysis/HiggsToTauTau/interface/MuonHistManager.h"                               // MuonHistManager
#include "tthAnalysis/HiggsToTauTau/interface/JetHistManager.h"                                // JetHistManager
#include "tthAnalysis/HiggsToTauTau/interface/MEtHistManager.h"                                // MEtHistManager
#include "analysis2mu1b1j/analysis2mu1b1j/interface/EvtHistManager_2mu1b1j.h"                  // EvtHistManager_2mu1b1j
#include "analysis2mu1b1j/analysis2mu1b1j/interface/EvtHistManager_2mu1b1jCategory.h"          // EvtHistManager_2mu1b1j
#include "tthAnalysis/HiggsToTauTau/interface/leptonTypes.h"                                   // getLeptonType, kElectron, kMuon
#include "tthAnalysis/HiggsToTauTau/interface/backgroundEstimation.h"                          // prob_chargeMisId
#include "tthAnalysis/HiggsToTauTau/interface/hltPath.h"                                       // hltPath, create_hltPaths, hltPaths_setBranchAddresses, hltPaths_isTriggered, hltPaths_delete
#include "tthAnalysis/HiggsToTauTau/interface/data_to_MC_corrections.h"
#include "tthAnalysis/HiggsToTauTau/interface/lutAuxFunctions.h"                               // loadTH2, get_sf_from_TH2
#include "tthAnalysis/HiggsToTauTau/interface/cutFlowTable.h"                                  // cutFlowTableType
#include "tthAnalysis/HiggsToTauTau/interface/analysisAuxFunctions.h"                          // kEra_2015, kEra_2016

#include <boost/range/algorithm/copy.hpp>                                                      // boost::copy()
#include <boost/range/adaptor/map.hpp>                                                         // boost::adaptors::map_keys

#include <iostream>                                                                            // std::cerr, std::fixed
#include <iomanip>                                                                             // std::setprecision(), std::setw()
#include <string>                                                                              // std::string
#include <vector>                                                                              // std::vector<>
#include <cstdlib>                                                                             // EXIT_SUCCESS, EXIT_FAILURE
#include <algorithm>                                                                           // std::sort(), std::find()
#include <fstream>                                                                             // std::ofstream
#include <assert.h>                                                                            // assert
#include <array>                                                                               // std::array<>

#define EPS 1E-2

typedef math::PtEtaPhiMLorentzVector LV;
typedef std::vector<std::string>     vstring;


/**
 * @brief Auxiliary function used for sorting leptons by decreasing pT
 * @param Given pair of leptons
 * @return True, if first lepton has higher pT; false if second lepton has higher pT
 */
bool isHigherPt(const GenParticle *particle1, const GenParticle *particle2)
{
  return particle1->pt() > particle2->pt();
}

/**
 * @brief Auxiliary function used for sorting jets by decreasing btagging CSV value
 * @param Given pair of jets
 * @return True, if first jet has higher CSV; false if second jet has higher CSV
 */
bool isHigherCSV(const RecoJet *jet1, const RecoJet *jet2)
{
  return jet1->BtagCSV() > jet2->BtagCSV();
}

/**
 * @brief Produce datacard and control plots for 2mu1b1j categories.
 */
int main(int argc, char *argv[])
{
  // --- parse command-line arguments
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return EXIT_SUCCESS;
  }

  std::cout << "<analyze_2mu1b1j>:" << std::endl;

  // --- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("analyze_2mu1b1j");

  // --- read python configuration parameters
  if (!edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process")) throw
      cms::Exception("analyze_2mu1b1j")
      << "No ParameterSet 'process' found in configuration file = " <<
      argv[1] << " !!\n";

  edm::ParameterSet cfg =
    edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfg_analyze = cfg.getParameter<edm::ParameterSet>("analyze_2mu1b1j");

  std::string treeName = cfg_analyze.getParameter<std::string>("treeName");

  std::string process_string = cfg_analyze.getParameter<std::string>("process");

  // bool isSignal = ( process_string == "signal" ) ? true : false;

  vstring triggerNames_1mu            = cfg_analyze.getParameter<vstring>("triggers_1mu");
  std::vector<hltPath *> triggers_1mu = create_hltPaths(triggerNames_1mu);
  bool use_triggers_1mu               = cfg_analyze.getParameter<bool>("use_triggers_1mu");

  vstring triggerNames_2mu            = cfg_analyze.getParameter<vstring>("triggers_2mu");
  std::vector<hltPath *> triggers_2mu = create_hltPaths(triggerNames_2mu);
  bool use_triggers_2mu               = cfg_analyze.getParameter<bool>("use_triggers_2mu");

  enum { kLoose, kFakeable, kTight };
  std::string leptonSelection_string = "Tight";
  int leptonSelection                = kTight;


  bool isMC                    = cfg_analyze.getParameter<bool>("isMC");
  std::string central_or_shift = cfg_analyze.getParameter<std::string>("central_or_shift");

  // double lumiScale = ( process_string != "data_obs" ) ? cfg_analyze.getParameter<double>("lumiScale") : 1.;

  std::string jet_btagWeight_branch = (isMC) ? "Jet_bTagWeight" : "";

  int jetPt_option = RecoJetReader::kJetPt_central;

  std::string era_string = cfg_analyze.getParameter<std::string>("era");
  int era                = -1;

  if      (era_string == "2015") era = kEra_2015;
  else if (era_string == "2016") era = kEra_2016;
  else throw cms::Exception("analyze_2mu1b1j")
          << "Invalid Configuration parameter 'era' = " << era_string << " !!\n";


  RunLumiEventSelector *run_lumi_eventSelector = 0;


  fwlite::InputSource inputFiles(cfg);
  int maxEvents = inputFiles.maxEvents();
  std::cout << " maxEvents = " << maxEvents << std::endl;
  unsigned reportEvery = inputFiles.reportAfter();

  fwlite::OutputFiles  outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  // A chain is a collection of files containg TTree objects.
  // https://root.cern.ch/doc/master/classTChain.html

  TChain *inputTree = new TChain(treeName.data());

  for (std::vector<std::string>::const_iterator inputFileName = inputFiles.files().begin();
       inputFileName != inputFiles.files().end(); ++inputFileName) {
    std::cout << "input Tree: adding file = " << (*inputFileName) << std::endl;
    inputTree->AddFile(inputFileName->data());
  }

  // if there are no entries, throw exception (why not just "== 0"?)

  if (inputTree->GetListOfFiles()->GetEntries() == 1) {
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
  inputTree->SetBranchAddress(RUN_KEY,  &run);
  LUMI_TYPE lumi;
  inputTree->SetBranchAddress(LUMI_KEY, &lumi);
  EVT_TYPE event;
  inputTree->SetBranchAddress(EVT_KEY,  &event);

  hltPaths_setBranchAddresses(inputTree, triggers_1mu);
  hltPaths_setBranchAddresses(inputTree, triggers_2mu);

  MET_PT_TYPE met_pt;
  inputTree->SetBranchAddress(MET_PT_KEY,  &met_pt);
  MET_ETA_TYPE met_eta;
  inputTree->SetBranchAddress(MET_ETA_KEY, &met_eta);
  MET_PHI_TYPE met_phi;
  inputTree->SetBranchAddress(MET_PHI_KEY, &met_phi);


  // # Declare particle collections

  RecoMuonReader *muonReader =
    new RecoMuonReader(era, "nselLeptons", "selLeptons");
  muonReader->setBranchAddresses(inputTree);

  // RecoMuonCollectionGenMatcher muonGenMatcher;
  RecoMuonCollectionSelectorLoose preselMuonSelector(era);
  RecoMuonCollectionSelectorFakeable fakeableMuonSelector(era);
  RecoMuonCollectionSelectorTight_2mu1b1j tightMuonSelector(era,
                                                            -1,
                                                            run_lumi_eventSelector !=
                                                            0);

  RecoJetReader *jetReader = new RecoJetReader(era, false, "nJet", "Jet");
  jetReader->setJetPt_central_or_shift(jetPt_option);
  jetReader->setBranchName_BtagWeight(jet_btagWeight_branch);
  jetReader->setBranchAddresses(inputTree);
  RecoJetCollectionGenMatcher jetGenMatcher;
  RecoJetCollectionCleaner    jetCleaner(0.5);
  RecoJetCollectionSelector_2mu1b1j jetSelector(era);
  RecoJetCollectionSelectorBtagLoose_2mu1b1j  jetSelectorBtagLoose(era);
  RecoJetCollectionSelectorBtagMedium_2mu1b1j jetSelectorBtagMedium(era);


  // --- open output file containing run:lumi:event numbers of events passing final event selection criteria
  std::string selEventsFileName_output = cfg_analyze.getParameter<std::string>(
    "selEventsFileName_output");
  std::ostream *selEventsFile = new std::ofstream(
    selEventsFileName_output.data(),
    std::ios::out);


  // categoryAHistManager

  EvtHistManager_2mu1b1jCategory categoryAHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryA_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryAHistManager.bookHistograms(fs);


  // categoryACompareHistManager

  EvtHistManager_2mu1b1jCategory categoryACompareHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryACompare_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryACompareHistManager.bookHistograms(fs);


  // categoryARelaxedHistManager

  EvtHistManager_2mu1b1jCategory categoryARelaxedHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryARelaxed_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryARelaxedHistManager.bookHistograms(fs);


  // categoryACompareRelaxedHistManager

  EvtHistManager_2mu1b1jCategory categoryACompareRelaxedHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryACompareRelaxed_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryACompareRelaxedHistManager.bookHistograms(fs);


  // categoryBHistManager

  EvtHistManager_2mu1b1jCategory categoryBHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryB_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryBHistManager.bookHistograms(fs);


  // categoryBCompareHistManager

  EvtHistManager_2mu1b1jCategory categoryBCompareHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryBCompare_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryBCompareHistManager.bookHistograms(fs);


  // categoryBRelaxedHistManager

  EvtHistManager_2mu1b1jCategory categoryBRelaxedHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryBRelaxed_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryBRelaxedHistManager.bookHistograms(fs);


  // categoryBCompareRelaxedHistManager

  EvtHistManager_2mu1b1jCategory categoryBCompareRelaxedHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryBCompareRelaxed_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryBCompareRelaxedHistManager.bookHistograms(fs);


  // categoryCHistManager

  EvtHistManager_2mu1b1jCategory categoryCHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryC_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryCHistManager.bookHistograms(fs);


  // categoryCCompareHistManager

  EvtHistManager_2mu1b1jCategory categoryCCompareHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryCCompare_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryCCompareHistManager.bookHistograms(fs);


  // categoryCRelaxedHistManager

  EvtHistManager_2mu1b1jCategory categoryCRelaxedHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryCRelaxed_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryCRelaxedHistManager.bookHistograms(fs);


  // categoryCCompareRelaxedHistManager

  EvtHistManager_2mu1b1jCategory categoryCCompareRelaxedHistManager(
    makeHistManager_cfg(
      process_string,
      Form("2mu1b1jCategoryCCompareRelaxed_%s/sel/evt",
           leptonSelection_string.data()),
      central_or_shift)
    );
  categoryCCompareRelaxedHistManager.bookHistograms(fs);


  int numEntries                  = inputTree->GetEntries();
  int analyzedEntries             = 0;
  int selectedEntries             = 0;
  double selectedEntries_weighted = 0.;
  cutFlowTableType cutFlowTable;

  // loop over all the events in inputTree

  for (int idxEntry = 0;
       idxEntry < numEntries && (maxEvents == -1 || idxEntry < maxEvents);
       ++idxEntry) {
    if ((idxEntry > 0) && ((idxEntry % reportEvery) == 0)) {
      std::cout << "processing Entry " << idxEntry << " (" << selectedEntries << " Entries selected)" << std::endl;
    }
    ++analyzedEntries;

    // load entry with ID

    inputTree->GetEntry(idxEntry);

    // check if run number matches any of the runs containing events to be selected

    if (run_lumi_eventSelector &&
        !(*run_lumi_eventSelector)(run, lumi, event)) continue;
    cutFlowTable.update("run:ls:event selection");

    bool isTriggered_1mu = hltPaths_isTriggered(triggers_1mu);
    bool isTriggered_2mu = hltPaths_isTriggered(triggers_2mu);

    bool selTrigger_1mu = use_triggers_1mu && isTriggered_1mu;
    bool selTrigger_2mu = use_triggers_2mu && isTriggered_2mu;

    if ((!selTrigger_1mu) && (!selTrigger_2mu)) {
      if (run_lumi_eventSelector) {
        std::cout << "event FAILS trigger selection." << std::endl;
        std::cout << ", selTrigger_1mu = " << selTrigger_1mu
                  << ", selTrigger_2mu = " << selTrigger_2mu;
      }
      continue;
    }

    // --- rank triggers by priority and ignore triggers of lower priority if a trigger of higher priority has fired for given event;
    //    the ranking of the triggers is as follows: 2mu, 1e1mu, 2e, 1mu, 1e
    // CV: this logic is necessary to avoid that the same event is selected multiple times when processing different primary datasets
    if (!isMC) {
      if (selTrigger_1mu && isTriggered_2mu) {
        if (run_lumi_eventSelector) {
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

    std::vector<RecoMuon> muons                 = muonReader->read();
    std::vector<const RecoMuon *> muon_ptrs     = convert_to_ptrs(muons);
    std::vector<const RecoMuon *> cleanedMuons  = muon_ptrs; // CV: no cleaning needed for muons, as they have the highest priority in the overlap removal
    std::vector<const RecoMuon *> preselMuons   = preselMuonSelector(cleanedMuons);
    std::vector<const RecoMuon *> fakeableMuons = fakeableMuonSelector(preselMuons);
    std::vector<const RecoMuon *> tightMuons    = tightMuonSelector(preselMuons);

    // setting selMuons tightMuons, because leptonSelection == 'tight'

    std::vector<const RecoMuon *> selMuons;

    if      (leptonSelection == kLoose) selMuons = preselMuons;
    else if (leptonSelection == kFakeable) selMuons = fakeableMuons;
    else if (leptonSelection == kTight) selMuons = tightMuons;
    else assert(0);


    // --- build collections of jets and select subset of jets passing b-tagging criteria
    std::vector<RecoJet> jets                    = jetReader->read();
    std::vector<const RecoJet *> jet_ptrs        = convert_to_ptrs(jets);
    std::vector<const RecoJet *> cleanedJets     = jetCleaner(jet_ptrs, selMuons);
    std::vector<const RecoJet *> selJets         = jetSelector(cleanedJets);
    std::vector<const RecoJet *> selBJets_loose  = jetSelectorBtagLoose(cleanedJets);
    std::vector<const RecoJet *> selBJets_medium = jetSelectorBtagMedium(cleanedJets);

    // --- build collections of generator level particles
    std::vector<GenLepton> genMuons;
    std::vector<GenJet>    genJets;


    // --- apply preselection
    std::sort(preselMuons.begin(), preselMuons.end(), isHigherPt);

    // require exactly two leptons passing loose preselection criteria to avoid overlap with 3l category
    if (preselMuons.size() != 2) {
      if (run_lumi_eventSelector) {
        std::cout << "event FAILS preselMuons selection." << std::endl;
        std::cout << " (#preselMuons = " << preselMuons.size() << ")" << std::endl;

        for (size_t idxPreselMuon = 0; idxPreselMuon < preselMuons.size(); ++idxPreselMuon) {
          std::cout << "preselMuons #" << idxPreselMuon << ":" << std::endl;
          std::cout << (*preselMuons[idxPreselMuon]);
        }
      }
      continue;
    }
    cutFlowTable.update("2 presel muons");

    const RecoMuon *preselMuon_lead    = preselMuons[0];
    const RecoMuon *preselMuon_sublead = preselMuons[1];

    // apply requirement on jets (incl. b-tagged jets)

    std::sort(selJets.begin(), selJets.end(), isHigherPt);

    if (selJets.size() < 2) {
      if (run_lumi_eventSelector) {
        std::cout << "event FAILS selJets selection." << std::endl;
        std::cout << " (#selJets = " << selJets.size() << ")" << std::endl;
      }
      continue;
    }
    cutFlowTable.update(">= 2 jets");

    const RecoJet *selJet_lead    = selJets[0];
    const RecoJet *selJet_sublead = selJets[1];


    // --- compute event-level weight for data/MC correction of b-tagging efficiency and mistag rate
    //   (using the method "Event reweighting using scale factors calculated with a tag and probe method",
    //    described on the BTV POG twiki https://twiki.cern.ch/twiki/bin/view/CMS/BTagShapeCalibration )
    double evtWeight = 1.;


    // check that muons have opposite charge
    if (!((preselMuon_lead->charge() + preselMuon_sublead->charge()) == 0.0)) {
      if (run_lumi_eventSelector) {
        std::cout << "event FAILS muons opposite charge criteria." << std::endl;
      }
      continue;
    }
    cutFlowTable.update("muons opposite charge criteria");


    double massOfOppositeChargeMuons   = (preselMuon_lead->p4() + preselMuon_sublead->p4()).mass();
    double deltaROfOppositeChargeMuons = deltaR(preselMuon_lead->p4(), preselMuon_sublead->p4());
    double ptOfOppositeChargeMuons     = (preselMuon_lead->p4() + preselMuon_sublead->p4()).pt();


    // --- apply final event selection

    // require exactly two leptons passing tight selection criteria of final event selection
    if (!(selMuons.size() == 2)) {
      if (run_lumi_eventSelector) {
        std::cout << "event FAILS selMuons selection." << std::endl;
        std::cout << " (#selMuons = " << selMuons.size() << ")" << std::endl;

        for (size_t idxSelMuon = 0; idxSelMuon < selMuons.size(); ++idxSelMuon) {
          std::cout << "selMuon #" << idxSelMuon << ":" << std::endl;
          std::cout << (*selMuons[idxSelMuon]);
        }
      }
      continue;
    }
    cutFlowTable.update("2 sel muons", evtWeight);

    const RecoMuon *selMuon_lead    = selMuons[0];
    const RecoMuon *selMuon_sublead = selMuons[1];


    // apply requirement on jets (incl. b-tagged jets)
    if (!(selJets.size() >= 2)) {
      if (run_lumi_eventSelector) {
        std::cout << "event FAILS selJets selection." << std::endl;
        std::cout << " (#selJets = " << selJets.size() << ")" << std::endl;

        for (size_t idxSelJet = 0; idxSelJet < selJets.size(); ++idxSelJet) {
          std::cout << "selJet #" << idxSelJet << ":" << std::endl;
          std::cout << (*selJets[idxSelJet]);
        }
      }
      continue;
    }
    cutFlowTable.update(">= 2 jets (2)", evtWeight);


    // muon pt cuts

    if (!((selMuon_lead->pt() > 20.) && (selMuon_sublead->pt() > 10.))) {
      if (run_lumi_eventSelector) {
        std::cout << "event FAILS muon pT selection." << std::endl;
        std::cout << " (leading selMuon pT = " << selMuon_lead->pt()
                  << ", subleading selMuon pT = " << selMuon_sublead->pt()
                  << std::endl;
      }
      continue;
    }
    cutFlowTable.update("has first muon with pt over 20, second over 10", evtWeight);


    // check that muon and antimuon

    bool hasTwoOppositeSignMuons = (selMuon_lead->charge() + selMuon_sublead->charge()) == 0;

    if (!hasTwoOppositeSignMuons) {
      if (run_lumi_eventSelector) {
        std::cout << "event FAILS muon charge selection." << std::endl;
        std::cout << " (leading selMuon charge = " << selMuon_lead->charge()
                  << ", subleading selMuon charge = " << selMuon_sublead->charge()
                  << ")" << std::endl;
      }
      continue;
    }
    cutFlowTable.update("has muon and antimuon");


    // populate needed counters

    int bTaggedJetWithPtOver30AndEtaLessThan24Count = 0;

    for (unsigned int i = 0; i < selBJets_medium.size(); i++) {
      const RecoJet *bJet = selBJets_medium.at(i);

      if ((bJet->pt() > 30) && (abs(bJet->eta()) < 2.4)) {
        bTaggedJetWithPtOver30AndEtaLessThan24Count++;
      }
    }


    int jetsWithPtOver30AndEtaLessThan24Count = 0;

    for (unsigned int i = 0; i < selJets.size(); i++) {
      const RecoJet *jet = selJets.at(i);

      if ((jet->pt() > 30) && (abs(jet->eta()) < 2.4)) {
        jetsWithPtOver30AndEtaLessThan24Count++;
      }
    }


    int jetsWithPtOver30AndEtaBigger24Count = 0;

    for (unsigned int i = 0; i < selJets.size(); i++) {
      const RecoJet *selJet = selJets.at(i);

      if ((selJet->pt() > 30) && (abs(selJet->eta()) > 2.4)) {
        jetsWithPtOver30AndEtaBigger24Count++;
      }
    }


    // category A criterias
    // ====================
    //
    // 1. two opposite sign muons with pT > 25 GeV, |η| < 2.1 with tight muon identification and loose tracker isolation

    bool hasTwoMuonsWithPtOver25 = selMuon_lead->pt() > 25 &&
                                   selMuon_sublead->pt() > 25;

    bool hasFirstMuonPtOver20SecondMuonPtOver10 = selMuon_lead->pt() > 20 &&
                                                  selMuon_sublead->pt() > 10;

    bool hasTwoMuonsWithAbsValueOfEtaSmallerThan21 = abs(selMuon_lead->eta()) < 2.1 && abs(selMuon_sublead->eta()) <
                                                     2.1;

    bool hasCategoryACriteria1Passed = hasTwoMuonsWithPtOver25 &&
                                       hasTwoMuonsWithAbsValueOfEtaSmallerThan21;

    bool hasCategoryARelaxedCriteria1Passed = hasFirstMuonPtOver20SecondMuonPtOver10 &&
                                              hasTwoMuonsWithAbsValueOfEtaSmallerThan21;


    // 2. one b–tagged jet pT > 30 GeV, |η| < 2.4 and no other jets with pT > 30 GeV, |η| < 2.4. Jet is tagged with CSV MVA algorithm and is required to have the b–tagging discriminator value greater that 0.783;

    bool hasCategoryACriteria2Passed = (bTaggedJetWithPtOver30AndEtaLessThan24Count == 1) &&
                                       (jetsWithPtOver30AndEtaLessThan24Count == 1); // jetsWithPtOver30AndEtaLessThan24Count contains btagged jets


    // 3. at least one jet pT > 30 GeV, |η| > 2.4;

    bool hasCategoryACriteria3Passed = jetsWithPtOver30AndEtaBigger24Count > 0;


    // Is it category A event?

    bool isCategoryAEvent = hasCategoryACriteria1Passed &&
                            hasCategoryACriteria2Passed &&
                            hasCategoryACriteria3Passed;
    bool isCategoryARelaxedEvent = hasCategoryARelaxedCriteria1Passed &&
                                   hasCategoryACriteria2Passed &&
                                   hasCategoryACriteria3Passed;

    if (hasTwoMuonsWithPtOver25) {
      cutFlowTable.update("hasTwoMuonsWithPtOver25", evtWeight);
    }

    if (hasFirstMuonPtOver20SecondMuonPtOver10) {
      cutFlowTable.update("hasFirstMuonPtOver20SecondMuonPtOver10", evtWeight);
    }

    if (hasTwoMuonsWithAbsValueOfEtaSmallerThan21) {
      cutFlowTable.update("hasTwoMuonsWithAbsValueOfEtaSmallerThan21", evtWeight);
    }

    if (hasCategoryACriteria1Passed) {
      cutFlowTable.update("hasCategoryACriteria1Passed", evtWeight);
    }

    if (hasCategoryARelaxedCriteria1Passed) {
      cutFlowTable.update("hasCategoryARelaxedCriteria1Passed", evtWeight);
    }

    if (hasCategoryACriteria2Passed) {
      cutFlowTable.update("hasCategoryACriteria2Passed", evtWeight);
    }

    if (hasCategoryACriteria3Passed) {
      cutFlowTable.update("hasCategoryACriteria3Passed", evtWeight);
    }

    if (isCategoryAEvent) {
      categoryAHistManager.fillHistograms(massOfOppositeChargeMuons,
                                          deltaROfOppositeChargeMuons,
                                          ptOfOppositeChargeMuons,
                                          evtWeight);
      cutFlowTable.update("isCategoryAEvent", evtWeight);
    }

    if (isCategoryARelaxedEvent) {
      categoryARelaxedHistManager.fillHistograms(massOfOppositeChargeMuons,
                                                 deltaROfOppositeChargeMuons,
                                                 ptOfOppositeChargeMuons,
                                                 evtWeight);
      cutFlowTable.update("isCategoryARelaxedEvent", evtWeight);
    }


    // category A compare criterias
    //

    // 0b, 1j barrel, 1j forward

    bool has1JetInBarrel        = jetsWithPtOver30AndEtaLessThan24Count == 1;
    bool hasMoreThan1JetForward = jetsWithPtOver30AndEtaBigger24Count > 1;

    bool isCategoryACompareEvent = hasCategoryACriteria1Passed &&
                                   has1JetInBarrel && hasMoreThan1JetForward;

    if (isCategoryACompareEvent) {
      categoryACompareHistManager.fillHistograms(massOfOppositeChargeMuons,
                                                 deltaROfOppositeChargeMuons,
                                                 ptOfOppositeChargeMuons,
                                                 evtWeight);
      cutFlowTable.update("isCategoryACompareEvent", evtWeight);
    }

    bool isCategoryACompareRelaxedEvent = hasCategoryARelaxedCriteria1Passed &&
                                          has1JetInBarrel && hasMoreThan1JetForward;

    if (isCategoryACompareRelaxedEvent) {
      categoryACompareRelaxedHistManager.fillHistograms(massOfOppositeChargeMuons,
                                                        deltaROfOppositeChargeMuons,
                                                        ptOfOppositeChargeMuons,
                                                        evtWeight);
      cutFlowTable.update("isCategoryACompareRelaxedEvent", evtWeight);
    }


    // category B criterias
    // ====================
    //

    // 1. two opposite sign muons with pT > 25 GeV, |η| < 2.1 with tight muon identification and loose tracker isolation;

    bool hasCategoryBCriteria1Passed        = hasCategoryACriteria1Passed;
    bool hasCategoryBRelaxedCriteria1Passed = hasCategoryARelaxedCriteria1Passed;


    // 2. two jets with pT > 30 GeV, |η| < 2.4 with at least one b–tagged jet. Jet tagging criteria are the same as for the first excess observation;

    bool hasCategoryBCriteria2Passed =
      (jetsWithPtOver30AndEtaLessThan24Count >= 2) &&
      (bTaggedJetWithPtOver30AndEtaLessThan24Count >= 1);


    // 3. no jets with pT > 30 GeV, |η| > 2.4;

    bool hasCategoryBCriteria3Passed = jetsWithPtOver30AndEtaBigger24Count == 0;


    // 4. missing ET < 40 GeV (against t ̄t background);

    bool hasCategoryBCriteria4Passed = met_pt < 40.0;


    // 5. di–muon and di–jet system are required to be back–to–back in the transverse plane of the detector, ∆φ(μμ − jj) > 2.5 (against t ̄t background);

    LV diMuonP4 = selMuon_lead->p4() + selMuon_sublead->p4();
    LV diJetP4  = selJet_lead->p4() + selJet_sublead->p4();
    double dPhi = reco::deltaPhi(diMuonP4.phi(), diJetP4.phi());

    bool hasCategoryBCriteria5Passed = dPhi > 2.5;


    // Is it category B event?

    bool isCategoryBEvent = hasCategoryBCriteria1Passed &&
                            hasCategoryBCriteria2Passed &&
                            hasCategoryBCriteria3Passed &&
                            hasCategoryBCriteria4Passed &&
                            hasCategoryBCriteria5Passed;
    bool isCategoryBRelaxedEvent = hasCategoryBRelaxedCriteria1Passed &&
                                   hasCategoryBCriteria2Passed &&
                                   hasCategoryBCriteria3Passed &&
                                   hasCategoryBCriteria4Passed &&
                                   hasCategoryBCriteria5Passed;

    if (hasCategoryBCriteria1Passed) {
      cutFlowTable.update("hasCategoryBCriteria1Passed", evtWeight);
    }

    if (hasCategoryBRelaxedCriteria1Passed) {
      cutFlowTable.update("hasCategoryBRelaxedCriteria1Passed", evtWeight);
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
      categoryBHistManager.fillHistograms(massOfOppositeChargeMuons,
                                          deltaROfOppositeChargeMuons,
                                          ptOfOppositeChargeMuons,
                                          evtWeight);
      cutFlowTable.update("isCategoryBEvent", evtWeight);
    }

    if (isCategoryBRelaxedEvent) {
      categoryBRelaxedHistManager.fillHistograms(massOfOppositeChargeMuons,
                                                 deltaROfOppositeChargeMuons,
                                                 ptOfOppositeChargeMuons,
                                                 evtWeight);
      cutFlowTable.update("isCategoryBRelaxedEvent", evtWeight);
    }


    // Category B compare criterias
    //

    // 0b, 2j barrel, 0j forward

    bool has2JetsInBarrel  = jetsWithPtOver30AndEtaLessThan24Count == 2;
    bool has0JetsInForward = jetsWithPtOver30AndEtaBigger24Count == 0;

    bool isCategoryBCompareEvent = hasCategoryBCriteria1Passed &&
                                   has2JetsInBarrel && has0JetsInForward;

    if (isCategoryBCompareEvent) {
      categoryBCompareHistManager.fillHistograms(massOfOppositeChargeMuons,
                                                 deltaROfOppositeChargeMuons,
                                                 ptOfOppositeChargeMuons,
                                                 evtWeight);
      cutFlowTable.update("isCategoryBCompareEvent", evtWeight);
    }

    bool isCategoryBRelaxedCompareEvent = hasCategoryBCriteria1Passed &&
                                          has2JetsInBarrel &&
                                          has0JetsInForward;

    if (isCategoryBRelaxedCompareEvent) {
      categoryBCompareRelaxedHistManager.fillHistograms(massOfOppositeChargeMuons,
                                                        deltaROfOppositeChargeMuons,
                                                        ptOfOppositeChargeMuons,
                                                        evtWeight);
      cutFlowTable.update("isCategoryBRelaxedCompareEvent", evtWeight);
    }


    // category C criterias (From Martti proposition)
    // =============================================
    //

    // 1. two opposite sign muons with pT > 25 GeV, |η| < 2.1 with tight muon identification and loose tracker isolation

    bool hasCategoryCCriteria1Passed = hasTwoMuonsWithPtOver25 &&
                                       hasTwoMuonsWithAbsValueOfEtaSmallerThan21;

    bool hasCategoryCRelaxedCriteria1Passed = hasFirstMuonPtOver20SecondMuonPtOver10 &&
                                              hasTwoMuonsWithAbsValueOfEtaSmallerThan21;


    // 2. has one bottom in central region

    bool hasCategoryCCriteria2Passed = bTaggedJetWithPtOver30AndEtaLessThan24Count == 1;


    // 3. no other jets

    bool hasCategoryCCriteria3Passed =
      (jetsWithPtOver30AndEtaBigger24Count + jetsWithPtOver30AndEtaLessThan24Count) == 1;


    // Is it category C event?

    bool isCategoryCEvent = hasCategoryCCriteria1Passed &&
                            hasCategoryCCriteria2Passed &&
                            hasCategoryCCriteria3Passed;

    if (isCategoryCEvent) {
      categoryCHistManager.fillHistograms(massOfOppositeChargeMuons,
                                          deltaROfOppositeChargeMuons,
                                          ptOfOppositeChargeMuons,
                                          evtWeight);
      cutFlowTable.update("isCategoryCEvent", evtWeight);
    }


    // Is it category C relaxed event?

    bool isCategoryCRelaxedEvent = hasCategoryCRelaxedCriteria1Passed &&
                                   hasCategoryCCriteria2Passed &&
                                   hasCategoryCCriteria3Passed;

    if (isCategoryCRelaxedEvent) {
      categoryCRelaxedHistManager.fillHistograms(massOfOppositeChargeMuons,
                                                 deltaROfOppositeChargeMuons,
                                                 ptOfOppositeChargeMuons,
                                                 evtWeight);
      cutFlowTable.update("isCategoryCRelaxedEvent", evtWeight);
    }


    // category C compare criterias
    // ============================
    //

    // 1. two opposite sign muons with pT > 25 GeV, |η| < 2.1 with tight muon identification and loose tracker isolation

    bool hasCategoryCCompareCriteria1Passed = hasTwoMuonsWithPtOver25 &&
                                              hasTwoMuonsWithAbsValueOfEtaSmallerThan21;

    bool hasCategoryCCompareRelaxedCriteria1Passed = hasFirstMuonPtOver20SecondMuonPtOver10 &&
                                                     hasTwoMuonsWithAbsValueOfEtaSmallerThan21;

    // 2. has 1 central jet

    bool hasCategoryCCompareCriteria2Passed = jetsWithPtOver30AndEtaLessThan24Count == 1;


    // 3. has no other jets

    bool hasCategoryCCompareCriteria3Passed = jetsWithPtOver30AndEtaBigger24Count == 0;


    // Is it category C compare event?

    bool isCategoryCCompareEvent = hasCategoryCCompareCriteria1Passed &&
                                   hasCategoryCCompareCriteria2Passed &&
                                   hasCategoryCCompareCriteria3Passed;

    if (isCategoryCCompareEvent) {
      categoryCCompareHistManager.fillHistograms(massOfOppositeChargeMuons,
                                                 deltaROfOppositeChargeMuons,
                                                 ptOfOppositeChargeMuons,
                                                 evtWeight);
      cutFlowTable.update("isCategoryCCompareEvent", evtWeight);
    }


    // Is it category C compare relaxed event?

    bool isCategoryCCompareRelaxedEvent = hasCategoryCCompareRelaxedCriteria1Passed &&
                                          hasCategoryCCompareCriteria2Passed &&
                                          hasCategoryCCompareCriteria3Passed;

    if (isCategoryCCompareRelaxedEvent) {
      categoryCCompareRelaxedHistManager.fillHistograms(massOfOppositeChargeMuons,
                                                        deltaROfOppositeChargeMuons,
                                                        ptOfOppositeChargeMuons,
                                                        evtWeight);
      cutFlowTable.update("isCategoryCCompareRelaxedEvent", evtWeight);
    }


    // DONE

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


  delete run_lumi_eventSelector;

  delete selEventsFile;

  delete muonReader;
  delete jetReader;

  hltPaths_delete(triggers_1mu);
  hltPaths_delete(triggers_2mu);

  clock.Show("analyze_2mu1b1j");

  return EXIT_SUCCESS;
}
