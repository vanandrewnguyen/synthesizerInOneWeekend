#ifndef SYNTHENGINE_H
#define SYNTHENGINE_H

#include <vector>
#include <string>

#include "utils.h"
#include "noiseMaker.h"

class SynthEngine {
private:
	std::vector<std::wstring> m_devices;
	NoiseMaker<short> m_sound;

public:
	SynthEngine();
	double createNoise(double a_time);
};

SynthEngine::SynthEngine()
	: m_devices(NoiseMaker<short>::Enumerate()),
	  m_sound(m_devices[0]) {
	std::cout << "Starting engine..." << std::endl;
	
	for (std::wstring d : m_devices) {
		std::wcout << "Found Output Device: " << d << std::endl;
	}

	// Keep window active
	while (1) {

	}
}

double SynthEngine::createNoise(double a_time) {
	// 440 Hz
	return 0.5 * sin(440.0 * 2 * Utility::pi * a_time);
}

#endif
