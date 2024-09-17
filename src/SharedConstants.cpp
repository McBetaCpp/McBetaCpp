#include "SharedConstants.h"

#include "java/Resource.h"

namespace SharedConstants
{

static jstring readAcceptableChars()
{
	std::string result;

	std::unique_ptr<std::istream> is(Resource::getResource(u"/font.txt"));
	std::string line;
	while (std::getline(*is, line))
	{
		if (line.empty() || line[0] == '#')
			continue;
		if (line.back() == '\r')
			line.pop_back();
		result.append(line);
	}
	return String::fromUTF8(result);
}

const int NETWORK_PROTOCOL_VERSION = 8;
const int maxChatLength = 100;
const jstring acceptableLetters = readAcceptableChars();

}
