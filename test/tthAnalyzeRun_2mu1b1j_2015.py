import os, logging, sys, getpass

from analysis2mu1b1j.analysis2mu1b1j.samples_2mu1b1j_2015 import samples_2015
from analysis2mu1b1j.analysis2mu1b1j.analyzeConfig_2mu1b1j import analyzeConfig_2mu1b1j
from tthAnalysis.HiggsToTauTau.jobTools import query_yes_no


ERA = "2015"

samples = samples_2015
LUMI =  2.3e+3 # 1/pb


version = "2016Oct20_v1"

if __name__ == '__main__':
  logging.basicConfig(
    stream = sys.stdout,
    level = logging.INFO,
    format = '%(asctime)s - %(levelname)s: %(message)s')

  analysis = analyzeConfig_2mu1b1j(
    outputDir = os.path.join("/home", getpass.getuser(), "analysis2mu1b1j", ERA, version),
    executable_analyze = "analyze_2mu1b1j",
    samples = samples,
    lepton_selections = [ "Tight" ],
    central_or_shifts = [ "central" ],
    max_files_per_job = 30,
    era = ERA,
    use_lumi = True,
    lumi = LUMI,
    debug = False,
    running_method = "sbatch",
    num_parallel_jobs = 4,
    histograms_to_fit = [ "EventCounter", "numJets" ],
    select_rle_output = False,
    select_root_output = False)

  analysis.create()

  run_analysis = query_yes_no("Start jobs ?")
  if run_analysis:
    analysis.run()
  else:
    sys.exit(0)
