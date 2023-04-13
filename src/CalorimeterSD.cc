#include "CalorimeterSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4Track.hh"
#include "G4MuonPlus.hh"
#include "G4Positron.hh"
#include "G4VProcess.hh"

namespace customSD
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorimeterSD::CalorimeterSD(const G4String& name,
                             const G4String& hitsCollectionName,
                             G4int nofCells)
 : G4VSensitiveDetector(name),
   fNofCells(nofCells)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection
  fHitsCollection
    = new CalorHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce
  auto hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection );

  // Create hits
  // fNofCells for cells + one more for total sums
  for (G4int i=0; i<fNofCells+1; i++ ) {
    fHitsCollection->insert(new CalorHit());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CalorimeterSD::ProcessHits(G4Step* step,
                                     G4TouchableHistory*)
{
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  if ( edep==0. && stepLength == 0. ) return false;

  auto touchable = (step->GetPreStepPoint()->GetTouchable());

  // Get calorimeter cell id
  auto layerNumber = touchable->GetReplicaNumber(1);

  // Get hit accounting data for this cell
  auto hit = (*fHitsCollection)[layerNumber];
  if ( ! hit ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hit " << layerNumber;
    G4Exception("CalorimeterSD::ProcessHits()",
      "MyCode0004", FatalException, msg);
  }

  // Get hit for total accounting
  auto hitTotal
    = (*fHitsCollection)[fHitsCollection->entries()-1];

  // Add values
  hit->Add(edep, stepLength);
  hitTotal->Add(edep, stepLength);

  // find stopped muons
  // check if this is a mu+ track
  if (step->GetTrack()->GetDefinition() == G4MuonPlus::MuonPlus()){
    // check for stopped track
    if (step->GetTrack()->GetTrackStatus() == fStopButAlive) {
      if ( verboseLevel>=0 ) {
        // print out some useful info about this track
        G4StepPoint *postStep = step->GetPostStepPoint();
        G4cout << "mu+ stopped at " << postStep->GetGlobalTime() / CLHEP::ns << " ns"
          << ", position " << postStep->GetPosition() / CLHEP::cm << " cm"
          << G4endl;
      }
    }

    // auto secondary = step->GetSecondaryInCurrentStep();
    // size_t size_secondary = (*secondary).size();

    // if (size_secondary){
      // for (size_t i=0; i<(size_secondary);i++){
        // auto secstep = (*secondary)[i];

        // G4String secondaryName = secstep->GetDefinition()->GetParticleName();
        // auto secondaryID = secstep->GetTrackID();
        // G4cout << "secondary " << i << ": " << secondaryName
          // << ", energy: " << secstep->GetKineticEnergy() / CLHEP::MeV << " MeV"
          // << ", time: " << secstep->GetGlobalTime() / CLHEP::ns << " ns"
          // << G4endl;
      // }
    // }
  }

  // find decay e+
  if (step->GetTrack()->GetDefinition() == G4Positron::Positron()){
    // check if its mother is the primary particle (id 1) and if this is the
    // first step of this particle

    if (step->GetTrack()->GetParentID() == 1 && step->IsFirstStepInVolume()){
      // && step->GetTrack()->GetCurrentStepNumber() == 0) {
      G4StepPoint* postPoint = step->GetPostStepPoint();
      G4StepPoint* prePoint=step->GetPreStepPoint();
      const G4VProcess *post_step=postPoint->GetProcessDefinedStep();
      const G4VProcess *pre_step=prePoint->GetProcessDefinedStep();
      const G4String& processName_pre = pre_step ? pre_step->GetProcessName() : "";
      const G4String& processName_post = post_step ? post_step->GetProcessName() : "";
      if (processName_pre == "") {
        if ( verboseLevel>=0 ) {
          // G4cout << "pre: " << processName_pre << ", post: " << processName_post << G4endl;
          // print out some useful info about this track
          G4StepPoint *postStep = step->GetPreStepPoint();
          G4cout << "e+ born at " << postStep->GetGlobalTime() / CLHEP::ns << " ns"
            << ", position " << postStep->GetPosition() / CLHEP::cm << " cm"
            << ", kinetic energy: " << postStep->GetKineticEnergy() / CLHEP::MeV << " MeV"
            << G4endl;
        }
        
      }

    }
  }


  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorimeterSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) {
     auto nofHits = fHitsCollection->entries();
     G4cout
       << G4endl
       << "-------->Hits Collection: in this event they are " << nofHits
       << " hits in the tracker chambers: " << G4endl;
     for ( std::size_t i=0; i<nofHits; ++i ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
