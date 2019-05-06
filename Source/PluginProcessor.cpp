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
											    1.0),
		  std::make_unique<AudioParameterFloat>("frequency",        // parameter ID
												"Cutoff Frequency", // parameter name
												20.0f,              // minimum value
												20000.0f,           // maximum value
												20000.0f),            // default value
		  std::make_unique<AudioParameterFloat>("resonance",
												"Resonance",
												1.0f,
												5.0f,
												2.0f),
		  std::make_unique<AudioParameterChoice>("filterMenu",  //ID
												 "Filter Type", //Name
												 menuChoices,   //Combobox Choices
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
    const int numberInputChannels = getTotalNumInputChannels();   //variable for number of input channels
    const int delayBufferSize = 2*(sampleRate + samplesPerBlock); //variable for buffer size
    lastSampleRate = sampleRate;									  //set sample rate to global variable
    delayBuffer.setSize(numberInputChannels, delayBufferSize);   //Setup parameters for delayBuffer
    dryBuffer.setSize(numberInputChannels, samplesPerBlock);     //Setup parameters for dryBuffer
	reverseBuffer.setSize(numberInputChannels, delayBufferSize); //Setup parameters for reverseBuffer

	//The ProcessSpec is a struct which contains information on the audio 
	//which gets passed to a dsp object's prepare() function.

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

	
    const int bufferLength = buffer.getNumSamples(); //local variable for buffersize
    const int delayBufferLength = delayBuffer.getNumSamples(); //local variable for length of delay buffer

	int delayTime = *delayParameter;
	float feedback = *feedbackParameter;
	bool isReverseEffect = *reverseParameter;


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* bufferData = buffer.getWritePointer(channel); //local pointer for audio data stored inside buffer
        float* delayBufferData = delayBuffer.getWritePointer(channel); //local pointer for delayed audio data
		
        fillDryBuffer(channel, bufferLength, bufferData);
        
		float* dryBufferData = dryBuffer.getWritePointer(channel);
		
        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);

		if (isReverseEffect == 1);
		{
			fillReverseBuffer(channel, delayBufferLength, bufferLength);
		}

		float* reverseBufferData = reverseBuffer.getWritePointer(channel);
        
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData, reverseBufferData, delayTime, isReverseEffect);
        
		fillDrawingBuffer(channel, dryBuffer, buffer, bufferLength);

		for (int sample = 0; sample < buffer.getNumSamples(); sample++)
		{									 /*----------------- DRY_MIX ---------------------- + ---------------- WET_MIX -------------- MEAN*/
			buffer.setSample(channel, sample, ((dryBufferData[sample] * (2.0 - *mixParameter)) + (bufferData[sample] * *mixParameter)) / 2.0f);
		}

		if (counter == 0 || counter % 8 == 0) 
		{
			feedbackDelay(channel, bufferLength, delayBufferLength, bufferData, oldFeedback, feedback, isReverseEffect);
		}

		if (counter > 8)
		{
			counter = 0;
		}
		counter++;
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
		indexValueScaled = scale(i, 0, bufferLength - 1, 0, audioDataToDraw.size() - 1);
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
//Copy delay buffer data into the main buffer according to the delay time parameter.
//This function creates a single delay in the output

void SimpleDelayAudioProcessor::getFromDelayBuffer(AudioBuffer<float> &buffer, int channel, const int bufferLength, const int delayBufferLength,
                                                   const float* bufferData, const float* delayBufferData, const float* reverseBufferData, int delayTime, bool isReverseEffect)
{
    int delayTimeSamples = lastSampleRate * delayTime / 1000;
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
//Feedback is achieved by copying the delay data back into the delay buffer with reduced gain
//The reduced gain amount is calculated using the feedback knob.
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
