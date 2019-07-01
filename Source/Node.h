/*
  ==============================================================================

    Node.h
    Created: 16 Jun 2019 12:44:46pm
    Author:  jonny

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class Node
{
public:
	Node(float xPosition, float yPosition, float nodeDiameter, Colour nodeColour);
	~Node();
	
	void drawNode(Graphics& g);

	void setDiameter(float newDiameterValue);
	void setXPosition(float newXPos);
	void setYPosition(float newYPos);
	void setColour(Colour newColour);
	float getDiameter();
	float getRadius();
	float getXPosition();
	float getYPosition();
	Colour getColour();

	bool isDelayNode = false;
	bool isSelectedForMovement = false;
	Rectangle<float> nodeArea;
	Rectangle<float> gradientArea;
private:

	float diameter = 0;
	float xPos = 0;
	float yPos = 0;

	Colour nodeColour;
};