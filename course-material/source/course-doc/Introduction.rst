
.. _ref-Introduction:

Introduction
--------------

What is :guilabel:`Geant4`?
....................................

The :guilabel:Geant4 toolkit provides building blocks to describe and simulate the transport of particles through matter using Monte Carlo methods.

As users of the toolkit, we do not modify its internal physics models. Instead, we build applications by combining predefined components in a modular way.

Key concepts
.......................................

To understand how we will work with the :guilabel:Geant4 toolkit, we first clarify a few basic concepts:

*Software toolkit*

   A software toolkit is a collection of classes and interfaces designed to be reused and combined to build applications. A toolkit is not a program by itself.

   The :guilabel:Geant4 toolkit contains thousands of such classes implementing geometry, physics processes, tracking, and more.

*Application*

   An application is a program that is executed by the user. It can make use of external libraries or toolkits. Geant4 does not provide a ready-to-run application.

*Geant4 application*

   A Geant4 application is a user-written program that combines a selected set of Geant4 classes to define and run a particle transport simulation.

*Link to our course*

   In this course, we will progressively build a Geant4 application from scratch, starting from a minimal C++ program and gradually introducing the necessary toolkit components until we reach a complete simulation similar to the one shown in the introductory demonstration.


Live demo
.....................................

Visualize an event either with the final application or an extended example


Introduction to key Geant4 components
.....................................

Geant4 does not provide a (main) *program*. Geant4 is a toolkit that provides the necessary components to describe and run a simulation. The user has to assemble these components in such a manner that the specific simulation problem is described properly.

The general flow of a Geant4 simulation is the following:

   - A geometry description and a physics list (set of particles and their interaction models with matter) must be provided before running a simulation

   - A *run* begins when the simulation starts. One run will simulate a certain number of *events*

   - An *event* begins when some particles are injected into the simulation. These first particles are called *primary particles*

   - Each particle is tracked separately (they do not know about each other). Each particle can create other particles, called *secondary particles* (e.g. by ionization)

   - When particles are injected into the simulation, they are placed in a *stack* to wait. Then, in a loop, Geant4 simulates the particles one by one, and start simulating its pasage through matter, until either is absorbed, destroyed, decays, thermalizes or reach the end of the world. If new secondary particles are created, they are added to the stack

   - In Geant4, a *track* represents a status of a given particle (postion, momentum, polarization, etc). The Geant4 *track* is not related to the HEP concept of *reconstructed track*

   - A particle is pushed to jump a *step* by Geant4 navigator, and each step is limited in length by either geometry boundaries or physical processes

   -The event ends when all primary and the subsequent secondary particles are track to the end (when particle is either absorbed, destroyed, decayed, thermalized or reaches the end of the world)

The mandatory components to run a Geant4 simulation are the geometry description, the physics and the primary particle generation. These components are registered in a central object that we have to create in our own main program, called **G4RunManager**. We will review them in the following sections, and come back to them when we write the corresponding code.

.. admonition:: **Take-home**
   :class: takehome

   Geant4 is a toolkit, not a standalone program.
   You build your own simulation by combining its components, just like assembling building blocks.


The G4RunManager
^^^^^^^^^^^^^^^^

The G4RunManager is the only mandatory manager object that user the needs to create. It is responsible to control the flow of a run, the top level simulation unit, including initialisation of the run (building, setting up the simulation environment). All problem specific information need to be given to the G4RunManager by the user through the
interfaces provided by the Geant4 toolkit (the concept of interface is reviewed in the following section):

   - G4VUserDetectorConstruction (mandatory): how the geometry should be constructed, built

   - G4VUserPhyscsList (mandatory): all the particles and their physics interactions to be simulated

   - G4VUserActionInitialization (mandatory):

      - G4VUserPrimaryGeneratorAction (mandatory): how the primary particle(s) in an event should be produced

      - additional, optional user actions (G4UserRunAction, G4UserEventAction, G4UserSteppingAction, etc)

- MT note: G4MTRunManager object needs to be created in case of Geant4 MT

The G4VUserDetectorConstruction
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The G4VUserDetectorConstruction interface is provided by the Geant4 toolkit to
describe the geometrical setup, including all volumes with their shape, position and
material definition.

Its G4VUserDetectorConstruction::Construct() interface method (pure virtual) is
invoked by the G4RunManager at initialisation

We have to implement our own detector description, e.g. YourDetectorConstruction class, derived from this
base class and (at minimum) implement the Construct() interface method. This method is expected to perform the following:
- create all materials will need to use in your geometry
- describe your detector geometry by creating and positioning all volumes
- return the pointer to the root of your geometry hierarchy i.e. the pointer to your “World”
G4VPhysicalVolume

Then, we have to create a YourDetectorConstruction object and register it in your G4RunManager object
by using the G4RunManager:SetUserinitialization method (see this in the source!)

MT note: the Construct() interface method is invoked only by the Master Thread in case
of Geant4 MT (i.e. only one detector object), while the other ConstructSDandField()
interface method is invoked by each Worker Threads (i.e. thread local objects created)

.. admonition:: **Take-home**
   :class: takehome
   Minimal detector constructor must build a geometry tree and the corresponding materials

The G4VUserPhysicsList
^^^^^^^^^^^^^^^^^^^^^

This interface is provided to allow the user to define a set of particles and their processes (that determine how each particle interacts with each material). Users can customize it at different levels, but requires extensive knowledge and it must be done with care. Please check the dedicated manual about the physics in Geant4.

In our case, we will use a pre-defined *physics list*, built by an ancillary class *G4PhysListFactory*. Then we have to register it in your G4RunManager object
by using the G4RunManager:SetUserinitialization method (see this in the source!)

Please check the dedicated manual about physics lists. Further details are given in the upcoming advance course.

.. admonition:: **Take-home**
   :class: takehome

   Geant4 provides possibilities with different level of granularity to build up or obtain even complete pre-defined physics list

The G4VUserActionInitialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The G4VUserPrimaryGeneratorAction interface is used to build the so called *user-actions*, contact points between the user and different parts of the simulation flow (primary generation, run, event, stepping, etc).

We have to derive our own action Initialisation, e.g. YourActionInitialization class, from this base class and implement G4VUserActionInitialization::Build() interface method. Inside this method, we have to create an object of each user-action class (derived from the corresponding interface) and register it using G4VUserActionInitialization::SetUserAction() base class method. There is only one mandatory user-action, the primary generation action, that is described in the following section.

The G4VUserPrimaryGeneratorAction
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The G4VUserPrimaryGeneratorAction interface is one of the many user actions is provided by the Geant4 toolkit. It is used to describe how the primary particle(s) in an event should be produced. Its G4VUserPrimaryGeneratorAction::GeneratePrimaries() interface method (pure virtual) is invoked by the G4RunManager during the event-loop (in its G4RunManager::GenerateEvent() method)

We have to derive our own primary generator action, e.g. YourPrimaryGeneratorAction class, from this base class and implement the GeneratePrimaries() interface method, which describes how the primary particle(s) in an event should be produced

Particularly, we will use a G4ParticleGun object, provided by the Geant4 toolkit, to generate primary particles. We will configure it to generate one particle per event with fixed initial kinematics (position, momentum).

Note that the Detector-Construction and the Physics-List need to be created directly in the main program and registered directly in the G4RunManager object. However, all User-Actions needs to be created and registered in a User-Action-Initialisation class (including the only mandatory Primary-Generator-Action as well as all other, optional
User-Actions)


.. admonition:: **Take-home**
   :class: takehome

   The only mandatory user-action is the primary-generator action


Step-by-step plan of the course
...............................

We will follow a build-from-scratch approach:

* start from a minimal C++ program
* introduce few Geant4 components step by step
* progressively assemble a full simulation application

Each new concept will be introduced only when it becomes necessary for the next step of implementation.

We will frequently refer to the Geant4 Application Developer Guide and inspect example applications provided with the toolkit. The example applications, provided by the **toolkit developers** as part of the toolkit itself, serve as a good starting point for developing your own application. Try to find the closest to your needs, use it as a starting point and modify, extend according to your own modelling problem.




In the next few days we will build step by step a simple particle transport application using the :guilabel:Geant4 toolkit.

We will start from very simple C++ programs and progressively during the course we will introduce the Geant4 components needed to reproduce the type of simulation you have just seen in the live demo.

We will first implement an minimal application to run a Geant4 simulation (source code in *intermediate application*). This will run a simulation without recording any data. To do so, we need to implement the mandatory components mentioned before:
- YourDetectorConstruction: a simple box (shape) as the detector/target made of silicon (material), placed in another box (shape) *world* volume filled with low density hydrogen gas (*universe*).
- YourPhysicsList: we will use one of the pre-defined, ready-to-use physics list provided by the Geant4 toolkit (therefore no need to write any user physics list class)
- YourPrimaryGeneratorAction: a simple particle gun (G4ParticleGun) that generates a single primary particle per event with pre-defined particle type and kinematics pointing toward to our target.
- YourActionInitialization: implement the construction and registration of our YourPrimaryGeneratorAction object
- develop the main function (application) and execute the simulation
- add functionality to the main method of our application to be able to run the application in interactive or batch mode, with or without visualisation, and write the corresponding macro files

Afterwards, we will extend the application with optional user-actions to collect data during the simulation. We will record the energy deposited in our target per event, and calculate the average and standard deviation. In addition, we will add custom User Interface (UI) commands to configure the material and thickness of our target.


.. admonition:: **What's next?**
   :class: whatsnext

   Before starting the implementation, we will set up the working environment (virtual machine, basic Unix tools, and build system) and write our first simple C++ program.
