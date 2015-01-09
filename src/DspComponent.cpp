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

#include "../include/DSPatch.h"

#include "DspCircuit.h"
#include "DspComponent.h"
#include "DspComponentThread.h"
#include "DspWire.h"

//=================================================================================================

DspComponent::DspComponent()
: _parentCircuit( NULL ),
	_componentName( "" ),
	_isAutoTickRunning( false ),
	_isAutoTickPaused( false ),
	_hasTicked( false ),
	_bufferCount( 0 ),
	_componentThread( NULL ) {}

//-------------------------------------------------------------------------------------------------

DspComponent::~DspComponent()
{
	if( _parentCircuit != NULL )
	{
		_parentCircuit->RemoveComponent( this );
	}

	StopAutoTick();
	SetBufferCount( 0 );
	DisconnectInputs();
}

//=================================================================================================

void DspComponent::SetComponentName( std::string componentName )
{
	_componentName = componentName;
}

//-------------------------------------------------------------------------------------------------

std::string DspComponent::GetComponentName()
{
	return _componentName;
}

//-------------------------------------------------------------------------------------------------

void DspComponent::SetParentCircuit( DspCircuit* parentCircuit )
{
	if( _parentCircuit != parentCircuit && parentCircuit != this )
	{
		DspCircuit* currentParent = _parentCircuit;
		_parentCircuit = NULL;

		StopAutoTick(); // a component can't auto tick within a circuit

		// if this component is part of another circuit, remove it from that circuit first
		if( currentParent != NULL )
		{
			currentParent->RemoveComponent( this );
		}

		_parentCircuit = parentCircuit;

		// this method is called from within AddComponent() so don't call AddComponent() here
	}
}

//-------------------------------------------------------------------------------------------------

DspCircuit* DspComponent::GetParentCircuit()
{
	return _parentCircuit;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspComponent* inputComponent, unsigned long fromOutput, unsigned long toInput )
{
	if( toInput < this->GetInputCount() && fromOutput < inputComponent->GetOutputCount() )
	{
		PauseAutoTick();
		_inputWires.AddWire( inputComponent, fromOutput, toInput );
		ResumeAutoTick();
		return true;
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspComponent* inputComponent, unsigned long fromOutput, std::string toInput )
{
	unsigned long toInputIndex;

	if( _inputBus.FindSignal( toInput, toInputIndex ) )
	{
		return ConnectInput( inputComponent, fromOutput, toInputIndex );
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspComponent* inputComponent, std::string fromOutput, unsigned long toInput )
{
	unsigned long fromOutputIndex;

	if( inputComponent->_outputBus.FindSignal( fromOutput, fromOutputIndex ) )
	{
		return ConnectInput( inputComponent, fromOutputIndex, toInput );
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspComponent* inputComponent, std::string fromOutput, std::string toInput )
{
	unsigned long fromOutputIndex;
	unsigned long toInputIndex;

	if( inputComponent->_outputBus.FindSignal( fromOutput, fromOutputIndex ) &&
			_inputBus.FindSignal( toInput, toInputIndex ) )
	{
		return ConnectInput( inputComponent, fromOutputIndex, toInputIndex );
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspComponent& inputComponent, unsigned long fromOutput, unsigned long toInput )
{
	return ConnectInput( &inputComponent, fromOutput, toInput );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspComponent& inputComponent, unsigned long fromOutput, std::string toInput )
{
	return ConnectInput( &inputComponent, fromOutput, toInput );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspComponent& inputComponent, std::string fromOutput, unsigned long toInput )
{
	return ConnectInput( &inputComponent, fromOutput, toInput );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspComponent& inputComponent, std::string fromOutput, std::string toInput )
{
	return ConnectInput( &inputComponent, fromOutput, toInput );
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspComponent* inputComponent, unsigned long fromOutput, unsigned long toInput )
{
	PauseAutoTick();
	_inputWires.RemoveWire( inputComponent, fromOutput, toInput );
	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspComponent* inputComponent, unsigned long fromOutput, std::string toInput )
{
	unsigned long toInputIndex;

	if( _inputBus.FindSignal( toInput, toInputIndex ) )
	{
		DisconnectInput( inputComponent, fromOutput, toInputIndex );
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspComponent* inputComponent, std::string fromOutput, unsigned long toInput )
{
	unsigned long fromOutputIndex;

	if( inputComponent->_outputBus.FindSignal( fromOutput, fromOutputIndex ) )
	{
		DisconnectInput( inputComponent, fromOutputIndex, toInput );
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspComponent* inputComponent, std::string fromOutput, std::string toInput )
{
	unsigned long fromOutputIndex;
	unsigned long toInputIndex;

	if( inputComponent->_outputBus.FindSignal( fromOutput, fromOutputIndex ) &&
			_inputBus.FindSignal( toInput, toInputIndex ) )
	{
		DisconnectInput( inputComponent, fromOutputIndex, toInputIndex );
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspComponent& inputComponent, unsigned long fromOutput, unsigned long toInput )
{
	DisconnectInput( &inputComponent, fromOutput, toInput );
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspComponent& inputComponent, unsigned long fromOutput, std::string toInput )
{
	DisconnectInput( &inputComponent, fromOutput, toInput );
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspComponent& inputComponent, std::string fromOutput, unsigned long toInput )
{
	DisconnectInput( &inputComponent, fromOutput, toInput );
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspComponent& inputComponent, std::string fromOutput, std::string toInput )
{
	DisconnectInput( &inputComponent, fromOutput, toInput );
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( unsigned long inputIndex )
{
	PauseAutoTick();

	// remove inputComponent from _inputWires
	DspWire* wire;
	for( unsigned long i = 0; i < _inputWires.GetWireCount(); i++ )
	{
		wire = _inputWires.GetWire( i );
		if( wire->toSignalIndex == inputIndex )
		{
			_inputWires.RemoveWire( i );
			break;
		}
	}

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( std::string inputName )
{
	unsigned long inputIndex;

	if( FindInput( inputName, inputIndex ) )
	{
		DisconnectInput( inputIndex );
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspComponent* inputComponent )
{
	PauseAutoTick();

	// remove inputComponent from _inputWires
	DspWire* wire;
	for( unsigned long i = 0; i < _inputWires.GetWireCount(); i++ )
	{
		wire = _inputWires.GetWire( i );
		if( wire->linkedComponent == inputComponent )
		{
			_inputWires.RemoveWire( i );
		}
	}

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInputs()
{
	PauseAutoTick();
	_inputWires.RemoveAllWires();
	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

unsigned long DspComponent::GetInputCount()
{
	return _inputBus.GetSignalCount();
}

//-------------------------------------------------------------------------------------------------

unsigned long DspComponent::GetOutputCount()
{
	return _outputBus.GetSignalCount();
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::FindInput( std::string signalName, unsigned long& returnIndex ) const
{
	return _inputBus.FindSignal( signalName, returnIndex );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::FindInput( unsigned long signalIndex, unsigned long& returnIndex ) const
{
	if( signalIndex < _inputBus.GetSignalCount() )
	{
		returnIndex = signalIndex;
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::FindOutput( std::string signalName, unsigned long& returnIndex ) const
{
	return _outputBus.FindSignal( signalName, returnIndex );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::FindOutput( unsigned long signalIndex, unsigned long& returnIndex ) const
{
	if( signalIndex < _outputBus.GetSignalCount() )
	{
		returnIndex = signalIndex;
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

void DspComponent::Tick()
{
	// continue only if this component has not already been ticked
	if( !_hasTicked )
	{
		// 1. set _hasTicked flag
		_hasTicked = true;

		// 2. get outputs required from input components
		DspWire* wire;
		for( unsigned long i = 0; i < _inputWires.GetWireCount(); i++ )
		{
			wire = _inputWires.GetWire( i );
			wire->linkedComponent->Tick();
			_ProcessInputWire( wire );
		}

		// 3. clear all outputs
		for( unsigned long i = 0; i < _outputBus.GetSignalCount(); i++ )
		{
			_outputBus.ClearValue( i );
		}

		// 4. call Process_() with newly aquired inputs
		Process_( _inputBus, _outputBus );
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::Reset()
{
	// clear all inputs
	for( unsigned long i = 0; i < _inputBus.GetSignalCount(); i++ )
	{
		_inputBus.ClearValue( i );
	}

	// reset _hasTicked flag
	_hasTicked = false;
}

//-------------------------------------------------------------------------------------------------

void DspComponent::StartAutoTick()
{
	// Global scoped components (components not within a circuit) are added to the "global circuit" in
	// order to be auto-ticked. Technically it is only the global circuit that auto-ticks -This in turn
	// auto-ticks all components contained.

	// if this is the global circuit
	if( DSPatch::IsThisGlobalCircuit( this ) )
	{
		if( _componentThread == NULL )
		{
			_componentThread = new DspComponentThread( *this );

			_isAutoTickRunning = true;
			_isAutoTickPaused = false;
		}
		else
		{
			ResumeAutoTick();
		}
	}
	// else if this component has no parent or it's parent is the global circuit
	else if( _parentCircuit == NULL || DSPatch::IsThisGlobalCircuit( _parentCircuit ) )
	{
		DSPatch::AddGlobalComponent( this );
		DSPatch::StartGlobalAutoTick();
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::StopAutoTick()
{
	// If a component is part of the global circuit, a call to StopAutoTick() removes it from the
	// global circuit as to stop it from being auto-ticked. When all components are removed, the
	// global circuit auto-ticking is stopped.

	// if this is the global circuit
	if( DSPatch::IsThisGlobalCircuit( this ) && _componentThread != NULL )
	{
		_componentThread->Stop();
		delete _componentThread;
		_componentThread = NULL;

		_isAutoTickRunning = false;
		_isAutoTickPaused = false;
	}
	// else if this component's parent is the global circuit
	else if( DSPatch::IsThisGlobalCircuit( _parentCircuit ) )
	{
		DSPatch::RemoveGlobalComponent( this );

		if( DSPatch::GetGlobalComponentCount() == 0 )
		{
			DSPatch::StopGlobalAutoTick();
		}
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::PauseAutoTick()
{
	// A call to PauseAutoTick() recursively traverses it's parent circuits until it reaches the
	// global circuit. When the global circuit is reach, it's auto-tick is paused.

	// if this is the global circuit
	if( DSPatch::IsThisGlobalCircuit( this ) && _componentThread != NULL )
	{
		if( _isAutoTickRunning )
		{
			_componentThread->Pause();
			_isAutoTickPaused = true;
			_isAutoTickRunning = false;
		}
	}
	else if( _parentCircuit != NULL )
	{
		_parentCircuit->PauseAutoTick(); // recursive call to find the global circuit
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::ResumeAutoTick()
{
	// A call to ResumeAutoTick() recursively traverses it's parent circuits until it reaches the
	// global circuit. When the global circuit is reach, it's auto-tick is resumed.

	// if this is the global circuit
	if( DSPatch::IsThisGlobalCircuit( this ) && _isAutoTickPaused )
	{
		_componentThread->Resume();
		_isAutoTickPaused = false;
		_isAutoTickRunning = true;
	}
	else if( _parentCircuit != NULL )
	{
		_parentCircuit->ResumeAutoTick(); // recursive call to find the global circuit
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::SetBufferCount( unsigned long bufferCount )
{
	// _bufferCount is the current thread count / bufferCount is new thread count

	// delete excess _hasTickeds (if new buffer count is less than current)
	for( long i = _bufferCount - 1; i >= (long)bufferCount; i-- )
	{
		delete _hasTickeds[i];
	}

	// resize local buffer array
	_hasTickeds.resize( bufferCount );

	// create excess _hasTickeds (if new buffer count is more than current)
	for( unsigned long i = _bufferCount; i < bufferCount; i++ )
	{
		_hasTickeds[i] = new bool();
	}

	_inputBuses.resize( bufferCount );
	_outputBuses.resize( bufferCount );

	_gotReleases.resize( bufferCount );
	_releaseMutexes.resize( bufferCount );
	_releaseCondts.resize( bufferCount );

	for( unsigned long i = _bufferCount; i < bufferCount; i++ )
	{
		*_hasTickeds[i] = false;
		_gotReleases[i] = false;

		for( unsigned long j = 0; j < _inputBus.GetSignalCount(); j++ )
		{
			DspSignal* variable;
			variable = _inputBus.GetSignal( j );
			_inputBuses[i].AddSignal( variable );
		}

		for( unsigned long j = 0; j < _outputBus.GetSignalCount(); j++ )
		{
			DspSignal* variable;
			variable = _outputBus.GetSignal( j );
			_outputBuses[i].AddSignal( variable );
		}
	}

	if( bufferCount > 0 )
	{
		_gotReleases[0] = true;
	}

	_bufferCount = bufferCount;
}

//-------------------------------------------------------------------------------------------------

unsigned long DspComponent::GetBufferCount()
{
	return _bufferCount;
}

//-------------------------------------------------------------------------------------------------

void DspComponent::ThreadTick( unsigned long threadNo )
{
	// continue only if this component has not already been ticked
	if( *_hasTickeds[threadNo] == false )
	{
		// 1. set _hasTicked flag
		*_hasTickeds[threadNo] = true;

		// 2. get outputs required from input components
		DspWire* wire;
		for( unsigned long i = 0; i < _inputWires.GetWireCount(); i++ )
		{
			wire = _inputWires.GetWire( i );

			// continue only if the linked component has enough buffers
			if( wire->linkedComponent->GetBufferCount() > threadNo )
			{
				wire->linkedComponent->ThreadTick( threadNo );
				_ProcessInputWire( wire, threadNo );
			}
		}

		// 3. clear all outputs
		for( unsigned long i = 0; i < _outputBuses[threadNo].GetSignalCount(); i++ )
		{
			_outputBuses[threadNo].ClearValue( i );
		}

		// 4. wait for your turn to process.
		_WaitForRelease( threadNo );

		// 5. call Process_() with newly aquired inputs
		Process_( _inputBuses[threadNo], _outputBuses[threadNo] );

		// 6. signal that you're done processing.
		_ReleaseThread( threadNo );
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::ThreadReset( unsigned long threadNo )
{
	// clear all inputs
	for( unsigned long i = 0; i < _inputBuses[threadNo].GetSignalCount(); i++ )
	{
		_inputBuses[threadNo].ClearValue( i );
	}

	// reset _hasTicked flag
	*_hasTickeds[threadNo] = false;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::SetInputSignal( unsigned long inputIndex, const DspSignal* newSignal )
{
	return _inputBus.SetSignal( inputIndex, newSignal );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::SetInputSignal( unsigned long inputIndex, unsigned long threadIndex, const DspSignal* newSignal )
{
	return _inputBuses[threadIndex].SetSignal( inputIndex, newSignal );
}

//-------------------------------------------------------------------------------------------------

DspSignal* DspComponent::GetOutputSignal( unsigned long outputIndex )
{
	return _outputBus.GetSignal( outputIndex );
}

//-------------------------------------------------------------------------------------------------

DspSignal* DspComponent::GetOutputSignal( unsigned long outputIndex, unsigned long threadIndex )
{
	return _outputBuses[threadIndex].GetSignal( outputIndex );
}

//=================================================================================================

bool DspComponent::AddInput_( std::string inputName )
{
	for( unsigned long i = 0; i < _inputBuses.size(); i++ )
	{
		_inputBuses[i].AddSignal( inputName );
	}
	return _inputBus.AddSignal( inputName );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::AddOutput_( std::string outputName )
{
	for( unsigned long i = 0; i < _outputBuses.size(); i++ )
	{
		_outputBuses[i].AddSignal( outputName );
	}
	return _outputBus.AddSignal( outputName );
}

//-------------------------------------------------------------------------------------------------

void DspComponent::ClearInputs_()
{
	for( unsigned long i = 0; i < _inputBuses.size(); i++ )
	{
		_inputBuses[i].RemoveAllSignals();
	}
	return _inputBus.RemoveAllSignals();
}

//-------------------------------------------------------------------------------------------------

void DspComponent::ClearOutputs_()
{
	for( unsigned long i = 0; i < _outputBuses.size(); i++ )
	{
		_outputBuses[i].RemoveAllSignals();
	}
	return _outputBus.RemoveAllSignals();
}

//=================================================================================================

void DspComponent::_WaitForRelease( unsigned long threadNo )
{
	_releaseMutexes[threadNo].Lock();
	if( !_gotReleases[threadNo] )
	{
		_releaseCondts[threadNo].Wait( _releaseMutexes[threadNo] );		//wait for resume
	}
	_gotReleases[threadNo] = false;											//reset the release flag
	_releaseMutexes[threadNo].Unlock();
}

//-------------------------------------------------------------------------------------------------

void DspComponent::_ReleaseThread( unsigned long threadNo )
{
	unsigned long nextThread = threadNo + 1;

	if( nextThread >= _bufferCount )
		nextThread = 0;

	_releaseMutexes[nextThread].Lock();
	_gotReleases[nextThread] = true;
	_releaseCondts[nextThread].WakeAll();
	_releaseMutexes[nextThread].Unlock();
}

//-------------------------------------------------------------------------------------------------

void DspComponent::_ProcessInputWire( DspWire* inputWire )
{
	DspSignal* signal;
	signal = inputWire->linkedComponent->_outputBus.GetSignal( inputWire->fromSignalIndex );

	if( !_inputBus.SetSignal( inputWire->toSignalIndex, signal ) )
	{
		// value not set
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::_ProcessInputWire( DspWire* inputWire, unsigned long threadNo )
{
	DspSignal* signal;
	signal = inputWire->linkedComponent->_outputBuses[threadNo].GetSignal( inputWire->fromSignalIndex );

	if( !_inputBuses[threadNo].SetSignal( inputWire->toSignalIndex, signal ) )
	{
		// value not set
	}
}

//=================================================================================================
