#pragma once

#include <string>
#include "math/matrix.hpp"

namespace Mechanics {

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
		/// \param [inout] _ship A ship where this system is registered.
		/// \param [in] _name A name to identify this system in a script.
		ComponentSystem(class Ship& _ship, const std::string& _name);

		/// \brief Detaches the system from the ship - may reassign all components
		virtual ~ComponentSystem();

		// *** Energetic state ***
		float m_energyDemand;		///< How much energy does this system and its sub system like to have?
		float m_energyIn;			///< How much energy is available (set by parent system)
		float m_energyMaxOut;		///< How much energy can be drawn by the parent system.
		float m_energyLoss;			///< How much of m_energyMaxOut is actually used form the parent system?

		/// \brief Calculate m_energyDemand and m_energyMaxOut for this frame.
		virtual void Estimate();

		/// \brief Make whatever is possible with the current energy.
		virtual void Process();

		/// \brief Called whenever a component is added to this system.
		/// \details You may update precomputed information here.
		/// \param [in] _position Voxel position in the ship model.
		/// \param [in] _type Type of the attached component.
		void OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type);

		/// \brief Called whenever a component is removed from this system.
		/// \details You may update precomputed information here.
		void OnRemove(const Math::IVec3& _position);

		/// \brief Called when a neighbor component is added/deleted. The neighbor
		///		is not necessarily part of the same system, but the current one
		///		(at position) is.
		/// \param [in] _position Position of the component to update.
		void OnNeighborChange(const Math::IVec3& _position);

		const std::string& GetName() const { return m_name; }
	private:
		std::string m_name;			///< A name which is used in the script to identify this system
		class Ship& m_ship;			///< The ship which contains this system
	};
};