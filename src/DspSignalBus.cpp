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

	DspSignal* newSignal = new DspSignal( signalName );

	_signals.push_back( newSignal );

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspSignalBus::AddSignal( const DspSignal* signal )
{
	if( signal != NULL )
	{
		DspSignal* newSignal;
		std::string signalName = signal->GetSignalName();

		newSignal = new DspSignal( signalName );

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

bool DspSignalBus::SetSignal( unsigned long signalIndex, const DspSignal* newSignal )
{
	DspSignal* signal = GetSignal( signalIndex );

	if( signal != NULL && newSignal != NULL )
	{
		return signal->SetSignal( newSignal );
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

bool DspSignalBus::SetSignal( std::string signalName, const DspSignal* newSignal )
{
	DspSignal* signal = GetSignal( signalName );

	if( signal != NULL && newSignal != NULL )
	{
		return signal->SetSignal( newSignal );
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------

DspSignal* DspSignalBus::GetSignal( unsigned long signalIndex )
{
	if( signalIndex < _signals.size() )
	{
		return _signals[signalIndex];
	}
	else
	{
		return NULL;
	}
}

//-------------------------------------------------------------------------------------------------

DspSignal* DspSignalBus::GetSignal( std::string signalName )
{
	unsigned long signalIndex;
	if( FindSignal( signalName, signalIndex ) )
	{
		return _signals[signalIndex];
	}
	else
	{
		return NULL;
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
	for( unsigned long i = 0; i < _signals.size(); i++ )
	{
		delete _signals[i];
	}

	_signals.clear();
}

//-------------------------------------------------------------------------------------------------

void DspSignalBus::ClearValue( unsigned long signalIndex )
{
	DspSignal* signal = GetSignal( signalIndex );

	if( signal != NULL )
	{
		return signal->ClearValue();
	}
}

//-------------------------------------------------------------------------------------------------

void DspSignalBus::ClearValue( std::string signalName )
{
	DspSignal* signal = GetSignal( signalName );

	if( signal != NULL )
	{
		return signal->ClearValue();
	}
}

//=================================================================================================
