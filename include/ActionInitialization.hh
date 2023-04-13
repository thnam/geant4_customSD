#ifndef B4cActionInitialization_h
#define B4cActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

namespace customSD
{

/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization() = default;
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;
};

}

#endif


