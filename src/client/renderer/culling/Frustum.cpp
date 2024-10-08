#include "client/renderer/culling/Frustum.h"

#include "util/Mth.h"

#include "OpenGL.h"

Frustum Frustum::frustum;

FrustumData &Frustum::getFrustum()
{
	frustum.calculateFrustum();
	return frustum;
}

void Frustum::normalizePlane(std::array<std::array<float, 16>, 16> &frustum, int_t side)
{
	// Here we calculate the magnitude of the normal to the plane (point A B C)
		// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
		// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
	float magnitude = Mth::sqrt(
		frustum[side][A] * frustum[side][A] +
		frustum[side][B] * frustum[side][B] +
		frustum[side][C] * frustum[side][C]
	);
	// Then we divide the plane's values by it's magnitude.
	// This makes it easier to work with.
	frustum[side][A] /= magnitude;
	frustum[side][B] /= magnitude;
	frustum[side][C] /= magnitude;
	frustum[side][D] /= magnitude;
}

void Frustum::calculateFrustum()
{
	// glGetFloatv() is used to extract information about our OpenGL world.
	// Below, we pass in GL_PROJECTION_MATRIX to abstract our projection matrix.
	// It then stores the matrix into an array of [16].
	glGetFloatv(GL_PROJECTION_MATRIX, proj.data());

	// By passing in GL_MODELVIEW_MATRIX, we can abstract our model view matrix.
	// This also stores it in an array of [16].
	glGetFloatv(GL_MODELVIEW_MATRIX, modl.data());

	// Now that we have our modelview and projection matrix, if we combine these 2 matrices,
	// it will give us our clipping planes.  To combine 2 matrices, we multiply them.

	clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
	clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
	clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
	clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

	clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
	clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
	clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
	clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

	clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
	clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
	clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

	// Now we actually want to get the sides of the frustum.  To do this we take
	// the clipping planes we received above and extract the sides from them.

	// This will extract the RIGHT side of the frustum
	m_Frustum[RIGHT][A] = clip[3] - clip[0];
	m_Frustum[RIGHT][B] = clip[7] - clip[4];
	m_Frustum[RIGHT][C] = clip[11] - clip[8];
	m_Frustum[RIGHT][D] = clip[15] - clip[12];

	// Now that we have a normal (A,B,C) and a distance (D) to the plane,
	// we want to normalize that normal and distance.

	// Normalize the RIGHT side
	normalizePlane(m_Frustum, RIGHT);

	// This will extract the LEFT side of the frustum
	m_Frustum[LEFT][A] = clip[3] + clip[0];
	m_Frustum[LEFT][B] = clip[7] + clip[4];
	m_Frustum[LEFT][C] = clip[11] + clip[8];
	m_Frustum[LEFT][D] = clip[15] + clip[12];

	// Normalize the LEFT side
	normalizePlane(m_Frustum, LEFT);

	// This will extract the BOTTOM side of the frustum
	m_Frustum[BOTTOM][A] = clip[3] + clip[1];
	m_Frustum[BOTTOM][B] = clip[7] + clip[5];
	m_Frustum[BOTTOM][C] = clip[11] + clip[9];
	m_Frustum[BOTTOM][D] = clip[15] + clip[13];

	// Normalize the BOTTOM side
	normalizePlane(m_Frustum, BOTTOM);

	// This will extract the TOP side of the frustum
	m_Frustum[TOP][A] = clip[3] - clip[1];
	m_Frustum[TOP][B] = clip[7] - clip[5];
	m_Frustum[TOP][C] = clip[11] - clip[9];
	m_Frustum[TOP][D] = clip[15] - clip[13];

	// Normalize the TOP side
	normalizePlane(m_Frustum, TOP);

	// This will extract the BACK side of the frustum
	m_Frustum[BACK][A] = clip[3] - clip[2];
	m_Frustum[BACK][B] = clip[7] - clip[6];
	m_Frustum[BACK][C] = clip[11] - clip[10];
	m_Frustum[BACK][D] = clip[15] - clip[14];

	// Normalize the BACK side
	normalizePlane(m_Frustum, BACK);

	// This will extract the FRONT side of the frustum
	m_Frustum[FRONT][A] = clip[3] + clip[2];
	m_Frustum[FRONT][B] = clip[7] + clip[6];
	m_Frustum[FRONT][C] = clip[11] + clip[10];
	m_Frustum[FRONT][D] = clip[15] + clip[14];

	// Normalize the FRONT side
	normalizePlane(m_Frustum, FRONT);
}
