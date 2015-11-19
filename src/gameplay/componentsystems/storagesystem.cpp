#include "storagesystem.hpp"
#include "voxel/voxel.hpp"

using namespace ei;

namespace Mechanics {

	void StorageSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		
	}

	void StorageSystem::OnAdd(const IVec3& _position, Voxel::ComponentType _type, uint8 _assignment)
	{
		m_storageVolume += Voxel::TypeInfo::GetStorageVolume(_type);
	}

	void StorageSystem::ClearSystem()
	{
		m_storageVolume = 0;
	}
}