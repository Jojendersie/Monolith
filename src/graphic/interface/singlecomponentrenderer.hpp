#include "predeclarations.hpp"
#include "voxel/voxel.hpp"
#include "graphic/core/vertexbuffer.hpp"

namespace Graphic {

	/// \brief This helper allows to draw single component volume voxels
	///		directly without the need of chunks.
	class SingleComponentRenderer
	{
	public:
		/// The construction allocates a vertex buffer with one instance of
		/// each component. Therefor voxel data must be loaded previously.
		SingleComponentRenderer();

		/// \brief Draw a single voxel with manually set transformation.
		/// \details This draw call expects the effect Effects::VOXEL_RENDER to be set
		void Draw( Voxel::ComponentType _type, int _sideFlags, const ei::Mat4x4& _worldView, const ei::Mat4x4& _projection );
	private:
		Graphic::VertexBuffer m_voxels;

		// Prevent copy constructor and operator = being generated.
		SingleComponentRenderer(const SingleComponentRenderer&);
		const SingleComponentRenderer& operator = (const SingleComponentRenderer&);
	};

} // namespace Graphic