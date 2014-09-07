#include "universe.hpp"
#include "../math/math.hpp"
#include <math.h>
#include <sstream>
#include <string>

using namespace Math;

namespace Physics{
	/// \brief Object for organizing and handling Models
	Universe::Universe(){}
	Universe::~Universe(){}

		void Universe::AddModel(Voxel::Model* _model){
			m_models.push_back(_model);
		}
		void Universe::AddCelestial(Voxel::Model* _model){
			AddModel(_model);
			m_celestials.push_back(_model);
		}
		void Universe::Update(double _deltaTime){
			for (std::vector<Voxel::Model *>::iterator cIter = m_celestials.begin(); cIter != m_celestials.end(); ++cIter){
				Voxel::Model *celestial = *cIter;
				float factor = celestial->GetMass()*m_GravConst;
				for (std::vector<Voxel::Model *>::iterator mIter = m_models.begin(); mIter != m_models.end(); ++mIter){
					Voxel::Model *model = *mIter;
					if (celestial == model){
						continue;
					}
					//TODO: Fix bug
					//Vec3 direction = celestial.GetCenter() - model.GetCenter();
					Vec3 direction = Vec3(celestial->GetPosition()[0] - model->GetPosition()[0],
						celestial->GetPosition()[1] - model->GetPosition()[1],
						celestial->GetPosition()[2] - model->GetPosition()[2]);

					float mFactor = length(direction);

					if (mFactor < 10){
						mFactor=10;
					}

					mFactor = factor*(model->GetMass()) / mFactor / mFactor / mFactor;
					//std::ostringstream ss;
					//ss << "\n mFactor" << factor << " - " << mFactor << " - " << length(direction);
					//ss << "\n direction:" << direction[0] << "/" << direction[1] << "/" << direction[2];
					direction *= mFactor;
					//TODO: Fix bug
					//model.SetVelocity( model.GetVelocity + direction);
					Vec3 newVel = model->GetVelocity();
					//ss << "\n speed:" << newVel[0] << "/" << newVel[1] << "/" << newVel[2];
					newVel[0] += direction[0];
					newVel[1] += direction[1];
					newVel[2] += direction[2];
					//LOG_LVL2(ss.str());
					model->SetVelocity(newVel);
				}
			}
			for (std::vector<Voxel::Model *>::iterator mIter = m_models.begin(); mIter != m_models.end(); ++mIter){
				Voxel::Model *model = *mIter;
				//TODO: Fix bug
				//model.SetPosition (model.GetPosition() + FixVec3(model.GetVelocity()*_deltaTime));
				FixVec3 newPosition = model->GetPosition();
				newPosition[0] += Fix(model->GetVelocity()[0] * _deltaTime);
				newPosition[1] += Fix(model->GetVelocity()[1] * _deltaTime);
				newPosition[2] += Fix(model->GetVelocity()[2] * _deltaTime);
				model->SetPosition(newPosition);

			}
		}
		std::vector<Voxel::Model*> Universe::getModels(){
			return m_models;
		}
};