    #include "YourPrimaryGeneratorAction.hh"

    #include "YourDetectorConstruction.hh"

    #include "G4ParticleGun.hh"
    #include "G4ParticleTable.hh"
    #include "G4Event.hh"

    YourPrimaryGeneratorAction::YourPrimaryGeneratorAction(YourDetectorConstruction* det):G4VUserPrimaryGeneratorAction(),fDetector(det){
        G4int nPrimaryParticles = 1;
        fGun = new G4ParticleGun(nPrimaryParticles);

        SetDefault();
    }

    YourPrimaryGeneratorAction::~YourPrimaryGeneratorAction(){
        delete fGun;
    }

    void YourPrimaryGeneratorAction::SetDefault(){
        G4ParticleDefinition * part = G4ParticleTable::GetParticleTable()->FindParticle("e-");
        //G4ParticleDefinition * part = G4Electron::Definition();
        fGun->SetParticleDefinition(part);

        G4ThreeVector partDirection(1.,0.,0.);
        fGun->SetParticleMomentumDirection(partDirection);

        G4double partKineticEnergy = 30*CLHEP::MeV;
        fGun->SetParticleEnergy(partKineticEnergy);

        UpdatePosition();
    }

    void YourPrimaryGeneratorAction::UpdatePosition(){
        G4ThreeVector gunPosition(fDetector->GetGunPositionX(),0.,0.);
        fGun->SetParticlePosition(gunPosition);
    }

    void YourPrimaryGeneratorAction::GeneratePrimaries(G4Event *evt) {
        fGun->GeneratePrimaryVertex(evt);
    }
