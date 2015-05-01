#pragma once

/// \brief Delete a resource end of scope.
/// \details In contrast to a unique_ptr you can duplicate the pointer...
///		but be sure you do not reference the resource after scope is left.
template<typename T>
class ScopedPtr
{
public:
	/// \brief Clean defaul ctor
	ScopedPtr() : m_ptr(nullptr)	{}

	/// \brief Construction from typed pointer (implicit too).
	ScopedPtr(T* _ptr) : m_ptr(_ptr) {}

	/// \brief Delete the resource!
	~ScopedPtr() { delete m_ptr; }

	/// \brief This swap function can only be found by ADL.
	friend void swap(ScopedPtr& _lhs, ScopedPtr& _rhs)
	{
		T* tmp = _lhs.m_ptr;
		_lhs.m_ptr = _rhs.m_ptr;
		_rhs.m_ptr = tmp;
	}

	/// \brief Assigning will delete the old object
	T* operator = (T* _rhs)
	{
		delete m_ptr;
		m_ptr = _rhs;
		return _rhs;
	}

	/// \brief Moving allowed
	ScopedPtr(ScopedPtr&& _rhs) : m_ptr(nullptr) {
		m_ptr = _rhs.m_ptr;
		_rhs.m_ptr = nullptr;
	}
	ScopedPtr& operator = (ScopedPtr&& _rhs) {
		swap(*this, _rhs);
		return *this;
	}

	/// \brief Auto cast
	operator T* () { return m_ptr; }
	operator const T* () const { return m_ptr; }

	T* operator -> () { return m_ptr; }
	T& operator * () { return *m_ptr; }

	operator bool () { return m_ptr != nullptr; }

	T& operator [] (unsigned _index) { return m_ptr[_index]; }
	const T& operator [] (unsigned _index) const { return m_ptr[_index]; }
private:
	T* m_ptr;

	// More than one deleting reference not allowed
	ScopedPtr(const ScopedPtr&);
	void operator = (const ScopedPtr&);
};