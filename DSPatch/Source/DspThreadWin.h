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

#ifndef DSPTHREADWIN_H
#define DSPTHREADWIN_H

//-------------------------------------------------------------------------------------------------

#include <windows.h>

//=================================================================================================

class DspThread
{
private:

	static DWORD WINAPI _ThreadFunc( LPVOID pv )
	{
		( reinterpret_cast<DspThread*>( pv ) )->_Run();
		return 0;
	}

	virtual void _Run() = 0;

	HANDLE _threadHandle;

public:
	DspThread()
	: _threadHandle( NULL ) {}

	DspThread( const DspThread& )
	: _threadHandle( NULL ) {}

	virtual ~DspThread()
	{
		CloseHandle( _threadHandle );
	}

	enum Priority
	{
		IdlePriority = -15,

		LowestPriority = -2,
		LowPriority = -1,
		NormalPriority = 0,
		HighPriority = 1,
		HighestPriority = 2,

		TimeCriticalPriority = 15,
	};

	virtual void Start( Priority priority = NormalPriority )
	{
		DWORD threadId;
		_threadHandle = CreateThread( NULL, 0, _ThreadFunc, this, CREATE_SUSPENDED, &threadId );
		SetThreadPriority( _threadHandle, priority );
		ResumeThread( _threadHandle );
	}

	static void SetPriority( Priority priority )
	{
		SetThreadPriority( GetCurrentThread(), priority );
	}

	static void MsSleep( unsigned long milliseconds )
	{
		Sleep( milliseconds );
	}
};

//=================================================================================================

class DspMutex
{
private:

	CRITICAL_SECTION _cs;

public:

	DspMutex()
	{
		InitializeCriticalSection( &_cs );
	}

	DspMutex( const DspMutex& )
	{
		InitializeCriticalSection( &_cs );
	}

	virtual ~DspMutex()
	{
		DeleteCriticalSection( &_cs );
	}

	void Lock()
	{
		EnterCriticalSection( &_cs );
	}

	void Unlock()
	{
		LeaveCriticalSection( &_cs );
	}
};

//=================================================================================================

class DspWaitCondition
{
private:

	HANDLE _hEvent;

public:

	DspWaitCondition()
	{
		_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	}

	DspWaitCondition( const DspWaitCondition& )
	{
		_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	}

	virtual ~DspWaitCondition()
	{
		CloseHandle( _hEvent );
	}

	void Wait( DspMutex& mutex )
	{
		ResetEvent( _hEvent );

		mutex.Unlock();

		WaitForSingleObject( _hEvent, INFINITE );

		mutex.Lock();
	}

	void WakeAll()
	{
		SetEvent( _hEvent );
	}
};

//=================================================================================================

#endif // DSPTHREADWIN_H
