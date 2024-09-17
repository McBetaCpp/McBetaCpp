#include "java/Math.h"

#include "java/Random.h"

namespace Math
{

double random()
{
	static Random random = Random();
	return random.nextDouble();
}

}
