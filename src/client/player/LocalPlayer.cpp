#include "client/player/LocalPlayer.h"

#include "client/Minecraft.h"

LocalPlayer::LocalPlayer(Minecraft &minecraft, Level &level, User *user, int_t dimension) : Player(level), minecraft(minecraft)
{
	this->dimension = dimension;
}


void LocalPlayer::updateAi()
{
	Player::updateAi();
	xxa = input->xa;
	yya = input->ya;
	jumping = input->jumping;
}

void LocalPlayer::aiStep()
{
	oPortalTime = portalTime;

	if (isInsidePortal)
	{
		// TODO
	}

	input->tick(*this);

	if (input->sneaking && ySlideOffset < 0.2f)
		ySlideOffset = 0.2f;

	Player::aiStep();
}

void LocalPlayer::releaseAllKeys()
{
	input->releaseAllKeys();
}

void LocalPlayer::setKey(int_t eventKey, bool eventKeyState)
{
	input->setKey(eventKey, eventKeyState);
}

void LocalPlayer::addAdditionalSaveData(CompoundTag &tag)
{
	Player::addAdditionalSaveData(tag);
}

void LocalPlayer::readAdditionalSaveData(CompoundTag &tag)
{
	Player::readAdditionalSaveData(tag);
}

void LocalPlayer::closeContainer()
{
	Player::closeContainer();
	minecraft.setScreen(nullptr);
}

void LocalPlayer::prepareForTick()
{

}

bool LocalPlayer::isSneaking()
{
	return input->sneaking;
}
