/************************************************************************
DSPatch - Cross-Platform, Object-Oriented, Flow-Based Programming Library
Copyright (c) 2012 Marcus Tomlinson

This file is part of DSPatch.

GNU Lesser General Public License Usage
This file may be used under the terms of the GNU Lesser General Public
License version 3.0 as published by the Free Software Foundation and
appearing in the file LGPLv3.txt included in the packaging of this
file. Please review the following information to ensure the GNU Lesser
General Public License version 3.0 requirements will be met:
http://www.gnu.org/copyleft/lgpl.html.

Other Usage
Alternatively, this file may be used in accordance with the terms and
conditions contained in a signed written agreement between you and
Marcus Tomlinson.

DSPatch is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
************************************************************************/

#include "../Source/DspCircuit.h"

/**

\mainpage Welcome

	\section intro_sec Introduction
	DSPatch, pronounced "dispatch", is a powerful C++ flow-based programming library that allows you
	to create and route (or "patch") high performance signal processing circuits. DSPatch is not
	limited to any particular type of circuit or signal, its generic object-oriented API allows you to
	create almost any process chain imaginable, from simple logic circuits to full-blown electronics
	simulation. DSPatch's simple framework makes development quick and easy, allowing you to hit the
	ground running on every project.

	The two most important classes to consider are DspComponent and DspCircuit. In order to route
	data to and from DspComponents they can either be added to an DspCircuit, where they can be
	wired together (recommended), or they can be wired directly via public DspComponent methods. The
	DSPatch engine takes care of all data transfer between interconnected components, when data is
  ready for a component to process, a callback: "Process_()" is executed in that component. For a
  component to form part of the DSPatch framework, designers simply have to derive their component
	from the DspComponent base class, configure the component IO in the component constructor, and
	implement the virtual Process_() callback method.

	For more detail on how DSPatch works, check out the <a href="spec_page.html"><b>DSPatch Design Specification</b></a>.

	\n

	\section install_sec Getting Started

		\subsection step1 Step 1: Download DSPatch
			The DSPatch library can be downloaded from the <a href="https://sourceforge.net/projects/dspatch/"><b>SourceForge project page</b></a>. 
			Download the project archive and extract the contents anywhere you like.

		\subsection step2 Step 2: Check out the DspDemo project
			In the "DspDemo" folder (found in the DSPatch root directory) you will find a simple example
			project that uses the DSPatch library. This project has been written to assist developers in
			understanding the DSPatch API as well as to demonstrate how it could be used to build a
			real-time audio process chain. The quickest way to get this project compiled and running is
      to simply open the "DSPatch.sln" (Windows) or "DSPatch.xcodeproj" (Mac OS X) file (found in
      the root, and "Mac OS X\DSPatch" directories respectively) depending on the platform used
      -This solution contains both the DSPatch library and DspDemo project side-by-side as would be
      typical in developing a DSPatch application.

		\subsection step3 Step 3: Create your own project
			To get started all you need to do from your project is #include "DSPatch.h" from the
			"Include" folder (found in the "DSPatch" folder), and link to the DSPatch library (either by
      including all DSPatch source or by linking to a compiled library file). To speed things up
      you may want to copy, rename, and edit the DspDemo project from step 2 to get up and running
      faster.

		\subsection step4 Step 4: Make your own DspComponents
			In the "DspDemo\Source" folder (found in the "DspDemo" folder) you will find 2 source files:
			"DspGain.h" and "DspGain.cpp". These files make up a very simple DspComponent that adjusts
      the gain of the audio data passing through it. Much like the example project, the a fast way
      to create your own DspComponent could be to copy these files to another destination, rename
			them, and edit the contents to satisfy your component's required behavior.

		\subsection step5 Step 5: Refer to the documentation
			Between the DspDemo project, the DspGain component template and the documentation found here,
			you should have enough resources to get started with DSPatch straight away. If you have any
			questions or wish to report a bug, feel free to email me at marcus@adaptaudio.com.

	\n

	\section release_notes Release Notes
	\subsection v21 v.2.1 (06 November 2012)
	- Removed DspSafePointer class.
	- Greatly improved overall performance.
	- Added commenting for doxygen.

	\subsection v20 v.2.0 (02 November 2012)
	- Re-licensed DSPatch to LGPL.
	- Added support for non-threaded platforms.
	- Circuit routing upgraded to more flexible template methods.

	\subsection v12 v.1.2 (20 October 2012)
	- Ported to Unix-based OS's.
	- Completed circuit-in-circuit logic.

	\subsection v11 v.1.1 (17 October 2012)
	- Varies bug fixes.
	- Added 2 more components to the DspDemo project.
	- Optimized threaded circuit processing.
	- AddComponent() updated to accept pre-constructed DspComponents.

	\page spec_page DSPatch Design Specification

1. The Circuit Concept:
-----------------------

A circuit is comprised of a collection of interconnected components. Each
component has 2 signal buses, on one end of the component there are input
pins (input bus), and on the other end there are output pins (output bus).
Components within the circuit are connected to each other via wires. Each
wire carries a signal from one component's output pin to another component's
input pin. A circuit can also comprise of interconnected circuits (E.g. ICs).
In this case a circuit acts as a component within another circuit.

______________________________________________________________________________

2. The DSPatch Circuit System:
------------------------------

**2.1 Structure:**

The nouns above are the classes we require in order to model our circuit in
code. Each component will contain an array of input wires. Each wire contains
references to the source component, the source output signal, and the
destination input signal. The signal bus class will contain an array of
signals, and lastly, the circuit class is derived from component and will
contain an array of internal components.

**2.2 Behavior:**

**2.2.1 Component:**

The component class will have a Tick() method responsible for acquiring its
next set of inputs from its input wires and populating the component's input
bus. To insure that these inputs are up-to-date, the dependent component
first calls all of its input components' Tick() methods -hence recursively
called in all components going backward through the circuit.  The acquired
input bus is then passed into a pure virtual method: Process() -it is the
responsibility of the (derived) component creator to implement this virtual
function. The Process() method has 2 input parameters: the input bus and the
output bus. This method's purpose is to pull its required inputs out of the
input bus, process these inputs, and populate the output bus with the results.
These resultant outputs in the output bus are then acquired by dependent
components via their Tick() functions.

Component input and output buses (signal count, string IDs etc.) will be
configurable by derived component classes via protected base methods, whilst
input wires to a component will be routable via public component class
methods.

The component class will also have a Reset() method. For optimization as well
as to avoid feedback deadlocks, a component needs to be aware of whether or
not it has already ticked during a circuit traversal so that if called to
Tick() again, it can ignore the call. The Reset() method informs the
component that the last circuit traversal has completed and hence can execute
the next Tick() request.

When a component is instantiated within a circuit, a unique string ID is
required. This name is used to identify the component in the circuit's
component collection.

**1.2.2 Circuit:**

In order to satisfy the statement above ("circuit acts as a component"), the
circuit class is derived from the component class. This means that the
circuit class has both Tick() and Process() methods. The Tick() method will
execute as normal, acquiring inputs for the circuit to process. This allows
us to expose the IO we require for internal components via the circuit's
input and output buses. Circuit IO-to-component wires, and
component-to-component wires, will be publicly routable via circuit class
methods. The circuit class' virtual Process() method is implemented as such:

* All internal components are Reset()ed.
* Inputs from the circuit's input bus are copied into their respective
internal component input buses.
* All internal components are Tick()ed.
* The circuit output bus is populated with the respective internal component
outputs.

All actions in respect to the circuit and the components within the circuit
will be made available via public functions in the circuit class. The circuit
object user will be able to add/remove components, connect/disconnect wires,
set/get circuit IO, set/get individual component IO, and acquire a handle to
any component within the circuit. This means only ever requiring one object
to perform all circuit actions.

**1.2.3 Signal:**

When it comes to transferring signals between components we require the same
level of abstraction for the data being moved around:

The base component class needs to supply its child class with any number of
inputs and outputs via the virtual Process() method. These inputs and outputs
may also need to be of different types. This requires a generic way of
containing variables of different types in a single collection -the signal
bus.

The signal class will hold a variable that can be dynamically typed at
run-time, which I've named "run-type". The run-type and signal classes make
use of template methods to allow object users to set and get the contained
variable as any type they wish. The run-type (and hence, a signal) has the
ability to change type at any point during program execution -this can be
useful for inputs that can accept a number of different types of data (E.g.
Varying sample size in an audio buffer: array of byte / int / float)

From the Process() method, a derived component can get and set the signals
it requires of the provided signal buses via public methods. As the component
creator is responsible for configuring the component's IO buses, the types
held within those buses can be assumed, and hence, read and written to
accordingly. Built-in typecasting and error checking prevents critical
run-time errors from occurring when signal types are mismatched.

When a signal is instantiated, an optional unique string ID can be assigned.
This name can be used to identify the signal in collections (E.g. a signal
bus). Alternatively, signals are referred to via index.
*/