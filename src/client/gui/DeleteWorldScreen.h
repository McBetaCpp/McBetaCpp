#pragma once

#include "client/gui/Screen.h"
#include "client/gui/SelectWorldScreen.h"

class DeleteWorldScreen : public SelectWorldScreen
{
public:
	DeleteWorldScreen(Minecraft &minecraft, std::shared_ptr<Screen> lastScreen);

	void postInit() override;

	void worldSelected(int_t i) override;

	void confirmResult(bool result, int_t id) override;
};
