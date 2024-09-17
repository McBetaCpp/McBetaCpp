#include "world/entity/EntityIO.h"

#include <unordered_map>

#include "world/entity/Entity.h"
#include "world/entity/Mob.h"

namespace EntityIO
{

#define ENTITYIO_ID(type, name, id) { name, [](Level &level) -> std::shared_ptr<Entity> { return Util::make_shared<type>(level); } },
static std::unordered_map<jstring, std::shared_ptr<Entity>(*)(Level &level)> idClassMap = {
#include "world/entity/EntityIDs.h"
};
#undef ENTITYIO_ID

#define ENTITYIO_ID(type, name, id) { id, [](Level &level) -> std::shared_ptr<Entity> { return Util::make_shared<type>(level); } },
static std::unordered_map<int_t, std::shared_ptr<Entity>(*)(Level &level)> numClassMap = {
#include "world/entity/EntityIDs.h"
};
#undef ENTITYIO_ID

std::shared_ptr<Entity> loadStatic(CompoundTag &tag, Level &level)
{
	try
	{
		std::shared_ptr<Entity> entity = idClassMap.at(tag.getString(u"id"))(level);
		entity->load(tag);
		return entity;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	return nullptr;
}

}
