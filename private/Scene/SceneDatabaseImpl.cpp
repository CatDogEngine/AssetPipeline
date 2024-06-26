#include "SceneDatabaseImpl.h"

#include "Base/NameOf.h"

#include <cassert>
#include <cfloat>

namespace details
{

void Dump(const char* label, const cd::Quaternion& quaternion)
{
	printf("%s : (w = %f, x = %f, y = %f, z = %f)\n", label, quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z());
}

void Dump(const char* label, const cd::Vec2f& vector)
{
	printf("%s : (x = %f, y = %f)\n", label, vector.x(), vector.y());
}

void Dump(const char* label, const cd::Vec3f& vector)
{
	printf("%s : (x = %f, y = %f, z = %f)\n", label, vector.x(), vector.y(), vector.z());
}

void Dump(const char* label, const cd::Vec4f& vector)
{
	printf("%s : (x = %f, y = %f, z = %f, w = %f)\n", label, vector.x(), vector.y(), vector.z(), vector.w());
}

void Dump(const cd::Transform& transform)
{
	details::Dump("\tTranslation", transform.GetTranslation());
	details::Dump("\tRotation", transform.GetRotation());
	details::Dump("\tScale", transform.GetScale());
}

void Dump(const char* label, const cd::Matrix4x4& matrix)
{
	details::Dump (label, matrix.GetTranslation());
	details::Dump(label, cd::Quaternion::FromMatrix(matrix.GetRotation()));
	details::Dump(label, matrix.GetScale());
}

}

namespace cd
{

SceneDatabaseImpl::SceneDatabaseImpl()
{
	GetAABB().Clear();

	// By default, it will be CDEngine for conveniences.
	// If you are outside user in other platforms, you can set it by yourself.
	// TODO : add a update method to change axis system for SceneDatabase dynamicly.
	SetAxisSystem(AxisSystem::CDEngine());
	SetUnit(Unit::CenterMeter);
}

///////////////////////////////////////////////////////////////////
// Node
///////////////////////////////////////////////////////////////////
Node* SceneDatabaseImpl::GetNodeByName(const char* pName)
{
	for (auto& node : GetNodes())
	{
		if (0 == strcmp(pName, node.GetName()))
		{
			return &node;
		}
	}

	return nullptr;
}

const Node* SceneDatabaseImpl::GetNodeByName(const char* pName) const
{
	for (const auto& node : GetNodes())
	{
		if (0 == strcmp(pName, node.GetName()))
		{
			return &node;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////
// Bone
///////////////////////////////////////////////////////////////////
Bone* SceneDatabaseImpl::GetBoneByName(const char* pName)
{
	for (auto& bone : GetBones())
	{
		if (0 == strcmp(pName, bone.GetName()))
		{
			return &bone;
		}
	}

	return nullptr;
}

const Bone* SceneDatabaseImpl::GetBoneByName(const char* pName) const
{
	for (const auto& bone : GetBones())
	{
		if (0 == strcmp(pName, bone.GetName()))
		{
			return &bone;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////
// Track
///////////////////////////////////////////////////////////////////
Track* SceneDatabaseImpl::GetTrackByName(const char* pName)
{
	for (auto& track : GetTracks())
	{
		if (0 == strcmp(pName, track.GetName()))
		{
			return &track;
		}
	}

	return nullptr;
}

const Track* SceneDatabaseImpl::GetTrackByName(const char* pName) const
{
	for (const auto& track : GetTracks())
	{
		if (0 == strcmp(pName, track.GetName()))
		{
			return &track;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::Dump() const
{
	printf("\nSceneDatabase : \n");
	printf("\tName : %s\n", GetName().c_str());
	printf("\tAABB :\n");
	details::Dump("\t\tMin", GetAABB().Min());
	details::Dump("\t\tMax", GetAABB().Max());
	printf("\tAxisSystem : \n");
	printf("\t\tHandedness : %s\n", cd::Handedness::Left == GetAxisSystem().GetHandedness() ? "LeftHandSystem" : "RightHandSystem");
	details::Dump("\t\tUpAxis", cd::Vec3f::GetUpAxis(GetAxisSystem()));
	details::Dump("\t\tFrontAxis", cd::Vec3f::GetFrontAxis(GetAxisSystem()));
	printf("\tUnitSystem : \n");
	printf("\t\tUnit : %s\n", cd::Unit::CenterMeter == GetUnit() ? "CenterMeter" : "Meter");

	printf("\tRootNode count : %d\n", GetRootNodeIDCount());
	printf("\tNode count : %d\n", GetNodeCount());
	printf("\tMesh count : %d\n", GetMeshCount());
	printf("\tBlendShape count : %d\n", GetBlendShapeCount());
	printf("\tMorph count : %d\n", GetMorphCount());
	printf("\tMaterial count : %d\n", GetMaterialCount());
	printf("\tTexture count : %d\n", GetTextureCount());
	printf("\tCamera count : %d\n", GetCameraCount());
	printf("\tLight count : %d\n", GetLightCount());
	printf("\tSkin count : %d\n", GetSkinCount());
	printf("\tSkeleton count : %d\n", GetSkeletonCount());
	printf("\tBone count : %d\n", GetBoneCount());
	printf("\tAnimation count : %d\n", GetAnimationCount());
	printf("\tTrack count : %d\n", GetTrackCount());
	printf("\tParticleEmitter count : %d\n", GetParticleEmitterCount());
	if (GetNodeCount() > 0U)
	{
		printf("\n");

		printf("[RootNodeIDs] ");
		for (auto rootNodeID : GetRootNodeIDs())
		{
			printf("%u ", rootNodeID.Data());
		}
		printf("\n");

		for (const auto& node : GetNodes())
		{
			printf("[Node %u] ParentID = %u, Name = %s\n", node.GetID().Data(), node.GetParentID().Data(), node.GetName());
			details::Dump(node.GetTransform());

			for (cd::MeshID meshID : node.GetMeshIDs())
			{
				printf("\t[Associated Mesh %u] Name = %s\n", meshID.Data(), GetMesh(meshID.Data()).GetName());
			}
		}
	}

	std::map<MaterialID, std::map<MeshID, std::vector<uint32_t>>> materialDrawMeshPolygonGroupIDs;
	if (GetMeshCount() > 0U)
	{
		printf("\n");
		for (const auto& mesh : GetMeshes())
		{
			printf("[Mesh %u] Name = %s, VertexCount = %u\n", mesh.GetID().Data(), mesh.GetName(), mesh.GetVertexCount());
			printf("\tPolygonCount = %u, VertexInstanceCount = %u\n", mesh.GetPolygonCount(), mesh.GetVertexInstanceToIDCount());
			const auto& polygonGroups = mesh.GetPolygonGroups();
			for (uint32_t polygonGroupIndex = 0U; polygonGroupIndex < polygonGroups.size(); ++polygonGroupIndex)
			{
				auto& polygonGroup = polygonGroups[polygonGroupIndex];
				printf("\t[PolygonGroup %u] PolygonCount = %u\n", polygonGroupIndex, static_cast<uint32_t>(polygonGroup.size()));

				if (mesh.GetMaterialIDCount() > 0U)
				{
					auto materialID = mesh.GetMaterialID(polygonGroupIndex);
					printf("\t\t[Associated Material %u] Name = %s\n", materialID.Data(), materialID.IsValid() ? GetMaterial(materialID.Data()).GetName() : "");
					if (materialID.IsValid())
					{
						materialDrawMeshPolygonGroupIDs[materialID][mesh.GetID()].push_back(polygonGroupIndex);
					}
				}
			}

			for (auto blendShapeID : mesh.GetBlendShapeIDs())
			{
				printf("\t[Associated BlendShape %u] Name = %s\n", blendShapeID.Data(), GetBlendShape(blendShapeID.Data()).GetName());
			}

			for (auto skinID : mesh.GetSkinIDs())
			{
				printf("\t[Associated Skin %u] Name = %s\n", skinID.Data(), GetSkin(skinID.Data()).GetName());
			}
		}
	}

	if (GetBlendShapeCount() > 0U)
	{
		printf("\n");
		for (const auto& blendShape : GetBlendShapes())
		{
			printf("[BlendShape %u] Name = %s\n", blendShape.GetID().Data(), blendShape.GetName());

			for (auto morphID : blendShape.GetMorphIDs())
			{
				printf("\t[Associated Morph %u]\n", morphID.Data());
			}
		}
	}

	if (GetMorphCount() > 0U)
	{
		printf("\n");
		for (const auto& morph : GetMorphs())
		{
			printf("[Morph %u] Name = %s, VertexCount = %u\n", morph.GetID().Data(), morph.GetName(), morph.GetVertexCount());
			printf("\tWeight = %f\n", morph.GetWeight());
			if (morph.GetBlendShapeID().IsValid())
			{
				printf("\t[Associated BlendShape %u]\n", morph.GetBlendShapeID().Data());
			}
		}
	}

	if (GetMaterialCount() > 0U)
	{
		printf("\n");
		for (const auto& material : GetMaterials())
		{
			printf("[MaterialID %u] Name = %s\n", material.GetID().Data(), material.GetName());

			if (auto optMetallic = material.GetFloatProperty(cd::MaterialPropertyGroup::Metallic, cd::MaterialProperty::Factor); optMetallic.has_value())
			{
				printf("\tMetallic = %f\n", optMetallic.value());
			}

			if (auto optRoughness = material.GetFloatProperty(cd::MaterialPropertyGroup::Roughness, cd::MaterialProperty::Factor); optRoughness.has_value())
			{
				printf("\tRoughness = %f\n", optRoughness.value());
			}

			if (auto optTwoSided = material.GetBoolProperty(cd::MaterialPropertyGroup::General, cd::MaterialProperty::TwoSided); optTwoSided.has_value())
			{
				printf("\t%s = %d\n", nameof::nameof_enum(cd::MaterialProperty::TwoSided).data(), optTwoSided.value());
			}

			if (auto optBlendMode = material.GetI32Property(cd::MaterialPropertyGroup::General, cd::MaterialProperty::BlendMode); optBlendMode.has_value())
			{
				cd::BlendMode blendMode = static_cast<cd::BlendMode>(optBlendMode.value());
				printf("\tBlendMode = %s\n", nameof::nameof_enum(blendMode).data());

				if (cd::BlendMode::Mask == blendMode)
				{
					auto optAlphaTestValue = material.GetFloatProperty(cd::MaterialPropertyGroup::General, cd::MaterialProperty::OpacityMaskClipValue);
					assert(optAlphaTestValue.has_value());
					printf("\t\tAlphaCutOff = %f\n", optAlphaTestValue.value());
				}
			}

			for (int textureTypeValue = 0; textureTypeValue < nameof::enum_count<cd::MaterialTextureType>(); ++textureTypeValue)
			{
				cd::MaterialTextureType textureType = static_cast<cd::MaterialTextureType>(textureTypeValue);
				if (material.IsTextureSetup(textureType))
				{
					auto textureID = material.GetTextureID(textureType);
					if (textureID.IsValid())
					{
						const auto& texture = GetTexture(textureID.Data());
						printf("\t[Associated Texture %u] Type = %s\n", textureID.Data(), nameof::nameof_enum(textureType).data());
						printf("\t\tPath = %s\n", texture.GetPath());
					}
				}

				if (auto optUVScale = material.GetVec2fProperty(textureType, cd::MaterialProperty::UVScale); optUVScale.has_value())
				{
					details::Dump("\t\tUVScale", optUVScale.value());
				}
				if (auto optUVOffset = material.GetVec2fProperty(textureType, cd::MaterialProperty::UVOffset); optUVOffset.has_value())
				{
					details::Dump("\t\tUVOffset", optUVOffset.value());
				}
			}

			if (auto itDrawMeshPolygonGroups = materialDrawMeshPolygonGroupIDs.find(material.GetID().Data());
				itDrawMeshPolygonGroups != materialDrawMeshPolygonGroupIDs.end())
			{
				for (const auto& [drawMeshID, drawPolygonGroupIndexes] : itDrawMeshPolygonGroups->second)
				{
					const auto& mesh = GetMesh(drawMeshID.Data());
					printf("\t[Associated Mesh %u] Name = %s\n", drawMeshID.Data(), GetMesh(drawMeshID.Data()).GetName());
					for (uint32_t drawPolygonGroupIndex : drawPolygonGroupIndexes)
					{
						printf("\t\t[PolygonGroup %u]\n", drawPolygonGroupIndex);
					}
				}
			}
		}
	}

	if (GetTextureCount() > 0U)
	{
		printf("\n");
		for (const auto& texture : GetTextures())
		{
			printf("[Texture %u] Name = %s\n", texture.GetID().Data(), texture.GetName());
			printf("\tPath = %s\n", texture.GetPath());
			printf("\tUVMapMode = (%s, %s)\n", nameof::nameof_enum(texture.GetUMapMode()).data(), nameof::nameof_enum(texture.GetVMapMode()).data());
		}
	}

	if (GetCameraCount() > 0U)
	{
		printf("\n");
		for (const auto& camera : GetCameras())
		{
			printf("[Camera %u] Name = %s\n", camera.GetID().Data(), camera.GetName());
			details::Dump("\tEye", camera.GetEye());
			details::Dump("\tLookAt", camera.GetLookAt());
			details::Dump("\tUp", camera.GetUp());
			printf("\tNearPlane = %f, FarPlane = %f\n", camera.GetNearPlane(), camera.GetFarPlane());
			printf("\tAspect = %f, Fov = %f\n", camera.GetAspect(), camera.GetFov());
		}
	}

	if (GetLightCount() > 0U)
	{
		printf("\n");
		for (const auto& light : GetLights())
		{
			printf("[Light %u] Type = %s, Name = %s\n", light.GetID().Data(), nameof::nameof_enum(light.GetType()).data(), light.GetName());
			printf("\tIntensity = %f\n", light.GetIntensity());
			printf("\tRange = %f, Radius = %f\n", light.GetRange(), light.GetRadius());
			printf("\tWidth = %f, Height = %f\n", light.GetWidth(), light.GetHeight());
			printf("\tAngleScale = %f, AngleOffset = %f\n", light.GetAngleScale(), light.GetAngleOffset());
			details::Dump("\tColor", light.GetColor());
			details::Dump("\tPosition", light.GetPosition());
			details::Dump("\tDirection", light.GetDirection());
			details::Dump("\tUp", light.GetUp());
		}
	}

	if (GetSkinCount() > 0U)
	{
		printf("\n");
		for (const auto& skin : GetSkins())
		{
			printf("[Skin %u] Name : %s\n", skin.GetID().Data(), skin.GetName());
			printf("\tMaxVertexInfluenceCount = %u\n", skin.GetMaxVertexInfluenceCount());

			auto meshID = skin.GetMeshID();
			if (meshID.IsValid())
			{
				printf("\t[Associated Mesh %u] Name = %s\n", meshID.Data(), GetMesh(meshID.Data()).GetName());
			}
			
			auto skeletonID = skin.GetSkeletonID();
			if (skeletonID.IsValid())
			{
				printf("\t[Associated Skeleton %u] Name = %s\n", skeletonID.Data(), GetSkeleton(skeletonID.Data()).GetName());
			}
		}
	}

	if (GetSkeletonCount() > 0U)
	{
		printf("\n");
		for (const auto& skeleton : GetSkeletons())
		{
			printf("[Skeleton %u] Name : %s\n", skeleton.GetID().Data(), skeleton.GetName());

			printf("\t[Associated RootBone %u]\n", skeleton.GetRootBoneID().Data());

			for (auto boneID : skeleton.GetBoneIDs())
			{
				printf("\t[Associated Bone %u]\n", boneID.Data());
			}
		}
	}

	if (GetBoneCount() > 0U)
	{
		printf("\n");
		for (const auto& bone : GetBones())
		{
			printf("[Bone %u] Name : %s, ParentID : %u\n", bone.GetID().Data(), bone.GetName(), bone.GetParentID().Data());
			details::Dump("\tRestPost", bone.GetOffset().Inverse());

			printf("\t[Associated Skeleton %u]\n", bone.GetSkeletonID().Data());
			for (auto childID : bone.GetChildIDs())
			{
				printf("\t[ChildBone %u]\n", childID.Data());
			}
		}
	}

	if (GetAnimationCount() > 0U)
	{
		printf("\n");
		for (const auto& animation : GetAnimations())
		{
			printf("[Animation %u] Name : %s\n", animation.GetID().Data(), animation.GetName());
			printf("\tDuration : %f, TicksPerSecond : %f\n", animation.GetDuration(), animation.GetTicksPerSecond());
		}
	}

	if (GetTrackCount() > 0U)
	{
		printf("\n");
		for (const auto& track : GetTracks())
		{
			printf("[Track %u] Name : %s\n", track.GetID().Data(), track.GetName());
			printf("\tTranslationKeyCount : %u, RotationKeyCount : %u, ScaleKeyCount : %u\n",
				track.GetTranslationKeyCount(), track.GetRotationKeyCount(), track.GetScaleKeyCount());

			details::Dump("\tFirstTranslationKey", track.GetTranslationKeys()[0].GetValue());
			details::Dump("\tFirstRotationKey", track.GetRotationKeys()[0].GetValue());
			details::Dump("\tFirstScaleKey", track.GetScaleKeys()[0].GetValue());
		}
	}

	if (GetParticleEmitterCount() > 0U)
	{
		printf("\n");
		for (const auto& particle : GetParticleEmitters())
		{
			printf("[ParticleEmitter %u] Name : %s\n", particle.GetID().Data(), particle.GetName());
			printf("\tType : %s\n", nameof::nameof_enum(particle.GetType()).data());
			printf("\tMaxCount : %d\n", particle.GetMaxCount());
			details::Dump("\tPosition", particle.GetPosition());
			details::Dump("\tVelocity", particle.GetVelocity());
			details::Dump("\tAccelerate", particle.GetAccelerate());
			details::Dump("\tColor", particle.GetColor());
			details::Dump("\tRotation", particle.GetFixedRotation());
			details::Dump("\tScale", particle.GetFixedScale());
		}
	}
}

void SceneDatabaseImpl::Validate() const
{
	for (uint32_t nodeIndex = 0U; nodeIndex < GetNodeCount(); ++nodeIndex)
	{
		const cd::Node& node = GetNode(nodeIndex);
		assert(nodeIndex == node.GetID().Data());
	}

	for (uint32_t meshIndex = 0U; meshIndex < GetMeshCount(); ++meshIndex)
	{
		const cd::Mesh& mesh = GetMesh(meshIndex);
		assert(meshIndex == mesh.GetID().Data());
	}

	for (uint32_t blendShapeIndex = 0U; blendShapeIndex < GetBlendShapeCount(); ++blendShapeIndex)
	{
		const cd::BlendShape& blendShape = GetBlendShape(blendShapeIndex);
		assert(blendShapeIndex == blendShape.GetID().Data());
		assert(blendShape.GetMeshID().IsValid());
		assert(blendShape.GetMorphIDCount() > 0U);
	}

	for (uint32_t morphIndex = 0U; morphIndex < GetMorphCount(); ++morphIndex)
	{
		const cd::Morph& morph = GetMorph(morphIndex);
		assert(morphIndex == morph.GetID().Data());
		assert(morph.GetBlendShapeID().IsValid());
		assert(morph.GetWeight() >= 0.0f && morph.GetWeight() <= 1.0f);
	}

	for (uint32_t materialIndex = 0U; materialIndex < GetMaterialCount(); ++materialIndex)
	{
		const cd::Material& material = GetMaterial(materialIndex);
		assert(materialIndex == material.GetID().Data());
	}

	for (uint32_t textureIndex = 0U; textureIndex < GetTextureCount(); ++textureIndex)
	{
		const cd::Texture& texture = GetTexture(textureIndex);
		assert(textureIndex == texture.GetID().Data());
	}

	for (uint32_t boneIndex = 0U; boneIndex < GetBoneCount(); ++boneIndex)
	{
		const cd::Bone& bone = GetBone(boneIndex);
		assert(boneIndex == bone.GetID().Data());
		assert(bone.GetSkeletonID().IsValid());
	}

	for (uint32_t animationIndex = 0U; animationIndex < GetAnimationCount(); ++animationIndex)
	{
		const cd::Animation& animation = GetAnimation(animationIndex);
		assert(animationIndex == animation.GetID().Data());
	}

	auto CheckKeyFramesTimeOrder = [](const cd::Track& track)
	{
		// Make sure keyframes are sorted by time LessNotEqual.
		float keyFrameTime = -FLT_MAX;
		for (const auto& key : track.GetTranslationKeys())
		{
			float keyTime = key.GetTime();
			assert(keyFrameTime < keyTime);
			keyFrameTime = keyTime;
		}

		keyFrameTime = -FLT_MAX;
		for (const auto& key : track.GetRotationKeys())
		{
			float keyTime = key.GetTime();
			assert(keyFrameTime < keyTime);
			keyFrameTime = keyTime;
		}

		keyFrameTime = -FLT_MAX;
		for (const auto& key : track.GetScaleKeys())
		{
			float keyTime = key.GetTime();
			assert(keyFrameTime < keyTime);
			keyFrameTime = keyTime;
		}
	};

	for (uint32_t trackIndex = 0U; trackIndex < GetTrackCount(); ++trackIndex)
	{
		const cd::Track& track = GetTrack(trackIndex);
		assert(trackIndex == track.GetID().Data());
		assert(track.GetTranslationKeyCount() > 0 || track.GetRotationKeyCount() > 0 || track.GetScaleKeyCount() > 0);

		//assert(GetBoneByName(track.GetName()));
		CheckKeyFramesTimeOrder(track);
	}

	for (uint32_t particleIndex = 0U; particleIndex < GetParticleEmitterCount(); ++particleIndex)
	{
		const cd::ParticleEmitter& particleEmitter = GetParticleEmitter(particleIndex);
		assert(particleIndex == particleEmitter.GetID().Data());
	}
}

void SceneDatabaseImpl::Merge(cd::SceneDatabaseImpl&& sceneDatabaseImpl)
{
	uint32_t originAnimationCount = GetAnimationCount();
	uint32_t originBlendShapeCount = GetBlendShapeCount();
	uint32_t originBoneCount = GetBoneCount();
	uint32_t originNodeCount = GetNodeCount();
	uint32_t originMeshCount = GetMeshCount();
	uint32_t originMaterialCount = GetMaterialCount();
	uint32_t originMorphCount = GetMorphCount();
	uint32_t originSkeletonCount = GetSkeletonCount();
	uint32_t originSkinCount = GetSkinCount();
	uint32_t originTextureCount = GetTextureCount();
	uint32_t originTrackCount = GetTrackCount();

	for (auto& node : sceneDatabaseImpl.GetNodes())
	{
		node.SetID(GetNodeCount());
		for (auto& meshID : node.GetMeshIDs())
		{
			meshID.Set(meshID.Data() + originMeshCount);
		}
		node.SetParentID(node.GetParentID().Data() + originNodeCount);
		for (auto& childID : node.GetChildIDs())
		{
			childID.Set(childID.Data() + originNodeCount);
		}
		AddNode(cd::MoveTemp(node));
	}

	for (auto& mesh : sceneDatabaseImpl.GetMeshes())
	{
		mesh.SetID(GetMeshCount());
		for (auto& materialID : mesh.GetMaterialIDs())
		{
			materialID.Set(materialID.Data() + originMaterialCount);
		}
		for (auto& blendShapeID : mesh.GetBlendShapeIDs())
		{
			blendShapeID.Set(blendShapeID.Data() + originBlendShapeCount);
		}
		for (auto& skinID : mesh.GetSkinIDs())
		{
			skinID.Set(skinID.Data() + originSkinCount);
		}

		AddMesh(cd::MoveTemp(mesh));
	}

	for (auto& skinMesh : sceneDatabaseImpl.GetSkins())
	{
		skinMesh.SetID(GetSkinCount());
		AddSkin(cd::MoveTemp(skinMesh));
	}

	for (auto& blendshape : sceneDatabaseImpl.GetBlendShapes())
	{
		blendshape.SetID(GetBlendShapeCount());

		for (auto& morphID : blendshape.GetMorphIDs())
		{
			morphID.Set(morphID.Data() + originMorphCount);
		}

		AddBlendShape(cd::MoveTemp(blendshape));
	}

	for (auto& morph : sceneDatabaseImpl.GetMorphs())
	{
		morph.SetID(GetMorphCount());
		morph.SetBlendShapeID(morph.GetBlendShapeID().Data() + originBlendShapeCount);
		AddMorph(cd::MoveTemp(morph));
	}

	for (auto& material : sceneDatabaseImpl.GetMaterials())
	{
		material.SetID(GetMaterialCount());
		for (uint32_t textureTypeIndex = 0U; textureTypeIndex < nameof::enum_count<cd::MaterialTextureType>(); ++textureTypeIndex)
		{
			auto textureType = static_cast<cd::MaterialTextureType>(textureTypeIndex);
			if (material.IsTextureSetup(textureType))
			{
				material.SetTextureID(textureType, material.GetTextureID(textureType).Data() + originTextureCount);
			}
		}
		AddMaterial(cd::MoveTemp(material));
	}

	for (auto& texture : sceneDatabaseImpl.GetTextures())
	{
		texture.SetID(GetTextureCount());
		AddTexture(cd::MoveTemp(texture));
	}

	for (auto& animation : sceneDatabaseImpl.GetAnimations())
	{
		animation.SetID(GetAnimationCount());
		for (auto& boneTrackID : animation.GetBoneTrackIDs())
		{
			boneTrackID.Set(boneTrackID.Data() + originTrackCount);
		}
		AddAnimation(cd::MoveTemp(animation));
	}

	for (auto& track : sceneDatabaseImpl.GetTracks())
	{
		track.SetID(GetTrackCount());
		AddTrack(cd::MoveTemp(track));
	}

	for (auto& skeleton : sceneDatabaseImpl.GetSkeletons())
	{
		skeleton.SetID(GetSkeletonCount());
		skeleton.SetRootBoneID(skeleton.GetRootBoneID().Data() + originBoneCount);

		for (auto& boneID : skeleton.GetBoneIDs())
		{
			boneID.Set(boneID.Data() + originBoneCount);
		}

		AddSkeleton(cd::MoveTemp(skeleton));
	}

	for (auto& bone : sceneDatabaseImpl.GetBones())
	{
		bone.SetID(GetBoneCount());
		bone.SetParentID(bone.GetParentID().Data() + originBoneCount);
		for (auto& childID : bone.GetChildIDs())
		{
			childID.Set(childID.Data() + originBoneCount);
		}
		bone.SetSkeletonID(bone.GetSkeletonID().Data() + originSkeletonCount);
		AddBone(cd::MoveTemp(bone));
	}

	for (auto& camera : sceneDatabaseImpl.GetCameras())
	{
		camera.SetID(GetCameraCount());
		AddCamera(cd::MoveTemp(camera));
	}

	for (auto& light : sceneDatabaseImpl.GetLights())
	{
		light.SetID(GetLightCount());
		AddLight(cd::MoveTemp(light));
	}

	for (auto& particle : sceneDatabaseImpl.GetParticleEmitters())
	{
		particle.SetID(GetParticleEmitterCount());
		AddParticleEmitter(cd::MoveTemp(particle));
	}
}

void SceneDatabaseImpl::UpdateAABB()
{
	cd::AABB sceneAABB(0.0f, 0.0f);
	for (const auto& mesh : GetMeshes())
	{
		sceneAABB.Merge(mesh.GetAABB());
	}
	SetAABB(cd::MoveTemp(sceneAABB));
}

}
