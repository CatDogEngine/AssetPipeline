#include "SceneDatabaseImpl.h"

#include "Base/Template.h"

#include <cassert>

namespace cd
{

///////////////////////////////////////////////////////////////////
// Transform
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetTransformCount(uint32_t transformCount)
{
	m_transforms.reserve(transformCount);
}

void SceneDatabaseImpl::AddTransform(Transform transform)
{
	m_transforms.emplace_back(MoveTemp(transform));
}

///////////////////////////////////////////////////////////////////
// Mesh
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetMeshCount(uint32_t meshCount)
{
	m_meshes.reserve(meshCount);
}

void SceneDatabaseImpl::AddMesh(Mesh mesh)
{
	m_meshes.emplace_back(MoveTemp(mesh));
}

///////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetMaterialCount(uint32_t materialCount)
{
	m_materials.reserve(materialCount);
}

void SceneDatabaseImpl::AddMaterial(Material material)
{
	m_materials.emplace_back(MoveTemp(material));
}

///////////////////////////////////////////////////////////////////
// Texture
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetTextureCount(uint32_t textureCount)
{
	m_textures.reserve(textureCount);
}

void SceneDatabaseImpl::AddTexture(Texture texture)
{
	m_textures.emplace_back(MoveTemp(texture));
}

///////////////////////////////////////////////////////////////////
// Light
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetLightCount(uint32_t lightCount)
{
	m_lights.reserve(lightCount);
}

void SceneDatabaseImpl::AddLight(Light light)
{
	m_lights.emplace_back(MoveTemp(light));
}

}