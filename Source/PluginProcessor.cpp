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
		{ std::make_unique<AudioParameterFloat>("mix",             // parameter id
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
											    1.0),
		  std::make_unique<AudioParameterFloat>("frequency",        
												"Cutoff Frequency",
												20.0f,             
												20000.0f,           
												20000.0f),          
		  std::make_unique<AudioParameterFloat>("resonance",
												"Resonance",
												1.0f,
												5.0f,
												2.0f),
		  std::make_unique<AudioParameterChoice>("filterMenu",  //parameter id
												 "Filter Type", //name
												 menuChoices,   //combobox choices
												 0),			//default index value
		  std::make_unique<AudioParameterBool>("isReversing",
											     "Is Reversing",
											     false,
											     "isReversing")
		})
#endif
{
	//Initialise parameters
	mixParameter = parameters.getRawParameterValue("mix");
	delayParameter = parameters.getRawParameterValue("delayTime");
	feedbackParameter = parameters.getRawParameterValue("feedback");
	reverseParameter = parameters.getRawParameterValue("isReversing");
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
    const int numberInputChannels = getTotalNumInputChannels();   
    const int delayBufferSize = 2*(sampleRate + samplesPerBlock); 
    lastSampleRate = sampleRate;									 
    delayBuffer.setSize(numberInputChannels, delayBufferSize);   
    dryBuffer.setSize(numberInputChannels, samplesPerBlock);    
	reverseBuffer.setSize(numberInputChannels, delayBufferSize); 

	/*
		The ProcessSpec is a struct which contains information on the audio 
		that gets passed to a dsp object's prepare() function.
	*/
	

	//Initialise Process Spec
	dsp::ProcessSpec spec;
	spec.sampleRate = lastSampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getMainBusNumOutputChannels();

	//Clear the State Variable Filter in case there's garbage left over in it
	stateVariableFilter.reset();
	updateFilter();

	//Pass info stored in spec to the prepare function of the filter.
	stateVariableFilter.prepare(spec);

	//Clear buffers that are instantiated in processor.h
	delayBuffer.clear();
	reverseBuffer.clear();
	
}

void SimpleDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

}

void SimpleDelayAudioProcessor::updateFilter()
{
	auto menuChoice = *parameters.getRawParameterValue("filterMenu");
	auto filterFreq = *parameters.getRawParameterValue("frequency");
	auto res = *parameters.getRawParameterValue("resonance");

	if (menuChoice == 0)
	{
		stateVariableFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, filterFreq, res);
}
	if (menuChoice == 1)
	{
		stateVariableFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::bandPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, filterFreq, res);

	}
	if (menuChoice == 2)
	{
		stateVariableFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::highPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, filterFreq, res);
	}
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
		reverseBuffer.clear(i, 0, reverseBuffer.getNumSamples());
	}

	
    const int bufferLength = buffer.getNumSamples(); 
    const int delayBufferLength = delayBuffer.getNumSamples(); 

	int delayTime = *delayParameter;
	float feedback = *feedbackParameter;
	bool isReverseEffect = *reverseParameter;

	//========================================================================\\
	
	/*	
	
		Within this for loop is all of the processing for the delay effect
	
*/
	
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* bufferData = buffer.getWritePointer(channel); //local pointer for raw audio data 
        float* delayBufferData = delayBuffer.getWritePointer(channel); //local pointer for delayed audio data
		float* dryBufferData = dryBuffer.getWritePointer(channel);
		float* reverseBufferData = reverseBuffer.getWritePointer(channel);

        fillDryBuffer(channel, bufferLength, bufferData); //fill the dry channel of audio		
        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);

		if (isReverseEffect);
		{
			fillReverseBuffer(channel, delayBufferLength, bufferLength);
		}

        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData, reverseBufferData, delayTime, isReverseEffect);
        
		for (int sample = 0; sample < buffer.getNumSamples(); sample++)
		{									 /*----------------- DRY_MIX ---------------------- + ---------------- WET_MIX -------------- MEAN*/
			buffer.setSample(channel, sample, ((dryBufferData[sample] * (2.0 - *mixParameter)) + (bufferData[sample] * *mixParameter)) / 2.0f);
		}

		feedbackDelay(channel, bufferLength, delayBufferLength, bufferData, oldFeedback, feedback, isReverseEffect);

		fillDrawingBuffer(channel, dryBuffer, buffer, bufferLength);

		oldFeedback = feedback;
    }
	
	dsp::AudioBlock<float> block(buffer);

	updateFilter();

	stateVariableFilter.process(dsp::ProcessContextReplacing<float>(block));

    //Update write position for tracking delay buffer position
    writePosition += bufferLength;
    writePosition %= delayBufferLength;
}

//Fills an array of samples for waveform drawing
void SimpleDelayAudioProcessor::fillDrawingBuffer(int channel, AudioBuffer<float> &dryBuffer, AudioBuffer<float> &buffer, const int bufferLength)
{
	int indexValueScaled;
	for (int i = 0; i < audioDataToDraw.size(); i++)
	{
		indexValueScaled = jmap((float)i, 0.0f, bufferLength - 1.0f, 0.0f, audioDataToDraw.size() - 1.0f);
		audioDataToDraw[i] = dryBuffer.getSample(channel, indexValueScaled);
		delayAudioDataToDraw[i] = buffer.getSample(channel, indexValueScaled);
	}
	drawBufferIsFull.sendChangeMessage();
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


//==============================================================================
/*
	In fillReverseBuffer() the data stored within the delayBuffer is copied in reverse
	into the reverseBuffer. This doesn't create a typical reverse that you would see
	in most delay plugins, however it sounds pretty cool.
*/
void SimpleDelayAudioProcessor::fillReverseBuffer(int channel, const int delayBufferLength, const int bufferLength)
{
	if (delayBufferLength > bufferLength + writePosition)
	{
		for (int i = writePosition; i < writePosition + dryBuffer.getNumSamples(); i++)
		{
			reverseBuffer.setSample(channel, reverseBuffer.getNumSamples() - 1 - i, delayBuffer.getSample(channel, i));
		}
	}
	else
	{
		const int bufferRemaining = delayBufferLength - writePosition;
		for (int i = 0; i < bufferRemaining; i++)
		{
			reverseBuffer.setSample(channel, bufferRemaining - i, delayBuffer.getSample(channel, delayBuffer.getNumSamples() - 1 - bufferRemaining + i));
			reverseBuffer.setSample(channel, delayBuffer.getNumSamples() - 1, delayBuffer.getSample(channel, i));
		}
	}
}
//==============================================================================
/*
	In fillDelayBuffer() the delayBuffer is filled with the raw audio data from the
	input device.The total size of the delayBuffer must be greater than the maximum
	delay time in samples as it acts as our memory bank for recalling the delayed audio.
*/
void SimpleDelayAudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength,
	const float* bufferData, const float* delayBufferData)
{

	if (delayBufferLength > bufferLength + writePosition)
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
/*
	getFromDelayBuffer() calculates a readPosition based on the delayTime parameter set
	by the user and the current writePosition to identify where in the delay buffer to
	begin copying from. The data stored within the delayBuffer is then copied back into
	the main buffer which creates a single delay.
*/
void SimpleDelayAudioProcessor::getFromDelayBuffer(AudioBuffer<float> &buffer, int channel, const int bufferLength, const int delayBufferLength,
                                                   const float* bufferData, const float* delayBufferData, const float* reverseBufferData, int delayTime, bool isReverseEffect)
{
	int delayTimeSamples = lastSampleRate * delayTime / 1000; //calculate the delayTime in samples

	//The readPosition is where in the delay buffer to start reading from when copying into the main buffer.
    const int readPosition = static_cast<int> (delayBufferLength + (writePosition - delayTimeSamples)) % delayBufferLength; 
	
	if (isReverseEffect == 1)
	{
		if (delayBufferLength > bufferLength + readPosition)
		{
			buffer.copyFrom(channel, 0, reverseBufferData + readPosition, bufferLength);
		}

		else
		{
			const int bufferRemaining = delayBufferLength - readPosition;
			buffer.copyFrom(channel, 0, reverseBufferData + readPosition, bufferRemaining);
			buffer.copyFrom(channel, bufferRemaining, reverseBufferData, bufferLength - bufferRemaining);
		}
	}

	else
	{
		if (delayBufferLength > bufferLength + readPosition)
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
}
//==============================================================================
/*
	feedbackDelay() provides the decaying repetitions of our delayed audio by copying the
	audio with a single delay stored in the main buffer back into the delayBuffer with a
	reduced gain value. This value is determined by the feedback parameter knob on the UI.
*/
void SimpleDelayAudioProcessor::feedbackDelay(int channel, const int bufferLength, const int delayBufferLength,
                                              const float* bufferData, float oldFeedback, float feedback, bool isReverseEffect)
{
	if (isReverseEffect == 1)
	{
		if (delayBufferLength > bufferLength + writePosition)
		{			
			reverseBuffer.addFromWithRamp(channel, writePosition, bufferData, bufferLength, oldFeedback, feedback);
		}
		else
		{
			const int bufferRemaining = delayBufferLength - writePosition;
			//calculate the gain based on the feedback knob and writeposition within the buffer
			const float midGain = oldFeedback + ((feedback - oldFeedback / bufferLength) * (bufferRemaining / bufferLength));

			reverseBuffer.addFromWithRamp(channel, writePosition, bufferData, bufferRemaining, oldFeedback, midGain);
			reverseBuffer.addFromWithRamp(channel, 0, bufferData, bufferLength - bufferRemaining, midGain, feedback);
		}
	}
	else
	{
		if (delayBufferLength > bufferLength + writePosition)
		{
			delayBuffer.addFromWithRamp(channel, writePosition, bufferData, bufferLength, oldFeedback, feedback);
		}
		else
		{
			const int bufferRemaining = delayBufferLength - writePosition;
			//calculate the gain based on the feedback knob and writeposition within the buffer
			const float midGain = oldFeedback + ((feedback - oldFeedback / bufferLength) * (bufferRemaining / bufferLength));

			delayBuffer.addFromWithRamp(channel, writePosition, bufferData, bufferRemaining, oldFeedback, midGain);
			delayBuffer.addFromWithRamp(channel, 0, bufferData, bufferLength - bufferRemaining, midGain, feedback);
		}
	}
}
//==============================================================================
bool SimpleDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SimpleDelayAudioProcessor::createEditor()
{
    return new SimpleDelayAudioProcessorEditor (*this, parameters, drawBufferIsFull);
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
