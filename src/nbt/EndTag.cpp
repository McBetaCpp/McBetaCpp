#include "nbt/EndTag.h"

EndTag::EndTag()
{

}

void EndTag::write(std::ostream &os)
{
}

void EndTag::load(std::istream &is)
{
}

byte_t EndTag::getId() const
{
	return TAG_End;
}

jstring EndTag::toString() const
{
	return u"END";
}
