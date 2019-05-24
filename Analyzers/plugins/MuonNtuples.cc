/** \class MuonNtuples
 */
      
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/Luminosity/interface/LumiDetails.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateIsolation.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Scalers/interface/LumiScalers.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "HLTrigger/HLTcore/interface/HLTEventAnalyzerAOD.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoMuon/TrackingTools/interface/MuonServiceProxy.h"
#include <map>
#include <string>
#include <iomanip>
#include "TTree.h"
#include "MuonHLTNtuples/Analyzers/src/MuonTree.h"
#include "DataFormats/MuonReco/interface/MuonTrackLinks.h"
#include "DataFormats/TrackerRecHit2D/interface/BaseTrackerRecHit.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit1D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/OmniClusterRef.h"
class MuonServiceProxy;

enum HLTCollectionType { 
  iL2muons=0,
  iL3muons,
  itkmuons,
  iL3OImuons,
  iL3IOmuons,
};


//*********INCLUDED*********// 
enum TrackCollectionType {
  ihltTrackOI=0,
  ihltTrackIOL1,
  ihltTrackIOL2,
};
//**************************//


const double NOMATCH = 999.;
const double NOMATCHITS =  0.;



class MuonNtuples : public edm::EDAnalyzer {

 public:
  MuonNtuples(const edm::ParameterSet& cfg);
  virtual ~MuonNtuples() {};

  virtual void analyze (const edm::Event& event, const edm::EventSetup & eventSetup);
  virtual void beginJob();
  virtual void endJob();
  virtual void beginEvent();
  virtual void beginRun(const edm::Run & run,    const edm::EventSetup & eventSetup);
  virtual void endRun  (const edm::Run & run,    const edm::EventSetup & eventSetup);


 private:

  void fillHlt(const edm::Handle<edm::TriggerResults> &, 
               const edm::Handle<trigger::TriggerEvent> &,
               const edm::TriggerNames &,
               const edm::Event &,
               bool 
              );
  
  void fillMuons(const edm::Handle<reco::MuonCollection> &,
                  trigger::TriggerObjectCollection & hltl3muons,
                 const edm::Handle<reco::RecoChargedCandidateCollection> & l3cands,
                 const edm::Handle<reco::MuonTrackLinksCollection>& links,
                 const reco::Vertex &, 
                 const edm::Event   & 
		 );
  //*****************************INCLUDED*****************************//
  void fillHltTrack(const edm::Handle<reco::TrackCollection>  &,
		    const edm::Event    &,
                    TrackCollectionType type
		    );
  //******************************************************************//


  void fillHltMuons(const edm::Handle<reco::RecoChargedCandidateCollection> &,
                    const edm::Event   &,
		    HLTCollectionType type
                   );

  void fillL1Muons(const edm::Handle<l1t::MuonBxCollection> &,
                    const edm::Event   &
                   );

  void MonteCarloStudies(const edm::Event&);
  

//   virtual void endEvent();

  edm::InputTag offlinePVTag_;
  edm::EDGetTokenT<reco::VertexCollection> offlinePVToken_;
  edm::InputTag offlineMuonTag_;
  edm::EDGetTokenT<std::vector<reco::Muon>> offlineMuonToken_;
  /// file service
  edm::Service<TFileService> outfile_;

//*****************************INCLUDED*****************************//
  edm::InputTag theTrackOITag_;
  edm::EDGetTokenT<reco::TrackCollection> theTrackOIToken_;
  edm::InputTag theTrackIOL2Tag_;
  edm::EDGetTokenT<reco::TrackCollection> theTrackIOL2Token_;
  edm::InputTag theTrackIOL1Tag_;
  edm::EDGetTokenT<reco::TrackCollection> theTrackIOL1Token_;
//******************************************************************//


  // Trigger process
  edm::InputTag triggerResultTag_;
  edm::EDGetTokenT<edm::TriggerResults>   triggerResultToken_;
  edm::InputTag triggerSummTag_;
  edm::EDGetTokenT<trigger::TriggerEvent> triggerSummToken_;
  edm::InputTag tagTriggerResultTag_;
  edm::EDGetTokenT<edm::TriggerResults>   tagTriggerResultToken_;
  edm::InputTag tagTriggerSummTag_;
  edm::EDGetTokenT<trigger::TriggerEvent> tagTriggerSummToken_;

  // Input tags
  edm::InputTag l3candTag_;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> l3candToken_; 
  edm::InputTag l2candTag_;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> l2candToken_; 
  edm::InputTag l1candTag_;
  edm::EDGetTokenT<l1t::MuonBxCollection> l1candToken_; 
  edm::InputTag tkMucandTag_;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> tkMucandToken_; 
  edm::InputTag l3OIcandTag_;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> l3OIcandToken_; 
  edm::InputTag l3IOcandTag_;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> l3IOcandToken_; 

  edm::EDGetTokenT<reco::MuonTrackLinksCollection>  theLinksToken_;
  edm::EDGetTokenT<reco::TrackExtraCollection>  theMuonsWithHitsToken_;
 


  //trigger process
  std::string triggerProcess_;
  std::string triggerName_;
  std::string l1filterLabel_;
  std::string l2filterLabel_;
  std::string l3filterLabel_;



  edm::InputTag lumiScalerTag_;
  edm::EDGetTokenT<LumiScalersCollection> lumiScalerToken_;

  edm::InputTag puTag_;
  edm::EDGetTokenT<std::vector< PileupSummaryInfo>> puToken_;

  edm::InputTag genTag_;
  edm::EDGetTokenT<reco::GenParticleCollection> genToken_;




  bool doOffline_;

  MuonEvent event_;
  std::map<std::string,TTree*> tree_;
  
  unsigned int nGoodVtx; 
  MuonServiceProxy *theService;

};

/// default constructor
MuonNtuples::MuonNtuples(const edm::ParameterSet& cfg): 
  offlinePVTag_           (cfg.getParameter<edm::InputTag>("offlineVtx")), 
    offlinePVToken_         (consumes<reco::VertexCollection>(offlinePVTag_)), 
  offlineMuonTag_         (cfg.getParameter<edm::InputTag>("offlineMuons")), 
    offlineMuonToken_       (consumes<std::vector<reco::Muon>>(offlineMuonTag_)), 

//********************************************INCLUDED********************************************//
  theTrackOITag_          (cfg.getUntrackedParameter<edm::InputTag>("theTrackOI")),
    theTrackOIToken_        (consumes<reco::TrackCollection>(theTrackOITag_)),
  theTrackIOL2Tag_          (cfg.getUntrackedParameter<edm::InputTag>("theTrackIOL2")),
    theTrackIOL2Token_        (consumes<reco::TrackCollection>(theTrackIOL2Tag_)),
  theTrackIOL1Tag_          (cfg.getUntrackedParameter<edm::InputTag>("theTrackIOL1")),
    theTrackIOL1Token_        (consumes<reco::TrackCollection>(theTrackIOL1Tag_)),
//************************************************************************************************//


  triggerResultTag_       (cfg.getUntrackedParameter<edm::InputTag>("triggerResult")), 
    triggerResultToken_     (consumes<edm::TriggerResults>(triggerResultTag_)),
  triggerSummTag_         (cfg.getUntrackedParameter<edm::InputTag>("triggerSummary")), 
    triggerSummToken_       (consumes<trigger::TriggerEvent>(triggerSummTag_)),
   //l3filterLabel_          (cfg.getParameter<std::string>("l3filterLabel")),

  tagTriggerResultTag_    (cfg.getUntrackedParameter<edm::InputTag>("tagTriggerResult")), 
    tagTriggerResultToken_  (consumes<edm::TriggerResults>(tagTriggerResultTag_)),
  tagTriggerSummTag_      (cfg.getUntrackedParameter<edm::InputTag>("tagTriggerSummary")), 
    tagTriggerSummToken_    (consumes<trigger::TriggerEvent>(tagTriggerSummTag_)),
  // triggerProcess_         (cfg.getParameter<std::string>("triggerProcess")),

  l3candTag_              (cfg.getUntrackedParameter<edm::InputTag>("L3Candidates")),
    l3candToken_            (consumes<reco::RecoChargedCandidateCollection>(l3candTag_)),
  l2candTag_              (cfg.getUntrackedParameter<edm::InputTag>("L2Candidates")),
    l2candToken_            (consumes<reco::RecoChargedCandidateCollection>(l2candTag_)),
  l1candTag_              (cfg.getUntrackedParameter<edm::InputTag>("L1Candidates")),
    l1candToken_            (consumes<l1t::MuonBxCollection>(l1candTag_)),
  tkMucandTag_            (cfg.getUntrackedParameter<edm::InputTag>("TkMuCandidates")),
    tkMucandToken_          (consumes<reco::RecoChargedCandidateCollection>(tkMucandTag_)),
  l3OIcandTag_            (cfg.getUntrackedParameter<edm::InputTag>("L3OIMuCandidates")),
    l3OIcandToken_          (consumes<reco::RecoChargedCandidateCollection>(l3OIcandTag_)),
  l3IOcandTag_          (cfg.getUntrackedParameter<edm::InputTag>("L3IOMuCandidates")),
    l3IOcandToken_        (consumes<reco::RecoChargedCandidateCollection>(l3IOcandTag_)),

  theLinksToken_          (consumes<reco::MuonTrackLinksCollection>(cfg.getUntrackedParameter<edm::InputTag>("MuonLinksTag"))),
  theMuonsWithHitsToken_  (consumes<reco::TrackExtraCollection>(edm::InputTag("globalMuons"))),


  lumiScalerTag_          (cfg.getUntrackedParameter<edm::InputTag>("lumiScalerTag")),
    lumiScalerToken_        (consumes<LumiScalersCollection>(lumiScalerTag_)), 
  puTag_                  (cfg.getUntrackedParameter<edm::InputTag>("puInfoTag")),
    puToken_                (consumes<std::vector< PileupSummaryInfo>>(puTag_)), 

  genTag_                 (cfg.getUntrackedParameter<edm::InputTag>("genParticlesTag")),
    genToken_               (consumes<reco::GenParticleCollection>(genTag_)), 

  doOffline_                 (cfg.getUntrackedParameter<bool>("doOffline"))
{

  theService = new MuonServiceProxy(cfg.getParameter<edm::ParameterSet>("ServiceParameters"));
  //usesResource("TFileService");


}

void MuonNtuples::beginJob() {

  TH1::SetDefaultSumw2() ;
  tree_["muonTree"] = outfile_-> make<TTree>("muonTree","muonTree");
  tree_["muonTree"] -> Branch("event" ,&event_, 64000,2);

}    

void MuonNtuples::endJob() {}

void MuonNtuples::beginRun(const edm::Run & run, const edm::EventSetup & eventSetup) {}

void MuonNtuples::endRun  (const edm::Run & run, const edm::EventSetup & eventSetup) {}
 
void MuonNtuples::analyze (const edm::Event &event, const edm::EventSetup &eventSetup) {

  theService->update(eventSetup);


  beginEvent();

  // Fill general info
  event_.runNumber             = event.id().run();
  event_.luminosityBlockNumber = event.id().luminosityBlock();
  event_.eventNumber           = event.id().event();


  // Fill vertex info
  //if (doOffline_){
    edm::Handle<reco::VertexCollection> vertices; 
    event.getByToken(offlinePVToken_, vertices);
    for(reco::VertexCollection::const_iterator it = vertices->begin(); it != vertices->end(); ++it) {
      if( !it->isValid())  continue;
      nGoodVtx++;
    }
    event_.nVtx = nGoodVtx;
    const reco::Vertex           & pv      = vertices->at(0);

  // Handle the offline muon collection and fill offline muons
    edm::Handle<std::vector<reco::Muon> > muons;
    event.getByToken(offlineMuonToken_, muons);
    //fillMuons(muons, pv, event);

    // Fill bx and inst lumi info
    if (event.isRealData()) {
      event_.bxId  = event.bunchCrossing();

      if (lumiScalerTag_.label() != "none")
      {
        edm::Handle<LumiScalersCollection> lumiScaler;
        event.getByToken(lumiScalerToken_, lumiScaler);

        if (lumiScaler->begin() != lumiScaler->end())
          event_.instLumi = lumiScaler->begin()->instantLumi();
      } 
    }
 // }


  // Fill PU info
  if (!event.isRealData()) {
    edm::Handle<std::vector< PileupSummaryInfo > > puInfo;
    if ( event.getByToken(puToken_,puInfo)){
      std::vector<PileupSummaryInfo>::const_iterator PVI;
      for(PVI = puInfo->begin(); PVI != puInfo->end(); ++PVI) 
      {
        if(PVI->getBunchCrossing()==0){
          event_.trueNI   = PVI->getTrueNumInteractions();
          continue;
        }
      }
    } 
    else  
      edm::LogError("") << "PU collection not found !!!";
  }

  
  // Fill MC GEN info
  if (!event.isRealData()) 
    MonteCarloStudies(event);


  //Track Outside-In
  edm::Handle<reco::TrackCollection> trackOI;
  event.getByToken(theTrackOIToken_, trackOI);
  
  if (event.getByToken(theTrackOIToken_, trackOI))
    fillHltTrack(trackOI, event, TrackCollectionType::ihltTrackOI);
  
  //Track Inside Out from L2
  edm::Handle<reco::TrackCollection> trackIOL2;
  if (event.getByToken(theTrackIOL2Token_, trackIOL2))
    fillHltTrack(trackIOL2, event, TrackCollectionType::ihltTrackIOL2);
  
  //Track Outside In from L1
  edm::Handle<reco::TrackCollection> trackIOL1;
  if (event.getByToken(theTrackIOL1Token_, trackIOL1))
    fillHltTrack(trackIOL1, event, TrackCollectionType::ihltTrackIOL1);
  
  
  // Fill trigger information for probe muon
  edm::Handle<edm::TriggerResults>   triggerResults;
  edm::Handle<trigger::TriggerEvent> triggerEvent;

  //if(triggerResults.isValid())std::cout<<"Trigger resultsValid"<<std::endl;
 // if(triggerEvent.isValid())std::cout<<"Trigger Event Valid"<<std::endl;
  //std::cout<<"triggerResultToken_: "<<triggerResultToken_<<std::endl;


  if (event.getByToken(triggerResultToken_, triggerResults) &&
      event.getByToken(triggerSummToken_  , triggerEvent)) {
      
    edm::TriggerNames triggerNames_ = event.triggerNames(*triggerResults);
    fillHlt(triggerResults, triggerEvent, triggerNames_, event, false);
  }
  else 
    edm::LogError("") << "Trigger collection for probe muon not found !!!";

  // Fill trigger information for tag muon
  edm::Handle<edm::TriggerResults>   tagTriggerResults;
  edm::Handle<trigger::TriggerEvent> tagTriggerEvent;
      
  if (event.getByToken(tagTriggerResultToken_, tagTriggerResults) &&
      event.getByToken(tagTriggerSummToken_  , tagTriggerEvent)) {
      
    edm::TriggerNames tagTriggerNames_ = event.triggerNames(*tagTriggerResults);
    fillHlt(tagTriggerResults, tagTriggerEvent, tagTriggerNames_, event, true);
  }
  else 
    edm::LogError("") << "Trigger collection for tag muon not found !!!";

 // Handle the online muon collection and fill online muons //the hltmuons branch
  edm::Handle<reco::RecoChargedCandidateCollection> l3cands;
  if (event.getByToken(l3candToken_, l3cands))
    fillHltMuons(l3cands, event, HLTCollectionType::iL3muons);



  // Handle the online muon collection and fill L2 muons //the l2muosn branch
  edm::Handle<reco::RecoChargedCandidateCollection> l2cands;
  if (event.getByToken(l2candToken_, l2cands))
    fillHltMuons(l2cands, event, HLTCollectionType::iL2muons);
//  else
//    edm::LogWarning("") << "Online L2 muon collection not found !!!";

  // Handle the online muon collection and fill L1 muons
  edm::Handle<l1t::MuonBxCollection> l1cands;
  if (event.getByToken(l1candToken_, l1cands))
    fillL1Muons(l1cands, event);
//  else
//    edm::LogWarning("") << "Online L1 muon collection not found !!!";
  
  // Handle the 2nd online muon collection and fill online muons
  edm::Handle<reco::RecoChargedCandidateCollection> tkMucands;
  if (event.getByToken(tkMucandToken_, tkMucands))
    fillHltMuons(tkMucands, event, HLTCollectionType::itkmuons);
//  else
//    edm::LogWarning("") << "Online tracker muon collection not found !!!";
//

  // Handle the online muon collection and fill online muons //the hltmuons branch
  edm::Handle<reco::RecoChargedCandidateCollection> l3OIcands;
  if (event.getByToken(l3OIcandToken_, l3OIcands))
    fillHltMuons(l3OIcands, event, HLTCollectionType::iL3OImuons);

   //if (l3OIcands.isValid())std::cout<<"OI cand size: "<<l3OIcands->size()<<std::endl;
//  else
//    edm::LogWarning("") << "Online OI muon collection not found !!!"; 

  edm::Handle<reco::RecoChargedCandidateCollection> l3IOcands;
  if (event.getByToken(l3IOcandToken_, l3IOcands))
    fillHltMuons(l3IOcands, event, HLTCollectionType::iL3IOmuons);
//  else
//    edm::LogWarning("") << "Online IO muon collection not found !!!"; 

  edm::Handle<reco::MuonTrackLinksCollection> links;
  event.getByToken(theLinksToken_, links);

  edm::Handle<reco::TrackExtraCollection> trackMuons;
  event.getByToken(theMuonsWithHitsToken_, trackMuons);

  edm::Handle<trigger::TriggerEvent> triggerSummary;
  event.getByToken(triggerSummToken_, triggerSummary);


  edm::Handle<reco::MuonCollection> MuonCol;
  event.getByToken(offlineMuonToken_, MuonCol);  



  reco::MuonCollection Muons;
  for(auto const& mu : *MuonCol) Muons.push_back(mu);

  reco::TrackExtraCollection trackerMuons;
  for(auto const& tkmu : *trackMuons) trackerMuons.push_back(tkmu);

  size_t L3MuonFilterIndex = (*triggerSummary).filterIndex(edm::InputTag("Mu","","TEST"));//(edm::InputTag( "hltL3fL1sMu22Or25L1f0L2f10QL3Filtered27Q","","TEST"));
  
  trigger::TriggerObjectCollection L3MuonTrigObjects;
  trigger::TriggerObjectCollection allTriggerObjects = triggerSummary->getObjects();


     if(L3MuonFilterIndex < (*triggerSummary).sizeFilters()) { //check if the trigger object is present
    const trigger::Keys &keysMuons = (*triggerSummary).filterKeys(L3MuonFilterIndex);
    for(size_t j = 0; j < keysMuons.size(); j++) {
      trigger::TriggerObject foundObject = (allTriggerObjects)[keysMuons[j]];
      L3MuonTrigObjects.push_back(foundObject);
    }
  }



  try{
     //std::cout<<"total muons in the event: "<<muons->size()<<std::endl;
     fillMuons(muons,L3MuonTrigObjects,l3cands,links, pv, event);
     }
   catch(...){}




  // endEvent();
  tree_["muonTree"] -> Fill();
}



//------------------------------------------------------------------------
void MuonNtuples::MonteCarloStudies(const edm::Event& event)
{
  edm::Handle<reco::GenParticleCollection> genParticles;
  event.getByToken(genToken_, genParticles);
  int muId  =    13;

  for ( size_t i=0; i< genParticles->size(); ++i) 
  { 
    const reco::GenParticle &p = (*genParticles)[i];
    // only save muons
    if(fabs(p.pdgId()) != muId )     continue; 
      
    GenParticleCand theGen;
    theGen.pdgId  = p.pdgId();
    theGen.pt     = p.pt() ;
    theGen.eta    = p.eta();
    theGen.phi    = p.phi();
    theGen.energy = p.energy();
    theGen.status = p.status();
    
    unsigned int n_moms = p.numberOfMothers();
    if (n_moms == 0 ){
      theGen.pdgMother.push_back(0);
      theGen.pdgRealMother.push_back(0);
    }
    else {
      for (unsigned int im=0; im < n_moms; ++im){
        theGen.pdgMother.push_back(p.motherRef(im)->pdgId());
        // if coming from a muon, go back one step ** to be improved **
        if(n_moms == 1 && fabs(p.motherRef(0)->pdgId()) == muId){
          for (unsigned int igm = 0; igm < p.motherRef(0)->numberOfMothers(); igm++){
            theGen.pdgRealMother.push_back(p.motherRef(0)->motherRef(igm)->pdgId());
          }
        }
        else
          theGen.pdgRealMother.push_back(0);
      }
    }

    event_.genParticles.push_back(theGen);

  }  // end for genParticles
}



// --------------------------------------------------------------------
void MuonNtuples::fillHlt(const edm::Handle<edm::TriggerResults>   & triggerResults, 
                          const edm::Handle<trigger::TriggerEvent> & triggerEvent  ,
                          const edm::TriggerNames                  & triggerNames  ,
                          const edm::Event                         & event         ,
                          bool                                       isTag         )
{    
   
  for (unsigned int itrig=0; itrig < triggerNames.size(); ++itrig) 
  {
    LogDebug ("triggers") << triggerNames.triggerName(itrig) ;
    if (triggerResults->accept(itrig)) 
    {
      std::string pathName = triggerNames.triggerName(itrig);
      
//      if ( pathName.find ("HLT_IsoMu"  ) !=std::string::npos ||
//           pathName.find ("HLT_Mu"     ) !=std::string::npos ||
//           pathName.find ("HLT_Mu5"    ) !=std::string::npos ||
//           pathName.find ("HLT_TkMu5"  ) !=std::string::npos ||
//           pathName.find ("HLT_IsoTkMu") !=std::string::npos ||
//           pathName.find ("HLT_Mu17"   ) !=std::string::npos ||
//           pathName.find ("HLT_Mu8_T"  ) !=std::string::npos 
//      ){
      if (isTag) event_.hltTag.triggers.push_back(pathName);
      else       event_.hlt   .triggers.push_back(pathName);
//      }
    }
  }
     
     
  const trigger::size_type nFilters(triggerEvent->sizeFilters());
  for (trigger::size_type iFilter=0; iFilter!=nFilters; ++iFilter) 
  {
    std::string filterTag = triggerEvent->filterTag(iFilter).encode();

    if ( ( filterTag.find ("Mu"     ) !=std::string::npos)
	 && 
	 filterTag.find ("Tau"       ) ==std::string::npos   &&
	 filterTag.find ("EG"        ) ==std::string::npos   &&
	 filterTag.find ("MultiFit"  ) ==std::string::npos  
	 )   
    {
      trigger::Keys objectKeys = triggerEvent->filterKeys(iFilter);
      const trigger::TriggerObjectCollection& triggerObjects(triggerEvent->getObjects());


      
      for (trigger::size_type iKey=0; iKey<objectKeys.size(); ++iKey) 
      {  
        trigger::size_type objKey = objectKeys.at(iKey);
        const trigger::TriggerObject& triggerObj(triggerObjects[objKey]);
        
        HLTObjCand hltObj;
        
        hltObj.filterTag = filterTag;
  
        hltObj.pt  = triggerObj.pt();
        hltObj.eta = triggerObj.eta();
        hltObj.phi = triggerObj.phi();
        
        if (isTag)       event_.hltTag.objects.push_back(hltObj);
        else             event_.hlt   .objects.push_back(hltObj);
      }  
    }       
  }
}


// ---------------------------------------------------------------------
//**********************************************INCLUDED*********************************************//
//Incluir hltTrack copiar collection dentro de muons en innertrack

void MuonNtuples::fillHltTrack(const edm::Handle<reco::TrackCollection>  & trackm ,
			       const edm::Event                          & tevent ,
			       TrackCollectionType type)
{
  int t_mu=0;
  for(reco::TrackCollection::const_iterator t=trackm->begin(); t!=trackm->end(); t++){

    t_mu++;
    HltTrackCand MuTrack;

    MuTrack.pt   = t -> pt() ;
    MuTrack.eta  = t -> eta();
    MuTrack.phi  = t -> phi();

    MuTrack.chi2           = t  ->  normalizedChi2();
    MuTrack.dxy            = t  ->  dxy();
    MuTrack.dz             = t  ->  dz() ;
    MuTrack.pixelHits      = t  ->  hitPattern().numberOfValidPixelHits();
    MuTrack.validHits      = t  ->  numberOfValidHits();

    MuTrack.layerHits         = t -> hitPattern().trackerLayersWithMeasurement(); 
    MuTrack.pixelLayers       = t -> hitPattern().pixelLayersWithMeasurement(); 
    MuTrack.fracValidTrackhit = t -> validFraction();  
  
    if (type == TrackCollectionType::ihltTrackOI)   {event_.hltTrackOI.push_back(MuTrack)  ;  continue; }
    if (type == TrackCollectionType::ihltTrackIOL1) {event_.hltTrackIOL1.push_back(MuTrack);  continue; }
    if (type == TrackCollectionType::ihltTrackIOL2) {event_.hltTrackIOL2.push_back(MuTrack);  continue; }

  }
}
//****************************************************************************************************//

// ---------------------------------------------------------------------
void MuonNtuples::fillMuons(const edm::Handle<reco::MuonCollection>       & muons ,
                            trigger::TriggerObjectCollection & hltl3muons,
                            const edm::Handle<reco::RecoChargedCandidateCollection> & l3cands,
                            const edm::Handle<reco::MuonTrackLinksCollection>& links,
                            const reco::Vertex                            & pv    ,
                            const edm::Event                              & event )
{


  


  int n_mu = 0;
  for(std::vector<reco::Muon>::const_iterator mu1=muons->begin(); mu1!=muons->end(); ++mu1) 
  { 

    n_mu++;
    MuonCand theMu;

    theMu.pt      = mu1 -> pt();
    theMu.eta     = mu1 -> eta();
    theMu.phi     = mu1 -> phi();
    theMu.charge  = mu1 -> charge();

    mu1 -> isGlobalMuon  () ? theMu.isGlobal  = 1 : theMu.isGlobal   = 0;
    mu1 -> isTrackerMuon () ? theMu.isTracker = 1 : theMu.isTracker  = 0;
    mu1 -> isPFMuon()       ? theMu.isPFMuon  = 1 : theMu.isPFMuon   = 0;

    muon::isTightMuon ( (*mu1), pv ) ? theMu.isTight  = 1 : theMu.isTight  = 0;
    muon::isLooseMuon ( (*mu1)     ) ? theMu.isLoose  = 1 : theMu.isLoose  = 0;
    muon::isMediumMuon( (*mu1)     ) ? theMu.isMedium = 1 : theMu.isMedium = 0;

 

    //New variables in the Ntuple 

    if (mu1 -> globalTrack().isNonnull()){
      theMu.chi2 = mu1 -> globalTrack() -> normalizedChi2(); //normalized global-track chi2
      theMu.validHits      = mu1 -> globalTrack() -> hitPattern().numberOfValidHits(); //number of muon-chamber  hit included in the global muon track
    } 


    if (mu1 -> innerTrack().isNonnull()){
      theMu.innerpt           = mu1 -> innerTrack() -> pt();
      theMu.innereta          = mu1 -> innerTrack() -> eta();
      theMu.innerphi          = mu1 -> innerTrack() -> phi();
      theMu.innervalidHits    = mu1 -> innerTrack() -> numberOfValidHits();
      theMu.innerchi2         = mu1 -> innerTrack() -> normalizedChi2();
      theMu.innerdxy          = mu1 -> innerTrack() -> dxy();
      theMu.innerdz           = mu1 -> innerTrack() -> dz();
      theMu.innerpixelHits    = mu1 -> innerTrack() -> hitPattern().numberOfValidPixelHits()      ; //number of pixel hits
      theMu.innerlayerHits    = mu1 -> innerTrack() -> hitPattern().trackerLayersWithMeasurement(); //hits per layer
      theMu.innerpixelLayers  = mu1 -> innerTrack() -> hitPattern().pixelLayersWithMeasurement()  ; //number of pixel layers
      theMu.innerfracValidTrackhit = mu1 -> innerTrack() -> validFraction();  //fraction of valid tracker hits 
    }

    theMu.dxy               = mu1 -> muonBestTrack() -> dxy();//transverse distance of the tracker track wrt primary vertex
    theMu.dz                = mu1 -> muonBestTrack() -> dz() ; //longitudinal distance of the tracker track wrt primary vertex
    theMu.chi2LocalPosition = mu1 -> combinedQuality().chi2LocalPosition; //tracker standalone position match
    theMu.kickFinder        = mu1 -> combinedQuality().trkKink; //kick finder
    theMu.matchedStations   = mu1 -> numberOfMatchedStations(); //number of muons stations with segment


    theMu.chargedDep_dR03 = mu1->pfIsolationR03().sumChargedHadronPt ;
    theMu.neutralDep_dR03 = mu1->pfIsolationR03().sumNeutralHadronEt ;
    theMu.photonDep_dR03  = mu1->pfIsolationR03().sumPhotonEt        ;
    theMu.puPt_dR03       = mu1->pfIsolationR03().sumPUPt            ;
    theMu.chargedDep_dR04 = mu1->pfIsolationR04().sumChargedHadronPt ;
    theMu.neutralDep_dR04 = mu1->pfIsolationR04().sumNeutralHadronEt ;
    theMu.photonDep_dR04  = mu1->pfIsolationR04().sumPhotonEt        ;
    theMu.puPt_dR04       = mu1->pfIsolationR04().sumPUPt            ;

    //float offlineiso04 = theMu.chargedDep_dR04 + std::max(0., theMu.photonDep_dR04 + theMu.neutralDep_dR04 - 0.5*theMu.puPt_dR04);
    //float Relofflineiso04       = float(offlineiso04) / float(theMu.pt);
    //if(theMu.isTight !=1 || Relofflineiso04 >0.15)continue;

    //std::cout<<"For offline muon with pt, eta, phi: "<<mu1 -> pt()<<", "<<mu1 -> eta()<<", "<<mu1 -> phi()<<std::endl; 




    float L3pt=-1.0;
    float L3eta=-1.0;
    float L3phi=-1.0;
    float delRmuL3=-1.0;
    float sharedFrac=-1.0;
    float sharedFracPixel=-1.0;
    float sharedFracStrip=-1.0;
   
    theMu.sharedFrac=0.5;//matchedL3hitFrac(mu1,l3cands,links,L3pt,L3eta,L3phi);
    theMu.L3pt=L3pt;
    theMu.L3eta=L3eta;
    theMu.L3phi=L3phi;
    float deta = mu1 -> eta() - L3eta;
    float dphi = std::abs(mu1 -> phi()-L3phi); if (dphi>3.14) dphi-=(2*3.14); 
    float DeltaRv=sqrt((deta*deta)+(dphi*dphi));
    delRmuL3=DeltaRv;
    theMu.delRmuL3=delRmuL3;

    float dummypt=0.;
    float dummyeta=0.;
    float dummyphi=0.;  

 
    theMu.sharedFracPixel=0.5;//matchedL3hitFracPixel(mu1,l3cands,links,dummypt,dummyeta,dummyphi);
    theMu.sharedFracStrip=0.5;//matchedL3hitFracStrip(mu1,l3cands,links,dummypt,dummyeta,dummyphi);
  





    event_.muons.push_back(theMu);
  }
}



// ---------------------------------------------------------------------
void MuonNtuples::fillHltMuons(const edm::Handle<reco::RecoChargedCandidateCollection> & l3cands , //candidates to HLT 
                               const edm::Event                                        & event   , 
			       HLTCollectionType type
                               )
{

  for( unsigned int il3 = 0; il3 < l3cands->size(); ++il3) {

    HLTMuonCand theL3Mu;

    reco::RecoChargedCandidateRef candref(l3cands, il3);
    theL3Mu.pt      = candref -> pt();
    theL3Mu.eta     = candref -> eta();
    theL3Mu.phi     = candref -> phi();
    theL3Mu.charge  = candref -> charge();


    reco::TrackRef trkmu = candref->track();
    theL3Mu.trkpt   = trkmu -> pt();
    if (type == HLTCollectionType::iL3muons)     { event_.hltmuons   .push_back(theL3Mu);  continue; }
    if (type == HLTCollectionType::iL3OImuons)   { event_.hltOImuons .push_back(theL3Mu);  continue; }
    if (type == HLTCollectionType::iL3IOmuons)   { event_.hltIOmuons .push_back(theL3Mu);  continue; }
    if (type == HLTCollectionType::itkmuons)     { event_.tkmuons    .push_back(theL3Mu);  continue; }
    if (type == HLTCollectionType::iL2muons)     { event_.L2muons    .push_back(theL3Mu);  continue; }
  }
}



// ---------------------------------------------------------------------
void MuonNtuples::fillL1Muons(const edm::Handle<l1t::MuonBxCollection> & l1cands ,
                              const edm::Event                         & event    
                              )
{

  for (int ibx = l1cands->getFirstBX(); ibx <= l1cands->getLastBX(); ++ibx) {
    if (ibx != 0) continue;
    for (auto it = l1cands->begin(ibx); it != l1cands->end(ibx); it++){

      l1t::MuonRef muon(l1cands, distance(l1cands->begin(l1cands->getFirstBX()),it) );

      L1MuonCand theL1Mu;

      theL1Mu.pt       = muon -> pt();
      theL1Mu.eta      = muon -> eta();
      theL1Mu.phi      = muon -> phi();
      theL1Mu.charge   = muon -> charge();
      theL1Mu.quality  = muon -> hwQual();

      event_.L1muons.push_back(theL1Mu);
    }
  }
}




//---------------------------------------------
void MuonNtuples::beginEvent()
{

  event_.hlt.triggers.clear();
  event_.hlt.objects.clear();


  event_.hltTag.triggers.clear();
  event_.hltTag.objects.clear();


  event_.genParticles.clear();

//*******************INCLUDED*******************//
  event_.hltTrackOI.clear();
  event_.hltTrackIOL1.clear();
  event_.hltTrackIOL2.clear();
//**********************************************//
  event_.muons.clear();
  event_.hltmuons.clear();
  event_.L2muons.clear();
  event_.L2muonsTSG.clear();
  event_.L1muons.clear();
  event_.tkmuons.clear();
  event_.hltOImuons.clear();
  event_.hltIOmuons.clear();

  for (unsigned int ix=0; ix<3; ++ix) {
    event_.primaryVertex[ix] = 0.;
    for (unsigned int iy=0; iy<3; ++iy) {
      event_.cov_primaryVertex[ix][iy] = 0.;
    }
  }
  event_.nVtx       = -1;
  event_.trueNI     = -1;
 
  event_.bxId       = -1;
  event_.instLumi   = -1;
  
  nGoodVtx = 0; 
}





// define this as a plug-in
DEFINE_FWK_MODULE(MuonNtuples);
