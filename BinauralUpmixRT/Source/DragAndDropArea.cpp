#include "DragAndDropArea.h"

DragAndDropArea::DragAndDropArea(String inMessage)
{
	message = inMessage;

	somethingIsBeingDraggedOver = false;
}

void DragAndDropArea::paint(Graphics& g)
{
	g.fillAll(Colours::green.withAlpha(0.2f));

	// draw a red line around the comp if the user's currently dragging something over it..
	if (somethingIsBeingDraggedOver)
	{
		g.setColour(Colours::red);
		g.drawRect(getLocalBounds(), 3);
	}

	g.setColour(getLookAndFeel().findColour(Label::textColourId));
	g.setFont(14.0f);
	g.drawFittedText(message, getLocalBounds().reduced(10, 0), Justification::centred, 4);
}

bool DragAndDropArea::isInterestedInFileDrag(const StringArray& /*files*/)
{
	// normally you'd check these files to see if they're something that you're
	// interested in before returning true, but for the demo, we'll say yes to anything..
	return true;
}

void DragAndDropArea::fileDragEnter(const StringArray& /*files*/, int /*x*/, int /*y*/)
{
	somethingIsBeingDraggedOver = true;
	repaint();
}

void DragAndDropArea::fileDragMove(const StringArray& /*files*/, int /*x*/, int /*y*/)
{
}

void DragAndDropArea::fileDragExit(const StringArray& /*files*/)
{
	somethingIsBeingDraggedOver = false;
	repaint();
}

void DragAndDropArea::filesDropped(const StringArray& files, int /*x*/, int /*y*/)
{
	message = "Files dropped: " + files.joinIntoString("\n");

	somethingIsBeingDraggedOver = false;
	repaint();
}