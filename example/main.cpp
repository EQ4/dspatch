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

#include "DSPatch.h"

#include "DspMp3Decoder.h"
#include "DspGain.h"
#include "DspAudioDevice.h"
#include "DspOscillator.h"
#include "DspAdder.h"

#include <iostream>

// The following unique ID strings are used to identify components within the circuit

#define MP3DECODER "Mp3 Decoder"
#define AUDIODEVICE "Audio Device"
#define GAIN1 "Gain Channel 1"
#define GAIN2 "Gain Channel 2"
#define ADDER1 "Adder Channel 1"
#define ADDER2 "Adder Channel 2"
#define OSCILLATOR "Oscillator"

//=================================================================================================
// This is a simple program that streams an mp3 out of an audio device,
// then overlays a 1KHz oscillator when a key is pressed.

int main()
{
	// 1. Stream MP3
	// =============

	// create a circuit 
	DspCircuit circuit;

	// declare pointers for components to be added to the circuit
	DspMp3Decoder* mp3Decoder;
	DspAudioDevice* audioDevice;
	DspGain* gainLeft;
	DspGain* gainRight;

	// set circuit thread count to 2 then start separate thread to tick the circuit continuously
	circuit.SetThreadCount( 2 );
	circuit.StartAutoTick();

	// add new components to the circuit (these methods return pointers to the new components)
	mp3Decoder = circuit.AddComponent< DspMp3Decoder >( MP3DECODER );
	audioDevice = circuit.AddComponent< DspAudioDevice >( AUDIODEVICE );
	gainLeft = circuit.AddComponent< DspGain >( GAIN1 );
	gainRight = circuit.AddComponent< DspGain >( GAIN2 );

	// connect component output signals to respective component input signals
	circuit.ConnectOutToIn( MP3DECODER, 0, GAIN1, 0 );	// mp3 left channel into gain1
	circuit.ConnectOutToIn( MP3DECODER, 1, GAIN2, 0 );	// mp3 right channel into gain2
	circuit.ConnectOutToIn( GAIN1, 0, AUDIODEVICE, 0 );	// gain1 into audio device left channel
	circuit.ConnectOutToIn( GAIN2, 0, AUDIODEVICE, 1 );	// gain2 into audio device right channel

	// set the gain of components gainLeft and gainRight (mp3 left and right channels)
	gainLeft->SetGain( 0.75 );
	gainRight->SetGain( 0.75 );

	// load an mp3 into the mp3 decoder and start playing the track
	mp3Decoder->LoadFile( "../../Tchaikovski-Swan-Lake-Scene.mp3" );
	mp3Decoder->Play();

	//wait for key press
	getchar();

	// 2. Overlay oscillator
	// =====================

	// declare pointer for a new oscillator component
	DspOscillator* oscillator;

	// New() the oscillator pointer with the desired constructor parametres
	float oscFreq = 1000.0f;						// 1Khz frequency
	float oscAmpl = 0.1f;								// 10% amplitude
	oscillator = new DspOscillator( oscFreq, oscAmpl );

	// A component input pin can only receive one signal at a time so an adders are required to combine the signals
	// add new components to the circuit
	circuit.AddComponent( oscillator, OSCILLATOR );	// add pre-constructed oscillator component
	circuit.AddComponent< DspAdder >( ADDER1 );
	circuit.AddComponent< DspAdder >( ADDER2 );

	// DspMp3Decoder has an output signal named "Sample Rate" that streams the current mp3's sample rate
	// DspOscillator's "Sample Rate" input receives a sample rate value and re-builds its wave table accordingly 
	circuit.ConnectOutToIn( MP3DECODER, "Sample Rate", OSCILLATOR, "Sample Rate" );	// sample rate sync

	// connect component output signals to respective component input signals
	circuit.ConnectOutToIn( MP3DECODER, 0, ADDER1, 0 );		// mp3 left channel into adder1 ch0
	circuit.ConnectOutToIn( OSCILLATOR, 0, ADDER1, 1 );		// oscillator output into adder1 ch1
	circuit.ConnectOutToIn( ADDER1, 0, AUDIODEVICE, 0 );	// adder1 output into audio device left channel

	circuit.ConnectOutToIn( MP3DECODER, 1, ADDER2, 0 );		// mp3 right channel into adder2 ch0
	circuit.ConnectOutToIn( OSCILLATOR, 0, ADDER2, 1 );		// oscillator output into adder2 ch1
	circuit.ConnectOutToIn( ADDER2, 0, AUDIODEVICE, 1 );	// adder2 output into audio device right channel

	//wait for key press
	getchar();

	return 0;
}

//=================================================================================================
