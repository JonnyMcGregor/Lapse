/*
  ==============================================================================

    Node.cpp
    Created: 16 Jun 2019 12:44:46pm
    Author:  jonny

  ==============================================================================
*/

#include "Node.h"

Node::Node(float xPosition, float yPosition, float nodeDiameter, Colour nodeColour)
{
	xPos = xPosition;
	yPos = yPosition;
	diameter = nodeDiameter;
	outlineColour = nodeColour;
}

Node::~Node()
{

}

void Node::paint(Graphics& g)
{
	Rectangle<float> nodeArea = Rectangle<float>(xPos, yPos, diameter, diameter);
	g.setColour(outlineColour);
	g.drawEllipse(nodeArea, 0.5);
}

void Node::resized()
{
	
}