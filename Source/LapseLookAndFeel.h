/*
  ==============================================================================

    LapseLookAndFeel.h
    Created: 21 Jul 2019 3:10:37pm
    Author:  jonny

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class LapseLookAndFeel : public LookAndFeel_V4
{
public:
	LapseLookAndFeel();
	~LapseLookAndFeel();

	void drawComboBox(Graphics&, int width, int height, bool isButtonDown,
		int buttonX, int buttonY, int buttonW, int buttonH, ComboBox&) override;
	Font getComboBoxFont(ComboBox& box) override;
	
};