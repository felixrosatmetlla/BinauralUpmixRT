#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DragAndDropArea : public Component, public FileDragAndDropTarget
{
public:
	DragAndDropArea(String message);

	void paint(Graphics& g) override;

	// Files drag and drop methods
	bool isInterestedInFileDrag(const StringArray& /*files*/) override;

	void fileDragEnter(const StringArray& /*files*/, int /*x*/, int /*y*/) override;

	void fileDragMove(const StringArray& /*files*/, int /*x*/, int /*y*/) override;

	void fileDragExit(const StringArray& /*files*/) override;

	void filesDropped(const StringArray& files, int /*x*/, int /*y*/) override;

	Array<File> filesToRead(const StringArray& files);

	Array<File> filesReaded;

private:
	String message;

	bool somethingIsBeingDraggedOver;};
