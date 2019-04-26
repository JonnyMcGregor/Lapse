/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayAudioProcessor::SimpleDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
	parameters(*this, nullptr, Identifier("SimpleDelay"),
		{ std::make_unique<AudioParameterFloat>("mix",             // parameter ID
												"Mix",             // parameter name
												0.0f,              // minimum value
												2.0f,              // maximum value
												1.0f),             // default value
		  std::make_unique<AudioParameterFloat>("delayTime",       
												"Delay Time",      
												0.0f,             
												1000.0f,           
												0.0f),             
		  std::make_unique<AudioParameterFloat>( "feedback",	
											   "Feedback",
											    0,
											    2.0,
											    1.0)
		})
#endif
{
	//Initialise parameters
	mixParameter = parameters.getRawParameterValue("mix");
	delayParameter = parameters.getRawParameterValue("delayTime");
	feedbackParameter = parameters.getRawParameterValue("feedback");
}

SimpleDelayAudioProcessor::~SimpleDelayAudioProcessor()
{
}

//==============================================================================
const String SimpleDelayAudioProcessor::getName() const
{
    return "SimpleDelay";
}

bool SimpleDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String SimpleDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SimpleDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const int numberInputChannels = getTotalNumInputChannels();   //variable for number of input channels
    const int delayBufferSize = 2*(sampleRate + samplesPerBlock); //variable for buffer size
    mSampleRate = sampleRate;									  //set sample rate to global variable
    delayBuffer.setSize(numberInputChannels, delayBufferSize);   //Setup parameters for mDelayBuffer
    dryBuffer.setSize(numberInputChannels, samplesPerBlock);     //Setup parameters for mDryBuffer
	
}

void SimpleDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleDelayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	
	//Clears out buffer for initialisation
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
	{
		buffer.clear(i, 0, buffer.getNumSamples());
		delayBuffer.clear(i, 0, delayBuffer.getNumSamples());
	}

	
    const int bufferLength = buffer.getNumSamples(); //local variable for buffersize
    const int delayBufferLength = delayBuffer.getNumSamples(); //local variable for length of delay buffer

	int delayTime = *delayParameter;
	float feedback = *feedbackParameter;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* bufferData = buffer.getWritePointer(channel); //local pointer for audio data stored inside buffer
        float* delayBufferData = delayBuffer.getWritePointer(channel); //local pointer for delayed audio data
        
        fillDryBuffer(channel, bufferLength, bufferData);
        
		float* dryBufferData = dryBuffer.getWritePointer(channel);
		
        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData, delayTime);
        
        delayBufferData = delayBuffer.getWritePointer(channel);
        
		for (int sample = 0; sample < buffer.getNumSamples(); sample++)
		{									 /*----------------- DRY_MIX ---------------------- + ---------------- WET_MIX -------------- MEAN*/
			buffer.setSample(channel, sample, ((dryBufferData[sample] * (2.0 - *mixParameter)) + (bufferData[sample] * *mixParameter)) / 2.0f);
		}
		
		feedbackDelay(channel, bufferLength, delayBufferLength, bufferData, oldFeedback, feedback);
		oldFeedback = feedback;
		
    }
    //Update write position for tracking delay buffer position
    writePosition += bufferLength;
    writePosition %= delayBufferLength;
}

//==============================================================================
//Fill dry buffer with raw audio data, this will then be applied to the mix variable
void SimpleDelayAudioProcessor::fillDryBuffer(int channel, const int bufferLength,
                                                const float* bufferData)
{
        dryBuffer.copyFrom(channel, 0, bufferData, bufferLength);
}
//==============================================================================
//Fill the delay buffer with dry data

void SimpleDelayAudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength,
                                                const float* bufferData, const float* delayBufferData)
{

    if(delayBufferLength > bufferLength + writePosition)
    {
        delayBuffer.copyFrom(channel, writePosition, bufferData, bufferLength);
    }

    else
    {
        const int bufferRemaining = delayBufferLength - writePosition;

        delayBuffer.copyFrom(channel, writePosition, bufferData, bufferRemaining);
        delayBuffer.copyFrom(channel, 0, bufferData, bufferLength - bufferRemaining);
    }
}
//==============================================================================
//Copy delay buffer data into the main buffer according to the delay time parameter.
//This function creates a single delay in the output

void SimpleDelayAudioProcessor::getFromDelayBuffer(AudioBuffer<float> &buffer, int channel, const int bufferLength, const int delayBufferLength,
                                                   const float* bufferData, const float* delayBufferData, int delayTime)
{
    int delayTimeSamples = mSampleRate * delayTime / 1000;
    const int readPosition = static_cast<int> (delayBufferLength + (writePosition - delayTimeSamples)) % delayBufferLength;

    if(delayBufferLength > bufferLength + readPosition)
    {
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferLength);
    }

    else
    {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.copyFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }

}
//==============================================================================
//Feedback is achieved by copying the delay data back into the delay buffer
//The feedback amount is calculated using the feedback knob.

void SimpleDelayAudioProcessor::feedbackDelay(int channel, const int bufferLength, const int delayBufferLength,
                                              const float* dryBuffer, float oldFeedback, float feedback)
{
    if(delayBufferLength > bufferLength + writePosition)
    {
        delayBuffer.addFromWithRamp(channel, writePosition, dryBuffer, bufferLength, oldFeedback, feedback);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - writePosition;
		//calculate the gain based on the feedback knob and writeposition within the buffer
		const float midGain = oldFeedback + ((feedback - oldFeedback / bufferLength) * (bufferRemaining / bufferLength));
	
        delayBuffer.addFromWithRamp(channel, writePosition, dryBuffer, bufferRemaining, oldFeedback, midGain);
        delayBuffer.addFromWithRamp(channel, 0, dryBuffer, bufferLength - bufferRemaining, midGain, feedback);
    }
}
//==============================================================================
bool SimpleDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SimpleDelayAudioProcessor::createEditor()
{
    return new SimpleDelayAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void SimpleDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	auto state = parameters.copyState();
	std::unique_ptr<XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void SimpleDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.replaceState(ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleDelayAudioProcessor();
}
