#include "scenegraph.hpp"
#include "math/box.hpp"
#include <algorithm>

using namespace Math;

SceneGraph::SceneGraph() :
	m_numActiveObjects(0)
{
}

// ************************************************************************* //
SOHandle SceneGraph::AddObject(ISceneObject* _object)
{
	SOHandle handle(_object);
	// Add bounding intervals to the lists
	m_xIntervals.push_back(IntervalEntry(handle, _object->GetBoundingBoxMin()[0], _object->GetBoundingBoxMax()[0]));
//	m_yIntervals.push_back(IntervalEntry(handle, _object->GetBoundingBoxMin()[1], _object->GetBoundingBoxMax()[1]));
//	m_zIntervals.push_back(IntervalEntry(handle, _object->GetBoundingBoxMin()[2], _object->GetBoundingBoxMax()[2]));
	handle->arrayPos[0] = &m_xIntervals.back();
//	handle->arrayPos[1] = &m_yIntervals.back();
//	handle->arrayPos[2] = &m_zIntervals.back();
	return std::move(handle);
}

// ************************************************************************* //
SOHandle SceneGraph::RayQuery(const Math::WorldRay& _ray, Voxel::Model::ModelData::HitResult& _hit, float _maxRange) const
{
	// Test recursively against bounding boxes build from a ray split objects ->
	// each split reduces the checked volume by 4x.
	// Thereby reject all objects which are no models.
	WorldBox box;
	FixVec3 rayEnd = _ray.origin + FixVec3(_maxRange * _ray.direction);
	box.min = min(_ray.origin, rayEnd);
	box.max = max(_ray.origin, rayEnd);
	Jo::HybridArray<SOHandle, 16> buffer;
	BoxQuery(box, buffer);
	// Remove non-models from query
	for( unsigned i = 0; i < buffer.Size(); ++i )
		if(!dynamic_cast<Voxel::Model*>(&buffer[i]))
			buffer.Delete(i--);

	// If there are too many potential colliders split the query
	if(buffer.Size() > 8) // TODO find best threshold for performance
	{
		SOHandle hit = RayQuery(_ray, _hit, _maxRange / 2);
		if(hit) return hit;
		WorldRay ray;
		ray.origin = _ray.origin + Fix(_maxRange / 2);
		ray.direction = _ray.direction;
		hit = RayQuery(ray, _hit, _maxRange / 2);
		return hit;
	} else {
		// Check all the colliders linearly and return closest intersection
		SOHandle closestHit;
		for( unsigned i = 0; i < buffer.Size(); ++i )
		{
			Voxel::Model* model = dynamic_cast<Voxel::Model*>(&buffer[i]);
			Voxel::Model::ModelData::HitResult hit;
			if( model->RayCast(_ray, 0, hit, _maxRange) )
			{
				closestHit = std::move(buffer[i]);
				_hit = hit;
			}
		}
		return closestHit;
	}
	return nullptr;
}

// ************************************************************************* //
void SceneGraph::BoxQuery(const Math::WorldBox _box, Jo::HybridArray<SOHandle, 16>& _out) const
{
	// TODO: Segment trees or similar. currently only x is restricted logarithmically.
	// Find first element which is a candidate for x
	auto it = std::lower_bound(m_xIntervals.begin(), m_xIntervals.end(), _box.min[0], [](const IntervalEntry& _i, const Fix& _ref){ return _i.bbmax < _ref; });
	// Iterate as long as the element intersects in x
	while( it != m_xIntervals.end() )	// VERY INEFFICIENT!
	{
		// The current element intersects in x direction. Does it also intersect
		// in the others?
		if( it->bbmin < _box.max[0] &&
			it->ref->GetBoundingBoxMin()[1] < _box.max[1] && it->ref->GetBoundingBoxMax()[1] > _box.min[1] &&
			it->ref->GetBoundingBoxMin()[2] < _box.max[2] && it->ref->GetBoundingBoxMax()[2] > _box.min[2] )
			_out.PushBack(it->ref);
		++it;
	}

	//for(auto it: m_xIntervals)
	//	_out.PushBack(it.ref);
}

// ************************************************************************* //
void SceneGraph::FrustumQuery(Jo::HybridArray<SOHandle, 32>& _out) const
{
	// TODO: Frustum culling (currently this returns all objects)
	for(auto it: m_xIntervals)
		_out.PushBack(it.ref);
}

// ************************************************************************* //
void SceneGraph::UpdateGraph()
{
	RemoveDeadObjects();
	ResortAxis();
}

// ************************************************************************* //
void SceneGraph::RemoveDeadObjects()
{
}

// ************************************************************************* //
void SceneGraph::ResortAxis()
{
	m_numActiveObjects = (int)m_xIntervals.size();

	// Sort axis
	std::sort(m_xIntervals.begin(), m_xIntervals.end(), [](const IntervalEntry& _lhs, const IntervalEntry& _rhs){
		return _lhs.bbmax < _rhs.bbmax;
	});
	/*std::sort(m_yIntervals.begin(), m_yIntervals.end(), [](const IntervalEntry& _lhs, const IntervalEntry& _rhs){
		return _lhs.bbmin < _rhs.bbmin;
	});
	std::sort(m_zIntervals.begin(), m_zIntervals.end(), [](const IntervalEntry& _lhs, const IntervalEntry& _rhs){
		return _lhs.bbmin < _rhs.bbmin;
	});*/

	// Update wrong references now
	for(auto& it: m_xIntervals) it.ref->arrayPos[0] = &it;
	//for(auto& it: m_yIntervals) it.ref->arrayPos[1] = &it;
	//for(auto& it: m_zIntervals) it.ref->arrayPos[2] = &it;
}
