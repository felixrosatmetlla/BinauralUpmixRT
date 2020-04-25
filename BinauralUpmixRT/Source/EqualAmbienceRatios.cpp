/*
  ==============================================================================

    EqualAmbienceRatios.cpp
    Created: 29 Feb 2020 4:52:37pm
    Author:  Felix

  ==============================================================================
*/

#include "EqualAmbienceRatios.h"

void EqualAmbienceRatios::AmbienceSignal(std::complex<float>* signal, std::complex<float>* crossCorrelationCoefficient, std::complex<float>* ambienceSignal, int bufferSize)
{
	float* alphaCommonMask = (float*)calloc(bufferSize, sizeof(float));
	AlphaCommonMask(crossCorrelationCoefficient, alphaCommonMask, bufferSize);

	for (int index = 0; index < bufferSize; index++)
	{
		ambienceSignal[index] = signal[index] * alphaCommonMask[index];
	}

	free(alphaCommonMask);
}

void EqualAmbienceRatios::DirectSignal(std::complex<float>* signal, std::complex<float>* crossCorrelationCoefficient, std::complex<float>* directSignal, int bufferSize)
{
	float* alphaCommonMask = (float*)calloc(bufferSize, sizeof(float));
	AlphaCommonMask(crossCorrelationCoefficient, alphaCommonMask, bufferSize);

	for (int index = 0; index < bufferSize; index++)
	{
		directSignal[index] = signal[index] * (1 - alphaCommonMask[index]);
	}

	free(alphaCommonMask);
}

void EqualAmbienceRatios::AlphaCommonMask(std::complex<float>* crossCorrelationCoefficient, float* alphaCommonMask, int bufferSize)
{
	for (int index = 0; index < bufferSize; index++)
	{
		alphaCommonMask[index] = sqrt(1 - abs(crossCorrelationCoefficient[index]));
	}
}