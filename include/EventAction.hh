#ifndef B4cEventAction_h
#define B4cEventAction_h 1

#include "G4UserEventAction.hh"

#include "CalorHit.hh"

#include "globals.hh"

namespace customSD
{

/// Event action class
///
/// In EndOfEventAction(), it prints the accumulated quantities of the energy
/// deposit and track lengths of charged particles in Absober and Gap layers
/// stored in the hits collections.

class EventAction : public G4UserEventAction
{
public:
  EventAction() = default;
  ~EventAction() override = default;

  void  BeginOfEventAction(const G4Event* event) override;
  void    EndOfEventAction(const G4Event* event) override;

private:
  // methods
  CalorHitsCollection* GetHitsCollection(G4int hcID,
                                            const G4Event* event) const;
  void PrintEventStatistics(G4double absoEdep, G4double absoTrackLength,
                            G4double gapEdep, G4double gapTrackLength) const;

  // data members
  G4int fAbsHCID = -1;
  G4int fGapHCID = -1;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


