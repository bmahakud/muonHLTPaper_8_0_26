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



