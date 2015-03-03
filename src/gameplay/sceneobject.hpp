#pragma once

#include "utilities/assert.hpp"
#include "math/box.hpp"

/// \brief All objects which should be handled by scene management must inherit
///		this interface.
class ISceneObject
{
public:
	ISceneObject() : m_referenceCounter(0), m_deleteRequest(false) {}
	virtual ~ISceneObject() {Assert(m_referenceCounter == 0, "Wrong reference counting occurred!");}

	/// \brief Remove the object from game
	void Delete()		{ m_deleteRequest = true; }

	/// \brief For anybody how keeps an reference to the object: check this flag.
	///		If true forget the reference and do not handle the object anymore.
	bool IsDeleted() const	{ m_deleteRequest; }

	const Math::FixVec3& GetBoundingBoxMin() const { return m_boundingBox.min; }
	const Math::FixVec3& GetBoundingBoxMax() const { return m_boundingBox.max; }

protected:
	Math::WorldBox m_boundingBox;

private:
	int m_referenceCounter;		///< Memory management of the scene
	bool m_deleteRequest;
	Math::Fix m_minOfAllMin;	// Helper information for axis separating interval queries
	friend class SOHandle;
	friend class SceneGraph;
};

/// \brief Specialized pointer type (shared pointer) for scene objects
class SOHandle
{
public:
	SOHandle() : m_ptr(nullptr) {}
	explicit SOHandle(ISceneObject* _object) : m_ptr(_object) { m_ptr->m_referenceCounter = 1; }
	SOHandle(const SOHandle& _rhs) : m_ptr(_rhs.m_ptr) { if(m_ptr) ++m_ptr->m_referenceCounter; }
	SOHandle(SOHandle&& _rhs) : m_ptr(_rhs.m_ptr) { _rhs.m_ptr = nullptr; }

	~SOHandle() { if(m_ptr) if(--m_ptr->m_referenceCounter <= 0) delete m_ptr; m_ptr = nullptr; }

	void operator = (const SOHandle& _rhs) { this->~SOHandle(); m_ptr = _rhs.m_ptr; ++m_ptr->m_referenceCounter; }
	void operator = (SOHandle&& _rhs) { this->~SOHandle(); m_ptr = _rhs.m_ptr; _rhs.m_ptr = nullptr; }

	ISceneObject* operator -> () { return m_ptr; }
	const ISceneObject* operator -> () const { return m_ptr; }
	ISceneObject& operator * () { return *m_ptr; }
	const ISceneObject& operator * () const { return *m_ptr; }
	ISceneObject* operator & () { return m_ptr; }
	const ISceneObject* operator & () const { return m_ptr; }

	// Implicit cast to bool for if(ptr) statements
	operator bool () const { return m_ptr != nullptr; }
	// Implicit cast from nullptr
	SOHandle(std::nullptr_t t) : m_ptr(nullptr) {}
private:
	ISceneObject* m_ptr;
};