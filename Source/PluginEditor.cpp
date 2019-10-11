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

    quantiseButton.setLookAndFeel(&lf);
	quantiseButton.setColour(ToggleButton::ColourIds::textColourId, textColour);
	quantiseButton.setColour(ToggleButton::ColourIds::tickColourId, textColour);
	quantiseButton.setColour(ToggleButton::ColourIds::tickDisabledColourId, textColour.darker());
	quantiseButton.setButtonText("quantise");
	addAndMakeVisible(&quantiseButton);

	nodeTimingBox.setLookAndFeel(&lf);
	nodeTimingBox.setColour(ComboBox::ColourIds::outlineColourId, textColour);
	nodeTimingBox.setColour(ComboBox::ColourIds::backgroundColourId, Colours::white);
	nodeTimingBox.setColour(ComboBox::ColourIds::textColourId, textColour);
	nodeTimingBox.setColour(ComboBox::ColourIds::arrowColourId, textColour);
    nodeTimingBox.addItem("1/8 note", 1);
	nodeTimingBox.addItem("1/4 note", 2);
	nodeTimingBox.addItem("1/2 note", 3);
	nodeTimingBox.addItem("1 bar", 4);
    nodeTimingBox.addItem("2 bars", 5);
	nodeTimingBox.setSelectedItemIndex(1);
	addAndMakeVisible(&nodeTimingBox);

	nodeField = Rectangle<float>(proportionOfWidth(0.25), 45, proportionOfWidth(0.5f) - 45, proportionOfHeight(0.666f) - 45);
    
    setUpAttachments();

	if (processor.isFirstTimeOpeningEditor)
	{
		
		processor.nodes.push_back(Node(nodeField.getCentreX(), nodeField.getCentreY(), defaultNodeSize, nodeColour[0], backgroundColour));
		processor.numberOfVisibleNodes++;
        selectedNode = &processor.nodes.back();
        
        //updateMixParameter();
        updatePanParameter();
        updateFeedbackParameter();
        updateDelayTimeParameter();
        
        repaint();
        processor.isFirstTimeOpeningEditor = false;
	}
}

void LapseAudioProcessorEditor::setUpAttachments()
{
	timeModeAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(state, "quantiseDelayTime", quantiseButton));
	nodeTimingBoxAttachment.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(state, "timerValue", nodeTimingBox));
}

//==============================================================================
void LapseAudioProcessorEditor::resized()
{
	quantiseButton.setBounds(proportionOfWidth(0.75), proportionOfHeight(0.8), 120, 30);
	nodeTimingBox.setBounds(proportionOfWidth(0.25) - 120, proportionOfHeight(0.8), 120, 30);
}

//==============================================================================
void LapseAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (backgroundColour);
	
	if (quantiseButton.getToggleState() == true)
	{
		drawQuantiseGrid(g);
	}

	for (int i = 0; i < processor.nodes.size(); i++)
	{
		processor.nodes[i].drawNode(g);
    }
    for (int i = 1; i < processor.nodes.size(); i++)
    {
			drawNodeConnectorLines(g, i, processor.nodes);
    }
	

	if (selectedNode != nullptr)
	{
		drawBorderOnSelectedNode(g, *selectedNode);
	}
    
    drawStaticUIElements(g);
}

void LapseAudioProcessorEditor::drawStaticUIElements(Graphics& g)
{
	Rectangle<float> titleFontArea = Rectangle<float>(0.0f, proportionOfHeight(0.666f), (float)getWidth(), proportionOfHeight(0.333f));
	Rectangle<float> panFontArea = Rectangle<float>(nodeField.getX(), nodeField.getBottom(), nodeField.getWidth(), proportionOfHeight(0.0625f));
	Rectangle<float> timeFontArea = Rectangle<float>(nodeField.getX() - 50, nodeField.getBottom(), nodeField.getHeight(), proportionOfHeight(0.0625));
    
	g.setColour(textColour);
	g.setFont(largeFont);
	g.drawText("lapse.", titleFontArea, Justification::centred);

	g.setFont(mediumFont);
	g.drawText("pan", panFontArea, Justification::centred);
	
	//Transform to draw text vertically
	g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, nodeField.getX() - 50.0f, nodeField.getBottom()));
	
	g.drawText("delay time", timeFontArea, Justification::centred);
	
	//Reset transform...
	g.addTransform(AffineTransform::rotation(MathConstants<float>::halfPi, nodeField.getX() - 50.0f, nodeField.getBottom()));
	
    g.setFont(smallFont);

	g.setColour(textColour.darker());
	
	g.drawText("L", panFontArea.getX(), panFontArea.getBottom() - 25, panFontArea.getWidth(), panFontArea.getHeight(), Justification::topLeft);
    g.drawText("R", panFontArea.getX(), panFontArea.getBottom() - 25, panFontArea.getWidth(), panFontArea.getHeight(), Justification::topRight);
    
    g.drawText("0ms", nodeField.getX() - 52, nodeField.getY() + 7, 45.0f, nodeField.getHeight(), Justification::bottomRight);
    g.drawText("2000ms", nodeField.getX() - 52, nodeField.getY() - 7, 45.0f, nodeField.getHeight(), Justification::topRight);
    
//    g.drawText("double-click = create a node", proportionOfWidth(0.708), proportionOfHeight(0.25), proportionOfWidth(0.25), 45, Justification::topLeft);
//    g.drawText("right-click = delete a node", proportionOfWidth(0.708), proportionOfHeight(0.25) + 25, proportionOfWidth(0.25), 45, Justification::topLeft);
//    g.drawText("shift-click & drag = change feedback", proportionOfWidth(0.708), proportionOfHeight(0.25) + 50, proportionOfWidth(0.25), 45, Justification::topLeft);
}

void LapseAudioProcessorEditor::drawQuantiseGrid(Graphics& g)
{
    if(processor.getPlayHead() != 0)
    {
        float distanceBetweenLines = jmap(processor.sixteenthNoteInSeconds * 1000, 0.0f, 2000.0f, 0.0f, nodeField.getHeight());
        int i = 0;
        for (int yPos = nodeField.getY(); yPos <= nodeField.getBottom(); yPos += distanceBetweenLines, i++)
        {
            if(i % 4 == 0)
            {
                g.setColour(textColour);
                g.drawLine(nodeField.getX(), yPos, nodeField.getRight(), yPos, 0.3);
            }
            else if(i % 1 == 0 && i % 2 != 0 && i % 4 != 0)
            {
                float dashPattern[2];
                dashPattern[0] = 8.0;
                dashPattern[1] = 8.0;
        
                g.setColour(textColour.withAlpha(0.6f));
                g.drawDashedLine(Line<float> (nodeField.getX(), yPos, nodeField.getRight(), yPos), dashPattern, 2, 0.25);
            }
            else
            {
                g.setColour(textColour.withAlpha(0.6f));
                g.drawLine(nodeField.getX(), yPos, nodeField.getRight(), yPos, 0.25);
            }
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
    if(selectedNode != nullptr)
    {
        previousNodeWidth = selectedNode->getDiameter();
    }
    
    repaint();
    if(m.mods.isRightButtonDown())
    {
        if (selectedNode != nullptr && processor.numberOfVisibleNodes > 1)
        {
            std::vector<Node>::iterator it = processor.nodes.begin();

            for(int i = 0; it != processor.nodes.end(); it++, i++)
            {
               if(&processor.nodes[i] == selectedNode)
               {
                   processor.nodes.erase(it);
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
        if (m.getMouseDownX() < (processor.nodes[i].nodeArea.getRight() + 10) && m.getMouseDownX() > (processor.nodes[i].nodeArea.getX() - 10) &&
            m.getMouseDownY() < (processor.nodes[i].nodeArea.getBottom() + 10) && m.getMouseDownY() > (processor.nodes[i].nodeArea.getY() - 10))
        {
            selectedNode = &processor.nodes[i];
            selectedNode->isPanNode = true;
        }
    }
}

//=======================================================================================================
//Nodes are added on mouseDoubleClick()
void LapseAudioProcessorEditor::mouseDoubleClick(const MouseEvent &m)
{
	//Prevents panNodes from being created outside of panNodeField
	if (m.getMouseDownX() < nodeField.getRight() && m.getMouseDownX() > nodeField.getX() &&
		m.getMouseDownY() < nodeField.getBottom() && m.getMouseDownY() > nodeField.getY())
	{
		if (processor.numberOfVisibleNodes < maximumNumberOfNodes)
		{
			processor.nodes.push_back(Node(m.getMouseDownX(), m.getMouseDownY(), defaultNodeSize, nodeColour[processor.nodes.size()], backgroundColour));
			selectedNode = &processor.nodes.back();
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
            if(m.mods.isShiftDown())
            {
                updateNodeSize(m, *selectedNode);
            }
            else
            {
                updateNodePosition(m, *selectedNode);
            }
        repaint();
    }
}
void LapseAudioProcessorEditor::updateNodeSize(const MouseEvent &m, Node& selectedNode)
{
    float newWidth = 0;
    
    if(m.getDistanceFromDragStartX() > 0)
    {
        newWidth = m.getDistanceFromDragStart() + previousNodeWidth;
        if(newWidth > maximumNodeSize)
        {
            newWidth = maximumNodeSize;
        }
    }
    else
    {
        newWidth = previousNodeWidth - m.getDistanceFromDragStart();
        if(newWidth < minimumNodeSize)
        {
            newWidth = minimumNodeSize;
        }
    }
    selectedNode.setDiameter(newWidth);
    updateFeedbackParameter();
}

void LapseAudioProcessorEditor::updateNodePosition(const MouseEvent &m, Node& selectedNode)
{
		float newX = m.getDistanceFromDragStartX() + m.getMouseDownX();
		float newY = m.getDistanceFromDragStartY() + m.getMouseDownY();
		
		keepNodeInField(newX, newY, selectedNode);

		if (quantiseButton.getToggleState() == true)
		{
			newY = quantisePosition(newY, (processor.sixteenthNoteInSeconds*1000));
		}

		selectedNode.setXPosition(newX);
		selectedNode.setYPosition(newY);
		//updateMixParameter();
		updatePanParameter();
		updateDelayTimeParameter();
}

//=========================================================================================
//This ensures that the nodes stay within their designated areas

//TODO: Create a better system for this. 

void LapseAudioProcessorEditor::keepNodeInField(float &newX, float &newY, Node selectedNode)
{
	if (selectedNode.isPanNode)
	{
		if (newX > nodeField.getRight())
			newX = nodeField.getRight();
		if (newX < nodeField.getX())
			newX = nodeField.getX();
		if (newY > nodeField.getBottom())
			newY = nodeField.getBottom();
		if (newY < nodeField.getY())
			newY = nodeField.getY();
	}
}
//================================================================================================

//This quantises the Y position of a node to increments of note values

float LapseAudioProcessorEditor::quantisePosition(float position, float noteLengthInMS)
{
	float noteLengthMappedToYAxis = jmap(noteLengthInMS, 0.0f, 2000.0f, 0.0f, nodeField.getHeight());
	position = (round((position - nodeField.getY()) / noteLengthMappedToYAxis) * noteLengthMappedToYAxis) + nodeField.getY();
	return position;
}
//================================================================================================

//When updating parameters the values must be scaled between 0 and 1 in order to behave as expected in the plugin host.

void LapseAudioProcessorEditor::updatePanParameter()
{
	pan = jmap(processor.nodes[processor.currentDelayNode].getXPosition(), nodeField.getX(), nodeField.getRight(), 0.0f, 1.0f);
	processor.parameters.getParameter("panPosition")->beginChangeGesture();
	processor.parameters.getParameter("panPosition")->setValueNotifyingHost(pan);
	processor.parameters.getParameter("panPosition")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateFeedbackParameter()
{
	feedback = jmap(processor.nodes[processor.currentDelayNode].getDiameter(), (float)minimumNodeSize, (float)maximumNodeSize, 0.0f, 1.0f);
	processor.parameters.getParameter("feedback")->beginChangeGesture();
	processor.parameters.getParameter("feedback")->setValueNotifyingHost(feedback);
	processor.parameters.getParameter("feedback")->endChangeGesture();
}

void LapseAudioProcessorEditor::updateDelayTimeParameter()
{
	delayTime = jmap(processor.nodes[processor.currentDelayNode].getYPosition(), nodeField.getBottom(), nodeField.getY(), 0.0f, 1.0f);
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
    quantiseButton.setLookAndFeel(nullptr);
	selectedNode = nullptr;
}


//=============== Unused Functions ===============\\

//void LapseAudioProcessorEditor::updateMixParameter()
//{
//    mix = jmap(processor.panNodes[processor.currentDelayNode].getYPosition(), panNodeField.getBottom(), panNodeField.getY(), 0.0f, 1.0f);
//    processor.parameters.getParameter("mix")->beginChangeGesture();
//    processor.parameters.getParameter("mix")->setValueNotifyingHost(mix);
//    processor.parameters.getParameter("mix")->endChangeGesture();
//}
