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
