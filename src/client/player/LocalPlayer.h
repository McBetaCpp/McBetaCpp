#pragma once

#include "client/player/Input.h"

#include "world/entity/player/Player.h"

#include "nbt/CompoundTag.h"

#include "java/Type.h"

class Minecraft;
class User;

class LocalPlayer : public Player
{
public:
	std::unique_ptr<Input> input;

protected:
	Minecraft &minecraft;

public:
	int_t changingDimensionDelay = 20;

	float portalTime = 0.0f;
	float oPortalTime = 0.0f;

private:
	bool isInsidePortal = false;

public:
	LocalPlayer(Minecraft &minecraft, Level &level, User *user, int_t dimension);

	void updateAi() override;
	void aiStep() override;

	void releaseAllKeys();
	void setKey(int_t eventKey, bool eventKeyState);

	void addAdditionalSaveData(CompoundTag &tag) override;
	void readAdditionalSaveData(CompoundTag &tag) override;

	void closeContainer() override;

	// TODO
	// openTextEdit
	// openContainer
	// startCrafing
	// openFurnace
	// openTrap
	
	void prepareForTick();

	bool isSneaking() override;
};
