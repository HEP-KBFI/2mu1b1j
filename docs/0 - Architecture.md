1. On all of the nodes /home is mounted.


2. When I run "python test/run_analyse.py", then needed files created, including Makefile is created, that includes:

	```
	sbatch:
		python /home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/sbatch_analyze_2mu1b1j.py
	```

3. /home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/sbatch_analyze_2mu1b1j.py:

	```
	from tthAnalysis.HiggsToTauTau.sbatchManager import sbatchManager

	m = sbatchManager()
	m.setWorkingDir('/home/margusp/VHbbNtuples_7_6_x/CMSSW_7_6_3/src/analysis2mu1b1j/analysis2mu1b1j/test')
	m.submitJob(
	  ['/input/file1.root', '/input/file2.root'],
	  'analyze_2mu1b1j',
	  '/home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/cfgs/2mu1b1j/Tight/DoubleMuon_Run2015D/analyze_2mu1b1j_DoubleMuon_Run2015D_Tight_central_30_cfg.py',
	  '/home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/histograms/2mu1b1j/Tight/DoubleMuon_Run2015D',
	  ['DoubleMuon_Run2015D_Tight_central_30.root'],
	  '/home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/logs/2mu1b1j/Tight/DoubleMuon_Run2015D/analyze_2mu1b1j_DoubleMuon_Run2015D_Tight_central_30.log',
	  True)
	m.submitJob(...)
	m.waitForJobs()
	```

4. /home/margusp/analysis2mu1b1j/2015/2016Oct28_v1/cfgs/2mu1b1j/Tight/DoubleMuon_Run2015D/analyze_2mu1b1j_DoubleMuon_Run2015D_Tight_central_30_cfg.py

	Is Python configuration file which parameters are available to and used by C++ executable.


5. /python/sbatchManger.py

	* submitJob - submits tasks to sbatch and registeres its ID
	* waitForJobs - returns when all the jobs are finished


6. Makefile task wait until sbatch has ran, then next tasks are done (histograms adding etc).

7. Commands that are executed in the node are described in python/sbatchManager.py
