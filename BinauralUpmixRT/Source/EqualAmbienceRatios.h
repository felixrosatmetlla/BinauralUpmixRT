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
	static float* AlphaCommonMask(std::complex<float>* crossCorrelationCoefficient, float* alphaCommonMask, int bufferSize);
};