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
#include "LapseLookAndFeel.h"
#include <vector>
#include <cmath>

//==============================================================================
/**
*/
class LapseAudioProcessorEditor  : public AudioProcessorEditor, public ChangeListener
{
public:
    LapseAudioProcessorEditor (LapseAudioProcessor&, AudioProcessorValueTreeState&, ChangeBroadcaster&);
    ~LapseAudioProcessorEditor();

	void setUpAttachments();

    //==============================================================================
	void resized() override;
	void paint (Graphics&) override;

	void drawStaticUIElements(Graphics&);

	void mouseDoubleClick(const MouseEvent&) override;
	void mouseDrag(const MouseEvent&) override;
	
	void selectNodeForMovement(const MouseEvent&);
	void updateNodePosition(const MouseEvent &m, Node&);
	void keepNodeInField(float&, float&, Node selecedNode);
	
	void updateFeedbackParameter();
	void updateMixParameter();
	void updatePanParameter();
	void updateDelayTimeParameter();

	float quantisePosition(float position, float noteLengthInMS);

	void changeCurrentDelayNode();

	void drawQuantiseGrid(Graphics&);
	void drawNodeConnectorLines(Graphics&, int i, std::vector<Node>&);
	void drawBorderOnSelectedNode(Graphics&, Node node);
	
	void changeListenerCallback(ChangeBroadcaster *source) override;

	std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> timeModeAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> nodeTimingBoxAttachment;

	/*
		Unused functions (May be reintroduced)
		
		
	*/
private:
	ChangeBroadcaster& broadcaster;
	
	Font largeFont = Font(Typeface::createSystemTypefaceFor(BinaryData::RobotoThin_ttf, BinaryData::RobotoThin_ttfSize));
	Font mediumFont = Font(Typeface::createSystemTypefaceFor(BinaryData::RobotoThin_ttf, BinaryData::RobotoThin_ttfSize));
	Font smallFont = Font(Typeface::createSystemTypefaceFor(BinaryData::RobotoThin_ttf, BinaryData::RobotoThin_ttfSize));
	
	Colour textColour = Colour::fromRGB(114, 114, 114);
	
	ToggleButton quantiseButton;

	ComboBox nodeTimingBox;

	LapseLookAndFeel lf;

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

	int maximumNodeSize = 80;
	int defaultNodeSize = 20;
	int numberOfVisibleNodes = 0;
	int currentDelayNode = 0;
	ModifierKeys modKeys;

	float feedback = 0;
	float pan = 0;
	float mix = 0;
	float delayTime = 0;

	std::vector<Node> panNodes;
	std::vector<Node> timeNodes;
	Node *selectedNode = nullptr;
	

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LapseAudioProcessorEditor)
};
