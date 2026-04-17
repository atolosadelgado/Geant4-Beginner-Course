
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

Format of the course
.....................................

We will follow a build-from-scratch approach:

* start from a minimal C++ program
* introduce few Geant4 components step by step
* progressively assemble a full simulation application

Each new concept will be introduced only when it becomes necessary for the next step of implementation.

We will frequently refer to the Geant4 Application Developer Guide and inspect example applications provided with the toolkit.


.. admonition:: **Take-home**
   :class: takehome

   Geant4 is a toolkit, not a standalone program.
   You build your own simulation by combining its components, just like assembling building blocks.


.. tip::

   The example applications, provided by the **toolkit developers** as part of the toolkit itself, serve as a good starting point for developing your own application. Try to find the closest to your needs, use it as a starting point and modify, extend according to your own modelling problem.

Live demo
.....................................

Visualize an event either with the final application or an extended example

Our goal in the next few days
.......................................

In the next few days we will build step by step a simple particle transport application using the :guilabel:Geant4 toolkit.

We will start from very simple C++ programs and progressively during the course we will introduce the Geant4 components needed to reproduce the type of simulation you have just seen in the demonstration:
- a detector geometry
- a set of physics processes
- a primary particle generator
- a run control mechanism


.. admonition:: **What's next?**
   :class: whatsnext

   Before starting the implementation, we will set up the working environment (virtual machine, basic Unix tools, and build system) and write our first simple C++ program.

