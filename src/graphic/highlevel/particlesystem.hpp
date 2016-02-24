#pragma once

#include <ei/3dtypes.hpp>
#include <hybridarray.hpp>
#include <vector>
#include <memory>

#include "utilities/metaproghelper.hpp"
#include "utilities/assert.hpp"
//#include "utilities/flagoperators.hpp"

namespace Graphic {

	/// \brief Component based particle system.
	/// \details There are different rendering systems with different parameters.
	///		A particle can also have different simulation components. Everything can
	///		be combined arbitrary.
	class ParticleSystem
	{
	public:
		/// \brief A list of valid components for different particle system definitions.
		/// \details All flags are orthogonal and any combination is valid. The
		///		functionality is inferred automatically. E.g. If you add POSITION
		///		and VELOCITY the particle will be moved each frame.
		struct PSComponent {
			enum Val: uint
			{
				POSITION = 1,
				VELOCITY = 2,
			};
		};

		enum struct RenderType
		{
			BLOB,
			QUAD,
			BOX,
			RAY
		};

		/// \brief Add a custom particle to the appropriate system. Simulation
		///		and rendering will be done efficiently together with all other
		///		particles of the same type.
		template<uint PFlags, typename... Params>
		static void AddParticle(Params... _params)
		{
			SubSystems<PFlags>::Get().AddParticle<1, PFlags>(_params...);
		}

		/// \brief Simulate all kinds of particle systems.
		static void Simulate();

	private:

		struct NoComponent
		{
			template<typename T>
			static void Add(const T&) {}
		};

		struct PositionComponents
		{
			std::vector<ei::Vec3> m_positions;
			void Add(const ei::Vec3& _pos) { m_positions.push_back(_pos); }
		};

		struct VeloctiyComponents
		{
			std::vector<ei::Vec3> m_velocities;
			void Add(const ei::Vec3& _vel) { m_velocities.push_back(_vel); }
		};

		template<typename Base>
		struct FuncNOP
		{
			void Run() {}
		};

		template<typename Base>
		struct FuncAdvection : public virtual Base
		{
			void Run()
			{
				for(size_t i = 0; i < m_velocities.size(); ++i)
					m_positions[i] += m_velocities[i];
			}
		};

		// The SubSystemData creates a type which contains the specific properties
		// defined by the flag word. This is the aggregation of different components into
		// a single identity.
		// It further creates an AddParticle which properly called only succeeds if an
		// initial value for each property is given with the proper type.
		// Example: SubSystemData<POSTITION | VELOCITY> will contain the arrays
		// m_positions and m_velocity.
		template<uint PFlags>
		class SubSystemData :
			public inherit_conditional<(PFlags & (uint)PSComponent::POSITION) != 0, PositionComponents, NoComponent>,
			public inherit_conditional<(PFlags & (uint)PSComponent::VELOCITY) != 0, VeloctiyComponents, NoComponent>
		{
			// Compiletime while loop which finds the next flag set in PFlags
			template<uint TheFlag, bool>
			struct NextFlag {
				static const uint Get = TheFlag;
			};
			template<uint TheFlag>
			struct NextFlag<TheFlag, false> {
				static const uint Get = NextFlag<TheFlag << 1, ((TheFlag << 1) & PFlags) != 0 || TheFlag == 0>::Get;
			};
		public:
			template<uint TheFlag, uint RemainingFlags>
			void AddParticle() // End of recursion
			{
				Assert(RemainingFlags == 0, "Too few parameters provided!");
			}

			template<uint TheFlag, uint RemainingFlags, typename Param0, typename... Params>
			void AddParticle(const Param0& _param0, Params... _params)
			{
				Assert(TheFlag != 0, "Too many parameters provided!");
				// The flag is set -> consume the argument.
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & (uint)PSComponent::POSITION) != 0, PositionComponents, NoComponent>::Add(_param0);
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & (uint)PSComponent::VELOCITY) != 0, VeloctiyComponents, NoComponent>::Add(_param0);
				AddParticle<NextFlag<TheFlag, false>::Get, RemainingFlags ^ TheFlag>(_params...);
			}
		};

		// Base class to make functions of particle systems callable independent of their
		// type.
		class SubSystemActions
		{
		public:
			virtual void Simulate() {}
		};

		// The SubSystem extends the SubSystemData by mixing in functions conditional.
		// Dependent on the available components different simulation functions are glued
		// together.
		// Conditional inheritance is used to execute code conditional which otherwise
		// would not compile. The trick is that an empty dummy function from the default
		// type gets called instead of invalid code if a condition is not fulfilled.
		template<uint PFlags>
		class SubSystem :
			public virtual SubSystemData<PFlags>,
			public inherit_conditional<(PFlags & (uint)PSComponent::POSITION) != 0 && (PFlags & (uint)PSComponent::VELOCITY) != 0, FuncAdvection<SubSystemData<PFlags>>, FuncNOP<SubSystemData<PFlags>>>,
			public SubSystemActions
		{
		public:
			SubSystem()
			{
				// Register the system where the runtime can see it.
				ParticleSystem::m_registeredSystems.push_back(this);
			}

			void Simulate() override
			{
				inherit_conditional<((PFlags & PSComponent::POSITION) != 0) && ((PFlags & PSComponent::VELOCITY) != 0), FuncAdvection<SubSystemData<PFlags>>, FuncNOP<SubSystemData<PFlags>>>::Run();
			}
		};

		// Kind of a hash-map to instantiate the dynamical produced types. For each type
		// which is used in the program a single instance will be created and mapped.
		// Thus all unused combinations of flags will not cost us anything.
		// I.e. the mapping is sparse!
		template<uint Key>
		class SubSystems {
		public:
			static SubSystem<Key>& Get () {
				static SubSystem<Key> v;
				return v;
			}
		};

		// A list of pointers to all instances of the above hash map.
		static std::vector<SubSystemActions*> m_registeredSystems;
	};


} // namespace Graphic