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

	bool SetSignal( const DspSafePointer< DspSignal > newSignal );

	void ClearValue();

	const std::type_info& GetSignalType();

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
			return false; // incorrect type matching
		}
	}
	else
	{
		return false; // no value available
	}
}

//=================================================================================================

#endif // DSPSIGNAL_H
