/*
  ==============================================================================

    Node.cpp
    Created: 16 Jun 2019 12:44:46pm
    Author:  jonny

  ==============================================================================
*/

#include "Node.h"

Node::Node(float xPosition, float yPosition, float nodeDiameter, Colour colourOfNode)
{
	xPos = xPosition;
	yPos = yPosition;
	diameter = nodeDiameter;
	nodeColour = colourOfNode;
}

Node::~Node()
{

}

void Node::drawNode(Graphics& g)
{
	nodeArea = Rectangle<float>(xPos - (diameter / 2), yPos - (diameter / 2), diameter, diameter);
	gradientArea = Rectangle<float>(xPos - diameter, yPos - diameter, diameter * 2, diameter * 2);
	Point<float> endOfGradient = Point<float>(gradientArea.getX() + nodeArea.getWidth(), gradientArea.getY());
	ColourGradient gradient = ColourGradient(nodeColour, nodeArea.getCentre(), Colours::transparentWhite, endOfGradient, true);

	if (isDelayNode)
		g.setColour(nodeColour);

	else
		g.setColour(Colours::white);

	g.drawEllipse(nodeArea, 2.0f);
	g.setGradientFill(gradient);
	g.fillEllipse(gradientArea);
}

void Node::setDiameter(float newDiameterValue)
{
	diameter = newDiameterValue;
}

void Node::setXPosition(float newXPos)
{
	xPos = newXPos;
}

void Node::setYPosition(float newYPos)
{
	yPos = newYPos;
}

void Node::setColour(Colour newColour)
{
	nodeColour = newColour;
}
float Node::getDiameter()
{
	return diameter;
}

float Node::getRadius()
{
	return diameter * 0.5f;
}
float Node::getXPosition()
{
	return xPos;
}

float Node::getYPosition()
{
	return yPos;
}

Colour Node::getColour()
{
	return nodeColour;
}