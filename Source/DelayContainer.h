/*
  ==============================================================================

    DelayContainer.h
    Created: 14 May 2019 4:13:06pm
    Author:  jonny

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

	void initialDelayEffect(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer, int delayTime);

	void feedbackDelay(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer, float oldFeedback, float feedback);

	void reverseDelayBuffer(int channel, AudioBuffer<float> &reverseBuffer, AudioBuffer<float> &delayBuffer);

	void mixBuffers(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &dryBuffer, float mixParameter);

private:

	int *writePosition;
	int lastSampleRate;

	int sourceBufferSize;
	int delayBufferSize;
};