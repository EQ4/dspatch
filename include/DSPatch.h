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

#ifndef DSPATCH_H
#define DSPATCH_H

//-------------------------------------------------------------------------------------------------

#include "../src/DspCircuit.h"

//=================================================================================================
/// System-wide DSPatch functionality

/** This class provides global DSPatch functionality.

At the core of the DSPatch framework is what's known as the "global circuit". The DSPatch class and
hence, the global circuit provides a transparent workspace for "global scoped components"
(components not within a DspCircuit) to benefit from circuit parallel processing. As it is not
required that components be explicitly added to a DspCircuit in order to be routed etc., in order
for these global scoped components to benefit from the multi threading associated with the circuit
scheduler, they are automatically added to the DSPatch global circuit when parallel processing is
required (i.e. StartAutoTick() is called). Although global circuit operations are automatic and
transparent to the user, if required, the user is may set the number of threads used by the global
circuit by calling SetGlobalThreadCount(). 

Lastly, the Finalize() method must be called on application exit in order for DSPatch to perform
the necessary memory cleanup.*/

class DLLEXPORT DSPatch
{
public:	
	static bool IsThisGlobalCircuit( DspComponent* thisComponent );

	static bool AddGlobalComponent( DspComponent* component, std::string componentName = "" );
	static void RemoveGlobalComponent( DspComponent* component, bool deleteComponent = false );

	static unsigned long GetGlobalComponentCount();

	static void StartGlobalAutoTick();
	static void StopGlobalAutoTick();

	static void SetGlobalThreadCount( unsigned long threadCount );

	static void Finalize();

private:
	static DspCircuit* _globalCircuit;
};

//=================================================================================================

#endif // DSPATCH_H

//=================================================================================================

/**

\mainpage Welcome

	\section intro_sec Introduction
		DSPatch, pronounced "dispatch", is a powerful C++ flow-based programming library that allows
    you to create and route (or "patch") high performance data processing circuits. DSPatch is not
		limited to any particular type of circuit or signal, its generic object-oriented API allows you
		to create almost any process chain imaginable, from simple logic circuits to full-blown
		electronics simulation. DSPatch's simple framework makes development quick and easy, allowing
    you to hit the ground running on every project.

    DSPatch is designed around the concept of a "circuit" containing "components", interconnected
    via "wires" that transfer "signals" to and from input and output "buses". For more detail on
    how DSPatch works, check out the <a href="spec_page.html"><b>DSPatch Design Specification</b></a>.
    
    The two most important classes to consider are DspComponent and DspCircuit.  In order to route
		data to and from DspComponents they can either be added to an DspCircuit, where they can be
		wired together (recommended), or they can be wired directly via public DspComponent methods.
    The DSPatch engine takes care of data transfer between interconnected components, when data is
    ready for a component to process, a callback: "Process_()" is executed in that component. For a
    component to form part of the DSPatch framework, designers simply have to derive their
    component from the DspComponent base class, configure the component's IO buses, and implement
    the virtual Process_() callback method.
    
  \n

	\section features_sec Features
		- Automatic branch synchronization.
		- Dynamic thread count adjustment.
		- Easy-to-use object-oriented API.
		- Feedback loops.
		- High performance parallel processing.
		- Integrated circuits.
		- Non-threaded platform support.
		- Run-time circuit wiring.
		- Run-time adaptive signal types.

	\n

	\section install_sec Getting Started

		\subsection step1 Step 1: Download DSPatch
			The DSPatch library can be downloaded from the
			<a href="https://sourceforge.net/projects/dspatch/"><b>SourceForge project page</b></a>. 
			Download the project archive and extract the contents anywhere you like.

		\subsection step2 Step 2: Read the Tutorials
			The "Tutorials" section below covers 2 vital aspects to developing with DSPatch:
			1. Creating a DspComponent - This tutorial is a start to finish demonstration of how to
			create a DSPatch component.
			2. Building a DspCircuit - In this tutorial you will learn how to use the DSPatch framework
			to interconnect and process DspComponent networks using the DspCircuit interface.

		\subsection step3 Step 3: Check out the DspDemo project
			In the "example" folder (in the DSPatch root directory) you will find a simple DSPatch
			example project: "DspDemo". This project has been written to assist developers in
			understanding the DSPatch API as well as to demonstrate how it could be used to build a
			real-time audio process chain. The quickest way to get this project compiled and running is
      to simply open the "dspdemo.sln" (Windows) or "dspdemo.xcodeproj" (Mac OS X) file (found in
      the "/example/windows", and "/example/mac_os_x" directories respectively) depending on the
			platform used -This solution contains both the DSPatch library and DspDemo project
			side-by-side as would be typical in developing a DSPatch application.

		\subsection step4 Step 4: Make your own DspComponents
			A fast way to create your own DspComponent could be to copy an existing component to another
			destination, rename it, and edit the contents to satisfy your component's required behavior.
			In the "example" folder (from Step 3) you will find 2 source files: "DspAdder.h" and
			"DspAdder.cpp". These files make up a very simple DspComponent that receives 2 floating-point
			buffers into it's 2 inputs, adds each buffer element of the 1st buffer to the corresponding
			element of the 2nd buffer, then passes the resultant buffer to the output. Alternatively, you
			could just copy / reference the source code from the "Creating a DspComponent" tutorial
			(found under the "tutorial" folder in the root directory).

		\subsection step5 Step 5: Roll your own DSPatch projects
			As DSPatch is not reliant on any non-standard 3rd party sources, getting a DSPatch project to
			compile and run is relatively painless. All you need to do from your project is #include
			"DSPatch.h" from the "include" folder (in the DSPatch root directory), and link to the
			DSPatch library (either by including all DSPatch source or by linking to a compiled library
			file). To speed things up you may want to copy, rename, and edit the DspDemo project from
			step 3 to get up and running faster.

		\subsection step6 Step 6: Refer to the documentation
			Between the DspDemo project, the DspAdder component template, and the documentation found
			here, you should have enough resources to get started with DSPatch straight away. If you have
			any questions or wish to report a bug, feel free to email me at marcus@adaptaudio.com.

	\n

	\section tutorial_sec Tutorials

		\subsection create_component 1. Creating a DspComponent
			In order to create a new DspComponent, we must derive our component class from the
			DspComponent base class, configure component IO, and implement the inherited virtual
			"Process_()" method.
			
			Lets take a look at how we would go about creating a boolean logic "AND" component. This
			component will accept 2 boolean input values and output the result of: input 1 && input 2.
			
			We begin by deriving our new "DspAnd" component from DspComponent:

			\code
// 1. Derive component class from DspComponent
// ===========================================
class DspAnd : public DspComponent
{
			\endcode

			The next step is to configure our component's input and output buses. This is achieved by
			calling the base protected methods AddInput_() and AddOutput_() respectively from our
			component's constructor. Each method must be called once per input / output required. In our
			component's case, we require 2 inputs and 1 output, therefore our constructor code will look
			like this:

			\code
public:
	// 2. Configure component IO buses
	// ===============================
	DspAnd()
	{
		// add 2 inputs
		AddInput_( "input1" );
		AddInput_( "input2" );

		// add 1 output
		AddOutput_( "output" );
	}
			\endcode
			
			The string values passed into the AddInput_() and AddOutput_() method calls are signal names
			/ IDs. As component IO can be referenced by either string ID or index, IO signal names are
			optional. If we do not require a signal to have a string ID associated with it, we can simply
			leave the parenthesis empty.

			Lastly, our component must implement the DspComponent virtual Process_() method. This is
			where our component does it's work. The Process_() method provides us with 2 parameters: the
			input bus and the output bus. It is our duty as the component designer to pull the inputs we
			require out of the input bus, process them accordingly, and populate the output bus with the
			results. Our component's process method will look something like this:

			\code
protected:
	// 3. Implement virtual Process_() method
	// ======================================
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs )
	{
		// create local stack variables to hold input values
		bool bool1 = false;
		bool bool2 = false;

		// get values from inputs bus ( GetValue() returns true if successful )
		if( inputs.GetValue( 0, bool1 ) && //OR inputs.GetValue( "input1", bool1 );
		    inputs.GetValue( 1, bool2 ) )  //OR inputs.GetValue( "input2", bool2 );
		{
			// set output as the result of bool1 AND bool2
			outputs.SetValue( 0, bool1 && bool2 ); //OR outputs.SetValue( "output", bool1 && bool2 );
		}
	}
};
			\endcode

			And that's it! Our component is now ready to form part of the DSPatch framework. Next we'll
			look at how we can add our component to a DspCircuit and route it to and from other
			DspComponents.

		\n

		\subsection use_component 2. Building a DspCircuit
			In order for us to get any real use out of our DspComponents, we need them to interact with
			each other. This is where the DspCircuit class comes in. A DspCircuit is a workspace for
			adding and routing DspComponents. In this section we will have a look at how to create a
			simple DSPatch application that generates random boolean pairs, performs a logic AND on each
			pair, then prints the result to screen.

			First we must include the DSPatch header and any other headers that contain DspComponents we
			wish to use in our application:

			\code
#include "DSPatch.h"
#include "components.h"
			\endcode

			Next, we must instantiate our DspCircuit object and all DspComponent objects needed for our
			circuit. Lets say we had 2 other components included with "DspAnd" (from the first tutorial):
			"DspRandBool" (generates a random boolean value then outputs the result) and "DspPrintBool"
			(receives a boolean value and outputs it to the console):

			\code
void main()
{
	// 1. Create a DspCircuit where we can route our components
	// ========================================================
	DspCircuit circuit;

	// 2. Create instances of the components needed for our circuit
	// ============================================================
	DspRandBool randBoolGen1;
	DspRandBool randBoolGen2;
	DspAnd logicAnd;
	DspPrintBool boolPrinter;
		\endcode

		Now that we have a circuit and some components, lets add all of our components to the circuit:

		\code
	// 3. Add component instances to circuit
	// =====================================
	circuit.AddComponent( randBoolGen1, "Bool Generator 1" );
	circuit.AddComponent( randBoolGen2, "Bool Generator 2" );
	circuit.AddComponent( logicAnd, "Logic AND" );
	circuit.AddComponent( boolPrinter, "Bool Printer" );
		\endcode

		The string values passed into the AddComponent() method calls are component names / IDs.
		Although we still have the option of referencing a component via it's pointer in a circuit,
		component string IDs can allow circuit objects to be entirely self-contained. This could give
		us the ability to pass circuits around by reference, allowing the receiver access to all
		circuit components via their string IDs, without having to manage both component and circuit
		references everywhere.

		We are now ready to begin wiring the circuit:

		\code
	// 4. Wire up the components inside the circuit
	// ============================================

	circuit.ConnectOutToIn( randBoolGen1, 0, logicAnd, 0 );
	//OR circuit.ConnectOutToIn( "Bool Generator 1", 0, "Logic AND", 0 );
	//OR circuit.ConnectOutToIn( "Bool Generator 1", 0, "Logic AND", "input1" );

	circuit.ConnectOutToIn( randBoolGen2, 0, logicAnd, 1 );
	//OR circuit.ConnectOutToIn( "Bool Generator 2", 0, "Logic AND", 1 );
	//OR circuit.ConnectOutToIn( "Bool Generator 2", 0, "Logic AND", "input2" );

	circuit.ConnectOutToIn( logicAnd, 0, boolPrinter, 0 );
	//OR circuit.ConnectOutToIn( "Logic AND", 0, "Bool Printer", 0 );
	//OR circuit.ConnectOutToIn( "Logic AND", "output", "Bool Printer", 0 );
		\endcode

		The code above results in the following wiring configuration:
		\code
	                               ___________
	["Bool Generator 1"] 0 ---> 0 |           |
	                              |"Logic AND"| 0 ---> 0 ["Bool Printer"]
	["Bool Generator 2"] 0 ---> 1 |___________|
		\endcode

		*N.B. Each component input can only accept one wire at a time. When another wire is connected
		to an input that already has a connected wire, that wire is replaced with the new one. One
		output, on the other hand, can be distributed to multiple inputs.

		Lastly, in order for our circuit to do any work it must be ticked over. This is performed by
		repeatedly calling the circuit's Tick() and Reset() methods. These methods can be called
		manually in a loop from the main application thread, or alternatively, by calling
		StartAutoTick(), a seperate thread will spawn, automatically calling Tick() and Reset()
		continuously. A circuit's thread count can be adjusted at runtime, allowing us to increase /
		decrease the number of threads use by the circuit as required during execution:

		\code
	// 5. Tick the circuit
	// ===================

	// Circuit tick method 1: Manual
	for( int i = 0; i < 10; i++ )
	{
		circuit.Tick();
		circuit.Reset();
	}

	// Circuit tick method 2: Automatic
	std::cout << "Press any key to begin circuit auto-tick.";
	getchar();
	circuit.StartAutoTick();

	// Increase circuit thread count for higher performance
	getchar();
	circuit.SetThreadCount( 4 );

	// Press any key to quit
	getchar();

		\endcode

		Lastly, the DSPatch::Finalize() method must be called on application exit in order for DSPatch
		to perform its own internal memory cleanup.

		\code
	// 6. Clean up
	// ===========
	DSPatch::Finalize();
}
		\endcode

		(All the source code from these tutorials can be found under the "tutorial" folder in the DSPatch root directory).

	\n

	\section release_notes Release Notes

		\subsection v23 v.2.3 (15 December 2012)
		- Circuit interface simplified.
		- Completed "circuit-less" component processing.
		- Component names now optional.
		- DspCircuit no longer deletes external memory.
		- Optimized circuit-in-circuit processing.

		\subsection v22 v.2.2 (08 December 2012)
		- A component can no longer exist in multiple circuits.
		- Added coding tutorials
		- Cleaned up project directory structure.
		- Comments and documentation updates.
		- Resolved component / circuit thread count clash.

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

		\subsection v10 v.1.0 (14 October 2012)
		- Extracted routing engine behind "Crosstalk" to form: "DSPatch".

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
input bus is then passed into a virtual method: Process() -it is the
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
optional. This name can be used to identify the component in the circuit's
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

______________________________________________________________________________

3. Parallel Circuit Processing:
-------------------------------

The multi-threading aspect of DSPatch is designed to allow the library user the
ability to specify the number of threads in which he/she required the circuit
to process, rather than the thread count growing as the system does. So for
example, an application running on a quad core CPU could be limited to 4
threads in order to allow each core to handle just one thread.

**3.1 The Circuit Thread:**

Circuit threads are threads that traverse entire circuits. The circuit runs
through its array of components, calling each components' process method in a
single thread (circuit thread) loop. As each component is done processing, it
hands over control to the next waiting circuit thread. Therefore, if you had 5
components in a process chain, and 5 circuit threads, at any point in time you
could have one thread per component processing in parallel. With this in place,
you now also have the option to select 0 circuit threads. In this state, the
circuit's Tick() and Reset() methods will block the calling thread while all
components in the circuit are processed, whereas with circuit threads enabled,
the calling thread will block only if all circuit threads are busy. (A null
thread class that implements stub thread methods allows for DSPatch to compile
on platforms with no native thread support)

**3.2 The Component Thread:**

The component thread simply ticks a single component over and over. As the
circuit class inherits from component, we use it's component thread to
"auto-tick" the circuit whilst freeing up the main application thread for
control. In a circuit-less system of interconnected DspComponents, each
component must be auto-ticked in order for the component network to be parallel
processed. Again, for non-threaded systems, a component's Tick() and Reset()
methods can simply be called in a loop from the main application thread.*/