#pragma once

#include "voxel/model.hpp"
#include "math/ray.hpp"
#include "math/box.hpp"
#include "sceneobject.hpp"


/// \brief A scene management for several queries.
/// \details This class provides the possibilities to return all objects which
///		intersect a frustum, a sphere or a ray.
class SceneGraph
{
public:
	SceneGraph();

	/// \returns A handle to find the object in the scene in constant time.
	SOHandle AddObject(ISceneObject* _object);

	/// \brief Find first object hit by a ray
	/// \param [out] _hit Descriptor where the returned model was hit
	/// \param [in] _maxRange The ray is clamped in a certain distance to
	///		improve performance and avoid extremely far objects to be selected.
	SOHandle RayQuery(const Math::WorldRay& _ray, Voxel::Model::ModelData::HitResult& _hit, float _maxRange = 1000.0f) const;

	/// \brief Find all objects whose bounding boxes intersect with the given box.
	/// \param [out] _out Empty container to be filled with the query results.
	void BoxQuery(const Math::WorldBox _box, Jo::HybridArray<SOHandle, 16>& _out) const;

	/// \brief Get all objects whose bounding boxes intersect a frustum.
	/// \param [in] _frustum TODO
	void FrustumQuery(Jo::HybridArray<SOHandle, 32>& _out) const;

	int NumActiveObjects() const { return m_numActiveObjects; }

	/// \brief Remove destroyed objects and insert the new ones.
	void UpdateGraph();
private:

	
	//std::vector<SOHandle> m_xIntervalMin;
	std::vector<SOHandle> m_xIntervalMax;
	/// \brief If a new object is added it is put to the interval list in the
	///		wrong place. It becomes active in the next frame
	int m_numActiveObjects;

	/// Delete old objects and their ref objects from all arrays
	/// \details The DRef state is inconsistent afterwards
	void RemoveDeadObjects();

	/// \details Repairs the DRef array (which does not need to be correct before call).
	void ResortAxis();
};