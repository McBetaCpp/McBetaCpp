#include "lwjgl/Display.h"

#include <tamtypes.h>
#include <libgs.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>

namespace lwjgl
{
namespace Display
{

static bool close_requested = false;
static DisplayMode current_display_mode(640, 448);
static bool initialized = false;

// GS display buffer setup
static GsDisplayMode gs_disp_mode;
static GsEnv gs_env;
static qword_t *packet_buffer = nullptr;
static int packet_index = 0;

void setDisplayMode(const DisplayMode &display_mode)
{
	current_display_mode = display_mode;
	// PS2 uses fixed resolution modes
}

DisplayMode getDisplayMode()
{
	return current_display_mode;
}

void setTitle(const jstring &string)
{
	// PS2 doesn't have window titles
}

void setFullscreen(bool fullscreen)
{
	// PS2 is always fullscreen
}

bool isCloseRequested()
{
	return close_requested;
}

bool isVisible()
{
	return initialized;
}

bool isActive()
{
	return initialized;
}

void processMessages()
{
	// Check for pad input to exit
	// This would be implemented with pad library
}

void swapBuffers()
{
	if (!initialized)
		return;
	
	// Wait for VSync
	GsSwapWait();
	
	// Swap buffers
	GsSwap();
}

void update(bool doProcessMessages)
{
	swapBuffers();
	if (doProcessMessages)
		processMessages();
}

void create()
{
	if (initialized)
		return;
	
	// Initialize SIFRPC
	SifInitRpc(0);
	
	// Load modules (would need ROM path or memory card)
	// SifLoadModule("rom0:SIO2MAN", 0, NULL);
	// SifLoadModule("rom0:PADMAN", 0, NULL);
	
	// Setup GS
	gs_disp_mode.mode = GS_MODE_NTSC;
	gs_disp_mode.interlace = GS_INTERLACE_FRAME;
	gs_disp_mode.field = GS_FIELD_FRAME;
	gs_disp_mode.width = 640;
	gs_disp_mode.height = 448;
	
	GsInit(&gs_disp_mode, &gs_env);
	GsSetEnv(&gs_env);
	
	// Allocate packet buffer
	packet_buffer = (qword_t*)align(16, 1024 * 256);
	packet_index = 0;
	
	// Set background color
	GsClearDisp(0, 0, 0, 0);
	
	initialized = true;
}

int_t getX()
{
	return 0;
}

int_t getY()
{
	return 0;
}

int_t getWidth()
{
	return current_display_mode.getWidth();
}

int_t getHeight()
{
	return current_display_mode.getHeight();
}

}
}
