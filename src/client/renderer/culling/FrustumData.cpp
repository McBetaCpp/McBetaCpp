#include "client/renderer/culling/FrustumData.h"

bool FrustumData::pointInFrustum(float x, float y, float z)
{
	for (int_t i = 0; i < 6; i++)
	{
		if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0.0f)
			return false;
	}
	return true;
}

bool FrustumData::sphereInFrustum(float x, float y, float z, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -radius)
			return false;
	}

	return true;
}

bool FrustumData::cubeFullyInFrustum(double x1, double y1, double z1, double x2, double y2, double z2)
{
	for (int i = 0; i < 6; i++)
	{
		if (!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0)) return false;
		if (!(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0)) return false;
		if (!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0)) return false;
		if (!(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0)) return false;
		if (!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0)) return false;
		if (!(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0)) return false;
		if (!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0)) return false;
		if (!(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0)) return false;
	}

	return true;
}

bool FrustumData::cubeInFrustum(double x1, double y1, double z1, double x2, double y2, double z2)
{
	for (int i = 0; i < 6; i++)
	{
		if (!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0) &&
			!(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0) &&
			!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0) &&
			!(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0) &&
			!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0) &&
			!(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0) &&
			!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0) &&
			!(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0))
			return false;
	}

	return true;
}

bool FrustumData::isVisible(AABB &aabb)
{
	return cubeInFrustum(aabb.x0, aabb.y0, aabb.z0, aabb.x1, aabb.y1, aabb.z1);
}
