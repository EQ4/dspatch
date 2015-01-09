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

#include "DspCircuit.h"
#include "DspCircuitThread.h"
#include "DspWire.h"

//=================================================================================================

DspCircuit::DspCircuit( unsigned long threadCount )
: _currentThreadIndex( 0 )
{
	_inToInWires = new DspWireBus( true );
	_outToOutWires = new DspWireBus( false );
	SetThreadCount( threadCount );
}

//-------------------------------------------------------------------------------------------------

DspCircuit::~DspCircuit()
{
	StopAutoTick();
	RemoveAllComponents( true );
	SetThreadCount( 0 );
	delete _inToInWires;
	delete _outToOutWires;
}

//=================================================================================================

void DspCircuit::PauseAutoTick()
{
	// pause auto tick
	DspComponent::PauseAutoTick();

	// manually tick until 0
	while( _currentThreadIndex != 0 )
	{
		Tick();
		Reset();
	}

	// sync all threads
	for( unsigned long i = 0; i < _circuitThreads.size(); i++ )
	{
		_circuitThreads[i]->Sync();
	}
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::SetThreadCount( unsigned long threadCount )
{
	// set thread count only if this circuit is not part of another circuit
	if( threadCount != GetThreadCount() && GetParentCircuit() == NULL )
	{
		PauseAutoTick();

		// set all components to the new thread count
		for( unsigned long i = 0; i < _components.size(); i++ )
		{
			_components[i]->SetThreadCount( threadCount );
		}

		// delete excess threads (if new thread count is less than current)
		for( long i = GetThreadCount() - 1; i >= (long)threadCount; i-- )
		{
			delete _circuitThreads[i];
		}

		// resize local thread array
		_circuitThreads.resize( threadCount );

		// create excess threads (if new thread count is more than current)
		for( unsigned long i = GetThreadCount(); i < threadCount; i++ )
		{
			_circuitThreads[i] = new DspCircuitThread( _components, i );
		}

		// set the system's new thread count
		DspComponent::SetThreadCount( threadCount );

		ResumeAutoTick();
	}
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::AddComponent( DspComponent* component, std::string componentName )
{
	if( componentName == "" && component->GetComponentName() == "" )
	{
		return false;	// a component can't be added to the circuit without a name
	}

	unsigned long componentIndex;

	if( _FindComponent( component, componentIndex ) )
	{
		return false;	// if the component is already in the array
	}
	if( _FindComponent( component->GetComponentName(), componentIndex ) )
	{
		return false;	// if the component name is already in the array
	}

	component->SetComponentName( componentName );
	component->SetThreadCount( GetThreadCount() );
	component->SetParentCircuit( this );

	PauseAutoTick();

	_components.push_back( component );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::RemoveComponent( DspComponent* component, bool deleteComponent )
{
	unsigned long componentIndex;

	if( _FindComponent( component, componentIndex ) )
	{
		_RemoveComponent( componentIndex, deleteComponent );
	}
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::RemoveComponent( std::string componentName, bool deleteComponent )
{
	unsigned long componentIndex;

	if( _FindComponent( componentName, componentIndex ) )
	{
		_RemoveComponent( componentIndex, deleteComponent );
	}
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::RemoveAllComponents( bool deleteComponents )
{
	for( unsigned long i = 0; i < _components.size(); i++ )
	{
		_RemoveComponent( i--, deleteComponents );	// size drops as one is removed
	}
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectComponent( std::string component )
{
	unsigned long componentIndex;

	if( !_FindComponent( component, componentIndex ) )	// verify component exists
	{
		return;
	}

	_DisconnectComponent( componentIndex );
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::AddInput( std::string inputName )
{
	return AddInput_( inputName );
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::AddOutput( std::string outputName )
{
	return AddOutput_( outputName );
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::ClearInputs()
{
	ClearInputs_();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::ClearOutputs()
{
	ClearOutputs_();
}

//=================================================================================================

void DspCircuit::Process_( DspSignalBus& inputs, DspSignalBus& outputs )
{
	// process in a single thread if this circuit has no threads, or is part of another circuit
	if( GetThreadCount() == 0 || GetParentCircuit() != NULL )
	{
		DspWire* wire;
		DspSignal* signal;

		// reset all internal components
		for( unsigned long i = 0; i < _components.size(); i++ )
		{
			_components[i]->Reset();
		}

		// set all internal component inputs from connected circuit inputs
		for( unsigned long i = 0; i < _inToInWires->GetWireCount(); i++ )
		{
			wire = _inToInWires->GetWire( i );
			signal = inputs.GetSignal( wire->fromSignalIndex );
			wire->linkedComponent->SetInputSignal( wire->toSignalIndex, signal );
		}

		// tick all internal components
		for( unsigned long i = 0; i < _components.size(); i++ )
		{
			_components[i]->Tick();
		}

		// set all circuit outputs from connected internal component outputs
		for( unsigned long i = 0; i < _outToOutWires->GetWireCount(); i++ )
		{
			wire = _outToOutWires->GetWire( i );
			signal = wire->linkedComponent->GetOutputSignal( wire->fromSignalIndex );
			outputs.SetSignal( wire->toSignalIndex, signal );
		}
	}
	else
	{
		_circuitThreads[_currentThreadIndex]->Resume(); // resume thread x

		if( ++_currentThreadIndex >= GetThreadCount() )	// shift to thread x+1
		{
			_currentThreadIndex = 0;
		}
	}
}

//=================================================================================================

bool DspCircuit::_FindComponent( DspComponent* component, unsigned long& returnIndex )
{
	for( unsigned long i = 0; i < _components.size(); i++ )
	{
		if( _components[i] == component )
		{
			returnIndex = i;
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::_FindComponent( std::string componentName, unsigned long& returnIndex )
{
	for( unsigned long i = 0; i < _components.size(); i++ )
	{
		if( _components[i]->GetComponentName() != "" && _components[i]->GetComponentName() == componentName )
		{
			returnIndex = i;
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::_FindComponent( unsigned long componentIndex, unsigned long& returnIndex )
{
	if( componentIndex < _components.size() )
	{
		returnIndex = componentIndex;
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::_DisconnectComponent( unsigned long componentIndex )
{
	PauseAutoTick();

	// remove component from _inputComponents and _inputWires
	_components[ componentIndex ]->DisconnectInputs();

	// remove component from _inToInWires
	DspWire* wire;
	for( unsigned long i = 0; i < _inToInWires->GetWireCount(); i++ )
	{
		wire = _inToInWires->GetWire( i );
		if( wire->linkedComponent == _components[ componentIndex ] )
		{
			_inToInWires->RemoveWire( i );
		}
	}

	// remove component from _outToOutWires
	for( unsigned long i = 0; i < _outToOutWires->GetWireCount(); i++ )
	{
		wire = _outToOutWires->GetWire( i );
		if( wire->linkedComponent == _components[ componentIndex ] )
		{
			_outToOutWires->RemoveWire( i );
		}
	}

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::_RemoveComponent( unsigned long componentIndex, bool deleteComponent )
{
	PauseAutoTick();

	_DisconnectComponent( componentIndex );

	if( deleteComponent )
	{
		delete _components[componentIndex];
	}
	else
	{
		_components[componentIndex]->SetParentCircuit( NULL );
	}

	for( unsigned long i = componentIndex; i < ( _components.size() - 1 ); i++ )
	{
		_components[i] = _components[i + 1];	// shift all lower elements up
	}

	_components.pop_back();	// remove end item

	ResumeAutoTick();
}

//=================================================================================================
