#pragma once

#include <memory>

#include "nbt/CompoundTag.h"

#include "java/Type.h"
#include "java/String.h"

class Entity;
class Level;

class EntityIO_detail;

namespace EntityIO
{

std::shared_ptr<Entity> loadStatic(CompoundTag &tag, Level &level);

}
