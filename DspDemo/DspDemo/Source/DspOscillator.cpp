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

#include "DspOscillator.h"
#include <math.h>

static const float PI = 3.1415926535897932384626433832795f;
static const float TWOPI = 6.283185307179586476925286766559f;

//=================================================================================================

DspOscillator::DspOscillator( float startFreq, float startAmpl )
: _freq( startFreq ),
	_ampl( startAmpl ),
	_lastPos( 0 ),
	_lookupLength( 0 ),
	_bufferSize( 128 ),
	_sampleRate( 44100 )
{
	_BuildLookup();

	AddInput_( "Buffer Size Sync" );
	AddInput_( "Sample Rate" );

	AddOutput_();
}

//-------------------------------------------------------------------------------------------------

DspOscillator::~DspOscillator() {}

//=================================================================================================

void DspOscillator::SetBufferSize( unsigned long bufferSize )
{
	_bufferSize = bufferSize;

	_processMutex.Lock();
	_BuildLookup();
	_processMutex.Unlock();
}

//-------------------------------------------------------------------------------------------------

void DspOscillator::SetSampleRate( unsigned long sampleRate )
{
	_sampleRate = sampleRate;

	_processMutex.Lock();
	_BuildLookup();
	_processMutex.Unlock();
}

//-------------------------------------------------------------------------------------------------

void DspOscillator::SetAmpl( float ampl )
{
	_ampl = ampl;

	_processMutex.Lock();
	_BuildLookup();
	_processMutex.Unlock();
}

//-------------------------------------------------------------------------------------------------

void DspOscillator::SetFreq( float freq )
{
	_freq = freq;

	_processMutex.Lock();
	_BuildLookup();
	_processMutex.Unlock();
}

//=================================================================================================

void DspOscillator::Process_( DspSignalBus& inputs, DspSignalBus& outputs )
{
	// Synchronise sample rate with the "Sample Rate" input feed
	// =========================================================
	unsigned long sampleRate;
	if( inputs.GetValue( "Sample Rate", sampleRate ) )
	{
		if( sampleRate != _sampleRate )
		{
			_sampleRate = sampleRate;
			_BuildLookup();
		}
	}

	// Synchronise buffer size with the size of incoming buffers
	// =========================================================
	if( inputs.GetValue( 0, _signal ) )
	{
		if( _bufferSize != _signal.size() )
		{
			_bufferSize = _signal.size();
			_BuildLookup();
		}
	}

	_processMutex.Lock();

	if( _signalLookup.size() != 0 )
	{
		for( unsigned long i = 0; i < _signal.size(); i ++ )
		{
			if( _lastPos >= _lookupLength )
				_lastPos = 0;
			_signal[i] = _signalLookup[_lastPos++];
		}

		outputs.SetValue( 0, _signal );
	}

	_processMutex.Unlock();
}

//=================================================================================================

void DspOscillator::_BuildLookup()
{
	float posFrac = ( float ) _lastPos / ( float ) _lookupLength;
	float angleInc = TWOPI * _freq / _sampleRate;

	_lookupLength = ( unsigned long ) ( ( float ) _sampleRate / _freq );

	_signal.resize( _bufferSize );
	_signalLookup.resize( _lookupLength );

	for( unsigned long i = 0; i < _lookupLength; i++ )
	{
		_signalLookup[i] = sin( angleInc * i ) * _ampl;
	}

	_lastPos = ( unsigned long ) ( posFrac * ( float ) _lookupLength + 0.5f );	//calculate new position (round up)
}

//=================================================================================================
