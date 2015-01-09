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

#ifndef DSPMP3DECODER_H
#define DSPMP3DECODER_H

#include "DSPatch.h"

#ifndef __ssize_t_defined
typedef unsigned long ssize_t;
#define __ssize_t_defined
#endif

struct mpg123_handle_struct;
typedef struct mpg123_handle_struct mpg123_handle;

//-------------------------------------------------------------------------------------------------

class DspMp3Decoder : public DspComponent
{
private:
	std::vector< float > _leftChannel;
	std::vector< float > _rightChannel;

	char* _filePath;
	unsigned long _fileSize;
	float* _fileSig;
	int _channelCount;

	mpg123_handle* _mpgHandle;

	long _sampleRate;
	unsigned long _sampleIndex;
	DspMutex _busyMutex;

	unsigned long _outSize;
	unsigned long _outMemSize;

	unsigned long _bufferSize;

	bool _isPlaying;
	void _ClearFile();

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs );

public:
	DspMp3Decoder();
	~DspMp3Decoder();

	void SetBufferSize( unsigned long bufferSize );

	bool LoadFile( const char* filePath );
	short Play();
	short Pause();
	short Stop();
	short Seek( long sampleIndex );

	unsigned long GetSampleIndex();
	long GetSampleRate();
	unsigned long GetFileSize();
	const char* GetFilePath();
	bool IsPlaying();
};

//-------------------------------------------------------------------------------------------------

#endif // DSPMP3DECODER_H
