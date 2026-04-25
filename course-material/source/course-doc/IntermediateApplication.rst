
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

.. _YourDetectorConstruction-class:

YourDetectorConstruction class
------------------------------

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

            if(G4RunManager::GetRunManager()){
                G4RunManager::GetRunManager()->PhysicsHasBeenModified();
            }

        }
        return;
    }

    void YourDetectorConstruction::SetTargetThickness(G4double thickness){
        fTargetThickness = thickness;
        if(G4RunManager::GetRunManager()){
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


