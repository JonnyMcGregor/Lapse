/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SimpleDelayAudioProcessorEditor  : public AudioProcessorEditor, public ChangeListener
{
public:
    SimpleDelayAudioProcessorEditor (SimpleDelayAudioProcessor&, AudioProcessorValueTreeState&, ChangeBroadcaster&);
    ~SimpleDelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void initialiseLabel(Label& labelName, String text, Colour colour, float fontSize);
	void initialiseFilterMenu();
	void initialiseKnob(Slider& knob, Colour colour1, Colour colour2);
	void setUpAttachments();
	void drawWaveform(Graphics&);
	void changeListenerCallback(ChangeBroadcaster *source) override;
	float scale(float A, float A1, float A2, float Min, float Max);

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> filterMenuAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> reverseAttachment;

	

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleDelayAudioProcessor& processor;
    //Manage parameter state to send to processor
	AudioProcessorValueTreeState& state;
	
	ChangeBroadcaster& broadcaster;

	//Visual GUI Components

	ComboBox filterMenu;
	Slider frequencyKnob;
	Slider resonanceKnob;

	Slider mixKnob;
	Slider delayTimeKnob;
	Slider feedbackKnob;
	
	DrawableButton reverseButton{ "reverseButton", DrawableButton::ButtonStyle::ImageRaw };

	Label lapseTitle;
	Label mixLabel;
	Label delayTimeLabel;
	Label feedbackLabel;
	Label frequencyLabel;
	Label resonanceLabel;

	LookAndFeel_V4 lf;

	ScopedPointer<Drawable> title = Drawable::createFromImageData(BinaryData::lapse_png, BinaryData::lapse_pngSize);
	ScopedPointer<Drawable> titleDown = Drawable::createFromImageData(BinaryData::lapseDown_png, BinaryData::lapseDown_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayAudioProcessorEditor)
};


