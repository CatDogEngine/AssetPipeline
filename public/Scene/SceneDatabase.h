#pragma once

#include "Math/AABB.hpp"
#include "Material.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Texture.h"

#include <optional>
#include <unordered_map>

namespace cdtools
{

class SceneDatabase : public ISerializable
{
public:
	using TextureMap = std::unordered_map<std::string, TextureID>;

public:
	explicit SceneDatabase() = default;
	SceneDatabase(const SceneDatabase&) = default;
	SceneDatabase& operator=(const SceneDatabase&) = default;
	SceneDatabase(SceneDatabase&&) = default;
	SceneDatabase& operator=(SceneDatabase&&) = default;
	~SceneDatabase() = default;

	void SetName(std::string sceneName) { m_name = std::move(sceneName); }
	const std::string& GetName() const { return m_name; }
	
	void SetAABB(AABB aabb) { m_aabb = std::move(aabb); }
	const AABB& GetAABB() const { return m_aabb; }

	// mesh
	void AddMesh(Mesh mesh) { m_meshes.emplace_back(std::move(mesh)); }
	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }
	void SetMeshCount(uint32_t meshCount) { m_meshes.reserve(meshCount); }
	const Mesh& GetMesh(uint32_t index) const { return m_meshes[index];  }
	uint32_t GetMeshCount() const { return static_cast<uint32_t>(m_meshes.size()); }

	// skeleton
	void AddSkeleton(Skeleton skeleton) { m_skeletons.emplace_back(std::move(skeleton)); }
	const std::vector<Skeleton>& GetSkeletons() const { return m_skeletons; }
	void SetSkeletonCount(uint32_t skeletonCount) { m_skeletons.reserve(skeletonCount); }
	const Skeleton& GetSkeleton(uint32_t index) const { return m_skeletons[index]; }
	uint32_t GetSkeletonCount() const { return static_cast<uint32_t>(m_skeletons.size()); }

	// material
	void AddMaterial(Material material) { m_materials.emplace_back(std::move(material)); }
	const std::vector<Material>& GetMaterials() const { return m_materials; }
	void SetMaterialCount(uint32_t materialCount) { m_materials.reserve(materialCount); }
	const Material& GetMaterial(uint32_t index) const { return m_materials[index]; }
	uint32_t GetMaterialCount() const { return static_cast<uint32_t>(m_materials.size()); }

	// texture
	void AddTexture(Texture texture) { m_textures.emplace_back(std::move(texture)); }
	const std::vector<Texture>& GetTextures() const { return m_textures; }
	void SetTextureCount(uint32_t textureCount) { m_textures.reserve(textureCount); }
	const Texture& GetTexture(uint32_t index) const { return m_textures[index]; }
	uint32_t GetTextureCount() const { return static_cast<uint32_t>(m_textures.size()); }

	// ISerializable
	virtual void ImportBinary(std::ifstream& fin) override;
	virtual void ExportBinary(std::ofstream& fout) const override;

private:
	std::string m_name;
	AABB m_aabb;

	// mesh data
	std::vector<Mesh> m_meshes;

	// animation data
	std::vector<Skeleton> m_skeletons;

	// material data
	std::vector<Material> m_materials;

	// texture data
	std::vector<Texture> m_textures;
};

}