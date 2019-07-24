/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LapseAudioProcessorEditor::LapseAudioProcessorEditor (LapseAudioProcessor& p, AudioProcessorValueTreeState& s, ChangeBroadcaster& b)
    : AudioProcessorEditor (&p), processor (p), state (s), broadcaster (b)
{
	b.addChangeListener(this);
	largeFont.setExtraKerningFactor(0.5);
	largeFont.setHeight(35);
	mediumFont.setExtraKerningFactor(0.25);
	mediumFont.setHeight(25);
	smallFont.setHeight(15);
	
	quantiseButton.setColour(ToggleButton::ColourIds::textColourId, textColour);
	quantiseButton.setColour(ToggleButton::ColourIds::tickColourId, textColour);
	quantiseButton.setColour(ToggleButton::ColourIds::tickDisabledColourId, textColour.darker());
	quantiseButton.setButtonText("Quantise");
	addAndMakeVisible(&quantiseButton);

	nodeTimingBox.setLookAndFeel(&lf);
	nodeTimingBox.setColour(ComboBox::ColourIds::outlineColourId, textColour);
	nodeTimingBox.setColour(ComboBox::ColourIds::backgroundColourId, Colours::white);
	nodeTimingBox.setColour(ComboBox::ColourIds::textColourId, textColour);
	nodeTimingBox.setColour(ComboBox::ColourIds::arrowColourId, textColour);

	nodeTimingBox.addItem("1/4", 1);
	nodeTimingBox.addItem("1/2", 2);
	nodeTimingBox.addItem("1 bar", 3);
	nodeTimingBox.addItem("2 bars", 4);
	nodeTimingBox.setSelectedItemIndex(2, true);
	addAndMakeVisible(&nodeTimingBox);

	setSize(800, 500);
	
	panNodeField = Rectangle<float>(30, 45, proportionOfWidth(0.5f) - 45, proportionOfHeight(0.666f) - 45);
	timeNodeField = Rectangle<float>(proportionOfWidth(0.5) + 15, 45, proportionOfWidth(0.5f) - 45, proportionOfHeight(0.666f) - 45);

	panNodes.push_back(Node(panNodeField.getCentreX(), panNodeField.getCentreY(), defaultNodeSize, nodeColour[0]));
	timeNodes.push_back(Node(timeNodeField.getCentreX(), timeNodeField.getCentreY(), defaultNodeSize, nodeColour[0]));
	numberOfVisibleNodes++;
	
	setUpAttachments();
}

void LapseAudioProcessorEditor::setUpAttachments()
{
	timeModeAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(state, "quantiseDelayTime", quantiseButton));
	nodeTimingBoxAttachment.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(state, "timerValue", nodeTimingBox));
}

void LapseAudioProcessorEditor::resized()
{
	quantiseButton.setBounds(proportionOfWidth(0.75), proportionOfHeight(0.8), 120, 50);
	nodeTimingBox.setBounds(proportionOfWidth(0.10), proportionOfHeight(0.8), 100, 30);
}

//==============================================================================
void LapseAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
	
	drawStaticUIElements(g);
	
	if (quantiseButton.getToggleState() == true)
	{
		drawQuantiseGrid(g);
	}

	for (int i = 0; i < panNodes.size(); i++)
	{
		panNodes[i].drawNode(g);
		timeNodes[i].drawNode(g);
		if (i >= 1)
		{
			drawNodeConnectorLines(g, i, panNodes);
			drawNodeConnectorLines(g, i, timeNodes);
		}
	}

	if (selectedNode != nullptr)
	{
		drawBorderOnSelectedNode(g, *selectedNode);
	}
}

void LapseAudioProcessorEditor::drawStaticUIElements(Graphics& g)
{
	Rectangle<float> titleFontArea = Rectangle<float>(0.0f, proportionOfHeight(0.666f), (float)getWidth(), proportionOfHeight(0.333f));
	Rectangle<float> panFontArea = Rectangle<float>(0.0f, proportionOfHeight(0.666f), proportionOfWidth(0.5), proportionOfHeight(0.0625f));
	Rectangle<float> timeFontArea = Rectangle<float>(proportionOfWidth(0.5f), proportionOfHeight(0.666f), proportionOfWidth(0.5), proportionOfHeight(0.0625f));

	g.setColour(textColour);
	g.setFont(largeFont);
	g.drawText("lapse.", titleFontArea, Justification::centred);

	g.setFont(mediumFont);
	g.drawText("pan", panFontArea, Justification::centred);
	g.drawText("time", timeFontArea, Justification::centred);

	g.setColour(textColour.darker());
	g.setFont(smallFont);
	g.drawText("L", 45.0f, proportionOfHeight(0.6), 45.0f, 45.0f, Justification::topLeft);
	g.drawText("R", proportionOfWidth(0.5) - 45.0f, proportionOfHeight(0.6), 45.0f, 45.0f, Justification::topLeft);
	g.drawText("0ms", proportionOfWidth(0.5) + 45.0f, proportionOfHeight(0.6), 45.0f, 45.0f, Justification::topLeft);
	g.drawText("1000ms", getWidth() - 90.0f, proportionOfHeight(0.6), 45.0f, 45.0f, Justification::centredTop);

	g.drawLine(proportionOfWidth(0.5), 45, proportionOfWidth(0.5f), proportionOfHeight(0.666f), 0.5f);
}

void LapseAudioProcessorEditor::drawQuantiseGrid(Graphics& g)
{
	float distanceBetweenLines = jmap(processor.sixteenthNoteInSeconds * 1000, 0.0f, 2000.0f, 0.0f, timeNodeField.getWidth());
	for (int xPos = timeNodeField.getX(); xPos <= timeNodeField.getRight(); xPos += distanceBetweenLines)
	{
		g.setColour(textColour.withAlpha(0.6f));
		g.drawLine(xPos, timeNodeField.getY(), xPos, timeNodeField.getBottom(), 0.25);
	}
}

void LapseAudioProcessorEditor::drawBorderOnSelectedNode(Graphics& g, Node selectedNode)
{
	g.setColour(Colours::black);
	g.drawLine(selectedNode.gradientArea.getX(), selectedNode.gradientArea.getY(), selectedNode.gradientArea.getX() + selectedNode.gradientArea.getWidth() * 0.125f, selectedNode.gradientArea.getY(), 0.5);
	g.drawLine(selectedNode.gradientArea.getRight(), selectedNode.gradientArea.getY(), selectedNode.gradientArea.getRight() - selectedNode.gradientArea.getWidth() * 0.125f, selectedNode.gradientArea.getY(), 0.5);
	g.drawLine(selectedNode.gradientArea.getX(), selectedNode.gradientArea.getY(), selectedNode.gradientArea.getX(), selectedNode.gradientArea.getY() + selectedNode.gradientArea.getHeight() * 0.125f, 0.5);
	g.drawLine(selectedNode.gradientArea.getRight(), selectedNode.gradientArea.getY(), selectedNode.gradientArea.getRight(), selectedNode.gradientArea.getY() + selectedNode.gradientArea.getHeight() * 0.125f, 0.5);

	g.drawLine(selectedNode.gradientArea.getX(), selectedNode.gradientArea.getBottom(), selectedNode.gradientArea.getX() + selectedNode.gradientArea.getWidth() * 0.125f, selectedNode.gradientArea.getBottom(), 0.5);
	g.drawLine(selectedNode.gradientArea.getRight(), selectedNode.gradientArea.getBottom(), selectedNode.gradientArea.getRight() - selectedNode.gradientArea.getWidth() * 0.125f, selectedNode.gradientArea.getBottom(), 0.5);
	g.drawLine(selectedNode.gradientArea.getX(), selectedNode.gradientArea.getBottom(), selectedNode.gradientArea.getX(), selectedNode.gradientArea.getBottom() - selectedNode.gradientArea.getHeight() * 0.125f, 0.5);
	g.drawLine(selectedNode.gradientArea.getRight(), selectedNode.gradientArea.getBottom(), selectedNode.gradientArea.getRight(), selectedNode.gradientArea.getBottom() - selectedNode.gradientArea.getHeight() * 0.125f, 0.5);

}

void LapseAudioProcessorEditor::drawNodeConnectorLines(Graphics& g, int i, std::vector<Node>& nodes)
{
	Line<float> nodeConnector = Line<float>(nodes[i - 1].getXPosition(), nodes[i - 1].getYPosition(), nodes[i].getXPosition(), nodes[i].getYPosition());
	ColourGradient lineGradient = ColourGradient(nodes[i - 1].getColour(), nodes[i - 1].getXPosition(), nodes[i - 1].getYPosition(),
												 nodes[i].getColour(), nodes[i].getXPosition(), nodes[i].getYPosition(), false);
	g.setGradientFill(lineGradient);
	g.drawLine(nodeConnector, 2);
}

//=======================================================================================================
//Nodes are added on mouseDoubleClick()
void LapseAudioProcessorEditor::mouseDoubleClick(const MouseEvent &m)
{
	
	//Prevents panNodes from being created outside of panNodeField
	if (m.getMouseDownX() < panNodeField.getRight() && m.getMouseDownX() > panNodeField.getX() &&
		m.getMouseDownY() < panNodeField.getBottom() && m.getMouseDownY() > panNodeField.getY())
	{
		if (numberOfVisibleNodes < 10)
		{
			panNodes.push_back(Node(m.getMouseDownX(), m.getMouseDownY(), defaultNodeSize, nodeColour[panNodes.size()]));
			timeNodes.push_back(Node(timeNodeField.getX() + (numberOfVisibleNodes * 30.0f), timeNodeField.getY() + 30, defaultNodeSize, nodeColour[timeNodes.size()]));
			selectedNode = &panNodes.back();
			repaint();
		}
	}
	//Prevents timeNodes from being created outside of timeNodeField
	else if (m.getMouseDownX() < timeNodeField.getRight() && m.getMouseDownX() > timeNodeField.getX() &&
			 m.getMouseDownY() < timeNodeField.getBottom() && m.getMouseDownY() > timeNodeField.getY())
	{
		if (numberOfVisibleNodes < 10)
		{
			timeNodes.push_back(Node(m.getMouseDownX(), m.getMouseDownY(), defaultNodeSize, nodeColour[timeNodes.size()]));
			panNodes.push_back(Node(panNodeField.getCentreX(), panNodeField.getY() + (numberOfVisibleNodes * 30.0f), defaultNodeSize, nodeColour[panNodes.size()]));
			selectedNode = &timeNodes.back();

			repaint();
		}
	}	
	numberOfVisibleNodes++;
}
//===============================================================================================

//Node positions are changed on mouseDrag().

void LapseAudioProcessorEditor::mouseDrag(const MouseEvent &m)
{
	selectNodeForMovement(m);
	if(selectedNode != nullptr)
		updateNodePosition(m, *selectedNode);
	repaint();
}

void LapseAudioProcessorEditor::selectNodeForMovement(const MouseEvent &m)
{
	for (int i = 0; i < numberOfVisibleNodes; i++)
	{
		//if mouse-click occurs within the node area, that node is used for movement.

		if (m.getMouseDownX() < panNodes[i].nodeArea.getRight() && m.getMouseDownX() > panNodes[i].nodeArea.getX() &&
			m.getMouseDownY() < panNodes[i].nodeArea.getBottom() && m.getMouseDownY() > panNodes[i].nodeArea.getY())
		{
			selectedNode = &panNodes[i];
			selectedNode->isPanNode = true;
			selectedNode->isTimeNode = false;
		}

		if (m.getMouseDownX() < timeNodes[i].nodeArea.getRight() && m.getMouseDownX() > timeNodes[i].nodeArea.getX() &&
			m.getMouseDownY() < timeNodes[i].nodeArea.getBottom() && m.getMouseDownY() > timeNodes[i].nodeArea.getY())
		{
			selectedNode = &timeNodes[i];
			selectedNode->isTimeNode = true;
			selectedNode->isPanNode = false;
		}
	}
}

void LapseAudioProcessorEditor::updateNodePosition(const MouseEvent &m, Node& selectedNode)
{
		float newX = m.getDistanceFromDragStartX() + m.getMouseDownX();
		float newY = m.getDistanceFromDragStartY() + m.getMouseDownY();
		
		keepNodeInField(newX, newY, selectedNode);

		if (selectedNode.isTimeNode && quantiseButton.getToggleState() == true)
		{
			newX = quantisePosition(newX, (processor.sixteenthNoteInSeconds*1000));
		}

		selectedNode.setXPosition(newX);
		selectedNode.setYPosition(newY);
		//updateMixParameter();
		updatePanParameter();
		updateFeedbackParameter();
		updateDelayTimeParameter();
}

//=========================================================================================
//This ensures that the nodes stay within their designated areas

//TODO: Create a better system for this. 

void LapseAudioProcessorEditor::keepNodeInField(float &newX, float &newY, Node selectedNode)
{
	if (selectedNode.isPanNode)
	{
		if (newX > panNodeField.getRight())
			newX = panNodeField.getRight();
		if (newX < panNodeField.getX())
			newX = panNodeField.getX();
		if (newY > panNodeField.getBottom())
			newY = panNodeField.getBottom();
		if (newY < panNodeField.getY())
			newY = panNodeField.getY();

	}
	else if (selectedNode.isTimeNode)
	{
		if (newX > timeNodeField.getRight())
			newX = timeNodeField.getRight();
		if (newX < timeNodeField.getX())
			newX = timeNodeField.getX();
		if (newY > timeNodeField.getBottom())
			newY = timeNodeField.getBottom();
		if (newY < timeNodeField.getY())
			newY = timeNodeField.getY();
	}
}
//================================================================================================

//This quantises the position of a node so that when it is translated to an audio parameter, 
//it will be synchronised to the DAW.

float LapseAudioProcessorEditor::quantisePosition(float position, float noteLengthInMS)
{
	float noteLengthMappedToXAxis = jmap(noteLengthInMS, 0.0f, 2000.0f, 0.0f, timeNodeField.getWidth());
	position = (round((position - timeNodeField.getX()) / noteLengthMappedToXAxis) * noteLengthMappedToXAxis) + timeNodeField.getX();
	return position;
}
//================================================================================================

//When updating parameters the values must be scaled between 0 and 1 in order to behave as expected in the plugin host.

void LapseAudioProcessorEditor::updatePanParameter()
{
	//Map pan field X min/max to pan parameter min/max
	pan = jmap(panNodes[currentDelayNode].getXPosition(), panNodeField.getX(), panNodeField.getRight(), 0.0f, 1.0f);
	processor.parameters.getParameter("panPosition")->beginChangeGesture();
	processor.parameters.getParameter("panPosition")->setValueNotifyingHost(pan);
	processor.parameters.getParameter("panPosition")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateFeedbackParameter()
{
	feedback = jmap(panNodes[currentDelayNode].getYPosition(), panNodeField.getBottom(), panNodeField.getY(), 0.0f, 1.0f);
	processor.parameters.getParameter("feedback")->beginChangeGesture();
	processor.parameters.getParameter("feedback")->setValueNotifyingHost(feedback);
	processor.parameters.getParameter("feedback")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateDelayTimeParameter()
{
	delayTime = jmap(timeNodes[currentDelayNode].getXPosition(), timeNodeField.getX(), timeNodeField.getRight(), 0.0f, 1.0f);
	processor.parameters.getParameter("delayTime")->beginChangeGesture();
	processor.parameters.getParameter("delayTime")->setValueNotifyingHost(delayTime);
	processor.parameters.getParameter("delayTime")->endChangeGesture();
}

void LapseAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{
	changeCurrentDelayNode();
	repaint();
	updatePanParameter();
	updateFeedbackParameter();
	updateDelayTimeParameter();
}

void LapseAudioProcessorEditor::changeCurrentDelayNode()
{
	if (panNodes[currentDelayNode].isDelayNode)
	{
		panNodes[currentDelayNode].isDelayNode = false;
		timeNodes[currentDelayNode].isDelayNode = false;
	}
	
	if (currentDelayNode < numberOfVisibleNodes && numberOfVisibleNodes > 1)
	{
		currentDelayNode++;
		if (currentDelayNode == numberOfVisibleNodes)
		{
			currentDelayNode = 0;
		}
	}
		
	else
		currentDelayNode = 0;

	panNodes[currentDelayNode].isDelayNode = true;
	timeNodes[currentDelayNode].isDelayNode = true;
}

LapseAudioProcessorEditor::~LapseAudioProcessorEditor()
{
	broadcaster.removeChangeListener(this);
	nodeTimingBox.setLookAndFeel(nullptr);
}

//==========================================================================================
/* 
	Unused Functions (May reintroduce in a later version)

	void LapseAudioProcessorEditor::updateMixParameter()
	{
		//Map pan field Y min/max to mix parameter min/max
		mix = jmap(panNodes[currentDelayNode].getYPosition(), panNodeField.getBottom(), panNodeField.getY(), 0.0f, 1.0f);
		processor.parameters.getParameter("mix")->beginChangeGesture();
		processor.parameters.getParameter("mix")->setValueNotifyingHost(mix);
		processor.parameters.getParameter("mix")->endChangeGesture();
	}
*/
