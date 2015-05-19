#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class StorageSystem: public ComponentSystem
	{
	public:
		StorageSystem(Ship& _theShip) : ComponentSystem(_theShip, "Storage") {}

		virtual void Estimate(float _deltaTime, SystemRequierements& _requirements) override;
		virtual void OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type) override;
		virtual void OnRemove(const Math::IVec3& _position, Voxel::ComponentType _type) override;
		virtual void OnNeighborChange(const Math::IVec3& _position) override;
	private:
		float m_storageVolume; ///< amount of stuff that can be stored inside in basic resource units
	};

}