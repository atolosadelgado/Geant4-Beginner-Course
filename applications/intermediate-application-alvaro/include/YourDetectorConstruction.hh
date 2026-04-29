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
