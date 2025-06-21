#ifndef SYNTHENGINE_H
#define SYNTHENGINE_H

#include <vector>
#include <string>

#include "utils.h"
#include "noiseMaker.h"

namespace noiseSpace {
	std::atomic<double> frequency = 440.0;
	std::atomic<double> amplitude = 0.5;

	double createSine(double a_time) {
		// 440 Hz
		return amplitude * sin(frequency * 2 * Utility::pi * a_time);
	}

	double createSquare(double a_time) {
		double wave = createSine(a_time) / amplitude;
		return (wave > 0.0) ? amplitude.load() : -amplitude.load();
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

	m_sound.SetUserFunction(noiseSpace::createSine);

	// Create keyboard piano of 2 octaves
	double octaveBaseFreq = 110.0;
	double root = std::pow(2.0, 1.0 / 12.0);
	while (1) {
		bool keyPressed = false;
		for (int i = 0; i < 15; i++) {
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe"[i])) & 0x8000) {
				noiseSpace::frequency = octaveBaseFreq * std::pow(root, i);
				keyPressed = true;
			}
		}

		if (!keyPressed) noiseSpace::frequency = 0.0;
	}
}

#endif
