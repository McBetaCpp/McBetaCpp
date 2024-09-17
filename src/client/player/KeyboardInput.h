#pragma once

#include <array>

#include "client/player/Input.h"

class Options;

class KeyboardInput : public Input
{
public:
	static constexpr int_t KEY_UP = 0;
	static constexpr int_t KEY_DOWN = 1;
	static constexpr int_t KEY_LEFT = 2;
	static constexpr int_t KEY_RIGHT = 3;
	static constexpr int_t KEY_JUMP = 4;
	static constexpr int_t KEY_SNEAK = 5;

private:
	std::array<bool, 10> keys = {};
	Options &options;

public:
	KeyboardInput(Options &options);

	void setKey(int_t key, bool state) override;
	void releaseAllKeys() override;
	void tick(Player &player) override;
};
