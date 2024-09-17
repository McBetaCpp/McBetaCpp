#pragma once

#include "java/String.h"

class User
{
public:
	jstring name;
	jstring sessionId;
	jstring mpPassword;

	User(const jstring &name, const jstring &sessionId);
};
