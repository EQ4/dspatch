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

bool DspSignalBus::AddSignal( const DspSafePointer< DspSignal >& signal )
{
	if( signal.IsPointerValid() )
	{
		DspSafePointer< DspSignal > newSignal;
		newSignal.New( signal->GetSignalName() );
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

bool DspSignalBus::SetSignal( unsigned long signalIndex, const DspSafePointer< DspSignal >& newSignal )
{
	DspSafePointer< DspSignal > signal;
	GetSignal( signalIndex, signal );

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

bool DspSignalBus::SetSignal( std::string signalName, const DspSafePointer< DspSignal >& newSignal )
{
	DspSafePointer< DspSignal > signal;
	GetSignal( signalName, signal );

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

bool DspSignalBus::GetSignal( unsigned long signalIndex, DspSafePointer< DspSignal >& returnSignal )
{
	if( signalIndex < _signals.size() )
	{
		returnSignal = _signals[signalIndex];
		return true;
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

bool DspSignalBus::GetSignal( std::string signalName, DspSafePointer< DspSignal >& returnSignal )
{
	unsigned long signalIndex;
	if( FindSignal( signalName, signalIndex ) )
	{
		returnSignal = _signals[signalIndex];
		return true;
	}
	else
	{
		return false;
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
	DspSafePointer< DspSignal > signal;
	GetSignal( signalIndex, signal );

	if( signal.IsPointerValid() )
	{
		return signal->ClearValue();
	}
}

//-------------------------------------------------------------------------------------------------

void DspSignalBus::ClearValue( std::string signalName )
{
	DspSafePointer< DspSignal > signal;
	GetSignal( signalName, signal );

	if( signal.IsPointerValid() )
	{
		return signal->ClearValue();
	}
}

//=================================================================================================
