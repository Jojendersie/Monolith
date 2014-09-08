#pragma once

#include "../voxel/model.hpp"
#include "../voxel/chunk.hpp"

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
		float m_GravConst = 1.e-3f;		//real: m_GravConst=6.67384e-11;
	private:
		std::vector<Voxel::Model *> m_models;
		std::vector<Voxel::Model *> m_celestials;
	};
};