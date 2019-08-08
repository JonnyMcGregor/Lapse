/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LapseAudioProcessor::LapseAudioProcessor()
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
																   2000.0f,
																   1000.0f),
							 std::make_unique<AudioParameterFloat>("feedback",
																  "Feedback",
																   0,
																   1.0,
																   0.5),
							 std::make_unique<AudioParameterFloat>("panPosition",
																   "Pan",
																	0,
																	1,
																	0.5),
							 std::make_unique<AudioParameterBool>("isReversing",
																  "Is Reversing",
																  false,
																  "isReversing"),
							 std::make_unique<AudioParameterBool>("quantiseDelayTime",
																  "Quantise Delay Time",
																  false),
							 std::make_unique<AudioParameterFloat>("timerValue",
																	"Node Change Time",
																	1,
																	5,
																	1
																  )						 
						   })

#endif
{
	mixParameter = parameters.getRawParameterValue("mix");
	delayParameter = parameters.getRawParameterValue("delayTime");
	feedbackParameter = parameters.getRawParameterValue("feedback");
	reverseParameter = parameters.getRawParameterValue("isReversing");
	panParameter = parameters.getRawParameterValue("panPosition");
	timeModeParameter = parameters.getRawParameterValue("quantiseDelayTime");
	timerInterval = parameters.getRawParameterValue("timerValue");
}

LapseAudioProcessor::~LapseAudioProcessor()
{
}

//==============================================================================
const String LapseAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LapseAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LapseAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LapseAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LapseAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LapseAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LapseAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LapseAudioProcessor::setCurrentProgram (int index)
{
}

const String LapseAudioProcessor::getProgramName (int index)
{
    return {};
}

void LapseAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void LapseAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	const int numberInputChannels = getTotalNumInputChannels();
	const int delayBufferSize = 2.0f * (sampleRate + samplesPerBlock);

	delayBuffer.setSize(numberInputChannels, delayBufferSize);
	dryBuffer.setSize(numberInputChannels, samplesPerBlock);
	reverseBuffer.setSize(numberInputChannels, delayBufferSize);

	delayBuffer.clear();
	dryBuffer.clear();
	reverseBuffer.clear();

	delayContainer.initialise(sampleRate, samplesPerBlock, delayBufferSize);

	playHead = getPlayHead();
	if (playHead != nullptr)
	{
		playHead->getCurrentPosition(playposinfo);
		bpm = playposinfo.bpm;
		timeSigNumerator = playposinfo.timeSigNumerator;
		currentBeat = playposinfo.ppqPosition;
		calculateNoteLengths();
	}
	startTimer(halfNoteInSeconds*1000);
}

void LapseAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LapseAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void LapseAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	//get bpm and time info from daw
	playHead = getPlayHead();
	if (playHead != nullptr)
	{
		playHead->getCurrentPosition(playposinfo);
		bpm = playposinfo.bpm;
		timeSigNumerator = playposinfo.timeSigNumerator;
		currentBeat = playposinfo.ppqPosition;
		calculateNoteLengths();
	}
	

	const int bufferLength = buffer.getNumSamples();
	const int delayBufferLength = delayBuffer.getNumSamples();

	float delayTime = *delayParameter;
	float feedback = *feedbackParameter;
	float panValue = *panParameter;
	bool isReverseEffect = *reverseParameter;
	
	//Delay Processing:
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
		
		panAudio(channel, buffer, panValue);
		oldFeedback = feedback;
    }
	//update writePosition for delay processing
	writePosition += bufferLength;
	writePosition %= delayBufferLength;
}

void LapseAudioProcessor::calculateNoteLengths()
{
	if (playposinfo.timeSigDenominator % 2 == 0)
	{
		quarterNoteInSeconds = 60 / bpm;
		halfNoteInSeconds = quarterNoteInSeconds * 2;
		oneBarInSeconds = halfNoteInSeconds * 2;
		twoBarsInSeconds = oneBarInSeconds * 2;
		eighthNoteInSeconds = quarterNoteInSeconds * 0.5;
		sixteenthNoteInSeconds = eighthNoteInSeconds * 0.5;
		thirtySecondNoteInSeconds = sixteenthNoteInSeconds * 0.5;
	}
}

void LapseAudioProcessor::panAudio(int channel, AudioBuffer<float> audioBuffer, float panValue)
{
	for (int sample = 0; sample < audioBuffer.getNumSamples(); sample++)
	{
		if (channel == 0)
			audioBuffer.setSample(channel, sample, audioBuffer.getSample(channel, sample) * cos(panValue*pi/2));
		else
			audioBuffer.setSample(channel, sample, audioBuffer.getSample(channel, sample) * sin(panValue*pi/2));
	}
}

void LapseAudioProcessor::timerCallback()
{
	if (!isFirstTimeOpeningEditor)
	{
		Node *previousNode = &panNodes[currentDelayNode];

		if (oldTimerValue != *timerValues[(int)*timerInterval])
		{
			startTimer(*timerValues[(int)*timerInterval] * 1000);
			oldTimerValue = *timerValues[(int)*timerInterval];
		}

		changeCurrentDelayNode();

		if (previousNode != &panNodes[currentDelayNode])
		{
			updatePanParameter();
			updateMixParameter();
			updateFeedbackParameter();
			updateDelayTimeParameter();
		}
	}

	firstBeatOfBar.sendChangeMessage();
}

void LapseAudioProcessor::changeCurrentDelayNode()
{
	if (panNodes[currentDelayNode].isDelayNode)
	{
		panNodes[currentDelayNode].isDelayNode = false;
		timeNodes[currentDelayNode].isDelayNode = false;
	}

	if (currentDelayNode < numberOfVisibleNodes && numberOfVisibleNodes > 1)
	{
		currentDelayNode++;
		if (currentDelayNode == numberOfVisibleNodes)
		{
			currentDelayNode = 0;
		}
	}

	else
		currentDelayNode = 0;

	panNodes[currentDelayNode].isDelayNode = true;
	timeNodes[currentDelayNode].isDelayNode = true;
}

void LapseAudioProcessor::updatePanParameter()
{
	float pan = jmap(panNodes[currentDelayNode].getXPosition(), 30.0f, 385.0f, 0.0f, 1.0f);
	parameters.getParameter("panPosition")->beginChangeGesture();
	parameters.getParameter("panPosition")->setValueNotifyingHost(pan);
	parameters.getParameter("panPosition")->endChangeGesture();
}

void LapseAudioProcessor::updateMixParameter()
{
	float mix = jmap(panNodes[currentDelayNode].getYPosition(), 333.0f, 45.0f, 0.0f, 1.0f);
	parameters.getParameter("mix")->beginChangeGesture();
	parameters.getParameter("mix")->setValueNotifyingHost(mix);
	parameters.getParameter("mix")->endChangeGesture();
}

void LapseAudioProcessor::updateFeedbackParameter()
{
	float feedback = jmap(panNodes[currentDelayNode].getYPosition(), 333.0f, 45.0f, 0.0f, 1.0f);
	parameters.getParameter("feedback")->beginChangeGesture();
	parameters.getParameter("feedback")->setValueNotifyingHost(feedback);
	parameters.getParameter("feedback")->endChangeGesture();
}

void LapseAudioProcessor::updateDelayTimeParameter()
{
	float delayTime = jmap(timeNodes[currentDelayNode].getXPosition(), 415.0f, 770.0f, 0.0f, 1.0f);
	parameters.getParameter("delayTime")->beginChangeGesture();
	parameters.getParameter("delayTime")->setValueNotifyingHost(delayTime);
	parameters.getParameter("delayTime")->endChangeGesture();
}

//==============================================================================
bool LapseAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* LapseAudioProcessor::createEditor()
{
    return new LapseAudioProcessorEditor (*this, parameters, firstBeatOfBar);
}

//==============================================================================
void LapseAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LapseAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LapseAudioProcessor();
}
