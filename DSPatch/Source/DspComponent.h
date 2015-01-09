/********************************************************************
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

#ifndef DSPCOMPONENT_H
#define DSPCOMPONENT_H

//-------------------------------------------------------------------------------------------------

#include "DspSignalBus.h"
#include "DspSafePointer.h"
#include "DspWireBus.h"

class DspComponentThread;

//=================================================================================================

class DspComponent
{
public:
	DspComponent();
	virtual ~DspComponent();

	bool ConnectInput( DspSafePointer< DspComponent > inputComponent, unsigned long fromOutput, unsigned long toInput );
	bool ConnectInput( DspSafePointer< DspComponent > inputComponent, unsigned long fromOutput, std::string toInput );
	bool ConnectInput( DspSafePointer< DspComponent > inputComponent, std::string fromOutput, unsigned long toInput );
	bool ConnectInput( DspSafePointer< DspComponent > inputComponent, std::string fromOutput, std::string toInput );

	void DisconnectInput( DspSafePointer< DspComponent > inputComponent, unsigned long fromOutput, unsigned long toInput );
	void DisconnectInput( DspSafePointer< DspComponent > inputComponent, unsigned long fromOutput, std::string toInput );
	void DisconnectInput( DspSafePointer< DspComponent > inputComponent, std::string fromOutput, unsigned long toInput );
	void DisconnectInput( DspSafePointer< DspComponent > inputComponent, std::string fromOutput, std::string toInput );

	void DisconnectInput( DspSafePointer< DspComponent > inputComponent );
	void DisconnectInputs();

	void Tick();
	void Reset();

	virtual void StartAutoTick();
	virtual void StopAutoTick();
	virtual void PauseAutoTick();
	virtual void ResumeAutoTick();

	virtual void SetThreadCount( unsigned long threadCount );

	void ThreadTick( unsigned long threadNo );
	void ThreadReset( unsigned long threadNo );

	// currently only works with non-threaded ticking (_threadCount = 0)
	// -----------------------------------------------------------------

	template< class ValueType >
	bool SetInputValue( unsigned long inputIndex, const ValueType& newValue );

	template< class ValueType >
	bool GetOutputValue( unsigned long outputIndex, ValueType& returnValue );

	bool SetInputSignal( unsigned long inputIndex, const DspSafePointer< DspSignal >& newSignal );
	bool GetOutputSignal( unsigned long outputIndex, DspSafePointer< DspSignal >& returnSignal );

	// currently only works with non-threaded ticking (_threadCount = 0)
	// -----------------------------------------------------------------

	unsigned long GetInputCount();
	unsigned long GetOutputCount();

	bool SetComponentName( std::string componentName );
	std::string GetComponentName();

	bool FindInput( std::string signalName, unsigned long& inputIndex ) const;
	bool FindOutput( std::string signalName, unsigned long& outputIndex ) const;

protected:
	unsigned long _threadCount;

	DspSignalBus _inputBus;
	DspSignalBus _outputBus;

	std::vector< DspSignalBus > _inputBuses;
	std::vector< DspSignalBus > _outputBuses;

	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs ) = 0;

	bool AddInput_( std::string inputName = "" );
	bool AddOutput_( std::string outputName = "" );

	void ClearInputs_();
	void ClearOutputs_();

private:
	std::string _componentName;
	bool _isAutoTickRunning;
	bool _isAutoTickPaused;

	DspWireBus _inputWires;

	// Tick members
	bool _hasTicked;

	// ThreadTick members
	DspComponentThread* _componentThread;

	std::vector< DspSafePointer< bool > > _hasTickeds;			// pointers ensure that parallel threads will only read from this vector

	std::vector< bool > _gotReleases;												// pointers not used here as only 1 thread writes to this vector at a time
	std::vector< DspMutex > _releaseMutexes;
	std::vector< DspWaitCondition > _releaseCondts;

	void _WaitForRelease( unsigned long threadNo );
	void _ReleaseThread( unsigned long threadNo );

	virtual void _ProcessInputWire( DspSafePointer< DspWire > inputWire );
	virtual void _ProcessInputWire( DspSafePointer< DspWire > inputWire, unsigned long threadNo );
};

//=================================================================================================

template< class ValueType >
bool DspComponent::SetInputValue( unsigned long inputIndex, const ValueType& newValue )
{
	return _inputBus.SetValue( inputIndex, newValue );
}

//-------------------------------------------------------------------------------------------------

template< class ValueType >
bool DspComponent::GetOutputValue( unsigned long outputIndex, ValueType& returnValue )
{
	return _outputBus.GetValue( outputIndex, returnValue );
}

//=================================================================================================

#endif // DSPCOMPONENT_H
