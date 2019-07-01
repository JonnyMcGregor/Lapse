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
#include <vector>

//==============================================================================
/**
*/
class LapseAudioProcessorEditor  : public AudioProcessorEditor, public ChangeListener
{
public:
    LapseAudioProcessorEditor (LapseAudioProcessor&, AudioProcessorValueTreeState&, ChangeBroadcaster&);
    ~LapseAudioProcessorEditor();

    //==============================================================================
	void setUpAttachments();
	void paint (Graphics&) override;
	void mouseDoubleClick(const MouseEvent&) override;
	void mouseDrag(const MouseEvent&) override;
	
	void selectNodeForMovement(const MouseEvent&);
	//void updateNodeSize(const MouseEvent &m, Node&);
	void updateNodePosition(const MouseEvent &m, Node&);

	void updateFeedbackParameter();
	void updatePanParameter();
	void updateMixParameter();
	void updateDelayTimeParameter();

	void changeCurrentDelayNode();

	void drawNodeConnectorLines(Graphics&, int i, std::vector<Node>&);
    void resized() override;

	float multiplyWindowWidth(float multiplyValue);
	float multiplyWindowHeight(float multiplyValue);

	void changeListenerCallback(ChangeBroadcaster *source) override;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> panAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> nodeCountAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> reverseAttachment;

private:
	ChangeBroadcaster& broadcaster;
	Font largeFont = Font(Typeface::createSystemTypefaceFor(BinaryData::RobotoThin_ttf, BinaryData::RobotoThin_ttfSize));
	Font mediumFont = Font(Typeface::createSystemTypefaceFor(BinaryData::RobotoThin_ttf, BinaryData::RobotoThin_ttfSize));
	Font smallFont = Font(Typeface::createSystemTypefaceFor(BinaryData::RobotoThin_ttf, BinaryData::RobotoThin_ttfSize));
	
	Colour textColour = Colour::fromRGB(114, 114, 114);
	
	Slider mixValue;
	Slider feedbackValue;
	Slider delayTimeValue;
	Slider panValue;

	ToggleButton reverseButton;

	Label titleLabel;

    LapseAudioProcessor& processor;

	AudioProcessorValueTreeState& state;

	Rectangle<float> nodeField;
	Rectangle<float> panNodeField;
	Rectangle<float> timeNodeField;
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

	
	/*float diameter[10] {30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
	float nodeX[10];
	float nodeY[10];*/
	int maximumNodeSize = 80;
	int defaultNodeSize = 20;
	int numberOfVisibleNodes = 0;
	//int selectedNode = 0;
	int currentDelayNode = 0;
	ModifierKeys modKeys;

	float feedback = 0;
	float pan = 0;
	float mix = 0;
	float delayTime = 0;

	std::vector<Node> panNodes;
	std::vector<Node> timeNodes;
	Node *selectedNode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LapseAudioProcessorEditor)
};
