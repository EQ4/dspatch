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
	RemoveAllComponents();
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
	if( threadCount != _circuitThreads.size() )
	{
		PauseAutoTick();

		unsigned long currentThreadCount = _circuitThreads.size();

		// set all components to the new thread count
		for( unsigned long i = 0; i < _components.size(); i++ )
		{
			_components[i]->SetBufferCount( threadCount );
		}

		// delete excess threads (if new thread count is less than current)
		for( long i = currentThreadCount - 1; i >= (long)threadCount; i-- )
		{
			delete _circuitThreads[i];
		}

		// resize local thread array
		_circuitThreads.resize( threadCount );

		// create excess threads (if new thread count is more than current)
		for( unsigned long i = currentThreadCount; i < threadCount; i++ )
		{
			_circuitThreads[i] = new DspCircuitThread( _components, i );
		}

		ResumeAutoTick();
	}
}

//-------------------------------------------------------------------------------------------------

unsigned long DspCircuit::GetThreadCount()
{
	return _circuitThreads.size();
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::AddComponent( DspComponent* component, std::string componentName )
{
	if( component != this )
	{
		// if the component has a name already
		if( component->GetComponentName() != "" && componentName == "" )
		{
			componentName = component->GetComponentName();
		}

		unsigned long componentIndex;

		if( _FindComponent( component, componentIndex ) )
		{
			return false;	// if the component is already in the array
		}
		if( _FindComponent( componentName, componentIndex ) )
		{
			return false;	// if the component name is already in the array
		}

		PauseAutoTick();

		// components within the circuit need to have as many buffers as there are threads in the circuit
		component->SetBufferCount( _circuitThreads.size() );
		component->SetComponentName( componentName );
		component->SetParentCircuit( this );

		_components.push_back( component );

		ResumeAutoTick();

		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::AddComponent( DspComponent& component, std::string componentName )
{
	return AddComponent( &component, componentName );
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::RemoveComponent( DspComponent* component )
{
	unsigned long componentIndex;

	if( _FindComponent( component, componentIndex ) )
	{
		_RemoveComponent( componentIndex );
	}
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::RemoveComponent( std::string componentName )
{
	unsigned long componentIndex;

	if( _FindComponent( componentName, componentIndex ) )
	{
		_RemoveComponent( componentIndex );
	}
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::RemoveAllComponents()
{
	for( unsigned long i = 0; i < _components.size(); i++ )
	{
		_RemoveComponent( i-- ); // size drops as one is removed
	}
}

//-------------------------------------------------------------------------------------------------

unsigned long DspCircuit::GetComponentCount()
{
	return _components.size();
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
	DspWire* wire;
	DspSignal* signal;

	// process in a single thread if this circuit has no threads
	// =========================================================
	if( _circuitThreads.size() == 0 )
	{
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

		// reset all internal components
		for( unsigned long i = 0; i < _components.size(); i++ )
		{
			_components[i]->Reset();
		}

		// set all circuit outputs from connected internal component outputs
		for( unsigned long i = 0; i < _outToOutWires->GetWireCount(); i++ )
		{
			wire = _outToOutWires->GetWire( i );
			signal = wire->linkedComponent->GetOutputSignal( wire->fromSignalIndex );
			outputs.SetSignal( wire->toSignalIndex, signal );
		}
	}
	// process in multiple thread if this circuit has threads
	// ======================================================
	else
	{
		_circuitThreads[_currentThreadIndex]->Sync();		// sync with thread x

		// set all circuit outputs from connected internal component outputs
		for( unsigned long i = 0; i < _outToOutWires->GetWireCount(); i++ )
		{
			wire = _outToOutWires->GetWire( i );
			signal = wire->linkedComponent->GetOutputSignal( wire->fromSignalIndex, _currentThreadIndex );
			outputs.SetSignal( wire->toSignalIndex, signal );
		}

		// set all internal component inputs from connected circuit inputs
		for( unsigned long i = 0; i < _inToInWires->GetWireCount(); i++ )
		{
			wire = _inToInWires->GetWire( i );
			signal = inputs.GetSignal( wire->fromSignalIndex );
			wire->linkedComponent->SetInputSignal( wire->toSignalIndex, _currentThreadIndex, signal );
		}

		_circuitThreads[_currentThreadIndex]->Resume(); // resume thread x

		if( ++_currentThreadIndex >= _circuitThreads.size() )	// shift to thread x+1
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

bool DspCircuit::_FindComponent( DspComponent& component, unsigned long& returnIndex )
{
	return _FindComponent( &component, returnIndex );
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

void DspCircuit::_RemoveComponent( unsigned long componentIndex )
{
	PauseAutoTick();

	_DisconnectComponent( componentIndex );

	// set the removed component's parent circuit to NULL
	if( _components[componentIndex]->GetParentCircuit() != NULL )
	{
		_components[componentIndex]->SetParentCircuit( NULL );
	}
	// setting a component's parent to NULL (above) calls _RemoveComponent (hence the following code will run)
	else if( _components.size() != 0 )
	{
		for( unsigned long i = componentIndex; i < _components.size() - 1; i++ )
		{
			_components[i] = _components[i + 1];	// shift all lower elements up
		}

		_components.pop_back();	// remove end item
	}

	ResumeAutoTick();
}

//=================================================================================================