#include "storagesystem.hpp"
#include "voxel/voxel.hpp"

namespace Mechanics {

	void StorageSystem::Estimate(float _deltaTime)
	{
		
	}

	void StorageSystem::OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type)
	{
		m_storageVolume += Voxel::TypeInfo::GetStorageVolume(_type);
	}

	void StorageSystem::OnRemove(const Math::IVec3& _position, Voxel::ComponentType _type)
	{
		m_storageVolume -= Voxel::TypeInfo::GetStorageVolume(_type);
	}

	void StorageSystem::OnNeighborChange(const Math::IVec3& _position)
	{
		//currently impossible
	}
}