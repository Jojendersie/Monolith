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
	m_xIntervalMax.push_back(handle);
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
	// Find first element which is a candidate for x
	auto startit = std::lower_bound(m_xIntervalMax.begin(), m_xIntervalMax.end(), box.min[0], [](const SOHandle& _i, const Fix& _ref){ return _i->GetBoundingBoxMax()[0] < _ref; });
	if( startit == m_xIntervalMax.end() ) return nullptr;
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
			if( it == m_xIntervalMax.end() )
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
	// TODO: Segment trees or similar. currently only x is restricted logarithmically.
	// Find first element which is a candidate for x
	auto it = std::lower_bound(m_xIntervalMax.begin(), m_xIntervalMax.end(), _box.min[0], [](const SOHandle& _i, const Fix& _ref){ return _i->GetBoundingBoxMax()[0] < _ref; });
	// Iterate as long as the element intersects in x
	while( it != m_xIntervalMax.end() && (*it)->m_minOfAllMin < _box.max[0] )
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
	// TODO: Frustum culling (currently this returns all objects)
	for(auto& it: m_xIntervalMax)
		_out.PushBack(it);
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
	m_numActiveObjects = (int)m_xIntervalMax.size();

	if(m_numActiveObjects > 1)
	{
		// Sort axis
		std::sort(m_xIntervalMax.begin(), m_xIntervalMax.end(), [](const SOHandle& _lhs, const SOHandle& _rhs){
			return _lhs->GetBoundingBoxMax()[0] < _rhs->GetBoundingBoxMax()[0];
		});

		// Update additional information for faster queries
		m_xIntervalMax[m_numActiveObjects-1]->m_minOfAllMin = m_xIntervalMax[m_numActiveObjects-1]->GetBoundingBoxMin()[0];
		for( int i = m_numActiveObjects-2; i >= 0; --i )
		{
			Fix minOfMin = min(m_xIntervalMax[i]->GetBoundingBoxMin()[0], m_xIntervalMax[i+1]->m_minOfAllMin);
			m_xIntervalMax[i]->m_minOfAllMin = minOfMin;
		}
	}
}
