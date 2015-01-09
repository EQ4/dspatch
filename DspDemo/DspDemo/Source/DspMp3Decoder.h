/************************************************************************
DSPatch - Cross-Platform, Object-Oriented, Flow-Based Programming Library
Copyright (c) 2012 Marcus Tomlinson

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

/*
copyright 1995-2010 by the mpg123 project - free software under the terms of the LGPL 2.1
see COPYING and AUTHORS files at http://mpg123.org
initially written by Michael Hipp
*/

#ifndef DSPMP3DECODER_H
#define DSPMP3DECODER_H

#include "DSPatch.h"

#ifndef DSP_UNIX
	#ifndef __ssize_t_defined
		typedef unsigned long ssize_t;
		#define __ssize_t_defined
	#endif
#endif

struct mpg123_handle_struct;
typedef struct mpg123_handle_struct mpg123_handle;

//-------------------------------------------------------------------------------------------------

class DspMp3Decoder : public DspComponent
{
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

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs );

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
};

//-------------------------------------------------------------------------------------------------

#endif // DSPMP3DECODER_H
