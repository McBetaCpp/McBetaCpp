#include "Minecraft.h"

#include <string>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <filesystem>

#include "SharedConstants.h"

#include "client/renderer/Chunk.h"
#include "client/renderer/Tesselator.h"
#include "client/gui/ScreenSizeCalculator.h"
#include "client/gui/PauseScreen.h"
#include "client/title/TitleScreen.h"
#include "client/player/KeyboardInput.h"

#include "world/phys/Vec3.h"
#include "world/phys/AABB.h"
#include "world/level/chunk/ChunkCache.h"
#include "world/level/Level.h"
#include "world/level/tile/Tile.h"

#include "java/System.h"
#include "java/Runtime.h"
#include "java/File.h"

#include "util/Mth.h"

#include "lwjgl/Display.h"
#include "lwjgl/Keyboard.h"

#include "nbt/NbtIo.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

const jstring Minecraft::VERSION_STRING = u"Minecraft " + SharedConstants::VERSION_STRING;

std::array<long, 512> Minecraft::frameTimes = {};
std::array<long, 512> Minecraft::tickTimes = {};
int_t Minecraft::frameTimePos = 0;

Minecraft::Minecraft(int_t width, int_t height, bool fullscreen)
{
	// orgWidth = width;
	orgHeight = height;

	this->width = width;
	this->height = height;
	this->fullscreen = fullscreen;
}

void Minecraft::onCrash(const std::string &message, const std::exception &e)
{
	// SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Minecraft has crashed!", (message + ": " + e.what()).c_str(), nullptr);
}

void Minecraft::init()
{
	Tile::initTiles();

	// Setup LWJGL
	if (fullscreen)
	{
		lwjgl::Display::setFullscreen(true);
		width = lwjgl::Display::getDisplayMode().getWidth();
		height = lwjgl::Display::getDisplayMode().getHeight();
		if (width < 1)
			width = 1;
		if (height < 1)
			height = 1;
	}
	else
	{
		lwjgl::Display::setDisplayMode(lwjgl::DisplayMode(width, height));
	}

	lwjgl::Display::setTitle(u"Minecraft " + VERSION_STRING);

	lwjgl::Display::create();

	// TODO
	// EntityRenderDispatcher.instance.itemInHandRenderer = new ItemInHandRenderer(this);

	workingDirectory = getWorkingDirectory();

	options.open(workingDirectory.get());
	texturePackRepository.updateListAndSelect();

	font = std::unique_ptr<Font>(new Font(options, u"/font/default.png", textures));

	// renderLoadingScreen();

	checkGlError("Pre startup");

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glCullFace(GL_BACK);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	checkGlError("Startup");

	// TODO
	// soundEngine

	// TODO
	// dynamic textures
	//textures.addDynamicTexture();

	setScreen(std::make_shared<TitleScreen>(*this));
}

void Minecraft::renderLoadingScreen()
{
	ScreenSizeCalculator ssc(width, height);
	int_t w = ssc.getWidth();
	int_t h = ssc.getHeight();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, w, h, 0.0, 1000.0, 3000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2000.0f);

	glViewport(0, 0, width, height);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Tesselator &tesselator = Tesselator::instance;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_FOG);

	glBindTexture(GL_TEXTURE_2D, textures.loadTexture(u"/title/mojang.png"));

	tesselator.begin();
	tesselator.color(0xFFFFFF);
	tesselator.vertexUV(0.0, height, 0.0, 0.0, 0.0);
	tesselator.vertexUV(width, height, 0.0, 0.0, 0.0);
	tesselator.vertexUV(width, 0.0, 0.0, 0.0, 0.0);
	tesselator.vertexUV(0.0, 0.0, 0.0, 0.0, 0.0);
	tesselator.end();

	short_t gw = 256;
	short_t gh = 256;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	tesselator.color(0xFFFFFF);
	blit((width / 2 - gw) / 2, (height / 2 - gh) / 2, 0, 0, gw, gh);

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);

	lwjgl::Display::swapBuffers();
}

void Minecraft::blit(int_t x, int_t y, int_t sx, int_t sy, int_t w, int_t h)
{
	const float us = 1.0f / 256.0f;
	const float vs = 1.0f / 256.0f;
	Tesselator &t = Tesselator::instance;
	t.begin();
	t.vertexUV(x + 0, y + h, 0.0, (sx + 0) * us, (sy + h) * vs);
	t.vertexUV(x + w, y + h, 0.0, (sx + w) * us, (sy + h) * vs);
	t.vertexUV(x + w, y + 0, 0.0, (sx + w) * us, (sy + 0) * vs);
	t.vertexUV(x + 0, y + 0, 0.0, (sx + 0) * us, (sy + 0) * vs);
	t.end();
}

std::unique_ptr<File> Minecraft::getWorkingDirectory() {
    #ifdef _WIN32
        wchar_t path[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, path, MAX_PATH);

        jstring wide((char16_t*)path);
        auto pos = wide.find_last_of(u'\\');

        if (pos != jstring::npos)
            wide = wide.substr(0, pos);

        std::unique_ptr<File> file(File::open(wide));
        return file;

    #else // Linux and Unix-like systems
        char path[PATH_MAX] = {};
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);

        if (count == -1) {
            // Handle error, return nullptr or throw an exception
            return nullptr;
        }

        std::string execPath(path, count);
        auto pos = execPath.find_last_of('/');

        if (pos != std::string::npos)
            execPath = execPath.substr(0, pos);

        jstring wide = String::fromUTF8(execPath);

        std::unique_ptr<File> file(File::open(wide));
        return file;
    #endif
}

void Minecraft::setScreen(std::shared_ptr<Screen> screen)
{
	if (this->screen != nullptr)
		this->screen->removed();

	if (screen == nullptr && level == nullptr)
		screen = std::make_shared<TitleScreen>(*this);
	else if (screen == nullptr) // TODO player check
		;// TODO death

	this->screen = std::move(screen);
	if (this->screen != nullptr)
	{
		releaseMouse();
		ScreenSizeCalculator ssc(width, height);
		int_t w = ssc.getWidth();
		int_t h = ssc.getHeight();
		this->screen->init(w, h);
		noRender = false;
	}
	else
	{
		grabMouse();
	}
}

void Minecraft::checkGlError(const std::string &at)
{
	GLenum error_code = glGetError();
	if (error_code != GL_NO_ERROR)
	{
		std::string error_string;
		switch (error_code)
		{
			default:
				error_string = "unknown error code";
				break;
			case GL_NO_ERROR:
				error_string = "no error";
				break;
			case GL_INVALID_ENUM:
				error_string = "invalid enumerant";
				break;
			case GL_INVALID_VALUE:
				error_string = "invalid value";
				break;
			case GL_INVALID_OPERATION:
				error_string = "invalid operation";
				break;
			case GL_STACK_OVERFLOW:
				error_string = "stack overflow";
				break;
			case GL_STACK_UNDERFLOW:
				error_string = "stack underflow";
				break;
			case GL_OUT_OF_MEMORY:
				error_string = "out of memory";
				break;
		}

		std::cout << "########## GL ERROR ##########\n";
		std::cout << "@ " << at << '\n';
		std::cout << error_code << ": " << error_string << '\n';
	}
}

void Minecraft::run()
{
	// init
	running = true;

#ifdef NDEBUG
	try
	{
#endif
		init();
#ifdef NDEBUG
	}
	catch (std::exception &e)
	{
		onCrash("Failed to start game", e);
		return;
	}
#endif

#ifdef NDEBUG
	try
	{
#endif
		long_t fpsMs = System::currentTimeMillis();
		int_t fpsFrames = 0;
		int_t fpsTicks = 0;

		while (running)
		{
			AABB::resetPool();
			Vec3::resetPool();

			if (lwjgl::Display::isCloseRequested())
				stop();

			if (pause && level != nullptr)
			{
				float aO = timer.a;
				timer.advanceTime();
				timer.a = aO;
			}
			else
			{
				timer.advanceTime();
			}

			// Tick game
			long_t tickNano = System::nanoTime();
			for (byte_t i = 0; i < timer.ticks; i++)
			{
				ticks++;
				fpsTicks++;
				tick();
			}
			long_t tickNanos = System::nanoTime() - tickNano;

			checkGlError("Pre render");

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_ALPHA_TEST);

			if (level != nullptr && !level->isOnline)
				level->updateLights();
			// Something was probably here at some point
			if (level != nullptr && level->isOnline)
				level->updateLights();

			if (options.limitFramerate)
				std::this_thread::sleep_for(std::chrono::milliseconds(5));

			if (!lwjgl::Keyboard::isKeyDown(lwjgl::Keyboard::KEY_F7))
				lwjgl::Display::update();

			if (!noRender)
			{
				if (gameMode != nullptr)
					gameMode->render(timer.a);
				gameRenderer.render(timer.a);
			}

			if (!lwjgl::Display::isActive())
			{
				if (level != nullptr && screen == nullptr)
				pauseGame();
				if (fullscreen)
					toggleFullscreen();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			if (lwjgl::Keyboard::isKeyDown(lwjgl::Keyboard::KEY_F3))
			{
				renderFpsMeter(tickNanos);
			}
			else
			{
				lastTimer = System::nanoTime();
			}

			std::this_thread::yield();
			if (lwjgl::Keyboard::isKeyDown(lwjgl::Keyboard::KEY_F7))
				lwjgl::Display::update();

			if (!fullscreen)
			{
				int_t w = lwjgl::Display::getWidth();
				int_t h = lwjgl::Display::getHeight();
				if (w != width || h != height)
				{
					width = w;
					height = h;
					if (width <= 0)
						width = 1;
					if (height <= 0)
						height = 1;
					resize(width, height);
				}
			}

			checkGlError("Post render");
			fpsFrames++;

			pause = !isOnline() && screen != nullptr && screen->isPauseScreen();

			while (System::currentTimeMillis() >= fpsMs + 1000)
			{
				fpsString = String::fromUTF8(std::to_string(fpsFrames) + " fps, " + std::to_string(Chunk::updates) + " chunk updates");
				Chunk::updates = 0;
				fpsMs += 1000;
				fpsFrames = 0;
			}
		}
#ifdef NDEBUG
	}
	catch (std::exception &e)
	{
		onCrash("Unexpected error", e);
		return;
	}
#endif
}

void Minecraft::renderFpsMeter(long tickNanos)
{
	// Update times
	long_t target = 16666666LL; // 1 / 60
	if (lastTimer == -1)
		lastTimer = System::nanoTime();

	long_t now = System::nanoTime();

	tickTimes[frameTimePos & (frameTimes.size() - 1)] = tickNanos;
	frameTimes[frameTimePos & (frameTimes.size() - 1)] = now - lastTimer;
	frameTimePos++;

	lastTimer = now;

	// Draw
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, height, 0.0, 1000.0, 3000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2000.0f);

	glLineWidth(1.0f);
	glDisable(GL_TEXTURE_2D);

	Tesselator &t = Tesselator::instance;
	t.begin(GL_QUADS);

	int_t targetHeight = target / 200000LL;

	t.color(0x20000000);
	t.vertex(0.0, this->height - targetHeight, 0.0);
	t.vertex(0.0, this->height, 0.0);
	t.vertex(frameTimes.size(), this->height, 0.0);
	t.vertex(frameTimes.size(), this->height - targetHeight, 0.0);

	t.color(0x20200000);
	t.vertex(0.0, this->height - targetHeight * 2, 0.0);
	t.vertex(0.0, this->height - targetHeight, 0.0);
	t.vertex(frameTimes.size(), this->height - targetHeight, 0.0);
	t.vertex(frameTimes.size(), this->height - targetHeight * 2, 0.0);

	t.end();

	int_t total = 0;
	for (int_t i = 0; i < frameTimes.size(); i++)
		total += frameTimes[i];
	int_t avg = total / 200000LL / frameTimes.size();

	t.begin(GL_QUADS);
	t.color(0x20400000);
	t.vertex(0.0, (height - avg), 0.0);
	t.vertex(0.0, height, 0.0);
	t.vertex(frameTimes.size(), height, 0.0);
	t.vertex(frameTimes.size(), (height - avg), 0.0);
	t.end();

	t.begin(GL_LINES);
	for (int_t i = 0; i < frameTimes.size(); i++)
	{
		int_t x = ((i - frameTimePos) & (frameTimes.size() - 1)) * 255 / frameTimes.size();
		int_t m = x * x / 255;
		m = m * m / 255;
		int_t n = m * m / 255;
		n = n * n / 255;

		if (frameTimes[i] > target)
			t.color(0xFF000000 + (m * 0x010000));
		else
			t.color(0xFF000000 + (m * 0x0100));

		long_t ft = frameTimes[i] / 200000LL;
		long_t tt = tickTimes[i] / 200000LL;

		t.vertex((i + 0.5F), (static_cast<float>(height - ft) + 0.5F), 0.0);
		t.vertex((i + 0.5F), (height + 0.5F), 0.0);

		t.color(0xFF000000 + (m * 0x010000) + (m * 0x0100) + (m * 0x01));
		t.vertex((i + 0.5F), (static_cast<float>(height - ft) + 0.5F), 0.0);
		t.vertex((i + 0.5F), (static_cast<float>(height - ft - tt) + 0.5F), 0.0);
	}
	t.end();

	glEnable(GL_TEXTURE_2D);
}

void Minecraft::stop()
{
	running = false;
}

void Minecraft::grabMouse()
{
	if (!lwjgl::Display::isActive())
		return;
	if (mouseGrabbed)
		return;
	mouseGrabbed = true;
	mouseHandler.grab();
	setScreen(nullptr);
	lastClickTick = ticks + 10000;
}

void Minecraft::releaseMouse()
{
	if (!mouseGrabbed)
		return;
	mouseGrabbed = false;
	mouseHandler.release();
}

void Minecraft::pauseGame()
{
	if (this->screen != nullptr)
		return;
	setScreen(std::make_shared<PauseScreen>(*this));
}

void Minecraft::handleMouseDown(int_t button, bool down)
{
	if (gameMode->instaBuild)
		return;
	if (button == 0 && missTime > 0)
		return;
	if (down && hitResult.type == HitResult::Type::TILE && button == 0)
	{
		int_t x = hitResult.x;
		int_t y = hitResult.y;
		int_t z = hitResult.z;
		gameMode->continueDestroyBlock(x, y, z, hitResult.f);
		// TODO
		// particleEngine
	}
	else
	{
		gameMode->stopDestroyBlock();
	}
}

void Minecraft::handleMouseClick(int_t button)
{
	if (button == 0 && missTime > 0)
		return;
	if (button == 0)
		player->swing();
}

void Minecraft::toggleFullscreen()
{
	fullscreen = !fullscreen;
	std::cout << "Toggle fullscreen!\n";

	lwjgl::Display::setFullscreen(fullscreen);
	width = lwjgl::Display::getWidth();
	height = lwjgl::Display::getHeight();
	if (width <= 0)
		width = 1;
	if (height <= 0)
		height = 1;

	releaseMouse();
	lwjgl::Display::update();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	if (fullscreen)
		grabMouse();

	if (screen != nullptr)
	{
		releaseMouse();
		resize(width, height);
	}

	std::cout << "Size: " << width << ", " << height << '\n';
}

void Minecraft::resize(int_t w, int_t h)
{
	if (w <= 0)
		w = 1;
	if (h <= 0)
		h = 1;

	width = w;
	height = h;

	if (screen != nullptr)
	{
		ScreenSizeCalculator ssc(width, height);
		int_t w = ssc.getWidth();
		int_t h = ssc.getHeight();
		screen->init(w, h);
	}
}

void Minecraft::handleGrabTexture()
{
	if (hitResult.type != HitResult::Type::NONE)
	{
		int_t i = level->getTile(hitResult.x, hitResult.y, hitResult.z);
		// TODO
	}
}

void Minecraft::tick()
{

	gameRenderer.pick(1.0f);

	// Update chunk caching
	if (player != nullptr)
	{
		player->prepareForTick();

		auto chunkSource = level->getChunkSource();
		if (chunkSource->isChunkCache())
		{
			ChunkCache &chunkCache = static_cast<ChunkCache &>(*chunkSource);
			int_t x = Mth::floor(static_cast<float>(static_cast<int_t>(player->x))) >> 4;
			int_t z = Mth::floor(static_cast<float>(static_cast<int_t>(player->z))) >> 4;
			chunkCache.centerOn(x, z);
		}
	}

	//
	if (!pause && level != nullptr)
		gameMode->tick();

	// Texture update
	glBindTexture(GL_TEXTURE_2D, textures.loadTexture(u"/terrain.png"));
	if (!pause)
		textures.tick();

	// Show death screen when dead
	if (screen == nullptr && player != nullptr && player->health == 0)
		setScreen(nullptr);

	// Tick screen
	if (screen != nullptr)
		lastClickTick = ticks + 10000;
	if (screen != nullptr)
	{
		std::shared_ptr<Screen> screen_ptr = screen; // Ensure that the screen is not deleted while processing events
		screen->updateEvents();
		if (screen != nullptr)
			screen->tick();
	}

	// Event processing
	if (screen == nullptr || screen->passEvents)
	{
		while (lwjgl::Mouse::next())
		{
			long_t tickDelta = System::currentTimeMillis() - lastTickTime;
			if (tickDelta > 200)
				continue;

			int_t dwheel = lwjgl::Mouse::getEventDWheel();
			// TODO inventory

			if (screen == nullptr)
			{
				if (!mouseGrabbed && lwjgl::Mouse::getEventButtonState())
				{
					grabMouse();
					continue;
				}
				if (lwjgl::Mouse::getEventButton() == 0 && lwjgl::Mouse::getEventButtonState())
				{
					handleMouseClick(0);
					lastClickTick = ticks;
				}
				if (lwjgl::Mouse::getEventButton() == 1 && lwjgl::Mouse::getEventButtonState())
				{
					handleMouseClick(1);
					lastClickTick = ticks;
				}
				if (lwjgl::Mouse::getEventButton() == 2 && lwjgl::Mouse::getEventButtonState())
					handleGrabTexture();
				continue;
			}
			if (screen != nullptr)
				screen->mouseEvent();
		}

		if (missTime > 0)
			missTime--;

		while (lwjgl::Keyboard::next())
		{
			player->setKey(lwjgl::Keyboard::getEventKey(), lwjgl::Keyboard::getEventKeyState());

			if (lwjgl::Keyboard::getEventKeyState())
			{
				if (lwjgl::Keyboard::getEventKey() == lwjgl::Keyboard::KEY_F11)
				{
					toggleFullscreen();
					continue;
				}

				if (screen != nullptr)
				{
					screen->keyboardEvent();
				}
				else
				{
					if (lwjgl::Keyboard::getEventKey() == lwjgl::Keyboard::KEY_ESCAPE)
						pauseGame();
					if (lwjgl::Keyboard::getEventKey() == lwjgl::Keyboard::KEY_S && lwjgl::Keyboard::isKeyDown(lwjgl::Keyboard::KEY_F3))
						reloadSound();
					if (lwjgl::Keyboard::getEventKey() == lwjgl::Keyboard::KEY_F5)
						options.thirdPersonView = !options.thirdPersonView;
				}

				for (int_t i = 0; i < 9; i++)
				{
					// TODO: inventory
					// if (lwjgl::Keyboard::getEventKey() == lwjgl::Keyboard::KEY_1 + i)
					// 	player->inventory.selected = i;
				}

				if (lwjgl::Keyboard::getEventKey() == options.keyFog.key)
					options.toggle(Options::Option::RENDER_DISTANCE, (lwjgl::Keyboard::isKeyDown(lwjgl::Keyboard::KEY_LSHIFT) || lwjgl::Keyboard::isKeyDown(lwjgl::Keyboard::KEY_RSHIFT)) ? -1 : 1);
			}
		}

		if (screen == nullptr)
		{
			if (lwjgl::Mouse::isButtonDown(0) && static_cast<float>(ticks - lastClickTick) >= (timer.ticksPerSecond / 4.0f) && mouseGrabbed)
			{
				handleMouseClick(0);
				lastClickTick = ticks;
			}
			if (lwjgl::Mouse::isButtonDown(1) && static_cast<float>(ticks - lastClickTick) >= (timer.ticksPerSecond / 4.0f) && mouseGrabbed)
			{
				handleMouseClick(1);
				lastClickTick = ticks;
			}
		}

		handleMouseDown(0, (screen == nullptr && lwjgl::Mouse::isButtonDown(0) && mouseGrabbed));
	}

	// Level tick
	if (level != nullptr)
	{
		if (player != nullptr)
		{
			recheckPlayerIn++;
			if (recheckPlayerIn == 30)
			{
				recheckPlayerIn = 0;
				level->ensureAdded(player);
			}
		}

		level->difficulty = options.difficulty;
		if (level->isOnline)
			level->difficulty = 3;

		if (!pause)
			gameRenderer.tick();
		if (!pause)
			levelRenderer.tick();
		if (!pause)
			level->tickEntities();
		if (!pause || isOnline())
		{
			level->setSpawnSettings(options.difficulty > 0, true);
			level->tick();
		}
		if (!pause && level != nullptr)
			level->animateTick(Mth::floor(player->x), Mth::floor(player->y), Mth::floor(player->z));
		// TODO
		// if (!pause)
		// 	particleEngine.tick();
	}

	lastTickTime = System::currentTimeMillis();
}

void Minecraft::reloadSound()
{
	std::cout << "FORCING RELOAD!\n";
	// TODO
}

bool Minecraft::isOnline()
{
	return level != nullptr && level->isOnline;
}

void Minecraft::selectLevel(const jstring &name)
{
	setLevel(nullptr);

	std::unique_ptr<File> saves(File::open(*getWorkingDirectory(), u"saves"));
	std::shared_ptr<Level> new_level = std::make_shared<Level>(saves.release(), name);
	if (new_level->isNew)
		setLevel(new_level, u"Generating level");
	else
		setLevel(new_level, u"Loading level");
}

void Minecraft::toggleDimension()
{
	/*
	if (player->dimension == -1)
		player->dimension = 0;
	else
		player->dimension = -1;

	level->removeEntity(player);
	player->removed = true;
	*/
}

void Minecraft::setLevel(std::shared_ptr<Level> level)
{
	setLevel(level, u"");
}

void Minecraft::setLevel(std::shared_ptr<Level> level, const jstring &title)
{
	setLevel(level, title, nullptr);
}

void Minecraft::setLevel(std::shared_ptr<Level> level, const jstring &title, std::shared_ptr<Player> player)
{
	progressRenderer->progressStart(title);
	progressRenderer->progressStage(u"");

	if (this->level != nullptr)
		this->level->forceSave(progressRenderer);
	this->level = level;

	std::cout << "Player is " << this->player << '\n';

	if (level != nullptr)
	{
		gameMode->initLevel(level);

		if (!isOnline())
		{
			if (player == nullptr)
				this->player = nullptr; // level->findSubclassOf<LocalPlayer>();
		}
		else if (this->player != nullptr)
		{
			this->player->resetPos();
			if (level != nullptr)
				level->addEntity(this->player);
		}

		if (!level->isOnline)
			prepareLevel(title);

		std::cout << "Player is now " << this->player << '\n';

		if (this->player == nullptr)
		{
			this->player = std::static_pointer_cast<LocalPlayer>(gameMode->createPlayer(*level));
			this->player->resetPos();
			gameMode->initPlayer(this->player);
		}
		this->player->input = std::unique_ptr<KeyboardInput>(new KeyboardInput(options));

		levelRenderer.setLevel(level);

		gameMode->adjustPlayer(this->player);
		if (player != nullptr)
			level->clearLoadedPlayerData();

		std::shared_ptr<ChunkSource> chunkSource = level->getChunkSource();
		if (chunkSource->isChunkCache())
		{
			ChunkCache &chunkCache = static_cast<ChunkCache &>(*chunkSource);
			int_t x = Mth::floor(static_cast<float>(static_cast<int_t>(this->player->x))) >> 4;
			int_t z = Mth::floor(static_cast<float>(static_cast<int_t>(this->player->z))) >> 4;
			chunkCache.centerOn(x, z);
		}

		level->loadPlayer(this->player);

		if (level->isNew)
			level->forceSave(progressRenderer);
	}
	else
	{
		this->player = nullptr;
	}
}

void Minecraft::prepareLevel(const jstring &title)
{
	progressRenderer->progressStart(title);
	progressRenderer->progressStage(u"Building terrain");

	short_t radius = 128;
	int_t i = 0;
	int_t max = radius * 2 / 16 + 1;
	max *= max;

	std::shared_ptr<ChunkSource> source = level->getChunkSource();
	int_t xSpawn = level->xSpawn;
	int_t zSpawn = level->zSpawn;
	if (player != nullptr)
	{
		xSpawn = static_cast<int_t>(player->x);
		zSpawn = static_cast<int_t>(player->z);
	}

	if (source->isChunkCache())
	{
		ChunkCache &cache = static_cast<ChunkCache &>(*source);
		cache.centerOn(xSpawn >> 4, zSpawn >> 4);
	}

	for (int_t x = -radius; x <= radius; x += 16)
	{
		for (int_t z = -radius; z <= radius; z += 16)
		{
			progressRenderer->progressStagePercentage(i++ * 100 / max);
			level->getTile(xSpawn + x, 64, zSpawn + z);

			while (level->updateLights());
		}
	}

	progressRenderer->progressStage(u"Simulating world for a bit");
}

jstring Minecraft::gatherStats1()
{
	return levelRenderer.gatherStats1();
}

jstring Minecraft::gatherStats2()
{
	return levelRenderer.gatherStats2();
}

jstring Minecraft::gatherStats4()
{
	return level->gatherChunkSourceStats();
}

jstring Minecraft::gatherStats3()
{
	return u"P: 0. T: " + level->gatherStats();
}

void Minecraft::respawnPlayer()
{

}

void Minecraft::start(const jstring *name, const jstring *sessionId)
{
	startAndConnectTo(name, sessionId, nullptr);
}

void Minecraft::startAndConnectTo(const jstring *name, const jstring *sessionId, const jstring *ip)
{
	std::unique_ptr<Minecraft> minecraft(new Minecraft(854, 480, false));

	minecraft->serverDomain = u"www.minecraft.net";
	if (name != nullptr && sessionId != nullptr)
		minecraft->user = std::unique_ptr<User>(new User(*name, *sessionId));
	else
		minecraft->user = std::unique_ptr<User>(new User(u"Player" + String::fromUTF8(std::to_string(System::currentTimeMillis() % 1000)), u""));

	if (ip != nullptr)
	{
		// TODO
	}

	minecraft->run();
	// minecraft.release(); // TODO: fix destructor order stuff
}
