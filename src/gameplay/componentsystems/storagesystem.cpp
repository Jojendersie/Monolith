#include "storagesystem.hpp"
#include "voxel/voxel.hpp"

namespace Mechanics {

	void StorageSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		
	}

	void StorageSystem::OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type, uint8_t _assignment)
	{
		m_storageVolume += Voxel::TypeInfo::GetStorageVolume(_type);
	}

	void StorageSystem::ClearSystem()
	{
		m_storageVolume = 0;
	}
}