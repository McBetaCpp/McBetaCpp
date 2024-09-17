#pragma once

#include "util/ProgressListener.h"

#include "java/System.h"

class Minecraft;

class ProgressRenderer : public ProgressListener
{
private:
	jstring status;
	Minecraft &minecraft;
	jstring title;

	long_t lastTime = System::currentTimeMillis();
	bool noAbort = false;

public:
	ProgressRenderer(Minecraft &minecraft);

	void progressStart(const jstring &title) override;
	void progressStartNoAbort(const jstring &title) override;

	void _progressStart(const jstring &title);

	void progressStage(const jstring &status) override;
	void progressStagePercentage(int_t i) override;
};
