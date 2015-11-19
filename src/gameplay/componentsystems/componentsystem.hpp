#pragma once

#include <string>
#include <ei/vector.hpp>
#include "predeclarations.hpp"

namespace Mechanics {

	/// \brief A collection of data which is communicated through all systems.
	struct SystemRequierements
	{
		ei::Vec3 thrust;	///< Required force to accelerate the ship
		ei::Vec3 torque;	///< Required torque to rotate the ship
	};

	/// \brief Base class for all different acting component types / groups.
	/// \details A components system handles energy flow and other component-specific 
	///		simulations. Each system has to define a state which is partially known
	///		and influenced by other systems.
	///
	///		It is possible to have a hierarchy of systems with a (script) defined
	///		control flow. In this case a ship can contain any number of system
	///		instances.
	///
	///		The resource flow, currently energy only, is simulated in two phases
	///		and the state is part of this base call (therefore contained in all systems).
	///		
	///		Components can reference a single system by ID
	class ComponentSystem
	{
	public:
		/// \param [in] _theShip Reference to the ship where the current
		///		system belongs too.
		/// \param [in] _name A name to identify this system in a script.
		ComponentSystem(Ship& _theShip, const std::string& _name, unsigned _id);

		virtual ~ComponentSystem();

		// *** Energetic state ***
		// All energy is measured in [kJ]
		float m_energyDemand;		///< How much energy does this system and its sub system like to have?
		float m_energyIn;			///< How much energy is available (set by parent system)
		float m_energyMaxOut;		///< How much energy can be drawn by the parent system.
		float m_energyLoss;			///< How much of m_energyMaxOut is actually used form the parent system?

		/// \brief Calculate m_energyDemand and m_energyMaxOut for this frame.
		virtual void Estimate(float _deltaTime, SystemRequierements& _requirements) {}

		/// \brief Make whatever is possible with the current energy.
		virtual void Process(float _deltaTime, SystemRequierements& _provided) {}

		/// \brief Called during recomputation if a component is added to the systems.
		/// \details Update precomputed information here.
		/// \param [in] _position Voxel position in the ship model.
		/// \param [in] _type Type of the attached component.
		/// \param [in] _assignment The assignment to the responsible system.
		virtual void OnAdd(const ei::IVec3& _position, Voxel::ComponentType _type, uint8 _assignment) {}

		/// \brief Recursively clear the system state. This is done if everything is recomputed.
		virtual void ClearSystem() {}

		const std::string& GetName() const { return m_name; }
	protected:
		class Ship& m_ship;
		const unsigned m_id;						///< System ID for this system and all its direct sub systems.
	private:
		std::string m_name;			///< A name which is used in the script to identify this system
	};
}