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

#include "DspSignalBus.h"

//=================================================================================================

DspSignalBus::~DspSignalBus() {};

//=================================================================================================

bool DspSignalBus::AddSignal( std::string signalName )
{
	if( signalName != "" )
	{
		unsigned long signalIndex;
		if( FindSignal( signalName, signalIndex ) )	// if the name already exists
		{
			return false;
		}
	}

	DspSafePointer< DspSignal > newSignal;
	newSignal.New( signalName );
	newSignal.LockPointer();

	_signals.push_back( newSignal );

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspSignalBus::AddSignal( const DspSafePointer< DspSignal > signal )
{
	if( signal.IsPointerValid() )
	{
		DspSafePointer< DspSignal > newSignal;
		std::string signalName = signal->GetSignalName();

		newSignal.New( signalName );
		newSignal.LockPointer();

		newSignal->SetSignal( signal );
		_signals.push_back( newSignal );
		return true;
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

bool DspSignalBus::SetSignal( unsigned long signalIndex, const DspSafePointer< DspSignal > newSignal )
{
	DspSafePointer< DspSignal > signal = GetSignal( signalIndex );

	if( signal.IsPointerValid() && newSignal.IsPointerValid() )
	{
		return signal->SetSignal( newSignal );
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

bool DspSignalBus::SetSignal( std::string signalName, const DspSafePointer< DspSignal > newSignal )
{
	DspSafePointer< DspSignal > signal = GetSignal( signalName );

	if( signal.IsPointerValid() && newSignal.IsPointerValid() )
	{
		return signal->SetSignal( newSignal );
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

DspSafePointer< DspSignal > DspSignalBus::GetSignal( unsigned long signalIndex )
{
	if( signalIndex < _signals.size() )
	{
		return _signals[signalIndex];
	}
	else
	{
		return DspSafePointer< DspSignal >();
	}
}

//-------------------------------------------------------------------------------------------------

DspSafePointer< DspSignal > DspSignalBus::GetSignal( std::string signalName )
{
	unsigned long signalIndex;
	if( FindSignal( signalName, signalIndex ) )
	{
		return _signals[signalIndex];
	}
	else
	{
		return DspSafePointer< DspSignal >();
	}
}

//-------------------------------------------------------------------------------------------------

bool DspSignalBus::FindSignal( std::string signalName, unsigned long& signalIndex ) const
{
	for( unsigned long i = 0; i < _signals.size(); i++ )
	{
		if( signalName != "" && _signals[i]->GetSignalName() == signalName )
		{
			signalIndex = i;
			return true;
		}
	}
	// if you get here the variable was not found.
	return false;
}

//-------------------------------------------------------------------------------------------------

unsigned long DspSignalBus::GetSignalCount() const
{
	return _signals.size();
}

//-------------------------------------------------------------------------------------------------

void DspSignalBus::RemoveAllSignals()
{
	_signals.clear();
}

//-------------------------------------------------------------------------------------------------

void DspSignalBus::ClearValue( unsigned long signalIndex )
{
	DspSafePointer< DspSignal > signal = GetSignal( signalIndex );

	if( signal.IsPointerValid() )
	{
		return signal->ClearValue();
	}
}

//-------------------------------------------------------------------------------------------------

void DspSignalBus::ClearValue( std::string signalName )
{
	DspSafePointer< DspSignal > signal = GetSignal( signalName );

	if( signal.IsPointerValid() )
	{
		return signal->ClearValue();
	}
}

//=================================================================================================
