#include "MemoryTracker.h"

#include "OpenGL.h"

std::vector<int_t> MemoryTracker::lists;
std::vector<int_t> MemoryTracker::textures;

int_t MemoryTracker::genLists(int_t count)
{
	GLint id = glGenLists(count);
	lists.push_back(id);
	lists.push_back(count);
	return id;
}

void MemoryTracker::genTextures(std::vector<int_t> &ib)
{
	static_assert(sizeof(GLuint) == sizeof(int_t), "sizeof(GLuint) != sizeof(int_t)");
	glGenTextures(ib.size(), reinterpret_cast<GLuint*>(ib.data()));
}

void MemoryTracker::release()
{
	for (int_t i = 0; i < lists.size(); i += 2)
		glDeleteLists(lists[i], lists[i + 1]);

	auto ib = createIntBuffer(textures.size());
	// This seems to be a bug in the original code
	// glDeleteTextures(ib.size(), reinterpret_cast<GLuint *>(ib.data()));

	for (int_t i = 0; i < textures.size(); i++)
		ib[i] = textures[i];

	glDeleteTextures(ib.size(), reinterpret_cast<GLuint*>(ib.data()));

	lists.clear();
	textures.clear();
}

std::vector<byte_t> MemoryTracker::createByteBuffer(int_t size)
{
	std::vector<byte_t> buffer(size);
	return buffer;
}
std::vector<short_t> MemoryTracker::createShortBuffer(int_t size)
{
	std::vector<short_t> buffer(size);
	return buffer;
}
std::vector<char_t> MemoryTracker::createCharBuffer(int_t size)
{
	std::vector<char_t> buffer(size);
	return buffer;
}
std::vector<int_t> MemoryTracker::createIntBuffer(int_t size)
{
	std::vector<int_t> buffer(size);
	return buffer;
}
std::vector<long_t> MemoryTracker::createLongBuffer(int_t size)
{
	std::vector<long_t> buffer(size);
	return buffer;
}
std::vector<float> MemoryTracker::createFloatBuffer(int_t size)
{
	std::vector<float> buffer(size);
	return buffer;
}
std::vector<double> MemoryTracker::createDoubleBuffer(int_t size)
{
	std::vector<double> buffer(size);
	return buffer;
}
