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
	largeFont.setExtraKerningFactor(0.5);
	largeFont.setHeight(35);
	mediumFont.setExtraKerningFactor(0.25);
	mediumFont.setHeight(25);
	smallFont.setHeight(15);
	

	setSize(800, 500);
	
	panNodeField = Rectangle<float>(30, 45, multiplyWindowWidth(0.5f) - 45, multiplyWindowHeight(0.666f) - 45);
	timeNodeField = Rectangle<float>(multiplyWindowWidth(0.5) + 15, 45, multiplyWindowWidth(0.5f) - 45, multiplyWindowHeight(0.666f) - 45);

	/*panNodes.push_back(Node(panNodeField.getCentreX(), panNodeField.getCentreY(), defaultNodeSize, nodeColour[0]));
	timeNodes.push_back(Node(timeNodeField.getCentreX(), timeNodeField.getCentreY(), defaultNodeSize, nodeColour[0]));*/

	//setUpAttachments();
    
   
}

LapseAudioProcessorEditor::~LapseAudioProcessorEditor()
{
}
void LapseAudioProcessorEditor::setUpAttachments()
{
	mixAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "mix", mixValue));
	delayTimeAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "delayTime", delayTimeValue));
	feedbackAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "feedback", feedbackValue));
	panAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(state, "panPosition", panValue));
	reverseAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(state, "isReversing", reverseButton));
}

void LapseAudioProcessorEditor::resized()
{

}

//==============================================================================
void LapseAudioProcessorEditor::paint (Graphics& g)
{
	//Fills background and draws the static gui elements
    g.fillAll (Colours::white);
	Rectangle<float> titleFontArea = Rectangle<float>(0.0f, multiplyWindowHeight(0.666f), (float)getWidth(),multiplyWindowHeight(0.333f));
	Rectangle<float> panFontArea = Rectangle<float>(0.0f, multiplyWindowHeight(0.666f), multiplyWindowWidth(0.5), multiplyWindowHeight(0.0625f));
	Rectangle<float> timeFontArea = Rectangle<float>(multiplyWindowWidth(0.5f), multiplyWindowHeight(0.666f), multiplyWindowWidth(0.5), multiplyWindowHeight(0.0625f));
	

	g.setColour(textColour);
    g.setFont (largeFont);
    g.drawText ("lapse.", titleFontArea, Justification::centred);
	g.setFont(mediumFont);
	g.drawText("pan", panFontArea, Justification::centred);
	g.drawText("time", timeFontArea, Justification::centred);

	g.setColour(textColour.darker());
	g.setFont(smallFont);
	g.drawText("L", 45.0f, multiplyWindowHeight(0.6), 45.0f, 45.0f, Justification::topLeft);
	g.drawText("R", multiplyWindowWidth(0.5) - 45.0f, multiplyWindowHeight(0.6), 45.0f, 45.0f, Justification::topLeft);
	g.drawText("0ms", multiplyWindowWidth(0.5) + 45.0f, multiplyWindowHeight(0.6), 45.0f, 45.0f, Justification::topLeft);
	g.drawText("1000ms", getWidth() - 90.0f, multiplyWindowHeight(0.6), 45.0f, 45.0f, Justification::centredTop);

	g.drawLine(multiplyWindowWidth(0.5), 45, multiplyWindowWidth(0.5f), multiplyWindowHeight(0.666f), 0.5f);

	//Draws nodes and node connector lines
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
			repaint();
		}
	}	
	numberOfVisibleNodes++;
	selectNodeForMovement(m);
}
//===============================================================================================

//Node positions are changed on mouseDrag().

void LapseAudioProcessorEditor::mouseDrag(const MouseEvent &m)
{
	selectNodeForMovement(m);
	updateNodePosition(m, *selectedNode);
	repaint();
}

void LapseAudioProcessorEditor::selectNodeForMovement(const MouseEvent &m)
{
	for (int i = 0; i < numberOfVisibleNodes; i++)
	{
		//if mouse-click occurs within the node area, that nodes is used for movement.

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
	//This condition prevents nodes from being moved outside of the nodeField rectangles.
	//I resent this if statement. Needs to be revisited but does the job.

		float newX = m.getDistanceFromDragStartX() + m.getMouseDownX();
		float newY = m.getDistanceFromDragStartY() + m.getMouseDownY();
		
		keepNodeInField(newX, newY, selectedNode);

		selectedNode.setXPosition(newX);
		selectedNode.setYPosition(newY);
		//updateMixParameter();
		updatePanParameter();
		updateFeedbackParameter();
		updateDelayTimeParameter();
	
	
}

//================================================================================================

//When updating parameters the values must be scaled between 0 and 1 in order to behave as expected in the plugin host.

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

void LapseAudioProcessorEditor::updatePanParameter()
{
	//Map pan field X min/max to pan parameter min/max
	pan = jmap(panNodes[currentDelayNode].getXPosition(), panNodeField.getX(), panNodeField.getRight(), 0.0f, 1.0f);
	processor.parameters.getParameter("panPosition")->beginChangeGesture();
	processor.parameters.getParameter("panPosition")->setValueNotifyingHost(pan);
	processor.parameters.getParameter("panPosition")->endChangeGesture();
}

//void LapseAudioProcessorEditor::updateMixParameter()
//{
//	//Map pan field Y min/max to mix parameter min/max
//	mix = jmap(panNodes[currentDelayNode].getYPosition(), panNodeField.getBottom(), panNodeField.getY(), 0.0f, 1.0f);
//	processor.parameters.getParameter("mix")->beginChangeGesture();
//	processor.parameters.getParameter("mix")->setValueNotifyingHost(mix);
//	processor.parameters.getParameter("mix")->endChangeGesture();
//}

void LapseAudioProcessorEditor::updateFeedbackParameter()
{
	feedback = jmap(panNodes[currentDelayNode].getYPosition(), panNodeField.getBottom(), panNodeField.getY(), 0.0f, 1.0f);
	processor.parameters.getParameter("feedback")->beginChangeGesture();
	processor.parameters.getParameter("feedback")->setValueNotifyingHost(feedback);
	processor.parameters.getParameter("feedback")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateDelayTimeParameter()
{
	if (currentDelayNode > 1)
	{
		delayTime = jmap(timeNodes[currentDelayNode].getXPosition() - timeNodes[currentDelayNode - 1].getXPosition(),
						 timeNodeField.getX(), timeNodeField.getRight(), 0.0f, 1.0f);
	}
	else
	{
		delayTime = jmap(timeNodes[currentDelayNode].getXPosition(), timeNodeField.getX(), timeNodeField.getRight(), 0.0f, 1.0f);
	}

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
	if (currentDelayNode < numberOfVisibleNodes)
		currentDelayNode++;
	else
		currentDelayNode = 0;

	panNodes[currentDelayNode].isDelayNode = true;
	timeNodes[currentDelayNode].isDelayNode = true;
}

float  LapseAudioProcessorEditor::multiplyWindowWidth(float fraction)
{
	return getWidth() * fraction;
}

float  LapseAudioProcessorEditor::multiplyWindowHeight(float fraction)
{
	return getHeight() * fraction;
}


