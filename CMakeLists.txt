cmake_minimum_required(VERSION 3.26)

project(McBetaCpp
	VERSION 1.2.2.0
	DESCRIPTION "A game about placing blocks to build anything you can imagine."
	LANGUAGES C CXX
)

set(McBetaCpp_SOURCES
	"src/client/Minecraft.cpp"
	"src/client/Minecraft.h"
	
	"src/Facing.h"
	"src/SharedConstants.cpp"
	"src/SharedConstants.h"
	
	"src/client/Lighting.cpp"
	"src/client/Lighting.h"
	"src/client/MemoryTracker.cpp"
	"src/client/MemoryTracker.h"
	"src/client/MouseHandler.cpp"
	"src/client/MouseHandler.h"
	"src/client/OpenGLCapabilities.cpp"
	"src/client/OpenGLCapabilities.h"
	"src/client/Options.cpp"
	"src/client/Options.h"
	"src/client/ProgressRenderer.cpp"
	"src/client/ProgressRenderer.h"
	"src/client/Timer.cpp"
	"src/client/Timer.h"
	"src/client/User.cpp"
	"src/client/User.h"

	"src/client/gamemode/GameMode.cpp"
	"src/client/gamemode/GameMode.h"
	"src/client/gamemode/SurvivalMode.cpp"
	"src/client/gamemode/SurvivalMode.h"
	
	"src/client/gui/Button.cpp"
	"src/client/gui/Button.h"
	"src/client/gui/ConfirmScreen.cpp"
	"src/client/gui/ConfirmScreen.h"
	"src/client/gui/ControlsScreen.cpp"
	"src/client/gui/ControlsScreen.h"
	"src/client/gui/DeleteWorldScreen.cpp"
	"src/client/gui/DeleteWorldScreen.h"
	"src/client/gui/Font.cpp"
	"src/client/gui/Font.h"
	"src/client/gui/Gui.cpp"
	"src/client/gui/Gui.h"
	"src/client/gui/GuiComponent.cpp"
	"src/client/gui/GuiComponent.h"
	"src/client/gui/OptionsScreen.cpp"
	"src/client/gui/OptionsScreen.h"
	"src/client/gui/PauseScreen.cpp"
	"src/client/gui/PauseScreen.h"
	"src/client/gui/Screen.cpp"
	"src/client/gui/Screen.h"
	"src/client/gui/ScreenSizeCalculator.cpp"
	"src/client/gui/ScreenSizeCalculator.h"
	"src/client/gui/SelectWorldScreen.cpp"
	"src/client/gui/SelectWorldScreen.h"
	"src/client/gui/SlideButton.cpp"
	"src/client/gui/SlideButton.h"
	"src/client/gui/SmallButton.cpp"
	"src/client/gui/SmallButton.h"

	"src/client/locale/Language.cpp"
	"src/client/locale/Language.h"
	
	"src/client/model/Cube.cpp"
	"src/client/model/Cube.h"
	"src/client/model/HumanoidModel.cpp"
	"src/client/model/HumanoidModel.h"
	"src/client/model/Model.cpp"
	"src/client/model/Model.h"
	"src/client/model/Polygon.cpp"
	"src/client/model/Polygon.h"
	"src/client/model/Vertex.cpp"
	"src/client/model/Vertex.h"
	
	"src/client/player/Input.cpp"
	"src/client/player/Input.h"
	"src/client/player/KeyboardInput.cpp"
	"src/client/player/KeyboardInput.h"
	"src/client/player/LocalPlayer.cpp"
	"src/client/player/LocalPlayer.h"
	
	"src/client/renderer/Chunk.cpp"
	"src/client/renderer/Chunk.h"
	"src/client/renderer/DirtyChunkSorter.h"
	"src/client/renderer/DistanceChunkSorter.h"
	"src/client/renderer/GameRenderer.cpp"
	"src/client/renderer/GameRenderer.h"
	"src/client/renderer/ItemInHandRenderer.cpp"
	"src/client/renderer/ItemInHandRenderer.h"
	"src/client/renderer/LevelRenderer.cpp"
	"src/client/renderer/LevelRenderer.h"
	"src/client/renderer/OffsettedRenderList.cpp"
	"src/client/renderer/OffsettedRenderList.h"
	"src/client/renderer/Tesselator.cpp"
	"src/client/renderer/Tesselator.h"
	"src/client/renderer/Textures.cpp"
	"src/client/renderer/Textures.h"
	"src/client/renderer/TileRenderer.cpp"
	"src/client/renderer/TileRenderer.h"

	"src/client/renderer/culling/Culler.h"
	"src/client/renderer/culling/Frustum.cpp"
	"src/client/renderer/culling/Frustum.h"
	"src/client/renderer/culling/FrustumCuller.cpp"
	"src/client/renderer/culling/FrustumCuller.h"
	"src/client/renderer/culling/FrustumData.cpp"
	"src/client/renderer/culling/FrustumData.h"
	
	"src/client/renderer/entity/EntityRenderDispatcher.cpp"
	"src/client/renderer/entity/EntityRenderDispatcher.h"
	"src/client/renderer/entity/EntityRenderer.cpp"
	"src/client/renderer/entity/EntityRenderer.h"
	"src/client/renderer/entity/MobRenderer.cpp"
	"src/client/renderer/entity/MobRenderer.h"
	"src/client/renderer/entity/PlayerRenderer.cpp"
	"src/client/renderer/entity/PlayerRenderer.h"

	"src/client/skins/DefaultTexturePack.cpp"
	"src/client/skins/DefaultTexturePack.h"
	"src/client/skins/FileTexturePack.cpp"
	"src/client/skins/FileTexturePack.h"
	"src/client/skins/TexturePack.cpp"
	"src/client/skins/TexturePack.h"
	"src/client/skins/TexturePackRepository.cpp"
	"src/client/skins/TexturePackRepository.h"
	"src/client/skins/TexturePackSelectScreen.cpp"
	"src/client/skins/TexturePackSelectScreen.h"

	"src/client/title/TitleScreen.cpp"
	"src/client/title/TitleScreen.h"

	"src/locale/Descriptive.h"
	
	"src/nbt/ByteArrayTag.cpp"
	"src/nbt/ByteArrayTag.h"
	"src/nbt/ByteTag.cpp"
	"src/nbt/ByteTag.h"
	"src/nbt/CompoundTag.cpp"
	"src/nbt/CompoundTag.h"
	"src/nbt/DoubleTag.cpp"
	"src/nbt/DoubleTag.h"
	"src/nbt/EndTag.cpp"
	"src/nbt/EndTag.h"
	"src/nbt/FloatTag.cpp"
	"src/nbt/FloatTag.h"
	"src/nbt/IntTag.cpp"
	"src/nbt/IntTag.h"
	"src/nbt/ListTag.cpp"
	"src/nbt/ListTag.h"
	"src/nbt/LongTag.cpp"
	"src/nbt/LongTag.h"
	"src/nbt/NbtIo.cpp"
	"src/nbt/NbtIo.h"
	"src/nbt/ShortTag.cpp"
	"src/nbt/ShortTag.h"
	"src/nbt/StringTag.cpp"
	"src/nbt/StringTag.h"
	"src/nbt/Tag.cpp"
	"src/nbt/Tag.h"

	"src/util/GLU.cpp"
	"src/util/GLU.h"
	"src/util/Memory.h"
	"src/util/Mth.cpp"
	"src/util/Mth.h"
	"src/util/ProgressListener.h"

	"src/world/entity/Entity.cpp"
	"src/world/entity/Entity.h"
	"src/world/entity/EntityEvent.h"
	"src/world/entity/EntityIO.cpp"
	"src/world/entity/EntityIO.h"
	"src/world/entity/EntityPos.cpp"
	"src/world/entity/EntityPos.h"
	"src/world/entity/Mob.cpp"
	"src/world/entity/Mob.h"

	"src/world/entity/player/Player.cpp"
	"src/world/entity/player/Player.h"

	"src/world/item/ItemInstance.cpp"
	"src/world/item/ItemInstance.h"
	
	"src/world/level/ChunkPos.h"
	"src/world/level/Coord.h"
	"src/world/level/FoliageColor.cpp"
	"src/world/level/FoliageColor.h"
	"src/world/level/GrassColor.cpp"
	"src/world/level/GrassColor.h"
	"src/world/level/Level.cpp"
	"src/world/level/Level.h"
	"src/world/level/LevelListener.h"
	"src/world/level/LevelSource.h"
	"src/world/level/LightLayer.h"
	"src/world/level/LightUpdate.cpp"
	"src/world/level/LightUpdate.h"
	"src/world/level/Region.cpp"
	"src/world/level/Region.h"
	"src/world/level/TilePos.h"

	"src/world/level/biome/BiomeSource.cpp"
	"src/world/level/biome/BiomeSource.h"

	"src/world/level/chunk/storage/OldChunkStorage.cpp"
	"src/world/level/chunk/storage/OldChunkStorage.h"
	"src/world/level/chunk/storage/ChunkStorage.h"
	
	"src/world/level/chunk/ChunkCache.cpp"
	"src/world/level/chunk/ChunkCache.h"
	"src/world/level/chunk/ChunkSource.h"
	"src/world/level/chunk/DataLayer.cpp"
	"src/world/level/chunk/DataLayer.h"
	"src/world/level/chunk/EmptyLevelChunk.cpp"
	"src/world/level/chunk/EmptyLevelChunk.h"
	"src/world/level/chunk/LevelChunk.cpp"
	"src/world/level/chunk/LevelChunk.h"

	"src/world/level/dimension/Dimension.cpp"
	"src/world/level/dimension/Dimension.h"
	
	"src/world/level/levelgen/feature/CaveFeature.cpp"
	"src/world/level/levelgen/feature/CaveFeature.h"
	"src/world/level/levelgen/feature/Feature.h"
	"src/world/level/levelgen/feature/TreeFeature.cpp"
	"src/world/level/levelgen/feature/TreeFeature.h"
	
	"src/world/level/levelgen/synth/ImprovedNoise.cpp"
	"src/world/level/levelgen/synth/ImprovedNoise.h"
	"src/world/level/levelgen/synth/PerlinNoise.cpp"
	"src/world/level/levelgen/synth/PerlinNoise.h"
	"src/world/level/levelgen/synth/PerlinSimplexNoise.cpp"
	"src/world/level/levelgen/synth/PerlinSimplexNoise.h"
	"src/world/level/levelgen/synth/SimplexNoise.cpp"
	"src/world/level/levelgen/synth/SimplexNoise.h"
	"src/world/level/levelgen/synth/Synth.cpp"
	"src/world/level/levelgen/synth/Synth.h"
	
	"src/world/level/levelgen/LargeCaveFeature.cpp"
	"src/world/level/levelgen/LargeCaveFeature.h"
	"src/world/level/levelgen/LargeFeature.cpp"
	"src/world/level/levelgen/LargeFeature.h"
	"src/world/level/levelgen/RandomLevelSource.cpp"
	"src/world/level/levelgen/RandomLevelSource.h"
	
	"src/world/level/material/GasMaterial.cpp"
	"src/world/level/material/GasMaterial.h"
	"src/world/level/material/Material.cpp"
	"src/world/level/material/Material.h"

	"src/world/level/tile/entity/TileEntity.cpp"
	"src/world/level/tile/entity/TileEntity.h"
	
	"src/world/level/tile/DirtTile.cpp"
	"src/world/level/tile/DirtTile.h"
	"src/world/level/tile/GrassTile.cpp"
	"src/world/level/tile/GrassTile.h"
	"src/world/level/tile/GravelTile.cpp"
	"src/world/level/tile/GravelTile.h"
	"src/world/level/tile/LeafTile.cpp"
	"src/world/level/tile/LeafTile.h"
	"src/world/level/tile/SandTile.cpp"
	"src/world/level/tile/SandTile.h"
	"src/world/level/tile/StoneTile.cpp"
	"src/world/level/tile/StoneTile.h"
	"src/world/level/tile/Tile.cpp"
	"src/world/level/tile/Tile.h"
	"src/world/level/tile/TransparentTile.cpp"
	"src/world/level/tile/TransparentTile.h"
	"src/world/level/tile/TreeTile.cpp"
	"src/world/level/tile/TreeTile.h"
	
	"src/world/phys/AABB.cpp"
	"src/world/phys/AABB.h"
	"src/world/phys/HitResult.cpp"
	"src/world/phys/HitResult.h"
	"src/world/phys/Vec3.cpp"
	"src/world/phys/Vec3.h"
	
	"src/java/BufferedImage.cpp"
	"src/java/BufferedImage.h"
	"src/java/File.cpp"
	"src/java/File.h"
	"src/java/IOUtil.cpp"
	"src/java/IOUtil.h"
	"src/java/Math.cpp"
	"src/java/Math.h"
	"src/java/Random.cpp"
	"src/java/Random.h"
	"src/java/Resource.h"
	"src/java/Runtime.h"
	"src/java/String.cpp"
	"src/java/String.h"
	"src/java/System.cpp"
	"src/java/System.h"
	"src/java/Type.h"
	
	"src/external/stb_image.cpp"
)

add_executable(McBetaCpp
	${McBetaCpp_SOURCES}
	
	"src/pc/Main.cpp"

	"src/pc/external/miniaudio.cpp"
	"src/pc/external/SDLException.h"
	"src/pc/external/stb_vorbis.cpp"
	"src/pc/external/stb_vorbis.h"

	"src/pc/lwjgl/Display.cpp"
	"src/pc/lwjgl/Display.h"
	"src/pc/lwjgl/GLContext.cpp"
	"src/pc/lwjgl/GLContext.h"
	"src/pc/lwjgl/Keyboard.cpp"
	"src/pc/lwjgl/Keyboard.h"
	"src/pc/lwjgl/Mouse.cpp"
	"src/pc/lwjgl/Mouse.h"
)
target_include_directories(McBetaCpp PRIVATE "src/pc")

target_include_directories(McBetaCpp PRIVATE "src")

# Compile platform specific code
if(WIN32)
	target_sources(McBetaCpp PRIVATE
		"src/pc/win32/java/File.cpp"
		"src/pc/win32/java/Resource.cpp"
		"src/pc/win32/java/Runtime.cpp"
	)

	target_compile_definitions(McBetaCpp PRIVATE MC_WIN32)
elseif(APPLE)
	message(FATAL_ERROR "MacOS is not supported yet.")
elseif(UNIX)
	target_sources(McBetaCpp PRIVATE
		"src/pc/linux/java/File.cpp"
		"src/pc/linux/java/Resource.cpp"
		"src/pc/linux/java/Runtime.cpp"
	)
	
	target_compile_definitions(McBetaCpp PRIVATE MC_LINUX)
endif()

# Use C++11
set_target_properties(McBetaCpp PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

# Third party
add_subdirectory("external/glad" EXCLUDE_FROM_ALL)
target_link_libraries(McBetaCpp PRIVATE glad)

add_subdirectory("external/SDL2" EXCLUDE_FROM_ALL)
target_link_libraries(McBetaCpp PRIVATE SDL2-static)

add_subdirectory("external/zlib" EXCLUDE_FROM_ALL)

target_link_libraries(McBetaCpp PRIVATE zlibstatic)
target_include_directories(McBetaCpp PRIVATE "external/zlib")
target_include_directories(McBetaCpp PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/external/zlib")

target_include_directories(McBetaCpp PRIVATE "external/stb")
target_include_directories(McBetaCpp PRIVATE "external/gzip-hpp/include")

target_include_directories(McBetaCpp PRIVATE "external/miniaudio")

# Output to bin/ directory
set_target_properties(McBetaCpp PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
