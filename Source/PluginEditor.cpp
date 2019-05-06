/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayAudioProcessorEditor::SimpleDelayAudioProcessorEditor (SimpleDelayAudioProcessor& p, AudioProcessorValueTreeState& s, ChangeBroadcaster& b)
    : AudioProcessorEditor (&p), processor (p), state (s), broadcaster(b)
{
    //Set up Labels
	initialiseLabel(lapseTitle, "lapse", Colour::fromRGB(239, 152, 24), 22);
	initialiseLabel(mixLabel, "mix", Colour::fromRGB(239, 152, 24), 18);
	initialiseLabel(feedbackLabel, "feedback", Colour::fromRGB(239, 152, 24), 18);
	initialiseLabel(delayTimeLabel, "delay time", Colour::fromRGB(239, 152, 24), 18);
	initialiseLabel(frequencyLabel, "freq", Colour::fromRGB(158, 202, 219), 15);
	initialiseLabel(resonanceLabel, "res", Colour::fromRGB(158, 202, 219), 15);
	//Set up filter menu
	initialiseFilterMenu();
	//Set up knobs
	initialiseKnob(mixKnob, Colour::fromRGB(239, 152, 24), Colour::fromRGB(244, 174, 81));
	initialiseKnob(delayTimeKnob, Colour::fromRGB(239, 152, 24), Colour::fromRGB(244, 174, 81));
	initialiseKnob(feedbackKnob, Colour::fromRGB(239, 152, 24), Colour::fromRGB(244, 174, 81));
	initialiseKnob(frequencyKnob, Colour::fromRGB(122, 184, 244), Colour::fromRGB(158, 202, 219));
	initialiseKnob(resonanceKnob, Colour::fromRGB(122, 184, 244), Colour::fromRGB(158, 202, 219));
	//Initialise Button
	reverseButton.setImages(title, titleDown, titleDown, nullptr, titleDown, nullptr, nullptr, nullptr);
	reverseButton.setClickingTogglesState(true);
	reverseButton.setColour(DrawableButton::ColourIds::backgroundOnColourId, Colour::fromRGB(249, 212, 157));
	//Set up attachments
	setUpAttachments();
	
	//Make GUI elements visible
	addAndMakeVisible(mixLabel);
	addAndMakeVisible(delayTimeLabel);
	addAndMakeVisible(feedbackLabel);
	addAndMakeVisible(frequencyLabel);
	addAndMakeVisible(resonanceLabel);

	addAndMakeVisible(mixKnob);
	addAndMakeVisible(delayTimeKnob);
	addAndMakeVisible(feedbackKnob);
	addAndMakeVisible(frequencyKnob);
	addAndMakeVisible(resonanceKnob);

	addAndMakeVisible(filterMenu);
	addAndMakeVisible(lapseTitle);

	addAndMakeVisible(reverseButton);

	frequencyKnob.setSkewFactorFromMidPoint(1000.0f);

	broadcaster.addChangeListener(this);

    setSize (500, 300);
}

SimpleDelayAudioProcessorEditor::~SimpleDelayAudioProcessorEditor()
{
	mixKnob.setLookAndFeel(nullptr);
	delayTimeKnob.setLookAndFeel(nullptr);
	feedbackKnob.setLookAndFeel(nullptr);
	frequencyKnob.setLookAndFeel(nullptr);
	resonanceKnob.setLookAndFeel(nullptr);
}
//==============================================================================
void SimpleDelayAudioProcessorEditor::initialiseLabel(Label& labelName, String text, Colour colour, float fontSize)
{
	Font font = Font(fontSize, Font::plain);
	labelName.setText(text, dontSendNotification);
	labelName.setJustificationType(Justification::centred);
	labelName.setColour(Label::ColourIds::textColourId, colour);
	labelName.setFont(font);
}
//==============================================================================

void SimpleDelayAudioProcessorEditor::initialiseFilterMenu()
{
	filterMenu.setJustificationType(Justification::centred);
	filterMenu.setColour(ComboBox::ColourIds::outlineColourId, Colour::fromRGB(122, 184, 244));
	filterMenu.setColour(ComboBox::ColourIds::arrowColourId, Colour::fromRGB(122, 184, 244));
	filterMenu.setColour(ComboBox::ColourIds::textColourId, Colour::fromRGB(122, 184, 244));
	filterMenu.setColour(ComboBox::ColourIds::backgroundColourId, Colour::fromRGB(249, 212, 157));
	filterMenu.setColour(ComboBox::ColourIds::focusedOutlineColourId, Colour::fromRGB(249, 212, 157));
	filterMenu.setColour(ComboBox::ColourIds::buttonColourId, Colour::fromRGB(249, 212, 157));

	filterMenu.addItem("LPF", 1);
	filterMenu.addItem("BPF", 2);
	filterMenu.addItem("HPF", 3);
}
//==============================================================================
void SimpleDelayAudioProcessorEditor::initialiseKnob(Slider& knob, Colour colour1, Colour colour2)
{
	knob.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	knob.setTextBoxStyle(Slider::NoTextBox, false, 50, 15);
	knob.setLookAndFeel(&lf);
	knob.setColour(Slider::ColourIds::textBoxTextColourId, Colours::aliceblue);
	knob.setColour(Slider::ColourIds::rotarySliderFillColourId, colour1);
	knob.setColour(Slider::ColourIds::rotarySliderOutlineColourId, colour2);
	knob.setColour(Slider::ColourIds::thumbColourId, colour1);
	knob.setColour(Slider::ColourIds::thumbFillColourId, colour2);
}

void SimpleDelayAudioProcessorEditor::setUpAttachments()
{
	mixAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "mix", mixKnob));
	delayTimeAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "delayTime", delayTimeKnob));
	feedbackAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "feedback", feedbackKnob));
	freqAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "frequency", frequencyKnob));
	resonanceAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "resonance", resonanceKnob));
	filterMenuAttachment.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(state, "filterMenu", filterMenu));
	reverseAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(state, "isReversing", reverseButton));
}

//==============================================================================
void SimpleDelayAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll(Colour::fromRGB(249, 212, 157));
	g.setColour(Colour::fromRGB(239, 152, 24));
	g.drawLine(Line<float>((getWidth() * 0.6), getHeight() * 0.25, getWidth() * 0.6, getHeight() * 0.666), 0.5);
	//g.drawRoundedRectangle(getWidth() * 0.25 - 27.5, getHeight() * 0.125 - 20, 120, 40, 10, 0.5);
	drawWaveform(g);
}

void SimpleDelayAudioProcessorEditor::resized()
{
	auto r = getLocalBounds();
	mixKnob.setBounds((getWidth() * 0.25f) - 30, (getHeight() * 0.75) - 10, 60, 60);
	delayTimeKnob.setBounds((getWidth() * 0.50f) - 30, (getHeight() * 0.75) - 10, 60, 60);
	feedbackKnob.setBounds((getWidth() * 0.75f) - 30, (getHeight() * 0.75) - 10, 60, 60);

	mixLabel.setBounds((getWidth() * 0.25f) - 40, (getHeight() * 0.75) + 20, 80, 60);
	delayTimeLabel.setBounds((getWidth() * 0.50f) - 40, (getHeight() * 0.75) + 20, 80, 60);
	feedbackLabel.setBounds((getWidth() * 0.75f) - 40, (getHeight() * 0.75) + 20, 80, 60);

	filterMenu.setBounds((getWidth() * 0.75) - 40, getHeight() * 0.333, 80, 30);

	frequencyKnob.setBounds((getWidth() * 0.666) - 5, getHeight() * 0.5 - 10, 50, 50);
	resonanceKnob.setBounds((getWidth() * 0.666) + 40, getHeight() * 0.5 - 10, 50, 50);

	frequencyLabel.setBounds((getWidth() * 0.666) - 10, (getHeight() * 0.5) - 10, 60, 100);
	resonanceLabel.setBounds((getWidth() * 0.666) + (resonanceKnob.getWidth() - 15), (getHeight() * 0.5) - 10, 60, 100);

	reverseButton.setBounds(getWidth() * 0.25 - 27.5, getHeight() * 0.125 - 20, 97, 36);

	//lapseTitle.setBounds(getWidth() * 0.25, (getHeight() * 0.125) - 30, 60, 60);
}

void SimpleDelayAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{
	repaint();
}

float SimpleDelayAudioProcessorEditor::scale(float bufferData, float bufferMin, float bufferMax, float areaMin, float areaMax)
{
	long double percentage = (bufferData - bufferMin) / (bufferMin - bufferMax);
	return (percentage) * (areaMin - areaMax) + areaMin;
}

void SimpleDelayAudioProcessorEditor::drawWaveform(Graphics& g)
{
	Rectangle<float> waveformArea = Rectangle<float>(getWidth() * 0.1, getHeight() * 0.25, getWidth() * 0.48, getHeight() * 0.5);
	
	float scaledAmplitude;

	g.setColour(Colour::fromRGB(239, 152, 24));
	
	for (int i = 0; i < waveformArea.getWidth() - 1; i++)
	{
		scaledAmplitude = scale(processor.audioDataToDraw[i], -0.8f, 0.8f, waveformArea.getY(), waveformArea.getY() + waveformArea.getHeight());
		g.drawVerticalLine(waveformArea.getX() + i, abs(scaledAmplitude), (waveformArea.getY() + waveformArea.getHeight()) - (abs(scaledAmplitude) - waveformArea.getY()));
	}
	g.setColour(Colour::fromRGB(122, 184, 244));

	for (int i = 0; i < waveformArea.getWidth() - 1; i++)
	{
		scaledAmplitude = scale(processor.delayAudioDataToDraw[i], -0.6f, 0.6f, waveformArea.getY(), waveformArea.getY() + waveformArea.getHeight());
		g.drawVerticalLine(waveformArea.getX() + i, jmin(scaledAmplitude, waveformArea.getY() + (waveformArea.getHeight() * 0.5f)), jmax(scaledAmplitude, waveformArea.getY() + (waveformArea.getHeight() * 0.5f)));
	}
}



