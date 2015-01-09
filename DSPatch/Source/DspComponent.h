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

#ifndef DSPCOMPONENT_H
#define DSPCOMPONENT_H

//-------------------------------------------------------------------------------------------------

#include "DspSignalBus.h"
#include "DspWireBus.h"

class DspComponentThread;

//=================================================================================================
/// Abstract base class for all DSPatch components

/** Classes derived from DspComponent can be added to an DspCircuit and routed to and from other
DspComponents. On construction, derived classes must configure the component's IO buses by calling
AddInput_() and AddOutput_() respectively. Derived classes must also implement the pure virtual
method: Process_(). The Process_() method is a callback from the DSPatch engine that occurs when a
new set of input signals is ready for processing. The Process() method has 2 input parameters: the
input bus and the output bus. This method's purpose is to pull its required inputs out of the input
bus, process these inputs, and populate the output bus with the results (see DspSignalBus).

In order for a component to do any work it must be ticked over. This is performed by repeatedly
calling the Tick() and Reset() methods. The Tick() method is responsible for acquiring the next set
of input signals from component input wires and populating the component's input bus. To insure
that these inputs are up-to-date, the dependent component first calls all of its input components'
Tick() methods -hence recursively called in all components going backward through the circuit (This
is what's classified as a "pull system"). The acquired input bus is then passed to the Process_()
method. The Reset() method then informs the component that the last circuit traversal has completed
and hence can execute the next Tick() request. A component's Tick() and Reset() methods can be
called in a loop from the main application thread, or alternatively, by calling StartAutoTick(), a
seperate thread will spawn, automatically calling Tick() and Reset() methods continuosly (This is
most commonly used to tick over an instance of DspCircuit).*/

class DLLEXPORT DspComponent
{
public:
	DspComponent();
	virtual ~DspComponent();

	bool ConnectInput( DspComponent* inputComponent, unsigned long fromOutput, unsigned long toInput );
	bool ConnectInput( DspComponent* inputComponent, unsigned long fromOutput, std::string toInput );
	bool ConnectInput( DspComponent* inputComponent, std::string fromOutput, unsigned long toInput );
	bool ConnectInput( DspComponent* inputComponent, std::string fromOutput, std::string toInput );

	void DisconnectInput( DspComponent* inputComponent, unsigned long fromOutput, unsigned long toInput );
	void DisconnectInput( DspComponent* inputComponent, unsigned long fromOutput, std::string toInput );
	void DisconnectInput( DspComponent* inputComponent, std::string fromOutput, unsigned long toInput );
	void DisconnectInput( DspComponent* inputComponent, std::string fromOutput, std::string toInput );

	void DisconnectInput( unsigned long inputIndex );
	void DisconnectInput( std::string inputName );
	void DisconnectInput( DspComponent* inputComponent );
	void DisconnectInputs();

	void Tick();
	void Reset();

	virtual void StartAutoTick();
	virtual void StopAutoTick();
	virtual void PauseAutoTick();
	virtual void ResumeAutoTick();

	virtual void SetThreadCount( unsigned long threadCount );
	unsigned long GetThreadCount();

	void ThreadTick( unsigned long threadNo );
	void ThreadReset( unsigned long threadNo );

	// Only works with non-threaded ticking (_threadCount = 0)
	// =======================================================

	template< class ValueType >
	bool SetInputValue( unsigned long inputIndex, const ValueType& newValue );

	template< class ValueType >
	bool SetInputValue( std::string inputName, const ValueType& newValue );

	template< class ValueType >
	bool GetOutputValue( unsigned long outputIndex, ValueType& returnValue );

	template< class ValueType >
	bool GetOutputValue( std::string outputName, ValueType& returnValue );

	bool SetInputSignal( unsigned long inputIndex, const DspSignal* newSignal );
	bool SetInputSignal( std::string inputName, const DspSignal* newSignal );
	DspSignal* GetOutputSignal( unsigned long outputIndex );
	DspSignal* GetOutputSignal( std::string outputName );

	// Only works with non-threaded ticking (_threadCount = 0)
	// =======================================================

	unsigned long GetInputCount();
	unsigned long GetOutputCount();

	bool SetComponentName( std::string componentName );
	std::string GetComponentName();

	bool FindInput( std::string signalName, unsigned long& returnIndex ) const;
	bool FindInput( unsigned long signalIndex, unsigned long& returnIndex ) const;
	bool FindOutput( std::string signalName, unsigned long& returnIndex ) const;
	bool FindOutput( unsigned long signalIndex, unsigned long& returnIndex ) const;

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs ) = 0;

	bool AddInput_( std::string inputName = "" );
	bool AddOutput_( std::string outputName = "" );

	void ClearInputs_();
	void ClearOutputs_();

private:
	unsigned long _threadCount;

	DspSignalBus _inputBus;
	DspSignalBus _outputBus;

	std::vector< DspSignalBus > _inputBuses;
	std::vector< DspSignalBus > _outputBuses;

	std::string _componentName;
	bool _isAutoTickRunning;
	bool _isAutoTickPaused;

	DspWireBus _inputWires;

	bool _hasTicked;

	DspComponentThread* _componentThread;

	std::vector< bool* > _hasTickeds;									// pointers ensure that parallel threads will only read from this vector

	std::vector< bool > _gotReleases;									// pointers not used here as only 1 thread writes to this vector at a time
	std::vector< DspMutex > _releaseMutexes;
	std::vector< DspWaitCondition > _releaseCondts;

	void _WaitForRelease( unsigned long threadNo );
	void _ReleaseThread( unsigned long threadNo );

	virtual void _ProcessInputWire( DspWire* inputWire );
	virtual void _ProcessInputWire( DspWire* inputWire, unsigned long threadNo );
};

//=================================================================================================

template< class ValueType >
bool DspComponent::SetInputValue( unsigned long inputIndex, const ValueType& newValue )
{
	return _inputBus.SetValue( inputIndex, newValue );
}

//-------------------------------------------------------------------------------------------------

template< class ValueType >
bool DspComponent::SetInputValue( std::string inputName, const ValueType& newValue )
{
	return _inputBus.SetValue( inputName, newValue );
}

//-------------------------------------------------------------------------------------------------

template< class ValueType >
bool DspComponent::GetOutputValue( unsigned long outputIndex, ValueType& returnValue )
{
	return _outputBus.GetValue( outputIndex, returnValue );
}

//-------------------------------------------------------------------------------------------------

template< class ValueType >
bool DspComponent::GetOutputValue( std::string outputName, ValueType& returnValue )
{
	return _outputBus.GetValue( outputName, returnValue );
}

//=================================================================================================

#endif // DSPCOMPONENT_H
