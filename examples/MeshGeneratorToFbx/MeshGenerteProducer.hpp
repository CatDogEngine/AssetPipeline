#pragma once

#include "Framework/IProducer.h"
#include "Math/Box.hpp"
#include "Math/Plane.hpp"
#include "Math/Sphere.hpp"
#include "Math/MeshGenerator.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"

#include <string>

namespace cdtools
{

class MeshGenerteProducer final : public IProducer
{
public:
	MeshGenerteProducer() = delete;
	explicit MeshGenerteProducer(const char* pFilePath) : m_filePath(pFilePath) {}
	MeshGenerteProducer(const MeshGenerteProducer&) = delete;
	MeshGenerteProducer& operator=(const MeshGenerteProducer&) = delete;
	MeshGenerteProducer(MeshGenerteProducer&&) = delete;
	MeshGenerteProducer& operator=(MeshGenerteProducer&&) = delete;
	virtual ~MeshGenerteProducer() {}

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override
	{
		pSceneDatabase->SetName("MeshGenerator");

		cd::Box box(cd::Vec3f(-1.0f, -1.0f, -1.0f), cd::Vec3f(1.0f, 1.0f, 1.0f));
		cd::VertexFormat vertexFormat;
		vertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		vertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		std::optional<cd::Mesh> optMesh = cd::MeshGenerator::Generate(box, vertexFormat);
		if (!optMesh.has_value())
		{
			return;
		}

		pSceneDatabase->SetMeshCount(1);
		pSceneDatabase->AddMesh(MoveTemp(optMesh.value()));
	}

private:
	std::string m_filePath;
};

}