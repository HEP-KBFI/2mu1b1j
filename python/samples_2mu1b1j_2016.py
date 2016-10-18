from collections import OrderedDict as OD

from tthAnalysis.HiggsToTauTau.samples_2016 import samples_2016

allowed_datasets = [
    "/DoubleMuon/Run2016B-PromptReco-v2/MINIAOD",
    "/DoubleMuon/Run2016C-PromptReco-v2/MINIAOD",
    "/DoubleMuon/Run2016D-PromptReco-v2/MINIAOD",
    "/DoubleMuon/Run2016E-PromptReco-v2/MINIAOD",
    "/DoubleMuon/Run2016F-PromptReco-v1/MINIAOD",
    "/DoubleMuon/Run2016G-PromptReco-v1/MINIAOD",
    "/SingleMuon/Run2016B-PromptReco-v2/MINIAOD",
    "/SingleMuon/Run2016C-PromptReco-v2/MINIAOD",
    "/SingleMuon/Run2016D-PromptReco-v2/MINIAOD",
    "/SingleMuon/Run2016E-PromptReco-v2/MINIAOD",
    "/SingleMuon/Run2016F-PromptReco-v1/MINIAOD",
    "/SingleMuon/Run2016G-PromptReco-v1/MINIAOD"
]

for sample_name, sample_info in samples_2016.items():
    # if sample_info["type"] == "mc":
    #    sample_info["triggers"] = [ "1e", "1mu" ]

    if sample_name not in allowed_datasets:
        sample_info["use_it"] = False
