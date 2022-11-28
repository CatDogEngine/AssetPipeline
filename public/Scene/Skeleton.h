#pragma once

#include "Core/ISerializable.hpp"
#include "ObjectID.h"

namespace cdtools
{

class Skeleton final : public ISerializable
{
public:
	Skeleton() = delete;
	explicit Skeleton(SkeletonID skeletonID);
	explicit Skeleton(std::ifstream& fin) { ImportBinary(fin); }
	Skeleton(const Skeleton&) = default;
	Skeleton& operator=(const Skeleton&) = default;
	Skeleton(Skeleton&&) = default;
	Skeleton& operator=(Skeleton&&) = default;
	~Skeleton() = default;

	// ISerializable
	virtual void ImportBinary(std::ifstream& fin) override;
	virtual void ExportBinary(std::ofstream& fout) const override;

private:
	SkeletonID m_id;
};

}