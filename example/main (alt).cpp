/************************************************************************
DSPatch - Cross-Platform, Object-Oriented, Flow-Based Programming Library
Copyright (c) 2013 Marcus Tomlinson

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

//=================================================================================================
// This is a simple program that streams an mp3 out of an audio device,
// then overlays a 1KHz oscillator when a key is pressed.

int main()
{
	// 1. Stream MP3
	// =============

	// declare components to be added to the circuit
	DspMp3Decoder mp3Decoder;
	DspAudioDevice audioDevice;
	DspGain gainLeft;
	DspGain gainRight;

	// set circuit thread count to 2
	DSPatch::SetGlobalThreadCount( 2 );

	// start separate thread to tick the components continuously ("auto-tick")
	mp3Decoder.StartAutoTick();
	audioDevice.StartAutoTick();
	gainLeft.StartAutoTick();
	gainRight.StartAutoTick();

	// connect component output signals to respective component input signals
	gainLeft.ConnectInput( mp3Decoder, 0, 0 );		// mp3 left channel into gain1
	gainRight.ConnectInput( mp3Decoder, 1, 0 );		// mp3 right channel into gain2
	audioDevice.ConnectInput( gainLeft, 0, 0 );		// gain1 into audio device left channel
	audioDevice.ConnectInput( gainRight, 0, 1 );	// gain2 into audio device right channel

	// set the gain of components gainLeft and gainRight (mp3 left and right channels)
	gainLeft.SetGain( 0.75 );
	gainRight.SetGain( 0.75 );

	// load an mp3 into the mp3 decoder and start playing the track
	mp3Decoder.LoadFile( "../../Tchaikovski-Swan-Lake-Scene.mp3" );
	mp3Decoder.Play();

	// wait for key press
	getchar();

	// 2. Overlay oscillator
	// =====================

	// A component input pin can only receive one signal at a time so an adders are required to combine the signals

	// declare components to be added to the circuit
	DspOscillator oscillator( 1000.0f, 0.1f );
	DspAdder adder1;
	DspAdder adder2;

	// auto-tick new components
	oscillator.StartAutoTick();
	adder1.StartAutoTick();
	adder2.StartAutoTick();

	// DspMp3Decoder has an output signal named "Sample Rate" that streams the current mp3's sample rate
	// DspOscillator's "Sample Rate" input receives a sample rate value and re-builds its wave table accordingly 
	oscillator.ConnectInput( mp3Decoder, "Sample Rate", "Sample Rate" );	// sample rate sync

	// connect component output signals to respective component input signals
	adder1.ConnectInput( gainLeft, 0, 0 );			// mp3 left channel into adder1 ch0
	adder1.ConnectInput( oscillator, 0, 1 );		// oscillator output into adder1 ch1
	audioDevice.ConnectInput( adder1, 0, 0 );		// adder1 output into audio device left channel

	adder2.ConnectInput( gainRight, 0, 0 );			// mp3 right channel into adder2 ch0
	adder2.ConnectInput( oscillator, 0, 1 );		// oscillator output into adder2 ch1
	audioDevice.ConnectInput( adder2, 0, 1 );		// adder2 output into audio device right channel

	// wait for key press
	getchar();

	// clean up DSPatch
	DSPatch::Finalize();

	return 0;
}

//=================================================================================================
