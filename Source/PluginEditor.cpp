/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LapseAudioProcessorEditor::LapseAudioProcessorEditor (LapseAudioProcessor& p, AudioProcessorValueTreeState& s)
    : AudioProcessorEditor (&p), processor (p), state (s)
{
	lapseFont.setExtraKerningFactor(0.5);
	lapseFont.setHeight(50);
	titleLabel.setFont(lapseFont);
	titleLabel.setText("lapse.", NotificationType::dontSendNotification);
	titleLabel.setJustificationType(Justification::centred);
	titleLabel.setColour(Label::ColourIds::textColourId, textColour);

	addAndMakeVisible(titleLabel);
	setUpAttachments();
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 500);
}

LapseAudioProcessorEditor::~LapseAudioProcessorEditor()
{
}
void LapseAudioProcessorEditor::setUpAttachments()
{
	mixAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "mix", mixValue));
	delayTimeAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "delayTime", delayTimeValue));
	feedbackAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "feedback", feedbackValue));
	reverseAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(state, "isReversing", reverseButton));
}

void LapseAudioProcessorEditor::resized()
{
	titleLabel.setBounds(0, fractionOfWindowHeight(2 / 3), getWidth(), fractionOfWindowHeight(1 / 3));
}

//==============================================================================
void LapseAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
	Rectangle<float> fontArea = Rectangle<float>(0, fractionOfWindowHeight(0.666), getWidth(),fractionOfWindowHeight(0.333));
    g.setColour (textColour);
    g.setFont (lapseFont);
    g.drawText ("lapse.", fontArea, Justification::centred);
	drawNodes(g);
}

void LapseAudioProcessorEditor::drawNodes(Graphics &g)
{
	for (int i = 0; i < nodeCount; i++)
	{
		Rectangle<float> nodeArea = Rectangle<float>(nodeX[i] - (diameter[i] / 2), nodeY[i] - (diameter[i] / 2), diameter[i], diameter[i]);
		Rectangle<float> gradientArea = Rectangle<float>(nodeX[i] - diameter[i], nodeY[i] - diameter[i], diameter[i] * 2, diameter[i] * 2);
		Point<float> endOfGradient = Point<float>(gradientArea.getX() + nodeArea.getWidth(), gradientArea.getY());
		ColourGradient gradient = ColourGradient(nodeColour[i], nodeArea.getCentre(), Colours::transparentWhite, endOfGradient, true);
		
		g.setColour(nodeColour[i]);
		g.drawEllipse(nodeArea, 2.0f);
		g.setGradientFill(gradient);
		g.fillEllipse(gradientArea);

		if (i >= 1)
		{
			Line<float> nodeConnector = Line<float>(nodeX[i - 1], nodeY[i - 1], nodeX[i], nodeY[i]);
			ColourGradient lineGradient = ColourGradient(nodeColour[i -1], nodeX[i - 1], nodeY[i - 1], nodeColour[i], nodeX[i], nodeY[i], false);
			g.setGradientFill(lineGradient);
			g.drawLine(nodeConnector, 2);
		}
	}
}

void LapseAudioProcessorEditor::mouseDoubleClick(const MouseEvent &m)
{
	if (nodeCount < 10)
	{
		nodeX[nodeCount] = m.getMouseDownX();
		nodeY[nodeCount] = m.getMouseDownY();
		createNode();
	}
}

void LapseAudioProcessorEditor::createNode()
{
	nodeCount++;
	repaint();
}

void LapseAudioProcessorEditor::mouseDrag(const MouseEvent &m)
{
	selectNodeForMovement(m);

	if (m.mods.isShiftDown())
	{
		updateNodeSize(m);
	}
	else
	{
		updateNodePosition(m);
	}
	
	repaint();
}


void LapseAudioProcessorEditor::selectNodeForMovement(const MouseEvent &m)
{
	for (int i = 0; i < nodeCount; i++)
	{
		if (m.getMouseDownX() < nodeX[i] + (diameter[i] / 2) && m.getMouseDownX() > nodeX[i] - (diameter[i] / 2) &&
			m.getMouseDownY() < nodeY[i] + (diameter[i] / 2) && m.getMouseDownY() > nodeY[i] - (diameter[i] / 2))
		{
			selectedNode = i;
		}
	}
}

void LapseAudioProcessorEditor::updateNodeSize(const MouseEvent &m)
{
	if (diameter[selectedNode] < 80)
	{
		diameter[selectedNode] = m.getDistanceFromDragStart();

		if (diameter[selectedNode] < 0)
		{
			diameter[selectedNode] = diameter[selectedNode] * -1;
		}
	}

	else 
	{
		diameter[selectedNode] = 79;
	}

	updateFeedbackParameter();
}

void LapseAudioProcessorEditor::updateFeedbackParameter()
{
	feedbackValue.setValue(jmap(diameter[currentDelayNode], 0.0f, 80.0f, 0.0f, 2.0f), dontSendNotification);
}

void LapseAudioProcessorEditor::updateNodePosition(const MouseEvent &m)
{
	nodeX[selectedNode] = m.getDistanceFromDragStartX() + m.getMouseDownX();
	nodeY[selectedNode] = m.getDistanceFromDragStartY() + m.getMouseDownY();
}

float  LapseAudioProcessorEditor::fractionOfWindowWidth(float fraction)
{
	return getWidth() * fraction;
}

float  LapseAudioProcessorEditor::fractionOfWindowHeight(float fraction)
{
	return getHeight() * fraction;
}

