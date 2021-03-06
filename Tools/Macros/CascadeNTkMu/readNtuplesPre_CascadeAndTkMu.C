

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TTree.h"
#include "TBranch.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include "DataFormats/Math/interface/deltaR.h"
#include "MuonHLTNtuples/Analyzers/src/MuonTree.h"
#include "TLorentzVector.h"

double muonmass = 0.10565837;

bool selectTagMuon  (MuonCand, TH1F* );
bool selectProbeMuon(MuonCand, MuonCand, TH1F* );
bool selectMuon     (MuonCand);
bool matchMuon      (MuonCand, std::vector<HLTObjCand>, std::string);
//HLTMuonCand  matchL3        (MuonCand, std::vector<HLTMuonCand>);
//L1MuonCand   matchL1        (MuonCand, std::vector<L1MuonCand>);
bool  matchMuonWithL3 (MuonCand, std::vector<HLTMuonCand>);
void printProgBar(int);

std::string L1filter      =  "hltL1fL1sMu22Or25L1Filtered0::TEST"; 
std::string L2filter      = "hltL2fL1sMu22or25L1f0L2Filtered10Q::TEST";
std::string L3filter      =  "hltL3fL1sMu22Or25L1f0L2f10QL3Filtered27Q::TEST"; 
std::string isofilterTag  = "hltL3crIsoL1sMu22Or25L1f0L2f10QL3f27QL3trkIsoFiltered0p09::HLT";

double pt_bins[17]  = { 5, 7, 9, 12, 16,  20 ,  24 ,  27 ,   30,   35,   40,   45,   50,  60, 70 ,  90, 150 };
double dz_bins[11]  = { -15., -8., -6., -4., -2.,  0.,  2.,  4.,   6.,   8.,  15.};
double eta_bins[16] = {-2.4, -2.1, -1.6, -1.2, -1.04, -0.9, -0.3, -0.2,  0.2, 0.3, 0.9, 1.04, 1.2, 1.6, 2.1, 2.4};
double iso_bins[12] = { 0  , 0.02, 0.04, 0.06, 0.08,  0.1, 0.12, 0.16, 0.2, 0.3, 0.6, 1   };
double offlineIsoCut = 0.15;

// ******************************************
//                                          *
//                                          *
std::string hltname        = "HLT_IsoMu27_v10"; 
std::string thepassfilter  = L3filter;
std::string the2016filter  = "hltL3crIsoL1sMu22Or25L1f0L2f10QL3f27QL3trkIsoFiltered0p07::HLT";
std::string theprobefilter = L1filter; 
float offlinePtCut         = 30.; 
//                                          *
//                                          *
// ******************************************

void readNtuplesPre_CascadeAndTkMu(TString inputfilename="../../muonNtuple2016Data.root",/*/eos/uscms/store/user/bmahakud/ProductionCasTest_v1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/ProductionCasTest_v1/181205_133338/0000/muonNtupleCorrCasTk.root",*/ std::string effmeasured="CascadeORTkMu_"){

  ///eos/uscms/store/user/bmahakud/ProductionCasTest_v1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/ProductionCasTest_v1/181205_133338/0000/NtupleTMP.root
  ///eos/uscms/store/user/bmahakud/TestCascade_LPC_v3/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/TestCascade_LPC_v3/181203_142416/0000/muonNtupleCasNew.root

  bool doingL1 = thepassfilter.find("L1fL1") != std::string::npos; 

  TFile* outfile = TFile::Open(Form("%s_efficiency_prefilter.root", effmeasured.c_str()),"RECREATE");
  std::cout << "output file: " << outfile -> GetName() << std::endl;

  //Create histograms  
  TH1F* dimuon_mass             = new TH1F("h_dimuon_mass"          ,"dimuon_mass"      , 1500,  0,  150 );
  TH1F* tagiso                  = new TH1F("h_tagiso"               ,"tagiso"           ,  100,  0,  1   );
  TH1F* tagMuonPt               = new TH1F("h_tagMuonPt"            ,"tagMuonPt"        ,  150,  0,  150 );
  TH1F* nvtx_event              = new TH1F("h_nvtx_event"           ,"nvtx_event"       ,   60,  0,   60 );
 
  TEfficiency* muonPt_barrel    = new TEfficiency("muonPt_barrel"   ,"muonPt_barrel"    ,   16,  pt_bins );
  TEfficiency* muonPt_endcap    = new TEfficiency("muonPt_endcap"   ,"muonPt_endcap"    ,   16,  pt_bins );
  TEfficiency* muonPt           = new TEfficiency("muonPt"          ,"muonPt"           ,   16,  pt_bins ); 
  TEfficiency* muonPtTurnOn     = new TEfficiency("muonPtTurnOn"    ,"muonPtTurnOn"     ,   16,  pt_bins ); 
  TEfficiency* muonEta          = new TEfficiency("muonEta"         ,"muonEta"          ,   15, eta_bins );
  TEfficiency* muonPhi          = new TEfficiency("muonPhi"         ,"muonPhi"          ,   20, -3.2, 3.2);
  TEfficiency* muonEff          = new TEfficiency("muonEff"         ,"muonEff"          ,    1,   0., 1.0);

  // GLOBAL QUANTITIES//
  TEfficiency* muonchi2         = new TEfficiency("muonchi2"        , "muonchi2"        ,  20,    0.,  7.);
  TEfficiency* muondxy          = new TEfficiency("muondxy"         , "muondxy"         ,  100,  -0.3, 0.3);
  TEfficiency* muondz           = new TEfficiency("muondz"          , "muondz"          ,   10,   dz_bins);
  TEfficiency* muonPixHit       = new TEfficiency("muonPixHit"      , "muonPixHit"      ,   20,  -0.5,19.5);
  TEfficiency* muonLayHit       = new TEfficiency("muonLayHit"      , "muonLayHit"      ,   16,   2.5,18.5);
  TEfficiency* muonPixLay       = new TEfficiency("muonPixLay"      , "muonPixLay"      ,    7,  -0.5, 6.5);
  TEfficiency* muoninnerPt      = new TEfficiency("muoninnerPt"     , "muoninnerPt"     ,   16,   pt_bins );
  TEfficiency* muoninnerEta     = new TEfficiency("muoninnerEta"    , "muoninnerEta"    ,   15,   eta_bins);
  TEfficiency* muoninnerPhi     = new TEfficiency("muoninnerPhi"    , "muoninnerPhi"    ,   20,  -3.2, 3.2);
  TEfficiency* muonValHits      = new TEfficiency("muonValHits"     , "muonValHits"     ,   20, -0.5, 19.5);

  // BARREL//
  TEfficiency* muoninnerPt_barrel    = new TEfficiency("muoninnerPt_barrel"   , "muoninnerPt_barrel"   ,   16,   pt_bins );
  TEfficiency* muoninnerEta_barrel   = new TEfficiency("muoninnerEta_barrel"  , "muoninnerEta_barrel"  ,   15,   eta_bins);
  TEfficiency* muoninnerPhi_barrel   = new TEfficiency("muoninnerPhi_barrel"  , "muoninnerPhi_barrel"  ,   20,  -3.2, 3.2);
  TEfficiency* muonchi2_barrel       = new TEfficiency("muonchi2_barrel"      , "muonchi2_barrel"      ,  20,    0.,   7.);
  TEfficiency* muondxy_barrel        = new TEfficiency("muondxy_barrel"       , "muondxy_barrel"       ,  100, -0.3,  0.3);
  TEfficiency* muondz_barrel         = new TEfficiency("muondz_barrel"        , "muondz_barrel"        ,   10,  dz_bins);
  TEfficiency* muonPixHit_barrel     = new TEfficiency("muonPixHit_barrel"    , "muonPixHit_barrel"    ,   20,  -0.5, 19.5);
  TEfficiency* muonLayHit_barrel     = new TEfficiency("muonLayHit_barrel"    , "muonLayHit_barrel"    ,   16,   2.5, 18.5);
  TEfficiency* muonPixLay_barrel     = new TEfficiency("muonPixLay_barrel"    , "muonPixLay_barrel"    ,    7,  -0.5,  6.5);
  TEfficiency* muonValHits_barrel    = new TEfficiency("muonValHits_barrel"   , "muonValHits_barrel"   ,   20, -0.5, 19.5);

  // INTERMEDIATE REGION//
  TEfficiency* muonPt_int       = new TEfficiency("muonPt_int"       , "muonPt_int"        ,   16,  pt_bins );
  TEfficiency* nvtx_int         = new TEfficiency("nvtx_int"         , "nvtx_int"          ,   60,    0,   60);
  TEfficiency* muoninnerPt_int  = new TEfficiency("muoninnerPt_int"  , "muoninnerPt_int"   ,   16,   pt_bins );
  TEfficiency* muoninnerEta_int = new TEfficiency("muoninnerEta_int" , "muoninnerEta_int"  ,   15,   eta_bins);
  TEfficiency* muoninnerPhi_int = new TEfficiency("muoninnerPhi_int" , "muoninnerPhi_int"  ,   20,  -3.2, 3.2);
  TEfficiency* muonchi2_int     = new TEfficiency("muonchi2_int"     , "muonchi2_int"      ,  20,   0.,  15.);
  TEfficiency* muondxy_int      = new TEfficiency("muondxy_int"      , "muondxy_int"       ,  100, -0.3,  0.3);
  TEfficiency* muondz_int       = new TEfficiency("muondz_int"       , "muondz_int"        ,   10, dz_bins);
  TEfficiency* muonPixHit_int   = new TEfficiency("muonPixHit_int"   , "muonPixHit_int"    ,   20, -0.5, 19.5);
  TEfficiency* muonLayHit_int   = new TEfficiency("muonLayHit_int"   , "muonLayHit_int"    ,   16,  2.5, 18.5);
  TEfficiency* muonPixLay_int   = new TEfficiency("muonPixLay_int"   , "muonPixLay_int"    ,    7, -0.5,  6.5);
  TEfficiency* muonValHits_int  = new TEfficiency("muonValHits_int"  , "muonValHits_int"   ,   20, -0.5, 19.5);

  // ENDCAP//
  TEfficiency* muoninnerPt_endcap  = new TEfficiency("muoninnerPt_endcap"  , "muoninnerPt_endcap"   ,   16,   pt_bins );
  TEfficiency* muoninnerEta_endcap = new TEfficiency("muoninnerEta_endcap" , "muoninnerEta_endcap"  ,   15,   eta_bins);
  TEfficiency* muoninnerPhi_endcap = new TEfficiency("muoninnerPhiendcap"  , "muoninnerPhi_endcap"  ,   20,  -3.2, 3.2);
  TEfficiency* muonchi2_endcap     = new TEfficiency("muonchi2_endcap"     , "muonchi2_endcap"      ,  20,   0.,   7.);
  TEfficiency* muondxy_endcap      = new TEfficiency("muondxy_endcap"      , "muondxy_endcap"       ,  100, -0.3,  0.3);
  TEfficiency* muondz_endcap       = new TEfficiency("muondz_endcap"       , "muondz_endcap"        ,   10, dz_bins);
  TEfficiency* muonPixHit_endcap   = new TEfficiency("muonPixHit_endcap"   , "muonPixHit_endcap"    ,   20, -0.5, 19.5);
  TEfficiency* muonLayHit_endcap   = new TEfficiency("muonLayHit_endcap"   , "muonLayHit_endcap"    ,   16,  2.5, 18.5);
  TEfficiency* muonPixLay_endcap   = new TEfficiency("muonPixLay_endcap"   , "muonPixLay_endcap"    ,    7, -0.5,  6.5);
  TEfficiency* muonValHits_endcap  = new TEfficiency("muonValHits_endcap"  , "muonValHits_endcap"   ,   20, -0.5, 19.5);

  TEfficiency* muonOver16Pt     = new TEfficiency("muonOver16Pt"    ,"muonOver16Pt"     ,   16,  pt_bins ); 
  TEfficiency* muonOver16Eta    = new TEfficiency("muonOver16Eta"   ,"muonOver16Eta"    ,   15, eta_bins );
  TEfficiency* muonOver16Phi    = new TEfficiency("muonOver16Phi"   ,"muonOver16Phi"    ,   20, -3.2, 3.2);
  TEfficiency* muonOver16Eff    = new TEfficiency("muonOver16Eff"   ,"muonOver16Eff"    ,   1 ,   0., 1.0);
  
  TEfficiency* failingMuonPt    = new TEfficiency("failingMuonPt"   ,"failingMuonPt"    ,   16,  pt_bins ); 
  TEfficiency* failingMuonEta   = new TEfficiency("failingMuonEta"  ,"failingMuonEta"   ,   15, eta_bins );
  TEfficiency* failingMuonPhi   = new TEfficiency("failingMuonPhi"  ,"failingMuonPhi"   ,   20, -3.2, 3.2);
  TEfficiency* failingMuonEff   = new TEfficiency("failingMuonEff"  ,"failingMuonEff"   ,   1 ,   0., 1.0);

  TH1F* PassingProbePt          = new TH1F("h_PassingProbePt"       ,"PassingMuonPt"    ,  16,  pt_bins );
  TH1F* PassingProbeEta         = new TH1F("h_PassingProbeEta"      ,"PassingMuonEta"   ,  15, eta_bins );
  TH1F* PassingProbePhi         = new TH1F("h_PassingProbePhi"      ,"PassingMuonPhi"   ,  20, -3.2, 3.2);
  TH1F* PassingProbeMll         = new TH1F("h_PassingProbeMll"      ,"PassingMuonMll"   ,  20,  86., 96.);

  TH1F* FailingProbePt          = new TH1F("h_FailingProbePt"       ,"FailingMuonPt"    ,  16,  pt_bins );
  TH1F* FailingProbeEta         = new TH1F("h_FailingProbeEta"      ,"FailingMuonEta"   ,  15, eta_bins );
  TH1F* FailingProbePhi         = new TH1F("h_FailingProbePhi"      ,"FailingMuonPhi"   ,  20, -3.2, 3.2);
  TH1F* FailingProbeMll         = new TH1F("h_FailingProbeMll"      ,"FailingMuonMll"   ,  20,  86., 96.);

  // for dimuon eff: 
  TEfficiency* diMuonPt         = new TEfficiency("diMuonPt"        ,"diMuonPt"         ,   16,  pt_bins ); 
  TEfficiency* diMuonEta        = new TEfficiency("diMuonEta"       ,"diMuonEta"        ,   15, eta_bins );
  TEfficiency* diMuonPhi        = new TEfficiency("diMuonPhi"       ,"diMuonPhi"        ,   20, -3.2, 3.2);

  // for noFilter matching: 
  TEfficiency* hltmuonPt        = new TEfficiency("hltmuonPt"       ,"hltmuonPt"        ,   16,  pt_bins ); 
  TEfficiency* hltmuonEta       = new TEfficiency("hltmuonEta"      ,"hltmuonEta"       ,   15, eta_bins );
  TEfficiency* hltmuonPhi       = new TEfficiency("hltmuonPhi"      ,"hltmuonPhi"       ,   20, -3.2, 3.2);
  TEfficiency* hltmuonEff       = new TEfficiency("hltmuonEff"      ,"hltmuonEff"       ,   1 ,   0.,  1.);

  TEfficiency* hltmuonFromL2Pt  = new TEfficiency("hltmuonFromL2Pt"  ,"hltmuonFromL2Pt"  ,   16,  pt_bins ); 
  TEfficiency* hltmuonFromL2Eta = new TEfficiency("hltmuonFromL2Eta" ,"hltmuonFromL2Eta" ,   15, eta_bins );
  TEfficiency* hltmuonFromL2Phi = new TEfficiency("hltmuonFromL2Phi" ,"hltmuonFromL2Phi" ,   20, -3.2, 3.2);
  TEfficiency* hltmuonFromL2Eff = new TEfficiency("hltmuonFromL2Eff" ,"hltmuonFromL2Eff" ,   1 ,   0.,  1.);

  TEfficiency* nvtx             = new TEfficiency("nvtx"             ,"nvtx"             ,   60,    0,  60);
  TEfficiency* nvtx_barrel      = new TEfficiency("nvtx_barrel"      ,"nvtx_barrel"      ,   60,    0,  60);
  TEfficiency* nvtx_endcap      = new TEfficiency("nvtx_endcap"      ,"nvtx_endcap"      ,   60,    0,  60);
   
  double offlineiso04 = 100;
  
  TFile* inputfile = TFile::Open(inputfilename, "READ");
  std::cout << "input file: " << inputfile -> GetName() << std::endl;

  TTree *tree = (TTree*) inputfile -> Get("muonNtuples/muonTree"); 
  
  if (!tree) {
    std::cout << " *** tree not found *** " << std::endl;
    return;
  }
    
  MuonEvent* ev      = new MuonEvent();
  TBranch*  evBranch = tree->GetBranch("event");
  evBranch -> SetAddress(&ev);

  int nentries = tree->GetEntriesFast();
  std::cout << "Number of entries = " << nentries << std::endl;

  bool flagfile = false;
  for (Int_t eventNo=0; eventNo < nentries; eventNo++) 
    {
      Int_t IgetEvent   = tree   -> GetEvent(eventNo); 
      printProgBar((int)(eventNo*100./nentries));
    
      unsigned int nmuons = ev->muons.size();
      if (nmuons < 2) continue;
      unsigned int nhltmuons = ev->hltmuons.size(); 
    
      //    if (!ev-> hltTag.find(hltname)) continue;
      //nvtx_event-> Fill( ev -> nVtx   ); 

      for (int imu = 0; imu < nmuons; imu++){ 
	// select the tag muon        
	if (! selectTagMuon(ev -> muons.at(imu), tagiso)) continue;

	if (! matchMuon(ev -> muons.at(imu), ev -> hltTag.objects, isofilterTag)) continue;

        cout<<"....."<<endl;
	tagMuonPt -> Fill ( ev -> muons.at(imu).pt) ; 

	for (int jmu = 0; jmu < nmuons; jmu++){
	  bool pass   = false;
	  bool matchWith2016 = false;

	  // select the probe muon
	  if (!selectProbeMuon(ev -> muons.at(jmu), ev -> muons.at(imu), dimuon_mass)) continue;
	   //cout<< "probe muon"<< endl;

	  if (!doingL1 && !(matchMuon(ev -> muons.at(jmu), ev -> hlt.objects, theprobefilter))) continue;

	  //PRE FILTER
          bool passhlt=false;
          bool passTkMu=false;
          if(matchMuonWithL3(ev->muons.at(jmu),ev->hltmuons))passhlt=true;
          if(matchMuonWithL3(ev->muons.at(jmu),ev->tkmuons))passTkMu=true;

	  //if ( matchMuonWithL3(ev->muons.at(jmu),ev->hltmuons) || matchMuonWithL3(ev->muons.at(jmu),ev->tkmuons)) pass=true;
          if(passTkMu  || passhlt)pass=true;
	  //	  if ( matchMuonWithL3(ev->muons.at(jmu),ev->hltmuons)) pass=true;

	  muonPtTurnOn -> Fill( pass, ev -> muons.at(jmu).pt ); 
 
	  // now require pT cut: 
	  if (ev -> muons.at(jmu).pt < offlinePtCut) continue; 

	  TLorentzVector mu1, mu2;
	  mu1.SetPtEtaPhiM (ev->muons.at(imu).pt,ev->muons.at(imu).eta,ev->muons.at(imu).phi, muonmass); 
	  mu2.SetPtEtaPhiM (ev->muons.at(jmu).pt,ev->muons.at(jmu).eta,ev->muons.at(jmu).phi, muonmass);
	  double mumumass = (mu1 + mu2).M();

	  // if (pass) { 
	  //   PassingProbePt  -> Fill( ev -> muons.at(jmu).pt  );
	  //   PassingProbeEta -> Fill( ev -> muons.at(jmu).eta );
	  //   PassingProbePhi -> Fill( ev -> muons.at(jmu).phi );
	  //   PassingProbeMll -> Fill( mumumass                );

	  // }	      
	  // else {       
	  //   FailingProbePt  -> Fill( ev -> muons.at(jmu).pt  );
	  //   FailingProbeEta -> Fill( ev -> muons.at(jmu).eta );
	  //   FailingProbePhi -> Fill( ev -> muons.at(jmu).phi );
	  //   FailingProbeMll -> Fill( mumumass                );
	  // }
	  muonPt       -> Fill( pass, ev -> muons.at(jmu).pt );
	  muonEta      -> Fill( pass, ev -> muons.at(jmu).eta);
	  muonPhi      -> Fill( pass, ev -> muons.at(jmu).phi);
	  muonEff      -> Fill( pass, 0.5                    );

	  muoninnerPt  -> Fill( pass, ev -> muons.at(jmu).innerpt);
	  muoninnerEta -> Fill( pass, ev -> muons.at(jmu).innereta); 
	  muoninnerPhi -> Fill( pass, ev -> muons.at(jmu).innerphi); 

	  muonchi2   -> Fill( pass, ev -> muons.at(jmu).innerchi2 );
	  muondxy    -> Fill( pass, ev -> muons.at(jmu).innerdxy);
	  muondz     -> Fill( pass, ev -> muons.at(jmu).innerdz);
	  muonPixHit -> Fill( pass, ev -> muons.at(jmu).innerpixelHits);
	  muonLayHit -> Fill( pass, ev -> muons.at(jmu).innerlayerHits);
	  muonPixLay -> Fill( pass, ev -> muons.at(jmu).innerpixelLayers);
	  muonValHits-> Fill( pass, ev -> muons.at(jmu).innervalidHits);


	  // muonOver16Pt  -> Fill( pass && matchWith2016, ev -> muons.at(jmu).pt );
	  // muonOver16Eta -> Fill( pass && matchWith2016, ev -> muons.at(jmu).eta);
	  // muonOver16Phi -> Fill( pass && matchWith2016, ev -> muons.at(jmu).phi);
	  // muonOver16Eff -> Fill( pass && matchWith2016, 0.5                    );

	  // nvtx           -> Fill( pass, ev -> nVtx             );
	  // failingMuonPt  -> Fill( !pass, ev -> muons.at(jmu).pt );
	  // failingMuonEta -> Fill( !pass, ev -> muons.at(jmu).eta);
	  // failingMuonPhi -> Fill( !pass, ev -> muons.at(jmu).phi);
	  // failingMuonEff -> Fill( !pass, 0.5                    );

	  // if ( (ev -> muons.at(jmu).innerpixelHits) <= 3) pixhitcount++; 


	  // BARREL//
	  if (fabs(ev -> muons.at(jmu).eta) <= 0.9){
	    muonPt_barrel       -> Fill( pass, ev -> muons.at(jmu).pt );
	    muoninnerPt_barrel  -> Fill( pass, ev -> muons.at(jmu).innerpt); 
	    muoninnerEta_barrel -> Fill( pass, ev -> muons.at(jmu).innereta); 
	    muoninnerPhi_barrel -> Fill( pass, ev -> muons.at(jmu).innerphi); 
	    nvtx_barrel         -> Fill( pass, ev -> nVtx );
	    muonchi2_barrel     -> Fill( pass, ev -> muons.at(jmu).innerchi2 );
	    muondxy_barrel      -> Fill( pass, ev -> muons.at(jmu).innerdxy);
	    muondz_barrel       -> Fill( pass, ev -> muons.at(jmu).innerdz); 
	    muonPixHit_barrel   -> Fill( pass, ev -> muons.at(jmu).innerpixelHits); 
	    muonLayHit_barrel   -> Fill( pass, ev -> muons.at(jmu).innerlayerHits); 
	    muonPixLay_barrel   -> Fill( pass, ev -> muons.at(jmu).innerpixelLayers);  
	    muonValHits_barrel  -> Fill( pass, ev -> muons.at(jmu).innervalidHits);

	  }

	  // INTERMEDIATE REGION//
	  if (fabs(ev -> muons.at(jmu).eta)>0.9 && fabs(ev -> muons.at(jmu).eta)<1.6){
	    muonPt_int       -> Fill( pass, ev -> muons.at(jmu).pt );
	    muoninnerPt_int  -> Fill( pass, ev -> muons.at(jmu).innerpt); 
	    muoninnerEta_int -> Fill( pass, ev -> muons.at(jmu).innereta); 
	    muoninnerPhi_int -> Fill( pass, ev -> muons.at(jmu).innerphi); 
	    nvtx_int         -> Fill( pass, ev -> nVtx );
	    muonchi2_int     -> Fill( pass, ev -> muons.at(jmu).innerchi2 );
	    muondxy_int      -> Fill( pass, ev -> muons.at(jmu).innerdxy);
	    muondz_int       -> Fill( pass, ev -> muons.at(jmu).innerdz); 
	    muonPixHit_int   -> Fill( pass, ev -> muons.at(jmu).innerpixelHits); 
	    muonLayHit_int   -> Fill( pass, ev -> muons.at(jmu).innerlayerHits); 
	    muonPixLay_int   -> Fill( pass, ev -> muons.at(jmu).innerpixelLayers);  
	    muonValHits_int  -> Fill( pass, ev -> muons.at(jmu).innervalidHits);
	  }
	  // ENDCAP//
	  if ( fabs(ev -> muons.at(jmu).eta)>=1.6){
	    muonPt_endcap       -> Fill( pass, ev -> muons.at(jmu).pt );
	    muoninnerPt_endcap  -> Fill( pass, ev -> muons.at(jmu).innerpt); 
	    muoninnerEta_endcap -> Fill( pass, ev -> muons.at(jmu).innereta); 
	    muoninnerPhi_endcap -> Fill( pass, ev -> muons.at(jmu).innerphi); 
	    nvtx_endcap         -> Fill( pass, ev -> nVtx );
	    muonchi2_endcap     -> Fill( pass, ev -> muons.at(jmu).innerchi2 );
	    muondxy_endcap      -> Fill( pass, ev -> muons.at(jmu).innerdxy);
	    muondz_endcap       -> Fill( pass, ev -> muons.at(jmu).innerdz); 
	    muonPixHit_endcap   -> Fill( pass, ev -> muons.at(jmu).innerpixelHits); 
	    muonLayHit_endcap   -> Fill( pass, ev -> muons.at(jmu).innerlayerHits); 
	    muonPixLay_endcap   -> Fill( pass, ev -> muons.at(jmu).innerpixelLayers); 
	    muonValHits_endcap  -> Fill( pass, ev -> muons.at(jmu).innervalidHits);
	   }


	  // /// MATCH to TRIGGER objects	 
	  // pass=false;
	  // if (matchMuonWithL3(ev->muons.at(jmu),ev->tkmuons) || matchMuonWithL3(ev->muons.at(jmu),ev->hltmuons)) pass = true; 
	  // hltmuonPt       -> Fill( pass, ev -> muons.at(jmu).pt ); 
	  // hltmuonEta      -> Fill( pass, ev -> muons.at(jmu).eta);
	  // hltmuonPhi      -> Fill( pass, ev -> muons.at(jmu).phi);
	  // hltmuonEff      -> Fill( pass, 0.5                    );


	  // /// MATCH to TRIGGER objects	 
	  // pass=false;
	  // if (matchMuonWithL3(ev->muons.at(jmu),ev->tkmuons)) pass = true;
	  // hltmuonFromL2Pt       -> Fill( pass, ev -> muons.at(jmu).pt );
	  // hltmuonFromL2Eta      -> Fill( pass, ev -> muons.at(jmu).eta);
	  // hltmuonFromL2Phi      -> Fill( pass, ev -> muons.at(jmu).phi);
	  // hltmuonFromL2Eff      -> Fill( pass, 0.5                    );
	} // nmuons
      }
      // NOW FOR DIMUONS:
      // for (int imu = 0; imu < nmuons; imu++){
      // 	if (!selectMuon(ev -> muons.at(imu))) continue; 
      
      // 	for (int jmu = imu+1; jmu < nmuons; jmu++){
      // 	  if (!selectMuon(ev -> muons.at(jmu))) continue; 
      // 	  bool pass = false;
      // 	  if (matchMuonWithL3(ev->muons.at(jmu),ev->hltmuons) && matchMuonWithL3(ev->muons.at(imu),ev->hltmuons)) pass = true;
      // 	  diMuonPt       -> Fill( pass, ev -> muons.at(imu).pt ); 
      // 	  diMuonEta      -> Fill( pass, ev -> muons.at(imu).eta);
      // 	  diMuonPhi      -> Fill( pass, ev -> muons.at(imu).phi);
      // 	  diMuonPt       -> Fill( pass, ev -> muons.at(jmu).pt );
      // 	  diMuonEta      -> Fill( pass, ev -> muons.at(jmu).eta);
      // 	  diMuonPhi      -> Fill( pass, ev -> muons.at(jmu).phi);
      // 	}
      // }
    }  

 
  //Writing the histograms in a file.
  outfile           -> cd();
  tagMuonPt         -> Write();
  
  muonPt            -> Write();
  muonPtTurnOn      -> Write();
  muonEta           -> Write();
  muonPhi           -> Write();
  muonEff           -> Write();

  muonchi2     -> Write();
  muondxy      -> Write();
  muondz       -> Write();
  muonPixHit   -> Write();
  muonLayHit   -> Write();
  muonPixLay   -> Write();
  muoninnerPt  -> Write();
  muoninnerEta -> Write();
  muoninnerPhi -> Write();
  muonValHits  -> Write();

  muonPt_barrel       -> Write();
  muoninnerPt_barrel  -> Write();
  muoninnerEta_barrel -> Write();
  muoninnerPhi_barrel -> Write();
  nvtx_barrel         -> Write();
  muonchi2_barrel     -> Write();
  muondxy_barrel      -> Write();
  muondz_barrel       -> Write();
  muonPixHit_barrel   -> Write();
  muonLayHit_barrel   -> Write();
  muonPixLay_barrel   -> Write();
  muonValHits_barrel  -> Write();

  muonPt_int       -> Write();
  muoninnerPt_int  -> Write();
  muoninnerEta_int -> Write();
  muoninnerPhi_int -> Write();
  nvtx_int         -> Write();
  muonchi2_int     -> Write();
  muondxy_int      -> Write();
  muondz_int       -> Write();
  muonPixHit_int   -> Write();
  muonLayHit_int   -> Write();
  muonPixLay_int   -> Write();
  muonValHits_int  -> Write();

  muonPt_endcap       -> Write();
  muoninnerPt_endcap  -> Write();
  muoninnerEta_endcap -> Write();
  muoninnerPhi_endcap -> Write();
  nvtx_endcap         -> Write();
  muonchi2_endcap     -> Write();
  muondxy_endcap      -> Write();
  muondz_endcap       -> Write();
  muonPixHit_endcap   -> Write();
  muonLayHit_endcap   -> Write();
  muonPixLay_endcap   -> Write();
  muonValHits_endcap  -> Write();

  failingMuonPt   -> Write();
  failingMuonEta  -> Write();
  failingMuonPhi  -> Write();
  failingMuonEff  -> Write();

  PassingProbePt  -> Write();
  PassingProbeEta -> Write();
  PassingProbePhi -> Write();
  PassingProbeMll -> Write();
  
  FailingProbePt  -> Write();
  FailingProbeEta -> Write();
  FailingProbePhi -> Write();
  FailingProbeMll -> Write();

  hltmuonPt       -> Write();
  hltmuonEta      -> Write();
  hltmuonPhi      -> Write();
  hltmuonEff      -> Write();

  hltmuonFromL2Pt  -> Write();
  hltmuonFromL2Eta -> Write();
  hltmuonFromL2Phi -> Write();
  hltmuonFromL2Eff -> Write();

  diMuonPt         -> Write();
  diMuonEta        -> Write();
  diMuonPhi        -> Write();

  nvtx_event       -> Write();
  nvtx             -> Write();
  
  dimuon_mass      -> Write();
  tagiso           -> Write();
  
  outfile          -> Close();  
  
  return;
}

bool matchMuon(MuonCand mu, std::vector<HLTObjCand> toc, std::string tagFilterName){

  bool match = false;
  int ntoc = toc.size();

  float minDR = 0.1; 
  if (tagFilterName.find("L1fL1") != std::string::npos) minDR = 1.0;
  float theDR = 100;
  for ( std::vector<HLTObjCand>::const_iterator it = toc.begin(); it != toc.end(); ++it ) { 
    if ( it->filterTag.compare(tagFilterName) == 0) { 
      theDR = deltaR(it -> eta, it -> phi, mu.eta, mu.phi); 
      if (theDR < minDR){
        minDR = theDR;
        match = true;
      }
    }
  }
  
  return match;
}

bool selectTagMuon(MuonCand mu, TH1F* tagh){
  
  if (!( mu.pt         > 27  )) return false; 
  if (!( fabs(mu.eta)  < 2.4 )) return false; 
  if (!( mu.isTight    == 1  )) return false; 
  
  //add isolation cut
  float offlineiso04 = mu.chargedDep_dR04 + std::max(0., mu.photonDep_dR04 + mu.neutralDep_dR04 - 0.5*mu.puPt_dR04);
  offlineiso04       = offlineiso04 / mu.pt;
  tagh -> Fill(offlineiso04);
  if (offlineiso04   > offlineIsoCut) return false; 

  return true;
}


bool selectMuon(MuonCand mu){  
  if (!( mu.pt         > offlinePtCut  )) return false; 
  if (!( fabs(mu.eta)  < 2.4 )) return false;
  if (!( mu.isLoose    == 1  )) return false; 
  return true;
}


//select the probe muon
bool selectProbeMuon(MuonCand mu, MuonCand tagMu, TH1F* dimuon_mass){
  
  if (mu.pt == tagMu.pt  && 
      mu.pt == tagMu.eta &&
      mu.pt == tagMu.phi ) 
    return false;
  
  if (!( mu.pt          > 0  )) return false; 
  if (!( fabs(mu.eta)  < 2.4 )) return false; 
  if (!( mu.isTight    == 1  )) return false; 
  if (mu.charge * tagMu.charge > 0) return false;
  //add isolation cut
  float offlineiso04 = mu.chargedDep_dR04 + std::max(0., mu.photonDep_dR04 + mu.neutralDep_dR04 - 0.5*mu.puPt_dR04);
  offlineiso04       = offlineiso04 / mu.pt;
  if (offlineiso04   > offlineIsoCut) return false; 
  
  TLorentzVector mu1, mu2;
  mu1.SetPtEtaPhiM (mu.pt   , mu.eta   , mu.phi   , muonmass);
  mu2.SetPtEtaPhiM (tagMu.pt, tagMu.eta, tagMu.phi, muonmass);
  double mumumass = (mu1 + mu2).M();
  dimuon_mass -> Fill(mumumass); 
  if (! (mumumass > 86. && mumumass < 96. )) return false;
  
  return true;
}

//*********************************************************************************************************************
//HLTMuonCand matchL3(MuonCand mu, std::vector<HLTMuonCand> L3cands){

//  bool match = false;
//  int nL3 = L3cands.size();

//  float minDR = 0.1;
//  float theDR = 100;
//  HLTMuonCand theL3;
//  theL3.pt        = -1000;
//  theL3.eta       = -1000;
//  theL3.phi       = -1000;
//  theL3.trkpt     = -1000;
//  theL3.ecalDep   = -1000;
//  theL3.hcalDep   = -1000;
//  theL3.trkDep    = -1000;
//  theL3.ecalDep05 = -1000;
//  theL3.hcalDep05 = -1000;
//  theL3.ecalDep1  = -1000;
//  theL3.hcalDep1  = -1000;
  
//  for ( std::vector<HLTMuonCand>::const_iterator it = L3cands.begin(); it != L3cands.end(); ++it ) {
//    theDR = deltaR(it -> eta, it -> phi, mu.eta, mu.phi);
//    if (theDR < minDR){
//      minDR = theDR;
//      match = true;
//      theL3 = *it;
//    }
//  }
//  return theL3;
//}


bool matchMuonWithL3(MuonCand mu, std::vector<HLTMuonCand> L3cands){

  bool match = false;
  float minDR = 0.1;
  float theDR = 100;
  for ( std::vector<HLTMuonCand>::const_iterator it = L3cands.begin(); it != L3cands.end(); ++it ) { 
    theDR = deltaR(it -> eta, it -> phi, mu.eta, mu.phi); 
    if (theDR < minDR){ 
      minDR = theDR;
      match = true;
    }
  }
  return match;
}


//L1MuonCand matchL1(MuonCand mu, std::vector<L1MuonCand> L1cands){

//  bool match = false;
//  int nL1 = L1cands.size();

//  float minDR = 0.3;
//  float theDR = 100;
//  L1MuonCand theL1;
//  theL1.pt        = -1000;
//  theL1.eta       = -1000;
//  theL1.phi       = -1000;
  
//  for ( std::vector<L1MuonCand>::const_iterator it = L1cands.begin(); it != L1cands.end(); ++it ) {
//    theDR = deltaR(it -> eta, it -> phi, mu.eta, mu.phi);
//    if (theDR < minDR){
//      minDR = theDR;
//      match = true;
//      theL1 = *it;
//    }
//  }
//  return theL1;
//}

void printProgBar( int percent ){
  std::string bar;  
  for(int i = 0; i < 50; i++){
    if( i < (percent/2)){
      bar.replace(i,1,"=");
    }else if( i == (percent/2)){
      bar.replace(i,1,">");
    }else{
      bar.replace(i,1," ");
    }
  }

  std::cout<< "\r" "[" << bar << "] ";
  std::cout.width( 3 );
  std::cout<< percent << "%     " << std::flush;
}
