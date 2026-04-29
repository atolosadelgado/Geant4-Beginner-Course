#include "YourActionInitialization.hh"
#include "YourPrimaryGeneratorAction.hh"

YourActionInitialization::YourActionInitialization(YourDetectorConstruction * det):G4VUserActionInitialization(),fDetector(det){}

YourActionInitialization::~YourActionInitialization(){}

void YourActionInitialization::Build() const {
  YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction(fDetector);
  SetUserAction(primaryAction);
}
