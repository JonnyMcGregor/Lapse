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
	Rectangle<float> titleFontArea = Rectangle<float>(0.0f, multiplyWindowHeight(0.666f), getWidth(),multiplyWindowHeight(0.333f));
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
	g.drawText("L", 45, multiplyWindowHeight(0.6), 45, 45, Justification::topLeft);
	g.drawText("R", multiplyWindowWidth(0.5) - 45, multiplyWindowHeight(0.6), 45, 45, Justification::topLeft);


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
			numberOfVisibleNodes++;
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
			timeNodes.push_back(Node(m.getMouseDownX(), m.getMouseDownY(), defaultNodeSize, nodeColour[panNodes.size()]));
			panNodes.push_back(Node(panNodeField.getCentreX(), panNodeField.getY() + (numberOfVisibleNodes * 30.0f), defaultNodeSize, nodeColour[timeNodes.size()]));

			numberOfVisibleNodes++;
			repaint();
		}
	}
	processor.parameters.getParameter("numberOfNodes")->beginChangeGesture();
	processor.parameters.getParameter("numberOfNodes")->setValueNotifyingHost(numberOfVisibleNodes);
	processor.parameters.getParameter("numberOfNodes")->endChangeGesture();
}
//===============================================================================================

//Node positions and size are changed on mouseDrag().

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
		//if mouse-click occurs within the node area, that nodes index is used for movement.

		if (m.getMouseDownX() < panNodes[i].nodeArea.getRight() && m.getMouseDownX() > panNodes[i].nodeArea.getX() &&
			m.getMouseDownY() < panNodes[i].nodeArea.getBottom() && m.getMouseDownY() > panNodes[i].nodeArea.getY())
		{
			selectedNode = &panNodes[i];
		}

		if (m.getMouseDownX() < timeNodes[i].nodeArea.getRight() && m.getMouseDownX() > timeNodes[i].nodeArea.getX() &&
			m.getMouseDownY() < timeNodes[i].nodeArea.getBottom() && m.getMouseDownY() > timeNodes[i].nodeArea.getY())
		{
			selectedNode = &timeNodes[i];
		}
	}
}

//void LapseAudioProcessorEditor::updateNodeSize(const MouseEvent &m, Node& selectedNode)
//{
//	if (selectedNode.getDiameter() < maximumNodeSize)
//	{
//		float controlDiameter = selectedNode.getDiameter() - m.getDistanceFromDragStart();
//		selectedNode.setDiameter(m.getDistanceFromDragStart());
//
//		if (selectedNode.getDiameter() < 0)
//		{
//			selectedNode.setDiameter(selectedNode.getDiameter() * -1);
//		}
//	}
//
//	updateFeedbackParameter();
//}

void LapseAudioProcessorEditor::updateNodePosition(const MouseEvent &m, Node& selectedNode)
{
	float newX = m.getDistanceFromDragStartX() + m.getMouseDownX();
	float newY = m.getDistanceFromDragStartY() + m.getMouseDownY();
	selectedNode.setXPosition(newX);
	selectedNode.setYPosition(newY);

	updatePanParameter();
	updateMixParameter();
	updateFeedbackParameter();
	updateDelayTimeParameter();
}
void LapseAudioProcessorEditor::updatePanParameter()
{
	//Map window X min/max to pan parameter min/max
	//panValue.setValue(jmap(panNodes[currentDelayNode].getXPosition(), panNodeField.getX(), panNodeField.getRight(), 0.0f, 1.0f));
	pan = jmap(panNodes[currentDelayNode].getXPosition(), panNodeField.getX(), panNodeField.getRight(), 0.0f, 1.0f);
	processor.parameters.getParameter("panPosition")->beginChangeGesture();
	//processor.parameters.getParameter("panPosition")->setValueNotifyingHost(panValue.getValue());
	processor.parameters.getParameter("panPosition")->setValueNotifyingHost(pan);
	processor.parameters.getParameter("panPosition")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateMixParameter()
{
	//Map window Y min/max to mix parameter min/max
	//mixValue.setValue(jmap(panNodes[currentDelayNode].getYPosition(), panNodeField.getBottom(), panNodeField.getY(), 0.0f, 1.0f));
	mix = jmap(panNodes[currentDelayNode].getYPosition(), panNodeField.getBottom(), panNodeField.getY(), 0.0f, 1.0f);
	processor.parameters.getParameter("mix")->beginChangeGesture();
	//processor.parameters.getParameter("mix")->setValueNotifyingHost(mixValue.getValue());
	processor.parameters.getParameter("mix")->setValueNotifyingHost(mix);
	processor.parameters.getParameter("mix")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateFeedbackParameter()
{
	//feedbackValue.setValue(jmap(timeNodes[currentDelayNode].getYPosition(), timeNodeField.getBottom(), timeNodeField.getY(), 0.0f, 2.0f));
	feedback = jmap(timeNodes[currentDelayNode].getYPosition(), timeNodeField.getBottom(), timeNodeField.getY(), 0.0f, 2.0f);
	processor.parameters.getParameter("feedback")->beginChangeGesture();
	//processor.parameters.getParameter("feedback")->setValueNotifyingHost(feedbackValue.getValue());
	processor.parameters.getParameter("feedback")->setValueNotifyingHost(feedback);
	processor.parameters.getParameter("feedback")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateDelayTimeParameter()
{
	if (currentDelayNode > 1)
	{
		/*delayTimeValue.setValue(jmap(timeNodes[currentDelayNode].getXPosition() - timeNodes[currentDelayNode - 1].getXPosition(), 
									 timeNodeField.getX(), timeNodeField.getRight(), 0.0f, 1000.0f));*/
		delayTime = jmap(timeNodes[currentDelayNode].getXPosition() - timeNodes[currentDelayNode - 1].getXPosition(),
			timeNodeField.getX(), timeNodeField.getRight(), 0.0f, 1000.0f);
	}
	else
	{
		//delayTimeValue.setValue(jmap(timeNodes[currentDelayNode].getXPosition(), timeNodeField.getX(), timeNodeField.getRight(), 0.0f, 1000.0f));
		delayTime = jmap(timeNodes[currentDelayNode].getXPosition(), timeNodeField.getX(), timeNodeField.getRight(), 0.0f, 1000.0f);
	}
	processor.parameters.getParameter("delayTime")->beginChangeGesture();
	//processor.parameters.getParameter("delayTime")->setValueNotifyingHost(delayTimeValue.getValue());
	processor.parameters.getParameter("delayTime")->setValueNotifyingHost(delayTime);
	processor.parameters.getParameter("delayTime")->endChangeGesture();
}

void LapseAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{
	//changeCurrentDelayNode();
}

void LapseAudioProcessorEditor::changeCurrentDelayNode()
{
	if (currentDelayNode < numberOfVisibleNodes)
		currentDelayNode++;
	else
		currentDelayNode == 0;
}

float  LapseAudioProcessorEditor::multiplyWindowWidth(float fraction)
{
	return getWidth() * fraction;
}

float  LapseAudioProcessorEditor::multiplyWindowHeight(float fraction)
{
	return getHeight() * fraction;
}


