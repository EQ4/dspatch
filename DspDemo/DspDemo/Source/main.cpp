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

#include "DSPatch.h"
#include "DspMp3Decoder.h"
#include "DspGain.h"
#include "DspAudioDevice.h"
#include "DspOscillator.h"
#include "DspAdder.h"

#include <iostream>

#define MP3DECODER "Mp3 Decoder"
#define AUDIODEVICE "Audio Device"
#define GAINCH1 "Gain Channel 1"
#define GAINCH2 "Gain Channel 2"
#define ADDERCH1 "Adder Channel 1"
#define ADDERCH2 "Adder Channel 2"
#define OSCILLATOR "Oscillator"

/*-----------------------------------------------------------------------------------------------------*/
// This is a simple program that streams an mp3 out of an audio device,
// then overlays a 1KHz oscillator when a key is pressed.

void main()
{
	// 1. Stream MP3
	// =============

	DspCircuit circuit;

	DspSafePointer< DspMp3Decoder > mp3Decoder;
	DspSafePointer< DspAudioDevice > audioDevice;
	DspSafePointer< DspGain > gainCh1;
	DspSafePointer< DspGain > gainCh2;

	circuit.StartAutoTick();	// start separate thread to tick the circuit continuously

	mp3Decoder = circuit.AddComponent< DspMp3Decoder >( MP3DECODER );
	audioDevice = circuit.AddComponent< DspAudioDevice >( AUDIODEVICE );
	gainCh1 = circuit.AddComponent< DspGain >( GAINCH1 );
	gainCh2 = circuit.AddComponent< DspGain >( GAINCH2 );

	circuit.ConnectOutToIn( MP3DECODER, "Sample Rate", AUDIODEVICE, "Sample Rate" );	// sample rate sync

	circuit.ConnectOutToIn( MP3DECODER, 0, GAINCH1, 0 );
	circuit.ConnectOutToIn( MP3DECODER, 1, GAINCH2, 0 );
	circuit.ConnectOutToIn( GAINCH1, 0, AUDIODEVICE, 0 );
	circuit.ConnectOutToIn( GAINCH2, 0, AUDIODEVICE, 1 );

	gainCh1->SetGain( 0.75 );
	gainCh2->SetGain( 0.75 );

	mp3Decoder->LoadFile( "../05 Tchaikovski-Swan Lake-Scene.mp3" );
	mp3Decoder->Play();

	getchar();	//wait for key press

	// 2. Overlay oscillator
	// =====================

	// A component input pin can only receive one signal at a time so an adders are required to combine the signals

	DspSafePointer< DspOscillator > oscillator;
	float oscFreq = 1000.0f;
	float oscAmpl = 0.1f;
	oscillator.New( oscFreq, oscAmpl );

	circuit.AddComponent( oscillator, OSCILLATOR );
	circuit.AddComponent< DspAdder >( ADDERCH1 );
	circuit.AddComponent< DspAdder >( ADDERCH2 );

	circuit.ConnectOutToIn( MP3DECODER, "Sample Rate", OSCILLATOR, "Sample Rate" );	// sample rate sync

	circuit.ConnectOutToIn( MP3DECODER, 0, ADDERCH1, 0 );
	circuit.ConnectOutToIn( OSCILLATOR, 0, ADDERCH1, 1 );
	circuit.ConnectOutToIn( ADDERCH1, 0, AUDIODEVICE, 0 );

	circuit.ConnectOutToIn( MP3DECODER, 1, ADDERCH2, 0 );
	circuit.ConnectOutToIn( OSCILLATOR, 0, ADDERCH2, 1 );
	circuit.ConnectOutToIn( ADDERCH2, 0, AUDIODEVICE, 1 );

	getchar();	//wait for key press
}

/*-----------------------------------------------------------------------------------------------------*/
