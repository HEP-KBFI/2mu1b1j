import logging

from tthAnalysis.HiggsToTauTau.analyzeConfig import *
from tthAnalysis.HiggsToTauTau.jobTools import generate_file_ids, generate_input_list
from tthAnalysis.HiggsToTauTau.jobTools import create_if_not_exists


class analyzeConfig_2mu1b1j(analyzeConfig):
    """Configuration metadata needed to run analysis in a single go.

    Sets up a folder structure by defining full path names; no directory creation is delegated here.

    Args specific to analyzeConfig_2mu1b1j:
      lepton_selection: either `Tight`, `Loose` or `Fakeable`

    See $CMSSW_BASE/src/tthAnalysis/HiggsToTauTau/python/analyzeConfig.py
    for documentation of further Args.

    """

    def __init__(self, outputDir, executable_analyze, samples, lepton_selections, central_or_shifts,
                 max_files_per_job, era, use_lumi, lumi, debug, running_method, num_parallel_jobs,
                 histograms_to_fit, select_rle_output=False, executable_prep_dcard="prepareDatacard",
                 select_root_output=False):
        analyzeConfig.__init__(self, outputDir, executable_analyze, "2mu1b1j", central_or_shifts,
                               max_files_per_job, era, use_lumi, lumi, debug, running_method, num_parallel_jobs,
                               histograms_to_fit)

        self.samples = samples

        self.lepton_selections = lepton_selections

        for sample_name, sample_info in self.samples.items():
            if not sample_info["use_it"] or sample_info["sample_category"] in ["additional_signal_overlap", "background_data_estimate"]:
                continue
            process_name = sample_info["process_name_specific"]
            for lepton_selection in self.lepton_selections:
                key_dir = getKey(sample_name, lepton_selection)
                for dir_type in DIRLIST:
                    if (not select_rle_output and dir_type == DKEY_RLES) or \
                       (not select_root_output and dir_type == DKEY_ROOT):
                           continue
                    initDict(self.dirs, [key_dir, dir_type])
                    self.dirs[key_dir][dir_type] = os.path.join(
                                                                self.outputDir,
                                                                dir_type,
                                                                self.channel,
                                                                "_".join([lepton_selection]),
                                                                process_name
                                                                )
        print "self.dirs = ", self.dirs

        self.cfgFile_analyze_original = os.path.join(
            self.workingDir, "analyze_2mu1b1j_cfg_"+era+".py")
        self.histogramDir_prep_dcard = "2mu1b1j_Tight"
        self.select_rle_output = select_rle_output
        self.select_root_output = select_root_output
        self.era = era


    def createCfg_analyze(self, inputFiles, outputFile, sample_category, era, triggers, lepton_selection,
                          is_mc, central_or_shift, lumi_scale, cfgFile_modified, rle_output_file, root_output_file):
        """Create python configuration file for the analyze_2mu1b1j executable (analysis code)

        Args:
          inputFiles: list of input files (Ntuples)
          outputFile: output file of the job -- a ROOT file containing histogram
          process: either `TT`, `TTW`, `TTZ`, `EWK`, `Rares`, `data_obs`, `ttH_hww`, `ttH_hzz` or `ttH_htt`
          is_mc: flag indicating whether job runs on MC (True) or data (False)
          lumi_scale: event weight (= xsection * luminosity / number of events)
          central_or_shift: either 'central' or one of the systematic uncertainties defined in $CMSSW_BASE/src/tthAnalysis/HiggsToTauTau/bin/analyze_2mu1b1j.cc
        """
        lines = []
        #lines.append("process.fwliteOutput.fileName = cms.string('%s')" % outputFile)
        lines.append("process.fwliteInput.fileNames = cms.vstring(%s)" % inputFiles)
        lines.append("process.fwliteOutput.fileName = cms.string('%s')" % os.path.basename(outputFile))
        lines.append("process.analyze_2mu1b1j.process = cms.string('%s')" % sample_category)
        lines.append("process.analyze_2mu1b1j.use_triggers_1mu = cms.bool(%s)" % ("1mu" in triggers))
        lines.append("process.analyze_2mu1b1j.use_triggers_2mu = cms.bool(%s)" % ("2mu" in triggers))
        lines.append("process.analyze_2mu1b1j.leptonSelection = cms.string('%s')" % lepton_selection)
        lines.append("process.analyze_2mu1b1j.isMC = cms.bool(%s)" % is_mc)
        lines.append("process.analyze_2mu1b1j.central_or_shift = cms.string('%s')" % central_or_shift)
        lines.append("process.analyze_2mu1b1j.lumiScale = cms.double(%f)" % lumi_scale)
        lines.append("process.analyze_2mu1b1j.selEventsFileName_output = cms.string('%s')" % rle_output_file)
        lines.append("process.analyze_2mu1b1j.selEventsTFileName = cms.string('%s')" % root_output_file)
        lines.append("process.analyze_2mu1b1j.era = cms.string('%s')" % era)
        create_cfg(self.cfgFile_analyze_original, cfgFile_modified, lines)


    def create(self):
        """Creates all necessary config files and runs the complete analysis workfow -- either locally or on the batch system
        """

        for key in self.dirs.keys():
            for dir_type in self.dirs[key].keys():
                create_if_not_exists(self.dirs[key][dir_type])

        self.inputFileIds = {}
        for sample_name, sample_info in self.samples.items():
            if not sample_info["use_it"] or sample_info["sample_category"] in ["additional_signal_overlap", "background_data_estimate"]:
                continue

            process_name = sample_info["process_name_specific"]

            logging.info("Creating configuration files to run '%s' for sample %s" % (
                self.executable_analyze, process_name))

            (secondary_files, primary_store, secondary_store) = self.initializeInputFileIds(
                sample_name, sample_info)

            is_mc = (sample_info["type"] == "mc")
            lumi_scale = 1. if not (self.use_lumi and is_mc) else sample_info[
                "xsection"] * self.lumi / sample_info["nof_events"]
            sample_category = sample_info["sample_category"]
            triggers = sample_info["triggers"]

            for lepton_selection in self.lepton_selections:

                key_dir = getKey(sample_name, lepton_selection)

                for central_or_shift in self.central_or_shifts:

                    if self.select_root_output:
                        rootOutputSingleFile = os.path.join(self.dirs[key_dir][DKEY_ROOT], "out_%s_%s_%s_%s.root" %
                                                            (self.channel, process_name, lepton_selection, central_or_shift))
                        self.rootOutputAux[rootOutputSingleFile] = os.path.join(self.dirs[key_dir][DKEY_ROOT], "out_%s_%s_%s_%s_*.root" %
                                                                                (self.channel, process_name, lepton_selection, central_or_shift))

                    for jobId in range(len(self.inputFileIds[sample_name])):
                        if central_or_shift != "central" and not is_mc:
                            continue


                        key_dir = getKey(sample_name, lepton_selection)
                        key_file = getKey(sample_name, lepton_selection, central_or_shift, jobId)

                        inputFiles = generate_input_list(self.inputFileIds[sample_name][jobId], secondary_files, primary_store, secondary_store, self.debug)
                        self.ntupleFiles[key_file] = inputFiles

                        self.cfgFiles_analyze_modified[key_file] = os.path.join(self.dirs[key_dir][DKEY_CFGS], "analyze_%s_%s_%s_%s_%i_cfg.py" %
                                                                                (self.channel, process_name, lepton_selection, central_or_shift, jobId))
                        self.histogramFiles[key_file] = os.path.join(self.dirs[key_dir][DKEY_HIST], "%s_%s_%s_%i.root" %
                                                                     (process_name, lepton_selection, central_or_shift, jobId))
                        self.logFiles_analyze[key_file] = os.path.join(self.dirs[key_dir][DKEY_LOGS], "analyze_%s_%s_%s_%s_%i.log" %
                                                                       (self.channel, process_name, lepton_selection, central_or_shift, jobId))
                        self.rleOutputFiles[key_file] = os.path.join(self.dirs[key_dir][DKEY_RLES], "rle_%s_%s_%s_%s_%i.txt" %
                                                                     (self.channel, process_name, lepton_selection, central_or_shift, jobId)) if self.select_rle_output else ""
                        self.rootOutputFiles[key_file] = os.path.join(self.dirs[key_dir][DKEY_ROOT], "out_%s_%s_%s_%s_%i.root" %
                                                                      (self.channel, process_name, lepton_selection, central_or_shift, jobId)) if self.select_root_output else ""

                        self.createCfg_analyze(inputFiles,
                                                self.histogramFiles[key_file],
                                                sample_category,
                                                self.era,
                                                triggers,
                                                lepton_selection,
                                                is_mc,
                                                central_or_shift,
                                                lumi_scale,
                                                self.cfgFiles_analyze_modified[key_file],
                                                self.rleOutputFiles[key_file],
                                                self.rootOutputFiles[key_file]
                                                )

        if self.is_sbatch:
            logging.info(
                "Creating script for submitting '%s' jobs to batch system" % self.executable_analyze)
            self.createScript_sbatch()

        lines_makefile = []
        self.addToMakefile_analyze(lines_makefile)
        # # TODO hackfix (Margus)
        self.datacardFiles['this_value_is_useless_and_not_used_but_somehow_it_is_important_should_be_fixed'] = 'hadd_stage1'

        self.createMakefile(lines_makefile)

        logging.info("Done")
