/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayContainer.h"
#include "Node.h"

//==============================================================================
/**
*/
class LapseAudioProcessor  : public AudioProcessor, public Timer

{
public:
    //==============================================================================
    LapseAudioProcessor();
    ~LapseAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
	void timerCallback() override;
	void changeCurrentDelayNode();


   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
	void calculateNoteLengths();
	void panAudio(int channel, AudioBuffer<float> audioBuffer, float panValue);
	float smoothParameterChange(float& currentValue, float& previousValue);
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	AudioProcessorValueTreeState parameters;

	double bpm = 0;
	int timeSigNumerator = 4;
	int currentBeat = 0;

	
	float twoBarsInSeconds, oneBarInSeconds, halfNoteInSeconds, quarterNoteInSeconds,
		eighthNoteInSeconds, sixteenthNoteInSeconds, thirtySecondNoteInSeconds;
	
	std::vector<Node> panNodes;
	std::vector<Node> timeNodes;

	void updateFeedbackParameter();
	void updateMixParameter();
	void updatePanParameter();
	void updateDelayTimeParameter();
	
	int currentDelayNode = 0;
	int numberOfVisibleNodes = 0;

	bool isFirstTimeOpeningEditor = true;

private:

	int lastSampleRate = 0;

	DelayContainer delayContainer = DelayContainer(&writePosition);
	AudioBuffer<float> delayBuffer, dryBuffer, reverseBuffer;

	float* mixParameter = 0;
	float* delayParameter = 0;
	float* feedbackParameter = 0;
	float* panParameter = 0;
	float* timeModeParameter = 0;
	float* timerInterval = 0;

	float *timerValues[4]{ &quarterNoteInSeconds,&halfNoteInSeconds, &oneBarInSeconds, &twoBarsInSeconds};

	float oldTimerValue = 0;
	int writePosition = 0;

	float oldFeedback = 0;
	float oldPan = 0;

	const double pi = 3.141592654;

	AudioPlayHead* playHead;
	AudioPlayHead::CurrentPositionInfo playposinfo;

	ChangeBroadcaster firstBeatOfBar;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LapseAudioProcessor)
};

//==================================================================================
//Unused Code (May reintroduce in a later version):
	//float* reverseParameter = 0;
