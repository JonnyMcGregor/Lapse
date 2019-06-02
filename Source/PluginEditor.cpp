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
	initialiseLabel(lapseTitle, "lapse", primaryColour, 22);
	initialiseLabel(mixLabel, "mix", primaryColour, 18);
	initialiseLabel(feedbackLabel, "feedback", primaryColour, 18);
	initialiseLabel(delayTimeLabel, "delay time", primaryColour, 18);
	initialiseLabel(frequencyLabel, "freq", secondaryColour, 15);
	initialiseLabel(resonanceLabel, "res", secondaryColour, 15);
	//Set up filter menu
	initialiseFilterMenu();
	//Set up knobs
	initialiseKnob(mixKnob, primaryColour, primaryColourLighter);
	initialiseKnob(delayTimeKnob, primaryColour, primaryColourLighter);
	initialiseKnob(feedbackKnob, primaryColour, primaryColourLighter);
	initialiseKnob(frequencyKnob, secondaryColour, secondaryColourLighter);
	initialiseKnob(resonanceKnob, secondaryColour, secondaryColourLighter);
	//Initialise Button
	reverseButton.setImages(title, titleDown, titleDown, nullptr, titleDown, nullptr, nullptr, nullptr);
	reverseButton.setClickingTogglesState(true);
	reverseButton.setColour(DrawableButton::ColourIds::backgroundOnColourId, backgroundColour);
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

	//This skew factor makes the freq knob behave logarithmically
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
	filterMenu.setColour(ComboBox::ColourIds::outlineColourId, secondaryColour);
	filterMenu.setColour(ComboBox::ColourIds::arrowColourId, secondaryColour);
	filterMenu.setColour(ComboBox::ColourIds::textColourId, secondaryColour);
	filterMenu.setColour(ComboBox::ColourIds::backgroundColourId, backgroundColour);
	filterMenu.setColour(ComboBox::ColourIds::focusedOutlineColourId, secondaryColourLighter);
	filterMenu.setColour(ComboBox::ColourIds::buttonColourId, secondaryColourLighter);

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
	knob.setColour(Slider::ColourIds::rotarySliderFillColourId, colour1);
	knob.setColour(Slider::ColourIds::rotarySliderOutlineColourId, colour2);
	knob.setColour(Slider::ColourIds::thumbColourId, colour1);
	knob.setColour(Slider::ColourIds::thumbFillColourId, colour2);
}
//==============================================================================
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
float  SimpleDelayAudioProcessorEditor::fractionOfWindowWidth(float fraction)
{
	return getWidth() * fraction;
}

float  SimpleDelayAudioProcessorEditor::fractionOfWindowHeight(float fraction)
{
	return getHeight() * fraction;
}

//==============================================================================
void SimpleDelayAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll(backgroundColour);
	g.setColour(primaryColour);
	g.drawLine(Line<float>(fractionOfWindowWidth(0.60), fractionOfWindowHeight(0.25), fractionOfWindowWidth(0.60), fractionOfWindowHeight(0.66)), 0.5);
	drawWaveform(g);
}

//==============================================================================
void SimpleDelayAudioProcessorEditor::resized()
{
	mixKnob.setBounds(fractionOfWindowWidth(0.25f) - 30, fractionOfWindowHeight(0.75f) - 10, 60, 60);
	delayTimeKnob.setBounds(fractionOfWindowWidth(0.5f) - 30, fractionOfWindowHeight(0.75f) - 10, 60, 60);
	feedbackKnob.setBounds(fractionOfWindowWidth(0.75f) - 30, fractionOfWindowHeight(0.75f) - 10, 60, 60);

	mixLabel.setBounds(fractionOfWindowWidth(0.25f) - 40, fractionOfWindowHeight(0.75f) + 20, 80, 60);
	delayTimeLabel.setBounds(fractionOfWindowWidth(0.5f) - 40, fractionOfWindowHeight(0.75f) + 20, 80, 60);
	feedbackLabel.setBounds(fractionOfWindowWidth(0.75f) - 40, fractionOfWindowHeight(0.75f) + 20, 80, 60);

	filterMenu.setBounds(fractionOfWindowWidth(0.75f) - 40, fractionOfWindowHeight(0.33f), 80, 30);

	frequencyKnob.setBounds(fractionOfWindowWidth(0.66f) - 5, fractionOfWindowHeight(0.5f) - 10, 50, 50);
	resonanceKnob.setBounds(fractionOfWindowWidth(0.66f) + 40, fractionOfWindowHeight(0.5f) - 10, 50, 50);

	frequencyLabel.setBounds(fractionOfWindowWidth(0.66f) - 10, fractionOfWindowHeight(0.5f) - 10, 60, 100);
	resonanceLabel.setBounds(fractionOfWindowWidth(0.66f) + 35, fractionOfWindowHeight(0.5f) - 10, 60, 100);

	reverseButton.setBounds(fractionOfWindowWidth(0.25f) - 18, fractionOfWindowHeight(0.125f) - 20, 97, 36);
}

//==============================================================================
void SimpleDelayAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{
	repaint();
}

//=============================================================================
void SimpleDelayAudioProcessorEditor::drawWaveform(Graphics& g)
{
	Rectangle<float> waveformArea = Rectangle<float>(fractionOfWindowWidth(0.1), fractionOfWindowHeight(0.25), 
												     fractionOfWindowWidth(0.45), fractionOfWindowHeight(0.5));
	int indexValueScaled;
	float scaledAmplitude;
	float delayScaledAmplitude;
	Path waveform1;
	Path waveform2;
	
	waveform1.startNewSubPath(Point<float>(waveformArea.getX(), waveformArea.getCentreY()));
	waveform2.startNewSubPath(Point<float>(waveformArea.getX(), waveformArea.getCentreY()));


	for (int i = 0; i < processor.audioDataToDraw.size() - 1; i++)
	{
		indexValueScaled = jmap((float)i, 0.0f, processor.delayAudioDataToDraw.size() - 1.0f, waveformArea.getX(), waveformArea.getRight());
		scaledAmplitude = jmap(processor.audioDataToDraw[i], -0.6f, 0.6f, waveformArea.getY(), waveformArea.getBottom());
		delayScaledAmplitude = jmap(processor.delayAudioDataToDraw[i], -0.6f, 0.6f, waveformArea.getY(), waveformArea.getBottom());
		
		waveform1.lineTo(indexValueScaled, scaledAmplitude);
		waveform2.lineTo(indexValueScaled, delayScaledAmplitude);
	}

	waveform1.lineTo(waveformArea.getRight(), waveformArea.getCentreY());
	waveform2.lineTo(waveformArea.getRight(), waveformArea.getCentreY());
	
	g.setColour(secondaryColour);
	g.strokePath(waveform2, PathStrokeType (waveLineThickness), AffineTransform());
	g.setColour(primaryColour);
	g.strokePath(waveform1, PathStrokeType(waveLineThickness), AffineTransform());
}



