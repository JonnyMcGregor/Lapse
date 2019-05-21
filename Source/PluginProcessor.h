/*
  ==============================================================================

    This is a delay plugin with mix, feedback, and delay time parameters.
    
    The algorithm was sourced from Daniel Walz's Tape Delay Plugin, can be found
    at "https://github.com/ffAudio/ffTapeDelay/".  

	Daniel Walz's method breaks the delay process into three functions:

	1. fillDelayBuffer() 
	2. getFromDelayBuffer()
	3. feedbackDelay()

	By keeping track of both a readPosition and a writePosition these three functions 
	can carry out the delay effect. 

	
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayContainer.h"
//==============================================================================
/**
*/
class SimpleDelayAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SimpleDelayAudioProcessor();
    ~SimpleDelayAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

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

	void updateFilter();
	//==============================================================================
	
	void fillDrawingBuffer(int channel, AudioBuffer<float> &dryBuffer, AudioBuffer<float> &buffer, const int bufferLength);

	std::array<float, 175> audioDataToDraw; //this takes a portion of the audio data to be drawn
	std::array<float, 175> delayAudioDataToDraw; //this takes a portion of delayed audio data to be drawn

private:

    AudioBuffer<float> delayBuffer; //Buffer to hold delayed audio content
    AudioBuffer<float> dryBuffer; //Buffer to hold dry content for mix
	AudioBuffer<float> reverseBuffer; //Buffer to hold reversed content

	int writePosition { 0 }; //Stores the index position of the delay buffer to manage the overlap when writing to output buffer
	int readPosition { 0 };
	int lastSampleRate  { 44100 };

	DelayContainer delayContainer = DelayContainer(&writePosition);

	
	//Parameters values that correspond to knobs in UI
	float* mixParameter = 0;
	float* delayParameter = 0;
	float* feedbackParameter = 0;
	float* frequencyParameter = 0;
	float* resonanceParameter = 0;
	float* reverseParameter = 0;

	const StringArray menuChoices{ "Low Pass", "Band Pass", "High Pass" };
	const StringArray reverseChoices{ "False", "True" };

	// State Variable Filter Object - this is instantiated through a ProcessorDuplicator as the filter processes in mono, the 
	// ProcessorDuplicator enables stereo processing of the DSP object
	dsp::ProcessorDuplicator<dsp::StateVariableFilter::Filter<float>, dsp::StateVariableFilter::Parameters<float>> stateVariableFilter;
	
	float oldFeedback = 0; //Used for calculating feedback gain, stores previous value of feedbackParameter in feedbackDelay()

	AudioProcessorValueTreeState parameters;

	ChangeBroadcaster drawBufferIsFull;

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayAudioProcessor)
};
