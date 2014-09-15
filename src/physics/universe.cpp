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

			/*quick and dirty collision detection*/
			for (int i = 0; i < m_models.size(); i++){
				for (int j = i+1; j < m_models.size(); j++){
					Voxel::Model* model1 = m_models[i];
					Voxel::Model* model2 = m_models[j];
					FixVec3 start1 = model1->GetPosition();
					FixVec3 start2 = model2->GetPosition();
					FixVec3 end1 = *newPoss[i];
					FixVec3 end2 = *newPoss[j];
					float t = collisionCheck(model1, model2, start1, start2, end1, end2);
					if (t>0){
						/*placeholder calculation for collision Point and normal*/
						float r1 = model1->GetRadius(), r2 = model2->GetRadius();
						Fix fixT = Fix(t), fixR = Fix(1.) - fixT;
						FixVec3 pos1 = FixVec3(fixT*start1[0] + fixR*end1[0],
							fixT*start1[1] + fixR*end1[1],
							fixT*start1[2] + fixR*end1[2]);
						FixVec3 pos2 = FixVec3(fixT*start2[0] + fixR*end2[0],
							fixT*start2[1] + fixR*end2[1],
							fixT*start2[2] + fixR*end2[2]);
						fixT = Fix(r1 / (r1 + r2));
						fixR = Fix(1.) - fixT;
						FixVec3 collisionPoint = FixVec3(pos1[0] * fixT + pos2[0] * fixR,
							pos1[1] * fixT + pos2[1] * fixR,
							pos1[2] * fixT + pos2[2] * fixR);
						Vec3 collisionNormal = Vec3(pos2[0] - pos1[0], pos2[1] - pos1[1], pos2[2] - pos1[2]);
		
						/*actual collision calculation*/
						Fix fMass1 = Fix(model1->GetMass()),fMass2 =Fix(model2->GetMass());
						// centerOfMass needed to apply conservation of rotational momentum
						FixVec3 centerOfMass = FixVec3(pos1[0] * fMass1  + pos2[0] * fMass2,
							pos1[1] * fMass1 + pos2[1] * fMass2,
							pos1[2] * fMass1 + pos2[2] * fMass2);
						// in the local system the sum of linear momentum is zero
						float m1 = model1->GetMass(), m2 = model2->GetMass();
						Vec3 v1 = model1->GetVelocity, v2 = model1->GetVelocity,n=collisionNormal;
						//workaround
						float f1, f2 = 1 / (m1 + m2);
						f1 = m1 * f2;
						f2 *= m2;
						Vec3 systemVelocity = Vec3(v1[0] * f1 + v2[0] * f2, v1[1] * f1 + v2[1] * f2, v1[2] * f1 + v2[2] * f2);
						//Vec3 systemVelocity = (m1*v1+m2*v2)/(m1+m2);
						v1 -= systemVelocity;
						v2 -= systemVelocity;
						Vec3 p1 = model1->Transform(collisionPoint);
						Vec3 p2 = model2->Transform(collisionPoint);
						Mat3x3 I1 = model1->GetInertiaMoment(), I2=model2->GetInertiaMoment();
						Quaternion R1 = model1->GetRotation();
						Quaternion R2 = model1->GetRotation();
						Vec3 q1 = /*invert(I1) * */ ((Mat3x3)(~R1)) * cross(p1, n);
						Vec3 q2 = /*invert(I2) * */ ((Mat3x3)(~R2)) * cross(p2, n);
						// rotational Velocity in model[i] coordinates, w is actually small omega ^^.
						Vec3 w1 =(Vec3)((~R1)*model1->GetAngularVelocity()*R1), w2 =(Vec3)((~R2)*model2->GetAngularVelocity()*R2);

						float l = 2 * (dot(v1, n) - dot(v2, n) + dot(w1, I1*q1) - dot(w2, I2*q2));
						l /= (1 / m1 + 1 / m2)*lengthSq(n) + dot(q1, I1*q1)+dot(q2,I2*q2);
						v1 -= l / m1*n;
						v2 += l / m1*n;
						w1 -= l*q1;
						w2 += l*q2;
					}
				}
			}

			for (int i = 0; i < m_models.size(); i++){
				//ss << "\n old Pos" << m_models[i]->GetPosition()[0] << " " << m_models[i]->GetPosition()[1] << " " << m_models[i]->GetPosition()[2];
				//ss << "\n new Pos" << (*(newPoss[i]))[0] << " " << (*(newPoss[i]))[1] << " " << (*(newPoss[i]))[2];
				m_models[i]->SetPosition(*(newPoss[i]));
			}
			//LOG_LVL2(ss.str());




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
		///brief placeholder Collision Check by linear Interpolation and bounding spheres only
		float Universe::collisionCheck(Voxel::Model* _model1, Voxel::Model* _model2, FixVec3 _start1, FixVec3 _start2, FixVec3 _end1, FixVec3 _end2){
			Vec3 normalizedStart = Vec3(_start2[0] - _start1[0],
				_start2[1] - _start1[1],
				_start2[2] - _start1[2]);
			Vec3 normalizedMove = Vec3((_end2[0] - _end1[0]) - normalizedStart[0],
				(_end2[1] - _end1[1]) - normalizedStart[1],
				(_end2[2] - _end1[2]) - normalizedStart[2]);
			float r1 = _model1->GetRadius(), r2 = _model2->GetRadius();
			float moveLengthSq = lengthSq(normalizedMove);
			float p = -dot(normalizedStart, normalizedMove) / moveLengthSq;
			float d = p*p - (lengthSq(normalizedStart) - (r1 + r2)*(r1 + r2)) / moveLengthSq;
			// no collision
			if (d < 0){
				return -1.f;
			}
			d = sqrt(d);
			// moving away
			if (p + d < 0)
				return -1.f;
			// is inside
			if (p-d < 0 && p+d>=0){
				return 0.f;
			}
			// collision in this frame
			if (p - d >= 0 && p - d < 1){
				return p - d;
			}
			return 0.f;
			//LOG_LVL2("collision");
		}
		
};