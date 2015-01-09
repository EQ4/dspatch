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

#include "DspComponentThread.h"
#include "DspComponent.h"

//=================================================================================================

DspComponentThread::DspComponentThread( DspComponent& component )
: _component( component )
{
	Start();
}

//-------------------------------------------------------------------------------------------------

DspComponentThread::~DspComponentThread()
{
	Stop();
}

//=================================================================================================

void DspComponentThread::Start( Priority priority )
{
	_stop = false;
	_stopped = false;
	_pause = false;
	DspThread::Start( priority );
}

//-------------------------------------------------------------------------------------------------

void DspComponentThread::Stop()
{
	_stop = true;

	while( _stopped != true )
	{
		_pauseCondt.WakeAll();
		_resumeCondt.WakeAll();
		MsSleep( 1 );
	}
}

//-------------------------------------------------------------------------------------------------

void DspComponentThread::Pause()
{
	_pauseMutex.Lock();

	_pause = true;
	_pauseCondt.Wait( _pauseMutex );		//wait for resume
	_pause = false;

	_pauseMutex.Unlock();
}

//-------------------------------------------------------------------------------------------------

void DspComponentThread::Resume()
{
	_resumeMutex.Lock();
	_resumeCondt.WakeAll();
	_resumeMutex.Unlock();
}

//=================================================================================================

void DspComponentThread::_Run()
{
	while( !_stop )
	{
		_component.Tick();
		_component.Reset();

		if( _pause )
		{
			_resumeMutex.Lock();

			_pauseMutex.Lock();
			_pauseCondt.WakeAll();
			_pauseMutex.Unlock();

			_resumeCondt.Wait( _resumeMutex );		//wait for resume

			_resumeMutex.Unlock();
		}
	}

	_stopped = true;
}

//=================================================================================================
