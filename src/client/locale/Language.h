#pragma once

#include <unordered_map>

#include "java/String.h"

class Language
{
private:
	static Language singleton;
	std::unordered_map<jstring, jstring> elements;

	Language();

	void load(const jstring &name);

public:
	static Language &getInstance();

	jstring getElement(const jstring &elementId) const;
	jstring getElementName(const jstring &elementId) const;
	jstring getElementDesc(const jstring &elementId) const;
};
