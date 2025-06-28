#ifndef SYNTHENGINE_H
#define SYNTHENGINE_H

#include <algorithm>
#include <vector>
#include <string>

#include "utils.h"
#include "envelope.h"
#include "instrument.h"
#include "noiseMaker.h"

class SynthEngine {
private:
	std::vector<std::wstring> mDevices;
	NoiseMaker<short> mSound;

	double mOctaveBaseFreq;
	double mRoot;
	std::atomic<double> mFrequency;
	EnvelopeADSR mEnvelope;
	std::vector<Note> mNotes;
	std::mutex mMutexNotes;

	BellInstrument mInstBell;
	HarmonicaInstrument mInstHarm;

public:
	SynthEngine();

	double makeNoise(int aChannel, double aTime);
};

SynthEngine::SynthEngine()
	: mDevices(NoiseMaker<short>::Enumerate()),
	  mSound(mDevices[0]) {

	// Frequency of octave represented by keyboard, e.g. A2
	mOctaveBaseFreq = 220.0;
	mRoot = std::pow(2.0, 1.0 / 12.0);
	// Frequency output of instrument
	mFrequency = 440.0;

	std::cout << "Starting engine..." << std::endl;
	
	for (std::wstring d : mDevices) {
		std::wcout << "Found Output Device: " << d << std::endl;
	}
	std::wcout << endl <<
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |"   << endl <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |"   << endl <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__" << endl <<
		"|     |     |     |     |     |     |     |     |     |     |" << endl <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |" << endl <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << endl << endl;

	mSound.SetUserFunction([this](int aChannel, double aTime) {
		return makeNoise(aChannel, aTime);
	});

	// Create keyboard piano of 2 octaves
	bool keyPressed = false;
	while (1) {
		keyPressed = false;
		for (int i = 0; i < 16; i++) {
			short nKeyState = GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[i]));
			double currTime = mSound.GetTime();

			// Check if note already exists in currently playing notes
			mMutexNotes.lock();
			auto noteFound = std::find_if(mNotes.begin(), mNotes.end(), [&i](Note const& item) { return item.mId == i; });
			if (noteFound == mNotes.end()) {
				// Note not found in vector
				if (nKeyState & 0x8000)	{
					// Key has been pressed so create a new note
					Note n;
					n.mId = i;
					n.mTimeOn = currTime;
					n.mChannel = 1;
					n.mActive = true;
					mNotes.emplace_back(n);
				} else {
					// Note not in vector, but key has been released, do nothing
				}
			} else {
				// Note exists in vector
				if (nKeyState & 0x8000)	{
					// Key is still held, so do nothing
					if (noteFound->mTimeOff > noteFound->mTimeOn)	{
						// Key has been pressed again during release phase
						noteFound->mTimeOn = currTime;
						noteFound->mActive = true;
					}
				} else {
					// Key has been released, so switch off
					if (noteFound->mTimeOff < noteFound->mTimeOn) {
						noteFound->mTimeOff = currTime;
					}
				}
			}
			mMutexNotes.unlock();
		}
	std::wcout << "\rNotes: " << mNotes.size() << "    ";
	}
}

double SynthEngine::makeNoise(int aChannel, double aTime) {
	std::unique_lock<mutex> lenM(mMutexNotes);
	double mixedOutput = 0.0;

	for (auto& note : mNotes) {
		bool isNoteFinished = false;
		double currSound = 0;
		/*if (note.mChannel == 2) {
			currSound = mInstBell.sound(aTime, note, isNoteFinished);
		}
		if (note.mChannel == 1) {
			currSound = mInstHarm.sound(aTime, note, isNoteFinished) * 0.5;
		}*/
		currSound = mInstHarm.sound(aTime, note, isNoteFinished);
		mixedOutput += currSound;

		if (isNoteFinished && note.mTimeOff > note.mTimeOn) {
			note.mActive = false;
		}
	}

	safe_remove<std::vector<Note>>(mNotes, [](Note const& item) { return item.mActive; });

	double threshold = 1.0;
	return std::max(std::min(mixedOutput, threshold), -threshold) * 0.02;
}



#endif
