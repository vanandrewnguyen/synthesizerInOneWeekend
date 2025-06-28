#ifndef SYNTHENGINE_H
#define SYNTHENGINE_H

#include <algorithm>
#include <vector>
#include <string>

#include "utils.h"
#include "noiseMaker.h"

struct EnvelopeADSR {
	double m_attackTime;
	double m_decayTime;
	double m_releaseTime;

	double m_sustainAmp;
	double m_startAmp;

	double m_triggerOnTime;
	double m_triggerOffTime;

	bool m_noteOn;

	EnvelopeADSR() {
		m_attackTime = 0.05; // 50 ms
		m_decayTime = 0.01;
		m_startAmp = 0.25;
		m_sustainAmp = 0.2;
		m_releaseTime = 0.1;
		m_triggerOnTime = 0.0;
		m_triggerOffTime = 0.0;
		m_noteOn = false;
	}

	double getAmp(double a_time) {
		double amp = 0.0;
		double lifeTime = a_time - m_triggerOnTime;

		if (m_noteOn) {
			// ADS
			// Attack
			if (lifeTime <= m_attackTime) {
				amp = (lifeTime / m_attackTime) * m_startAmp;
			}
			// Decay
			if (lifeTime > m_attackTime && lifeTime <= (m_attackTime + m_decayTime)) {
				// Index of 0..1 of percentage of how far into decay period
				amp = ((lifeTime - m_attackTime) / m_decayTime) * (m_sustainAmp - m_startAmp) + m_startAmp;
			}
			// Sustain
			if (lifeTime > (m_attackTime + m_decayTime)) {
				amp = m_sustainAmp;
			}
		}
		else {
			// R
			amp = ((a_time - m_triggerOffTime) / m_releaseTime) * (0.0 - m_sustainAmp) + m_sustainAmp;
		}

		// Epis val check
		if (amp <= Utility::espilon) {
			amp = 0.0;
		}
		
		// Don't blow out the speakers
		amp = std::min(amp, 1.0);

		return amp;
	}

	void noteOn(double a_timeOn) {
		m_triggerOnTime = a_timeOn;
		m_noteOn = true;
	}

	void noteOff(double a_timeOff) {
		m_triggerOffTime = a_timeOff;
		m_noteOn = false;
	}
};

class SynthEngine {
private:
	std::vector<std::wstring> m_devices;
	NoiseMaker<short> m_sound;

	double m_octaveBaseFreq;
	double m_root;
	std::atomic<double> m_frequency;
	EnvelopeADSR m_envelope;
	int m_currentKey;

public:
	SynthEngine();

	double osc(double a_hertz, double a_time, int a_type);
	 double makeNoise(double a_time);
};

SynthEngine::SynthEngine()
	: m_devices(NoiseMaker<short>::Enumerate()),
	  m_sound(m_devices[0]) {

	m_octaveBaseFreq = 110.0;
	m_root = std::pow(2.0, 1.0 / 12.0);
	m_frequency = 440.0;
	m_currentKey = -1;

	std::cout << "Starting engine..." << std::endl;
	
	for (std::wstring d : m_devices) {
		std::wcout << "Found Output Device: " << d << std::endl;
	}
	std::wcout << endl <<
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |"   << endl <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |"   << endl <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__" << endl <<
		"|     |     |     |     |     |     |     |     |     |     |" << endl <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |" << endl <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << endl << endl;

	//m_sound.SetUserFunction(makeNoise);
	m_sound.SetUserFunction([this](double a_time) {
		return makeNoise(a_time);
	});

	// Create keyboard piano of 2 octaves
	bool keyPressed = false;
	while (1) {
		keyPressed = false;
		for (int i = 0; i < 16; i++) {
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[i])) & 0x8000) {
				if (m_currentKey != i) {
					m_frequency = m_octaveBaseFreq * std::pow(m_root, i);
					m_envelope.noteOn(m_sound.GetTime());
					m_currentKey = i;

					std::wcout << "\rNote On : " << m_sound.GetTime() << "s " << m_frequency << "Hz";
				}
				
				keyPressed = true;
			}
		}

		if (!keyPressed) {
			if (m_currentKey != -1) {
				m_envelope.noteOff(m_sound.GetTime());
				m_currentKey = -1;

				wcout << "\rNote Off: " << m_sound.GetTime() << "s";
			}
		}
	}
}

double SynthEngine::osc(double a_hertz, double a_time, int a_type) {
	// Avoid large amplitudes with low frequencies
	switch (a_type) {
	case 0:
		// Sine
		return sin(Utility::freqToVel(m_frequency) * a_time);
	case 1:
		// Square
		return (sin(Utility::freqToVel(m_frequency) * a_time)) > 0.0 ? 1.0 : -1.0;
	case 2:
		// Triangle
		return asin(sin(Utility::freqToVel(m_frequency) * a_time)) * 2.0 / Utility::pi;
	case 3: {
		// Saw (using sum of sine lim inf, soft)
		double output = 0.0;
		for (double i = 1.0; i < 100.0; i++) {
			output += (sin(i * Utility::freqToVel(a_hertz) * a_time)) / i;
		}
		return output * (2.0 / Utility::pi);
	}
	case 4:
		// Saw (using mod)
		return (2.0 / Utility::pi) * (a_hertz * Utility::pi * std::fmod(a_time, 1.0 / a_hertz) - (Utility::pi / 2.0));
	case 5:
		// Rand Noise
		return Utility::randomDouble(); // freq does not affect... so plays constantly
	default:
		return 0.0;
	}
}

double SynthEngine::makeNoise(double a_time) {
	int index = 3;
	double output = osc(m_frequency, a_time, index) * m_envelope.getAmp(a_time);
	double threshold = 1.0;
	return std::max(std::min(output, threshold), -threshold);
}

#endif
