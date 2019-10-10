/*
  ==============================================================================

    DelayContainer.h
    Created: 14 May 2019 4:13:06pm
    Author:  jonny

	This DelayContainer class was heavily based off Daniel Walz's ffTapeDelay algorithm.

	Link: https://github.com/ffAudio/ffTapeDelay

	I essentially placed it into its own class and added a few extra functions such as 
	reverseDelayBuffer() and mixBuffers(). Ideally I would like to expand on this class
	in the future to add some more wacky delay effects.

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class DelayContainer
{
public:

	DelayContainer(int* writePositionHost);
	
	void initialise(int sampleRate, int sourceBufferLength, int delayBufferLength);

	void fillDryBuffer(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &dryBuffer);

	void fillDelayBuffer(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer);

	void initialDelayEffect(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer, float delayTime);

	void feedbackDelay(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer, float oldFeedback, float feedback);

	void reverseDelayBuffer(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer, float delayTime);

	void mixBuffers(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &dryBuffer, float mixParameter);

private:

	int *writePosition;
	int lastSampleRate;

	int sourceBufferSize;
	int delayBufferSize;

	int previousDelayTimeSamples = 0;
	float previousMixValue = 0;
};
