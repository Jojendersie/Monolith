#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <atomic>

namespace Utils {
	/// \brief A dynamic array wrapper (vector) for thread safe concurrent read and
	///		write accesses
	template<typename T>
	class ThreadSafeBuffer
	{
	public:
		class WriteGuard {
		public:
			WriteGuard() : parent(nullptr) {}

			/// \brief The destruction applies the writes by exchanging the
			///		double buffer.
			~WriteGuard()
			{
				// Write the new buffer back to the parent
				if(parent) parent->atomicStore(ptr);
			}

			WriteGuard(ThreadSafeBuffer* parent) :
				parent(parent),
				ptr(std::make_shared<std::vector<T>>(*parent->m_readPtr)) // Create a clone
			{
			}

			WriteGuard(WriteGuard&& _other)
			{
				ptr = std::move(_other.ptr);
				parent = _other.parent;
			}

			void operator = (WriteGuard&& _other)
			{
				if(parent) parent->atomicStore(ptr);

				ptr = std::move(_other.ptr);
				parent = _other.parent;
			}

			/// \brief Write back the double buffer and clear the write buffer.
			/// \details Internally the current write buffer is put into read
			///		position and a new empty one is created. Calling this method
			///		twice will lead to a clear of the write and the read buffer.
			///
			///		This method is intended for queues. All elements since the
			///		last call are made available and the older ones are deleted.
			/// \warning This will be done automatically from the destructor!
			void WriteNowAndClear()
			{
				if(parent)
				{
					// Replace the current write buffer with a new one
					auto writtenStuff = std::atomic_exchange(&ptr, std::make_shared<std::vector<T>>());
					// Store the written stuff in the parent read pointer.
					std::atomic_store(&parent->m_readPtr, writtenStuff);
					// TODO this still might fail if somebody is currently writing things!
					// Essentially to solve this whole buffer must be reimplemented with mutices.
				}
			}

			std::vector<T>& buf()	{ return *ptr; }
		private:
			std::shared_ptr<std::vector<T>> ptr;
			ThreadSafeBuffer* parent;
			WriteGuard(const WriteGuard&) = delete;
			void operator = (const WriteGuard&) = delete;
		};

		class ReadGuard {
		public:
			ReadGuard(const ThreadSafeBuffer* parent) :
				ptr(parent->m_readPtr) // Copy pointer only
			{
			}

			const T& operator [] (int _index) const { return (*ptr)[_index]; }
			int size() const { return (int)ptr->size(); }
			const std::vector<T>& buf() const { return *ptr; }
		private:
			std::shared_ptr<std::vector<T>> ptr;
		};

		ThreadSafeBuffer() : m_readPtr(std::make_shared<std::vector<T>>()) {}

		/// \brief Get a write able copy of the internal buffer.
		/// \details WriteAccess should be acquired from a single thread only.
		///		It is safe to write the buffer from different threads but this leads
		///		to severe locks.
		///		However getting the write access does not lock read accesses.
		void GetWriteAccess(WriteGuard& _out)
		{
			_out.~WriteGuard();
			m_lockWriteAccess.lock();
			new (&_out)WriteGuard(this);
		}

		/// \brief Get consistent access as long as the guard exists.
		ReadGuard GetReadAccess() const
		{
			return std::move(ReadGuard(this));
		}

	private:
		std::shared_ptr<std::vector<T>> m_readPtr;
		std::mutex m_lockWriteAccess;

		void atomicStore(std::shared_ptr<std::vector<T>> _ptr)
		{
			std::atomic_store(&m_readPtr, std::move(_ptr));
			m_lockWriteAccess.unlock();
		}
	};

}