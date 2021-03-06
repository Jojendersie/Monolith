#include "scenegraph.hpp"
#include "math/box.hpp"
#include "voxel/sparseoctree.hpp"
#include <algorithm>

using namespace ei;
using namespace Math;

// ************************************************************************* //
SceneGraph::SceneGraph() :
	m_xIntervalMax()
{
}

// ************************************************************************* //
SOHandle SceneGraph::AddObject(ISceneObject* _object)
{
	SOHandle handle(_object);
	// Add bounding intervals to the lists
	m_newObjects.push_back(handle);
	return std::move(handle);
}

// ************************************************************************* //
SOHandle SceneGraph::RayQuery(const Math::WorldRay& _ray, Voxel::Model::ModelData::HitResult& _hit, float _maxRange) const
{
	// The algorithm assumes always increasing x coordinates. Turn ray if necessary.
	FixVec3 origin;
	Vec3 direction;
	if( _ray.direction[0] < 0.0f ) {origin = _ray.origin + FixVec3(_maxRange * _ray.direction); direction = -_ray.direction;}
	else {origin = _ray.origin; direction = _ray.direction;}
	// Test iteratively against bounding boxes build from split ray segment ->
	// each logarithmic split reduces the checked volume by 4x.
	int numSplits = max(1, int(_maxRange) / 100); // Split into boxes of length CONST
	WorldBox box;
	FixVec3 offset = FixVec3((_maxRange / numSplits) * direction);
	FixVec3 rayEnd = origin + offset;
	box.min = min(origin, rayEnd);
	box.max = max(origin, rayEnd);

	// Check all the colliders inside box and return closest intersection
	SOHandle closestHit;
	auto xReadAccess = m_xIntervalMax.GetReadAccess();	// Copy shared pointer to assert that the buffer does not change during algorithm.
	// Find first element which is a candidate for x
	auto startit = std::lower_bound(xReadAccess.buf().begin(), xReadAccess.buf().end(), box.min[0], [](const SOHandle& _i, const Fix& _ref){ return _i->GetBoundingBoxMax()[0] < _ref; });
	if( startit == xReadAccess.buf().end() ) return nullptr;
	for(int step = 0; step < numSplits; ++step)
	{
		auto it = startit;
	//	LOG_ERROR(std::to_string((double)box.max[0]));
		// Iterate as long as the element intersects in x
		while( (*it)->m_minOfAllMin <= box.max[0] )
		{
			// The current element intersects in x direction. Does it also intersect
			// in the others?
			if( (*it)->GetBoundingBoxMin()[1] < box.max[1] && (*it)->GetBoundingBoxMax()[1] > box.min[1] &&
				(*it)->GetBoundingBoxMin()[2] < box.max[2] && (*it)->GetBoundingBoxMax()[2] > box.min[2] )
			{
				const Voxel::Model* model = dynamic_cast<const Voxel::Model*>(&(*it));
				if(model)
				{
					Voxel::Model::ModelData::HitResult hit;
					if( model->RayCast(_ray, 0, hit, _maxRange) )
					{
						closestHit = *it;
						_hit = hit;
					}
				}
			}
			++it;
			if( it == xReadAccess.buf().end() )
				return closestHit;
			if( (*startit)->GetBoundingBoxMax()[0] <= box.max[0] )
				++startit;
		}
		// Ray march to a next box segment
		box.min = rayEnd;
		rayEnd = rayEnd + offset;
		box.max = max(box.min, rayEnd);
		box.min = min(box.min, rayEnd);
	}

	return closestHit;
}

/*SOHandle SceneGraph::RayQueryCandidate(const SOHandle& _obj, Voxel::Model::ModelData::HitResult& _hit, float& _maxRange) const
{
}*/

// ************************************************************************* //
void SceneGraph::BoxQuery(const Math::WorldBox _box, Jo::HybridArray<SOHandle, 16>& _out) const
{
	auto xReadAccess = m_xIntervalMax.GetReadAccess();	// Copy shared pointer to assert that the buffer does not change during algorithm.
	// TODO: Segment trees or similar. currently only x is restricted logarithmically.
	// Find first element which is a candidate for x
	auto it = std::lower_bound(xReadAccess.buf().begin(), xReadAccess.buf().end(), _box.min[0], [](const SOHandle& _i, const Fix& _ref){ return _i->GetBoundingBoxMax()[0] < _ref; });
	// Iterate as long as the element intersects in x
	while( it != xReadAccess.buf().end() && (*it)->m_minOfAllMin < _box.max[0] )
	{
		// The current element intersects in x direction. Does it also intersect
		// in the others?
		if( (*it)->GetBoundingBoxMin()[1] < _box.max[1] && (*it)->GetBoundingBoxMax()[1] > _box.min[1] &&
			(*it)->GetBoundingBoxMin()[2] < _box.max[2] && (*it)->GetBoundingBoxMax()[2] > _box.min[2] )
			_out.PushBack(*it);
		++it;
	}
}

// ************************************************************************* //
void SceneGraph::FrustumQuery(Jo::HybridArray<SOHandle, 32>& _out) const
{
	auto xReadAccess = m_xIntervalMax.GetReadAccess();	// Copy shared pointer to assert that the buffer does not change during algorithm.
	// TODO: Frustum culling (currently this returns all objects)
	for(auto& it: xReadAccess.buf())
		_out.PushBack(it);
}

// ************************************************************************* //
void SceneGraph::UpdateGraph()
{
	// We want to add objects and reorder the buffer
	Utils::ThreadSafeBuffer<SOHandle>::WriteGuard xListAccess;
	m_xIntervalMax.GetWriteAccess(xListAccess);

	for (int i = 0; i < xListAccess.buf().size(); i++)
	{
		auto models = static_cast<Voxel::Model*>(&xListAccess.buf()[i])->UpdateCohesion();
		for (auto& model : models)
			AddObject(model);
	}

	ManageObjects(xListAccess);

	// Update objects them self (bounding volumes...)
	for( int i = 0; i < xListAccess.buf().size(); i++ )
	{
		xListAccess.buf()[i]->UpdateBoundingBox();
	}

	ResortAxis(xListAccess);
}

// ************************************************************************* //
void SceneGraph::Simulate(float _deltaTime)
{
	auto xReadAccess = m_xIntervalMax.GetReadAccess();	// Copy shared pointer to assert that the buffer does not change during algorithm.

	CollisionCheck collisionCheck;
	//check every possible pair for collision
	for (int i = 0; i < xReadAccess.size(); ++i)
		for (int j = i + 1; j < xReadAccess.size(); ++j)
			collisionCheck.Run(*(static_cast<Voxel::Model*>(&xReadAccess[i])), *(static_cast<Voxel::Model*>(&xReadAccess[j])));

	for (int i = 0; i < xReadAccess.size(); ++i){
		xReadAccess[i]->Simulate(_deltaTime);
	}
}

// ************************************************************************* //
void SceneGraph::ResortAxis(Utils::ThreadSafeBuffer<SOHandle>::WriteGuard& _xListAccess)
{
	// Resort if not empty
	if(_xListAccess.buf().size() > 1)
	{
		// Sort axis
		std::sort(_xListAccess.buf().begin(), _xListAccess.buf().end(), [](const SOHandle& _lhs, const SOHandle& _rhs){
			return _lhs->GetBoundingBoxMax()[0] < _rhs->GetBoundingBoxMax()[0];
		});

		// Update additional information for faster queries
		int n = (int)_xListAccess.buf().size();
		_xListAccess.buf()[n-1]->m_minOfAllMin = _xListAccess.buf()[n-1]->GetBoundingBoxMin()[0];
		for( int i = n-2; i >= 0; --i )
		{
			Fix minOfMin = min(_xListAccess.buf()[i]->GetBoundingBoxMin()[0], _xListAccess.buf()[i+1]->m_minOfAllMin);
			_xListAccess.buf()[i]->m_minOfAllMin = minOfMin;
		}
	}
}

// ************************************************************************* //
void SceneGraph::ManageObjects(Utils::ThreadSafeBuffer<SOHandle>::WriteGuard& _xListAccess)
{
	// Delete the old ones
	int n = NumActiveObjects();
	for( int i = 0; i < n; i++ )
	{
		if(_xListAccess.buf()[i]->IsDeleted()) {
			_xListAccess.buf()[i] = std::move(_xListAccess.buf()[--n]);
			_xListAccess.buf().pop_back();
		}
	}

	// Insert all new objects from the queue
	int nnew = (int)m_newObjects.size();
	if(nnew) {
		for( int i = 0; i < nnew; ++i )
			_xListAccess.buf().push_back( m_newObjects[i] );
		m_newObjects.clear();
	}
}

// ************************************************************************* //
const float IMPULSE_DAMAGE = 0.2f; //totally arbitrary factor

void SceneGraph::CollisionCheck::Run(Voxel::Model& _model0, Voxel::Model& _model1)
{
	//use already computed bounding box for the first try
	float r0 = _model0.GetRadius();
	r0 *= r0;

	float r1 = _model1.GetRadius();
	r1 *= r1;

	m_posSlf = Vec3(0.f);
	//manual translation to a local space
	//because transform rotates as well
	FixVec3 fixPos0 = _model0.GetPosition();
	FixVec3 fixPos1 = _model1.GetPosition(); 
	m_posOth = Vec3(float(fixPos1[0] - fixPos0[0]),
		float(fixPos1[1] - fixPos0[1]),
		float(fixPos1[2] - fixPos0[2])); 

	float dist = lensq(m_posOth - m_posSlf);

	// a previous collision resolve may have destroyed one of the models
	// todo: remove this with introduction of a general model physics update step
	if (dist < r0 + r1 && _model0.GetNumVoxels() && _model1.GetNumVoxels())
	{
		const Voxel::Model::ModelData& tree0 = _model0.GetVoxelTree();
		const Voxel::Model::ModelData& tree1 = _model1.GetVoxelTree();
		const Voxel::Model::ModelData::SVON* node0 = tree0.Get(tree0.GetRootPosition(), tree0.GetRootSize());
		const Voxel::Model::ModelData::SVON* node1 = tree1.Get(tree1.GetRootPosition(), tree1.GetRootSize());

		IVec4 pos0 = IVec4(tree0.GetRootPosition(), 0);
		pos0[3] = tree0.GetRootSize();
		IVec4 pos1 = IVec4(tree1.GetRootPosition(), 0);
		pos1[3] = tree1.GetRootSize();

		m_rotSlf = &_model0.GetRotationMatrix();
		m_rotOth = &_model1.GetRotationMatrix();

		m_posSlf -= *m_rotSlf * _model0.GetCenter();
		m_posOth -= *m_rotOth * _model1.GetCenter();

		//make sure that the bigger one is slf
		if (pos0[3] < pos1[3])
		{
			m_modelSlf = &_model1;
			m_modelOth = &_model0;

			std::swap(m_posSlf, m_posOth);

			std::swap(m_rotSlf, m_rotOth);

			TreeCollision(pos1, *node1, pos0, *node0);
		}
		else
		{
			m_modelSlf = &_model0;
			m_modelOth = &_model1;

			TreeCollision(pos0, *node0, pos1, *node1);
		}

		if (!m_hits.size()) return;

		//resolve hits

		float massSlf = m_modelSlf->GetMass();
		float massOth = m_modelOth->GetMass();

		//calculate an average position of all hits
		Vec3 hitLocSlf(0.f);
		Vec3 hitLocOth(0.f);
		for (auto& hit : m_hits)
		{
			hitLocSlf += hit.posSlf;
			hitLocOth += hit.posOth;
		}
		hitLocSlf /= (float)m_hits.size();
		hitLocOth /= (float)m_hits.size();

		//no more tree calcs take place
		//actual center of mass positions are required
		m_posSlf += *m_rotSlf * m_modelSlf->GetCenter();
		m_posOth += *m_rotOth * m_modelOth->GetCenter();

		Vec3 point = hitLocSlf + 0.5f*(hitLocOth - hitLocSlf);
		Vec3 radiusSlf = hitLocSlf - m_posSlf; //point
		Vec3 radiusOth = hitLocOth - m_posOth;

		Vec3 velocitySlf = m_modelSlf->GetVelocity() + cross(m_modelSlf->GetAngularVelocity(), radiusSlf);
		Vec3 velocityOth = m_modelOth->GetVelocity() + cross(m_modelOth->GetAngularVelocity(), radiusOth);

		Vec3 normal = normalize(hitLocSlf - hitLocOth);

		//check that they are really closing and not just intersecting from a previous crash
		if (dot((velocitySlf - velocityOth), normal) >= 0) return;

		float epsilon = 0.04f;

		float impulse = -(1 + epsilon) * dot((velocitySlf - velocityOth), normal);
		impulse /= (1 / massSlf + 1 / massOth) + dot(normal, cross(m_modelSlf->GetInertiaTensorInverse()* cross(radiusSlf, normal), radiusSlf)
			+ cross(m_modelOth->GetInertiaTensorInverse()* cross(radiusOth, normal), radiusOth));
		if (impulse  != impulse) _CrtDbgBreak();

		m_modelSlf->AddVelocity(impulse / massSlf * normal);
		m_modelOth->AddVelocity(-impulse / massOth * normal);

		m_modelSlf->AddAngularVelocity(m_modelSlf->GetInertiaTensorInverse() * cross(radiusSlf, normal) * 1.f * impulse);
		m_modelOth->AddAngularVelocity(m_modelOth->GetInertiaTensorInverse() * cross(radiusOth, normal) *-1.f * impulse);

		//trigger collision events
		m_modelSlf->EvtCollision(*m_modelOth);
		m_modelOth->EvtCollision(*m_modelSlf);

		// energy transfered in j?
		// this should be converted to damage (/500) 
		// and reduced further to account for that most energy is converted to velocity
		float fac = 0.5f * impulse * impulse * IMPULSE_DAMAGE;
		float damageSlf = fac / ((float)m_hits.size() * massOth);
		float damageOth = fac / ((float)m_hits.size() * massSlf);
		for (auto& hit : m_hits)
		{
			//	m_modelSlf->Get(IVec3(hit.gridPosSlf));
			m_modelSlf->Damage(hit.gridPosSlf, damageSlf);
			m_modelOth->Damage(hit.gridPosOth, damageOth);
	//		m_modelSlf->Set(IVec3(hit.gridPosSlf), Voxel::ComponentType::UNDEFINED);
		}
	
	}
}

// ************************************************************************* //
void SceneGraph::CollisionCheck::TreeCollision(const IVec4& _position0, const Voxel::Model::ModelData::SVON& _node0, const IVec4& _position1, const Voxel::Model::ModelData::SVON& _node1)
{
	int sizeSlf = 1 << _position0[3];
	int sizeOth = 1 << _position1[3];
	Vec3 posSlf = Vec3(IVec3(_position0) * sizeSlf);
	posSlf += sizeSlf*0.5f;
	posSlf = *m_rotSlf * posSlf + m_posSlf;
	Vec3 posOth = Vec3(IVec3(_position1) * sizeOth);
	posOth += sizeOth*0.5f;
	posOth = *m_rotOth * posOth + m_posOth;

	int sizeSq = sizeSlf + sizeOth;
	sizeSq *= sizeSq;
	float lenSq = lensq(posOth - posSlf);
	//diameter of the outer sphere
	// (sqrt(3)/2)^2
	if (0.7499993 * sizeSq > lenSq)
	{
		//break condition
		if (_position0[3] == 0)
		{
			//check with a smaller tolerance
			//(sqrt(2)/2)^2
			if (0.25 * sizeSq > lenSq)
			{
				m_hits.emplace_back(IVec3(_position0), IVec3(_position1), posSlf, posOth);
			}
		}
		//recursive call with higher resolution
		else
		{
			IVec4 position0(_position0[0] << 1, _position0[1] << 1, _position0[2] << 1, _position0[3]);
			
			//when they are not on the same resolution level decrease only the bigger one
			if (_position0[3] > _position1[3])
			{
				for (int i = 0; i < 8; ++i)
				{
					const Voxel::Model::ModelData::SVON* child0 = _node0.GetChild(i);
					if (child0) TreeCollision(position0 + Voxel::CHILD_OFFSETS[i], *child0, _position1, _node1);
				}
			}
			else
			{
				IVec4 position1(_position1[0] << 1, _position1[1] << 1, _position1[2] << 1, _position1[3]);

				for (int i = 0; i < 8; ++i)
				{
					const Voxel::Model::ModelData::SVON* child0 = _node0.GetChild(i);
					if (child0)
						for (int j = 0; j < 8; ++j)
						{
							const Voxel::Model::ModelData::SVON* child1 = _node1.GetChild(j);
							if (child1)
								TreeCollision(position0 + Voxel::CHILD_OFFSETS[i], *child0, position1 + Voxel::CHILD_OFFSETS[j], *child1);
						}
				}
			}
		}
	}
}