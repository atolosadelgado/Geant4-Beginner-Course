
.. _ref-DetectorDescription:

Detector Description
--------------------

In Geant4, We call "detector description" to the description of the materials and geometry of our case of study. The geometry can be instrumented to include fields (e.g., EM) or other features that will be mentioned in the dedicated section.

Unit system
...........

Geant4 uses the unit system defined in CLHEP (Class Library for High Energy Physics). This system is consistent within the toolkit, but it does not match with other frameworks such as ROOT, so special attention must be given if a combination of both happens.

To use the system of units we have to load this header::

    #include "G4SystemOfUnits.hh"


There are some natural units of Geant4 (ns, mm, eplus, MeV, radian, kelvin, candela, steradian). The units live in the CLHEP namespace. That means that to use the unit gram, we have to call it as `CLHEP::g`. When printing quantities, remember to divide by the unit, e.g.::

    G4cout << "Energy: " energy / CLHEP::eV << " eV" << G4endl;

.. warning::

    Please avoid loading a full namespace into our program (`using CLHEP;`), as it can lead to bugs very difficult to solve.

    It is possible to use `using CLHEP::g;` after the include, to avoid repeating the CLHEP namespace each time we use the unit. However, it can lead to problems difficult to debug (if for example there is another variable called `g`).



Geant4 Material model
.....................

The material model of Geant4 resembles the natural definition: a material is made of elements, and an element is made of isotopes. The 3 main classes to describe these objects are

- G4Isotope: describes the properties of isotopes (Z - atomic number, N - number of nucleons and A - molar mass) with unique name and index
- G4Element: describes the properties of atoms (Z - effective atomic number, N - effective number of nucleons and A - effective molar mass, number of isotopes, etc.) with unique name, symbol and index
- G4Material: describes the macroscopic properties of matter (density, state, temperature, pressure, etc.) with unique name and index

Each time an object of these kind is created, a pointer is stored in the corresponding table.

However, as in many other parts of Geant4, we can build what we need (e.g., materials) from their minimal components (e.g., isotopes for materials), or we can use Geant4 builtin components to get what we want (builders, factories, etc). The last is the preferred way to ensure consistency across the toolkit. In particular, for materials Geant4 provides a set of predefined materials whose composition and properties are taken from NIST database. See documentation for further details

There are some requisites for materials:
- if building density from scratch, we have to specify density larger than zero (if we get the material from Geant4 NIST database, we do not need to)
- temperatura and pressure can be optionally setup, default are normal conditions
- state of matter can be solid or gas; if not specified, assumed gas if density lower than kGasThreshold = 10 mg/cm3)
- solids are consider amorphous (isotropic properties) by default, and in general is a good approximation. However, there is a special extension to include some information about the crystaline structure

Isotopes
^^^^^^^^

Inspect header file G4Isotope.hh and learn what are the arguments for the constructor. We can start a minimal application with the following code::

    #include "globals.hh" // G4cout, G4endl

    #include "G4SystemOfUnits.hh"

    using CLHEP::g;
    using CLHEP::mole;

    #include "G4Isotope.hh"

    int main() {

        std::string name, symbol;
        double z,a,n;

        G4Isotope * iso12C = new G4Isotope("C12", z = 6, a = 12);

        // G4Isotope pointer has defined the operator <<
        G4cout << iso12C << G4endl;

    }

G4Isotope has defined the operator "<<", which prints the information stored in the object. The definition is here: https://github.com/Geant4/geant4/blob/41f2dd79968018de4efb966f2546970b30c4af78/source/materials/src/G4Isotope.cc#L84

We can keep using the following cmake configuration `CMakeLists.txt`::

    cmake_minimum_required(VERSION 3.16)
    find_package(Geant4 REQUIRED)
    include(${Geant4_USE_FILE})
    add_executable(ourmain ourmain.cc)
    target_link_libraries(ourmain ${Geant4_LIBRARIES})

To configure and build our project, we write the following commands in a terminal in the directory where the files are::

    cmake -S . -B build
    cmake --build build

Elements
^^^^^^^^

Inspect header file G4Element.hh and learn what are the arguments for the constructor. We can start by building an element by ourselves, providing element name, symbol, atomic number (z) and atomic mass (a). Geant4 will make this element with the natural isotopic composition (the isotopes are deduced by the symbol). The atomic mass is not updated after assigning the isotopes. A minimal code to code this looks like this::

    #include "globals.hh" // G4cout, G4endl

    #include "G4SystemOfUnits.hh"

    using CLHEP::g;
    using CLHEP::mole;

    #include "G4Element.hh"

    int main() {

        std::string name, symbol;
        double z,a,n;

        // way 1: create element without specifying isotopic composition
        //        isotopes are provided by Geant4, based on Z and the
        //        corresponding natural abundance

        G4Element * elC = new G4Element(name="Carbon", symbol="C", z = 6, a=12.01*g/mole);

        G4cout << elC << G4endl;

    }

And if we execute the code, we will see::

    Element: Carbon (C)   Z =  6.0   N =    12   A = 12.010 g/mole
            --->  Isotope:   C12   Z =  6   N =  12   A =  12.00 g/mole   abundance: 98.930 %
            --->  Isotope:   C13   Z =  6   N =  13   A =  13.00 g/mole   abundance:  1.070 %

.. TODO: why if Z is Zeff, it throws warning and round z to integer? Because it is not effective, it has to be exact!

Alternatively, we can build the element from our custom set of isotopes::

    #include "globals.hh" // G4cout, G4endl

    #include "G4SystemOfUnits.hh"

    using CLHEP::g;
    using CLHEP::mole;

    #include "G4Isotope.hh"
    #include "G4Element.hh"

    int main() {

        std::string name, symbol;
        double z,a,n, abundance;
        int numberIsotopes;


        // way 2: create element by specifying isotopic composition

        G4Element * elC = new G4Element(name="PrehistoricCarbon", symbol="C", numberIsotopes = 2);

        G4Isotope * iso12C = new G4Isotope("C12", z = 6, a = 12);
        G4Isotope * iso13C = new G4Isotope("C13", z = 6, a = 13);

        elC->AddIsotope(iso12C, abundance=0.95);
        elC->AddIsotope(iso13C, abundance=0.05);


        G4cout << elC << G4endl;

    }

And if we run it again, we will see this::

    Element: PrehistoricCarbon (C)   Z =  6.0   N =    12   A = 12.050 g/mole
            --->  Isotope:   C12   Z =  6   N =  12   A =  12.00 g/mole   abundance: 95.000 %
            --->  Isotope:   C13   Z =  6   N =  13   A =  13.00 g/mole   abundance:  5.000 %


Materials
^^^^^^^^^

Materials can be made of one or several elements. The idea is similar to the construction of an element, we can let Geant4 create the natural element based on the atomic number of the material, or we can specify the combination of elements and materials by ourselves. The composition can be specified as molecule/compound (fixed stoichiometry/number of atoms) or mixture (mass fraction), either way Geant4 will use effective elemental composition, and material effects are ignored::

    #include "globals.hh" // G4cout, G4endl

    #include "G4SystemOfUnits.hh"

    using CLHEP::g;
    using CLHEP::mole;
    using CLHEP::cm3;

    #include "G4Isotope.hh"
    #include "G4Element.hh"
    #include "G4Material.hh"

    int main() {

        std::string name, symbol;
        double z,a,n, density, abundance, massFraction;
        int numberIsotopes, numberOfAtoms, numberComponents;


        // way 1: create material of a single element, Geant4 determine the element and its natural composition

        G4Material * matDiamond = new G4Material("Diamond", z = 6, a=12.01*g*mole, density = 3.52*g/cm3 );

        G4cout << matDiamond << G4endl;

        // way 2: create material from a set of elements, only need to provide name and density
        //        particularly 1 single element with non-natural isotopic composition

        G4Element * elC = new G4Element(name="PrehistoricCarbon", symbol="C", numberIsotopes = 2);

        G4Isotope * iso12C = new G4Isotope("C12", z = 6, a = 12);
        G4Isotope * iso13C = new G4Isotope("C13", z = 6, a = 13);
        elC->AddIsotope(iso12C, abundance=0.95);
        elC->AddIsotope(iso13C, abundance=0.05);

        G4Material * matPrehistoricDiamond = new G4Material("PrehistoricDiamond", density = 3.52*g/cm3, numberComponents=1);
        matPrehistoricDiamond->AddElement(elC, massFraction=1.0);

        G4cout << matPrehistoricDiamond << G4endl;

        // way 2: create material from a set of elements, eg H2O


        G4Element * elH = new G4Element("Hydrogen","H", z = 1, a = 1.01 *  g/mole);
        G4Element * elO = new G4Element("Oxygen"  ,"O", z = 8, a = 16.00 * g/mole);

        G4Material * matWater = new G4Material("Water", density = 1.00 * g/cm3, numberComponents=2);

        matWater->AddElement( elH, numberOfAtoms=2);
        matWater->AddElement( elO, numberOfAtoms=1);

        G4cout << matWater << G4endl;

        G4Material * matImpureDiamond = new G4Material("ImpureDiamond", density = 3.52*g/cm3, numberComponents=2);
        matImpureDiamond->AddElement(elC, massFraction=0.99);
        matImpureDiamond->AddMaterial(matWater, massFraction=0.01);
        G4cout << matImpureDiamond << G4endl;

    }

And the corresponding output will be::

 Material:  Diamond    density:  3.520 g/cm3   RadL:  12.130 cm   Nucl.Int.Length:  22.771 cm
                       Imean:  81.000 eV   temperature: 293.15 K  pressure:   1.00 atm

   --->  Element: C (C)   Z =  6.0   N =    12   A = 12.011 g/mole
         --->  Isotope:   C12   Z =  6   N =  12   A =  12.00 g/mole   abundance: 98.930 %
         --->  Isotope:   C13   Z =  6   N =  13   A =  13.00 g/mole   abundance:  1.070 %
          ElmMassFraction: 100.00 %  ElmAbundance 100.00 %

 Material: PrehistoricDiamond    density:  3.520 g/cm3   RadL:  12.170 cm   Nucl.Int.Length:  22.796 cm
                       Imean:  81.000 eV   temperature: 293.15 K  pressure:   1.00 atm

   --->  Element: PrehistoricCarbon (C)   Z =  6.0   N =    12   A = 12.050 g/mole
         --->  Isotope:   C12   Z =  6   N =  12   A =  12.00 g/mole   abundance: 95.000 %
         --->  Isotope:   C13   Z =  6   N =  13   A =  13.00 g/mole   abundance:  5.000 %
          ElmMassFraction: 100.00 %  ElmAbundance 100.00 %

 Material:    Water    density:  1.000 g/cm3   RadL:  36.092 cm   Nucl.Int.Length:  75.356 cm
                       Imean:  68.998 eV   temperature: 293.15 K  pressure:   1.00 atm

   --->  Element: Hydrogen (H)   Z =  1.0   N =     1   A =  1.010 g/mole
         --->  Isotope:    H1   Z =  1   N =   1   A =   1.01 g/mole   abundance: 99.989 %
         --->  Isotope:    H2   Z =  1   N =   2   A =   2.01 g/mole   abundance:  0.011 %
          ElmMassFraction:  11.21 %  ElmAbundance  66.67 %

   --->  Element: Oxygen (O)   Z =  8.0   N =    16   A = 16.000 g/mole
         --->  Isotope:   O16   Z =  8   N =  16   A =  15.99 g/mole   abundance: 99.757 %
         --->  Isotope:   O17   Z =  8   N =  17   A =  17.00 g/mole   abundance:  0.038 %
         --->  Isotope:   O18   Z =  8   N =  18   A =  18.00 g/mole   abundance:  0.205 %
          ElmMassFraction:  88.79 %  ElmAbundance  33.33 %

 Material: ImpureDiamond    density:  3.520 g/cm3   RadL:  12.147 cm   Nucl.Int.Length:  22.781 cm
                       Imean:  80.856 eV   temperature: 293.15 K  pressure:   1.00 atm

   --->  Element: PrehistoricCarbon (C)   Z =  6.0   N =    12   A = 12.050 g/mole
         --->  Isotope:   C12   Z =  6   N =  12   A =  12.00 g/mole   abundance: 95.000 %
         --->  Isotope:   C13   Z =  6   N =  13   A =  13.00 g/mole   abundance:  5.000 %
          ElmMassFraction:  99.00 %  ElmAbundance  98.01 %

   --->  Element: Hydrogen (H)   Z =  1.0   N =     1   A =  1.010 g/mole
         --->  Isotope:    H1   Z =  1   N =   1   A =   1.01 g/mole   abundance: 99.989 %
         --->  Isotope:    H2   Z =  1   N =   2   A =   2.01 g/mole   abundance:  0.011 %
          ElmMassFraction:   0.11 %  ElmAbundance   1.32 %

   --->  Element: Oxygen (O)   Z =  8.0   N =    16   A = 16.000 g/mole
         --->  Isotope:   O16   Z =  8   N =  16   A =  15.99 g/mole   abundance: 99.757 %
         --->  Isotope:   O17   Z =  8   N =  17   A =  17.00 g/mole   abundance:  0.038 %
         --->  Isotope:   O18   Z =  8   N =  18   A =  18.00 g/mole   abundance:  0.205 %
          ElmMassFraction:   0.89 %  ElmAbundance   0.66 %


NIST material database
^^^^^^^^^^^^^^^^^^^^^^

Geant4 has a set of predefined elements materials whose properties come from NIST database (density, mean ionization energy, etc). We can retrieve elements by symbol or atomic number, and materials by name (starting by `G4_`). List of NIST elements and materials is found in the documentation: https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html or the code https://github.com/Geant4/geant4/blob/41f2dd79968018de4efb966f2546970b30c4af78/source/materials/src/G4NistMaterialBuilder.cc#L708

The following main program show some functionality of the Geant4 NIST material manager::


    #include "globals.hh" // G4cout, G4endl

    #include "G4NistManager.hh"
    #include "G4Element.hh"
    #include "G4Material.hh"

    int main() {

        G4cout << "Print NIST information that will be used to build element C" << G4endl;
        const G4String elementNameC = "C";
        G4NistManager::Instance()->PrintElement(elementNameC);
        G4cout << "------------------------------------------------------------\n" << G4endl;

        G4cout << "Print NIST element C" << G4endl;
        G4Element * elC = G4NistManager::Instance()->FindOrBuildElement(elementNameC);
        G4cout << elC << G4endl;
        G4cout << "------------------------------------------------------------\n" << G4endl;

        G4cout << "Print NIST element with Z=1" << G4endl;
        const G4int elementZ = 1;
        G4Element * elH = G4NistManager::Instance()->FindOrBuildElement(elementZ);
        G4cout << elH << G4endl;
        G4cout << "------------------------------------------------------------\n" << G4endl;

        G4cout << "Print NIST material Hydrogen" << G4endl;
        const G4String matNameH = "G4_H";
        G4Material * matH = G4NistManager::Instance()->FindOrBuildMaterial(matNameH);
        G4cout << matH << G4endl;
        G4cout << "------------------------------------------------------------\n" << G4endl;

        G4cout << "Print NIST material Air" << G4endl;
        const G4String matNameAir = "G4_AIR";
        G4Material * matAir = G4NistManager::Instance()->FindOrBuildMaterial(matNameAir);
        G4cout << matAir << G4endl;
        G4cout << "------------------------------------------------------------\n" << G4endl;

        G4cout << "Print NIST material Stainless Steel" << G4endl;
        const G4String matNameStainlessSteel = "G4_STAINLESS-STEEL";
        G4Material * matSteel = G4NistManager::Instance()->FindOrBuildMaterial(matNameStainlessSteel);
        G4cout << matSteel << G4endl;
        G4cout << "------------------------------------------------------------\n" << G4endl;

        G4cout << "Print NIST material that does not exist in the database" << G4endl;
        const G4String matNamePhilosopherStone = "G4_PHILOSOPHERS-STONE";
        G4Material * matPhilosopherStone = G4NistManager::Instance()->FindOrBuildMaterial(matNamePhilosopherStone);
        G4cout << matPhilosopherStone << G4endl;
        G4cout << "------------------------------------------------------------\n" << G4endl;



    }

.. tip::

    Geant4 distributes examples that may show the functionality that we need for our application. In our case, this example reviews what we have seen in this section
    https://github.com/Geant4/geant4/blob/41f2dd79968018de4efb966f2546970b30c4af78/examples/extended/electromagnetic/TestEm3/src/DetectorConstruction.cc#L89


And the corresponding output looks like::

    Nist Element: <C>  Z= 6  Aeff(amu)= 12.0107  15 isotopes:
                N: 8  9  10  11  12  13  14  15  16  17  18  19  20  21  22
            mass(amu): 7487.05 8412.36 9330.68 10257.1 11177.9 12112.6 13043.9 13982.3 14917.6 15856.5 16791.9 17731.3 18667.4 19607.3 20545.5
        abundance: 0 0 0 0 0.9893 0.0107 0 0 0 0 0 0 0 0 0
    Element: C (C)   Z =  6.0   N =    12   A = 12.011 g/mole
            --->  Isotope:   C12   Z =  6   N =  12   A =  12.00 g/mole   abundance: 98.930 %
            --->  Isotope:   C13   Z =  6   N =  13   A =  13.00 g/mole   abundance:  1.070 %
    Element: H (H)   Z =  1.0   N =     1   A =  1.008 g/mole
            --->  Isotope:    H1   Z =  1   N =   1   A =   1.01 g/mole   abundance: 99.989 %
            --->  Isotope:    H2   Z =  1   N =   2   A =   2.01 g/mole   abundance:  0.011 %
    Material:     G4_H    density:  0.084 mg/cm3  RadL:   7.528 km   Nucl.Int.Length:   4.179 km
                        Imean:  19.200 eV   temperature: 293.15 K  pressure:   1.00 atm

    --->  Element: H (H)   Z =  1.0   N =     1   A =  1.008 g/mole
            --->  Isotope:    H1   Z =  1   N =   1   A =   1.01 g/mole   abundance: 99.989 %
            --->  Isotope:    H2   Z =  1   N =   2   A =   2.01 g/mole   abundance:  0.011 %
            ElmMassFraction: 100.00 %  ElmAbundance 100.00 %

    Material:   G4_AIR    density:  1.205 mg/cm3  RadL: 303.921 m    Nucl.Int.Length: 710.095 m
                        Imean:  85.700 eV   temperature: 293.15 K  pressure:   1.00 atm

    --->  Element: C (C)   Z =  6.0   N =    12   A = 12.011 g/mole
            --->  Isotope:   C12   Z =  6   N =  12   A =  12.00 g/mole   abundance: 98.930 %
            --->  Isotope:   C13   Z =  6   N =  13   A =  13.00 g/mole   abundance:  1.070 %
            ElmMassFraction:   0.01 %  ElmAbundance   0.02 %

    --->  Element: N (N)   Z =  7.0   N =    14   A = 14.007 g/mole
            --->  Isotope:   N14   Z =  7   N =  14   A =  14.00 g/mole   abundance: 99.632 %
            --->  Isotope:   N15   Z =  7   N =  15   A =  15.00 g/mole   abundance:  0.368 %
            ElmMassFraction:  75.53 %  ElmAbundance  78.44 %

    --->  Element: O (O)   Z =  8.0   N =    16   A = 15.999 g/mole
            --->  Isotope:   O16   Z =  8   N =  16   A =  15.99 g/mole   abundance: 99.757 %
            --->  Isotope:   O17   Z =  8   N =  17   A =  17.00 g/mole   abundance:  0.038 %
            --->  Isotope:   O18   Z =  8   N =  18   A =  18.00 g/mole   abundance:  0.205 %
            ElmMassFraction:  23.18 %  ElmAbundance  21.07 %

    --->  Element: Ar (Ar)   Z = 18.0   N =    40   A = 39.948 g/mole
            --->  Isotope:  Ar36   Z = 18   N =  36   A =  35.97 g/mole   abundance:  0.337 %
            --->  Isotope:  Ar38   Z = 18   N =  38   A =  37.96 g/mole   abundance:  0.063 %
            --->  Isotope:  Ar40   Z = 18   N =  40   A =  39.96 g/mole   abundance: 99.600 %
            ElmMassFraction:   1.28 %  ElmAbundance   0.47 %

    Material: G4_STAINLESS-STEEL    density:  8.000 g/cm3   RadL:   1.738 cm   Nucl.Int.Length:  16.678 cm
                        Imean: 282.977 eV   temperature: 293.15 K  pressure:   1.00 atm

    --->  Element: Fe (Fe)   Z = 26.0   N =    56   A = 55.845 g/mole
            --->  Isotope:  Fe54   Z = 26   N =  54   A =  53.94 g/mole   abundance:  5.845 %
            --->  Isotope:  Fe56   Z = 26   N =  56   A =  55.93 g/mole   abundance: 91.754 %
            --->  Isotope:  Fe57   Z = 26   N =  57   A =  56.94 g/mole   abundance:  2.119 %
            --->  Isotope:  Fe58   Z = 26   N =  58   A =  57.93 g/mole   abundance:  0.282 %
            ElmMassFraction:  74.62 %  ElmAbundance  74.00 %

    --->  Element: Cr (Cr)   Z = 24.0   N =    52   A = 51.996 g/mole
            --->  Isotope:  Cr50   Z = 24   N =  50   A =  49.95 g/mole   abundance:  4.345 %
            --->  Isotope:  Cr52   Z = 24   N =  52   A =  51.94 g/mole   abundance: 83.789 %
            --->  Isotope:  Cr53   Z = 24   N =  53   A =  52.94 g/mole   abundance:  9.501 %
            --->  Isotope:  Cr54   Z = 24   N =  54   A =  53.94 g/mole   abundance:  2.365 %
            ElmMassFraction:  16.90 %  ElmAbundance  18.00 %

    --->  Element: Ni (Ni)   Z = 28.0   N =    59   A = 58.693 g/mole
            --->  Isotope:  Ni58   Z = 28   N =  58   A =  57.94 g/mole   abundance: 68.077 %
            --->  Isotope:  Ni60   Z = 28   N =  60   A =  59.93 g/mole   abundance: 26.223 %
            --->  Isotope:  Ni61   Z = 28   N =  61   A =  60.93 g/mole   abundance:  1.140 %
            --->  Isotope:  Ni62   Z = 28   N =  62   A =  61.93 g/mole   abundance:  3.635 %
            --->  Isotope:  Ni64   Z = 28   N =  64   A =  63.93 g/mole   abundance:  0.926 %
            ElmMassFraction:   8.48 %  ElmAbundance   8.00 %
    ------------------------------------------------------------

    Print NIST material that does not exist in the database
    Segmentation fault (core dumped)

The segmentation fault message at the end is caused by the use of a null pointer. The NIST manager does not know any material with name ``G4_PHILOSOPHERS-STONE``, and it returns silently a null pointer. In the following line, we try to print the information of the pointer, and the program crashes. This issue is common when developping a Geant4 application. To ensure our program runs without problem, we have to add a protection against null pointers::

        G4cout << "Print NIST material that does not exist in the database" << G4endl;
        const G4String matNamePhilosopherStone = "G4_PHILOSOPHERS-STONE";
        G4Material * matPhilosopherStone = G4NistManager::Instance()->FindOrBuildMaterial(matNamePhilosopherStone);
        if(nullptr != matPhilosopherStone){
            G4cout << matPhilosopherStone << G4endl;
        }
        else {
            G4cout << "Material <" << matNamePhilosopherStone << "> is not present in the NIST database" << G4endl;
        }

.. tip::

    Always check if a pointer is null before using it.


.. admonition:: **Take-home**
   :class: takehome

   Geant4 gives control on the material definition down to isotopic composition. However, Geant4 predefined NIST materials cover most of the needs and ensures consistency of material properties. We will use NIST materials from now on.


.. TODO: is there a difference between material compound, molecule or mixture, or Geant4 treats all the same?

Geant4 geometry model
.....................

The geometry model of Geant4 is made of 3 conceptual layers: solid, logical (volume) and physical (volume). Users must construct them directly in their user code (Detector Construction; we will see it later) by using "new". The objects get registered in the corresponding store (G4SolidStore, G4LogicalVolumeStore, G4PhysicalVolumeStore), which will take care of memory deallocation at the end.

Geometries can be as close to reality as needed. However, we will focus only in the parts that we need. More information on the detector geometry description can be found in the documentation and the topic is covered extensively in the Advance course in autumn.

The geometry and materials can be changed only when the Geant4 simulation is at idle state (i.e., no events being simulated). If geometry or material change, we have to pass that information to re-optimize the geometry (if change of geometry) or re-build physics tables (if change of materials). We will see more later.

Geant4 solids: G4VSolid
^^^^^^^^^^^^^^^^^^^^^^^
Geometrical shapes derive from G4VSolid base class. This class provides interface to a number of methods:
- check if a point is inside or not
- compute distance between shape and a given point
- compute the surface normal to the shape at a given point
- compute the extent of the shape

Geant4 makes use of Constructed Solid Geometry (CSG) to define these shapes: G4Box, G4Tube, G4Orb, etc. Geant4 implements all second-order mathematical shapes, and some special shapes like tessellated and boolean solids. Please see documentation and the upcoming advance course for further details. Each of these primitives implements the G4VSolid base class interface methods listed before. The reason of using an interface for shapes is that the implementation of each method is unique for each shape, but from a navigation point of view, it does not matter which shapes it is, only the distance to the surface or its normal. Using an interface decouples the problem of how these quantities are calculated and its use during the simulation.


.. tip::

    We will use only G4Box, please read its documentation

Geant4 Logical volume
^^^^^^^^^^^^^^^^^^^^^

A logical volume encapsulates the shape information (a pointer to an existing G4VSolid derived class) and a number of properties, at least the material it is made of. Other properties that can be attached to a Logical volume are a field, user limits, sensitive detector. Note that in this case G4LogicalVolume is a constructor::

    G4LogicalVolume(G4VSolid* pSolid,      // Pointer to the associated solid primitive.
                    G4Material* pMaterial, // Pointer to the associated material.
              const G4String& name,        // The volume name.
                    G4FieldManager* pFieldMgr = nullptr,        // optional field manager
                    G4VSensitiveDetector* pSDetector = nullptr, // optional Sensitive Det
                    G4UserLimits* pULimits = nullptr,           // optional user limits
                    G4bool optimise = true);

.. admonition:: **Take-home**
   :class: takehome

   The minimal definition of a logical volume requires a shape (G4VSolid) and a single material (G4Material).

Geant4 Physical volume
^^^^^^^^^^^^^^^^^^^^^^

A Physical volume encapsulates the information of a logical volume and its placement. The G4VPhysicalVolume is an abstract base class from which a number of other classes derive:

    -Single placement: class G4PVPlacement corresponds to a unique placement (translation and rotation) of a logical volume. The same logical volume can be placed more than once, minimizing the memory footprint.

    -Repeated placement: classes G4PVReplica, G4PVParameterised, G4PVDivision, G4ReplicatedSlice, are objects that represent multiple placements in a parametric manner (instead of specifying a translation+rotation for each placement).

The same Logical Volume can be placed many times (so there will be a G4VPhysicalVolume for each one). It is mandatory to provide a mother volume in which we are placing a new logical volume. There are two rules for placements:

    -The placed volume (daughter volume) must not protrude the mother volume

    -The placed volume must not overlap with other placed placed volumes within the same mother volume

    -One single physical volume can have no-mother: the toppest placed volume, typically known as "world". The simulation will happen inside this volume.

Geant4 provides tools to run an overlap check at runtime, or we can construct the placed volumes to check for overlaps before the simulation starts. Checking for overlaps may take some time (depending on the complexity), and therefore the first method is usually more convenient.

.. admonition:: **Take-home**
   :class: takehome

   A physical volume corresponds to the placement of a logical volume. One logical volume can be placed more than once. The placement can be unique (G4PVPlacement) or multiple  ( G4PVReplica, G4PVParameterised, G4PVDivision, G4ReplicatedSlice). Please ensure that there are no overlaps in the resulting geometry.


.. admonition:: **What's next?**
   :class: whatsnext

   We have reviewed in this section how to build materials and the three layers of the geometry model, so we are ready to start building our own application. Implementation details are given in a later section :ref:`YourDetectorConstruction-class`.
