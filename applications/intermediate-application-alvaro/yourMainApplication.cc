    #include "YourDetectorConstruction.hh"
    #include "YourActionInitialization.hh"

    #include "G4PhysListFactory.hh" // to retrieve reference physics list
    #include "G4RunManagerFactory.hh" // to produce default G4RunManager
    #include "G4UImanager.hh" // to pass some built-in UI commands

    int main(){

	    auto * runManager = G4RunManagerFactory::CreateRunManager();
	    runManager->SetNumberOfThreads(1);
	    
	    YourDetectorConstruction* detector = new YourDetectorConstruction();
		runManager->SetUserInitialization(detector);

	    const G4String plName = "FTFP_BERT";
		G4PhysListFactory plFactory;
		plFactory.SetVerbose(0);
		G4VModularPhysicsList *pl = plFactory.GetReferencePhysList( plName );
	    runManager->SetUserInitialization(pl);

	    YourActionInitialization * actionInitialization = new YourActionInitialization(detector);
	    runManager->SetUserInitialization( actionInitialization );

	    runManager->Initialize();
	    
	    G4UImanager * UImanager = G4UImanager::GetUIpointer();
	    UImanager->ApplyCommand("/tracking/verbose 1");
	    UImanager->ApplyCommand("/event/verbose 1");
	    
	    runManager->BeamOn(1);

		detector->SetTargetThickness(2*CLHEP::cm);


	    runManager->BeamOn(1);

	    return 0;
    }
