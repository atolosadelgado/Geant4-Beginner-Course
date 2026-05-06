
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

In this course we will write a Geant4 application to simulate the energy deposited by an electron beam in a thin layer of silicon. We can compile and run the final application that we can find in the  `git repository of the course <https://github.com/mnovak42/Geant4-Beginner-Course>`__

When we see a simulated event, what do we see in the main display pad? (ignore the menus on the top and left side)

- A small cube inside another bigger cube
- A particle entering the central volume (silicon sensor)
- Several particles being created inside the small cube

At the end of the course you will have written each line of code of this application.


.. admonition:: **What's next?**
   :class: whatsnext

   Before starting the implementation, we will set up the working environment (virtual machine, basic Unix tools, and build system) and write our first simple C++ program.
