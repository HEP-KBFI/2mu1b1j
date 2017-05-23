source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /home/`whoami`/VHbbNtuples_8_0_x/CMSSW_8_0_21/src/
cmsenv
python tthAnalysis/HiggsToTauTau/test/NtupleList.py

cd /home/margusp/VHbbNtuples_8_0_x/CMSSW_8_0_21/src/analysis2mu1b1j/analysis2mu1b1j/macros
bash run_macros.sh
