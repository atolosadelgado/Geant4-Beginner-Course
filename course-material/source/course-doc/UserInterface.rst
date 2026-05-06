
.. _UserInterface:

User interface
--------------

Geant4 is a toolking, a collection of classes that allow us to define our problem (detector description, physics, actions). The solution is provided by Geant4 when we assemble the pieces in proper manner and run the simulation.

Assembling the pieces of Geant4 toolkit is also known as developping a Geant4 application. This require some knowledge of C++ and the toolkit itself. However, the end-user may configure the application without the need of touching C++ by using the so-called User Interfaces.

Some parts of the toolkit implement an User Interface, for example most of the physics modules or the `G4ParticleGun`, allowing us to configure the physics and the primary generator respectively. In addition. the application developer can implement new User Interfaces.

The UI commands consist in 3 parts. For example, in the previous UI command that we used to show the steps during the simulation `/tracking/verbose 1` has a directory `/tracking/`, a command `verbose` and a parameter `1`. The directory structure is used to group the commands (not only tracking may have a command to configure its verbosity). The parameters sometimes are optional, are given separated by spaces, and can be any basic type (strings should be delimited by quotes). An exclamation mark can be used as a placeholder for a parameter without specifying its value (the default value will be used).

There are 3 ways of submitting the UI commands:

- batch mode. Only `G4UImanager` manager is needed. There are two options for batch mode:

    - batch mode hardcoded commands in the application, like here :ref:`RunManagerInitializeBeamOn`::

            // after initializing Run Manager
            G4UImanager * UImanager = G4UImanager::GetUIpointer();
            UImanager->ApplyCommand("/tracking/verbose 1");

    - batch mode using a macro file, by gathering all the commands in a text file and feed it to the UI manager (more in the next section)::

            // after initializing Run Manager
            G4UImanager * UImanager = G4UImanager::GetUIpointer();
            UImanager->ApplyCommand("/control/execute macro_file_name");

- (G)UI interactive command submission. A dedicated manager is needed, `G4UIExecutive` in addition to the UI manager, to keep the interactive loop::

            // before creating Run Manager
            G4UIExecutive* ui = new G4UIExecutive(argc, argv);

            // after initializing Run Manager
            ui->SessionStart();

            // before finishing the program
            delete ui;




The availability of the commmands may vary depending on the state of the run manager (for example `/run/beamOn 1` is available only in idle state). Some comands require the Run Manager to be initialized first, or the opposite.

Commands may be refused for a number of reasons:

- wrong run manager state
- wrong type of parameter
- wrong number of parameters
- parameter out of range (numerical parameters only)
- parameter is not present in candidate list (string only)
- command not found

The example `basic/B1/exampleB1.cc` shows how to run a macro file in batch mode if it is provided as main argument, otherwise it starts an interactive session::

    int main(int argc, char** argv)
    {
    // Detect interactive mode (if no arguments) and define UI session
    G4UIExecutive* ui = nullptr;
    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);
    }

    // ...

    // Get the pointer to the User Interface manager
    auto UImanager = G4UImanager::GetUIpointer();

    // Process macro or start UI session
    if (!ui) {
        // batch mode
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }
    else {
        // interactive mode
        ui->SessionStart();
        delete ui;
    }

Geant4 provides many different types of interfaces, either graphical (Qt-GUI, java-based GAG-GUI, Motif-based Xm-GUI) or shell-like terminals (tcsh, csh). All types derive from the base class `G4UIsession`. After being created, the method `SessionStart()` has to be called to set it up. By instantiating a `G4UIExecutive` object, Geant4 selects the most appropriate UI type available in the current environment (Graphical UI sessions have higher priority than terminal-like ones). We can also tell `G4UIExecutive` to try to select one in particular, for instance to select `tcsh` instead of others we can pass it as optional argument::

    ui = new G4UIExecutive(argc, argv, "tcsh");

.. note::

    Further details can be found in the `developers guide <http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Control/control.html>`__.  In particular, a list of built-in UI commands can be found `here <http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Control/commands.html>`__.

    New UI commands can be defined by the application developer as described `here <http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Control/userInterfaceCommand.html>`__.
    In the last stage of the application implementation, we will use this feature, see :ref:`ApplicationCustomUIcommands`.

One can use the application to get a list of available commands including the custom ones by:

- plain text format to standard output `/control/manual [directory]`
- HTML file(s) - one file per one (sub-)directory `/control/createHTML [directory]`

Macro files
^^^^^^^^^^^

Macro files are text file with a list of UI commands. All commands must be defined with the full directory. If the line starts by `#` it is consider a comment.

Macro files can be executed interactively, by typing::

    > /control/execute macro_file_name

or by hardcoding the execution in C++ as we saw before::

            G4UImanager * UImanager = G4UImanager::GetUIpointer();
            UImanager->ApplyCommand("/control/execute macro_file_name");

The use cases are many. For example, the macro file can configure the particle gun. So to run a simulation with different primary particles, we can re-use the C++ and only provide different

Interactive session
^^^^^^^^^^^^^^^^^^^

Interactive terminal can interpret some commands that have no effect on the Geant4 kernel and they cannot be used in a macro file (only in interactive session)

- cd, pwd : change and display current command directory. By setting the current command directory, you may omit (part of) directory string
- ls : list available UI commands and sub-directories
- history : show previous commands
- !historyID : re-issue previous command
- arrow keys and tab (TC-shell only)
- ?UIcommand : show current parameter values of the command
- help [UIcommand] : help
- exit : job termination

.. admonition:: **What's next?**
   :class: whatsnext

   In the next chapter, :ref:`IntermediateApplicationUI`, we will show how to add a user interface and use it to configure the simulation.
