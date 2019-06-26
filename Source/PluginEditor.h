/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Node.h"

//==============================================================================
/**
*/
class LapseAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    LapseAudioProcessorEditor (LapseAudioProcessor&, AudioProcessorValueTreeState&);
    ~LapseAudioProcessorEditor();

    //==============================================================================
	void setUpAttachments();
	void paint (Graphics&) override;
	void createNode();
	void mouseDoubleClick(const MouseEvent&);
	void mouseDrag(const MouseEvent&);
	
	void selectNodeForMovement(const MouseEvent&);
	void updateNodeSize(const MouseEvent &m);
	void updateNodePosition(const MouseEvent &m);
	
	void updateFeedbackParameter();
	void updatePanParameter();
	void updateDelayTimeParameter();

	void drawNodes(Graphics&);
    void resized() override;

	float fractionOfWindowWidth(float fraction);
	float fractionOfWindowHeight(float fraction);

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> reverseAttachment;

private:
    
	Font lapseFont = Font(Typeface::createSystemTypefaceFor(BinaryData::RobotoThin_ttf, BinaryData::RobotoThin_ttfSize));
	Colour textColour = Colour::fromRGB(114, 114, 114);
	
	Slider mixValue;
	Slider feedbackValue;
	Slider delayTimeValue;
	ToggleButton reverseButton;

	Label titleLabel;

    LapseAudioProcessor& processor;

	AudioProcessorValueTreeState& state;

	Rectangle<float> nodeField;
	
	Colour nodeColour[10]{ Colour::fromRGB(64, 94, 221), 
						   Colour::fromRGB(72, 181, 15),
						   Colour::fromRGB(239, 29, 129),
						   Colour::fromRGB(221, 162, 31),
						   Colour::fromRGB(239, 20, 20),
						   Colour::fromRGB(64, 94, 221),
						   Colour::fromRGB(72, 181, 15),
						   Colour::fromRGB(239, 29, 129),
						   Colour::fromRGB(221, 162, 31),
						   Colour::fromRGB(239, 20, 20)};

	
	float diameter[10] {30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
	float nodeX[10];
	float nodeY[10];
	int numberOfVisibleNodes = 0;
	int selectedNode = 0;
	int currentDelayNode = 0;
	ModifierKeys modKeys;

	float feedback = 0;
	float pan = 0;
	float delayTime = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LapseAudioProcessorEditor)
};
