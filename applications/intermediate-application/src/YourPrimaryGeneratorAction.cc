
#include "YourPrimaryGeneratorAction.hh"

#include "YourDetectorConstruction.hh"

#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"

YourPrimaryGeneratorAction::YourPrimaryGeneratorAction(YourDetectorConstruction* det)
:   G4VUserPrimaryGeneratorAction(),
    fYourDetector(det),
    fParticleGun(nullptr) {
    // create the particle-gun object
    G4int nParticle = 1;
    fParticleGun    = new G4ParticleGun(nParticle);
    SetDefaultKinematic();
}


YourPrimaryGeneratorAction::~YourPrimaryGeneratorAction() {
    delete fParticleGun;
}


void YourPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt) {
  // fParticleGun->GeneratePrimaryVertex(evt);
  G4PrimaryVertex* vertex = new G4PrimaryVertex(G4ThreeVector(0,0,0), 0*CLHEP::s);
  // lets create an electron with 10GeV mass...
  G4ThreeVector pmom(1*CLHEP::MeV,0,0);
  G4double      ene (10000*CLHEP::MeV);
  G4ParticleDefinition* partdef = G4ParticleTable::GetParticleTable()->FindParticle( "e-" );
  G4PrimaryParticle* particle = new G4PrimaryParticle(partdef, pmom.x(), pmom.y(), pmom.z(), ene);

    // G4PrimaryParticle::Set4Momentum does not protect against inconsiscy of mass value, PDF vs derived value as:
    //     mass2 = E * E - pmom * pmom;
    // leading to potential breaks, for example, if unstable particle has not enough mass to decay and produce daughters,
    // a fatal exception is thrown by DecayIt process
    // void G4PrimaryParticle::Set4Momentum(G4double px, G4double py, G4double pz, G4double E)
    // {
    //   G4double pmom = std::sqrt(px * px + py * py + pz * pz);
    //   if (pmom > 0.0) {
    //     direction.setX(px / pmom);
    //     direction.setY(py / pmom);
    //     direction.setZ(pz / pmom);
    //   }
    //   G4double mas2 = E * E - pmom * pmom;
    //   if (mas2 >= 0.) {
    //     mass = std::sqrt(mas2);
    //   }
    //   else {
    //     if (G4code != nullptr) {
    //       mass = G4code->GetPDGMass();
    //     }
    //     E = std::sqrt(pmom * pmom + mass * mass);
    //   }
    //   kinE = E - mass;
    // }

  vertex->SetPrimary(particle);
  evt->AddPrimaryVertex(vertex);

  particle->Print();

}


//
// default primary particle: 30 [MeV] e- perpendicular to the target i.e. [1,0,0]
// position is defined depending on the target and world sizes
void YourPrimaryGeneratorAction::SetDefaultKinematic() {
    G4ParticleDefinition* part = G4ParticleTable::GetParticleTable()->FindParticle( "e+" );
    fParticleGun->SetParticleDefinition( part );
    fParticleGun->SetParticleMomentumDirection( G4ThreeVector(1., 0., 0.) );
    fParticleGun->SetParticleEnergy( 30.*CLHEP::MeV );
    UpdatePosition();
}


// needs to be invoked for all workers at the begining of the run: user might
// have changed the target thickness
void YourPrimaryGeneratorAction::UpdatePosition() {
    fParticleGun->SetParticlePosition(
        G4ThreeVector( fYourDetector->GetGunXPosition(), 0.0, 0.0 ) );
}

/*
const G4String& YourPrimaryGeneratorAction::GetParticleName() const {
	return fParticleGun->GetParticleDefinition()->GetParticleName();
}


G4double YourPrimaryGeneratorAction::GetParticleEnergy() const {
	return fParticleGun->GetParticleEnergy();
}
*/
