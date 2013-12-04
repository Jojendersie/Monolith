#include "sphere.hpp"
#include "vector3.hpp"
#include "matrix.hpp"
#include <cassert>

namespace Math {

	// ********************************************************************* //
	Sphere::Sphere( const Vec3& _p1, const Vec3& _p2 )
	{
		m_center = (_p1 + _p2) * 0.5f;
		m_radiusSqr = (_p1 - _p2).LengthSq() * 0.25f;
		m_radius = sqrt(m_radiusSqr);
	}

	// ********************************************************************* //
	Sphere::Sphere( const Vec3& _p1, const Vec3& _p2, const Vec3& _p3 )
	{
		// The center of the circumscribed circle is at (barycentric coords)
		// v0*sin(2 alpha) + v1*sin(2 beta) + v2*sin(2 gamma) and has the radius
		// abc/4A.
		Vec3 c = _p1 - _p2;	float csq = c.LengthSq();
		Vec3 a = _p2 - _p3;	float asq = a.LengthSq();
		Vec3 b = _p3 - _p1;	float bsq = b.LengthSq();

		// one of the sifdes could be the longest side - the minimum sphere is
		// defined through only two points.
		// This can also handle the coplanar case.
		if( csq + bsq <= asq ) *this = Sphere(_p2, _p3);
		else if( asq + bsq <= csq ) *this = Sphere(_p2, _p1);
		else if( asq + csq <= bsq ) *this = Sphere(_p3, _p1);
		else {
			float area2Sq = 2*cross(a, c).LengthSq();
			m_center = 
				  _p1 * (-dot(c,b)*asq/area2Sq)
				+ _p2 * (-dot(c,a)*bsq/area2Sq)
				+ _p3 * (-dot(b,a)*csq/area2Sq);
			m_radiusSqr = asq*bsq*csq/(2*area2Sq);
			m_radius = sqrt(m_radiusSqr);
		}
	}

	// ********************************************************************* //
	Sphere::Sphere( const Vec3& _p1, const Vec3& _p2, const Vec3& _p3, const Vec3& _p4 )
	{
		// It is possible that not all 4 points lie on the surface of the sphere.
		// Just two of them could already define a sphere enclosing all other.
		// So we need to compute any combination of possible spheres (14), but
		// luckily we know a direct solution for any combination of 3 points.
		// The reduces the work to 4 cases: build a bounding sphere for 3 points
		// and have a look if the fourth point is inside.
		*this = Sphere( _p1, _p2, _p3 );
		if( !Encloses( _p4 ) ) {
			*this = Sphere( _p1, _p2, _p4 );
			if( !Encloses( _p3 ) ) {
				*this = Sphere( _p1, _p3, _p4 );
				if( !Encloses( _p2 ) ) {
					*this = Sphere( _p2, _p3, _p4 );
					if( !Encloses( _p1 ) ) {
						// All 4 points are on the boundary -> construct sphere
						// from 4 points.
						Vec3 a = _p2 - _p1;
						Vec3 b = _p3 - _p1;
						Vec3 c = _p4 - _p1;

						// TODO: Mat3x3
						Mat4x4 m = Mat4x4( _p2.x, _p2.y, _p2.z, 0.0f,
										   _p3.x, _p3.y, _p3.z, 0.0f,
										   _p4.x, _p4.y, _p4.z, 0.0f,
										   0.0f,  0.0f,  0.0f,  1.0f );

						float denominator = 0.5f / m.Det();

						Vec3 o = (c.LengthSq() * a.Cross(b) +
								  b.LengthSq() * c.Cross(a) +
								  a.LengthSq() * b.Cross(c)) * denominator;

						m_center = _p1 + o;
						m_radiusSqr = o.LengthSq();
						m_radius = sqrt(m_radiusSqr);
					}
				}
			}
		}
	}


	// ********************************************************************* //
	// Recursive subfunction for Welzl's algorithm
	// _pointSet is a list of all points where the boundary points are always
	// at the front. _boundarySet says how many of the first points must be
	// interpreted for the boundary
	Sphere MinimalBoundingSphere( std::list<Vec3>& _pointSet, int _n, int _boundarySet )
	{
		Sphere MBS(Vec3(0.0f));

		// If the boundary list is full or all points where added stop
		if( _pointSet.empty() || _boundarySet == 4 )
		{
			switch(_boundarySet)
			{
			case 0: return Sphere(Vec3(0.0f));
			case 1: return Sphere(_pointSet.front());
			case 2: {
				auto p = _pointSet.begin();
				Vec3 v0 = *p; ++p;
				return Sphere(v0, *p);
			}
			case 3: {
				auto p = _pointSet.begin();
				Vec3 v0 = *p; ++p;
				Vec3 v1 = *p; ++p;
				return Sphere(v0, v1, *p);
			}
			case 4: {
				auto p = _pointSet.begin();
				Vec3 v0 = *p; ++p;
				Vec3 v1 = *p; ++p;
				Vec3 v2 = *p; ++p;
				return Sphere(v0, v1, v2, *p);
			}
			}
		}


		auto p = _pointSet.begin();
		for( int i=0; i<_boundarySet; ++i ) ++p;
		for( int i=_boundarySet; i<_n; ++i )
		{
			// Make sure the iterator will go to the correct next one
			auto currentIt = p++;
			assert( currentIt != _pointSet.end() );
			Vec3 last = *currentIt;
			if( !MBS.Encloses(last) )
			{
				// The current point is not covered by the sphere add it to boundary
				// First resort (current point to the front)
				_pointSet.erase( currentIt );
				_pointSet.push_front( last );
				MBS = MinimalBoundingSphere(_pointSet, i+1, _boundarySet+1);
			}
		}

		return MBS;
	}

	Sphere MinimalBoundingSphere( std::list<Vec3>& _pointSet )
	{
		return MinimalBoundingSphere( _pointSet, _pointSet.size(), 0 );
	}
} // namespace Math