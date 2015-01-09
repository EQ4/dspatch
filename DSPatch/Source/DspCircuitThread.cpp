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

#include "DspCircuitThread.h"
#include "DspComponent.h"

//=================================================================================================

DspCircuitThread::DspCircuitThread( std::vector< DspSafePointer< DspComponent > >& components, unsigned long threadNo )
: _components( components ),
	_threadNo( threadNo )
{
	Start();
}

//-------------------------------------------------------------------------------------------------

DspCircuitThread::~DspCircuitThread()
{
	Stop();
}

//=================================================================================================

void DspCircuitThread::Start( Priority priority )
{
	_stop = false;
	_stopped = false;
	_gotResume = false;
	_gotSync = true;
	DspThread::Start( priority );
}

//-------------------------------------------------------------------------------------------------

void DspCircuitThread::Stop()
{
	_stop = true;

	while( _stopped != true )
	{
		_syncCondt.WakeAll();
		_resumeCondt.WakeAll();
		MsSleep( 1 );
	}
}

//-------------------------------------------------------------------------------------------------

void DspCircuitThread::Sync()
{
	_resumeMutex.Lock();
	if( !_gotSync )												//if haven't already got resume
	{
		_syncCondt.Wait( _resumeMutex );		//wait for resume
	}
	_resumeMutex.Unlock();
}

//-------------------------------------------------------------------------------------------------

void DspCircuitThread::Resume()
{
	_resumeMutex.Lock();
	_gotResume = true;
	_gotSync = false;											//reset the release flag
	_resumeCondt.WakeAll();
	_resumeMutex.Unlock();
}

//=================================================================================================

void DspCircuitThread::_Run()
{
	while( !_stop )
	{
		_resumeMutex.Lock();
		_gotSync = true;
		_syncCondt.WakeAll();
		if( !_gotResume )												//if haven't already got resume
		{
			_resumeCondt.Wait( _resumeMutex );		//wait for resume
		}
		_gotResume = false;											//reset the release flag

		if( !_stop )
		{
			for( unsigned long i = 0; i < _components.size(); i++ )
			{
				_components[i]->ThreadTick( _threadNo );
			}
			for( unsigned long i = 0; i < _components.size(); i++ )
			{
				_components[i]->ThreadReset( _threadNo );
			}
		}

		_resumeMutex.Unlock();
	}

	_stopped = true;
}

//=================================================================================================
