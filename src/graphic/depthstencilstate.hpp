#pragma once

namespace Graphic {

	/// \brief State object for z and stencil buffer options.
	class DepthStencilState
	{
	public:

		/// \brief Create the depth-stencil state object.
		DepthStencilState();

		/// \brief Unique number for each different state combination.
		/// \details Two different state objects with the same setup have the
		///		same hash.
		//int GetHash() const		{ return int(m_CullMode) + int(m_FillMode) * 10; }

	private:
	};
};