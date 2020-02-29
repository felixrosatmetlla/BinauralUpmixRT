/*
  ==============================================================================

    EqualAmbienceRatios.h
    Created: 29 Feb 2020 4:52:37pm
    Author:  Felix

  ==============================================================================
*/

#pragma once
#include <complex>

class EqualAmbienceRatios
{
public:
	static void AmbienceSignal(std::complex<float>* signal, std::complex<float>* crossCorrelationCoefficient, std::complex<float>* ambienceSignal, int bufferSize);

	static void DirectSignal(std::complex<float>* signal, std::complex<float>* crossCorrelationCoefficient, std::complex<float>* directSignal, int bufferSize);

private:
	static void AlphaCommonMask(std::complex<float>* crossCorrelationCoefficient, float* alphaCommonMask, int bufferSize);
};