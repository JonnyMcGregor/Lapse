/*
  ==============================================================================

    DelayContainer.cpp
    Created: 14 May 2019 4:13:08pm
    Author:  jonny

  ==============================================================================
*/

#include "DelayContainer.h"


DelayContainer::DelayContainer(int* writePositionHost)
{
	writePosition = writePositionHost;
}

void DelayContainer::initialise(int sampleRate, int sourceBufferLength, int delayBufferLength)
{
	lastSampleRate = sampleRate;
	sourceBufferSize = sourceBufferLength;
	delayBufferSize = delayBufferLength;
}

//Fill dry buffer with raw audio data, this will then be applied to the mix variable
void DelayContainer::fillDryBuffer(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &dryBuffer)
{
	dryBuffer.copyFrom(channel, 0, sourceBuffer.getReadPointer(channel), sourceBufferSize);
}

//==============================================================================
/*
	In fillDelayBuffer() the delayBuffer is filled with the raw audio data from the
	input device.The total size of the delayBuffer must be greater than the maximum
	delay time in samples as it acts as our memory bank for recalling the delayed audio.
*/

void DelayContainer::fillDelayBuffer(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer)
{

	if (delayBufferSize > sourceBufferSize + *writePosition)
	{
		delayBuffer.copyFrom(channel, *writePosition, sourceBuffer.getWritePointer(channel), sourceBufferSize);
	}

	else
	{
		const int bufferRemaining = delayBuffer.getNumSamples() - *writePosition;

		delayBuffer.copyFrom(channel, *writePosition, sourceBuffer.getWritePointer(channel), bufferRemaining);
		delayBuffer.copyFrom(channel, 0, sourceBuffer.getWritePointer(channel), sourceBufferSize - bufferRemaining);
	}
}

//==============================================================================
/*
	In reverseDelayBuffer() the data stored within the delayBuffer is copied in reverse
	into the reverseBuffer. This doesn't create a typical reverse that you would see
	in most delay plugins, however it sounds pretty cool.
*/

void DelayContainer::reverseDelayBuffer(int channel, AudioBuffer<float> &reverseBuffer, AudioBuffer<float> &delayBuffer)
{
	if (delayBufferSize > sourceBufferSize + *writePosition)
	{
		for (int i = *writePosition; i < *writePosition + sourceBufferSize; i++)
		{
			reverseBuffer.setSample(channel, delayBuffer.getNumSamples() - 1 - i, delayBuffer.getSample(channel, i));
		}
	}
	else
	{
		const int bufferRemaining = delayBufferSize - *writePosition;
		for (int i = 0; i < bufferRemaining; i++)
		{
			reverseBuffer.setSample(channel, bufferRemaining - i, delayBuffer.getSample(channel, delayBufferSize - 1 - bufferRemaining + i));
			reverseBuffer.setSample(channel, delayBufferSize - 1, delayBuffer.getSample(channel, i));
		}
	}

		
}
//==============================================================================
/*
	intitialDelyEffect() calculates a readPosition based on the delayTime parameter set
	by the user and the current writePosition to identify where in the delay buffer to
	begin copying from. The data stored within the delayBuffer is then copied back into
	the main buffer which creates a single delay.
*/
void DelayContainer::initialDelayEffect(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer, float delayTime)
{
	int delayTimeSamples = lastSampleRate * delayTime / 1000; //calculate the delayTime in samples
	//The readPosition is where in the delay buffer to start reading from when copying into the main buffer.
	const int readPosition = static_cast<int> (delayBufferSize + (*writePosition - delayTimeSamples)) % delayBufferSize;

	if (previousDelayTimeSamples != delayTimeSamples)
	{
		//smoothParameterChangeInt(delayTimeSamples, previousDelayTimeSamples);
	}
	
	if (delayBuffer.getNumSamples() > sourceBuffer.getNumSamples() + readPosition)
	{
		sourceBuffer.copyFrom(channel, 0, delayBuffer.getWritePointer(channel) + readPosition, sourceBufferSize);
	}

	else
	{
		const int bufferRemaining = delayBuffer.getNumSamples() - readPosition;
		sourceBuffer.copyFrom(channel, 0, delayBuffer.getWritePointer(channel) + readPosition, bufferRemaining);
		sourceBuffer.copyFrom(channel, bufferRemaining, delayBuffer.getWritePointer(channel), sourceBufferSize - bufferRemaining);
	}

	previousDelayTimeSamples = delayTimeSamples;
}

//==============================================================================
/*
	feedbackDelay() provides the decaying repetitions of our delayed audio by copying the
	audio with a single delay stored in the main buffer back into the delayBuffer with a
	reduced gain value. This value is determined by the feedback parameter on the UI.
*/
void DelayContainer::feedbackDelay(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer, float oldFeedback, float feedback)
{
	if (delayBufferSize > sourceBufferSize + *writePosition)
	{
		delayBuffer.addFromWithRamp(channel, *writePosition, sourceBuffer.getWritePointer(channel), sourceBufferSize, oldFeedback, feedback);
	}

	else
	{
		const int bufferRemaining = delayBufferSize - *writePosition;

		//calculate the gain based on the feedback knob and writeposition within the buffer
		const float midGain = oldFeedback + ((feedback - oldFeedback / sourceBufferSize) * (bufferRemaining / sourceBufferSize));

		delayBuffer.addFromWithRamp(channel, *writePosition, sourceBuffer.getWritePointer(channel), bufferRemaining, oldFeedback, midGain);
		delayBuffer.addFromWithRamp(channel, 0, sourceBuffer.getWritePointer(channel), sourceBufferSize - bufferRemaining, midGain, feedback);
	}
}

//mixBuffers() creates a dry/wet mix of the dryBuffer and sourceBuffer using a mixParameter value
void DelayContainer::mixBuffers(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &dryBuffer, float mixParameter)
{
	if (previousMixValue != mixParameter)
	{
		//smoothParameterChangeFloat(mixParameter, previousMixValue);
	}

	for (int sample = 0; sample < sourceBufferSize; sample++)
	{									 /*-------------------------- DRY_MIX -------------------------------- + ------------------------ WET_MIX ----------------------*/
		sourceBuffer.setSample(channel, sample, ((dryBuffer.getSample(channel, sample) * (1 - mixParameter)) + (sourceBuffer.getSample(channel, sample) * mixParameter)));
	}

	previousMixValue = mixParameter;
}

void DelayContainer::smoothParameterChangeFloat(float& currentValue, float& previousValue)
{
	currentValue = previousValue + ((currentValue - previousValue) * 0.01);
}

void DelayContainer::smoothParameterChangeInt(int& currentValue, int& previousValue)
{
	currentValue = previousValue + ((currentValue - previousValue) * 0.01);
}



