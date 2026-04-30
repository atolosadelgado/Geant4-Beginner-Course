
.. _OptionalUserAction:

Optional User Actions
---------------------

The only mandatory user action is the primary generator, because without primaries there is no simulation. We could see the steps during the simulation and visualize some events without these optional user actions. However, we will use these optional user actions as conecting point between our application and Geant4 simulation.

Key concepts of Geant4 tracking
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

What is G4Track
"""""""""""""""

- A ``G4Track`` object represents the state of a particle at a given instant.
- It is a snapshot and does not store past history.
- Contains:

  * ``G4ParticleDefinition`` : static properties: mass, charge, etc. Tracks of the same type share the same ``G4ParticleDefinition``
  * ``G4DynamicParticle`` : dynamic properties: energy, momentum, etc. These properties are updated each time the track is pushed step during simulation

- The interface `G4VUserTrackInformation` can be used to derive a class that stores custom information, and then attached to the `G4Track` object handled by Geant4 with the method `SetUserTrackInformation` (it can be useful for tagging a track with custom information)

- Managed by ``G4TrackingManager``
- Optional user hook:

  * ``G4UserTrackingAction``


What is G4Step
""""""""""""""

- Represents the change in a particle state during a simulation step.
- Contains two points:

  * Pre-step point
  * Post-step point

- These store:

  * Position
  * Energy
  * Direction
  * Material
  * Volume

- Step evolution: Post-step becomes next pre-step. The post-step is simulated based on the pre-step only (no memory of previous steps).

TODO: check if Geant4 is fully Markovian

- Boundary condition: If step ends at geometry boundary:

    - Post-step lies physically on the boundary
    - Logically belongs to next volume

        - To retrieve in which volume the step happened, we need to access the Pre-step point

    - Step status: ``fGeomBoundary``

- Managed by ``G4SteppingManager``

- G4Track object and G4Step know each other:
  * ``G4Step::GetTrack()``
  * ``G4Track::GetStep()``

- Optional user hook:

  * ``G4UserSteppingAction``

G4UserSteppingAction
""""""""""""""""""""

- This optional user action allow us to access information after each simulation step with the virtual method `UserSteppingAction(const G4Step * step)`

    - Notice that its name does not start by `G4V` as other classes we used, but the user class will still derive from this base class, which indicates that it is not pure virtual

    - The base class is virtual but not abstract: it gives a default implementation, see `G4UserSteppingAction.hh`

- Either from the step or the track, we can access all the information about the simulation

    - See `G4Step.hh`, `G4StepPoint.hh` , `G4ParticleDefinition.hh`, `G4DynamicParticle.hh`

- To implement custom stepping action:

  * Derive from ``G4UserSteppingAction``
  * Override virtual method ``UserSteppingAction(const G4Step* theStep)``
  * Register in an object of the class in ``ActionInitialization::Build()``

G4Event
"""""""

A Track is a snapshot of a particle state after each Step. An Event is the basic simulation unit that represents a set of ``G4Track`` objects (at least 1 primary particle). A full tracking of a particle is a collection of steps. The number of events is determined when we execute `/run/beamOn <N>`

Event workflow:

- At the begining of the event, rimary tracks are generated and pushed to a track-stack.
- Tracks are processed (simulated) one by one:

  * the track object is propagated step-by-step, updating its dynamical properties at each step. The step is limited by either physics interaction or geometrical boundary
  * Secondary tracks may be generated along each step, and accumulated in a track-stack for subsequent simulation
  * Continue until termination conditions:

    - Leaving world volume
    - Destructive interaction
    - Zero kinetic energy
    - User termination
  * When one track object reaches its termination condition, a new G4Track object is taken from the stack and its simulation starts. The tracking of each particle happens independently, the simulation has no memory of other tracked particles

- Event ends when the track-stack is empty

- At the end of the event, the corresponding G4Event object will store its input (list of primaries) and some output (trajectory and hit collections)

- ``G4EventManager`` gives access to the G4Event object

- The interface `G4VUserEventInformation` allows to derive a custom class that can be used to collect extra information, and attached to the `G4Event` object using the method `SetUserEventInformation`


G4UserEventAction
"""""""""""""""""

- Provides control before and after an event.

- Virtual methods:

  * ``BeginOfEventAction(const G4Event*)`` , called before a new event processing starts
  * ``EndOfEventAction(const G4Event*)``, called after an event processing is completed

- Typical usage:

  * Begin: clear data structures
  * End: store or aggregate results

- To implement custom Event Action:

  * Derive from ``G4UserEventAction`` and implement the 2 methods listed above
  * Create an object of the derived class and register it in ``ActionInitialization::Build()`` interface method


G4Run
"""""

- Collection of events, which are processed in a loop. Geometry and physics cannot be modified during the run. ``G4RunManager`` gives access to the G4Run object.
- We can derive a user class from G4Run class, see the list of virtual methods in `G4Run.hh`
- The user hook is ``G4UserRunAction``

Run lifecycle:

- Initialization:

  * Geometry constructed
  * Physics initialized

- Start (BeamOn):

  * Geometry optimized
  * Physics tables built



- Optional

G4UserRunAction
"""""""""""""""

- Optional User Action class that provides control before and after a run.

- Methods:

  * ``BeginOfRunAction(const G4Run*)``
  * ``EndOfRunAction(const G4Run*)``

- Typical usage: storing global simulation data

  * Begin: initialize histograms/data structures
  * End: output results

- To use a custom run class, derived from G4Run, we have to:

  * Override ``GenerateRun()``
  * Create custom ``G4Run`` subclass
  * The method ``GenerateRun()`` will be automatically invoked by G4RunManager at initialization to generate the derived G4Run class object

At this point, we can jump to the application and implement custom optional user actions

Multi-threading notes
~~~~~~~~~~~~~~~~~~~~~

Concepts
""""""""

- Event simulation is naturally parallel.
- Events are independent → can be processed in parallel.

Approaches:

Multi-processing (MP)
"""""""""""""""""""""

- Multiple program instances.
- Separate memory spaces.
- No data sharing.
- High memory usage.

Multi-threading (MT)
""""""""""""""""""""

- Single program, multiple threads.
- Shared memory.
- Lower memory usage.
- Requires:

  * Data distribution before run
  * Data collection after run


Geant4 MT model
"""""""""""""""

- Master + Worker threads.

- Shared data:

  * Read-only

- Thread-local data:

  * Modified data (run/event/track/step)


User action initialization
""""""""""""""""""""""""""

Two methods:

- ``BuildForMaster()``

  * Only for master thread
  * Should register only ``G4UserRunAction``

- ``Build()``

  * For worker threads
  * Register all user actions


Run merging in MT
"""""""""""""""""

- Each thread has its own ``G4Run``.
- Workers fill their runs during simulation.
- Master merges results at the end:

  .. math::

     run_{master} = \sum_i run_{worker,i}

- Custom runs must implement:

  * ``Merge(const G4Run*)``


Notes
"""""

- Master only uses RunAction.
- Workers use all actions.
- Each thread has its own instances.
- Random seeds must differ to avoid identical events.
