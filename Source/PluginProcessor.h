/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayContainer.h"

//==============================================================================
/**
*/
class LapseAudioProcessor  : public AudioProcessor

{
public:
    //==============================================================================
    LapseAudioProcessor();
    ~LapseAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
	void calculateNoteLengths();
	void panAudio(int channel, AudioBuffer<float> audioBuffer, float panValue);
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


private:

	DelayContainer delayContainer = DelayContainer(&writePosition);
	AudioBuffer<float> delayBuffer, dryBuffer, reverseBuffer;

	float* mixParameter = 0;
	float* delayParameter = 0;
	float* feedbackParameter = 0;
	float* reverseParameter = 0;
	float* panParameter = 0;
	int writePosition = 0;

	float oldFeedback = 0;

	const double pi = 3.141592654;
	
	float wholeNoteInSeconds, halfNoteInSeconds, quarterNoteInSeconds, 
		  eighthNoteInSeconds, sixteenthNoteInSeconds, thirtySecondNoteInSeconds;

	AudioPlayHead* playHead;
	AudioPlayHead::CurrentPositionInfo playposinfo;

	ChangeBroadcaster firstBeatOfBar;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LapseAudioProcessor)
};
