
.. _CustomUIcommand:

Custom UI commands
------------------

Geant4 allows to define new custom UI commands, to configure an application without touching C++ code.

Behind each set of UI commands there is a messenger class, derived from the interface `G4UImessenger`. Then, the messenger defines the UI commands using Geant4 specific classes for that, e.g. `G4UIcmdWithADoubleAndUnit` (see `source/intercoms/include` for other classes). The messenger class need a reference to the objects to act on. The messenger has a virtual method `SetNewValue(G4UIcommand*,G4String )`, that we have to override with our own implementation. The first argument is used to identify the command used, and the second to compute the value, using `G4UIcommand` methods if needed (for example to convert to a number)

The object on which the messenger has to act upon must be known to the object, and viceversa. To avoid this bi-directional loop, we will have to use a forward declaration.

Also, you can take a look how the `G4ParticleGunMessenger` looks like.

.. note::

    How many types of `G4UIcmd*` classes that are used in `G4ParticleGunMessenger.cc`?

