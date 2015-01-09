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

/*=========================================================================
PortAudio Portable Real-Time Audio Library
Copyright (c) 1999-2011 Ross Bencina and Phil Burk

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
USE OR OTHER DEALINGS IN THE SOFTWARE.
=========================================================================*/

#ifndef DSPAUDIODEVICE_H
#define DSPAUDIODEVICE_H

#include "DSPatch.h"

typedef void PaStream;
typedef unsigned long PaStreamCallbackFlags;

struct PaDeviceInfo;
struct PaStreamParameters;
struct PaStreamCallbackTimeInfo;

//-------------------------------------------------------------------------------------------------

class DspAudioDevice : public DspComponent
{
private:
	std::vector< std::vector< float > > _outputChannels;
	std::vector< std::vector< float > > _inputChannels;

	unsigned long _sampleRate;
	unsigned long _bufferSize;

	short _deviceCount;
	PaDeviceInfo* _deviceList;

	short _inputDevice;
	short _outputDevice;

	bool _prioritySet;

	PaStream* _audioStream;
	PaStreamParameters* _outputParams;
	PaStreamParameters* _inputParams;

	DspMutex _buffersMutex, _syncMutex;
	DspWaitCondition _waitCondt, _syncCondt;
	bool _gotWaitReady, _gotSyncReady;

	bool _streamStop, _isStreaming;

	short _currentBufferIndex;
	short _currentDevice;

	void _WaitForBuffer();
	void _SyncBuffer();

	short _StopStream();
	short _StartStream();

	int _DynamicCallback( const void* inputBuffer, void* outputBuffer );

	static int _StaticCallback( const void* inputBuffer, void* outputBuffer,
															unsigned long framesPerBuffer,
															const PaStreamCallbackTimeInfo* timeInfo,
															PaStreamCallbackFlags statusFlags,
															void* userData );

	static void _CallbackComplete( void* userData );

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs );

public:
	enum ApiType
	{
		Unknown=0,
		DirectSound=1,
		MME=2,
		ASIO=3,
		SoundManager=4,
		CoreAudio=5,
		OSS=7,
		ALSA=8,
		AL=9,
		BeOS=10,
		WDMKS=11,
		JACK=12,
		WASAPI=13,
		AudioScienceHPI=14
	};

	DspAudioDevice();
	~DspAudioDevice();

	short SetSampleRate( unsigned long sampleRate );

	short SetDevice( short deviceIndex );

	const char* GetDeviceName( short deviceIndex );
	const char* GetDeviceApiName( short deviceIndex );
	ApiType GetDeviceApiType( short deviceIndex );
	long GetDeviceInputCount( short deviceIndex );
	long GetDeviceOutputCount( short deviceIndex );

	short GetCurrentDevice();
	short GetDeviceCount();
	bool IsStreaming();
	unsigned long GetSampleRate();
};

//-------------------------------------------------------------------------------------------------

#endif // DSPAUDIODEVICE_H
