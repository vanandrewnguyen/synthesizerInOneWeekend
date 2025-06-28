#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "utils.h"

struct Envelope {
	virtual double getAmp(const double aTime, const double aTimeOn, const double aTimeOff) = 0;
};

struct EnvelopeADSR : public Envelope {
	double mAttackTime;
	double mDecayTime;
	double mReleaseTime;

	double mSustainAmp;
	double mStartAmp;

	EnvelopeADSR() {
		// 10 ms = 0.01
		mAttackTime = 0.01; 
		mDecayTime = 0.01;
		mStartAmp = 1.0;
		mSustainAmp = 1.0;
		mReleaseTime = 0.2;
	}

	double getAmp(const double aTime, const double aTimeOn, const double aTimeOff) {
		double amp = 0.0;
		double releaseAmp = 0.0;

		if (aTimeOn > aTimeOff) {
			// Note is on.
			// ADS
			// Attack
			double lifeTime = aTime - aTimeOn;
			if (lifeTime <= mAttackTime) {
				amp = (lifeTime / mAttackTime) * mStartAmp;
			}
			// Decay
			if (lifeTime > mAttackTime && lifeTime <= (mAttackTime + mDecayTime)) {
				// Index of 0..1 of percentage of how far into decay period
				amp = ((lifeTime - mAttackTime) / mDecayTime) * (mSustainAmp - mStartAmp) + mStartAmp;
			}
			// Sustain
			if (lifeTime > (mAttackTime + mDecayTime)) {
				amp = mSustainAmp;
			}
		} else {
			double lifeTime = aTime - aTimeOn;
			// R
			if (lifeTime <= mAttackTime) {
				releaseAmp = (lifeTime / mAttackTime) * mStartAmp;
			}
			if (lifeTime > mAttackTime && lifeTime <= (mAttackTime + mDecayTime)) {
				releaseAmp = ((lifeTime - mAttackTime) / mDecayTime) * (mSustainAmp - mStartAmp) + mStartAmp;
			}
			if (lifeTime > (mAttackTime + mDecayTime)) {
				releaseAmp = mSustainAmp;
			}
			amp = ((aTime - aTimeOff) / mReleaseTime) * (0.0 - releaseAmp) + releaseAmp;
		}

		// Epis val check
		if (amp <= Utility::espilon) {
			amp = 0.0;
		}

		// Don't blow out the speakers
		amp = std::min(amp, 1.0);

		return amp;
	}
};

#endif