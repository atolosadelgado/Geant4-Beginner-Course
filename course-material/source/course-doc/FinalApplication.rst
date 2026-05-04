
.. _ref-Final-Application:

Final application starting point
--------------------------------

We will start from the end of the previus section :ref:`ref-Intermediate-Application`, where we had a Geant4 simulation running without any user code to extract any information. In this section we will introduce optional user actions that we will use to extract information during the simulation steps with the final goal of calculating the average and RMS energy deposited in the target per event.


.. _ApplicationOptUseractions:

Optional User Actions
---------------------

There are plenty of ways to collect information during the simulation (Sensitive Detectors, Primitive scorers, G4Analysis). We will use a simpler approach, define some optional user actions that will allow to access the simulation information at each step, and then accumulate it during the event, and last to calculate some quantities. For this we will implement G4UserSteppingAction, G4UserEventAction and G4UserRunAction respectively.

G4UserSteppingAction
^^^^^^^^^^^^^^^^^^^^

Let's create our custom class `YourSteppingAction`. We can start the header file `./include/YourSteppingAction.hh`::

    #ifndef YourSteppingAction_hh
    #define YourSteppingAction_hh

    #include "G4UserSteppingAction.hh"

    class YourSteppingAction : public G4UserSteppingAction
    {
        YourSteppingAction();
    };

    #endif // YourSteppingAction_hh

The goal is to record the energy deposited in the target volume, so we have to pass the detector object to retrieve the physical volume of the target::

    #ifndef YourSteppingAction_hh
    #define YourSteppingAction_hh

    #include "G4UserSteppingAction.hh"

    class YourDetectorConstruction;

    class YourSteppingAction : public G4UserSteppingAction
    {
        public:
            YourSteppingAction(YourDetectorConstruction * detector);
            ~YourSteppingAction() override;

            void UserSteppingAction(const G4Step * step) override;
        private:
            YourDetectorConstruction * fDetector;

    };

    #endif // YourSteppingAction_hh

Now we create the `./src/YourSteppingAction.cc`::

    #include "G4Step.hh"
    #include "YourSteppingAction.hh"
    #include "YourDetectorConstruction.hh"

    YourSteppingAction::YourSteppingAction(YourDetectorConstruction * detector):
                G4UserSteppingAction(),
                fDetector(detector) {}

    YourSteppingAction::~YourSteppingAction(){}

    void UserSteppingAction(const G4Step * step){}


We can take a look to the `G4Step` and `G4StepPoint` and look for the method that retrieves the physical volume where the step point is. Then we will compare the pointer to the physical volume to the one retrieved by `YourDetectorConstruction::GetTargetPhysicalVolume` and print the energy deposited (energy loss process, the energy lost by secondaries which have NOT been generated because each of their energies was below the cut threshold)::

    #include "YourSteppingAction.hh"
    #include "YourDetectorConstruction.hh"
    #include "G4Step.hh"
    #include "G4Track.hh"
    #include "G4ParticleDefinition.hh"

    YourSteppingAction::YourSteppingAction(YourDetectorConstruction * detector):
                G4UserSteppingAction(),
                fDetector(detector) {}

    YourSteppingAction::~YourSteppingAction(){}

    void YourSteppingAction::UserSteppingAction(const G4Step * step){
        // return if volume is not target
        if(fDetector->GetTargetPhysicalVolume() != step->GetPreStepPoint()->GetPhysicalVolume()){
            return;
        }

        G4double edep = step->GetTotalEnergyDeposit();

        G4cout << "-- edep = " << edep / CLHEP::MeV << " MeV. "
               << "-- particle = " << step->GetTrack()->GetParticleDefinition()->GetParticleName()
               << G4endl;
    }

The last step, we have to create an object of this class in our Action Initialization, so the file `src/YourActionInitialization.cc` looks like this::

    #include "YourActionInitialization.hh"
    #include "YourPrimaryGeneratorAction.hh"
    #include "YourSteppingAction.hh"

    YourActionInitialization::YourActionInitialization(YourDetectorConstruction * det):G4VUserActionInitialization(),fDetector(det){}

    YourActionInitialization::~YourActionInitialization(){}

    void YourActionInitialization::Build() const {
        YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction(fDetector);
        SetUserAction(primaryAction);

        YourSteppingAction * stepAction = new YourSteppingAction(fDetector);
        SetUserAction(stepAction);

    }

And then we configure and compile the project again. In addition, we can write a macrofile to enable some verbosity and run the simulation without having to type each time in the interactive session or hardcode it in the C++ code::

    # macro file g4run.mac

    /run/initialize

    /tracking/verbose 1
    /event/verbose 1

    /run/beamOn 2

And our main looks like this::

        #include "YourDetectorConstruction.hh"
        #include "YourActionInitialization.hh"

        #include "G4PhysListFactory.hh" // to retrieve reference physics list
        #include "G4RunManagerFactory.hh" // to produce default G4RunManager
        #include "G4UImanager.hh" // to pass some built-in UI commands

        #include "G4UIExecutive.hh"
        #include "G4VisExecutive.hh"

        int main(int argc, char** argv){
            // Detect interactive mode (if no arguments) and define UI session
            G4UIExecutive* ui = nullptr;
            G4String macroFileName;
            if (argc == 1) {
            ui = new G4UIExecutive(argc, argv);
            }
            else{
            macroFileName = argv[1];
            }

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

            G4UImanager * UImanager = G4UImanager::GetUIpointer();
            G4VisExecutive * VisManager = new G4VisExecutive(argc, argv,"ogl");
            VisManager->Initialise();

            // Process macro in batch mode
            if (!ui) {
                G4String command = "/control/execute ";
                UImanager->ApplyCommand(command + macroFileName);
            }
            else {
                // interactive mode
                ui->SessionStart();
                delete ui;
            }
            delete VisManager;
            delete runManager;

            return 0;
        }

Now we can run the application with that macro file like this::

    ./build/yourMainApplication g4run.mac

And the output will look like this::

    G4WT0 > *************************************************************************************************
    G4WT0 > * G4Track Information:   Particle = e-,   Track ID = 1,   Parent ID = 0
    G4WT0 > *************************************************************************************************
    G4WT0 >
    G4WT0 > Step#    X(mm)    Y(mm)    Z(mm) KinE(MeV)  dE(MeV) StepLeng TrackLeng  NextVolume ProcName
    G4WT0 >     0    -5.25        0        0        30        0        0         0       World initStep
    G4WT0 >     1       -5        0        0        30 8.29e-27     0.25      0.25      Target Transportation
    G4WT0 >     2     2.52     0.31    0.665      19.3     2.41     7.58      7.83      Target eIoni
    G4WT0 > -- edep = 2.412515260813551 MeV
    G4WT0 > -- particle = e-
    G4WT0 >     3        5    0.599     1.34        18     1.26     2.61      10.4       World Transportation
    G4WT0 > -- edep = 1.262224395320662 MeV
    G4WT0 > -- particle = e-
    G4WT0 >     4      5.5    0.652     1.47        18 1.64e-26    0.519        11  OutOfWorld Transportation
    G4WT0 > Track (trackID 1, parentID 0) is processed with stopping code 2
    G4WT0 >
    G4WT0 > *************************************************************************************************
    G4WT0 > * G4Track Information:   Particle = e-,   Track ID = 2,   Parent ID = 1
    G4WT0 > *************************************************************************************************
    G4WT0 >
    G4WT0 > Step#    X(mm)    Y(mm)    Z(mm) KinE(MeV)  dE(MeV) StepLeng TrackLeng  NextVolume ProcName
    G4WT0 >     0     2.52     0.31    0.665      8.31        0        0         0      Target initStep
    G4WT0 >     1        5  -0.0731    0.566      7.33    0.983     2.59      2.59       World Transportation
    G4WT0 > -- edep = 0.9826179733663472 MeV
    G4WT0 > -- particle = e-
    G4WT0 >     2      5.5     -0.2    0.589      7.33  1.5e-26    0.516       3.1  OutOfWorld Transportation

Notice that the value `dE(MeV)` printed by tracking verbosity matches the value of our own printout.

G4UserEventAction
^^^^^^^^^^^^^^^^^

Several particles and step may deposit energy in our target, therefore we need something to accumulate this quantity during the event. The easiest way is to accumulate it in a custom class derived from the G4EventAction, and make it accessible to our own stepping action in a similar manner as we did for the detector.

We start by creating our own event action header file `./include/YourEventAction.hh` ::

    #ifndef YourEventAction_hh
    #define YourEventAction_hh

    #include "G4UserEventAction.hh"

    class YourEventAction : public G4UserEventAction
    {
        YourEventAction();
    };

    #endif // YourEventAction_hh

We have to override the destructor, BeginOfEventAction and EndOfEventAction virtual methods. In addition, we define a member of the class to store the energy of the event and a method `AddEdep`::

    #ifndef YourEventAction_hh
    #define YourEventAction_hh

    #include "G4UserEventAction.hh"

    class YourEventAction : public G4UserEventAction
    {
        public:
            YourEventAction();
            ~YourEventAction() override;

            void BeginOfEventAction(const G4Event*) override;
            void EndOfEventAction(const G4Event*) override;

            void AddEdep(G4double edep){fEdepPerEvent+=edep;}

        private:
            G4double fEdepPerEvent{0};
    };

    #endif // YourEventAction_hh

and now the implementation `./src/YourEventAction.cc`::

    #include "YourEventAction.hh"
    #include "G4SystemOfUnits.hh"

    YourEventAction::YourEventAction():G4UserEventAction(){}

    YourEventAction::~YourEventAction(){}

    void YourEventAction::BeginOfEventAction(const G4Event*){
        fEdepPerEvent = 0;
    }

    void YourEventAction::EndOfEventAction(const G4Event*){
        G4cout << " Event Edep (in target) = "
               << fEdepPerEvent / CLHEP::MeV
               << " MeV"
               << G4endl;
    }


But this is not enough: we have to make accesible `YourEventAction::AddEdep` from each step in `YourSteppingAction::UserSteppingAction`. To do so, we have to:

- create and register an object YourEventAction in the Action Initialization::

    #include "YourActionInitialization.hh"
    #include "YourPrimaryGeneratorAction.hh"
    #include "YourSteppingAction.hh"
    #include "YourEventAction.hh"

    YourActionInitialization::YourActionInitialization(YourDetectorConstruction * det):G4VUserActionInitialization(),fDetector(det){}

    YourActionInitialization::~YourActionInitialization(){}

    void YourActionInitialization::Build() const {
        YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction(fDetector);
        SetUserAction(primaryAction);

        YourEventAction * eventAction = new YourEventAction();
        SetUserAction(eventAction);

        YourSteppingAction * stepAction = new YourSteppingAction(fDetector,eventAction);
        SetUserAction(stepAction);

    }

- modify YourSteppingAction constructor, to be able to pass a pointer to the YourEventAction object (in the same manner as we have done for the detector construction) ::

        #ifndef YourSteppingAction_hh
        #define YourSteppingAction_hh

        #include "G4UserSteppingAction.hh"
        #include "globals.hh" // G4double

        class YourDetectorConstruction;
        class YourEventAction;

        class YourSteppingAction : public G4UserSteppingAction
        {
            public:
                YourSteppingAction(YourDetectorConstruction * detector, YourEventAction * eventAction);
                ~YourSteppingAction() override;

                void UserSteppingAction(const G4Step * step) override;
            private:
                YourDetectorConstruction * fDetector;
                YourEventAction          * fEventAction;
        };

        #endif // YourSteppingAction_hh

And the implementation would look like::

    #include "YourSteppingAction.hh"
    #include "YourEventAction.hh"
    #include "YourDetectorConstruction.hh"
    #include "G4Step.hh"
    #include "G4Track.hh"
    #include "G4ParticleDefinition.hh"

    YourSteppingAction::YourSteppingAction(YourDetectorConstruction * detector, YourEventAction * eventAction):
                G4UserSteppingAction(),
                fDetector(detector),
                fEventAction(eventAction){}

    YourSteppingAction::~YourSteppingAction(){}

    void YourSteppingAction::UserSteppingAction(const G4Step * step){
        // return if volume is not target
        if(fDetector->GetTargetPhysicalVolume() != step->GetPreStepPoint()->GetPhysicalVolume()){
            return;
        }

        G4double edep = step->GetTotalEnergyDeposit();

        G4cout << "-- edep = " << edep / CLHEP::MeV << " MeV" << G4endl;
        G4cout << "-- particle = " << step->GetTrack()->GetParticleDefinition()->GetParticleName() << G4endl;
        fEventAction->AddEdep(edep);
    }

Then we configure and compile again. Before running the program as before, we can change the macro file `g4run.mac` to reduce the verbosity and simplify the printout, to see our custom messages more clearly::

    G4WT0 > -- edep = 2.41252 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 1.26222 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 0.982618 MeV
    G4WT0 > -- particle = e-
    G4WT0 >  Event Edep (in target) = 4.65736 MeV
    G4WT0 > -- edep = 2.71039 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 0.685381 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 0.45162 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 0 MeV
    G4WT0 > -- particle = gamma
    G4WT0 > -- edep = 0 MeV
    G4WT0 > -- particle = gamma
    G4WT0 >  Event Edep (in target) = 3.84739 MeV

And now we will climb one level more: we can pass the energy deposited alog the step to the run.

G4UserRunAction
^^^^^^^^^^^^^^^

Our class derived from G4UserRunAction will allow us to collect event information. Inspect `G4UserRunAction.hh` and `G4Run.hh`.

Then we can implement the header file of our derived class `./include/YourRunAction.hh`::

    #ifndef YourRunAction_hh
    #define YourRunAction_hh

    #include "G4UserRunAction.hh"

    class YourRunAction : public G4UserRunAction
    {
        public:
            YourRunAction();
            ~YourRunAction() override;

            void BeginOfRunAction(const G4Run * ) override;
            void EndOfRunAction(const G4Run * ) override;

            void AddEventEdep(G4double val){fEdepInTarget+=val;}
    private:
        G4double fEdepInTarget{0};

    };

    #endif // YourRunAction_hh

And the definition of the methods in the file `src/YourRunAction.cc`::

    #include "YourRunAction.hh"

    #include "G4Run.hh"
    #include "G4SystemOfUnits.hh"
    #include "globals.hh"

    YourRunAction::YourRunAction():
            G4UserRunAction(){}

    YourRunAction::~YourRunAction(){}

    void YourRunAction::BeginOfRunAction(const G4Run *){
        // reset accumulator before each run
        fEdepInTarget = 0.0;
    }

    void YourRunAction::EndOfRunAction(const G4Run * run){

        G4int numberOfEvent = run->GetNumberOfEvent();
        G4double edepAverage = fEdepInTarget  / numberOfEvent;
        G4cout << " Mean energy deposited in the target per event : "
               << edepAverage / CLHEP::MeV
               << " MeV"
               << G4endl;
    }

We have to register it in the Action Initialization `src/YourActionInitialization.cc`::

    #include "YourActionInitialization.hh"
    #include "YourPrimaryGeneratorAction.hh"
    #include "YourSteppingAction.hh"
    #include "YourEventAction.hh"
    #include "YourRunAction.hh"

    YourActionInitialization::YourActionInitialization(YourDetectorConstruction * det):G4VUserActionInitialization(),fDetector(det){}

    YourActionInitialization::~YourActionInitialization(){}

    void YourActionInitialization::Build() const {
        YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction(fDetector);
        SetUserAction(primaryAction);

        YourRunAction * runAction = new YourRunAction();
        SetUserAction(runAction);

        YourEventAction * eventAction = new YourEventAction();
        SetUserAction(eventAction);

        YourSteppingAction * stepAction = new YourSteppingAction(fDetector,eventAction);
        SetUserAction(stepAction);
    }

If we configure and build the executable, and we run, there will be an extra line at the end::

    Mean energy deposited in the target per event : 0 MeV

We need to update the total energy from each event! To do so, we will connect the run action to the event action as we did already for the stepping action. The `src/YourActionInitialization.cc` file will look like::

    #include "YourActionInitialization.hh"
    #include "YourPrimaryGeneratorAction.hh"
    #include "YourSteppingAction.hh"
    #include "YourEventAction.hh"
    #include "YourRunAction.hh"

    YourActionInitialization::YourActionInitialization(YourDetectorConstruction * det):G4VUserActionInitialization(),fDetector(det){}

    YourActionInitialization::~YourActionInitialization(){}

    void YourActionInitialization::Build() const {
        YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction(fDetector);
        SetUserAction(primaryAction);

        YourRunAction * runAction = new YourRunAction();
        SetUserAction(runAction);

        YourEventAction * eventAction = new YourEventAction(runAction);
        SetUserAction(eventAction);

        YourSteppingAction * stepAction = new YourSteppingAction(fDetector,eventAction);
        SetUserAction(stepAction);

    }

And the event action header and definition (modifying the constructor and adding a member to keep a pointer to the run action)::

    #ifndef YourEventAction_hh
    #define YourEventAction_hh

    #include "G4UserEventAction.hh"
    #include "globals.hh"

    class YourRunAction;

    class YourEventAction : public G4UserEventAction
    {
        public:
            YourEventAction(YourRunAction * runAction);
            ~YourEventAction() override;

            void BeginOfEventAction(const G4Event*) override;
            void EndOfEventAction(const G4Event*) override;

            void AddEdep(G4double edep){fEdepPerEvent+=edep;}

        private:
            G4double fEdepPerEvent{0};
            YourRunAction * fRunAction;
    };

    #endif // YourEventAction_hh

and event action definition (adding a call to run action to add the event energy)::

    #include "YourEventAction.hh"
    #include "YourRunAction.hh"
    #include "G4SystemOfUnits.hh"

    YourEventAction::YourEventAction(YourRunAction * runAction):
            G4UserEventAction(),
            fRunAction(runAction){}

    YourEventAction::~YourEventAction(){}

    void YourEventAction::BeginOfEventAction(const G4Event*){
        fEdepPerEvent = 0;
    }

    void YourEventAction::EndOfEventAction(const G4Event*){
        G4cout << " Event Edep (in target) = "
               << fEdepPerEvent / CLHEP::MeV
               << " MeV"
               << G4endl;
        fRunAction->AddEventEdep(fEdepPerEvent);
    }

And if we recompile and run again, the output will look like this::

    G4WT0 > -- edep = 2.41252 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 1.26222 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 0.982618 MeV
    G4WT0 > -- particle = e-
    G4WT0 >  Event Edep (in target) = 4.65736 MeV
    G4WT0 > -- edep = 2.71039 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 0.685381 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 0.45162 MeV
    G4WT0 > -- particle = e-
    G4WT0 > -- edep = 0 MeV
    G4WT0 > -- particle = gamma
    G4WT0 > -- edep = 0 MeV
    G4WT0 > -- particle = gamma
    G4WT0 >  Event Edep (in target) = 3.84739 MeV
    G4WT0 >  Mean energy deposited in the target per event : 4.25237 MeV


Update gun position in YourRunAction::BeginOfRunAction
""""""""""""""""""""""""""""""""""""""""""""""""""""""

We can pass `YourPrimaryGenerator` to `YourRunAction`, so it setups the gun position before starting the run. For that, we need to update 3 files:

1. `YourRunAction.hh`: we have to extend the contructor to take as argument a pointer to the primary generator action object, and store it internally in a new member::

        #ifndef YourRunAction_hh
        #define YourRunAction_hh

        #include "G4UserRunAction.hh"

        class YourPrimaryGeneratorAction; // <-- NEW

        class YourRunAction : public G4UserRunAction
        {
            public:
                YourRunAction(YourPrimaryGeneratorAction * pgenerator); // <-- NEW
                ~YourRunAction() override;

                void BeginOfRunAction(const G4Run * ) override;
                void EndOfRunAction(const G4Run * ) override;

                void AddEventEdep(G4double val){fEdepInTarget+=val;}
        private:
            G4double fEdepInTarget{0};
            YourPrimaryGeneratorAction * fPrimaryGeneratorAction; // <-- NEW

        };

        #endif // YourRunAction_hh

2. `YourRunAction.cc` : we have to initialize the pointer to the primary generator action in the constructor, and call the  `YourPrimaryGeneratorAction::UpdatePosition` in the begin run action method::

    #include "YourRunAction.hh"
    #include "YourPrimaryGeneratorAction.hh"  // <-- NEW

    #include "G4Run.hh"
    #include "G4SystemOfUnits.hh"
    #include "globals.hh"

    YourRunAction::YourRunAction(YourPrimaryGeneratorAction * pgenerator):  // <-- NEW
            G4UserRunAction(),
            fPrimaryGeneratorAction(pgenerator){}  // <-- NEW

    YourRunAction::~YourRunAction(){}

    void YourRunAction::BeginOfRunAction(const G4Run *){
        // reset accumulator before each run
        fEdepInTarget = 0.0;
        if(nullptr != fPrimaryGeneratorAction){
            fPrimaryGeneratorAction->UpdatePosition();  // <-- NEW
        }
    }

    void YourRunAction::EndOfRunAction(const G4Run * run){

        G4int numberOfEvent = run->GetNumberOfEvent();
        G4double edepAverage = fEdepInTarget  / numberOfEvent;
        G4cout << " Mean energy deposited in the target per event : "
               << edepAverage / CLHEP::MeV
               << " MeV"
               << G4endl;
    }

3. `YourActionInitialization.cc`: when instantiating the object runAction, pass the pointer to the primary generator:

  .. code-block:: diff

            #include "YourActionInitialization.hh"
            #include "YourPrimaryGeneratorAction.hh"
            #include "YourSteppingAction.hh"
            #include "YourEventAction.hh"
            #include "YourRunAction.hh"

            YourActionInitialization::YourActionInitialization(YourDetectorConstruction * det):
                G4VUserActionInitialization(),
                fDetector(det){}

            YourActionInitialization::~YourActionInitialization(){}

            void YourActionInitialization::Build() const {
                YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction(fDetector);
                SetUserAction(primaryAction);

            +    YourRunAction * runAction = new YourRunAction(primaryAction);
                SetUserAction(runAction);

                YourEventAction * eventAction = new YourEventAction(runAction);
                SetUserAction(eventAction);

                YourSteppingAction * stepAction = new YourSteppingAction(fDetector,eventAction);
                SetUserAction(stepAction);

            }

.. _ApplicationMultithreading:

How to application Multithreaded Geant4 application
---------------------------------------------------

Let's start simplifying the setup from previous sections:

1. reduce Geant4 verbosity in the macro file `g4run.mac` by removing or comenting the following lines

    .. code-block:: diff

        # macro file g4run.mac

        /run/initialize

        -/tracking/verbose 1
        -/event/verbose 1

        /run/beamOn 2

2. If we have not done yet, we should comment the printouts in the `YourSteppingAction` methods.

Then, if we run the application, we will see something like this::

    G4WT0 >  Event Edep (in target) = 4.65736 MeV
    G4WT0 >  Event Edep (in target) = 3.84739 MeV
    G4WT0 >  Mean energy deposited in the target per event : 4.25237 MeV

Now we are ready to try out Geant4 in MT. To do so, we are going to remove the line from the main function that is limiting the number of threads to 1

.. code-block:: diff

        #include "YourDetectorConstruction.hh"
        #include "YourActionInitialization.hh"

        #include "G4PhysListFactory.hh" // to retrieve reference physics list
        #include "G4RunManagerFactory.hh" // to produce default G4RunManager
        #include "G4UImanager.hh" // to pass some built-in UI commands

        int main(){

            auto * runManager = G4RunManagerFactory::CreateRunManager();
        -    runManager->SetNumberOfThreads(1);

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

            return 0;
        }

If we recompile and run again with the same `g4run.macro`, we will notice an extra line was printed out::

    G4WT5 >  Event Edep (in target) = 4.65736 MeV
    G4WT3 >  Event Edep (in target) = 3.84739 MeV
    G4WT5 >  Mean energy deposited in the target per event : 4.65736 MeV
    G4WT3 >  Mean energy deposited in the target per event : 3.84739 MeV

After removing the limitation to the number of threads (and therefore letting Geant4 choose MT), we notice these changes in the prinout:

- each lines starts with `G4WT5` or  `G4WT3` (before it was `G4WT0` for all user printout) -> this identifies the threads, so we had 2 thread application
- The mean energy is equal to the energy of each event
- The `YourRunAction::EndOfRunAction` method is called twice, because of the double printout  `Mean energy deposited...` -> we will see in the coming sections how to produce 1 single report

In summary, when running in MT mode, each thread has its own copy of the user actions and only the geometry is shared. We cannot use directly the `YourRunAction::EndOfRunAction` method, because it is invoked separately for each thread. The topic is introduced with more details in a dedicated section :ref:`MT`.

Implementing custom run object
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let's start taking a look to `G4Run` class, and notice the virtual methods and other methods that can be useful to us. We will derive our own run object ``YourRun``, and then we will modify the `YourRunAction` to generate our run objects intead of `G4Run`.

Creating our ``YourRun``
""""""""""""""""""""""""

Now let's create the file `./include/YourRun.hh` and fill it with the following::

    #ifndef YourRun_hh
    #define YourRun_hh

    #include "G4Run.hh"

    class Hist;

    class YourRun : public G4Run
    {
        public:
            YourRun();
            ~YourRun() override;

            void Merge(const G4Run * ) override;

            void AddEventEdep(G4double eventEdep);

            void EndOfRunSummary();

    private:
        Hist* fHistEnergy;
    };

    #endif // YourRun_hh


This class will be our container for the energy deposited in the target per event. In the header, we have to override the base class methods destructor and `Merge`, and in addition we introduce two methods that will be useful later.

We can implement the methods in the file `./src/YourRun.cc`. We will go one by one:
1. Constructor: we will initialize the histogram here (the axis limits and bining are arbitrary) ::

    YourRun::YourRun():
            G4Run() {

        G4String outputFileName = "Hist_Edep.dat";
        // this choice is made on simulation with few events

        // implicit range in keV
        // convert into keV when filling histogram
        G4double xmin  = 0.0;
        G4double xmax  = 10.0;
        G4int    nbins = 100;

        fHistEnergy = new Hist(outputFileName, xmin, xmax, nbins);

    }

2. Destructor: we have to de-allocate the memory of the histogram calling the function `delete` ::

    YourRun::~YourRun(){
        delete fHistEnergy;

3. Write method to add energy deposited during one event to the histogram::

    void YourRun::AddEventEdep(G4double eventEdep)
    {
        // histogram range is in keV
        this->fHistEnergy->Fill(eventEdep / CLHEP::keV);
    }

4. Merge: this function will allow to sum up 2 run objects. It will be called automatically by Geant4. Notice that the argument is type `G4Run` but we have to merge our derived class `YourRun`. To convert the base class into our derived class we have to use a *cast*. Then we will add the input run histogram `workerRun->fHistEnergy` to the histogram owned by the object ( `this->fHistEnergy` ). To do so, we will use the method `Hist::Add`. In addition, notice that the histogram `fHistEnergy` is private, but still other instance of the same class can access to it::

    void YourRun::Merge(const G4Run* run)
    {
        // cast into our class
        const YourRun * workerRun = dynamic_cast<const YourRun*>(run);

        // add histograms using Hist::Add
        this->fHistEnergy->Add(workerRun->fHistEnergy);

        // call Merge base class method
        G4Run::Merge(run);
    }


5. Write method to print a summary ::

    void YourRun::EndOfRunSummary()
    {
        G4cout << " === End of Run summary === " << G4endl;
        G4cout << " Number of events : " << G4Run::GetNumberOfEvent() << G4endl;

        fHistEnergy->WriteToFile(true);
    }


The final file looks like this::

    #include "YourRun.hh"
    #include "Hist.hh"
    #include "G4SystemOfUnits.hh"
    #include "globals.hh"

    YourRun::YourRun():G4Run(){

        G4String outputFileName = "Hist_Edep.dat";
        // this choice is made on simulation with few events
        G4double xmin  = 0.0;
        G4double xmax  = 10.0*CLHEP::keV;
        G4int    nbins = 100;

        fHistEnergy = new Hist(outputFileName, xmin, xmax, nbins);

    }
    YourRun::~YourRun(){
        delete fHistEnergy;
    }

    void YourRun::AddEventEdep(G4double eventEdep)
    {
        this->fHistEnergy->Fill(eventEdep);
    }

    void YourRun::Merge(const G4Run* run)
    {
        // cast into our class
        const YourRun * workerRun = dynamic_cast<const YourRun*>(run);

        // add histograms using Hist::Add
        this->fHistEnergy->Add(workerRun->fHistEnergy);

        // call Merge base class method
        G4Run::Merge(run);
    }

    void YourRun::EndOfRunSummary()
    {
        G4cout << " === End of Run summary === " << G4endl;
        G4cout << " Number of events : " << G4Run::GetNumberOfEvent() << G4endl;

        fHistEnergy->WriteToFile(true);
    }

Modify Run action to generate custom run objects
""""""""""""""""""""""""""""""""""""""""""""""""

We have to add a new method to our `YourRunAction`. In the header file, we add these lines

.. code-block:: diff

    #ifndef YourRunAction_hh
    #define YourRunAction_hh

    #include "G4UserRunAction.hh"

    +class YourRun;
    class YourPrimaryGeneratorAction;

    class YourRunAction : public G4UserRunAction
    {
        public:
            YourRunAction(YourPrimaryGeneratorAction * pgenerator);
            ~YourRunAction() override;

            void BeginOfRunAction(const G4Run * ) override;
            void EndOfRunAction(const G4Run * ) override;

    +        G4Run* GenerateRun() override;

            void AddEventEdep(G4double val){fEdepInTarget+=val;}
    private:
    +    YourRun * fRun;
        G4double fEdepInTarget{0};
        YourPrimaryGeneratorAction * fPrimaryGeneratorAction;

    };

    #endif // YourRunAction_hh

And in the implementation, we add the code for the new method


.. code-block:: diff

    #include "YourRunAction.hh"
    #include "YourPrimaryGeneratorAction.hh"
    +#include "YourRun.hh"

    #include "G4Run.hh"
    #include "G4SystemOfUnits.hh"
    #include "globals.hh"

    YourRunAction::YourRunAction(YourPrimaryGeneratorAction * pgenerator):
            G4UserRunAction(),
            fPrimaryGeneratorAction(pgenerator){}

    YourRunAction::~YourRunAction(){}

    void YourRunAction::BeginOfRunAction(const G4Run *){
        // reset accumulator before each run
        fEdepInTarget = 0.0;
    +    if(fPrimaryGeneratorAction){
            fPrimaryGeneratorAction->UpdatePosition();
    +    }
    }

    void YourRunAction::EndOfRunAction(const G4Run * run){

        G4int numberOfEvent = run->GetNumberOfEvent();
        G4double edepAverage = fEdepInTarget  / numberOfEvent;
        G4cout << " Mean energy deposited in the target per event : "
               << edepAverage / CLHEP::MeV
               << " MeV"
               << G4endl;

    +    if(IsMaster()){
    +        fRun->EndOfRunSummary();
    +    }

    }

    +G4Run * YourRunAction::GenerateRun()
    +{
    +    fRun = new YourRun();
    +    return fRun;
    +}


Modify Event action to register information into custom run
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

At the end of the event, we can register the energy deposited into our custom run object. To access it, we invoke the Run manager as follows

.. code-block:: diff

    #include "YourEventAction.hh"
    #include "YourRunAction.hh"
    +#include "YourRun.hh"

    #include "G4SystemOfUnits.hh"
    +#include "G4RunManager.hh"

    YourEventAction::YourEventAction(YourRunAction * runAction):
            G4UserEventAction(),
            fRunAction(runAction){}

    YourEventAction::~YourEventAction(){}

    void YourEventAction::BeginOfEventAction(const G4Event*){
        fEdepPerEvent = 0;
    }

    void YourEventAction::EndOfEventAction(const G4Event*){
        G4cout << " Event Edep (in target) = "
               << fEdepPerEvent / CLHEP::MeV
               << " MeV"
               << G4endl;
        fRunAction->AddEventEdep(fEdepPerEvent);

    +    G4Run * baseRun = G4RunManager::GetRunManager()->GetNonConstCurrentRun();
    +    YourRun * run = dynamic_cast<YourRun*>(baseRun);
    +    run->AddEventEdep(fEdepPerEvent);

    }


Add BuildForMaster for Action initialization
""""""""""""""""""""""""""""""""""""""""""""

To do so, we have to declare the override of the base class method in the header :

.. code-block:: diff

    #ifndef YourActionInitialization_hh
    #define YourActionInitialization_hh

    #include "G4VUserActionInitialization.hh"
    class YourDetectorConstruction;
    class YourActionInitialization : public G4VUserActionInitialization
    {
        public:
            YourActionInitialization(YourDetectorConstruction * det);
            virtual ~YourActionInitialization() override;

    +        virtual void BuildForMaster() const override;
            virtual void Build() const override;

        private:
            YourDetectorConstruction * fDetector;
    };
    #endif // YourActionInitialization_hh

and now we implement it as follows

.. code-block:: diff

    #include "YourActionInitialization.hh"
    #include "YourPrimaryGeneratorAction.hh"
    #include "YourSteppingAction.hh"
    #include "YourEventAction.hh"
    #include "YourRunAction.hh"

    YourActionInitialization::YourActionInitialization(YourDetectorConstruction * det):G4VUserActionInitialization(),fDetector(det){}

    YourActionInitialization::~YourActionInitialization(){}

    +void YourActionInitialization::BuildForMaster() const
    +{
    +    YourRunAction * runAction = new YourRunAction(nullptr);
    +    SetUserAction(runAction);
    +}

    void YourActionInitialization::Build() const {
        YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction(fDetector);
        SetUserAction(primaryAction);

        YourRunAction * runAction = new YourRunAction(primaryAction);
        SetUserAction(runAction);

        YourEventAction * eventAction = new YourEventAction(runAction);
        SetUserAction(eventAction);

        YourSteppingAction * stepAction = new YourSteppingAction(fDetector,eventAction);
        SetUserAction(stepAction);

    }


.. _ApplicationValidation:

Validation
----------

This publication reports energy loss in silicon sensors of different thickness, DOI 10.1088/1748-0221/6/06/P06013. We are going to simulate and compare against experimental data the energy deposited in 5.6 um of Silicon. To do so, we have to do a number of changes:

1. Change the hardcoded thicknes value in the YourDetectorConstruction constructor:

.. code-block:: diff

    YourDetectorConstruction::YourDetectorConstruction() : G4VUserDetectorConstruction() {
        SetTargetMaterial("G4_Si");
    +    SetTargetThickness(5.6*CLHEP::um);
    }

2. Create a macro file to configure the particle gun as in the experiment and launch a simulation with large number of events::

    # g4run_experiment.mac
    /gun/energy 100 MeV
    /gun/particle e-

    /run/initialize
    /run/beamOn 1000

We can recompile and run with this macro. The end of the application printout should look like this::

    G4WT4 >  Event Edep (in target) = 0.00059607 MeV
    G4WT5 >  Event Edep (in target) = 0.00198152 MeV
    G4WT5 >  Event Edep (in target) = 0.000848766 MeV
    G4WT4 >  Event Edep (in target) = 0.000960645 MeV
    G4WT5 >  Event Edep (in target) = 0.00191575 MeV
    G4WT5 >  Event Edep (in target) = 0.000627627 MeV
    G4WT4 >  Event Edep (in target) = 0.00138192 MeV
    G4WT5 >  Event Edep (in target) = 0.0014833 MeV
    G4WT4 >  Event Edep (in target) = 0.000860053 MeV
    G4WT4 >  Event Edep (in target) = 0.000884142 MeV
    G4WT6 >  Mean energy deposited in the target per event : 0.00155673 MeV
    G4WT5 >  Mean energy deposited in the target per event : 0.00167646 MeV
    G4WT2 >  Mean energy deposited in the target per event : 0.00310956 MeV
    G4WT4 >  Mean energy deposited in the target per event : 0.00168833 MeV
    G4WT7 >  Mean energy deposited in the target per event : 0.00188463 MeV
    G4WT3 >  Mean energy deposited in the target per event : 0.00166165 MeV
    G4WT0 >  Mean energy deposited in the target per event : 0.00243022 MeV
    G4WT1 >  Mean energy deposited in the target per event : 0.00152314 MeV
    Mean energy deposited in the target per event : 0 MeV
    === End of Run summary ===
    Number of events : 1000
    Graphics systems deleted.
    Visualization Manager deleting...

In addition, a new file `Hist_Edep.dat` was created, containing 3 colums: the bin number, the bin center and the bin content of our histogram::

    0       5e-05   0
    1       0.00015 40.48583
    2       0.00025 50.607287
    3       0.00035 202.42915
    4       0.00045 151.82186
    5       0.00055 334.0081
    6       0.00065 566.80162
    7       0.00075 495.95142
    8       0.00085 587.04453

We can plot it for example with `gnuplot` ::

    gnuplot
    gnuplot> plot 'Hist_Edep.dat' u 2:3 w l

The experimental data is provided in the course git repository, https://github.com/mnovak42/Geant4-Beginner-Course/blob/master/applications/final-application/experiment/exp_Meroli_100MeV_electron_5p6um_Si.dat, we can retrieve it locally by doing ::

    wget https://raw.githubusercontent.com/mnovak42/Geant4-Beginner-Course/refs/heads/master/applications/final-application/experiment/exp_Meroli_100MeV_electron_5p6um_Si.dat

now we can plot together, keeping in mind that the experimental data is not normalized::

    gnuplot
    gnuplot>  plot 'Hist_Edep.dat' u 2:3 w l, "exp_Meroli_100MeV_electron_5p6um_Si.dat" u 1:($2/6300) pt 6

We can observe fluctuations in our simulation. We can try again but with 100 times more events (by modifying the macro file). Notice how the fluctuations reduce when increasing the number of events.

There are some discrepancies between the simulation and the experimental data. Geant4 physics allow to configure a crucial parameter using an UI command, the so-called **production cut* that determines the threshold below which the secondary particle energy is deposited in place instead of creating a new secondary particle and tracking it::

    # macro file g4run.mac

    /run/initialize

    /tracking/verbose 0
    /event/verbose 0

    /gun/energy 100 MeV
    /gun/particle e-

    /run/setCut 1.0 um

    /run/beamOn 100000

In addition, we can choose a more accurate electromagnetic physicslist *EMZ*, in the main function we only have to change 1 line:

.. code-block:: diff

    #include "YourDetectorConstruction.hh"
    #include "YourActionInitialization.hh"

    #include "G4PhysListFactory.hh" // to retrieve reference physics list
    #include "G4RunManagerFactory.hh" // to produce default G4RunManager
    #include "G4UImanager.hh" // to pass some built-in UI commands

	#include "G4UIExecutive.hh"
	#include "G4VisExecutive.hh"

	int main(int argc, char** argv){
		// Detect interactive mode (if no arguments) and define UI session
		G4UIExecutive* ui = nullptr;
		G4String macroFileName;
		if (argc == 1) {
		ui = new G4UIExecutive(argc, argv);
		}
		else{
		macroFileName = argv[1];
		}

	    auto * runManager = G4RunManagerFactory::CreateRunManager();
	    // runManager->SetNumberOfThreads(1);

	    YourDetectorConstruction* detector = new YourDetectorConstruction();
		runManager->SetUserInitialization(detector);

    +       const G4String plName = "FTFP_BERT_EMZ";
		G4PhysListFactory plFactory;
		plFactory.SetVerbose(0);
		G4VModularPhysicsList *pl = plFactory.GetReferencePhysList( plName );
	    runManager->SetUserInitialization(pl);

	    YourActionInitialization * actionInitialization = new YourActionInitialization(detector);
	    runManager->SetUserInitialization( actionInitialization );

	    // runManager->Initialize();

	    G4UImanager * UImanager = G4UImanager::GetUIpointer();
		G4VisExecutive * VisManager = new G4VisExecutive(argc, argv,"ogl");
		VisManager->Initialise();

		// Process macro in batch mode
		if (!ui) {
			G4String command = "/control/execute ";
			UImanager->ApplyCommand(command + macroFileName);
		}
		else {
			// interactive mode
			ui->SessionStart();
			delete ui;
		}
		delete VisManager;
		delete runManager;

	    return 0;
    }


We can rerun and plot again the results. Is the result improved?

.. admonition:: **Take-home**
   :class: takehome

   This section showed how to create a custom run object to store a histogram, merge the objects at the end of each thread run and write it to an output file. However, it is adviced to use **G4Analysis** if possible, since it gives for free machinery to create and handle histograms and columnar data automatically within Geant4 without writing custom run objects. Please refer to the documentation and examples for further details.


.. _ApplicationCustomUIcommands:

Writing our own UI commands
---------------------------

Please take a look first to :ref:`CustomUIcommand`.


