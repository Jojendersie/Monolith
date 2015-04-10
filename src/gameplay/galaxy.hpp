#pragma once

#include <vector>
#include "../graphic/core/device.hpp"
#include "../graphic/core/vertexbuffer.hpp"
#include "../graphic/core/effect.hpp"
#include "predeclarations.hpp"
#include "starsystem.hpp"

/// \brief A cluster of starsystems.
class Galaxy
{
public:
	/// \brief Generates a galaxy at (0,0,0).
	/// \param [in] _stars the amount of stars in the galaxy
	/// \param [in] _size the approximate diameter
	Galaxy(int _stars, float _size);

	/// \brief Draws all background stars of this galaxy.
	void Draw(const Input::Camera& _camera);

private:
	std::vector< StarSystem > m_starSystems;

	Graphic::VertexBuffer m_starInfos;
};

