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
	bool AddSignal( const DspSafePointer< DspSignal > signal );

	bool SetSignal( unsigned long signalIndex, const DspSafePointer< DspSignal > newSignal );
	bool SetSignal( std::string signalName, const DspSafePointer< DspSignal > newSignal );

	DspSafePointer< DspSignal > GetSignal( unsigned long signalIndex );
	DspSafePointer< DspSignal > GetSignal( std::string signalName );

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
	DspSafePointer< DspSignal > signal = GetSignal( signalIndex );
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
	DspSafePointer< DspSignal > signal = GetSignal( signalName );
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
