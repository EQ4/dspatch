DSPatch Design Specification
============================
______________________________________________________________________________

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
signals. Lastly, the circuit class is derived from component and will contain
an array of internal components.

**2.2 Behavior:**

**2.2.1 Component:**

The component class will have a Tick() method responsible for acquiring its
next set of inputs from its input wires and populating the component's input
bus. To insure that these inputs are up-to-date, the dependent component
first calls all of its input component's Tick() methods -hence recursively
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
