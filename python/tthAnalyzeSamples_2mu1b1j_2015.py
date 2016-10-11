from collections import OrderedDict as OD

from tthAnalysis.HiggsToTauTau.tthAnalyzeSamples_2015 import samples_2015


allowed_datasets = [
    "/DoubleMuon/Run2015C_25ns-16Dec2015-v1/MINIAOD",
    "/DoubleMuon/Run2015D-16Dec2015-v1/MINIAOD",
    "/SingleMuon/Run2015C_25ns-16Dec2015-v1/MINIAOD",
    "/SingleMuon/Run2015D-16Dec2015-v1/MINIAOD"
]

for sample_name, sample_info in samples_2015.items():
    # if sample_info["type"] == "mc":
    #    sample_info["triggers"] = [ "1e", "1mu" ]

    if sample_name not in allowed_datasets:
        sample_info["use_it"] = False
