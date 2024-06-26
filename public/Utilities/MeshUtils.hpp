#pragma once

#include "Scene/SceneDatabase.h"

namespace cd
{

using VertexBuffer = std::vector<std::byte>;
using IndexBuffer = std::vector<std::byte>;

static std::optional<VertexBuffer> BuildVertexBufferForStaticMesh(const cd::Mesh& mesh, const cd::VertexFormat& requiredVertexFormat)
{
	const bool containsPosition = requiredVertexFormat.Contains(cd::VertexAttributeType::Position);
	const bool containsNormal = requiredVertexFormat.Contains(cd::VertexAttributeType::Normal);
	const bool containsTangent = requiredVertexFormat.Contains(cd::VertexAttributeType::Tangent);
	const bool containsBiTangent = requiredVertexFormat.Contains(cd::VertexAttributeType::Bitangent);
	const bool containsUV = requiredVertexFormat.Contains(cd::VertexAttributeType::UV);
	const bool containsColor = requiredVertexFormat.Contains(cd::VertexAttributeType::Color);

	VertexBuffer vertexBuffer;
	bool mappingSurfaceAttributes = mesh.GetVertexInstanceToIDCount() > 0U;
	const uint32_t vertexInstanceCount = mappingSurfaceAttributes ? mesh.GetVertexInstanceToIDCount() : mesh.GetVertexCount();
	const uint32_t vertexFormatStride = requiredVertexFormat.GetStride();
	vertexBuffer.resize(vertexInstanceCount * vertexFormatStride);

	uint32_t vbDataSize = 0U;
	auto vbDataPtr = vertexBuffer.data();
	auto FillVertexBuffer = [&vbDataPtr, &vbDataSize](const void* pData, uint32_t dataSize)
	{
		std::memcpy(&vbDataPtr[vbDataSize], pData, dataSize);
		vbDataSize += dataSize;
	};

	for (uint32_t vertexInstance = 0; vertexInstance < vertexInstanceCount; ++vertexInstance)
	{
		if (containsPosition)
		{
			uint32_t vertexID = vertexInstance;
			if (mappingSurfaceAttributes)
			{
				vertexID = mesh.GetVertexInstanceToID(vertexInstance).Data();
			}

			constexpr uint32_t dataSize = cd::Point::Size * sizeof(cd::Point::ValueType);
			FillVertexBuffer(mesh.GetVertexPosition(vertexID).begin(), dataSize);
		}

		if (containsNormal)
		{
			constexpr uint32_t dataSize = cd::Direction::Size * sizeof(cd::Direction::ValueType);
			FillVertexBuffer(mesh.GetVertexNormal(vertexInstance).begin(), dataSize);
		}

		if (containsTangent)
		{
			constexpr uint32_t dataSize = cd::Direction::Size * sizeof(cd::Direction::ValueType);
			FillVertexBuffer(mesh.GetVertexTangent(vertexInstance).begin(), dataSize);
		}

		if (containsBiTangent)
		{
			constexpr uint32_t dataSize = cd::Direction::Size * sizeof(cd::Direction::ValueType);
			FillVertexBuffer(mesh.GetVertexBiTangent(vertexInstance).begin(), dataSize);
		}

		if (containsUV)
		{
			constexpr uint32_t dataSize = cd::UV::Size * sizeof(cd::UV::ValueType);
			FillVertexBuffer(mesh.GetVertexUV(0)[vertexInstance].begin(), dataSize);
		}

		if (containsColor)
		{
			constexpr uint32_t dataSize = cd::Color::Size * sizeof(cd::Color::ValueType);
			FillVertexBuffer(mesh.GetVertexColor(0)[vertexInstance].begin(), dataSize);
		}
	}

	assert(vbDataSize == vertexBuffer.size());
	return vertexBuffer;
}

static std::optional<VertexBuffer> BuildVertexBufferForSkeletalMesh(const cd::Mesh& mesh, const cd::VertexFormat& requiredVertexFormat, const cd::Skin& skin, const std::vector<const cd::Bone*>& skeletonBones)
{
	assert(skin.GetVertexBoneNameArrayCount() == skin.GetVertexBoneWeightArrayCount());

	const bool containsBoneIndex = requiredVertexFormat.Contains(cd::VertexAttributeType::BoneIndex);
	const bool containsBoneWeight = requiredVertexFormat.Contains(cd::VertexAttributeType::BoneWeight);
	if (!containsBoneIndex || !containsBoneWeight)
	{
		return std::nullopt;
	}

	// TODO : refine hardcoded 4 bones.
	constexpr uint32_t VertexMaxInfluenceCount = 4U;
	assert(skin.GetMaxVertexInfluenceCount() <= VertexMaxInfluenceCount);

	// TODO : 127 is hardcoded in shader logic which means invalid bone index.
	uint16_t defaultVertexBoneIndex = 127;
	float defaultVertexBoneWeight = 0.0f;
	std::vector<uint16_t> vertexBoneIndexes;
	std::vector<cd::VertexWeight> vertexBoneWeights;

	// Building a mapping table from skeleton bone name to bone index in the skeleton bone tree.
	std::map<std::string, uint16_t> skeletonBoneNameToIndex;
	for (size_t boneIndex = 0U; boneIndex < skeletonBones.size(); ++boneIndex)
	{
		const cd::Bone* pBone = skeletonBones[boneIndex];
		skeletonBoneNameToIndex[pBone->GetName()] = static_cast<uint16_t>(boneIndex);
	}

	// Build vertex buffer.
	const bool containsPosition = requiredVertexFormat.Contains(cd::VertexAttributeType::Position);
	const bool containsNormal = requiredVertexFormat.Contains(cd::VertexAttributeType::Normal);
	const bool containsTangent = requiredVertexFormat.Contains(cd::VertexAttributeType::Tangent);
	const bool containsBiTangent = requiredVertexFormat.Contains(cd::VertexAttributeType::Bitangent);
	const bool containsUV = requiredVertexFormat.Contains(cd::VertexAttributeType::UV);
	const bool containsColor = requiredVertexFormat.Contains(cd::VertexAttributeType::Color);

	VertexBuffer vertexBuffer;
	bool mappingSurfaceAttributes = mesh.GetVertexInstanceToIDCount() > 0U;
	const uint32_t vertexInstanceCount = mappingSurfaceAttributes ? mesh.GetVertexInstanceToIDCount() : mesh.GetVertexCount();
	const uint32_t vertexFormatStride = requiredVertexFormat.GetStride();
	vertexBuffer.resize(vertexInstanceCount * vertexFormatStride);

	uint32_t vbDataSize = 0U;
	auto vbDataPtr = vertexBuffer.data();
	auto FillVertexBuffer = [&vbDataPtr, &vbDataSize](const void* pData, uint32_t dataSize)
	{
		std::memcpy(&vbDataPtr[vbDataSize], pData, dataSize);
		vbDataSize += dataSize;
	};

	for (uint32_t vertexInstance = 0; vertexInstance < vertexInstanceCount; ++vertexInstance)
	{
		uint32_t vertexID = vertexInstance;
		if (mappingSurfaceAttributes)
		{
			vertexID = mesh.GetVertexInstanceToID(vertexInstance).Data();
		}

		if (containsPosition)
		{
			constexpr uint32_t dataSize = cd::Point::Size * sizeof(cd::Point::ValueType);
			FillVertexBuffer(mesh.GetVertexPosition(vertexID).begin(), dataSize);
		}

		if (containsNormal)
		{
			constexpr uint32_t dataSize = cd::Direction::Size * sizeof(cd::Direction::ValueType);
			FillVertexBuffer(mesh.GetVertexNormal(vertexInstance).begin(), dataSize);
		}

		if (containsTangent)
		{
			constexpr uint32_t dataSize = cd::Direction::Size * sizeof(cd::Direction::ValueType);
			FillVertexBuffer(mesh.GetVertexTangent(vertexInstance).begin(), dataSize);
		}

		if (containsBiTangent)
		{
			constexpr uint32_t dataSize = cd::Direction::Size * sizeof(cd::Direction::ValueType);
			FillVertexBuffer(mesh.GetVertexBiTangent(vertexInstance).begin(), dataSize);
		}

		if (containsUV)
		{
			constexpr uint32_t dataSize = cd::UV::Size * sizeof(cd::UV::ValueType);
			FillVertexBuffer(mesh.GetVertexUV(0)[vertexInstance].begin(), dataSize);
		}

		if (containsColor)
		{
			constexpr uint32_t dataSize = cd::Color::Size * sizeof(cd::Color::ValueType);
			FillVertexBuffer(mesh.GetVertexColor(0)[vertexInstance].begin(), dataSize);
		}

		vertexBoneIndexes.resize(VertexMaxInfluenceCount, defaultVertexBoneIndex);
		vertexBoneWeights.resize(VertexMaxInfluenceCount, defaultVertexBoneWeight);

		auto& vertexBoneNameArray = skin.GetVertexBoneNameArray(vertexID);
		auto& vertexBoneWeightArray = skin.GetVertexBoneWeightArray(vertexID);
		for (uint32_t vertexInfluenceIndex = 0U; vertexInfluenceIndex < VertexMaxInfluenceCount; ++vertexInfluenceIndex)
		{
			if (vertexInfluenceIndex < vertexBoneNameArray.size())
			{
				const std::string& influenceBoneName = vertexBoneNameArray[vertexInfluenceIndex];
				auto itBoneIndex = skeletonBoneNameToIndex.find(influenceBoneName);
				// Skeleton and Skin mismatch.
				assert(itBoneIndex != skeletonBoneNameToIndex.end());

				vertexBoneIndexes[vertexInfluenceIndex] = itBoneIndex->second;
				vertexBoneWeights[vertexInfluenceIndex] = vertexBoneWeightArray[vertexInfluenceIndex];
			}
			else
			{
				vertexBoneIndexes[vertexInfluenceIndex] = defaultVertexBoneIndex;
				vertexBoneWeights[vertexInfluenceIndex] = defaultVertexBoneWeight;
			}
		}

		FillVertexBuffer(vertexBoneIndexes.data(), static_cast<uint32_t>(vertexBoneIndexes.size() * sizeof(uint16_t)));
		FillVertexBuffer(vertexBoneWeights.data(), static_cast<uint32_t>(vertexBoneWeights.size() * sizeof(cd::VertexWeight)));
	}

	assert(vbDataSize == vertexBuffer.size());
	return vertexBuffer;
}

static std::optional<IndexBuffer> BuildIndexBufferesForPolygonGroup(const cd::Mesh& mesh, uint32_t polygonGroupIndex, bool forceIndex32 = false)
{
	if (polygonGroupIndex >= mesh.GetPolygonGroupCount())
	{
		return std::nullopt;
	}

	IndexBuffer indexBuffer;

	const auto& polygonGroup = mesh.GetPolygonGroup(polygonGroupIndex);
	uint32_t vertexCount = mesh.GetVertexCount();
	uint32_t polygonCount = static_cast<uint32_t>(polygonGroup.size());
	const bool useU16Index = !forceIndex32 && vertexCount <= static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()) + 1U;
	const uint32_t indexTypeSize = useU16Index ? sizeof(uint16_t) : sizeof(uint32_t);
	const uint32_t indicesCount = polygonCount * 3U;
	indexBuffer.resize(indicesCount * indexTypeSize);

	uint32_t ibDataSize = 0U;
	auto ibDataPtr = indexBuffer.data();
	auto FillIndexBuffer = [&ibDataPtr, &ibDataSize](const void* pData, uint32_t dataSize)
	{
		std::memcpy(&ibDataPtr[ibDataSize], pData, dataSize);
		ibDataSize += dataSize;
	};

	for (const auto& polygon : mesh.GetPolygonGroup(polygonGroupIndex))
	{
		for (auto vertexIndex : polygon)
		{
			cd::VertexID vertexID = vertexIndex;

			if (useU16Index)
			{
				// Endian safe. Can optimize for little endian to avoid cast.
				uint16_t vertexIndex16 = static_cast<uint16_t>(vertexID.Data());
				FillIndexBuffer(&vertexIndex16, indexTypeSize);
			}
			else
			{
				FillIndexBuffer(&vertexIndex, indexTypeSize);
			}
		}
	}

	return indexBuffer;
}

static std::vector<std::optional<IndexBuffer>> BuildIndexBufferesForMesh(const cd::Mesh& mesh, bool forceIndex32 = false)
{
	std::vector<std::optional<IndexBuffer>> indexBufferes;

	uint32_t polygonGroupCount = mesh.GetPolygonGroupCount();
	for (uint32_t polygonGroupIndex = 0U; polygonGroupIndex < polygonGroupCount; ++polygonGroupIndex)
	{
		indexBufferes.push_back(BuildIndexBufferesForPolygonGroup(mesh, polygonGroupIndex, forceIndex32));
	}

	return indexBufferes;
}

}