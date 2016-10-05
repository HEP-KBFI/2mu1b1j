import os, logging, sys, getpass

import tthAnalysis.HiggsToTauTau.tthAnalyzeSamples_2mu1b1j
from tthAnalysis.HiggsToTauTau.analyzeConfig_2mu1b1j import analyzeConfig_2mu1b1j
from tthAnalysis.HiggsToTauTau.jobTools import query_yes_no

LUMI = 2301. # 1/pb

version = "2016Oct05_dR03mvaTight"

if __name__ == '__main__':
  logging.basicConfig(
    stream = sys.stdout,
    level = logging.INFO,
    format = '%(asctime)s - %(levelname)s: %(message)s')

  analysis = analyzeConfig_2mu1b1j(
    outputDir = os.path.join("/home", getpass.getuser(), "ttHAnalysis", version),
    executable_analyze = "analyze_2mu1b1j",
    lepton_selections = [ "Tight" ],
    central_or_shifts = [
      "central"
    ],
    max_files_per_job = 30,
    use_lumi = True, lumi = LUMI,
    debug = False,
    running_method = "sbatch",
    num_parallel_jobs = 4,
    histograms_to_fit = [ "EventCounter", "numJets", "mvaDiscr_3l", "mTauTauVis" ],
    select_rle_output = False,
    select_root_output = False)

  analysis.create()

  run_analysis = query_yes_no("Start jobs ?")
  if run_analysis:
    analysis.run()
  else:
    sys.exit(0)
