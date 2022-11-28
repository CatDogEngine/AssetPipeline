#include "SceneDatabase.h"

namespace cdtools
{

///////////////////////////////////////////////////////////////////
// Import/Export
///////////////////////////////////////////////////////////////////
void SceneDatabase::ImportBinary(std::ifstream& fin)
{
	std::string sceneName;
	ImportData(fin, sceneName);
	SetName(std::move(sceneName));

	AABB sceneAABB;
	ImportDataBuffer(fin, sceneAABB.Min().begin());
	ImportDataBuffer(fin, sceneAABB.Max().begin());
	SetAABB(std::move(sceneAABB));

	uint32_t meshCount = 0;
	uint32_t materialCount = 0;
	uint32_t textureCount = 0;
	ImportData(fin, meshCount);
	ImportData(fin, materialCount);
	ImportData(fin, textureCount);
	SetMeshCount(meshCount);
	SetMaterialCount(materialCount);
	SetTextureCount(textureCount);

	for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		AddMesh(Mesh(fin));
	}

	for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
	{
		AddTexture(Texture(fin));
	}

	for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		AddMaterial(Material(fin));
	}
}

void SceneDatabase::ExportBinary(std::ofstream& fout) const
{
	ExportData<std::string>(fout, GetName());

	ExportDataBuffer(fout, GetAABB().Min().begin(), GetAABB().Min().size());
	ExportDataBuffer(fout, GetAABB().Max().begin(), GetAABB().Max().size());

	ExportData<uint32_t>(fout, GetMeshCount());
	ExportData<uint32_t>(fout, GetMaterialCount());
	ExportData<uint32_t>(fout, GetTextureCount());

	for (const Mesh& mesh : GetMeshes())
	{
		mesh.ExportBinary(fout);
	}

	for (const Texture& texture : GetTextures())
	{
		texture.ExportBinary(fout);
	}

	for (const Material& material : GetMaterials())
	{
		material.ExportBinary(fout);
	}
}

}