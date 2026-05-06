
.. _ref-Intermediate-Application:

Intermediate application starting point
---------------------------------------

We will start by creating a working directory and moving into that directory, for example::

    mkdir ~/work
    cd ~/work

Inside this directory we create a file called `CMakeLists.txt` with the following content::

    #----------------------------------------------------------------------------
    # Setup the project
    cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
    project(yourG4Application)


    #----------------------------------------------------------------------------
    # Find Geant4 package, no UI and Vis drivers activated
    #
    #find_package(Geant4 REQUIRED)

    #----------------------------------------------------------------------------
    # Find Geant4 package, with UI and Vis drivers activated
    #
    find_package(Geant4 REQUIRED ui_all vis_all)


    #----------------------------------------------------------------------------
    # Setup Geant4 include directories and compile definitions
    #
    include(${Geant4_USE_FILE})


    #----------------------------------------------------------------------------
    # Locate sources and headers for this project
    #
    include_directories(${PROJECT_SOURCE_DIR}/include
                        ${Geant4_INCLUDE_DIR})

    file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
    file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

    #----------------------------------------------------------------------------
    # Add the executables, and link it to the Geant4 libraries
    #
    add_executable(yourMainApplication yourMainApplication.cc ${sources} ${headers})
    target_link_libraries(yourMainApplication ${Geant4_LIBRARIES})


and inside this working directory we need to create the source and include directories as follow::

    mkdir src
    mkdir include

Now we are ready to start populating the project with source code of the 4 mandatory components to run a Geant4 simulation.

.. admonition:: **What's next?**
   :class: whatsnext

    Please, check the notes about Detector description in the previous chapter :ref:`ref-DetectorDescription` before continuing.


.. _YourDetectorConstruction-class:

YourDetectorConstruction class
------------------------------


.. tip::

    Before coding the detector description, it is usually a good practice to draw a sketch first

We will start by creating the file `YourDetectorConstruction.hh` in the directory include. Inside this header file we are going to declare our class `YourDetectorConstruction`. Every header should include a *guard define* by creating a preprocessor macro like this::

    // File: YourDetectorConstruction.hh

    #ifndef YourDetectorConstruction_hh
    #define YourDetectorConstruction_hh

    class YourDetectorConstruction{};

    #endif // YourDetectorConstruction_hh

The preprocessor guard define prevents the class declaration from being included more than once (which is required by C++ standard).

.. tip::

    Use consistent naming of the header file name and the class declared inside, and the preprocessor guard define.

Now we can declare our class `YourDetectorConstruction` derived from the Geant4 interface `G4VUserDetectorConstruction`. In addition, we need to include the corresponding Geant4 header file `G4VUserDetectorConstruction.hh`. Then we can start declaring public methods such as the constructor, destructor and the method `Construct`. The latter method and the destructor are virtual, so we mark them with `override`. This keyword tells the compiler to use our implementation for these methods instead of the baseclass. It is a good practice because it helps to spot simple errors at compile time.

In addition, we are going to add two private class members, the target material and the target thickness. This is done to keep the information inside the object `YourDetectorConstruction`. The class members are visible to all methods of the class. Note that we need a forward declaration for the G4Material class, but not for G4VPhysicalVolume. This is because the base class G4VUserDetectorConstruction already has a forward declaration of G4VPhysicalVolume. If we need to use some methods of that class, we need to include it. If we want to just declare or pass a pointer as argument/return of a function, a forward declaration is enough. Forward declarations allow to break circular dependencies between classes. Member class initialization can be done in 3 places: at declaration (in the header), in the constructor intialization list, or in the constructor body function. The first is always recommended to avoid subtle bugs and keep clarity. The file now looks like this::

    // File: YourDetectorConstruction.hh

    #ifndef YourDetectorConstruction_hh
    #define YourDetectorConstruction_hh

    #include "G4VUserDetectorConstruction.hh"

    class G4Material;

    class YourDetectorConstruction : public G4VUserDetectorConstruction {
    public:
        YourDetectorConstruction();
        ~YourDetectorConstruction() override;

        // method to build the geometry description
        // returns pointer to the most top physical volume (the world)
        G4VPhysicalVolume * Construct() override;

        // setters and getters
        // getters should return copy or const reference/ptr
        const G4Material* GetTargetMaterial(){return fTargetMaterial;}
        G4double          GetTargetThickness(){return fTargetThickness;}

        // we need dedicated methods to update the values to also notify the G4RunManager
        void SetTargetMaterial(const G4String& matName);
        void SetTargetThickness(G4double thickness);

        // this value is set during the construction
        // the returned value is constant because nobody should changed outside the class
        const G4VPhysicalVolume* GetTargetPhysicalVolume(){return fTargetPhysicalVolume;}

    private:
        G4Material* fTargetMaterial{nullptr};
        G4double    fTargetThickness{0.0};
        G4VPhysicalVolume* fTargetPhysicalVolume{nullptr};
    };

    #endif // YourDetectorConstruction_hh

.. tip::

    Use forward declaration of a class if just declaring a pointer without using it. Use include of the class if instantiation or call to some method is needed.

Now we create the main function in the file `yourMainApplication.cc` like this::

    #include "YourDetectorConstruction.hh"

    int main(){
        return 0;
    }

.. tip::

    The header files of the project are placed in a subdirectory called `inc` or `include`, while the implementation files are placed in `src` or `source` directory.
    In case of a Geant4 application, the file with the main function (e.g. `yourMainApplication.cc`) can be placed in the main directory, together with the cmake configuration file `CMakeLists.txt`

To configure, compile and run the final executable, from the main directory, as follows::

    cmake -S . -B build -D Geant4_DIR=$G4INSTALL/lib64/cmake
    cmake --build build -- -j8
    ./build/yourMainApplication

Once we check that it compiles and run without errors, we can start implementing the definition of the `YourDetectorConstruction` class. We have to create the file `YourDetectorConstruction.cc` in the `src` directory. The first step is to include the header file of the class. We start by implementing the constructor and destructor. The constructor will set the material using the method of the class, so we have to define that method too. The final implementation look like this

.. code-block:: cpp

    #include "YourDetectorConstruction.hh"

    #include "globals.hh" // G4cout, G4endl
    #include "G4PhysicalConstants.hh"

    // for geometry definitions
    #include "G4Box.hh"
    #include "G4LogicalVolume.hh"
    #include "G4PVPlacement.hh"

    // for material definitions
    #include "G4Material.hh"
    #include "G4NistManager.hh"

    #include "G4RunManager.hh"

    YourDetectorConstruction::YourDetectorConstruction() : G4VUserDetectorConstruction() {
        SetTargetMaterial("G4_Si");
        SetTargetThickness(1.0*CLHEP::cm);
    }

    YourDetectorConstruction::~YourDetectorConstruction() {}

    void YourDetectorConstruction::SetTargetMaterial(const G4String& matName){
        G4Material* mat = G4NistManager::Instance()->FindOrBuildMaterial(matName);
        if(nullptr == mat){
            G4cerr << "Error in YourDetectorConstruction::SetTargetMaterial. Material "
                   << matName << " not included in G4NistManager" << G4endl;
            exit(-1);
        }
        if(fTargetMaterial != mat)
        {
            fTargetMaterial = mat;

            G4RunManager::GetRunManager()->PhysicsHasBeenModified();
            }

        }
        return;
    }

    void YourDetectorConstruction::SetTargetThickness(G4double thickness){
        fTargetThickness = thickness;
        G4RunManager::GetRunManager()->ReinitializeGeometry();
        }
    }


    G4VPhysicalVolume* YourDetectorConstruction::Construct(){
        // I. CREATE/SET MATERIALS:
        // (note that we use fixed material here. One could use messenger to set them)
        // 1. Material for the world: G4_Galactic (low density hydrogen)
        G4String materialNameWorld = "G4_Galactic";
        G4Material* materialWorld  = G4NistManager::Instance()->FindOrBuildMaterial(materialNameWorld);
        // 2. Material for the target: material pointer stored in fTargetMaterial
        G4Material* materialTarget = fTargetMaterial;

        // II. CREATE GEOMETRY:
        // 1. Define target and world sizes
        G4double targetXSize  = fTargetThickness;
        G4double targetYZSize = 1.25*targetXSize;
        G4double worldXSize   = 1.1*targetXSize;
        G4double worldYZSize  = 1.1*targetYZSize;
        // we could compute now the position of the gun
        // fGunXPosition         = -0.25*( worldXSize + targetXSize );
        // 2. Create the world and the target (both will be box):
        // a. world
        G4Box*              worldSolid   = new G4Box("solid-World",  // name
                                                0.5*worldXSize,   // half x-size
                                                0.5*worldYZSize,  // half y-size
                                                0.5*worldYZSize); // half z-size
        G4LogicalVolume*    worldLogical = new G4LogicalVolume(worldSolid,     // solid
                                                            materialWorld,  // material
                                                            "logic-World"); // name
        G4VPhysicalVolume*  worldPhyscal = new G4PVPlacement(nullptr,                 // (no) rotation
                                                            G4ThreeVector(0.,0.,0.), // translation
                                                            worldLogical,            // its logical volume
                                                            "World",                 // its name
                                                            nullptr,                 // its mother volume
                                                            false,                   // not used
                                                            0);                      // cpy number
        // b. target
        G4Box*              targetSolid   = new G4Box("solid-Target",    // name
                                                    0.5*targetXSize,   // half x-size
                                                    0.5*targetYZSize,  // half y-size
                                                    0.5*targetYZSize); // half z-size
        G4LogicalVolume*    targetLogical = new G4LogicalVolume(targetSolid,    // solid
                                                            materialTarget,  // material
                                                            "logic-Target"); // name
        G4VPhysicalVolume*  targetPhyscal = new G4PVPlacement(nullptr,                 // (no) rotation
                                                            G4ThreeVector(0.,0.,0.), // translation
                                                            targetLogical,           // its logical volume
                                                            "Target",                // its name
                                                            worldLogical,            // its mother volume
                                                            false,                   // not used
                                                            0);                      // cpy number
        //
        // III. CREATE FIELD and RETURN WITH THE World PHYSICAL-VOLUME POINTER:
        return worldPhyscal;

        return nullptr;
    }


This code will compile, but it will give a segmentation fault, because nobody created a RunManager, and therefore the instance is null but we are using that pointer (without checking). We will have to create a run manager in the next section.

As an improvement, we can move the calculation of the world and target sizes to dedicated methods, so the hardcoded scaling factors are in a more apropiate place. In addition, we will add a function to calculate the gun position along the X axis. The header file would look like this now::

    // File: YourDetectorConstruction.hh

    #ifndef YourDetectorConstruction_hh
    #define YourDetectorConstruction_hh

    #include "G4VUserDetectorConstruction.hh"

    class G4Material;

    class YourDetectorConstruction : public G4VUserDetectorConstruction {
    public:
        YourDetectorConstruction();
        ~YourDetectorConstruction() override;

        // method to build the geometry description, called by G4RunManager
        // returns pointer to the most top physical volume (the world)
        G4VPhysicalVolume * Construct() override;

        // setters and getters for target material and thickness
        // getters should return copy or const reference/ptr
        const G4Material* GetTargetMaterial(){return fTargetMaterial;}
        G4double          GetTargetThickness(){return fTargetThickness;}
        void SetTargetMaterial(const G4String& matName);
        void SetTargetThickness(G4double thickness);

        // fTargetPhysicalVolume is set during the construction, default nullptr
        // the returned value is constant because nobody should change it outside the class
        const G4VPhysicalVolume* GetTargetPhysicalVolume(){return fTargetPhysicalVolume;}

        // encapsulate scale factors in this functions
        G4double GetTargetSizeX() {return      fTargetThickness; }
        G4double GetTargetSizeYZ(){return  1.2*fTargetThickness; }
        G4double GetWorldSizeX()  {return  1.1*GetTargetSizeX(); }
        G4double GetWorldSizeYZ() {return  1.1*GetTargetSizeYZ();}

        // Calculate Gun position from World and Target Size-X
        G4double GetGunPositionX(){return -0.25*( GetWorldSizeX() + GetTargetSizeX() );}

    private:
        G4Material* fTargetMaterial{nullptr};
        G4double    fTargetThickness{0.0};
        G4VPhysicalVolume* fTargetPhysicalVolume{nullptr};
    };

    #endif // YourDetectorConstruction_hh

.. tip::

    Avoid hardcoding literal numbers in the code, they are not self-explanatory. Instead, use objects with meaningful names to store the values. Add comments adding context of what/why/how/when. Think about the scope of the data (used within a function or several, within a class? should it be exposed or keep it private? will this value change or it will remain constant?).

And the implementation would change a bit too; in addition, we have added a printout to know when the `Construct` Method is being called::

    #include "YourDetectorConstruction.hh"

    #include "globals.hh" // G4cout, G4endl
    #include "G4PhysicalConstants.hh"

    // for geometry definitions
    #include "G4Box.hh"
    #include "G4LogicalVolume.hh"
    #include "G4PVPlacement.hh"

    // for material definitions
    #include "G4Material.hh"
    #include "G4NistManager.hh"

    #include "G4RunManager.hh"

    YourDetectorConstruction::YourDetectorConstruction() : G4VUserDetectorConstruction() {
        SetTargetMaterial("G4_Si");
        SetTargetThickness(1.0*CLHEP::cm);
    }

    YourDetectorConstruction::~YourDetectorConstruction() {}

    void YourDetectorConstruction::SetTargetMaterial(const G4String& matName){
        G4Material* mat = G4NistManager::Instance()->FindOrBuildMaterial(matName);
        if(nullptr == mat){
            G4cerr << "Error in YourDetectorConstruction::SetTargetMaterial. Material "
                   << matName << " not included in G4NistManager" << G4endl;
            exit(-1);
        }
        if(fTargetMaterial != mat)
        {
            fTargetMaterial = mat;

        if(G4RunManager::GetRunManager()) G4RunManager::GetRunManager()->PhysicsHasBeenModified();

        }
        return;
    }

    void YourDetectorConstruction::SetTargetThickness(G4double thickness){
        fTargetThickness = thickness;

        if(G4RunManager::GetRunManager()) G4RunManager::GetRunManager()->ReinitializeGeometry();
    }


    G4VPhysicalVolume* YourDetectorConstruction::Construct(){

        G4cout << "Starting YourDetectorConstruction::Construct()" << G4endl;

        // I. CREATE/SET MATERIALS:

        // 1. Material for the world: G4_Galactic (low density hydrogen)
        G4String materialNameWorld = "G4_Galactic";
        G4Material* materialWorld  = G4NistManager::Instance()->FindOrBuildMaterial(materialNameWorld);

        // 2. Material for the target: material pointer stored in fTargetMaterial
        G4Material* targetMaterial = fTargetMaterial;

        // II. CREATE GEOMETRY:

        // Create the world and the target (both will be box):

        // a. world
        G4Box*              worldSolid   = new G4Box("solid-World",  // name
                                                0.5*GetWorldSizeX(),   // half x-size
                                                0.5*GetWorldSizeYZ(),  // half y-size
                                                0.5*GetWorldSizeYZ()); // half z-size
        G4LogicalVolume*    worldLogical = new G4LogicalVolume(worldSolid,     // solid
                                                            materialWorld,  // material
                                                            "logic-World"); // name
        G4VPhysicalVolume*  worldPhyscal = new G4PVPlacement(nullptr,                 // (no) rotation
                                                            G4ThreeVector(0.,0.,0.), // translation
                                                            worldLogical,            // its logical volume
                                                            "World",                 // its name
                                                            nullptr,                 // its mother volume
                                                            false,                   // not used
                                                            0);                      // cpy number
        // b. target
        G4Box*              targetSolid   = new G4Box("solid-Target",    // name
                                                    0.5*GetTargetSizeX(),   // half x-size
                                                    0.5*GetTargetSizeYZ(),  // half y-size
                                                    0.5*GetTargetSizeYZ()); // half z-size
        G4LogicalVolume*    targetLogical = new G4LogicalVolume(targetSolid,    // solid
                                                            targetMaterial,  // material
                                                            "logic-Target"); // name
        G4VPhysicalVolume*  targetPhyscal = new G4PVPlacement(nullptr,                 // (no) rotation
                                                            G4ThreeVector(0.,0.,0.), // translation
                                                            targetLogical,           // its logical volume
                                                            "Target",                // its name
                                                            worldLogical,            // its mother volume
                                                            false,                   // not used
                                                            0);                      // cpy number

        fTargetPhysicalVolume = targetPhyscal;

        // III. RETURN WITH THE World PHYSICAL-VOLUME POINTER:
        return worldPhyscal;

    }

.. admonition:: **What's next?**
   :class: whatsnext

    Please, check the notes about Physics in the previous chapter :ref:`PhysicsLists` before continuing.

.. _how-to-reference-physics-list:

Retrieving a reference physics list
-----------------------------------

To retrieve a reference physics list, we will use of a so-called factory. Then we can ask the factory to retrieve the corresponding physics list by name. In our case, we will take the hadronic option `FTFP_BERT`, and the default EM option, `EM0` (which can be omited). There is another option `EMZ` that is the most accurate EM physics implemented in Geant4. The meaning of each acronym is outside the scope of this course, and more information can be found in the manual and the upcoming course. In the main function, we can add these lines. Remember to add the Geant4 haeader files for the new classes used in these lines!

.. code-block:: cpp

    const G4String plName = "FTFP_BERT";
    G4PhysListFactory plFactory;
    G4VModularPhysicsList *pl = plFactory.GetReferencePhysList( plName );


.. tip::

    Take a look to the file `FTFP_BERT.cc` and the the physics registered therein. You can do the same with the physics modules, for example the EM extra physics `G4EmExtraPhysics.cc`

Creating a Geant4 Run Manager
-----------------------------

There are seveal Run Managers in Geant4, so we will use a helper class called factory to build our manager. The file `yourMainApplication.cc` will look like this::

    #include "G4RunManagerFactory.hh"

    #include "YourDetectorConstruction.hh"

    #include "G4PhysListFactory.hh"

    int main(){

        auto * runManager = G4RunManagerFactory::CreateRunManager();

        YourDetectorConstruction* detector = new YourDetectorConstruction();
        runManager->SetUserInitialization(detector);

        const G4String plName = "FTFP_BERT";
        G4PhysListFactory plFactory;
        G4VModularPhysicsList *pl = plFactory.GetReferencePhysList( plName );
        runManager->SetUserInitialization(pl);

        return 0;
    }

We can compile and run, we will see few printout messages::

    **************************************************************
    Geant4 version Name: geant4-11-04-ref-00    (5-December-2025)
                        Copyright : Geant4 Collaboration
                        References : NIM A 506 (2003), 250-303
                                    : IEEE-TNS 53 (2006), 270-278
                                    : NIM A 835 (2016), 186-225
                                WWW : http://geant4.org/
    **************************************************************

    G4PhysListFactory::GetReferencePhysList <FTFP_BERT>
    <<< Geant4 Physics List simulation engine: FTFP_BERT

To be able to initialize the run manager and run a simulation, we are still missing the primary generator, the last mandatory component. We will see how to implement it in the next section.

.. _ref-Application-Primary-generator:

Primary generator
-----------------

We will implement a pencil beam of monoenergetic electrons. We will need 2 new classes for this:

- G4VUserPrimaryGenerationAction, it is the Geant4 interface for our own primary generator. The method `Generate primaries` is invoked at the begining of each event to create some primary particles, and this is the method we have to override from the base class
- G4UserActionInitialization, it is the Geant4 interface to declare the user actions, a collection of interfaces that serve as contact point between the user and the simulation. At its minimum, we have to derive our own from the interface and override the `Build()` method to register our own primary generator.

YourPrimaryGenerator
^^^^^^^^^^^^^^^^^^^^

The position of the gun has to be retrieved from the detector constructor. So we will psas the pointer to the detector constructor object to the constructor and we are going to store it as a member of the primary generator class. We need also to override the method `GeneratePrimaries`. Geant4 provides different generators of primaries, we will use the simplest is called `G4ParticleGun`, so we will have to create a field to store it in the object. To configure the gun, we will add two helper methods `SetDefault` and `UpdatePosition`.

The file `./include/YourPrimaryGeneratorAction.hh` will look like this::

    #ifndef YourPrimaryGeneratorAction_hh
    #define YourPrimaryGeneratorAction_hh

    #include "G4VUserPrimaryGeneratorAction.hh"

    class YourDetectorConstruction;
    class G4ParticleGun;
    class G4Event;

    class YourPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
    {
        public:
            YourPrimaryGeneratorAction(YourDetectorConstruction * det);
            virtual ~YourPrimaryGeneratorAction() override;

            virtual void GeneratePrimaries(G4Event *) override;

            void SetDefault();

            void UpdatePosition();
        private:
            YourDetectorConstruction * fDetector;
            G4ParticleGun *            fGun;

    };
    #endif // YourPrimaryGeneratorAction_hh

And its implementation in `./src/YourPrimaryGeneratorAction.cc` will look like this::

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

        G4ThreeVector xAxisDirection(1.,0.,0.);
        fGun->SetParticleMomentumDirection(xAxisDirection);

        G4double pEnergy = 30*CLHEP::MeV;
        fGun->SetParticleEnergy(pEnergy);

        UpdatePosition();
    }

    void YourPrimaryGeneratorAction::UpdatePosition(){
        G4ThreeVector gunPosition(fDetector->GetGunPositionX(),0.,0.);
        fGun->SetParticlePosition(gunPosition);

    }

    void YourPrimaryGeneratorAction::GeneratePrimaries(G4Event *evt) {
        fGun->GeneratePrimaryVertex(evt);
    }

Notice that we have to allocate and de-allocate memory for the particle gun (with new/delete in the constructor/destructor). Check the header of `G4ParticleGun` for the list of methods.

If we try to compile, we will have an error message like `error: class YourDetectorConstruction has no member named GetGunPositionX`. So we have to go back to

- the `YourDetectorConstruction.hh` file, add a private member called `G4double fGunPositionX{0.0};`, and a getter like, `G4double GetGunPositionX(){return fGunPositionX;}`;

- the `YourDetectorConstruction.cc` file, and in the contructor method, we have to calculate its value as `fGunPositionX = -0.25*( worldXSize + targetXSize );`.

After these 3 additions, the code should compile successfully.

YourActionInitialization
^^^^^^^^^^^^^^^^^^^^^^^^

Now we have to register the primary generator as user action in a derived class from the Geant4 interface dedicated to collect all the user actions, and then we have to create an instance of this class in the main and register the main user action initalization class into the run manager.

The header file `include/YourActionInitialization.hh` will look like this::

    #ifndef YourActionInitialization_hh
    #define YourActionInitialization_hh

    #include "G4VUserActionInitialization.hh"
    class YourDetectorConstruction;
    class YourActionInitialization : public G4VUserActionInitialization
    {
        public:
            YourActionInitialization(YourDetectorConstruction * det);
            virtual ~YourActionInitialization() override;

            virtual void Build() const override;

        private:
            YourDetectorConstruction * fDetector;
    };
    #endif // YourActionInitialization_hh


And the implementation in the file `src/YourActionInitialization.cc` looks like this::

    #include "YourActionInitialization.hh"
    #include "YourPrimaryGeneratorAction.hh"

    YourActionInitialization::YourActionInitialization(YourDetectorConstruction * det):G4VUserActionInitialization(),fDetector(det){}

    YourActionInitialization::~YourActionInitialization(){}

    void YourActionInitialization::Build() const {
    YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction(fDetector);
    SetUserAction(primaryAction);
    }

The final step is to register an instance of `YourActionInitialization` into the run manager. To do so, in the main file `yourMainApplication.cc` we have to add few lines, and the file will look as below::

    #include "YourDetectorConstruction.hh"
    #include "YourActionInitialization.hh"

    #include "G4PhysListFactory.hh"
    #include "G4RunManagerFactory.hh"

    int main(){

        auto * runManager = G4RunManagerFactory::CreateRunManager();

        YourDetectorConstruction* detector = new YourDetectorConstruction();
        runManager->SetUserInitialization(detector);

        const G4String plName = "FTFP_BERT";
        G4PhysListFactory plFactory;
        G4VModularPhysicsList *pl = plFactory.GetReferencePhysList( plName );
        runManager->SetUserInitialization(pl);

        YourActionInitialization * actionInitialization = new YourActionInitialization(detector);
        runManager->SetUserInitialization( actionInitialization );

        return 0;
    }

If we compile and run, we should see the same output as before. The next section explains how to run a simulation by simply adding another 2 lines in the main function.

.. _RunManagerInitializeBeamOn:

Run manager initialize and BeamOn methods
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After registering the geometry, the physics and the actions into the run manager, We can call its `Initialize` and `BeamOn` methods, so the main file will look like this::

    #include "YourDetectorConstruction.hh"
    #include "YourActionInitialization.hh"

    #include "G4PhysListFactory.hh"
    #include "G4RunManagerFactory.hh"

    int main(){

        auto * runManager = G4RunManagerFactory::CreateRunManager();

        YourDetectorConstruction* detector = new YourDetectorConstruction();
        runManager->SetUserInitialization(detector);

        const G4String plName = "FTFP_BERT";
        G4PhysListFactory plFactory;
        G4VModularPhysicsList *pl = plFactory.GetReferencePhysList( plName );
        runManager->SetUserInitialization(pl);

        YourActionInitialization * actionInitialization = new YourActionInitialization(detector);
        runManager->SetUserInitialization( actionInitialization );

        runManager->Initialize();
        runManager->BeamOn(2);

        return 0;
    }

Now a long printout appears. The `Initialize` method calls the `YourDetectorConstruction::Construct` and the physics contructors for particles and processes. The printout is coming from the physics modules, in particular about their default parameter configuration. The `BeamOn(2)` method will run a simulation with 2 events, but nothing is being printed about it. We will activate some verbosity to printout the steps of the particules during the simulation. To do so, we will first enable 1 single thread, so the printout is linear. The main function should look like this::

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

            return 0;
        }

After that we should see something like this::

    *************************************************************************************************
    * G4Track Information:   Particle = e-,   Track ID = 1,   Parent ID = 0
    *************************************************************************************************

    Step#    X(mm)    Y(mm)    Z(mm) KinE(MeV)  dE(MeV) StepLeng TrackLeng  NextVolume ProcName
        0    -5.25        0        0        30        0        0         0       World initStep
        1       -5        0        0        30 8.29e-27     0.25      0.25      Target Transportation
        2     2.06    0.191   -0.638      22.3     2.65     7.12      7.37      Target eBrem
        3        5     0.53   -0.824      21.3     1.03     2.99      10.4       World Transportation
        4      5.5    0.651   -0.823      21.3 1.65e-26    0.515      10.9  OutOfWorld Transportation

    *************************************************************************************************
    * G4Track Information:   Particle = gamma,   Track ID = 2,   Parent ID = 1
    *************************************************************************************************

    Step#    X(mm)    Y(mm)    Z(mm) KinE(MeV)  dE(MeV) StepLeng TrackLeng  NextVolume ProcName
        0     2.06    0.191   -0.638      5.04        0        0         0      Target initStep
        1        5    0.534   -0.517      5.04        0     2.97      2.97       World Transportation
        2      5.5    0.592   -0.497      5.04        0    0.504      3.47  OutOfWorld Transportation

A primary electron crosses the world, the target, and before exiting it produces a gamma ray by Bremsstrahlung, and then it reaches the end of the world indicated as `OutOfWorld`. The tracking of the secondary gamma ray starts once the primary particle has been fully tracked. The gamma ray does not interact with anything and it is only transported until the end of the world. We do not know exactly what happened between the steps (continuous energy loss and MSC in a condensed history approach: many interactions are captured by a single step). Notice that the track ID is a counter that is increased for each particle being simulated. The parent ID corresponds to the track ID of the mother particle.

If we take a look to another event::

    *************************************************************************************************
    * G4Track Information:   Particle = e-,   Track ID = 1,   Parent ID = 0
    *************************************************************************************************

    Step#    X(mm)    Y(mm)    Z(mm) KinE(MeV)  dE(MeV) StepLeng TrackLeng  NextVolume ProcName
        0    -5.25        0        0        30        0        0         0       World initStep
        1       -5        0        0        30 8.29e-27     0.25      0.25      Target Transportation
        2    -4.71  0.00526 0.000107      29.5   0.0982    0.286     0.536      Target eIoni
        3    -1.99 -0.00292  -0.0997        28    0.933     2.73      3.27      Target eIoni
        4        5    0.183   -0.372      25.1     2.97     7.06      10.3       World Transportation
        5      5.5     0.17   -0.314      25.1 1.64e-26    0.503      10.8  OutOfWorld Transportation
    Track (trackID 1, parentID 0) is processed with stopping code 2

    *************************************************************************************************
    * G4Track Information:   Particle = e-,   Track ID = 3,   Parent ID = 1
    *************************************************************************************************

    Step#    X(mm)    Y(mm)    Z(mm) KinE(MeV)  dE(MeV) StepLeng TrackLeng  NextVolume ProcName
        0    -1.99 -0.00292  -0.0997     0.519        0        0         0      Target initStep
        1    -1.78   -0.266  0.00283         0    0.519    0.997     0.997      Target eIoni
    Track (trackID 3, parentID 1) is processed with stopping code 2

    *************************************************************************************************
    * G4Track Information:   Particle = e-,   Track ID = 2,   Parent ID = 1
    *************************************************************************************************

    Step#    X(mm)    Y(mm)    Z(mm) KinE(MeV)  dE(MeV) StepLeng TrackLeng  NextVolume ProcName
        0    -4.71  0.00526 0.000107     0.426        0        0         0      Target initStep
        1    -4.59  -0.0526  -0.0516     0.377   0.0492    0.156     0.156      Target msc
        2    -4.53  -0.0554    0.189     0.272    0.105    0.294      0.45      Target msc
        3    -4.59   -0.116    0.293         0    0.272    0.392     0.842      Target eIoni

Notice that Geant4 tracks first the last secondary because particles to be tracked are placed in stacks, and the first to be taken is the last placed in the stack.

Notice also that if we run the same code in the same machine, we should get the same printout. This is because Geant4 uses pseudo-random numbers.

The number of steps is not something we can decide, Geant4 uses Monte Carlo methods for tracking and interactions. To calculate if an interaction happens, the processes associated to the particle are invoked and asked to provide the mean interaction length (inversely proportional to the atomic cross section and material density), and interaction length is sampled from an exponential disitribution with that mean length, and the process with the shortest sampled interaction length wins and that is the process that happens. The interaction probability distribution is exponential because the probability is constant, and that happens if we assume constant material.

In general, it is preferred to keep an event as simple as possible (1M events rather than 1 event with 1M of primaries).


.. tip::

    The uncertainty of a MC simulation has 2 components, as in a real experiment: the precision is given by the number of events, and an accuracy given by the (particles and) processes we choose for our simulation.

Changing detector size between runs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We can change the thickness of the target and run a second run by adding two lines at the end of our main, so it would look like this::

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

If we inspect the printout, we will notice that the second run shows that the geometry changed but the primary particles start in the same position as before. This happens because our primary generator was not notified with the cnage of thickness. We will see in the following sessiosn how to address this elegantly, but for now we can simply call the `UpdatePosition` method before generating each primary, so the method of `YourPrimaryGeneratorAction` would look like this::

    void YourPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt) {
        UpdatePosition();
        fParticleGun->GeneratePrimaryVertex(evt);
    }

If we recompile and run, we will see that the position of the primary electron is now correct.

Importance of notifying of geometry changes to the run manager
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When we set a new target thickness, we added a line to notify the run manager that geometry has changed::

    void YourDetectorConstruction::SetTargetThickness(G4double thickness){
        fTargetThickness = thickness;
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

We can play to remove that line, recompile and run again to see what happens. Now the call to `YourDetectorConstruction::Construct` is not done, the printout does not appear.


.. admonition:: **What's next?**
   :class: whatsnext

    Please, check the notes about User Interfaces (UI) in the previous chapter :ref:`UserInterface` before continuing.

.. _IntermediateApplicationUI:

UI session
----------

In a previous section :ref:`UserInterface` we have reviewed the different types of UI sessions. We can implement an optional behaviour: if we provide an argument to the executable, we will assume if the name of a macro file, that we will execute in batch mode and then we will exit the application. If we do not provide any argument, we will start an interactive session with the preferred session, which will be a graphical interface based on Qt. After implementing the `G4UIExecutive` and the control flow to decide what to do, the main function should look like this::

        #include "YourDetectorConstruction.hh"
        #include "YourActionInitialization.hh"

        #include "G4PhysListFactory.hh" // to retrieve reference physics list
        #include "G4RunManagerFactory.hh" // to produce default G4RunManager
        #include "G4UImanager.hh" // to pass some built-in UI commands
        #include "G4UIExecutive.hh" // for interactive session

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

            // runManager->Initialize();
            //
            // 	G4UImanager * UImanager = G4UImanager::GetUIpointer();
            // 	UImanager->ApplyCommand("/tracking/verbose 1");
            // 	UImanager->ApplyCommand("/event/verbose 1");
            //
            // 	runManager->BeamOn(1);
            //
            // 	detector->SetTargetThickness(2*CLHEP::cm);
            //
            //
            // 	runManager->BeamOn(1);
            // Get the pointer to the User Interface manager
            G4UImanager * UImanager = G4UImanager::GetUIpointer();

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

            delete runManager;
            return 0;
        }

After compiling and launching the program, a new window will open. This is because the Geant4 in the virtual machine was compiled with Qt and OpenGL, and Geant4 selects this as default option. On the left we have a menu with Geant4 UI commands. We can try to look for our command we used to increate the verbosity of the tracking. If we try to start a simulation with the command `/run/beamOn 1`, an error message will happen::

    Geant4 kernel should be initialized
    before the first BeamOn(). - BeamOn ignored.

This is caused by the run manager not being initialized (commented line `runManager->Initialize();`). In general, the applicability of some commands depends on the status of the Run Manager. We can use the UI command to Initialize the Run Manager, for example by typing in the terminal the following::

    /run/initialize

If we now try to run a simulation, it will printout the individual steps during the simulation as before.

We can specify the type of session we would like, instead of letting Geant4 decide. For example, we can ask for a terminal-like session by adding a third argument to the constructor of the `G4UIExecutive`::

    ui = new G4UIExecutive(argc, argv),"tcsh");

Now we simple stay in the terminal, instead of a new window popping up. Notice that the promt representes the state of the Geant4 Run Manager (`PreInit`, `Idle`, etc). See `G4ApplicationState.hh` for further details.

Configuring the primary generator using UI commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Our primary generator uses the Geant4 class `G4ParticleGun`. This class defines some UI commands, added to the `/gun/` directory. We can use these commands to configure it without writing C++ code. We can start by printing the usage of the `/gun` directory::

        PreInit> /control/manual /gun/
        Command directory path : /gun/


        Guidance :
        Particle Gun control commands.

        Sub-directories :
        Commands :
        List * List available particles.
        particle * Set particle to be generated.
        direction * Set momentum direction.
        energy * Set kinetic energy.
        momentum * Set momentum. This command is equivalent to two commands
        momentumAmp * Set absolute value of momentum.
        position * Set starting position of the particle.
        time * Set initial time of the particle.
        polarization * Set polarization.
        number * Set number of particles to be generated.
        ion * Set properties of ion to be generated.
        ionL * THIS COMMAND IS DEPRECATED and will be removed in future releases.
        checkVolume * Switch on/off the check if the vertex position is inside the world volume.
        ...

We can use `/gun/list` to list the available particles. We can select gamma as particle by doing::

        /gun/particle gamma

For the energy, we can scroll in the previous printout and look for the command `/gun/energy`::

        Command /gun/energy
        Guidance :
        Set kinetic energy.

        Parameter : Energy
        Parameter type  : d
        Omittable       : True
        Default value   : taken from the current value

        Parameter : Unit
        Parameter type  : s
        Omittable       : True
        Default value   : GeV
        Candidates      : eV keV MeV GeV TeV PeV meV J electronvolt kiloelectronvolt megaelectronvolt gigaelectronvolt teraelectronvolt petaelectronvolt millielectronVolt joule

We can select an energy of 500 keV::

        /gun/energy 500 keV

We can take a look to the `G4ParticleGunMessenger.hh` to see the implementation of these commands.

.. admonition:: **What's next?**
   :class: whatsnext

   Please, check the notes about visualization in the previous chapter :ref:`Visualization` before continuing.


.. _IntermediateApplicationVisualization:

Adding the Visualization Manager (interactive)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Geant4 graphics interface is provided by the Visualization Manager `G4VisManager` base class (with the `RegisterGraphicsSystems()` pure virtual method)

Similarly to the UI session, Geant4 provides the G4VisExecutive as one implementation of this interface, that can be used directly in the main method of the application:

- include `G4VisExecutive.hh` ::

        #include "G4VisExecutive.hh"

- create the Visualization Manager object (Geant4 will choose a Visualization manager based on its configuration), and initialise the Visualization Manager before the run. We can add the Visualization Executive by adding these lines to our main (just before creating the `G4UImanager`)::

		G4VisExecutive * VisManager = new G4VisExecutive();
		VisManager->Initialise();

- delete the Visualization Manager object at the end of the application::

        delete VisManager;

This class can be used even if Geant4 was configured without visualization because there is a number of drivers that are always available and can create output files (non-interactive visualization).

The main file looks then like this::

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

            runManager->Initialize();

            G4UImanager * UImanager = G4UImanager::GetUIpointer();
            G4VisExecutive * VisManager = new G4VisExecutive();
            VisManager->Initialise();

            // UImanager->ApplyCommand("/tracking/verbose 1");
            // UImanager->ApplyCommand("/event/verbose 1");
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

If we recompile and run, we will see some messages related to the visualization after initializing the run manager::

    You have successfully registered the following graphics systems.
    Registered graphics systems are:
    ASCIITree (ATree)
    DAWNFILE (DAWNFILE)
    RayTracer (RT)
    VRML2FILE (VRML2FILE)
    gMocrenFile (gMocrenFile)
    TOOLSSG_OFFSCREEN (TSG_OFFSCREEN, TSG_FILE)
    OpenGLImmediateQt (OGLIQt, OGLI)
    OpenGLStoredQt (OGLSQt, OGLS)
    OpenGLImmediateX (OGLIX, OGLIQt_FALLBACK)
    OpenGLStoredX (OGLSX, OGLSQt_FALLBACK)
    RayTracerQt (RTQt)
    TOOLSSG_X11_GLES (TSG_X11_GLES, TSGX11, TSG_QT_GLES_FALLBACK)
    TOOLSSG_X11_ZB (TSG_X11_ZB, TSGX11ZB)
    TOOLSSG_QT_GLES (TSG_QT_GLES, TSGQt, TSG, OGL)
    TOOLSSG_QT_ZB (TSG_QT_ZB, TSGQtZB, TSGZB)
    You may choose a graphics system (driver) with a parameter of
    the command "/vis/open" or "/vis/sceneHandler/create",
    or you may omit the driver parameter and choose at run time:
    - by argument in the construction of G4VisExecutive;
    - by environment variable "G4VIS_DEFAULT_DRIVER";
    - by entry in "~/.g4session";
    - by build flags.
    - Note: This feature is not allowed in batch mode.
    For further information see "examples/basic/B1/exampleB1.cc"
    and "vis.mac".


Then we can investigate the different parts of the Qt window:

- the main visualization pad (we can create several tabs)
- the terminal space below, with the output from Geant4 kernel and the interactive input box
- the menu on the left, where we can select the UI commands, navigate the scene tree, visualize the event as a movie, or see the history of commands
- toolbar on top, where some buttons encapsulate some functionality

Then, we can run the following commands to display the geometry::

    /vis/open OGL
    /vis/drawVolume

If we now run the simulation, we will simply see the printout (in case we enable the `/tracking/verbosity 1` as before). However, we can visualize the tracjectory of each particle during one event if we add these commands to accumulate the simulation information and display it at the end of the event::

    /vis/scene/add/trajectories
    /vis/scene/endOfEventAction accumulate 100
    /run/beamOn 100


We can change the point of view, with a UI command (or the mouse in case of Qt+openGL)::

    /vis/viewer/set/viewpointThetaPhi -40 -50

To finish the interactive session, we can type `exit`.

We have reviewed the UI commands to configure the particle gun and visualize the geometry and the particle trajectories. Now we can collect them in a text file which we can later execute to visualize the run. In a file called `g4macro_vis.mac` we can write the following::

    # Lines starting by # are comments

    # Set the tracking verbose to see each simulation step
    /tracking/verbose 0

    # Run initialization
    /run/initialize

    # Set the primary particle type and energy
    /gun/energy 4 MeV
    /gun/particle e-

    # open Visualization and draw full geometry
    /vis/open OGL
    /vis/drawVolume

    # configure the scene to accumulate trajectories over 100 events
    /vis/scene/add/trajectories
    /vis/scene/endOfEventAction accumulate 100

    # run 100 events
    /run/beamOn 100

We can execute our application, and in the terminal write the following command to execute this macro file::

    /control/execute g4macro_vis.mac

