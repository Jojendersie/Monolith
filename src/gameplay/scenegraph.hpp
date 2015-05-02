#pragma once

#include "voxel/model.hpp"
#include "math/ray.hpp"
#include "math/box.hpp"
#include "sceneobject.hpp"
#include "utilities/threadsafebuffer.hpp"


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
	SOHandle RayQuery(const Math::WorldRay& _ray, Voxel::Model::ModelData::HitResult& _hit, float _maxRange = 5000.0f) const;

	/// \brief Find all objects whose bounding boxes intersect with the given box.
	/// \param [out] _out Empty container to be filled with the query results.
	void BoxQuery(const Math::WorldBox _box, Jo::HybridArray<SOHandle, 16>& _out) const;

	/// \brief Get all objects whose bounding boxes intersect a frustum.
	/// \param [in] _frustum TODO
	void FrustumQuery(Jo::HybridArray<SOHandle, 32>& _out) const;

	int NumActiveObjects() const { return m_xIntervalMax.GetReadAccess().size(); }

	/// \brief Remove destroyed objects and insert the new ones.
	void UpdateGraph();

	/// \brief Simulate physics and AI and ships...
	void Simulate(float _deltaTime);
private:
	std::vector<SOHandle> m_newObjects;	///< Added since last update
	Utils::ThreadSafeBuffer<SOHandle> m_xIntervalMax;

	/// \details Repairs the DRef array (which does not need to be correct before call).
	///		Also add all new objects.
	void ResortAxis(Utils::ThreadSafeBuffer<SOHandle>::WriteGuard& _xListAccess);

	/// \brief Add and remove objects from the queue
	void ManageObjects(Utils::ThreadSafeBuffer<SOHandle>::WriteGuard& _xListAccess);

	//SOHandle RayQueryCandidate(const SOHandle& _obj, Voxel::Model::ModelData::HitResult& _hit, float& _maxRange) const;
};