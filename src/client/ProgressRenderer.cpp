#include "client/ProgressRenderer.h"

#include <thread>

#include "client/Minecraft.h"
#include "client/gui/ScreenSizeCalculator.h"
#include "client/renderer/Tesselator.h"

#include "lwjgl/Display.h"

#include "OpenGL.h"

ProgressRenderer::ProgressRenderer(Minecraft &minecraft) : minecraft(minecraft)
{

}

void ProgressRenderer::progressStart(const jstring &title)
{
	noAbort = false;
	_progressStart(title);
}

void ProgressRenderer::progressStartNoAbort(const jstring &title)
{
	noAbort = true;
	_progressStart(title);
}

void ProgressRenderer::_progressStart(const jstring &title)
{
	if (!minecraft.running)
	{
		if (this->noAbort) return;
		throw std::runtime_error("StopGameException");
	}

	this->title = title;

	ScreenSizeCalculator ssc(minecraft.width, minecraft.height);
	int_t screenWidth = ssc.getWidth();
	int_t screenHeight = ssc.getHeight();

	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, screenWidth, screenHeight, 0.0, 100.0, 300.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -200.0f);
}

void ProgressRenderer::progressStage(const jstring &status)
{
	if (!minecraft.running)
	{
		if (this->noAbort) return;
		throw std::runtime_error("StopGameException");
	}

	lastTime = 0;
	this->status = status;
	progressStagePercentage(-1);
	lastTime = 0;
}

void ProgressRenderer::progressStagePercentage(int_t i)
{
	if (!minecraft.running)
	{
		if (this->noAbort) return;
		throw std::runtime_error("StopGameException");
	}

	long_t now = System::currentTimeMillis();
	if (now - lastTime < 20) return;
	lastTime = now;

	ScreenSizeCalculator ssc(minecraft.width, minecraft.height);
	int_t screenWidth = ssc.getWidth();
	int_t screenHeight = ssc.getHeight();

	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, screenWidth, screenHeight, 0.0, 100.0, 300.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -200.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Tesselator &t = Tesselator::instance;
	int_t id = minecraft.textures.loadTexture(u"/gui/background.png");
	glBindTexture(GL_TEXTURE_2D, id);
	float s = 32.0f;
	t.begin();
	t.color(0x404040);
	t.vertexUV(0.0, screenHeight, 0.0, 0.0, screenHeight / s);
	t.vertexUV(screenWidth, screenHeight, 0.0, screenWidth / s, screenHeight / s);
	t.vertexUV(screenWidth, 0.0, 0.0, screenWidth / s, 0.0);
	t.vertexUV(0.0, 0.0, 0.0, 0.0, 0.0);
	t.end();

	if (i >= 0)
	{
		int_t w = 100;
		int_t h = 2;
		int_t x = screenWidth / 2 - w / 2;
		int_t y = screenHeight / 2 + 16;

		glDisable(GL_TEXTURE_2D);

		t.begin();
		t.color(0x808080);
		t.vertex(x, y, 0.0);
		t.vertex(x, y + h, 0.0);
		t.vertex(x + w, y + h, 0.0);
		t.vertex(x + w, y, 0.0);

		t.color(0x80FF80);
		t.vertex(x, y, 0.0);
		t.vertex(x, y + h, 0.0);
		t.vertex(x + i, y + h, 0.0);
		t.vertex(x + i, y, 0.0);
		t.end();

		glEnable(GL_TEXTURE_2D);
	}

	minecraft.font->drawShadow(title, (screenWidth - minecraft.font->width(title)) / 2, screenHeight / 2 - 4 - 16, 0xFFFFFF);
	minecraft.font->drawShadow(status, (screenWidth - minecraft.font->width(status)) / 2, screenHeight / 2 - 4 + 8, 0xFFFFFF);

	lwjgl::Display::update();

	std::this_thread::yield();
}
