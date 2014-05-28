#pragma once

#include "math/vector.hpp"
#include "utilities/assert.hpp"

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
		for( voxel[0] = 0; voxel[0] < N; ++voxel[0] )
		{
			for( voxel[1] = 0; voxel[1] < N; ++voxel[1] )
			{
				for( voxel[2] = 0; voxel[2] < N; ++voxel[2] )
				{
					if( voxel[0] == 0 || voxel[0] == (N-1)
						|| voxel[1] == 0 || voxel[1] == (N-1)
						|| voxel[2] == 0 || voxel[2] == (N-1) )
					{
						// Create a sampling direction
						Vec3 dir = voxel - ((N-1) * 0.5f);
						dir = normalize(dir);

						// evaluate and save sample
						m_data[indexOf(voxel)] = _function(dir);
					}
				}
			}
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

		/* STANDARD CUBE MAP easy up to point sampling, adjacency handling
		for interpolation difficult
		// Get tile
		int t = pdir * 2 + (_direction[pdir] > 0.0f ? 1 : 0);
		// Get coordinate on tile
		float fu = (_direction[vdir] / absDir[pdir] + 1.0f) / 2.0f;
		float fv = (_direction[udir] / absDir[pdir] + 1.0f) / 2.0f;
		int u = (int)fu;
		int v = (int)fv;

		// Get adjacent indices
		int u1 = u+1;
		if( u1 > tesselation )
		{
			u1 = 0;
			...
		}
		...
		*/

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














/* PROBLEM: To support bilinear samples the paper used extra seem-point-copies.
This would increase the required data too hard.
/// \brief A equal solid-angle spherical map.
/// \datails Source: http://graphics.tju.edu.cn/people/lwan/paper/sphquadmap/sphquadmap.htm
template<int tesselation>
class HealPixMap
{
public:
	float sample( const Vec3& _direction ) const
	{
		int tile, u, v;
		float t, tt, tf;
		float ya, tmp;

		// t = phi * 2 / pi
		t = atan2(-_direction[2], _direction[0]) * 0.636619772f;
		t += t < 0.0f ? 4.0f : 0.0f;
		assert( t >= 0.0f );

		// Equatorial or polar zone: y in [-2/3, 2/3]?
		ya = 3.0f * abs(_direction[1]);
		int tn = (int)t;	// floor (t is positive)
		tf = t - tn;

		if( ya < 2.0f )
		{
			// Equatorial zone
			tt    = t + 0.5f;
			tmp   = _direction[1] * 0.75;	// y*3/4
			assert(tt + tmp >= 0.0f);
			assert(tt - tmp >= 0.0f);
			float fu = tt + tmp;
			float fv = tt - tmp;
			u = (int)fu;
			v = (int)fv;
			tile = min(u, v);
			tile += int(4 + (sign(v-u)-Math::floor(tile*0.25))*4);
			// u, v are the local coordinates within tile
			u = int((fu - (int)(fu)) * tesselation);
			v = int((fv - (int)(fv)) * tesselation);
			//res   = float3(fn, u, v);
		} else {
			// Polar zone
			tmp   = sqrt(3.0f - ya);
			bool south = _direction[1] < 0;              // check if it is south or north pole 
			tt    = tmp * tf;
			tmp   = 1.0 - tmp;
			tf    = south ? tmp : 0.0f;
			// Compute face index and local coordinates in the polar zone
			u = int(tmp-tf+tt);
			v = int(tf+tt);
			tile = tn + south ? 8 : 0;
		}

		return m_data(tile*tesselation*tesselation + v*tesselation + u);
	}

private:
	float m_data[12*tesselation*tesselation];
};*/