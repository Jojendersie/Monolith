#include "galaxy.hpp"
#include "../graphic/content.hpp"
#include "generators/random.hpp"
#include "../graphic/core/device.hpp"
#include "../graphic/core/opengl.hpp"


using namespace ei;

Galaxy::Galaxy(int _stars, float _size, int _ambientStars)
	: m_starInfos ("3c1", Graphic::VertexBuffer::PrimitiveType::POINT)
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
			pos[0] = rnd.Normal(radius * 10);
			pos[1] = rnd.Normal(radius * 10);
			pos[2] = rnd.Normal(radius * 10);
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
			float f2 = (1.2f - relative) * radius / 12;
			//2 arms starting on the same axis (+-)
			pos[0] = (r*radius + rnd.Uniform(-f2, f2)) *(i % 2 ? 1.f : -1.f);

			pos[1] = 0.f;
			//rotation is linear
			float a = relative * 2 * PI * rotationRate;
			Mat3x3 rotator = Mat3x3(cos(a), -sin(a), 0.f,
									sin(a),  cos(a), 0.f,
									0.f,     0.f,    1.f);
			pos = rotator * pos;
			pos[2] = rnd.Uniform(-height, height);
		}
		//alternative distribution
	/*	pos[0] = rnd.Uniform(-radius, radius);

		float max = sqrtf(radiusSq - pos[0] * pos[0]);
		pos[1] = rnd.Uniform(-max, max);
		
		float f = pos[0] * pos[0] + pos[1] * pos[1];
		f = 1 - f / radiusSq;
		pos[2] = rnd.Uniform(-f*height, f*height);*/

		m_starSystems.emplace_back(
			Math::FixVec3(pos),
			rnd.Uniform(2000, 40000),
			(float)rnd.Uniform(1,2)
			);
	}
	for (int i = 0; i < _ambientStars; ++i)
	{
		m_ambientStars.emplace_back(
			Math::FixVec3(),
			rnd.Uniform(2000, 40000),
			(float)rnd.Uniform(1, 2),
			Quaternion(rnd.Uniform(0.f, 2.f*PI), rnd.Uniform(0.f, 2.f*PI), 0.f),
			true);
	}
}

void Galaxy::Draw(const Input::Camera& _camera)
{
	//Mat4x4 worldView = GetTransformation(_camera.RenderState());
	m_starInfos.Clear();

	for (auto& starSystem : m_ambientStars)
	{
		StarVertex& star = starSystem.ComputeVertex(_camera);
		if (star.position[2] >= 0)
			m_starInfos.Add(star);
	}

	for (auto& starSystem : m_starSystems)
		m_starInfos.Add(starSystem.ComputeVertex(_camera));

	m_starInfos.SetDirty();

	Graphic::Effect& effect = Graphic::Resources::GetEffect(Graphic::Effects::BACKGROUNDSTAR);
	Graphic::Device::SetEffect(effect);
	effect.SetUniform(0, false);
	Graphic::Device::DrawVertices(m_starInfos, 0, m_starInfos.GetNumVertices());
}

