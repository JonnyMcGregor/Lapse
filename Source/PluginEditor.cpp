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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	mixLabel.setText("mix", dontSendNotification);
	mixLabel.setColour(Label::ColourIds::textColourId, Colours::aliceblue);
	delayTimeLabel.setText("delay time", dontSendNotification);
	delayTimeLabel.setColour(Label::ColourIds::textColourId, Colours::aliceblue);
	feedbackLabel.setText("feedback", dontSendNotification);
    feedbackLabel.setColour(Label::ColourIds::textColourId, Colours::aliceblue);

	addAndMakeVisible(mixLabel);
	addAndMakeVisible(delayTimeLabel);
	addAndMakeVisible(mixSlider);
	addAndMakeVisible(delayTimeSlider);
	addAndMakeVisible(feedbackLabel);
	addAndMakeVisible(feedbackSlider);

	mixSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	mixSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 15);
	mixSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colours::aliceblue);

	delayTimeSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	delayTimeSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 15);
	delayTimeSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colours::aliceblue);

	feedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	feedbackSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 15);
	feedbackSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colours::aliceblue);

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
    // (Our component is opaque, so we must completely fill the background with a solid colour)	
	g.setGradientFill(ColourGradient(Colours::lightsteelblue, getWidth()/2, getHeight()/2, Colours::lightsteelblue.darker(), getWidth()*0.2, getHeight()*0.2, true));
	g.fillAll();
    g.setColour (Colours::white);

}

void SimpleDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	auto r = getLocalBounds();
    mixSlider.setBounds((getWidth()/5), 60, 100, 100);
	delayTimeSlider.setBounds(3*getWidth()/5, 60, 100, 100);
	feedbackSlider.setBounds(2 * getWidth() / 5, 60, 100, 100);
	
	mixLabel.setBounds((getWidth() / 5) + 30, 15, 50, 80);
	delayTimeLabel.setBounds((3 * getWidth() / 5)+ 10, 15, 80, 80);
	feedbackLabel.setBounds((2 * getWidth() / 5) + 15, 15, 80, 80);
	
}
