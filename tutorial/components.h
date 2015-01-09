#include "DSPatch.h"

#include <time.h>
#include <iostream>

//=================================================================================================
// DspAnd:
// DspAnd has 2 inputs and 1 output.
// This component performs a logic AND on 2 boolean input values and outputs the result.

class DspAnd : public DspComponent
{
public:
	DspAnd()
	{
		// add 2 inputs
		AddInput_( "input1" );
		AddInput_( "input2" );

		// add 1 output
		AddOutput_( "output" );
	}

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs )
	{
		// create local stack variables to hold input values
		bool state1 = false;
		bool state2 = false;

		// get values from inputs bus ( GetValue() returns true if successful )
		if( inputs.GetValue( "input1", state1 ) &&	// equivalent: inputs.GetValue( 0, state1 );
				inputs.GetValue( "input2", state2 ) )		// equivalent: inputs.GetValue( 1, state2 );
		{
			// set output as the result of state1 AND state2
			outputs.SetValue( "output", state1 && state2 );	// equivalent: outputs.SetValue( 0, state1 && state2 );
		}
	}
};

//=================================================================================================

// DspRandBool:
// DspRandBool has 1 output.
// This component generates a random boolean value then outputs the result.

class DspRandBool : public DspComponent
{
public:
	DspRandBool()
	{
		// add 1 output
		AddOutput_();

		// seed randomizer
		srand( (unsigned int) time( NULL ) );
	}

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs )
	{
		// set output as randomized true / false
		outputs.SetValue( 0, rand() % 2 == 0 );
	}
};

//=================================================================================================

// DspPrintBool:
// DspPrintBool has 1 input.
// This component receives a boolean value and outputs it to the console.

class DspPrintBool : public DspComponent
{
public:
	DspPrintBool()
	{
		// add 1 input
		AddInput_();
	}

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs )
	{
		// create a local stack variable to hold input value
		bool inputBool;

		// get boolean value from inputs bus
		if( inputs.GetValue( 0, inputBool ) )
		{
			// print "true" / "false" depending on boolean value received
			switch( inputBool )
			{
				case true:
					std::cout << "true" << '\n';
				case false:
					std::cout << "false" << '\n';
			}
		}
	}
};

//=================================================================================================