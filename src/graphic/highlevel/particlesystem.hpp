#pragma once

#include <ei/3dtypes.hpp>
#include <hybridarray.hpp>
#include <vector>
#include <memory>

#include "utilities/metaproghelper.hpp"
//#include "utilities/flagoperators.hpp"

namespace Graphic {

	/// \brief Component based particle system.
	/// \details There are different rendering systems with different parameters.
	///		A particle can also have different simulation components. Everything can
	///		be combined arbitrary.
	class ParticleSystem
	{
	public:
		enum struct Comp
		{
			POSITION = 1,
			VELOCITY = 2,
		};

		/// \brief Base class for data components of particles. There are components
		///		which all particle use.
		/*struct Component
		{
		protected:
		private:
			int particleID;
			friend class ParticleSystem;
		};

		struct VeloctiyComponent: public Component
		{
			ei::Vec3 velocity;
		};

		struct RotationComponent: public Component
		{
			ei::Quaternion rotation;
		};

		/// \brief A spawner component adds additional properties to new particles.
		struct SpawnerPropertyComponent: public Component
		{
			/// \brief Add a new component to an already generated particle
			virtual void extend(int _particleID) const;
		};

		/// \brief Adds a spawner ability to the current particle
		struct SpawnerComponent: public Component
		{
			float particlesPerSecond;
			/// \brief Create a new particle now
			virtual int create() const;
		protected:
			Jo::HybridArray<int, 4> spawnerProperties;
		};

		/// \brief Spawn particles at the position of this spawner particle.
		struct PointSpawnerComponent: public SpawnerComponent
		{
			virtual int create() const override;
		};

		struct BoxSpawnerComponent: public SpawnerComponent
		{
			ei::Box box;		///< Spawn area box relative to the current particle position
			virtual int create() const override;
		};

		/// \brief Set a random spawn direction
		/// \brief If the spawner is moving its own velocity is added too.
		struct SpawnerVelocityComponent: public SpawnerPropertyComponent
		{
			float spawnVelocity;		///< Magnitude of initial velocity of newly spawned particles.
			virtual void extend(int _particleID) const override;
		};*/

		enum struct RenderType
		{
			BLOB,
			QUAD,
			BOX,
			RAY
		};

		/// \brief Add an particle and get its id to be able to add components
		/*int AddParticle(const ei::Vec3& _position, const ei::Vec3& _scale);
		void AddComponent(int _particleID, const VeloctiyComponent& _component);
		void AddComponent(int _particleID, const RotationComponent& _component);
		void AddComponent(int _particleID, const PointSpawnerComponent& _component);
		void AddComponent(int _particleID, const BoxSpawnerComponent& _component);*/

		template<uint PFlags, typename... Params>
		void AddParticle(Params... _params)
		{
			SubSystems<PFlags>::Get().AddParticle<1>(_params...);
		}

		public:
		/*struct ComponentHandle
		{
		};

		struct Particle
		{
			Jo::HybridArray<int, 4> components;		///< Indices of optional components
		};

		std::unordered_map<> m_subSystems;

		std::vector<Particle> m_particles;
		/// Each particle has a position
		std::vector<ei::Vec3> m_partPositions;
		/// Each particle has a scale
		std::vector<ei::Vec3> m_partScale;
		std::vector<VeloctiyComponent> m_coVelocity; // TODO: velocity ist pflicht
		std::vector<RotationComponent> m_coRotation;
		std::vector<PointSpawnerComponent> m_coPointSpawner;
		std::vector<BoxSpawnerComponent> m_coBoxSpawner;*/

		struct NoComponent
		{
			template<typename T>
			static void add(const T&) {}
		};

		struct PositionComponents
		{
			std::vector<ei::Vec3> m_positions;
			void add(const ei::Vec3& _pos) { m_positions.push_back(_pos); }
		};

		struct VeloctiyComponents
		{
			std::vector<ei::Vec3> m_velocities;
			void add(const ei::Vec3& _vel) { m_velocities.push_back(_vel); }
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
			public inherit_conditional<(PFlags & (uint)Comp::POSITION) != 0, PositionComponents, NoComponent>,
			public inherit_conditional<(PFlags & (uint)Comp::VELOCITY) != 0, VeloctiyComponents, NoComponent>
		{
		public:
			template<uint>
			void AddParticle() { /*End of recursion*/ }

			template<uint TheFlag, typename Param0, typename... Params>
			void AddParticle(const Param0& _param0, Params... _params)
			{
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & (uint)Comp::POSITION) != 0, PositionComponents, NoComponent>::add(_param0);
				inherit_conditional<(TheFlag & PFlags) != 0 && (TheFlag & (uint)Comp::VELOCITY) != 0, VeloctiyComponents, NoComponent>::add(_param0);
				AddParticle<TheFlag << 1>(_params...);
			}
		};

		// The SubSystem extends the SubSystemData by mixing in functions conditional.
		// Dependent on the available components different simulation functions are glued
		// together.
		// Conditional inheritance is used to execute code conditional which otherwise
		// would not compile. The trick is that an empty dummy function from the default
		// type gets called instead of invalid code if a condition is not fullfilled.
		template<uint PFlags>
		class SubSystem :
			public virtual SubSystemData<PFlags>,
			public inherit_conditional<(PFlags & (uint)Comp::POSITION) != 0 && (PFlags & (uint)Comp::VELOCITY) != 0, FuncAdvection<SubSystemData<PFlags>>, FuncNOP<SubSystemData<PFlags>>>
		{
		public:
			void Run()
			{
				inherit_conditional<PFlags & Comp::POSITION != 0 && PFlags & Comp::VELOCITY != 0, FuncAdvection<SubSystemData<PFlags>>, FuncNOP<SubSystemData<PFlags>>>::Run();
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
	};


} // namespace Graphic