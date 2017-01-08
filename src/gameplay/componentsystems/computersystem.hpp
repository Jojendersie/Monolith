#pragma once

#include "batterysystem.hpp"
#include "drivesystem.hpp"
#include "reactorsystem.hpp"
#include "sensorsystem.hpp"
#include "shieldsystem.hpp"
#include "storagesystem.hpp"
#include "weaponsystem.hpp"
#include "utilities/scriptengineinst.hpp"
#include <vector>

namespace Mechanics {

	/// \brief A computer maintains the resource distribution in a ship.
	class ComputerSystem: public ComponentSystem
	{
	public:
		ComputerSystem(Ship& _theShip, const std::string& _name);
		~ComputerSystem();

		virtual void Estimate(float _deltaTime, SystemRequierements& _requirements) override;
		virtual void Process(float _deltaTime, SystemRequierements& _provided) override;
		virtual void OnAdd(const ei::IVec3& _position, Voxel::ComponentType _type, uint8 _assignment) override;
		virtual void ClearSystem() override;

	//	float TempGetCharge() const { return m_batteries.RelativeCharge(); }
		// compiles the given script and reboots the system
		void Flash();
		// writes all owned component systems to a script file.
		void ExportSystems(std::ofstream& _file);

		void GetDisplayVars(Script::VarRefContainer& _target);
	private:

		std::vector<ComputerSystem> m_subSystems;	///< May contain more computers
		BatterySystem m_batteries;					///< Handle all attached batteries (not these of the sub systems)
		DriveSystem m_drives;						///< Handle all attached drives (not these of the sub systems)
		ReactorSystem m_reactors;					///< Handle all attached reactors (not these of the sub systems)
		SensorSystem m_sensors;						///< Handle all attached sensors (not these of the sub systems)
		ShieldSystem m_shields;						///< Handle all attached shields (not these of the sub systems)
		StorageSystem m_storage;					///< Handle all attached storages (not these of the sub systems)
		WeaponSystem m_weapons;						///< Handle all attached weapons (not these of the sub systems)
		bool m_enabled;								///< The system is disabled if its computer voxel is destroyed. Repairing this ship reenables the system again.

		NaReTi::FunctionHandle m_script;
		NaReTi::Module* m_scriptModule;
		friend class Ship;
	};

}