
.. _Visualization:

Visualization
-------------

Different visualization engines (Qt, dawn, VTK, etc) can be linked to Geant4, and they allow to visualize a number of features:
- Geant4 simulation data:
    - detector geometry,
    - the particle trajectory,
    - the individual hits,
    - primitive quantities (energy, flux, etc),
    - fields
    - histograms
- User defined objects:
    - polylines (e.g. axis)
    - markers
    - text

Further documentation can be found here: http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Visualization/visualization.html

The different types of drivers and features were described here: Comput. Phys. Comm. 178 (2008) 331-365

From controlling point of view:
- some visualization drivers work directly from Geant4: OpenGL, OpenInventor, RayTracer, ASCIITree
- for other visualization drivers, a (special) file must be first produced by Geant4 then this file will be rendered by another application: HepRep, DAWN, VRML, gMocren

The Geant4 code stays basically the same independently from the choice of the driver. Visualization is performed either with commands or from C++ code. For the present tutorial, we confine ourselves to command-driven visualization (both in interactive and batch modes)

Availability of drives:
- six of the visualization drivers are always included by default (since they require no external libraries): RayTracer, ASCIITree, HepRep, DAWN, VRML, gMocren
- other visualization drives (e.g. OpenGL, OpenInventor) will be included only if they were explicitly required during the Geant4 build (through cmake using the appropriate cmake option):
    - DGEANT4_USE_OPENGL_X11=ON OpenGL visualization driver with X11 window
    - DGEANT4_USE_QT=ON Qt GUI with OpenGL visualization driver
    -
in all cases some headers and libraries (X11, Qt, OpenGL or MesaGL) need to be available on the system on your virtual machine, Geant4 is available with Qt GUI and OpenGL support

Qt GUI with OpenGL visualization driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Recent developments focused on this combination. Documentation is available at Qt+OpenGL, http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Visualization/visdrivers.html#qt

Geant4 OpenGL tutorial with commands here http://geant4.slac.stanford.edu/Presentations/vis/G4OpenGLTutorial/G4OpenGLTutorial.html

To visualize the geometry, we need a driver, for instance `OGL` and ask to draw the volumes (if no arguments are passed, it will draw all)::

    /vis/open OGL
    /vis/drawVolume

If the Run Manager state is not idle, that is, the initalize step is not done yet, it will print out this error::

    /vis/drawVolume
    ERROR: G4VisCommandSceneAddVolume::SetNewValue:
    No world.  Maybe the geometry has not yet been defined.
    Try "/run/initialize"

    -------- WWWW ------- G4Exception-START -------- WWWW -------
    *** G4Exception : visman0106
        issued by : G4VSceneHandler::ProcessScene
    The scene has no extent.
    *** This is just a warning message. ***
    -------- WWWW -------- G4Exception-END --------- WWWW -------


Most of the Geant4 examples comes with a visualization macro (`vis.mac`), and we will learn how to write it in :ref:`IntermediateApplicationVisualization`.
