#include "FbxProducer.h"
#include "Framework/Processor.h"
#include "Scene/SceneDatabase.h"
#include "Utilities/MeshUtils.hpp"
#include "Utilities/PerformanceProfiler.h"

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : input file path
	if (argc != 2)
	{
		return 1;
	}

	using namespace cdtools;

	PerformanceProfiler profiler("AssetPipeline");

	const char* pInputFilePath = argv[1];
	
	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();
	FbxProducer producer(pInputFilePath);
	Processor processor(&producer, nullptr, pSceneDatabase.get());
	processor.Run();

	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		auto& skeleton = pSceneDatabase->GetSkeleton(0);
		std::vector<const cd::Bone*> bones;
		for (auto boneID : skeleton.GetBoneIDs())
		{
			bones.push_back(&pSceneDatabase->GetBone(boneID.Data()));
		}
		auto vb = cd::BuildVertexBufferForSkeletalMesh(mesh, mesh.GetVertexFormat(),pSceneDatabase->GetSkin(0), bones);
		assert(vb.has_value());

		auto ibs = cd::BuildIndexBufferesForMesh(mesh);
		for (const auto& ib : ibs)
		{
			assert(ib.has_value());
		}
	}

	return 0;
}