#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <limits>
#include "vec2.h"
#include "vec3.h"

namespace Utility {
	/////////////////
	// Constants
	/////////////////
	const double infinity = std::numeric_limits<double>::infinity();
	const double pi = 3.1415926535897932385;
	const double espilon = 0.0001;

	/////////////////
	// Random noise
	/////////////////
	double randomDoubleNormalised() {
		// Between 0.0 .. 1.0
		return (double)rand() / ((double)RAND_MAX + 1.0);
	}

	double randomDouble() {
		// Between -1.0 .. 1.0
		return 2.0 * (double)rand() / ((double)RAND_MAX) - 1.0;
	}

	double randomDouble(double aMin, double aMax) {
		return aMin + (aMax - aMin) * randomDouble();
	}

	int randomInt(int aMin, int aMax) {
		int randNum = rand() % (aMax - aMin + 1) + aMin;
		return randNum;
	}

	float hash21(vec2 co) {
		return std::fmod(std::sin(co.getX() * 12.9898 + co.getY() * 78.233) * 43758.5453, 1.0);
	}

	float hash31(vec3 p) {
		vec3 fract = vec3(std::fmod(p.getX(), 1.0f), std::fmod(p.getY(), 1.0f), std::fmod(p.getZ(), 1.0f));
		fract = fract * vec3(.1031, .11369, .13787);
		float res = std::fmod(dot(fract, vec3(1.0f, 1.0f, 1.0f)), 19.19f);
		fract += vec3(res, res, res);

		float output = -1.0f + 2.0f * std::fmod((fract.getX() + fract.getY()) * fract.getZ(), 1.0f);

		return output;
	}

	vec3 hash33(vec3 p3) {
		vec3 p = p3 * vec3(0.1031, 0.11369, 0.13787);
		p = p.fract();
		float dotProd = dot(p, vec3(p.getY(), p.getX(), p.getZ()) + vec3(19.19, 19.19, 19.19));
		p += vec3(dotProd, dotProd, dotProd);
		return vec3(-1.0, -1.0, -1.0) + 2.0 * vec3(
			std::fmod((p.getX() + p.getY()) * p.getZ(), 1.0),
			std::fmod((p.getX() + p.getZ()) * p.getY(), 1.0),
			std::fmod((p.getY() + p.getZ()) * p.getX(), 1.0)
		);
	}

	/////////////////
	// Math
	/////////////////
	double clamp(double x, double aMin, double aMax) {
		if (x < aMin) return aMin;
		if (x > aMax) return aMax;
		return x;
	}

	int clamp(int x, int aMin, int aMax) {
		if (x < aMin) return aMin;
		if (x > aMax) return aMax;
		return x;
	}

	// Convert freq (Hz) to angular velocity
	double freqToVel(double aHertz) {
		return aHertz * 2.0 * pi;
	}

	// Scale to freq convert
	const int SCALE_DEFAULT = 0;
	double scale(const int aNoteId, const int aScaleId = SCALE_DEFAULT) {
		switch (aScaleId) {
		case SCALE_DEFAULT: default:
			return 256 * std::pow(1.0594630943592952645618252949463, aNoteId);
		}
	}
}


namespace Synth {
	enum WaveForm {
		OSC_SINE = 0,
		OSC_SQUARE,
		OSC_TRIANGLE,
		OSC_SAW_LIM,
		OSC_SAW,
		OSC_NOISE,
	};

	double osc(double aTime,
		double aHertz,
		WaveForm a_type = OSC_SINE,
		double aLFOHertz = 0.0,
		double aLFOAmp = 0.0,
		double aCustom = 50.0) {
		// Avoid large amplitudes with low frequencies
		// LFO
		double freq = Utility::freqToVel(aHertz) * aTime + aLFOAmp * aHertz * sin(Utility::freqToVel(aLFOHertz) * aTime);

		switch (a_type) {
		case OSC_SINE:
			return sin(freq);

		case OSC_SQUARE:
			return (sin(freq) > 0.0) ? 1.0 : -1.0;

		case OSC_TRIANGLE:
			return asin(freq) * 2.0 / Utility::pi;

		case OSC_SAW_LIM: {
			// Saw (using sum of sine lim inf, soft)
			double output = 0.0;
			for (double i = 1.0; i < 100.0; i++) {
				output += (sin(i * freq)) / i;
			}
			return output * (2.0 / Utility::pi);
		}

		case OSC_SAW:
			// Saw (using mod)
			return (2.0 / Utility::pi) * (aHertz * Utility::pi * std::fmod(aTime, 1.0 / aHertz) - (Utility::pi / 2.0));

		case OSC_NOISE:
			return Utility::randomDouble(); // freq does not affect... so plays constantly

		default:
			return 0.0;
		}
	}
}

#endif