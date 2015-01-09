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

#ifndef DSPSIGNAL_H
#define DSPSIGNAL_H

//-------------------------------------------------------------------------------------------------

#include <string>
#include <vector>

#include "DspSafePointer.h"
#include "DspRunType.h"

//=================================================================================================

class DspSignal
{
public:
	DspSignal( std::string signalName = "" );

	virtual ~DspSignal();

	template< class ValueType >
	bool SetValue( const ValueType& newValue );

	template< class ValueType >
	bool GetValue( ValueType& returnValue );

	bool SetSignal( const DspSafePointer< DspSignal >& newSignal );

	void ClearValue();

	const type_info& GetSignalType();

	std::string GetSignalName();

private:
	DspRunType _signalValue;
	std::string _signalName;
	bool _valueAvailable;
};

//=================================================================================================

template< class ValueType >
bool DspSignal::SetValue( const ValueType& newValue )
{
	_signalValue = newValue;
	_valueAvailable = true;
	return true;
}

//-------------------------------------------------------------------------------------------------

template< class ValueType >
bool DspSignal::GetValue( ValueType& returnValue )
{
	if( _valueAvailable )
	{
		ValueType* returnValuePtr = DspRunType::RunTypeCast< ValueType >( &_signalValue );
		if( returnValuePtr != NULL )
		{
			returnValue = *returnValuePtr;
			return true;
		}
		else
		{
			assert( false );	// incorrect type matching
			return false;
		}
	}
	else
	{
		return false;
	}
}

//=================================================================================================

#endif // DSPSIGNAL_H
