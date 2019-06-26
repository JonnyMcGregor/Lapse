/*
  ==============================================================================

    Node.h
    Created: 16 Jun 2019 12:44:46pm
    Author:  jonny

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class Node : public Component
{
public:
	Node(float xPosition, float yPosition, float nodeDiameter, Colour nodeColour);
	~Node();
	void paint(Graphics&) override;
	void resized() override;
	
private:

	float diameter = 0;
	float xPos = 0;
	float yPos = 0;
	Colour outlineColour;
	Colour fillColour;
};