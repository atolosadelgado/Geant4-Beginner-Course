
.. _ref-Introduction:

Introduction
--------------

What is :guilabel:`Geant4`?
...........................

The Geant4 toolkit provides the building blocks required to describe and simulate the transport of particles through matter using Monte Carlo methods

As users of the toolkit, we do not modify its internal physics models. Instead, we build applications by combining predefined components in a modular way.

Key concepts
............

To understand how we will work with the :guilabel:Geant4 toolkit, we first clarify a few basic concepts:

*Software toolkit*

   A software toolkit is a collection of classes and interfaces designed to be reused and combined to build applications. A toolkit is not a program by itself.

   The :guilabel:Geant4 toolkit contains thousands of such classes implementing geometry, physics processes, tracking, and more.

*Application*

   An application is a program that is executed by the user. It can make use of external libraries or toolkits. Geant4 does not provide a ready-to-run application.

*Geant4 application*

   A Geant4 application is a user-written program that combines a selected set of Geant4 classes to define and run a particle transport simulation.

.. admonition:: **Take-home**
   :class: takehome

   Geant4 does not provide a ready-to-run simulation application.
   Instead, it provides reusable components that users combine to build their own simulations.

What :guilabel:`Geant4` is vs What it is not
............................................

+--------------------------------------------------+------------------------------------------------------+
| What :guilabel:`Geant4` *is*                     | What :guilabel:`Geant4` *is not*                     |
+==================================================+======================================================+
| A software toolkit for building simulations      | A ready-to-run simulation application                |
+--------------------------------------------------+------------------------------------------------------+
| A Monte Carlo-based particle transport framework | A detector reconstruction framework                  |
+--------------------------------------------------+------------------------------------------------------+
| Written mainly in C++                            | A realistic event generator                          |
+--------------------------------------------------+------------------------------------------------------+
| A modular set of physics, geometry, and tracking | A fixed black box simulator                          |
| components                                       |                                                      |
+--------------------------------------------------+------------------------------------------------------+
| A framework where the user defines geometry,     | A system that defines your experiment for you        |
| physics and primary particles                    |                                                      |
+--------------------------------------------------+------------------------------------------------------+
| A tool to simulate particle interactions in      | A full data analysis framework                       |
| matter                                           |                                                      |
+--------------------------------------------------+------------------------------------------------------+
| A flexible simulation engine                     | Validated for your detector setup                    |
+--------------------------------------------------+------------------------------------------------------+
| Output must be explicitly defined by the user    | A system that automatically produces ROOT files      |
|                                                  | or analysis output                                   |
+--------------------------------------------------+------------------------------------------------------+
| Highly extensible simulation framework           | A built-in CAD geometry importer (by default)        |
+--------------------------------------------------+------------------------------------------------------+

.. _ref-LiveDemo:

Live demo
.....................................

In this course we will write a Geant4 application to simulate the energy deposited by an electron beam in a thin layer of silicon. We can compile and run the final application that we can find in the  `git repository of the course <https://github.com/mnovak42/Geant4-Beginner-Course>`__

When we display a simulated event, what do we see in the main display pad? (ignore the menus on the top and left side)

- A small cube inside another bigger cube
- A particle entering the central volume (silicon sensor)
- Several particles being created inside the small cube

By the end of the course, you will have written every line of code of this application.

.. admonition:: **What's next?**
   :class: whatsnext

   In this course, we will progressively build a Geant4 application from scratch, starting from a minimal C++ program and gradually introducing the necessary toolkit components until we reach a complete simulation similar to the one shown in the introductory demonstration.

   Before starting the implementation, we will set up the working environment (virtual machine, basic Unix tools, and build system) and write our first simple C++ program.
