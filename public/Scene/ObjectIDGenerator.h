#pragma once

#include "ObjectID.h"

#include <assert.h>
#include <optional>
#include <unordered_map>

namespace cdtools
{

// Different objects have different id generate rules to be unique and quick to search in the scene database.
// 1. Vertex prefers to use position(x, y, z) to calculate a hash value if we don't duplicate vertices.
// 2. Texture prefers to use texture file path string as a hash value to reuse the same file.
// The range of ObjectIDs is better to equal to the size of object array so the objectID is also the index to
// address object quickly.
template<typename Oty>
class ObjectIDGenerator
{
private:
	using Vty = Oty::ValueType;

public:
	explicit ObjectIDGenerator() = default;
	ObjectIDGenerator(const ObjectIDGenerator&) = delete;
	ObjectIDGenerator& operator=(const ObjectIDGenerator&) = delete;
	ObjectIDGenerator(ObjectIDGenerator&&) = delete;
	ObjectIDGenerator& operator=(ObjectIDGenerator&&) = delete;
	~ObjectIDGenerator() = default;

	// Set generated id range in [min, max]
	void SetRange(Vty min, Vty max) { m_minID = min; m_maxID = max; }

	// Pass hash value is used to keep allocating unique ObjectID.
	Oty AllocateID(Vty hashValue, bool& isReused)
	{
		const auto itID = m_objectIDLUT.find(hashValue);
		if(itID != m_objectIDLUT.end())
		{
			isReused = true;
			return itID->second;
		}

		isReused = false;
		Vty newID = m_currentID++;
		assert(newID >= m_minID && newID <= m_maxID && "The allocated id is out of range.");
		m_objectIDLUT[hashValue] = Oty(newID);
		return m_objectIDLUT[hashValue];
	}

private:
	Vty m_minID = Oty::MinID;
	Vty m_maxID = Oty::MaxID;
	Vty m_currentID = static_cast<Vty>(0);

	std::unordered_map<Vty, Oty> m_objectIDLUT;
};

}