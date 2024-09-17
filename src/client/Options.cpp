#include "client/Options.h"

#include "client/Minecraft.h"
#include "client/locale/Language.h"

#include "lwjgl/Keyboard.h"

Options::Option::Element Options::Option::MUSIC = { true, false, u"options.music" };
Options::Option::Element Options::Option::SOUND = { true, false, u"options.sound" };
Options::Option::Element Options::Option::INVERT_MOUSE = { false, true, u"options.invertMouse" };
Options::Option::Element Options::Option::SENSITIVITY = { true, false, u"options.sensitivity" };
Options::Option::Element Options::Option::RENDER_DISTANCE = { false, false, u"options.renderDistance" };
Options::Option::Element Options::Option::VIEW_BOBBING = { false, true, u"options.viewBobbing" };
Options::Option::Element Options::Option::ANAGLYPH = { false, true, u"options.anaglyph" };
Options::Option::Element Options::Option::LIMIT_FRAMERATE = { false, true, u"options.limitFramerate" };
Options::Option::Element Options::Option::DIFFICULTY = { false, false, u"options.difficulty" };
Options::Option::Element Options::Option::GRAPHICS = { false, false, u"options.graphics" };

const char16_t *Options::RENDER_DISTANCE_NAMES[] = {
	u"options.renderDistance.far",
	u"options.renderDistance.normal",
	u"options.renderDistance.short",
	u"options.renderDistance.tiny"
};
const char16_t *Options::DIFFICULTY_NAMES[] = {
	u"options.difficulty.peaceful",
	u"options.difficulty.easy",
	u"options.difficulty.normal",
	u"options.difficulty.hard"
};

Options::Options(Minecraft &minecraft) : minecraft(minecraft)
{

}

void Options::open(File *optionsFile)
{
	this->optionsFile.reset(File::open(*optionsFile, u"options.txt"));
	load();
	save();
}

jstring Options::getKeyDescription(int_t i)
{
	Language &l = Language::getInstance();
	return l.getElement(keyMappings[i]->name);
}

jstring Options::getKeyMessage(int_t i)
{
	return lwjgl::Keyboard::getKeyName(keyMappings[i]->key);
}

void Options::setKey(int_t i, int_t key)
{
	keyMappings[i]->key = key;
	save();
}

void Options::set(Option::Element &option, float value)
{
	if (&option == &Option::MUSIC)
		music = value;
	else if (&option == &Option::SOUND)
		sound = value;
	else if (&option == &Option::SENSITIVITY)
		mouseSensitivity = value;
}

void Options::toggle(Option::Element &option, int_t add)
{
	if (&option == &Option::INVERT_MOUSE)
		invertYMouse = !invertYMouse;
	else if (&option == &Option::RENDER_DISTANCE)
		viewDistance = (viewDistance + add) & 3;
	else if (&option == &Option::VIEW_BOBBING)
		bobView = !bobView;
	else if (&option == &Option::ANAGLYPH)
	{
		anaglyph3d = !anaglyph3d;
		minecraft.textures.reloadAll();
	}
	else if (&option == &Option::LIMIT_FRAMERATE)
		limitFramerate = !limitFramerate;
	else if (&option == &Option::DIFFICULTY)
		difficulty = (difficulty + add) & 3;
	else if (&option == &Option::GRAPHICS)
	{
		fancyGraphics = !fancyGraphics;
		minecraft.levelRenderer.allChanged();
	}

	save();
}

float Options::getProgressValue(Option::Element &option)
{
	if (&option == &Option::MUSIC)
		return music;
	else if (&option == &Option::SOUND)
		return sound;
	else if (&option == &Option::SENSITIVITY)
		return mouseSensitivity;
	return 0.0f;
}

bool Options::getBooleanValue(Option::Element &option)
{
	if (&option == &Option::INVERT_MOUSE)
		return invertYMouse;
	else if (&option == &Option::VIEW_BOBBING)
		return bobView;
	else if (&option == &Option::ANAGLYPH)
		return anaglyph3d;
	else if (&option == &Option::LIMIT_FRAMERATE)
		return limitFramerate;
	return false;
}

jstring Options::getMessage(Option::Element &option)
{
	Language &l = Language::getInstance();
	jstring result = l.getElement(option.captionId) + u": ";

	if (option.isProgress)
	{
		float progress = getProgressValue(option);
		if (&option == &Option::SENSITIVITY)
		{
			if (progress == 0.0f)
				return result + l.getElement(u"options.sensitivity.min");
			else if (progress == 1.0f)
				return result + l.getElement(u"options.sensitivity.max");
			else
				return result + String::fromUTF8(std::to_string(static_cast<int_t>(progress * 200.0f))) + u'%';
		}
		else
		{
			if (progress == 0.0f)
				return result + l.getElement(u"options.off");
			else
				return result + String::fromUTF8(std::to_string(static_cast<int_t>(progress * 100.0f))) + u'%';
		}
	}
	else if (option.isBoolean)
	{
		bool value = getBooleanValue(option);
		if (value)
			return result + l.getElement(u"options.on");
		else
			return result + l.getElement(u"options.off");
	}
	else if (&option == &Option::RENDER_DISTANCE)
	{
		return result + l.getElement(RENDER_DISTANCE_NAMES[viewDistance]);
	}
	else if (&option == &Option::DIFFICULTY)
	{
		return result + l.getElement(DIFFICULTY_NAMES[difficulty]);
	}
	else if (&option == &Option::GRAPHICS)
	{
		if (fancyGraphics)
			return result + l.getElement(u"options.graphics.fancy");
		else
			return result + l.getElement(u"options.graphics.fast");
	}

	return result;
}

void Options::load()
{
	if (!optionsFile->exists())
		return;

	std::unique_ptr<std::istream> is(optionsFile->toStreamIn());
	if (!is)
		return;
	
	std::string line;
	while (std::getline(*is, line))
	{
		if (line.back() == '\r')
			line.pop_back();

		size_t pos = line.find(':');
		if (pos == std::string::npos)
			continue;

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		if (key == "music")
			music = readFloat(value);
		if (key == "sound")
			sound = readFloat(value);
		if (key == "mouseSensitivity")
			mouseSensitivity = readFloat(value);
		if (key == "invertYMouse")
			invertYMouse = value == "true";
		if (key == "viewDistance")
			viewDistance = std::stoi(value);
		if (key == "bobView")
			bobView = value == "true";
		if (key == "anaglyph3d")
			anaglyph3d = value == "true";
		if (key == "limitFramerate")
			limitFramerate = value == "true";
		if (key == "difficulty")
			difficulty = std::stoi(value);
		if (key == "fancyGraphics")
			fancyGraphics = value == "true";
		if (key == "skin")
			skin = String::fromUTF8(value);
		if (key == "lastServer")
			lastMpIp = String::fromUTF8(value);

		jstring jkey = String::fromUTF8(key);
		for (int_t i = 0; i < keyMappings.size(); i++)
		{
			if (jkey == u"key_" + keyMappings[i]->name)
				keyMappings[i]->key = std::stoi(value);
		}
	}
}

float Options::readFloat(const std::string &s)
{
	if (s == "true")
		return 1.0f;
	else if (s == "false")
		return 0.0f;
	else
		return std::stof(s);
}

void Options::save()
{
	std::unique_ptr<std::ostream> os(optionsFile->toStreamOut());
	if (!os)
		return;

	*os << std::boolalpha;
	*os << "music:" << music << '\n';
	*os << "sound:" << sound << '\n';
	*os << "invertYMouse:" << invertYMouse << '\n';
	*os << "mouseSensitivity:" << mouseSensitivity << '\n';
	*os << "viewDistance:" << viewDistance << '\n';
	*os << "bobView:" << bobView << '\n';
	*os << "anaglyph3d:" << anaglyph3d << '\n';
	*os << "limitFramerate:" << limitFramerate << '\n';
	*os << "difficulty:" << difficulty << '\n';
	*os << "fancyGraphics:" << fancyGraphics << '\n';
	*os << "skin:" << String::toUTF8(skin) << '\n';
	*os << "lastServer:" << String::toUTF8(lastMpIp) << '\n';

	for (int_t i = 0; i < keyMappings.size(); i++)
		*os << "key_" << String::toUTF8(keyMappings[i]->name) << ':' << keyMappings[i]->key << '\n';
}
