.. _PhysicsLists:

Physics list
------------

This section covers briefly the physics list, one of the **three mandatory objects** that the user must provide to the ``G4RunManager``.

A *Physics List* is an object that specify all particles used in the simulation, and the attached physics processes to each particle that describe how it interacts with a given material.

Geant4 provides a flexible way to configure the physics environment. The user can:

  - Choose which particles to include
  - Select which physics processes to assign to each particle

  .. warning::

     Defining a physics list from scratch requires proper physics understanding. Omitting relevant particles or interactions may lead to poor simulation results.

There is no universal default physics configuration suitable for all simulations. Different models describe the same interaction with trade-offs of **accuracy** and **computational cost**. For instance, doing a maximum detailed simulation of HEP detectors would be a waste of CPU time. Also, most simulations do not require all possible particles/interactions.

Geant4  provides independent physics components (processes), and users build their own Physics Lists by selecting components, or even defining their own custom processes. The granularity can be compared to the one of materials: we can build our own material with custom isotopic composition, or we can use the Geant4 NIST material definition


.. admonition:: **Take-home**
   :class: takehome

   Different models describe reality with different accuracy or in different energy ranges. Geant4 offer a large set of already implemented processes, and the possibility to implement new custom ones (because the interfaces to do so are public). A collection of particles and processes is called physics list, and Geant4 also provide some default ones.

The Geant4 Physics List Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Our physics list must derive from ``G4VUserPhysicsList``, and we should implement at least ``ConstructParticle`` and ``ConstructProcess`` methods, and optionally ``SetCuts``.

- ConstructParticle: defines the particles used in the simulation, by either individual construction or by using helper classes. Particle definition is global and static, that means it does not change during the simulation (the name, charge, mass, etc, does not change during the simulation).


.. tip::

   Try to create an electron `G4ParticleDefinition` (retrieved via `G4ParticleTable->FindParticle("e-");` or `G4Electron::Definition();`) and printout its mass.

- ConstructProcess: defines the physics processes and attach them to each particle. A process describes how a particle interacts with matter through a given interaction, e.g. an electron ionization process. Transportation is a special process that determine how the particle interact with the geometry (no physics, just navigation).

Geant4 includes the definition of tens of particles, but it is possible to introduce new particles (and their corresponding processes).

.. note::

   This part of the implementation can become complex for beginners.


Modular and Reference Physics Lists
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Geant4 provides *physics modules* that handle a well defined category of physics such as EM, hadronic, nuclear decay, optical physics, etc. Transportation is automatically added to all particles

The toolkit also provide *Reference Physics Lists* that include predefined combinations of EM and hadronic physics models. These are ready-to-use Physics Lists, but the user is responsible for validating it.

Characteristics:

- Used by large collaborations (e.g., ATLAS, CMS)
- Delivered "as-is" (but it might need some custom configuration)
- The user is responsible for validation

.. seealso::

   Please see the Guide for Physics List:
   http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/PhysicsListGuide/html/index.html


How to retrieve and implement a reference physics list is detailed in a later section :ref:`how-to-reference-physics-list`.
