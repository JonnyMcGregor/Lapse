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
        for(int i = 0; i < sourceBufferSize; i++)
        {
            delayBuffer.setSample(channel, *writePosition + i, sourceBuffer.getSample(channel, i));
        }
		//delayBuffer.copyFrom(channel, *writePosition, sourceBuffer.getWritePointer(channel), sourceBufferSize);
	}

	else
	{
		const int bufferRemaining = delayBuffer.getNumSamples() - *writePosition;
        
        for (int i = 0; i < bufferRemaining; i++)
        {
            delayBuffer.setSample(channel, *writePosition + i, sourceBuffer.getSample(channel, i));
        }
        for(int i = bufferRemaining; i < sourceBufferSize; i++)
        {
            delayBuffer.setSample(channel, i, sourceBuffer.getSample(channel, i));
        }
//        delayBuffer.copyFrom(channel, *writePosition, sourceBuffer.getWritePointer(channel), bufferRemaining);
//        delayBuffer.copyFrom(channel, 0, sourceBuffer.getWritePointer(channel), sourceBufferSize - bufferRemaining);
	}
}

//==============================================================================
/*
	intitialDelayEffect() calculates a readPosition based on the delayTime parameter set
	by the user and the current writePosition to identify where in the delay buffer to
	begin copying from. The data stored within the delayBuffer is then copied back into
	the main buffer which creates a single delay.
*/
void DelayContainer::initialDelayEffect(int channel, AudioBuffer<float> &sourceBuffer, AudioBuffer<float> &delayBuffer, float delayTime)
{
	int delayTimeSamples = lastSampleRate * delayTime / 1000; //calculate the delayTime in samples
	//The readPosition is where in the delay buffer to start reading from when copying into the main buffer.
	const int readPosition = static_cast<int> (delayBufferSize + (*writePosition - delayTimeSamples)) % delayBufferSize;

	if (delayBuffer.getNumSamples() > sourceBuffer.getNumSamples() + readPosition)
	{
        for(int i = 0; i < sourceBufferSize; i++)
        {
            sourceBuffer.setSample(channel, i, *(delayBuffer.getWritePointer(channel) + readPosition + i));
        }
	}

	else
	{
		const int bufferRemaining = delayBuffer.getNumSamples() - readPosition;
        
        for (int i = 0; i < bufferRemaining; i++)
        {
            sourceBuffer.setSample(channel, i, delayBuffer.getSample(channel, readPosition + i));
        }
        for(int i = 0; i < sourceBufferSize - bufferRemaining; i++)
        {
            sourceBuffer.setSample(channel, bufferRemaining, delayBuffer.getSample(channel, i));
        }
	}
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
	for (int sample = 0; sample < sourceBufferSize; sample++)
	{									 /*-------------------------- DRY_MIX -------------------------------- + ------------------------ WET_MIX ----------------------*/
		sourceBuffer.setSample(channel, sample, ((dryBuffer.getSample(channel, sample) * (0.8 - mixParameter)) + (sourceBuffer.getSample(channel, sample) * (0.2 + mixParameter))));
	}
}







