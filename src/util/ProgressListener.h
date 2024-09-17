#pragma once

#include "java/Type.h"
#include "java/String.h"

class ProgressListener
{
public:
	virtual void progressStartNoAbort(const jstring &title) = 0;
	virtual void progressStart(const jstring &title) = 0;
	virtual void progressStage(const jstring &status) = 0;
	virtual void progressStagePercentage(int_t i) = 0;
};
