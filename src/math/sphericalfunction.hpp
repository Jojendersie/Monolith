#pragma once

#include "math/vector.hpp"
#include "utilities/assert.hpp"

namespace Math {

	/// \brief Approximates a spherical function/map in form of a cube.
	/// \details In contrast to a standard cube map this one shares the edge
	///		pixels for all adjacent faces.
	///		For a 5x5 surface tessellation this makes a difference of 150-98
	///		samples. So there are 52 samples less!
	///
	///		The cube maps span a vector space: 'cm +' and 'scalar *'
	template<int N>
	class CubeMap
	{
		static_assert(N>1, "Are you sure you want to use a cube map for a single voxel?");
		static_assert(N<=1024, "So huge cube maps are probably undesired.");
	public:
		enum {
			SAMPLE_COUNT = 2*N*N + 2*N*(N-2) + 2*(N-2)*(N-2)
		};

		/// \brief Create a cube map which discretizes an arbitrary function.
		/// \details For each cube map sample the function is called once.
		/// \param [in] _function A spherical function which maps a direction to
		///		a value.
		CubeMap( std::function<float(const Math::Vec3&)> _function )
		{
			// Go over the whole cube and skip the inner parts
			Math::IVec3 voxel;
			auto end = end();
			for( auto it = begin(); begin() != end; ++it )
			{
				// Create a sampling direction +
				// evaluate and save sample
				it.value() = _function(it.direction());
			}
		}

		/// \brief Do a bilinear sample of the cube map
		float sample( const Math::Vec3& _direction ) const
		{
			// Find the principal direction
			Math::Vec3 absDir = abs(_direction);
			int pdir = 0, udir = 1, vdir = 2;
			if( absDir[1] > absDir[0] && absDir[1] > absDir[2] )		{ pdir = 1; udir = 0; }
			else if( absDir[2] > absDir[0] && absDir[2] > absDir[1] )	{ pdir = 2; vdir = 0; }

			// Get 3D outer cube position
			// Project ray to the cube faces.
			Math::Vec3 projDir = _direction / absDir[pdir];
			// Transform [-1,1] to [0, N-1)
			projDir = (projDir * 0.5f + 0.5f) * std::nexttoward(N-1, 0.0);	// TODO: constexpr vs1xx
			Assert( projDir[0] <= N && projDir[0] >= 0.0f
				&& projDir[1] <= N && projDir[1] >= 0.0f
				&& projDir[2] <= N && projDir[2] >= 0.0f,
				"Cube map sampling out of range"
			);
			// Get Integer coordinate [0,N-2]
			Math::IVec3 voxel = projDir;

			// Get 4 point samples and interpolate 
			Math::Vec3 samplePos = voxel;
			float s00 = m_data[indexOf(samplePos)];
			++samplePos[udir];
			float s10 = m_data[indexOf(samplePos)];
			++samplePos[vdir];
			float s11 = m_data[indexOf(samplePos)];
			--samplePos[udir];
			float s01 = m_data[indexOf(samplePos)];

			float fu = projDir[udir]-voxel[udir];
			float fv = projDir[vdir]-voxel[vdir];
			// Cosine interpolation for much smoother function representations
			fu = cos(fu * PI) * 0.5f + 0.5f;
			fv = cos(fv * PI) * 0.5f + 0.5f;
			return lerp(lerp(s00, s10, fu), lerp(s01, s11, fu), fv)
		}

		// TODO: derivative

		/// \brief Add two cube map functions
		CubeMap& operator += (const CubeMap& _rhs)
		{
			for( int n = 0; n < SAMPLE_COUNT; ++n )
				m_data[n] += _rhs.m_data[n];
			return *this;
		}

		CubeMap operator + (CubeMap _rhs) const
		{
			// Let the compiler do the instantiation during call
			return _rhs += *this;
		}

		/// \brief Multiply with a scalar
		CubeMap& operator *= (float _scalar)
		{
			for( int n = 0; n < SAMPLE_COUNT; ++n )
				m_data[n] *= _scalar;
			return *this;
		}

		CubeMap operator * (float _scalar) const
		{
			CubeMap cm(*this);
			return cm *= _scalar;
		}

		friend CubeMap operator * (float _scalar, CubeMap _rhs)
		{
			return _rhs *= _scalar;
		}

		/// \brief Iterate over all pixels of the map.
		/// \details There is no guaranteed order.
		struct Iterator
		{
			/// \brief Pre-increment operator only
			Iterator& operator ++ ()
			{
				// Get next box position
				if( ++m_position[2] == N )
				{
					m_position[2] = 0;
					if( ++m_position[1] == N )
					{
						m_position[1] = 0;
						++m_position[0];
						// m_position[0] == N -> end()
					}
				}
				// Advance if inside the box
				if( voxel[0] > 0 && voxel[0] < (N-1)
					&& voxel[1] > 0 && voxel[1] < (N-1)
					&& voxel[2] > 0 && voxel[2] < (N-1) )
				{
					m_position[2] += N - 2;
				}
			}

			/// \brief Returns a normalized direction of the sample.
			/// \details The direction is recomputed each time you call this
			///		function. Store the result of you need it multiple times.
			Vec3 direction()
			{
				Vec3 dir = voxel - ((N-1) * 0.5f);
				return normalize(dir);
			}

			/// \brief Return the pixels value.
			/// \details The index is recomputed each time.
			float& value()
			{
				return m_map->m_data[indexOf(m_position)];
			}
			float value() const
			{
				return m_map->m_data[indexOf(m_position)];
			}

			bool operator == (const Iterator& _rhs) const { return m_position == _rhs.m_position; }
			bool operator != (const Iterator& _rhs) const { return m_position != _rhs.m_position; }
		private:
			IVec3 m_position;
			const CubeMap* m_map;

			Iterator(CubeMap* _map) : m_position(0,0,0), m_map(_map) {}
			Iterator() : m_position(N,N,N), m_map(nullptr) {}
		};

		Iterator begin() { return Iterator(this); }
		const Iterator begin() const { return Iterator(this); }
		Iterator end() { return Iterator(); }

	private:
		float m_data[SAMPLE_COUNT];

		/// \brief Compute the index of an voxel on the surface.
		int indexOf( const Math::IVec3& _voxel )
		{
			if( _voxel[0] == 0 ) {							// N*N sides
				return _voxel[1] + N * _voxel[2];
			} else if( _voxel[0] == (N-1) ) {
				return _voxel[1] + N * _voxel[2] + N*N;
			} else if( _voxel[1] == 0 ) {					// (N-2)*N sides
				return _voxel[0] + N * _voxel[2] + 2*N*N;
			} else if( _voxel[1] == (N-1) ) {
				return _voxel[0] + N * _voxel[2] + 2*N*N+N*(N-2);
			} else if( _voxel[2] == 0 ) {					// (N-2)*(N-2) sides
				return _voxel[0] + (N-2) * _voxel[1] + 2*N*N+2*N*(N-2);
			} else {
				Assert( _voxel[2] == (N-1), "Cube map sample was not projected to the surface. Impossible!" );
				return _voxel[0] + (N-2) * _voxel[1] + 2*N*N+2*N*(N-2)+(N-2)*(N-2);
			}
		}
	};


} // namespace Math