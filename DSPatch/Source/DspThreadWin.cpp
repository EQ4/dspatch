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

#include "DspThreadWin.h"

//=================================================================================================

DspThread::DspThread()
: _threadHandle( NULL ) {}

//-------------------------------------------------------------------------------------------------

DspThread::DspThread( const DspThread& )
: _threadHandle( NULL ) {}

//-------------------------------------------------------------------------------------------------

DspThread::~DspThread()
{
	CloseHandle( _threadHandle );
}

//-------------------------------------------------------------------------------------------------

DWORD WINAPI DspThread::_ThreadFunc( LPVOID pv )
{
	( reinterpret_cast<DspThread*>( pv ) )->_Run();
	return 0;
}

//-------------------------------------------------------------------------------------------------

void DspThread::MsSleep( unsigned long milliseconds )
{
	Sleep( milliseconds );
}

//-------------------------------------------------------------------------------------------------

void DspThread::Start( Priority priority )
{
	DWORD threadId;
	_threadHandle = CreateThread( NULL, 0, _ThreadFunc, this, CREATE_SUSPENDED, &threadId );
	SetThreadPriority( _threadHandle, priority );
	ResumeThread( _threadHandle );
}

//-------------------------------------------------------------------------------------------------

void DspThread::SetPriority( Priority priority )
{
	SetThreadPriority( GetCurrentThread(), priority );
}

//=================================================================================================

DspMutex::DspMutex()
{
	InitializeCriticalSection( &_cs );
}

//-------------------------------------------------------------------------------------------------

DspMutex::DspMutex( const DspMutex& )
{
	InitializeCriticalSection( &_cs );
}

//-------------------------------------------------------------------------------------------------

DspMutex::~DspMutex()
{
	DeleteCriticalSection( &_cs );
}

//-------------------------------------------------------------------------------------------------

void DspMutex::Lock()
{
	EnterCriticalSection( &_cs );
}

//-------------------------------------------------------------------------------------------------

void DspMutex::Unlock()
{
	LeaveCriticalSection( &_cs );
}

//=================================================================================================

DspWaitCondition::DspWaitCondition()
{
	_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
}

//-------------------------------------------------------------------------------------------------

DspWaitCondition::DspWaitCondition( const DspWaitCondition& )
{
	_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
}

//-------------------------------------------------------------------------------------------------

DspWaitCondition::~DspWaitCondition()
{
	CloseHandle( _hEvent );
}

//-------------------------------------------------------------------------------------------------

void DspWaitCondition::Wait( DspMutex& mutex )
{
	ResetEvent( _hEvent );

	mutex.Unlock();

	WaitForSingleObject( _hEvent, INFINITE );

	mutex.Lock();
}

//-------------------------------------------------------------------------------------------------

void DspWaitCondition::WakeAll()
{
	SetEvent( _hEvent );
}

//=================================================================================================
