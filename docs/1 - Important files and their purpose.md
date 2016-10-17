# Important files and their purpose

In this part of the documentation will be described how to set up your custom analysis.

For that you need to know:

1. Location of data
2. Contents of your data (is it Monte Carlo or observed? What parameters are available?)
3. Cuts you have to apply (thresholds for filtering signal from the background)
4. In which files of the software you specify location of the data and where you apply the cuts

Only point 4 is in the scope of this documentation.


## Before doing anything you have to set up your environment variables, use commands:

```bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /home/`whoami`/VHbbNtuples_7_6_x/CMSSW_7_6_3/src/
cmsenv
python tthAnalysis/HiggsToTauTau/test/NtupleList.py
```


## Installing latest version

```bash
cd $CMSSW_BASE/src
rm -rf $CMSSW_BASE/src/tthAnalysis/HiggsToTauTau
git clone https://github.com/HEP-KBFI/tth-htt  $CMSSW_BASE/src/tthAnalysis/HiggsToTauTau
rm -rf $CMSSW_BASE/src/analysis2mu1b1j/analysis2mu1b1j
git clone -b cleanup https://github.com/HEP-KBFI/2mu1b1j $CMSSW_BASE/src/analysis2mu1b1j/analysis2mu1b1j
scram b -j8

```


## How I run the tests?

```bash
cd /home/`whoami`/VHbbNtuples_7_6_x/CMSSW_7_6_3/src/analysis2mu1b1j/analysis2mu1b1j/test
python tthAnalyzeRun_2mu1b1j_2015.py
```


## How do I cancel tests

```bash
scancel -u margusp
```



## How to I specify the cuts?

There usually are cuts in your observed or MC data. This paragraph describes,
how to add additional cuts.


# Cuts that should be applied:

type a:

* two opposite sign muons with pT > 25 GeV, |η| < 2.1 with tight muon identification and loose tracker isolation
* one b–tagged jet pT > 30 GeV, |η| < 2.4 and no other jets with pT > 30 GeV, |η| < 2.4. Jet is tagged with CSV MVA algorithm and is required to have the b–tagging discriminator value greater that 0.783;
* at least one jet pT > 30 GeV, |η| > 2.4;

type b:

* two opposite sign muons with pT > 25 GeV, |η| < 2.1 with tight muon identification and loose tracker isolation;
* two jets with pT > 30 GeV, |η| < 2.4 with at least one b–tagged jet. Jet tagging criteria are the same as for the first excess observation;
* no jets with pT > 30 GeV, |η| > 2.4;
* missing ET < 40 GeV (against t ̄t background);
* di–muon and di–jet system are required to be back–to–back in the transverse plane of the detector, ∆φ(μμ − jj) > 2.5 (against t ̄t background);


# Steps to be done:

1. Copy important files (remember to rename all the files)
2. Replace their contents so that type a or b cuts would be applied
3. Draw histograms (mμμ) for data first (MC later?)
