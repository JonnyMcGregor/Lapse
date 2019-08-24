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
    : AudioProcessorEditor (&p), broadcaster (b), processor (p), state (s)
{
	b.addChangeListener(this);
    setSize(800, 500);
    
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
	nodeTimingBox.setSelectedItemIndex(2);
	addAndMakeVisible(&nodeTimingBox);

	panNodeField = Rectangle<float>(30, 45, proportionOfWidth(0.5f) - 45, proportionOfHeight(0.666f) - 45);
	timeNodeField = Rectangle<float>(proportionOfWidth(0.5) + 15, 45, proportionOfWidth(0.5f) - 45, proportionOfHeight(0.666f) - 45);

	if (processor.isFirstTimeOpeningEditor)
	{
		
		processor.panNodes.push_back(Node(panNodeField.getCentreX(), panNodeField.getCentreY(), defaultNodeSize, nodeColour[0]));
		processor.timeNodes.push_back(Node(timeNodeField.getCentreX(), timeNodeField.getCentreY(), defaultNodeSize, nodeColour[0]));
		processor.numberOfVisibleNodes++;
		processor.isFirstTimeOpeningEditor = false;
		repaint();
	}
	setUpAttachments();
}

void LapseAudioProcessorEditor::setUpAttachments()
{
	timeModeAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(state, "quantiseDelayTime", quantiseButton));
	nodeTimingBoxAttachment.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(state, "timerValue", nodeTimingBox));
}

//==============================================================================
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

	for (int i = 0; i < processor.panNodes.size(); i++)
	{
		processor.panNodes[i].drawNode(g);
		processor.timeNodes[i].drawNode(g);
		if (i >= 1)
		{
			drawNodeConnectorLines(g, i, processor.panNodes);
			drawNodeConnectorLines(g, i, processor.timeNodes);
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
	Rectangle<float> mixFontArea = Rectangle<float>(25.0f, panNodeField.getBottom(), panNodeField.getHeight(), proportionOfHeight(0.0625));
	Rectangle<float> feedbackFontArea = Rectangle<float>(25, proportionOfWidth(0.5) + 150, timeNodeField.getHeight(), timeNodeField.getX());

	g.setColour(textColour);
	g.setFont(largeFont);
	g.drawText("lapse.", titleFontArea, Justification::centred);

	g.setFont(mediumFont);
	g.drawText("pan", panFontArea, Justification::centred);
	g.drawText("time", timeFontArea, Justification::centred);
	
	//Transform to draw text vertically
	g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, 25.0f, panNodeField.getBottom()));
	
	g.drawText("mix", mixFontArea, Justification::centred);
	g.drawText("feedback", feedbackFontArea, Justification::centred);
	
	//Reset transform...
	g.addTransform(AffineTransform::rotation(MathConstants<float>::halfPi, 25.0f, panNodeField.getBottom()));
	
    g.setFont(smallFont);
    g.drawText("double-click to create a node", panNodeField.getX(), panNodeField.getY(), panNodeField.getWidth(), 25.0f, Justification::centredTop);
   g.drawText("right-click to delete a node", timeNodeField.getX(), timeNodeField.getY(), timeNodeField.getWidth(), 45.0f, Justification::centredTop);
    
	g.setColour(textColour.darker());
	
	g.drawText("L", 45.0f, proportionOfHeight(0.6), 45.0f, 45.0f, Justification::topLeft);
	g.drawText("R", proportionOfWidth(0.5) - 45.0f, proportionOfHeight(0.6), 45.0f, 45.0f, Justification::topLeft);
	g.drawText("0ms", proportionOfWidth(0.5) + 45.0f, proportionOfHeight(0.6), 45.0f, 45.0f, Justification::topLeft);
	g.drawText("2000ms", getWidth() - 90.0f, proportionOfHeight(0.6), 45.0f, 45.0f, Justification::centredTop);
    
    
	g.drawLine(proportionOfWidth(0.5), 45, proportionOfWidth(0.5f), proportionOfHeight(0.666f), 0.5f);
}

void LapseAudioProcessorEditor::drawQuantiseGrid(Graphics& g)
{
    if(processor.getPlayHead() != 0)
    {
        float distanceBetweenLines = jmap(processor.sixteenthNoteInSeconds * 1000, 0.0f, 2000.0f, 0.0f, timeNodeField.getWidth());
        for (int xPos = timeNodeField.getX(); xPos <= timeNodeField.getRight(); xPos += distanceBetweenLines)
        {
            g.setColour(textColour.withAlpha(0.6f));
            g.drawLine(xPos, timeNodeField.getY(), xPos, timeNodeField.getBottom(), 0.25);
        }
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
void LapseAudioProcessorEditor::mouseDown(const MouseEvent &m)
{
    selectNodeForMovement(m);
    repaint();
    if(m.mods.isRightButtonDown())
    {
        if (selectedNode != nullptr && processor.numberOfVisibleNodes > 1)
        {
            std::vector<Node>::iterator it = processor.panNodes.begin();
            std::vector<Node>::iterator it2 = processor.timeNodes.begin();

            for(int i = 0; it != processor.panNodes.end(); it++, it2++, i++)
            {
               if(&processor.panNodes[i] == selectedNode || &processor.timeNodes[i] == selectedNode)
               {
                   processor.panNodes.erase(it);
                   processor.timeNodes.erase(it2);
                   selectedNode = nullptr;
                   processor.numberOfVisibleNodes--;
                   repaint();
                   break;
               }
            }
        }
    }
}

void LapseAudioProcessorEditor::selectNodeForMovement(const MouseEvent &m)
{
    for (int i = 0; i < processor.numberOfVisibleNodes; i++)
    {
        //if mouse-click occurs within the node area (+ 10 px for some error space), that node is used for movement.
        if (m.getMouseDownX() < (processor.panNodes[i].nodeArea.getRight() + 10) && m.getMouseDownX() > (processor.panNodes[i].nodeArea.getX() - 10) &&
            m.getMouseDownY() < (processor.panNodes[i].nodeArea.getBottom() + 10) && m.getMouseDownY() > (processor.panNodes[i].nodeArea.getY() - 10))
        {
            selectedNode = &processor.panNodes[i];
            selectedNode->isPanNode = true;
            selectedNode->isTimeNode = false;
        }
        
        if (m.getMouseDownX() < (processor.timeNodes[i].nodeArea.getRight() + 10) && m.getMouseDownX() > (processor.timeNodes[i].nodeArea.getX() - 10) &&
            m.getMouseDownY() < (processor.timeNodes[i].nodeArea.getBottom() + 10) && m.getMouseDownY() > (processor.timeNodes[i].nodeArea.getY() - 10))
        {
            selectedNode = &processor.timeNodes[i];
            selectedNode->isTimeNode = true;
            selectedNode->isPanNode = false;
        }
    }
}

//=======================================================================================================
//Nodes are added on mouseDoubleClick()
void LapseAudioProcessorEditor::mouseDoubleClick(const MouseEvent &m)
{
	//Prevents panNodes from being created outside of panNodeField
	if (m.getMouseDownX() < panNodeField.getRight() && m.getMouseDownX() > panNodeField.getX() &&
		m.getMouseDownY() < panNodeField.getBottom() && m.getMouseDownY() > panNodeField.getY())
	{
		if (processor.numberOfVisibleNodes < 10)
		{
			processor.panNodes.push_back(Node(m.getMouseDownX(), m.getMouseDownY(), defaultNodeSize, nodeColour[processor.panNodes.size()]));
			processor.timeNodes.push_back(Node(timeNodeField.getX() + (processor.numberOfVisibleNodes * 30.0f), timeNodeField.getY() + 30, defaultNodeSize, nodeColour[processor.timeNodes.size()]));
			selectedNode = &processor.panNodes.back();
			processor.numberOfVisibleNodes++;
			repaint();
		}
	}
	//Prevents timeNodes from being created outside of timeNodeField
	else if (m.getMouseDownX() < timeNodeField.getRight() && m.getMouseDownX() > timeNodeField.getX() &&
			 m.getMouseDownY() < timeNodeField.getBottom() && m.getMouseDownY() > timeNodeField.getY())
	{
		 if (processor.numberOfVisibleNodes < 10)
		 {
			processor.timeNodes.push_back(Node(m.getMouseDownX(), m.getMouseDownY(), defaultNodeSize, nodeColour[processor.timeNodes.size()]));
			processor.panNodes.push_back(Node(panNodeField.getCentreX(), panNodeField.getY() + (processor.numberOfVisibleNodes * 30.0f), defaultNodeSize, nodeColour[processor.panNodes.size()]));
			selectedNode = &processor.timeNodes.back();
			processor.numberOfVisibleNodes++;
			repaint();
		 }
	}	
}

//===============================================================================================


//==============================================================================
//Node positions are changed on mouseDrag().
void LapseAudioProcessorEditor::mouseDrag(const MouseEvent &m)
{
    if(m.mouseWasDraggedSinceMouseDown())
    {
        selectNodeForMovement(m);
        if(selectedNode != nullptr)
            updateNodePosition(m, *selectedNode);
        repaint();
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
		updateMixParameter();
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
	pan = jmap(processor.panNodes[processor.currentDelayNode].getXPosition(), panNodeField.getX(), panNodeField.getRight(), 0.0f, 1.0f);
	processor.parameters.getParameter("panPosition")->beginChangeGesture();
	processor.parameters.getParameter("panPosition")->setValueNotifyingHost(pan);
	processor.parameters.getParameter("panPosition")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateMixParameter()
{
	mix = jmap(processor.panNodes[processor.currentDelayNode].getYPosition(), panNodeField.getBottom(), panNodeField.getY(), 0.0f, 1.0f);
	processor.parameters.getParameter("mix")->beginChangeGesture();
	processor.parameters.getParameter("mix")->setValueNotifyingHost(mix);
	processor.parameters.getParameter("mix")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateFeedbackParameter()
{
	feedback = jmap(processor.panNodes[processor.currentDelayNode].getYPosition(), timeNodeField.getBottom(), timeNodeField.getY(), 0.0f, 1.0f);
	processor.parameters.getParameter("feedback")->beginChangeGesture();
	processor.parameters.getParameter("feedback")->setValueNotifyingHost(feedback);
	processor.parameters.getParameter("feedback")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateDelayTimeParameter()
{
	delayTime = jmap(processor.timeNodes[processor.currentDelayNode].getXPosition(), timeNodeField.getX(), timeNodeField.getRight(), 0.0f, 1.0f);
	processor.parameters.getParameter("delayTime")->beginChangeGesture();
	processor.parameters.getParameter("delayTime")->setValueNotifyingHost(delayTime);
	processor.parameters.getParameter("delayTime")->endChangeGesture();
}

//==============================================================================

void LapseAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{
	repaint();
}

LapseAudioProcessorEditor::~LapseAudioProcessorEditor()
{
	broadcaster.removeChangeListener(this);
	nodeTimingBox.setLookAndFeel(nullptr);
	selectedNode = nullptr;
}
