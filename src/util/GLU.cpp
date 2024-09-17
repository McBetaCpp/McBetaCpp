#include "util/GLU.h"

#define _USE_MATH_DEFINES
#include <cmath>

void gluPerspective(float fovy, float aspect, float zNear, float zFar)
{
    double const height = zNear * tanf(fovy * M_PI / 360.0);
    double const width = height * aspect;
    glFrustum(-width, width, -height, height, zNear, zFar);
}
