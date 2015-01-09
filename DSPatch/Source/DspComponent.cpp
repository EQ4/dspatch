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

#include "DspComponent.h"
#include "DspComponentThread.h"
#include "DspWire.h"

//=================================================================================================

DspComponent::DspComponent()
: _componentName( "" ),
	_isAutoTickRunning( false ),
	_isAutoTickPaused( false ),
	_hasTicked( false ),
	_threadCount( 0 ),
	_componentThread( NULL ) {}

//-------------------------------------------------------------------------------------------------

DspComponent::~DspComponent()
{
	StopAutoTick();
	SetThreadCount( 0 );
	DisconnectInputs();
}

//=================================================================================================

bool DspComponent::ConnectInput( DspSafePointer< DspComponent > inputComponent, unsigned long fromOutput, unsigned long toInput )
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

bool DspComponent::ConnectInput( DspSafePointer< DspComponent > inputComponent, unsigned long fromOutput, std::string toInput )
{
	unsigned long toInputIndex;

	if( _inputBus.FindSignal( toInput, toInputIndex ) )
	{
		return ConnectInput( inputComponent, fromOutput, toInputIndex );
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspSafePointer< DspComponent > inputComponent, std::string fromOutput, unsigned long toInput )
{
	unsigned long fromOutputIndex;

	if( inputComponent->_outputBus.FindSignal( fromOutput, fromOutputIndex ) )
	{
		return ConnectInput( inputComponent, fromOutputIndex, toInput );
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::ConnectInput( DspSafePointer< DspComponent > inputComponent, std::string fromOutput, std::string toInput )
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

void DspComponent::DisconnectInput( DspSafePointer< DspComponent > inputComponent, unsigned long fromOutput, unsigned long toInput )
{
	PauseAutoTick();
	_inputWires.RemoveWire( inputComponent, fromOutput, toInput );
	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspSafePointer< DspComponent > inputComponent, unsigned long fromOutput, std::string toInput )
{
	unsigned long toInputIndex;

	if( _inputBus.FindSignal( toInput, toInputIndex ) )
	{
		DisconnectInput( inputComponent, fromOutput, toInputIndex );
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspSafePointer< DspComponent > inputComponent, std::string fromOutput, unsigned long toInput )
{
	unsigned long fromOutputIndex;

	if( inputComponent->_outputBus.FindSignal( fromOutput, fromOutputIndex ) )
	{
		DisconnectInput( inputComponent, fromOutputIndex, toInput );
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::DisconnectInput( DspSafePointer< DspComponent > inputComponent, std::string fromOutput, std::string toInput )
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

void DspComponent::DisconnectInput( unsigned long inputIndex )
{
	PauseAutoTick();

	// remove inputComponent from _inputWires
	DspSafePointer< DspWire > wire;
	for( unsigned long i = 0; i < _inputWires.GetWireCount(); i++ )
	{
		_inputWires.GetWire( i, wire );
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

void DspComponent::DisconnectInput( DspSafePointer< DspComponent > inputComponent )
{
	PauseAutoTick();

	// remove inputComponent from _inputWires
	DspSafePointer< DspWire > wire;
	for( unsigned long i = 0; i < _inputWires.GetWireCount(); i++ )
	{
		_inputWires.GetWire( i, wire );
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

void DspComponent::Tick()
{
	// continue only if this component has not already been ticked
	if( !_hasTicked )
	{
		// 1. set _hasTicked flag
		_hasTicked = true;

		// 2. get outputs required from input components
		DspSafePointer< DspWire > wire;
		for( unsigned long i = 0; i < _inputWires.GetWireCount(); i++ )
		{
			_inputWires.GetWire( i, wire );
			wire->linkedComponent->Tick();
			_ProcessInputWire( wire );
		}

		// 3. call Process_() with newly aquired inputs
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

//-------------------------------------------------------------------------------------------------

void DspComponent::StopAutoTick()
{
	if( _componentThread != NULL )
	{
		_componentThread->Stop();
		delete _componentThread;
		_componentThread = NULL;

		_isAutoTickRunning = false;
		_isAutoTickPaused = false;
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::PauseAutoTick()
{
	if( _isAutoTickRunning )
	{
		_componentThread->Pause();
		_isAutoTickPaused = true;
		_isAutoTickRunning = false;
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::ResumeAutoTick()
{
	if( _isAutoTickPaused )
	{
		_componentThread->Resume();
		_isAutoTickPaused = false;
		_isAutoTickRunning = true;
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::SetThreadCount( unsigned long threadCount )
{
	// _threadCount is the current thread count / _threadCount is new thread count

	_hasTickeds.resize( threadCount );
	_inputBuses.resize( threadCount );
	_outputBuses.resize( threadCount );

	_gotReleases.resize( threadCount );
	_releaseMutexes.resize( threadCount );
	_releaseCondts.resize( threadCount );

	for( unsigned long i = _threadCount; i < threadCount; i++ )
	{
		_hasTickeds[i] = false;
		_gotReleases[i] = false;

		for( unsigned long j = 0; j < _inputBus.GetSignalCount(); j++ )
		{
			DspSafePointer< DspSignal > variable;
			_inputBus.GetSignal( j, variable );
			_inputBuses[i].AddSignal( variable );
		}

		for( unsigned long j = 0; j < _outputBus.GetSignalCount(); j++ )
		{
			DspSafePointer< DspSignal > variable;
			_outputBus.GetSignal( j, variable );
			_outputBuses[i].AddSignal( variable );
		}
	}

	if( threadCount > 0 )
	{
		_gotReleases[0] = true;
	}

	_threadCount = threadCount;
}

//-------------------------------------------------------------------------------------------------

unsigned long DspComponent::GetThreadCount()
{
	return _threadCount;
}

//-------------------------------------------------------------------------------------------------

void DspComponent::ThreadTick( unsigned long threadNo )
{
	// continue only if this component has not already been ticked
	if( _hasTickeds[threadNo] == false )
	{
		// 1. set _hasTicked flag
		_hasTickeds[threadNo] = true;

		// 2. get outputs required from input components
		DspSafePointer< DspWire > wire;
		for( unsigned long i = 0; i < _inputWires.GetWireCount(); i++ )
		{
			_inputWires.GetWire( i, wire );
			wire->linkedComponent->ThreadTick( threadNo );
			_ProcessInputWire( wire, threadNo );
		}

		// 3. wait for your turn to process.
		_WaitForRelease( threadNo );

		// 4. call Process_() with newly aquired inputs
		Process_( _inputBuses[threadNo], _outputBuses[threadNo] );

		// 5. signal that you're done processing.
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
	_hasTickeds[threadNo] = false;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::SetInputSignal( unsigned long inputIndex, const DspSafePointer< DspSignal >& newSignal )
{
	return _inputBus.SetSignal( inputIndex, newSignal );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::SetInputSignal( std::string inputName, const DspSafePointer< DspSignal >& newSignal )
{
	return _inputBus.SetSignal( inputName, newSignal );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::GetOutputSignal( unsigned long outputIndex, DspSafePointer< DspSignal >& returnSignal )
{
	return _outputBus.GetSignal( outputIndex, returnSignal );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::GetOutputSignal( std::string outputName, DspSafePointer< DspSignal >& returnSignal )
{
	return _outputBus.GetSignal( outputName, returnSignal );
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

bool DspComponent::SetComponentName( std::string componentName )
{
	if( _componentName == "" )
	{
		_componentName = componentName;
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------

std::string DspComponent::GetComponentName()
{
	return _componentName;
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::FindInput( std::string signalName, unsigned long& returnIndex ) const
{
	return _inputBus.FindSignal( signalName, returnIndex );
}

//-------------------------------------------------------------------------------------------------

bool DspComponent::FindOutput( std::string signalName, unsigned long& returnIndex ) const
{
	return _outputBus.FindSignal( signalName, returnIndex );
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

	if( nextThread >= _threadCount )
		nextThread = 0;

	_releaseMutexes[nextThread].Lock();
	_gotReleases[nextThread] = true;
	_releaseCondts[nextThread].WakeAll();
	_releaseMutexes[nextThread].Unlock();
}

//-------------------------------------------------------------------------------------------------

void DspComponent::_ProcessInputWire( DspSafePointer< DspWire > inputWire )
{
	DspSafePointer< DspSignal > signal;
	inputWire->linkedComponent->_outputBus.GetSignal( inputWire->fromSignalIndex, signal );

	if( !_inputBus.SetSignal( inputWire->toSignalIndex, signal ) )
	{
		// value not set
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponent::_ProcessInputWire( DspSafePointer< DspWire > inputWire, unsigned long threadNo )
{
	DspSafePointer< DspSignal > signal;
	inputWire->linkedComponent->_outputBuses[threadNo].GetSignal( inputWire->fromSignalIndex, signal );

	if( !_inputBuses[threadNo].SetSignal( inputWire->toSignalIndex, signal ) )
	{
		// value not set
	}
}

//=================================================================================================
