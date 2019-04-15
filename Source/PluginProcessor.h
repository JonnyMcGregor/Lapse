/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

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

	//==============================================================================
	
	// Delay Functions:

    void fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength,
                         const float* bufferData, const float* delayBufferData);

    void getFromDelayBuffer(AudioBuffer<float> &buffer, int channel, const int bufferLength, const int delayBufferLength,
                            const float* bufferData, const float* delayBufferData, int delayTime);

    void feedbackDelay(int channel, const int bufferLength, const int delayBufferLength,
                       const float* dryBuffer);

    void fillDryBuffer(int channel, const int bufferLength, const float* bufferData);

private:

    AudioBuffer<float> mDelayBuffer; //Buffer to hold delayed audio content
    AudioBuffer<float> mDryBuffer; //Buffer to hold dry content for mix
    
	int mWritePosition { 0 }; //Stores the index position of the delay buffer to manage the overlap when writing to output buffer
	int mSampleRate  { 44100 };
	
	//Parameters values that correspond to knobs in UI
	float* mixParameter = 0;
	float* delayParameter = 0;
	float* feedbackParameter = 0;

	AudioProcessorValueTreeState parameters; //means of connecting dsp params to ui elements


    //==============================================================================
	//float gain = 0;
	//float delayGain = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayAudioProcessor)
};
