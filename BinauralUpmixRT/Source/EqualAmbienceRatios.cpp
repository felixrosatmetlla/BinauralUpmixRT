/*
  ==============================================================================

    EqualAmbienceRatios.cpp
    Created: 29 Feb 2020 4:52:37pm
    Author:  Felix

  ==============================================================================
*/

#include "EqualAmbienceRatios.h"

float* EqualAmbienceRatios::AlphaCommonMask(std::complex<float>* crossCorrelationCoefficient, float* alphaCommonMask, int bufferSize)
{
	for (int index = 0; index < bufferSize; index++)
	{
		alphaCommonMask[index] = sqrt(1 - abs(crossCorrelationCoefficient[index]));
	}

	return alphaCommonMask;
}
