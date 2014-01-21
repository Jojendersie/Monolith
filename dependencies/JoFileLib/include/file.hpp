#pragma once

#include <cstdint>

namespace Jo {
namespace Files {

	/**************************************************************************//**
	 * \class	Jo::Files::IFile
	 * \brief	A file system abstraction layer.
	 * \details	All files derived from this one should support random access read
	 *			and write operations.
	 *****************************************************************************/
	class IFile
	{
	protected:
		uint64_t m_size;
		mutable uint64_t m_cursor;
		bool m_writeAccess;
		bool m_readAccess;

		IFile( uint64_t _size, bool _read, bool _write ) :
			m_size( _size ), m_cursor( 0 ), m_readAccess( _read ), m_writeAccess( _write )
		{}
	public:

		virtual ~IFile()	{}

		enum struct SeekMode {
			SET,
			MOVE_FORWARD,
			MOVE_BACKWARD
		};

		virtual void Read( uint64_t _numBytes, void* _to ) const = 0;

		/// \brief Read the next character from the file
		virtual uint8_t Next() const = 0;

		virtual void Write( const void* _from, uint64_t _numBytes ) = 0;
		virtual void Seek( uint64_t _numBytes, SeekMode _mode = SeekMode::SET ) const = 0;

		/// \brief Returns the cursor position within the file.
		/// \return A cursor position with large file support.
		uint64_t GetCursor() const		{ return m_cursor; }

		/// \brief Get the actual file size.
		/// \details A write operation must only update the size after succeeding.
		/// \return Returns the actual file size.
		uint64_t GetSize() const		{ return m_size; }

		/// \brief Was this file opened with write access?
		/// \return true if things can be written into this file.
		bool CanWrite() const			{ return m_writeAccess; }

		/// \brief Was this file opened with read access?
		/// \return true if file can be read.
		bool CanRead() const			{ return m_readAccess; }

		bool IsEof() const	{ return m_size == m_cursor; }
	};
};
};