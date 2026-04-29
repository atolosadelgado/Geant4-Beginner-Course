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
