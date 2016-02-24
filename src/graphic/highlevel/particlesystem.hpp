#pragma once

#include <ei/3dtypes.hpp>
#include <hybridarray.hpp>
#include <vector>
#include <memory>

#include "utilities/metaproghelper.hpp"
#include "utilities/assert.hpp"
//#include "utilities/flagoperators.hpp"

namespace Graphic {

	/// \brief Component based particle systems.
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
		struct Component {
			enum Val: uint
			{
				POSITION = 1,	///< Every particle needs a position. Always add this component!
				VELOCITY = 2,	///< Change of position over time.
				LIFETIME = 4,	///< Lifetime in seconds until despawn. If not provided the particle lasts forever.
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
		static void Simulate(float _deltaTime);

	private:

		struct NoComponent
		{
			template<typename T>
			static void Add(const T&) {}
			static void Remove(size_t _idx) {}
		};

		struct PositionComponents
		{
			std::vector<ei::Vec3> m_positions;
			void Add(const ei::Vec3& _pos) { m_positions.push_back(_pos); }
			void Remove(size_t _idx) { m_positions[_idx] = m_positions.back(); m_positions.pop_back(); }
		};

		struct VeloctiyComponents
		{
			std::vector<ei::Vec3> m_velocities;
			void Add(const ei::Vec3& _vel) { m_velocities.push_back(_vel); }
			void Remove(size_t _idx) { m_velocities[_idx] = m_velocities.back(); m_velocities.pop_back(); }
		};

		struct LifetimeComponents
		{
			std::vector<float> m_lifetimes;
			void Add(float _life) { m_lifetimes.push_back(_life); }
			void Remove(size_t _idx) { m_lifetimes[_idx] = m_lifetimes.back(); m_lifetimes.pop_back(); }
		};

		template<typename Base>
		struct FuncNOP
		{
			void Run(float _deltaTime) {}
		};

		template<typename Base>
		struct FuncAdvection : public virtual Base
		{
			void Run(float _deltaTime)
			{
				for(size_t i = 0; i < m_velocities.size(); ++i)
					m_positions[i] += m_velocities[i];
			}
		};

		template<typename Base>
		struct FuncDie : public virtual Base
		{
			void Run(float _deltaTime)
			{
				for(size_t i = 0; i < m_lifetimes.size(); ++i)
				{
					m_lifetimes[i] -= _deltaTime;
					if(m_lifetimes[i] < 0.0f)
						Remove(i--);
				}
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
			public inherit_conditional<(PFlags & (uint)Component::POSITION) != 0, PositionComponents, NoComponent>,
			public inherit_conditional<(PFlags & (uint)Component::VELOCITY) != 0, VeloctiyComponents, NoComponent>,
			public inherit_conditional<(PFlags & (uint)Component::LIFETIME) != 0, LifetimeComponents, NoComponent>
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
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & (uint)Component::POSITION) != 0, PositionComponents, NoComponent>::Add(_param0);
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & (uint)Component::VELOCITY) != 0, VeloctiyComponents, NoComponent>::Add(_param0);
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & (uint)Component::LIFETIME) != 0, LifetimeComponents, NoComponent>::Add(_param0);
				AddParticle<NextFlag<TheFlag, false>::Get, RemainingFlags ^ TheFlag>(_params...);
			}

			void Remove(size_t _idx)
			{
				inherit_conditional<(PFlags & (uint)Component::POSITION) != 0, PositionComponents, NoComponent>::Remove(_idx);
				inherit_conditional<(PFlags & (uint)Component::VELOCITY) != 0, VeloctiyComponents, NoComponent>::Remove(_idx);
				inherit_conditional<(PFlags & (uint)Component::LIFETIME) != 0, LifetimeComponents, NoComponent>::Remove(_idx);
			}
		};

		// Base class to make functions of particle systems callable independent of their
		// type.
		class SubSystemActions
		{
		public:
			virtual void Simulate(float _deltaTime) {}
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
			public inherit_conditional<(PFlags & (uint)Component::POSITION) != 0 && (PFlags & (uint)Component::VELOCITY) != 0, FuncAdvection<SubSystemData<PFlags>>, FuncNOP<SubSystemData<PFlags>>>,
			public inherit_conditional<(PFlags & (uint)Component::LIFETIME) != 0, FuncDie<SubSystemData<PFlags>>, FuncNOP<SubSystemData<PFlags>>>,
			public SubSystemActions
		{
		public:
			SubSystem()
			{
				// Register the system where the runtime can see it.
				ParticleSystem::m_registeredSystems.push_back(this);
			}

			void Simulate(float _deltaTime) override
			{
				inherit_conditional<((PFlags & Component::POSITION) != 0) && ((PFlags & Component::VELOCITY) != 0), FuncAdvection<SubSystemData<PFlags>>, FuncNOP<SubSystemData<PFlags>>>::Run(_deltaTime);
				inherit_conditional<((PFlags & Component::LIFETIME) != 0), FuncDie<SubSystemData<PFlags>>, FuncNOP<SubSystemData<PFlags>>>::Run(_deltaTime);
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

	using PSComponent = ParticleSystem::Component;

} // namespace Graphic