/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent():state(Stopped), forwardFFT(8), dropArea("Drop and AudioFile or a SOFA file")
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

	addAndMakeVisible(dropArea);

	addAndMakeVisible(&openButton);
	openButton.setButtonText("Open...");
	openButton.onClick = [this] { openButtonClicked(); };

	addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.onClick = [this] { playButtonClicked(); };
	playButton.setColour(TextButton::buttonColourId, Colours::green);
	playButton.setEnabled(false);

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.onClick = [this] { stopButtonClicked(); };
	stopButton.setColour(TextButton::buttonColourId, Colours::red);
	stopButton.setEnabled(false);

	addAndMakeVisible(&loopingToggle);
	loopingToggle.setButtonText("Loop");
	loopingToggle.onClick = [this] { loopButtonChanged(); };

	addAndMakeVisible(&currentPositionLabel);
	currentPositionLabel.setText("Stopped", dontSendNotification);

	formatManager.registerBasicFormats();
	transportSource.addChangeListener(this);

	startTimer(20);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
	// This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

	// Memory Allocation
	fftBuffer = (float**)calloc(N_CH, sizeof(float*));
	for (int i = 0; i < N_CH; i++) {
		if (fftBuffer != NULL)
		{
			fftBuffer[i] = (float*)calloc(forwardFFT.getSize() * 2.0, sizeof(float));
		}
	}

	complexFFTBuffer = (std::complex<float>**)calloc(N_CH, sizeof(std::complex<float>*));
	for (int i = 0; i < N_CH; i++) {
		if (complexFFTBuffer != NULL)
		{
			complexFFTBuffer[i] = (std::complex<float>*)calloc(forwardFFT.getSize(), sizeof(std::complex<float>));
		}
	}

	rightFFTChannel = (std::complex<float>*)calloc(forwardFFT.getSize(), sizeof(std::complex<float>));
	leftFFTChannel = (std::complex<float>*)calloc(forwardFFT.getSize(), sizeof(std::complex<float>));

	rightAutoCorrelation = (std::complex<float>*)calloc(forwardFFT.getSize(), sizeof(std::complex<float>));
	leftAutoCorrelation = (std::complex<float>*)calloc(forwardFFT.getSize(), sizeof(std::complex<float>));

	crossCorrelationLR = (std::complex<float>*)calloc(forwardFFT.getSize(), sizeof(std::complex<float>));
	crossCorrelationCoefficient = (std::complex<float>*)calloc(forwardFFT.getSize(), sizeof(std::complex<float>));

	// Transport Source setup
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!
    // For more details, see the help for AudioProcessor::getNextAudioBlock()
	
	if (readerSource.get() == nullptr) {
		bufferToFill.clearActiveBufferRegion();
		return;
	}

	// --- Audio Processing ---
	
	// To Frequency domain
	for (int ch = 0; ch < N_CH; ch++) 
	{
		forwardFFT.performRealOnlyForwardTransform(fftBuffer[ch], true);
	}

	getComplexFFTBuffer(fftBuffer, forwardFFT.getSize() + 1.0);

	// Separate the channels into different variables
	for (int channel = 0; channel < N_CH; channel++)
	{
		for (int sample = 0; sample < forwardFFT.getSize(); sample++)
		{
			if (channel == 0)
			{
				leftFFTChannel[sample] = complexFFTBuffer[channel][sample];
			}
			else if (channel == 1)
			{
				rightFFTChannel[sample] = complexFFTBuffer[channel][sample];
			}
		}
	}

	channelAutoCorrelation(leftFFTChannel, leftAutoCorrelation, 0.7, forwardFFT.getSize());
	channelAutoCorrelation(rightFFTChannel, rightAutoCorrelation, 0.7, forwardFFT.getSize());


	// Pass next audio block to the transport source to play
	transportSource.getNextAudioBlock(bufferToFill);

    // (to prevent the output of random noise)
    //bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

	// Memory Allocation Release
	for (int ch = 0; ch < N_CH; ch++) {
		free(fftBuffer[ch]);
	}
	free(fftBuffer);

	for (int ch = 0; ch < N_CH - 1; ch++) {
		free(complexFFTBuffer[ch]);
	}
	free(complexFFTBuffer);

	free(rightFFTChannel);
	free(leftFFTChannel);

	free(rightAutoCorrelation);
	free(leftAutoCorrelation);

	transportSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	
	auto r = getLocalBounds().reduced(8);

	dropArea.setBounds(r.removeFromBottom(150).removeFromRight(250));

	openButton.setBounds(10, 10, getWidth() - 20, 20);
	playButton.setBounds(10, 40, getWidth() - 20, 20);
	stopButton.setBounds(10, 70, getWidth() - 20, 20);
	loopingToggle.setBounds(10, 100, getWidth() - 20, 20);
	currentPositionLabel.setBounds(10, 130, getWidth() - 20, 20);
}

//==============================================================================
//When changes in the transport are reported, this function will be called 
//This will be called asynchronously on the message thread
void MainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &transportSource)
	{
		if (transportSource.isPlaying()) {
			changeState(Playing);
		}
		else if((state == Stopping) || (state == Playing)) {
			changeState(Stopped);
		}
		else if ((state == Pausing)) {
			changeState(Paused);
		}
	}
}

void MainComponent::timerCallback()
{
	if (transportSource.isPlaying())
	{
		RelativeTime position(transportSource.getCurrentPosition());

		int minutes = ((int)position.inMinutes()) % 60;
		int seconds = ((int)position.inSeconds()) % 60;
		int millis = ((int)position.inMilliseconds()) % 1000;

		juce::String positionString = String::formatted("%02d:%02d:%03d", minutes, seconds, millis);

		currentPositionLabel.setText(positionString, dontSendNotification);
	}
	else
	{
		currentPositionLabel.setText("Stopped", dontSendNotification);
	}
}

void MainComponent::updateLoopState(bool shouldLoop)
{
	if (readerSource.get() != nullptr) readerSource->setLooping(shouldLoop);
}

//==============================================================================
void MainComponent::changeState(TransportState newState)
{
	if (state != newState) 
	{
		state = newState;

		switch (state) 
		{
			case Stopped:
				stopButton.setEnabled(false);
				playButton.setButtonText("Play");
				transportSource.setPosition(0.0);
				break;

			case Starting:
				transportSource.start();
				break;

			case Playing:
				playButton.setButtonText("Pause");
				stopButton.setEnabled(true);
				break;

			case Pausing:
				transportSource.stop();
				break;

			case Paused:
				playButton.setButtonText("Play");
				break;

			case Stopping:
				transportSource.stop();
				break;

		}
	}
}

void MainComponent::openButtonClicked()
{
	FileChooser chooser("Select a Wave file to play...",
		File::getSpecialLocation(File::userHomeDirectory),"*.wav");

	if (chooser.browseForFileToOpen()) 
	{
		File file = chooser.getResult();

		AudioFormatReader* reader = formatManager.createReaderFor(file);

		if (reader != nullptr)
		{
			std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource(reader, true));
			transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
			playButton.setEnabled(true);
			readerSource.reset(newSource.release());
		}
	}
}

void MainComponent::playButtonClicked()
{
	if ((state == Stopped) || (state == Paused)) {
		
		changeState(Starting);
	}
	else if ((state == Playing)) {
		changeState(Pausing);
	}
}

void MainComponent::stopButtonClicked()
{
	if (state == Paused) {
		changeState(Stopped);
	}
	else {
		changeState(Stopping);
	}
}

void MainComponent::loopButtonChanged()
{
	updateLoopState(loopingToggle.getToggleState());
}

//==============================================================================
void MainComponent::getComplexFFTBuffer(float** fftBuffer, size_t fftSize)
{
	for (int channel = 0; channel < N_CH; channel++) 
	{
		for (int sample = 0, fftsample = 0; sample < fftSize; sample++, fftsample += 2) 
		{
			complexFFTBuffer[channel][sample].real(fftBuffer[channel][fftsample]);
			complexFFTBuffer[channel][sample].imag(fftBuffer[channel][fftsample + 1]);
			//std::cout << "complex: " << complexFFTBuffer[channel][sample] << std::endl;
		}
	}
}

void MainComponent::channelAutoCorrelation(std::complex<float>* channelFFT, std::complex<float>* autoCorrelationChannel,float FF, int bufferSize)
{
	for (int sample = 0; sample < bufferSize; sample++)
	{
		if (sample == 0) 
		{
			autoCorrelationChannel[sample] = pow(abs(channelFFT[sample]), 2);

			std::complex<float> lastAutoCorrelation = 0;
			std::complex<float> actualAutoCorrelation = (1 - FF) * autoCorrelationChannel[sample];

			autoCorrelationChannel[sample] = lastAutoCorrelation + actualAutoCorrelation;
		}
		else
		{
			autoCorrelationChannel[sample] = pow(abs(channelFFT[sample]), 2);

			std::complex<float> lastAutoCorrelation = FF * autoCorrelationChannel[sample - 1];
			std::complex<float> actualAutoCorrelation = (1 - FF) * autoCorrelationChannel[sample];

			autoCorrelationChannel[sample] = lastAutoCorrelation + actualAutoCorrelation;
		}
	}
}

void MainComponent::audioCrossCorrelation(std::complex<float>* rightFFTBuffer, std::complex<float>* leftFFTBuffer, float FF, int bufferSize)
{
	for (int sample = 0; sample < bufferSize; sample++)
	{
		if (sample == 0)
		{
			crossCorrelationLR[sample] = leftFFTBuffer[sample] * std::conj(rightFFTBuffer[sample]);

			std::complex<float> lastCrossCorrelation = 0;
			std::complex<float> actualCrossCorrelation = (1 - FF) * crossCorrelationLR[sample];

			crossCorrelationLR[sample] = lastCrossCorrelation + actualCrossCorrelation;
		}
		else
		{
			crossCorrelationLR[sample] = leftFFTBuffer[sample] * std::conj(rightFFTBuffer[sample]);

			std::complex<float> lastCrossCorrelation = FF * crossCorrelationLR[sample - 1];
			std::complex<float> actualCrossCorrelation = (1 - FF) * crossCorrelationLR[sample];

			crossCorrelationLR[sample] = lastCrossCorrelation + actualCrossCorrelation;
		}
	}
}

void MainComponent::computeCrossCorrelationCoefficient(std::complex<float>* crossCorrelationChannel, std::complex<float>* leftAutoCorrelation, std::complex<float>* rightAutoCorrelation, int bufferSize)
{
	for (int i = 0; i < bufferSize; i++)
	{
		crossCorrelationCoefficient[i] = crossCorrelationChannel[i] / std::sqrt(leftAutoCorrelation[i] * rightAutoCorrelation[i]);
	}
}
