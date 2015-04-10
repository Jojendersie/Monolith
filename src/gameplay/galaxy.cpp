#include "galaxy.hpp"
#include "../graphic/content.hpp"
#include "generators/random.hpp"
#include "../graphic/core/device.hpp"

using namespace Math;

Galaxy::Galaxy(int _stars, float _size)
	: m_starInfos ("3c1", Graphic::VertexBuffer::PrimitiveType::POINT)
{
	m_starSystems.reserve(_stars);

	float radius = _size / 2;
	float radiusSq = radius * radius;
	float height = radius / 10;

	Generators::Random rnd(_stars);
	for (int i = 0; i < _stars; ++i)
	{
		Vec3 pos;
		pos[1] = rnd.Normal(radius*2);
		pos[0] = rnd.Normal(radius*2);
		pos[2] = rnd.Normal(height);

		//alternative distribution
	/*	pos[0] = rnd.Uniform(-radius, radius);

		float max = sqrtf(radiusSq - pos[0] * pos[0]);
		pos[1] = rnd.Uniform(-max, max);
		
		float f = pos[0] * pos[0] + pos[1] * pos[1];
		f = 1 - f / radiusSq;
		pos[2] = rnd.Uniform(-f*height, f*height);*/

		m_starSystems.emplace_back(
			FixVec3(pos),
			rnd.Uniform(2000, 40000)
			);
	}
}

void Galaxy::Draw(const Input::Camera& _camera)
{
	m_starInfos.Clear();

	for (auto& starSystem : m_starSystems)
		m_starInfos.Add(starSystem.ComputeVertex(_camera));

	m_starInfos.SetDirty();

	Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::BACKGROUNDSTAR));
	Graphic::Device::DrawVertices(m_starInfos, 0, m_starInfos.GetNumVertices());
}

