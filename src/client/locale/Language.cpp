#include "client/locale/Language.h"

#include "java/Resource.h"

Language Language::singleton;

Language::Language()
{
	load(u"/lang/en_US.lang");
}

void Language::load(const jstring &name)
{
	std::unique_ptr<std::istream> is(Resource::getResource(name));
	std::string line;

	while (std::getline(*is, line))
	{
		size_t pos = line.find_first_of('=');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			if (value.back() == '\r')
				value.pop_back();

			elements[String::fromUTF8(key)] = String::fromUTF8(value);
		}
	}
}

Language &Language::getInstance()
{
	return singleton;
}

jstring Language::getElement(const jstring &elementId) const
{
	auto it = elements.find(elementId);
	if (it != elements.end())
		return it->second;
	return u"";
}
jstring Language::getElementName(const jstring &elementId) const
{
	return getElement(elementId + u".name");
}
jstring Language::getElementDesc(const jstring &elementId) const
{
	return getElement(elementId + u".desc");
}
