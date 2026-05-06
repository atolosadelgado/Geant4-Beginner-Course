
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
    - text (e.g., titles, labels)

Further documentation can be found here: http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Visualization/visualization.html

The different types of drivers and features were described here: Comput. Phys. Comm. 178 (2008) 331-365

From controlling point of view:

- some visualization drivers work directly from Geant4: OpenGL, OpenInventor, RayTracer, ASCIITree
- for other visualization drivers, a (special) file must be first produced by Geant4 then this file will be rendered by another application: HepRep, DAWN, VRML, gMocren

The Geant4 code stays basically the same independently from the choice of the driver. Visualization is performed either with commands or from C++ code. For the present tutorial, we confine ourselves to command-driven visualization (both in interactive and batch modes)

Availability of drivers:

- six of the visualization drivers are always included by default, because they require no external libraries and they are non-interactive (but they are configurable using UI commands or C++):
    - ASCIITree : to printout the geometry tree and associated properties to each solid/logical/physical volume (the output can be redirected to a file using a dedicated UI command)
    - TSG_OFFSCREEN : creates a png in batch mode
    - DAWN : for high fidelity geometry visualization
    - VRML :
    - HepRep,
    - RayTracer,
    - gMocren,

- other visualization drivers (e.g. OpenGL, OpenInventor) will be available only if they were explicitly configured during the Geant4 build through cmake using the appropriate cmake option:

    - DGEANT4_USE_OPENGL_X11=ON : OpenGL visualization driver with X11 window
    - DGEANT4_USE_QT=ON : Qt GUI with OpenGL visualization driver

in all cases some headers and libraries (X11, Qt, OpenGL or MesaGL) need to be available on the system on your virtual machine, Geant4 is available with Qt GUI and OpenGL support

For the sake of completeness, this is the error message when trying to use OpenGL driver but Geant4 was not compiled with that option::

        Idle> /vis/open OGL
        parameter value (OGL) is not listed in the candidate List.
            Candidates are: ASCIITree ATree DAWNFILE G4HepRepFile HepRepFile RayTracer VRML2FILE gMocrenFile TOOLSSG_OFFSCREEN TSG_OFFSCREEN TOOLSSG_OFFSCREEN TSG_OFFSCREEN TSG_FILE TSG
        sub-command "/vis/sceneHandler/create" failed.
        Registered graphics systems are:
            ASCIITree (ATree)
            DAWNFILE (DAWNFILE)
            G4HepRepFile (HepRepFile)
            RayTracer (RayTracer)
            VRML2FILE (VRML2FILE)
            gMocrenFile (gMocrenFile)
            TOOLSSG_OFFSCREEN (TSG_OFFSCREEN)
            TOOLSSG_OFFSCREEN (TSG_OFFSCREEN, TSG_FILE, TSG)
        Default graphics system is: ogl (based on G4VisExecuitive argument).
        Default window size hint is: 600x600-0+0 (based on G4VisManager initialisation).
        Note: Parameters specified on the command line will override these defaults.
            Use "vis/open" without parameters to get these defaults.

        -------- WWWW ------- G4Exception-START -------- WWWW -------
        *** G4Exception : UIMAN0123
            issued by : G4UImanager::ApplyCommand
        Invoked command has failed - see above.
        Error code : 500
        *** This is just a warning message. ***
        -------- WWWW -------- G4Exception-END --------- WWWW -------

        Parameter is out of candidate list (index 0)
        Candidates :


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


Please check some Geant4 examples and their visualization macro file (usually named `vis.mac`)

.. admonition:: **What's next?**
   :class: whatsnext

   In the next chapter, :ref:`IntermediateApplicationVisualization`, we will show how to visualize the detector and a simulated event.
