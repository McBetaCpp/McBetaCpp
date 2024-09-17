#pragma once

#include "java/String.h"

template <typename T>
class Descriptive
{
	virtual T &setDescriptionId(const jstring &id) = 0;
	virtual jstring getDescriptionId() const = 0;
};
