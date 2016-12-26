#include <cassert>
#include <cinttypes>
#include <type_traits>
#include <cstring>


template<typename K, typename T, typename Hash = std::hash<K>, typename Compare = std::equal_to<K>>
class HashMap
{
public:
	// Handles are direct accesses into a specific hashmap.
	// Any add or remove in the HM will invalidate the handle without notification.
	// A handle might be usable afterwards, but there is no guaranty.
	class Handle
	{
		HashMap* map;
		uint32_t idx;

		Handle(HashMap* _map, uint32_t _idx) :
			map(_map),
			idx(_idx)
		{}

		friend HashMap;
	public:


/*		T* operator -> ()
		{
			return data + idx;
		}

		const T* operator -> () const
		{
			return data + idx;
		}*/
		K& key()
		{
			return map->m_keys[idx].key;
		}

		const K& key() const
		{
			return map->m_keys[idx].key;
		}

		T& data()
		{
			return map->m_data[idx];
		}

		const T& data() const
		{
			return map->m_data[idx];
		}


		operator bool () const
		{
			return map != nullptr;
		}
	};

	explicit HashMap(uint32_t _expectedElementCount = 15) :
		m_capacity((uint32_t(_expectedElementCount * 1.3) | 1) + 2),
		m_size(0)
	{
		m_keys = static_cast<Key*>(malloc(sizeof(Key) * m_capacity));
		m_data = static_cast<T*>(malloc(sizeof(T) * m_capacity));
		
		for(uint32_t i = 0; i < m_capacity; ++i)
			m_keys[i].dist = 0xffffffff;
	}

	HashMap(HashMap&& _other) :
		m_capacity(_other.m_capacity),
		m_size(_other.m_size),
		m_keys(_other.m_keys),
		m_data(_other.m_data)
	{
		_other.m_keys = nullptr;
		_other.m_data = nullptr;
	}

	HashMap& operator = (HashMap&& _rhs)
	{
		this->~HashMap();
		m_capacity = _rhs.m_capacity;
		m_size = _rhs.m_size;
		m_keys = _rhs.m_keys;
		m_data = _rhs.m_data;
		_rhs.m_keys = nullptr;
		_rhs.m_data = nullptr;
		return *this;
	}

	~HashMap()
	{
		if(m_keys && m_data)
		{
			for(uint32_t i = 0; i < m_capacity; ++i)
				if(m_keys[i].dist != 0xffffffff)
				{
					m_data[i].~T();
					m_keys[i].key.~K();
				}
		}
		// TODO: call destructors of allocated keys and data
		free(m_keys);
		free(m_data);
	}

//	void add(const K& _key, const T& _data) { K k(_key); T t(_data); add(std::move(k), std::move(t)); }

	template<class _KeyT, class _DataT>
	void add(_KeyT&& _key, _DataT&& _data)
	{
		uint32_t h = (uint32_t)m_hash(_key);//hash(reinterpret_cast<const uint32_t*>(&_key), sizeof(_key) / 4);
	restartAdd:
		uint32_t d = 0;
		uint32_t idx = h % m_capacity;
		while(m_keys[idx].dist != 0xffffffff) // while not empty cell
		{
			if (m_keyCompare(m_keys[idx].key, _key)) // overwrite if keys are identically
			{
				m_data[idx] = _data;
				return;
			}
			// probing (collision)
			// Since we have encountered a collision: should we resize?
			if(m_size > 0.77 * m_capacity) {
				resize(m_size * 2); // Try to keep the capacity odd (prime would be even better)
				// The resize changed everything beginning from the index
				// to the content of the target cell. Restart the search.
				goto restartAdd;
			}

			if(m_keys[idx].dist < d) // Swap and then insert the element from this location instead
			{
				std::swap(_key, m_keys[idx].key);
				std::swap(d, m_keys[idx].dist);
				std::swap(_data, m_data[idx]);
			}
			++d;
		//	idx = (idx + 1) % m_capacity;
			if(++idx >= m_capacity) idx = 0;
		}
		new (&m_keys[idx].key)(K)(std::forward<_KeyT>(_key));
		m_keys[idx].dist = d;
		new (&m_data[idx])(T)(std::forward<_DataT>(_data));
		++m_size;
	}

	// Remove an element if it exists
	void remove(const K& _key)
	{
		remove(find(_key));
	}

	// Remove an existing element
	void remove(const Handle& _element)
	{
		if(_element)
		{
			m_keys[_element.idx].dist = 0xffffffff;
			m_data[_element.idx].~T();
			m_keys[_element.idx].key.~K();
			--m_size;
			uint32_t i = _element.idx;
			uint32_t next = (_element.idx + 1) % m_capacity;
			while((m_keys[next].dist != 0) && (m_keys[next].dist != 0xffffffff))
			{
				std::swap(m_keys[i], m_keys[next]);
				--m_keys[i].dist;
				m_data[i] = std::move(m_data[next]);
				i = next;
				if(++next >= m_capacity) next = 0;
			}
		}
	}

	Handle find(const K& _key)
	{
		uint32_t d = 0;
		uint32_t h = (uint32_t)m_hash(_key);//hash(reinterpret_cast<const uint32_t*>(&_key), sizeof(_key) / 4);
		uint32_t idx = h % m_capacity;
		while(m_keys[idx].dist != 0xffffffff)// && d <= m_keys[idx].dist)
		{
			if (m_keyCompare(m_keys[idx].key, _key))
				return Handle(this, idx);
			if(++idx >= m_capacity) idx = 0;
			++d;
		}
		return Handle(nullptr, 0);
	}

	// Change the capacity if possible. It cannot be decreased below 'size'.
	void resize(uint32_t _newCapacity)
	{
		//if(_newCapacity == m_capacity) return;
		if(_newCapacity < m_size) _newCapacity = m_size;

		HashMap<K,T> tmp(_newCapacity);
		// Find all data sets and readd them to the new temporary hm
		for(uint32_t i = 0; i < m_capacity; ++i)
		{
			if(m_keys[i].dist != 0xffffffff)
			{
				tmp.add(std::move(m_keys[i].key), std::move(m_data[i]));
				m_keys[i].dist = 0xffffffff;
			}
		}

		// Use the temporary map now and let the old memory be destroyed.
		std::swap(*this, tmp);
	}

	void reserve(uint32_t _exptectedElementCount)
	{
		resize((uint32_t(_exptectedElementCount * 1.3) | 1) + 2);
	}

	uint32_t size() const { return m_size; }

	// returns the first element found in the map or an invalid handle when the map is empty
	Handle first()
	{
		for (uint32_t i = 0; i < m_capacity; ++i)
			if (m_keys[i].dist != 0xffffffff)
				return Handle(this, i);

		return Handle(nullptr, 0);
	}
private:
	uint32_t m_capacity;
	uint32_t m_size;

	struct Key
	{
		K key;
		unsigned dist; // robin hood cashing offset
	};

	Key* m_keys;
	T* m_data;
	Hash m_hash;
	Compare m_keyCompare;

	uint32_t hash(const uint32_t* _key, unsigned _numWords)
	{
		// TODO: general purpose hash function
		return *_key;
	}
};