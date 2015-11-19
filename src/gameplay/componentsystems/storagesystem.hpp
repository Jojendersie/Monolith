#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class StorageSystem: public ComponentSystem
	{
	public:
		StorageSystem(Ship& _theShip, unsigned _id) : ComponentSystem(_theShip, "Storage", _id) {}

		virtual void Estimate(float _deltaTime, SystemRequierements& _requirements) override;
		virtual void OnAdd(const ei::IVec3& _position, Voxel::ComponentType _type, uint8 _assignment) override;
		virtual void ClearSystem() override;
	private:
		float m_storageVolume; ///< amount of stuff that can be stored inside in basic resource units
	};

}