#pragma once

#include <iostream>

#include "nbt/CompoundTag.h"

namespace NbtIo
{

CompoundTag *readCompressed(std::istream &is);
CompoundTag *read(std::istream &is);

void writeCompressed(CompoundTag &tag, std::ostream &os);
void write(CompoundTag &tag, std::ostream &os);

}
