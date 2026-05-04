
.. _MT:

Introduction to Geant4 Multithreading
-------------------------------------

Simulation events are fully independent, therefore can be processed in parallel. On the other hand, multiple CPU offer the possibility of executing parallel tasks. There are 2 main schemas :

- Multi-processing (MP): it means we run multiple program instances. Therefore, the memory or data are not shared, and the total memory usage can be high. In case of Geant4 application, we would be responsible for providing different initial random seed to each instance, otherwise the result may be the same for all.

- Multi-threading (MT): it means we run a single program, with multiple threads. The main program have a shared memory, therefore the total memory usage can be lower than in MP case. However, it requires

  * Data distribution before run (created as thread local). For example, a variable to accumulate energy deposited during the event-
  * Data collection after run


The CPU usage in multi-processing schema may be limited by the availability of memory. while multithreading allow to share memory and therefore push the memory limit leading to maximal usage of CPU. However, there may be cases where geometry and/or physics is simple, so memory consumption is low, and MP is acceptable (still paying attention to setting different random number seeds to simulate independent events, or in case input data is needed, how different threads access this shared resource)

Geant4 MT model
"""""""""""""""

Geant4 has to be configured and built with MT enabled (default) to be able to create the necesary components for MT. The run manager will create a master thread and one or more worker threads. The master thread does not run any simulation, only orchestrate the begin and end of the set of runs. Geometry and physics configuration cannot be changed during the run, therefore it is shared among the workers. Anything that the worker threads can modify (run, event, track, step, etc) is created as thread local data.

To write a Geant4 application which is compliant with Geant4 MT model, we need to write a method `BuildForMaster` of our class derived from `G4VUserActionInitialization`, and register our derived `G4UserRunAction`. This method will be invoked by the master thread only once. The `Build` method (that does not change) will be inkoved by the worker threads.

The simulation flow in MT is the following:

- Each thread (master and workers) have their own G4UserRunAction object (independent from the rest)

- Before the run starts, each thread invokes `G4Run* G4UserRunAction::GenerateRun()` interface method, and create their own independent run object (G4Run or derived class if implemented by the user)

- While simulation is running (between Begin and End of Run), the **run object owned by master is not used**. Worker threads may populate their own run object with some information (e.g., the number of simulated events)

- At the end of the run, the master thread invoke the method `G4Run::Merge(const G4Run* run_i)` method of its own `G4Run` object, passing as argument the `G4Run` object of each worker thread.

- Then, the `G4Run` object of the master thread has collected all the information from each worker run object (e.g., the total number of events)


If we want to collect custom information during the run (e.g., energy deposited per event, energy spectrum, etc), we need to implement our own derived class from `G4Run` and implement its method `Merge`, so the results can be summed up at the end of the simulation

Notes
"""""

- Master only uses RunAction. This run action may be different from worker run action, for example to write output file at the end of the run
- Workers use all actions
- We can use `G4UserRunAction::IsMaster()` to identify in which thread we are
- Each thread has its own instances

TODO: in MT mode, we need to setup different seeds per thread?

The implementations details are detailed in :ref:`ApplicationMultithreading`
