#pragma once

#include "../voxel/model.hpp"
#include "../voxel/chunk.hpp"
#include "../math/math.hpp"

using namespace Math;

#include <vector>

namespace Physics{
	/// \brief Object for organizing and handling Models
	class Universe{
	public:
		Universe();
		~Universe();

		void AddModel(Voxel::Model * _model);
		void AddCelestial(Voxel::Model * _model);
//		void RemoveModel(Voxel::Model _model);
		void Update(double _deltaTime);
		std::vector<Voxel::Model *> getModels();
		float m_GravConst = 1.e-4f;		//real: m_GravConst=6.67384e-11;
	private:
		float collisionCheck(Voxel::Model* _model1, Voxel::Model* _model2, FixVec3 _start1, FixVec3 _start2, FixVec3 _end1, FixVec3 _end2);
		std::vector<Voxel::Model *> m_models;
		std::vector<Voxel::Model *> m_celestials;
	};
/*	class CollisionInformation{
	public:
		CollisionInformation(float _time, FixVec3 _position){ m_time = _time; m_position = _position; }
		float m_time;
		FixVec3 m_position;
	};*/
};