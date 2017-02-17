#include "galaxy.hpp"
#include "../graphic/content.hpp"
#include "generators/random.hpp"
#include "../graphic/core/device.hpp"
#include "../graphic/core/opengl.hpp"


using namespace ei;

Galaxy::Galaxy(int _stars, float _size, int _ambientStars) :
	m_starInfos(Graphic::VertexArrayBuffer::PrimitiveType::POINT, {{Graphic::VertexAttribute::VEC3, 0}, {Graphic::VertexAttribute::COLOR, 3}, {Graphic::VertexAttribute::FLOAT, 11}}),
	m_ambientStars(Graphic::VertexArrayBuffer::PrimitiveType::POINT, {{Graphic::VertexAttribute::VEC3, 0}, {Graphic::VertexAttribute::COLOR, 3}, {Graphic::VertexAttribute::FLOAT, 11}})
{
	//needs to be called to allow custom point size in the geometry shader
	GL_CALL(glEnable, GL_PROGRAM_POINT_SIZE);

	m_starSystems.reserve(_stars);

	//generator params
	//stars in the core blob / stars in the spiral arms
	float coreToRemoteRatio = 0.40f;
	//rotation of the arms in complete circles
	float rotationRate = 2.25f;

	float radius = _size / 2;
	float radiusSq = radius * radius;
	float height = radius / 32;
	float coreStars = coreToRemoteRatio * _stars;
	float factor = 0.99f / powf(_stars - coreStars, 2.f);

	Vec3 pos;

	Generators::Random rnd(_stars);
	for (int i = 0; i < _stars; ++i)
	{
		//center is a cloud
		if (i / (float)_stars < coreToRemoteRatio)
		{
			pos[0] = rnd.Normal(radius * 8);
			pos[1] = rnd.Normal(radius * 8);
			pos[2] = rnd.Normal(radius * 8);
		}
		else
		{
			//spiral arms start in the core
			float c = i - coreStars;
			//radius function
			//Dr = [coreStars,_stars - coreStars]
			//Wr = [0,1]
			// y = a(x-d)^2
			//float f = c / float(100 + c);
			//float r = c / float(_stars - coreStars);
			//non linear so that the density of stars decreases with growing radius
			float r = factor*pow(c, 2.f) + 0.01f;
		
			//noise decreases linear with radius
			float relative = c / (float)(_stars - coreStars);
			float f2 = (1.2f - relative) * radius * 2;
			//2 arms starting on the same axis (+-)
			pos[0] = (r*radius + rnd.Normal(f2)) *(i % 2 ? 1.f : -1.f);
			pos[2] = 0.f;
			//rotation is linear
			float a = relative * 2 * PI * rotationRate + rnd.Exponential(relative);
			pos = rotationY(a) * pos;

			pos[1] = rnd.Uniform(-height, height);
		}

		m_starSystems.emplace_back(
			Math::FixVec3(pos),
			rnd.Uniform(2000, 40000),
			rnd.Uniform(1.0f, 2.3f)
			);
	}

	auto vbGuard = m_ambientStars.GetBuffer(0);
	for (int i = 0; i < _ambientStars; ++i)
	{
		StarVertex star;
		star.color = StarSystem::TemperatureToRGB(rnd.Uniform(2000, 40000)).RGBA();
		star.color -= (rnd.Uniform(-32, 32) + 115) << 24; // background stars have less brightness
		star.position = rnd.Direction() * 10.0f;
		star.size = rnd.Uniform(1.0f, 2.0f);

		vbGuard->Add(star);
	}
}

void Galaxy::Draw(const Input::Camera& _camera)
{
	m_starInfos.Clear();

	{
		auto vbGuard = m_starInfos.GetBuffer(0);
		for (auto& starSystem : m_starSystems)
			vbGuard->Add(starSystem.ComputeVertex(_camera));
	}

	Graphic::Effect& effect = Graphic::Resources::GetEffect(Graphic::Effects::BACKGROUNDSTAR);
	Graphic::Device::SetEffect(effect);
	effect.SetUniform(0, true);
	Graphic::Device::DrawVertices(m_ambientStars, 0, m_ambientStars.GetNumVertices());
	effect.SetUniform(0, false);
	Graphic::Device::DrawVertices(m_starInfos, 0, m_starInfos.GetNumVertices());
}

