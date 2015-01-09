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

#ifndef DSPTHREADUNIX_H
#define DSPTHREADUNIX_H

//-------------------------------------------------------------------------------------------------

#include <pthread.h>
#include <unistd.h>

//=================================================================================================

class DspThread
{
public:
	DspThread() {}
    
	virtual ~DspThread()
    {
        pthread_detach( _thread );
    }

	enum Priority
	{
		IdlePriority,

		LowestPriority,
		LowPriority,
		NormalPriority,
		HighPriority,
		HighestPriority,

		TimeCriticalPriority,
	};

	virtual void Start( Priority priority = NormalPriority )
    {
        pthread_create( &_thread, NULL, _ThreadFunc, this );
        
        _SetPriority( _thread, priority );        
    }

	static void SetPriority( Priority priority )
    {        
        _SetPriority( pthread_self(), priority );
    }

	static void MsSleep( unsigned long milliseconds )
    {
        //usleep( ( unsigned int )milliseconds );
    }
    
private:
	static void* _ThreadFunc( void *pv )
	{
		( reinterpret_cast<DspThread*>( pv ) )->_Run();
		return NULL;
	}
    
    virtual void _Run() = 0;
    
    static void _SetPriority( pthread_t threadID, Priority priority )
    {
        int retcode;
        int policy;
        struct sched_param param;
        
        retcode = pthread_getschedparam( threadID, &policy, &param );
        
        policy = SCHED_FIFO;
        param.sched_priority = ( ( priority - IdlePriority ) * ( 99 - 1 ) / TimeCriticalPriority ) + 1;
        
        retcode = pthread_setschedparam( threadID, policy, &param );
    }
    
    pthread_t _thread;
    
};

//=================================================================================================

class DspMutex
{
friend class DspWaitCondition;

private:
    pthread_mutex_t _mutex;

public:

	DspMutex()
    {
        pthread_mutex_init( &_mutex, NULL );
    }

	virtual ~DspMutex()
    {
        pthread_mutex_destroy( &_mutex );
    }

	void Lock()
    {
        pthread_mutex_lock( &_mutex );
    }

	void Unlock()
    {
        pthread_mutex_unlock( &_mutex );
    }
 };

//=================================================================================================

class DspWaitCondition
{
private:
    pthread_cond_t _cond;

public:

	DspWaitCondition()
    {
        pthread_cond_init( &_cond, NULL );
    }

	virtual ~DspWaitCondition()
    {
        pthread_cond_destroy( &_cond );
    }

	void Wait( DspMutex& mutex )
    {
        pthread_cond_wait( &_cond, &(mutex._mutex) );
    }

	void WakeAll()
    {
        pthread_cond_broadcast( &_cond );
    }
};

//=================================================================================================

#endif // DSPTHREADUNIX_H
