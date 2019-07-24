/*
  ==============================================================================

    LapseLookAndFeel.cpp
    Created: 21 Jul 2019 3:10:37pm
    Author:  jonny

  ==============================================================================
*/

#include "LapseLookAndFeel.h"

LapseLookAndFeel::LapseLookAndFeel()
{

}

LapseLookAndFeel::~LapseLookAndFeel()
{

}

void LapseLookAndFeel::drawComboBox (Graphics& g, int width, int height, bool,
									 int, int, int, int, ComboBox& box)
{
	auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
	Rectangle<int> boxBounds(0, 0, width, height);

	//g.setColour(box.findColour(ComboBox::backgroundColourId));
	//g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

	g.setColour(box.findColour(ComboBox::outlineColourId));
	g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

	Rectangle<int> arrowZone(width - 30, 0, 20, height);
	Path path;
	path.startNewSubPath(arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
	path.lineTo(static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
	path.lineTo(arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

	g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
	g.strokePath(path, PathStrokeType(2.0f));
}

Font LapseLookAndFeel::getComboBoxFont(ComboBox& box)
{
	Font f = Font(Typeface::createSystemTypefaceFor(BinaryData::RobotoThin_ttf, BinaryData::RobotoThin_ttfSize));
	f.setHeight(jmin(15.0f, box.getHeight() * 0.85f));
	f.setHorizontalScale(0.9f);
	return f;
}