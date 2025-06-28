#ifndef SYNTHENGINE_H
#define SYNTHENGINE_H

#include <vector>
#include <string>

#include "utils.h"
#include "noiseMaker.h"

namespace noiseSpace {
	double octaveBaseFreq = 110.0;
	double root = std::pow(2.0, 1.0 / 12.0);
	std::atomic<double> frequency = 440.0;
	std::atomic<double> amplitude = 0.2;

	double osc(double a_hertz, double a_time, int a_type) {
		// Avoid large amplitudes with low frequencies
		switch (a_type) {
		case 0:
			// Sine
			return sin(Utility::freqToVel(frequency) * a_time);
		case 1:
			// Square
			return (sin(Utility::freqToVel(frequency) * a_time)) > 0.0 ? 1.0 : -1.0;
		case 2:
			// Triangle
			return asin(sin(Utility::freqToVel(frequency) * a_time)) * 2.0 / Utility::pi;
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

	double makeNoise(double a_time) {
		double output = osc(frequency, a_time, 3);
		return output * amplitude;
	}
}

class SynthEngine {
private:
	std::vector<std::wstring> m_devices;
	NoiseMaker<short> m_sound;

public:
	SynthEngine();
};

SynthEngine::SynthEngine()
	: m_devices(NoiseMaker<short>::Enumerate()),
	  m_sound(m_devices[0]) {
	std::cout << "Starting engine..." << std::endl;
	
	for (std::wstring d : m_devices) {
		std::wcout << "Found Output Device: " << d << std::endl;
	}
	std::wcout << endl <<
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |" << endl <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |" << endl <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__" << endl <<
		"|     |     |     |     |     |     |     |     |     |     |" << endl <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |" << endl <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << endl << endl;

	m_sound.SetUserFunction(noiseSpace::makeNoise);

	// Create keyboard piano of 2 octaves
	while (1) {
		bool keyPressed = false;
		for (int i = 0; i < 15; i++) {
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe"[i])) & 0x8000) {
				noiseSpace::frequency = noiseSpace::octaveBaseFreq * std::pow(noiseSpace::root, i);
				wcout << "\rNote On : " << m_sound.GetTime() << "s " << noiseSpace::frequency << "Hz";
				keyPressed = true;
			}
		}

		if (!keyPressed) noiseSpace::frequency = 0.0;
	}
}

#endif
