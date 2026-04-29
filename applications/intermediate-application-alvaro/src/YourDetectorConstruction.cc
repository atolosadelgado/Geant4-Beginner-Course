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
        return;
    }

    void YourDetectorConstruction::SetTargetThickness(G4double thickness){
        fTargetThickness = thickness;

        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }


    G4VPhysicalVolume* YourDetectorConstruction::Construct(){

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

