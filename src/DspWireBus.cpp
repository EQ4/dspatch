/************************************************************************
DSPatch - Cross-Platform, Object-Oriented, Flow-Based Programming Library
Copyright (c) 2013 Marcus Tomlinson

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

#include "DspWireBus.h"
#include "DspComponent.h"
#include "DspWire.h"

//=================================================================================================

DspWireBus::DspWireBus( bool isLinkedComponentReceivingSignals )
: _isLinkedComponentReceivingSignals( isLinkedComponentReceivingSignals ) {}

//-------------------------------------------------------------------------------------------------

DspWireBus::~DspWireBus()
{
	RemoveAllWires();
}

//=================================================================================================

bool DspWireBus::AddWire( DspComponent* linkedComponent, unsigned long fromSignalIndex, unsigned long toSignalIndex )
{
	for( unsigned long i = 0; i < _wires.size(); i++ )
	{
		if( _wires[i]->linkedComponent == linkedComponent &&
				_wires[i]->fromSignalIndex == fromSignalIndex &&
				_wires[i]->toSignalIndex == toSignalIndex )
		{
			return false;	// wire already exists
		}
	}

	for( unsigned long i = 0; i < _wires.size(); i++ )
	{
		if( _isLinkedComponentReceivingSignals &&
				_wires[i]->linkedComponent == linkedComponent &&
				_wires[i]->toSignalIndex == toSignalIndex )	// if there's a wire to the receiving component's input already
		{
			RemoveWire( i );	// remove the wire (only one wire can connect to an input at a time)
			break;
		}
		else if( !_isLinkedComponentReceivingSignals &&
						 _wires[i]->toSignalIndex == toSignalIndex )
		{
			RemoveWire( i );	// remove the wire (only one wire can connect to an input at a time)
			break;
		}
	}

	DspWire* newWire = new DspWire();

	newWire->linkedComponent = linkedComponent;
	newWire->fromSignalIndex = fromSignalIndex;
	newWire->toSignalIndex = toSignalIndex;
	_wires.push_back( newWire );

	return true;
}

//-------------------------------------------------------------------------------------------------

bool DspWireBus::RemoveWire( DspComponent* linkedComponent, unsigned long fromSignalIndex, unsigned long toSignalIndex )
{
	for( unsigned long i = 0; i < _wires.size(); i++ )
	{
		if( _wires[i]->linkedComponent == linkedComponent &&
				_wires[i]->fromSignalIndex == fromSignalIndex &&
				_wires[i]->toSignalIndex == toSignalIndex )
		{
			RemoveWire( i );
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DspWireBus::RemoveWire( unsigned long wireIndex )
{
	if( wireIndex > _wires.size() )
	{
		return false;
	}

	delete _wires[wireIndex];

	for( unsigned long j = wireIndex; j < ( _wires.size() - 1 ); j++ )
	{
		_wires[j] = _wires[j + 1];	// shift all other elements up
	}
	_wires.pop_back();	// remove end item

	return true;
}

//-------------------------------------------------------------------------------------------------

void DspWireBus::RemoveAllWires()
{
	for( unsigned long i = 0; i < _wires.size(); i++ )
	{
		RemoveWire( i );
	}
}

//-------------------------------------------------------------------------------------------------

DspWire* DspWireBus::GetWire( unsigned long wireIndex )
{
	if( wireIndex > _wires.size() )
	{
		return NULL;
	}
	else
	{
		return _wires[wireIndex];
	}
}

//-------------------------------------------------------------------------------------------------

unsigned long DspWireBus::GetWireCount()
{
	return _wires.size();
}

//=================================================================================================
