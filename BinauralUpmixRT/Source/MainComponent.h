/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DragAndDropArea.h"

// Constants
#define N_CH 2

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent,
						public ChangeListener,
						public Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

	//==============================================================================
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void timerCallback() override;
	void updateLoopState(bool shouldLoop);

	//==============================================================================
	void getComplexFFTBuffer(float** fftBuffer, size_t fftSize);

	void channelAutoCorrelation(std::complex<float>* channelFFT, std::complex<float>* autoCorrelationChannel, float FF, int bufferSize);
	void audioCrossCorrelation(std::complex<float>* rightFFTBuffer, std::complex<float>* leftFFTBuffer, float FF, int bufferSize);

	void computeCrossCorrelationCoefficient(std::complex<float>* crossCorrelationChannel, std::complex<float>* leftAutoCorrelation, std::complex<float>* rightAutoCorrelation, int bufferSize);

private:
	//==============================================================================
	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
		Paused,
		Pausing,
		Stopping
	};

	//==============================================================================
	void changeState(TransportState newState);
	void openButtonClicked();
	void playButtonClicked();
	void stopButtonClicked();
	void loopButtonChanged();

    //==============================================================================

	AudioFormatManager formatManager;
	std::unique_ptr<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;
	TransportState state;

	// STFT variables
	dsp::FFT forwardFFT;
	float** fftBuffer;
	
	std::complex<float>** complexFFTBuffer;
	std::complex<float>* rightFFTChannel;
	std::complex<float>* leftFFTChannel;

	// Correlation variables
	std::complex<float>* rightAutoCorrelation;
	std::complex<float>* leftAutoCorrelation;

	std::complex<float>* crossCorrelationLR;
	std::complex<float>* crossCorrelationCoefficient;


	// UI --- Still to determine
	TextButton openButton;
	TextButton playButton;
	TextButton stopButton;
	ToggleButton loopingToggle;
	Label currentPositionLabel;

	DragAndDropArea dropArea;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
