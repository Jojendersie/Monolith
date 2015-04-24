#include "scenegraph.hpp"
#include "math/box.hpp"
#include <algorithm>

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

	ManageObjects(xListAccess);

	// Update objects them self (bounding volumes...)
	for( int i = 0; i < xListAccess.buf().size(); i++ )
	{
		xListAccess.buf()[i]->Update();
	}

	ResortAxis(xListAccess);
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