# Simplified setup for muonHLTPaper for plots using CMSSW_8_0_29 (2016 data and MC)
This setup is tested in cmslpc. Should work in lxplus too. 

```
cmsrel CMSSW_8_0_29 
# please not this cmsrel version is different from the name of the repository. The name of repository should be 8_0_29 to be consistent but that was a typo. 
cd src
cmsenv
git cms-addpkg HLTrigger/Configuration
git clone https://github.com/bmahakud/muonHLTPaper_8_0_26 MuonHLTNtuples
scram b -j 8
voms-proxy-init -voms cms
cd MuonHLTNtuples/Tools
cmsRun HLTCfg2016Data_Mu.py
```
The above config file is generated using the command 

```
hltGetConfiguration orcoff:/cdaq/physics/Run2016/25ns15e33/v4.2.3/HLT/V2 \
--globaltag auto:run2_hlt_GRun \
--path HLTriggerFirstPath,\
HLT_IsoMu24_v*,\
HLT_IsoMu27_v*,\
HLT_IsoTkMu24_v*,\
HLT_IsoTkMu27_v*,\
HLT_Mu50_v*,\
HLT_MuTk50_v*,\
HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_*,\
HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_*,\
HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_*,\
HLTriggerFinalPath,\
HLTAnalyzerEndpath \
--input file:/afs/cern.ch/user/k/kplee/work/public/ROOTFile_Test/RAW_SingleMuon_Run2016H_Run284044.root \
--process MYHLT --full --offline \
--l1-emulator uGT \
--prescale none --max-events 100 --output none > HLTCfg2016Data_Mu.py
```

You can look inside of the file HLTCfg2016Data_Mu.py to see the lines of code needed to write to the ntuple.

If you endup with a  succesful cmsRun crabjobs could be submitted using CrabConfig_DataD_v2.py 
# Crab submission 
```
crab submit -c CrabConfig_DataH_v1.py   #change the output file location inside this file.
```
# Ntuple location 
Some ntuples can be found in the following directory. At the time of documenting this (May 24 2019) jobs were still running.
/eos/uscms/store/user/bmahakud/muHLTPaper_SingleMu_PromptReco2016H_v1_test4/.. . 



# Getting the L3/L1 efficiency plots fot the paper.
you need to run three macros after putting the input  files correctly. Codes are located inside the dir "Tools/Macros/CascadeNTkMu/" For running on Cascade
```
root -l readNtuplesPre_Cascade.C

```
For running TkMu


```
root -l readNtuplesPre_TkMu.C
```

for running CascadeORTkMU
```
root -l readNtuplesPre_CascadeAndTkMu.C
```
This will produce three different root files containing efficiency histograms, which could be plotted in diffrent styles.







