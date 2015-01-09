/********************************************************************
DSPatch - Real-Time, Multi-Purpose Circuit Builder / Simulator Engine
Copyright (c) 2012 Marcus Tomlinson / Adapt Audio

This file is part of DSPatch.

DSPatch is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DSPatch is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with DSPatch.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

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
	if( threadCount != GetThreadCount() )
	{
		PauseAutoTick();

		// set all components to the new thread count
		for( unsigned long i = 0; i < _components.size(); i++ )
		{
			_components[i]->DspComponent::SetThreadCount( threadCount );
		}

		// resize local thread array
		_circuitThreads.resize( threadCount );

		// create excess threads (if new thread count is more than current)
		for( unsigned long i = GetThreadCount(); i < threadCount; i++ )
		{
			_circuitThreads[i].New( _components, i );
			_circuitThreads[i].LockPointer();
		}

		// set the system's new thread count
		DspComponent::SetThreadCount( threadCount );

		ResumeAutoTick();
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
		_RemoveComponent( i-- );	// size drops as one is removed
	}
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectOutToIn( std::string fromComponent, unsigned long fromOutput, std::string toComponent, unsigned long toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) || !_FindComponent( toComponent, toComponentIndex ) )
	{
		return false;
	}

	PauseAutoTick();

	bool result = _components[toComponentIndex]->ConnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();

	return result;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectOutToIn( std::string fromComponent, unsigned long fromOutput, std::string toComponent, std::string toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) || !_FindComponent( toComponent, toComponentIndex ) )
	{
		return false;
	}

	PauseAutoTick();

	bool result = _components[toComponentIndex]->ConnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();

	return result;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectOutToIn( std::string fromComponent, std::string fromOutput, std::string toComponent, unsigned long toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) || !_FindComponent( toComponent, toComponentIndex ) )
	{
		return false;
	}

	PauseAutoTick();

	bool result = _components[toComponentIndex]->ConnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();

	return result;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectOutToIn( std::string fromComponent, std::string fromOutput, std::string toComponent, std::string toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) || !_FindComponent( toComponent, toComponentIndex ) )
	{
		return false;
	}

	PauseAutoTick();

	bool result = _components[toComponentIndex]->ConnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();

	return result;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectInToIn( unsigned long fromInput, std::string toComponent, unsigned long toInput )
{
	unsigned long toComponentIndex;

	if( fromInput >= GetInputCount() ||															// verify circuit input
			!_FindComponent( toComponent, toComponentIndex ) ||					// verify component exists
			toInput >= _components[toComponentIndex]->GetInputCount() )	// verify component input
	{
		return false;
	}

	PauseAutoTick();

	_components[toComponentIndex]->DisconnectInput( toInput );
	_inToInWires->AddWire( _components[toComponentIndex], fromInput, toInput );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectInToIn( unsigned long fromInput, std::string toComponent, std::string toInput )
{
	unsigned long toComponentIndex;
	unsigned long toInputIndex;

	if( fromInput >= GetInputCount() ||																				// verify circuit input
			!_FindComponent( toComponent, toComponentIndex ) ||										// verify component exists
			!_components[toComponentIndex]->FindInput( toInput, toInputIndex ) )	// verify component exists
	{
		return false;
	}

	PauseAutoTick();

	_components[toComponentIndex]->DisconnectInput( toInput );
	_inToInWires->AddWire( _components[toComponentIndex], fromInput, toInputIndex );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectInToIn( std::string fromInput, std::string toComponent, unsigned long toInput )
{
	unsigned long toComponentIndex;
	unsigned long fromInputIndex;

	if( !FindInput( fromInput, fromInputIndex ) ||									// verify circuit input
			!_FindComponent( toComponent, toComponentIndex ) ||					// verify component exists
			toInput >= _components[toComponentIndex]->GetInputCount() )	// verify component input
	{
		return false;
	}

	PauseAutoTick();

	_components[toComponentIndex]->DisconnectInput( toInput );
	_inToInWires->AddWire( _components[toComponentIndex], fromInputIndex, toInput );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectInToIn( std::string fromInput, std::string toComponent, std::string toInput )
{
	unsigned long toComponentIndex;
	unsigned long fromInputIndex;
	unsigned long toInputIndex;

	if( !FindInput( fromInput, fromInputIndex ) ||														// verify circuit input
			!_FindComponent( toComponent, toComponentIndex ) ||										// verify component exists
			!_components[toComponentIndex]->FindInput( toInput, toInputIndex ) )	// verify component exists
	{
		return false;
	}

	PauseAutoTick();

	_components[toComponentIndex]->DisconnectInput( toInput );
	_inToInWires->AddWire( _components[toComponentIndex], fromInputIndex, toInputIndex );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectOutToOut( std::string fromComponent, unsigned long fromOutput, unsigned long toOutput )
{
	unsigned long fromComponentIndex;

	if( !_FindComponent( fromComponent, fromComponentIndex ) ||							// verify component exists
			fromOutput >= _components[fromComponentIndex]->GetOutputCount() ||	// verify component output
			toOutput >= GetOutputCount() )																			// verify circuit output
	{
		return false;
	}

	PauseAutoTick();

	_outToOutWires->AddWire( _components[fromComponentIndex], fromOutput, toOutput );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectOutToOut( std::string fromComponent, unsigned long fromOutput, std::string toOutput )
{
	unsigned long fromComponentIndex;
	unsigned long toOutputIndex;

	if( !_FindComponent( fromComponent, fromComponentIndex ) ||							// verify component exists
			fromOutput >= _components[fromComponentIndex]->GetOutputCount() ||			// verify component output
			!FindOutput( toOutput, toOutputIndex ) )															// verify circuit output
	{
		return false;
	}

	PauseAutoTick();

	_outToOutWires->AddWire( _components[fromComponentIndex], fromOutput, toOutputIndex );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectOutToOut( std::string fromComponent, std::string fromOutput, unsigned long toOutput )
{
	unsigned long fromComponentIndex;
	unsigned long fromOutputIndex;

	if( !_FindComponent( fromComponent, fromComponentIndex ) ||												// verify component exists
			!_components[fromComponentIndex]->FindOutput( fromOutput, fromOutputIndex ) ||		// verify component output
			toOutput >= GetOutputCount() )																										// verify circuit output
	{
		return false;
	}

	PauseAutoTick();

	_outToOutWires->AddWire( _components[fromComponentIndex], fromOutputIndex, toOutput );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspCircuit::ConnectOutToOut( std::string fromComponent, std::string fromOutput, std::string toOutput )
{
	unsigned long fromComponentIndex;
	unsigned long fromOutputIndex;
	unsigned long toOutputIndex;

	if( !_FindComponent( fromComponent, fromComponentIndex ) ||													// verify component exists
			!_components[fromComponentIndex]->FindOutput( fromOutput, fromOutputIndex ) ||		// verify component output
			!FindOutput( toOutput, toOutputIndex ) )																				// verify circuit output
	{
		return false;
	}

	PauseAutoTick();

	_outToOutWires->AddWire( _components[fromComponentIndex], fromOutputIndex, toOutputIndex );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectOutToIn( std::string fromComponent, unsigned long fromOutput, std::string toComponent, unsigned long toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) || !_FindComponent( toComponent, toComponentIndex ) )
	{
		return;
	}

	PauseAutoTick();

	_components[toComponentIndex]->DisconnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectOutToIn( std::string fromComponent, unsigned long fromOutput, std::string toComponent, std::string toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) || !_FindComponent( toComponent, toComponentIndex ) )
	{
		return;
	}

	PauseAutoTick();

	_components[toComponentIndex]->DisconnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectOutToIn( std::string fromComponent, std::string fromOutput, std::string toComponent, unsigned long toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) || !_FindComponent( toComponent, toComponentIndex ) )
	{
		return;
	}

	PauseAutoTick();

	_components[toComponentIndex]->DisconnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectOutToIn( std::string fromComponent, std::string fromOutput, std::string toComponent, std::string toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) || !_FindComponent( toComponent, toComponentIndex ) )
	{
		return;
	}

	PauseAutoTick();

	_components[toComponentIndex]->DisconnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectInToIn( unsigned long fromInput, std::string toComponent, unsigned long toInput )
{
	unsigned long toComponentIndex;

	if( fromInput >= GetInputCount() ||														// verify circuit input
			!_FindComponent( toComponent, toComponentIndex ) ||					// verify component exists
			toInput >= _components[toComponentIndex]->GetInputCount() )	// verify component input
	{
		return;
	}

	PauseAutoTick();

	_inToInWires->RemoveWire( _components[toComponentIndex], fromInput, toInput );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectInToIn( unsigned long fromInput, std::string toComponent, std::string toInput )
{
	unsigned long toComponentIndex;
	unsigned long toInputIndex;

	if( fromInput >= GetInputCount() ||																			// verify circuit input
			!_FindComponent( toComponent, toComponentIndex ) ||										// verify component exists
			!_components[toComponentIndex]->FindInput( toInput, toInputIndex ) )	// verify component exists
	{
		return;
	}

	PauseAutoTick();

	_inToInWires->RemoveWire( _components[toComponentIndex], fromInput, toInputIndex );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectInToIn( std::string fromInput, std::string toComponent, unsigned long toInput )
{
	unsigned long toComponentIndex;
	unsigned long fromInputIndex;

	if( !FindInput( fromInput, fromInputIndex ) ||								// verify circuit input
			!_FindComponent( toComponent, toComponentIndex ) ||					// verify component exists
			toInput >= _components[toComponentIndex]->GetInputCount() )	// verify component input
	{
		return;
	}

	PauseAutoTick();

	_inToInWires->RemoveWire( _components[toComponentIndex], fromInputIndex, toInput );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectInToIn( std::string fromInput, std::string toComponent, std::string toInput )
{
	unsigned long toComponentIndex;
	unsigned long fromInputIndex;
	unsigned long toInputIndex;

	if( !FindInput( fromInput, fromInputIndex ) ||													// verify circuit input
			!_FindComponent( toComponent, toComponentIndex ) ||										// verify component exists
			!_components[toComponentIndex]->FindInput( toInput, toInputIndex ) )	// verify component exists
	{
		return;
	}

	PauseAutoTick();

	_inToInWires->RemoveWire( _components[toComponentIndex], fromInputIndex, toInputIndex );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectOutToOut( std::string fromComponent, unsigned long fromOutput, unsigned long toOutput )
{
	unsigned long fromComponentIndex;

	if( !_FindComponent( fromComponent, fromComponentIndex ) ||						// verify component exists
			fromOutput >= _components[fromComponentIndex]->GetOutputCount() ||		// verify component output
			toOutput >= GetOutputCount() )																				// verify circuit output
	{
		return;
	}

	PauseAutoTick();

	_outToOutWires->RemoveWire( _components[fromComponentIndex], fromOutput, toOutput );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectOutToOut( std::string fromComponent, unsigned long fromOutput, std::string toOutput )
{
	unsigned long fromComponentIndex;
	unsigned long toOutputIndex;

	if( !_FindComponent( fromComponent, fromComponentIndex ) ||							// verify component exists
			fromOutput >= _components[fromComponentIndex]->GetOutputCount() ||			// verify component output
			!FindOutput( toOutput, toOutputIndex ) )															// verify circuit output
	{
		return;
	}

	PauseAutoTick();

	_outToOutWires->RemoveWire( _components[fromComponentIndex], fromOutput, toOutputIndex );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectOutToOut( std::string fromComponent, std::string fromOutput, unsigned long toOutput )
{
	unsigned long fromComponentIndex;
	unsigned long fromOutputIndex;

	if( !_FindComponent( fromComponent, fromComponentIndex ) ||												// verify component exists
			!_components[fromComponentIndex]->FindOutput( fromOutput, fromOutputIndex ) ||		// verify component output
			toOutput >= GetOutputCount() )																										// verify circuit output
	{
		return;
	}

	PauseAutoTick();

	_outToOutWires->RemoveWire( _components[fromComponentIndex], fromOutputIndex, toOutput );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspCircuit::DisconnectOutToOut( std::string fromComponent, std::string fromOutput, std::string toOutput )
{
	unsigned long fromComponentIndex;
	unsigned long fromOutputIndex;
	unsigned long toOutputIndex;

	if( !_FindComponent( fromComponent, fromComponentIndex ) ||												// verify component exists
			!_components[fromComponentIndex]->FindOutput( fromOutput, fromOutputIndex ) ||		// verify component output
			!FindOutput( toOutput, toOutputIndex ) )																				// verify circuit output
	{
		return;
	}

	PauseAutoTick();

	_outToOutWires->RemoveWire( _components[fromComponentIndex], fromOutputIndex, toOutputIndex );

	ResumeAutoTick();
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
	DspSafePointer< DspWire > wire;
	DspSafePointer< DspSignal > signal;

	// set all internal component inputs from connected circuit inputs
	for( unsigned long i = 0; i < _inToInWires->GetWireCount(); i++ )
	{
		_inToInWires->GetWire( i, wire );
		inputs.GetSignal( wire->fromSignalIndex, signal );
		wire->linkedComponent->SetInputSignal( wire->toSignalIndex, signal );
	}

	if( _circuitThreads.size() == 0 || GetThreadCount() == 0 )
	{
		// tick and reset all internal components
		for( unsigned long i = 0; i < _components.size(); i++ )
		{
			_components[i]->Tick();
		}

		for( unsigned long i = 0; i < _components.size(); i++ )
		{
			_components[i]->Reset();
		}
	}
	else
	{
		_circuitThreads[_currentThreadIndex]->Sync(); // sync with thread x
		_circuitThreads[_currentThreadIndex]->Resume(); // resume thread x

		if( ++_currentThreadIndex >= GetThreadCount() )	// shift to thread x+1
		{
			_currentThreadIndex = 0;
		}
	}

	// set all circuit outputs from connected internal component outputs
	for( unsigned long i = 0; i < _outToOutWires->GetWireCount(); i++ )
	{
		_outToOutWires->GetWire( i, wire );
		wire->linkedComponent->GetOutputSignal( wire->fromSignalIndex, signal );
		outputs.SetSignal( wire->toSignalIndex, signal );
	}
}

//=================================================================================================

bool DspCircuit::_FindComponent( DspSafePointer< DspComponent > component, unsigned long& returnIndex )
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

void DspCircuit::_DisconnectComponent( unsigned long componentIndex )
{
	PauseAutoTick();

	// remove component from _inputComponents and _inputWires
	_components[ componentIndex ]->DisconnectInputs();

	// remove component from _inToInWires
	DspSafePointer< DspWire > wire;
	for( unsigned long i = 0; i < _inToInWires->GetWireCount(); i++ )
	{
		_inToInWires->GetWire( i, wire );
		if( wire->linkedComponent == _components[ componentIndex ] )
		{
			_inToInWires->RemoveWire( i );
		}
	}

	// remove component from _outToOutWires
	for( unsigned long i = 0; i < _outToOutWires->GetWireCount(); i++ )
	{
		_outToOutWires->GetWire( i, wire );
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

	for( unsigned long i = componentIndex; i < ( _components.size() - 1 ); i++ )
	{
		_components[i] = _components[i + 1];	// shift all lower elements up
	}

	_components.pop_back();	// remove end item

	ResumeAutoTick();
}

//=================================================================================================
