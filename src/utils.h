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

	double randomDouble(double min, double max) {
		return min + (max - min) * randomDouble();
	}

	int randomInt(int min, int max) {
		int randNum = rand() % (max - min + 1) + min;
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
	double clamp(double x, double min, double max) {
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	int clamp(int x, int min, int max) {
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	// Convert freq (Hz) to angular velocity
	double freqToVel(double a_hertz) {
		return a_hertz * 2.0 * pi;
	}
}

#endif