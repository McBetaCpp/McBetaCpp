#include "User.h"

User::User(const jstring &name, const jstring &sessionId)
{
	this->name = name;
	this->sessionId = sessionId;
}
