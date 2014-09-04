#pragma once

#include "../voxel/model.hpp"

#include <vector>

namespace Physics{
	/// \brief Object for organizing and handling Models
	class Universe{
	public:
		Universe();
		~Universe();

		void AddModel(Voxel::Model _model);
		void AddCelestial(Voxel::Model _model);
//		void RemoveModel(Voxel::Model _model);
		void Update(double _deltaTime);
		std::vector<Voxel::Model> getModels();
	private:
		const float m_GravConst = 0.001f;		//real: m_GravConst=6.67384e-11;
		std::vector<Voxel::Model> m_models;
		std::vector<Voxel::Model> m_celestials;
	};
};