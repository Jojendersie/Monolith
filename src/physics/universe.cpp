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
			float fDT = (float)_deltaTime;
			/* leapfrog  integration
			*/
			std::vector<FixVec3 *> newPoss;
			//std::vector<Vec3 *> accs;

			//std::ostringstream ss;
			for (std::vector<Voxel::Model *>::iterator mIter = m_models.begin(); mIter != m_models.end(); ++mIter){
				Vec3 acc1(0.f,0.f,0.f);
				Voxel::Model *model = *mIter;
				for (std::vector<Voxel::Model *>::iterator cIter = m_celestials.begin(); cIter != m_celestials.end(); ++cIter){
					if (*cIter == *mIter)
						continue;
					Voxel::Model *celestial = *cIter;
					Vec3 direction = Vec3(celestial->GetPosition()[0] - model->GetPosition()[0],
						celestial->GetPosition()[1] - model->GetPosition()[1],
						celestial->GetPosition()[2] - model->GetPosition()[2]);
					//ss << "\n direction:" << direction[0] << "/" << direction[1] << "/" << direction[2];
					float mFactor = length(direction);
					if (mFactor < 1)
						mFactor = 1;
					mFactor = m_GravConst*(celestial->GetMass())*(model->GetMass()) /
						(mFactor*mFactor*mFactor);
					//ss << "\n mFactor " << mFactor << " direction " << length(direction);
					acc1 += mFactor*direction;
				}
				//ss << "\n acc1 " << acc1[0] << " / " << acc1[1] << " / " << acc1[2];

				FixVec3 *newPos = new FixVec3(model->GetPosition());
				(*newPos)[0] += Fix(((model->GetVelocity())[0] + acc1[0] * fDT / 2)*fDT);
				(*newPos)[1] += Fix(((model->GetVelocity())[1] + acc1[1] * fDT / 2)*fDT);
				(*newPos)[2] += Fix(((model->GetVelocity())[2] + acc1[2] * fDT / 2)*fDT);
				newPoss.push_back(newPos);
				//ss << "\n" << ((model->GetVelocity())[0] + acc1[0] * fDT / 2)*fDT;
				//ss << " / " << ((model->GetVelocity())[1] + acc1[2] * fDT / 2)*fDT;
				//ss << " / " << ((model->GetVelocity())[1] + acc1[2] * fDT / 2)*fDT;
				Vec3 acc2(0.f, 0.f, 0.f);
				for (std::vector<Voxel::Model *>::iterator cIter = m_celestials.begin(); cIter != m_celestials.end(); ++cIter){
					Voxel::Model *celestial = *cIter;
					Vec3 direction = Vec3(celestial->GetPosition()[0] - *newPos[0],
						celestial->GetPosition()[1] - *newPos[1],
						celestial->GetPosition()[2] - *newPos[2]);
					float mFactor = length(direction);
					mFactor = m_GravConst*(celestial->GetMass())*(model->GetMass()) /
						(mFactor*mFactor*mFactor);
					acc2 += mFactor*direction;
				}
				Vec3 vel = model->GetVelocity();
				//vel += (acc1 + acc2)*fDT / 2;
				vel[0] += (acc1[0] + acc2[0])*fDT / 2;
				vel[1] += (acc1[1] + acc2[1])*fDT / 2;
				vel[2] += (acc1[2] + acc2[2])*fDT / 2;
				model->SetVelocity(vel);
				//ss << "\n acc1 " << acc1[0] << " / " << acc1[1] << " / " << acc1[2];
				//ss << " | acc1 " << length(acc1) << " vel " << length(model->GetVelocity());
			}
			for (int i = 0; i < m_models.size(); i++){
				//ss << "\n old Pos" << m_models[i]->GetPosition()[0] << " " << m_models[i]->GetPosition()[1] << " " << m_models[i]->GetPosition()[2];
				//ss << "\n new Pos" << (*(newPoss[i]))[0] << " " << (*(newPoss[i]))[1] << " " << (*(newPoss[i]))[2];
				m_models[i]->SetPosition(*(newPoss[i]));
			}
			//LOG_LVL2(ss.str());



			/*quick and dirty collision detection*/
			//for (std::vector<Voxel::Model *>::iterator mIter = m_models.begin(); mIter != m_models.end();){
			//	for (std::vector<Voxel::Model *>::iterator mIter2 = ++mIter; mIter2 != m_models.end(); ++mIter2){
			//		Voxel::Model *model1, *model2;
			//		model1 = *mIter;
			//		model2 = *mIter2;
			//		if (model1 == model2)
			//			continue;
			//		Vec3 direction = Vec3(model1->GetPosition()[0] - model2->GetPosition()[0],
			//			model1->GetPosition()[1] - model2->GetPosition()[1],
			//			model1->GetPosition()[2] - model2->GetPosition()[2]);
			//		if (length(direction) < model1->GetRadius() + model2->GetRadius()){
			//			float factor = (model1->GetRadius() + model2->GetRadius()) / length(direction)-1;
			//			std::ostringstream ss;
			//			ss << "\n collision:" << factor;
			//			LOG_LVL2(ss.str());


			//			FixVec3 newPosition = model1->GetPosition();
			//			newPosition[0] += Fix(direction[0] * factor);
			//			newPosition[1] += Fix(direction[1] * factor);
			//			newPosition[2] += Fix(direction[2] * factor);
			//			model1->SetPosition(newPosition);

			//			Vec3 newVel = model1->GetVelocity();
			//			newVel[0] += direction[0] * factor / 100;
			//			newVel[1] += direction[1] * factor / 100;
			//			newVel[2] += direction[2] * factor / 100;
			//			model1->SetVelocity(newVel);

			//			newPosition = model2->GetPosition();
			//			newPosition[0] -= Fix(direction[0] * factor);
			//			newPosition[1] -= Fix(direction[1] * factor);
			//			newPosition[2] -= Fix(direction[2] * factor);
			//			model2->SetPosition(newPosition);

			//			newVel = model2->GetVelocity();
			//			newVel[0] -= direction[0] * factor / 100;
			//			newVel[1] -= direction[1] * factor / 100;
			//			newVel[2] -= direction[2] * factor / 100;
			//			model2->SetVelocity(newVel);

			//		}
			//	}
			//}
		}
		std::vector<Voxel::Model*> Universe::getModels(){
			return m_models;
		}
};