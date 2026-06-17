#pragma once

#include "java/Type.h"

namespace lwjgl
{
namespace Mouse
{

bool isButtonDown(int button);
int getDX();
int getDY();
int getX();
int getY();
int getDWheel();
bool isCreated();
void create();
void grab();
void release();
void poll();

}
}
