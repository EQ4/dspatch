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

#ifndef DSPSIGNALBUS_H
#define DSPSIGNALBUS_H

//-------------------------------------------------------------------------------------------------

#include "DspSignal.h"
#include "DspSafePointer.h"

//=================================================================================================

class DspSignalBus
{
public:
	virtual ~DspSignalBus();

	bool AddSignal( std::string signalName = "" );
	bool AddSignal( const DspSafePointer< DspSignal >& signal );

	bool SetSignal( unsigned long signalIndex, const DspSafePointer< DspSignal >& newSignal );
	bool SetSignal( std::string signalName, const DspSafePointer< DspSignal >& newSignal );

	bool GetSignal( unsigned long signalIndex, DspSafePointer< DspSignal >& returnSignal );
	bool GetSignal( std::string signalName, DspSafePointer< DspSignal >& returnSignal );

	bool FindSignal( std::string signalName, unsigned long& signalIndex ) const;

	unsigned long GetSignalCount() const;

	void RemoveAllSignals();

	template< class ValueType >
	bool SetValue( unsigned long signalIndex, const ValueType& newValue );

	template< class ValueType >
	bool SetValue( std::string signalName, const ValueType& newValue );

	template< class ValueType >
	bool GetValue( unsigned long signalIndex, ValueType& returnValue ) const;

	template< class ValueType >
	bool GetValue( std::string signalName, ValueType& returnValue ) const;

	void ClearValue( unsigned long signalIndex );
	void ClearValue( std::string signalName );

private:
	std::vector< DspSafePointer< DspSignal > > _signals;
};

//=================================================================================================

template< class ValueType >
bool DspSignalBus::SetValue( unsigned long signalIndex, const ValueType& newValue )
{
	DspSafePointer< DspSignal > signal;
	GetSignal( signalIndex, signal );
	if( signal.IsPointerValid() )
	{
		return signal->SetValue( newValue );
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

template< class ValueType >
bool DspSignalBus::SetValue( std::string signalName, const ValueType& newValue )
{
	DspSafePointer< DspSignal > signal;
	GetSignal( signalName, signal );
	if( signal.IsPointerValid() )
	{
		return signal->SetValue( newValue );
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

template< class ValueType >
bool DspSignalBus::GetValue( unsigned long signalIndex, ValueType& returnValue ) const
{
	DspSafePointer< DspSignal > signal;

	if( signalIndex < _signals.size() )
	{
		signal = _signals[signalIndex];
	}

	if( signal.IsPointerValid() )
	{
		return signal->GetValue( returnValue );
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

template< class ValueType >
bool DspSignalBus::GetValue( std::string signalName, ValueType& returnValue ) const
{
	DspSafePointer< DspSignal > signal;
	unsigned long signalIndex;

	if( FindSignal( signalName, signalIndex ) )
	{
		signal = _signals[signalIndex];
	}

	if( signal.IsPointerValid() )
	{
		return signal->GetValue( returnValue );
	}
	else
	{
		return false;
	}
}

//=================================================================================================

#endif // DSPSIGNALBUS_H
