#pragma once

#include <string>
#include <iostream>

#include "java/Resource.h"
#include "java/String.h"

class Minecraft;

class TexturePack
{
public:
	jstring name;
	jstring desc1;
	jstring desc2;
	jstring id;

	virtual ~TexturePack() {}

	virtual void select();
	virtual void deselect();
	virtual void load(Minecraft &minecraft);
	virtual void unload(Minecraft &minecraft);
	virtual void bindTexture(Minecraft &minecraft);
	virtual std::istream *getResource(const jstring &name);
};
