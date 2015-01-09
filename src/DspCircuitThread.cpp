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

#include "DspCircuitThread.h"
#include "DspComponent.h"

//=================================================================================================

DspCircuitThread::DspCircuitThread( std::vector< DspComponent* >& components, unsigned long threadNo )
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

	if( !_gotSync )												//if haven't already got resume
	{
		_syncCondt.Wait( _resumeMutex );		//wait for resume
	}
	_gotSync = false;											//reset the release flag

	_gotResume = true;
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
