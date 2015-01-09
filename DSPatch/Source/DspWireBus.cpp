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

#include "DspWireBus.h"
#include "DspComponent.h"
#include "DspWire.h"

//=================================================================================================

DspWireBus::~DspWireBus()
{
	_wires.clear();
}

//=================================================================================================

void DspWireBus::AddWire( DspSafePointer< DspComponent > linkedComponent,	unsigned long fromSignalIndex, unsigned long toSignalIndex )
{
	for( unsigned long i = 0; i < _wires.size(); i++ )
	{
		if( _wires[i]->linkedComponent == linkedComponent &&
			_wires[i]->fromSignalIndex == fromSignalIndex &&
			_wires[i]->toSignalIndex == toSignalIndex )
		{
			return;	// wire already exists
		}
	}

	for( unsigned long i = 0; i < _wires.size(); i++ )
	{
		if( _wires[i]->toSignalIndex == toSignalIndex )	// if there's a wire to this input already
		{
			RemoveWire( i );	// remove the wire (only one wire can connect to an input at a time)
			break;
		}
	}

	DspSafePointer< DspWire > newWire;
	newWire.New();
	newWire.LockPointer();

	newWire->linkedComponent = linkedComponent;
	newWire->fromSignalIndex = fromSignalIndex;
	newWire->toSignalIndex = toSignalIndex;
	_wires.push_back( newWire );
}

//-------------------------------------------------------------------------------------------------

void DspWireBus::RemoveWire( unsigned long wireIndex )
{
	if( wireIndex > _wires.size() )
	{
		return;
	}
	else
	{
		for( unsigned long j = wireIndex; j < ( _wires.size() - 1 ); j++ )
		{
			_wires[j] = _wires[j + 1];	// shift all other elements up
		}
		_wires.pop_back();	// remove end item
	}
}

//-------------------------------------------------------------------------------------------------

void DspWireBus::RemoveWire( DspSafePointer< DspComponent > linkedComponent,	unsigned long fromSignalIndex, unsigned long toSignalIndex )
{
	for( unsigned long i = 0; i < _wires.size(); i++ )
	{
		if( _wires[i]->linkedComponent == linkedComponent &&
			_wires[i]->fromSignalIndex == fromSignalIndex &&
			_wires[i]->toSignalIndex == toSignalIndex )
		{
			RemoveWire( i );
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------

void DspWireBus::RemoveAllWires()
{
	_wires.clear();
}

//-------------------------------------------------------------------------------------------------

bool DspWireBus::GetWire( unsigned long wireIndex, DspSafePointer< DspWire >& returnWire )
{
	if( wireIndex > _wires.size() )
	{
		return false;
	}
	else
	{
		returnWire = _wires[wireIndex];
		return true;
	}
}

//-------------------------------------------------------------------------------------------------

unsigned long DspWireBus::GetWireCount()
{
	return _wires.size();
}

//=================================================================================================
