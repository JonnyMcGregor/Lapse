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
												1.0f,              // maximum value
												0.5f),             // default value
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
	pitchShiftBuffer.setSize(numberInputChannels, samplesPerBlock/2);
	/*
		The ProcessSpec is a struct which contains information on the audio 
		that gets passed to a dsp object's prepare() function.
	*/
	
	delayContainer.initialise(sampleRate, samplesPerBlock, delayBuffer.getNumSamples());

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
       	delayContainer.fillDryBuffer(channel, buffer, dryBuffer);
		delayContainer.fillDelayBuffer(channel, buffer, delayBuffer);
		if (isReverseEffect)
		{
			delayContainer.reverseDelayBuffer(channel, reverseBuffer, delayBuffer);
			delayContainer.initialDelayEffect(channel, buffer, reverseBuffer, delayTime);
			delayContainer.mixBuffers(channel, buffer, dryBuffer, *mixParameter);
			delayContainer.feedbackDelay(channel, buffer, reverseBuffer, oldFeedback, feedback);
		}
		else
		{
			delayContainer.initialDelayEffect(channel, buffer, delayBuffer, delayTime);
			delayContainer.mixBuffers(channel, buffer, dryBuffer, *mixParameter);
			delayContainer.feedbackDelay(channel, buffer, delayBuffer, oldFeedback, feedback);
		}
		
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
