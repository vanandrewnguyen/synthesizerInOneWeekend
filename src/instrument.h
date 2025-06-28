#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "envelope.h"

struct Note {
	int mId;
	double mTimeOn;
	double mTimeOff;
	bool mActive;
	int mChannel;

	Note() {
		mId = 0;
		mTimeOn = 0.0;
		mTimeOff = 0.0;
		mActive = false;
		mChannel = 0;
	}
};

typedef bool(*lambda)(Note const& item);
template<class T>
void safe_remove(T& v, lambda f) {
	auto n = v.begin();
	while (n != v.end())
		if (!f(*n))
			n = v.erase(n);
		else
			++n;
}

struct Instrument {
	double mVolume;
	EnvelopeADSR mEnvelope;

	virtual double sound(double aTime, Note aNote, bool& aNoteFinished) = 0;
};

struct BellInstrument : public Instrument {
	BellInstrument() {
		mEnvelope.mAttackTime = 0.01;
		mEnvelope.mDecayTime = 1.0;
		mEnvelope.mSustainAmp = 0.0;
		mEnvelope.mReleaseTime = 1.0;
		mVolume = 1.0;
	}

	virtual double sound(double aTime, Note aNote, bool& aNoteFinished) {
		double amp = mEnvelope.getAmp(aTime, aNote.mTimeOn, aNote.mTimeOff);
		if (amp <= 0.0) {
			aNoteFinished = true;
		}
		
		double output = (
			+ 1.00 * Synth::osc(aNote.mTimeOn - aTime, Utility::scale(aNote.mId + 12), Synth::OSC_SINE, 5.0, 0.001)
			+ 0.50 * Synth::osc(aNote.mTimeOn - aTime, Utility::scale(aNote.mId + 24))
			+ 0.25 * Synth::osc(aNote.mTimeOn - aTime, Utility::scale(aNote.mId + 36))
		);
		 
		return amp * output * mVolume;
	}
};

struct HarmonicaInstrument : public Instrument {
	HarmonicaInstrument() {
		mEnvelope.mAttackTime = 0.05;
		mEnvelope.mDecayTime = 1.0;
		mEnvelope.mSustainAmp = 0.95;
		mEnvelope.mReleaseTime = 0.1;
		mVolume = 1.0;
	}

	virtual double sound(double aTime, Note aNote, bool& aNoteFinished) {
		double amp = mEnvelope.getAmp(aTime, aNote.mTimeOn, aNote.mTimeOff);
		if (amp <= 0.0) {
			aNoteFinished = true;
		}

		double output = (
			+ 1.00 * Synth::osc(aNote.mTimeOn - aTime, Utility::scale(aNote.mId), Synth::OSC_SQUARE, 5.0, 0.001)
			+ 0.50 * Synth::osc(aNote.mTimeOn - aTime, Utility::scale(aNote.mId + 12), Synth::OSC_SQUARE)
			+ 0.05 * Synth::osc(aNote.mTimeOn - aTime, Utility::scale(aNote.mId + 24), Synth::OSC_SQUARE)
		);

		return amp * output * mVolume;
	}
};

#endif