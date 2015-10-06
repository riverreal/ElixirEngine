#pragma once

#include "../Helper/TypeHelper.h"

class BasicShapes
{
public:
	void CreateCube(float width, float height, float depth, MeshData& meshData);
	void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);
	void CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData);
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
	void CreateScreenLayer(MeshData& meshData);
private:
	void Subdivide(MeshData& meshData);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
};