/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayAudioProcessorEditor::SimpleDelayAudioProcessorEditor (SimpleDelayAudioProcessor& p, AudioProcessorValueTreeState& s)
    : AudioProcessorEditor (&p), processor (p), state (s)
{
    //Set up Labels
	mixLabel.setText("mix", dontSendNotification);
	mixLabel.setJustificationType(Justification::centred);
	mixLabel.setColour(Label::ColourIds::textColourId, Colour::fromRGB(239, 152, 24));

	delayTimeLabel.setText("delay time", dontSendNotification);
	delayTimeLabel.setJustificationType(Justification::centred);
	delayTimeLabel.setColour(Label::ColourIds::textColourId, Colour::fromRGB(239, 152, 24));

	feedbackLabel.setText("feedback", dontSendNotification);
	feedbackLabel.setJustificationType(Justification::centred);
    feedbackLabel.setColour(Label::ColourIds::textColourId, Colour::fromRGB(239, 152, 24));

	//Make GUI elements visible
	addAndMakeVisible(mixLabel);
	addAndMakeVisible(delayTimeLabel);
	addAndMakeVisible(mixSlider);
	addAndMakeVisible(delayTimeSlider);
	addAndMakeVisible(feedbackLabel);
	addAndMakeVisible(feedbackSlider);

	//Set up sliders
	mixSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	mixSlider.setTextBoxStyle(Slider::NoTextBox, false, 50, 15);
	mixSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colours::aliceblue);
	mixSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colour::fromRGB(239,152,24));
	mixSlider.setColour(Slider::ColourIds::rotarySliderOutlineColourId, Colour::fromRGB(244, 174, 81));
	mixSlider.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(239, 152, 24));
	
	delayTimeSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	delayTimeSlider.setTextBoxStyle(Slider::NoTextBox, false, 50, 15);
	delayTimeSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colours::aliceblue);
	delayTimeSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colour::fromRGB(239, 152, 24));
	delayTimeSlider.setColour(Slider::ColourIds::rotarySliderOutlineColourId, Colour::fromRGB(244, 174, 81));
	delayTimeSlider.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(239, 152, 24));

	feedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	feedbackSlider.setTextBoxStyle(Slider::NoTextBox, false, 50, 15);
	feedbackSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colours::aliceblue);
	feedbackSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colour::fromRGB(239, 152, 24));
	feedbackSlider.setColour(Slider::ColourIds::rotarySliderOutlineColourId, Colour::fromRGB(244, 174, 81));
	feedbackSlider.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(239, 152, 24));

	//Setup Attachments
	mixAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "mix", mixSlider));
	delayTimeAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "delayTime", delayTimeSlider));
	feedbackAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "feedback", feedbackSlider));
    setSize (500, 200);
}

SimpleDelayAudioProcessorEditor::~SimpleDelayAudioProcessorEditor()
{
}

//==============================================================================
void SimpleDelayAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll(Colour::fromRGB(249, 212, 157));
    g.setColour (Colours::white);
}

void SimpleDelayAudioProcessorEditor::resized()
{
	auto r = getLocalBounds();
    mixSlider.setBounds((getWidth() * 0.25f) - 40, (getHeight() * 0.5) - 40 , 80, 80);
	delayTimeSlider.setBounds((getWidth() * 0.50f) - 40, (getHeight() * 0.5) - 40, 80, 80);
	feedbackSlider.setBounds((getWidth() * 0.75f) - 40, (getHeight() * 0.5) - 40, 80, 80);
	
	mixLabel.setBounds((getWidth() * 0.25f) - 15, (getHeight() * 0.5f) + 5, 30, 80);
	delayTimeLabel.setBounds((getWidth() * 0.50f) - 40, (getHeight() * 0.5f) + 5, 80, 80);
	feedbackLabel.setBounds((getWidth() * 0.75f) - 40, (getHeight() * 0.5f) + 5, 80, 80);	
}
