#include "model.hpp"
#include "voxel.hpp"
#include <cstdlib>

using namespace Math;

namespace Voxel {

	Model::Model() :
		m_chunks(nullptr),
		m_numChunks(0),
		m_position(0.0f),
		m_mass(0.0f),
		m_center(0.0f),
		m_boundingSphereRadius(0.0f)
	{
	}

	Model::~Model()
	{
		for( int i=0; i<m_numChunks; ++i )
			delete m_chunks[i];
		free(m_chunks);
	}

	void Model::Draw( Graphic::UniformBuffer& _objectConstants, const Math::Matrix& _viewProjection )
	{
		// TODO: culling

		// Create a new model space transformation
		Math::Matrix mModelViewProjection = MatrixTranslation(-m_center) * MatrixRotation(m_rotation) * MatrixTranslation( m_position+m_center ) * _viewProjection;

		// Draw all chunks
		for( int i=0; i<m_numChunks; ++i )
		{
			m_chunks[i]->Draw( _objectConstants, mModelViewProjection );
		}
	}

	void Model::Set( const Math::IVec3& _position, int _level, VoxelType _type )
	{
		// Compute which chunk is searched. The chunk position is in units of
		// the smallest voxels but rounded to 32 voxel alignment.
		IVec3 chunkPos = _position / (1<<_level);
		IVec3 posInsideChunk = _position - chunkPos * (1<<_level);
		chunkPos *= 32;

		Chunk* targetChunk = nullptr;

		// Search the correct chunk
		for( int i=0; i<m_numChunks; ++i )
		{
			if( m_chunks[i]->GetPosition() == chunkPos )
			{
				// Add, update and ready
				targetChunk = m_chunks[i];
				break;
			}
		}

		// Nothing found create a new chunk
		if( !targetChunk )
		{
			m_chunks = (Chunk**)realloc(m_chunks, sizeof(Chunk*) * (m_numChunks+1));
			targetChunk = m_chunks[m_numChunks] = new Chunk();
			targetChunk->SetPosition( chunkPos );
			++m_numChunks;
		}

		targetChunk->Set(posInsideChunk, _level, _type);

		// Update mass center
		if( m_mass == 0.0f )
			m_center = Vec3(_position);
		else
			m_center = (m_center * m_mass + _position * VOXEL_INFO[int(_type)].mass) / (m_mass + VOXEL_INFO[int(_type)].mass);
		m_mass += VOXEL_INFO[int(_type)].mass;

		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume; only works if build in one direction
		m_boundingSphereRadius = Math::max(m_boundingSphereRadius, (m_center - _position).Length() );
	}

	bool CollisionTest(Math::Vec3& _collisionPoint, Math::Vec3& _collisionNormal, const Model _modelOne,const Model _modelTwo)
	{
		//TODO proper intersection test
		float minDist = _modelOne.m_boundingSphereRadius + _modelTwo.m_boundingSphereRadius;
		if ((_modelOne.m_center - _modelTwo.m_center).LengthSq() > (minDist*minDist))
			return false;
		Polyhedron collisionZone = Polyhedron(_modelOne.m_boundingBox, _modelTwo.m_boundingBox);
		if (collisionZone.IsEmpty())
			return false;
		_collisionPoint = collisionZone.GetCenter();
		_collisionNormal = _modelOne.m_center - _modelTwo.m_center;
		return true;
	}
	void CollisionCompute(Math::Vec3& _elasticVelocityOne, Math::Quaternion& _elasticRotaryOne, Math::Vec3& _elasticVelocityTwo, Math::Quaternion& _elasticRotaryTwo,
		Math::Vec3& _plasticVelocityOne, Math::Quaternion& _plasticRotaryOne, Math::Vec3& _plasticVelocityTwo, Math::Quaternion& _plasticRotaryTwo,
		const Model _modelOne, const Model _modelTwo, const Math::Vec3 _collisionPoint, const Math::Vec3 _collisionNormal)
	{
		//relative Positions of the collision Point
		Vec3 r1 = _collisionPoint - _modelOne.m_center;
		Vec3 r2 = _collisionPoint - _modelTwo.m_center;
		//TODO add cP=mC cases
		//orthogonal directions in which the rotational velocity can change
		Vec3 s1 = orth(r1);
		Vec3 s2 = orth(r2);
		Vec3 t1 = cross(r1, s1);
		Vec3 t2 = cross(r2, s2);
		//local representation of degrees of freedom with scalar values xi,yi,zi
		//dvi = xi*ri, dwi=yi*si+zi*ti

		//calculation of coefficients for the system of equations
		//Prerservation of Momentum 
		//a1*x1+b1*y1+c1*z1+a2*x2+b2*y2+c2*z2=0
		Vec3 a1 = _modelOne.m_mass*r1;
		Vec3 b1 = _modelOne.m_inertiaMomentum*s1;
		Vec3 c1 = _modelOne.m_inertiaMomentum*t1;
		Vec3 a2 = _modelTwo.m_mass*r2;
		Vec3 b2 = _modelTwo.m_inertiaMomentum*s2;
		Vec3 c2 = _modelTwo.m_inertiaMomentum*t2;
		
		//resulting forces can only work in colNormal direction
		//d1*x1+e1*y1+f1*z1=0
		//either one component of the coefficients is 0 or the 3 equations aren't linear independent
		Vec3 d1 = cross(a1, _collisionNormal);
		Vec3 e1 = cross(b1, _collisionNormal);
		Vec3 f1 = cross(c1, _collisionNormal);

	}

	// ********************************************************************* //
};