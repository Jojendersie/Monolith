#include "universe.hpp"
#include "../math/math.hpp"
#include <math.h>
#include <sstream>
#include <string>
#include <algorithm>

using namespace Math;

namespace Physics{
	/// \brief Object for organizing and handling Models
	Universe::Universe()    {}
	Universe::~Universe()   {}

	void Universe::AddModel(Voxel::Model* _model) {
		// TODO: make this method stable for multi-threading
		m_models.push_back(IntersectionIdentifier(_model));
		m_axisChanged = true;
		IntersectionIdentifier* ident = &*(m_models.end()-1);
		ident->Begin(0) = (uint32_t)m_xAxis.size(); m_xAxis.push_back(Interval(ident, true));
		ident->End(0)   = (uint32_t)m_xAxis.size(); m_xAxis.push_back(Interval(ident, false));
		ident->Begin(1) = (uint32_t)m_xAxis.size(); m_yAxis.push_back(Interval(ident, true));
		ident->End(1)   = (uint32_t)m_xAxis.size(); m_yAxis.push_back(Interval(ident, false));
		ident->Begin(2) = (uint32_t)m_xAxis.size(); m_zAxis.push_back(Interval(ident, true));
		ident->End(2)   = (uint32_t)m_xAxis.size(); m_zAxis.push_back(Interval(ident, false));
	}

	void Universe::AddCelestial(Voxel::Model* _model) {
		AddModel(_model);
		m_celestials.push_back(_model);
	}

	void Universe::Update(double _deltaTime) {
		/* leapfrog  integration*/
		float fDeltaTime = (float)_deltaTime;
		for (auto mIter = m_models.begin(); mIter != m_models.end(); ++mIter){
			mIter->Model()->AgeTransformation();
		}
		/*movement via normal formula for uniform accelerated movement */
		for (auto mIter = m_models.begin(); mIter != m_models.end(); ++mIter){
			Voxel::Model *model = mIter->Model();
			FixVec3 newPos(
				model->GetOldPosition()[0] + Fix(((model->GetVelocity())[0] + model->GetAcceleration()[0] * _deltaTime / 2)*_deltaTime) , 
				model->GetOldPosition()[1] + Fix(((model->GetVelocity())[1] + model->GetAcceleration()[1] * _deltaTime / 2)*_deltaTime) ,
				model->GetOldPosition()[2] + Fix(((model->GetVelocity())[2] + model->GetAcceleration()[2] * _deltaTime / 2)*_deltaTime));
			model->SetPosition(newPos);
		}
		/*calaculate new speed with the mean of new and old acceleration*/
		for (auto mIter = m_models.begin(); mIter != m_models.end(); ++mIter){
			Voxel::Model *model = mIter->Model();
			Vec3 newAcceleration(0.f, 0.f, 0.f);
			for (std::vector<Voxel::Model *>::iterator cIter = m_celestials.begin(); cIter != m_celestials.end(); ++cIter){
				Voxel::Model *celestial = *cIter;
				if (celestial == model)
					continue;
				Vec3 direction = Vec3(celestial->GetPosition() - model->GetPosition());
				float mFactor = length(direction);
				mFactor = m_GravConst*(celestial->GetMass())*(model->GetMass()) /
					(mFactor*mFactor*mFactor);
				newAcceleration += mFactor*direction;
			}
			Vec3 newVelocity = model->GetVelocity() + (model->GetAcceleration() + newAcceleration)*fDeltaTime / 2.f;
			model->SetVelocity(newVelocity);
			model->SetAcceleration(newAcceleration);
		}

		// <- refactored up to this point
		/*quick and dirty collision detection*/
		for (int i = 0; i < m_models.size(); i++){
			for (int j = i+1; j < m_models.size(); j++){
				Voxel::Model* model1 = m_models[i].Model();
				Voxel::Model* model2 = m_models[j].Model();
				float collisionTime = 0;
				if (collisionCheck(model1, model2, 0, collisionTime)){
					/*placeholder calculation for collision Point and normal*/
					FixVec3 start1 = model1->GetOldPosition();
					FixVec3 start2 = model2->GetOldPosition();
					FixVec3 end1 = model1->GetPosition();
					FixVec3 end2 = model2->GetPosition();
					float r1 = model1->GetRadius(), r2 = model2->GetRadius();
					Fix fixT = Fix(collisionTime), fixR = Fix(1.) - fixT;
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
					Vec3 v1 = model1->GetVelocity(), v2 = model2->GetVelocity(),n=collisionNormal;
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
					Vec3 q1 = /*invert(I1) * */ ((Mat3x3)~R1) *cross(p1, n);
					Vec3 q2 = /*invert(I2) * */ ((Mat3x3)~R2) * cross(p2, n);
					// rotational Velocity in model[i] coordinates, w is actually small omega ^^.
					Vec3 w1 =(Vec3)((~R1)*model1->GetAngularVelocity()*R1), w2 =(Vec3)((~R2)*model2->GetAngularVelocity()*R2);

					float l = 2 * (dot(v1, n) - dot(v2, n) );// +dot(w1, I1*q1) - dot(w2, I2*q2));
					l /= (1 / m1 + 1 / m2)*lengthSq(n) ;// +dot(q1, I1*q1) + dot(q2, I2*q2);
					v1 -= l / m1*n;
					v2 += l / m2*n;
					w1 -= l*q1;
					w2 += l*q2;
					std::ostringstream ss;
					float foo = 2 * (dot(v1, n) - dot(v2, n) + dot(w1, I1*q1) - dot(w2, I2*q2));
					v1 += systemVelocity;
					v2 += systemVelocity;
					ss << "\n model 1: " << model1->GetVelocity()[0] << " " << model1->GetVelocity()[1] << " " << model1->GetVelocity()[2];
					ss << "///" << v1[0] << " " << v1[1] << " " << v1[2];
					ss << "\n model 2: " << model2->GetVelocity()[0] << " " << model2->GetVelocity()[1] << " " << model2->GetVelocity()[2];
					ss << "///" << v2[0] << " " << v2[1] << " " << v2[2];
					model1->SetVelocity(v1);
					model2->SetVelocity(v2);
					model1->SetPosition(model1->GetOldPosition());
					model2->SetPosition(model2->GetOldPosition());
					LOG_LVL2(ss.str());

				}
			}
		}
	}

	const std::vector<IntersectionIdentifier>& Universe::getModels() {
		return m_models;
	}

	/// placeholder Collision Check by linear Interpolation and bounding spheres only
	bool Universe::collisionCheck(const Voxel::Model* _model1,  const Voxel::Model* _model2, const float& _frameTime, float& _out){
		//erroneous frameTime
		if (_frameTime < 0.f || _frameTime >= 1.f)
			return false;
		Vec3 normalizedStart = Vec3(_model2->GetOldPosition() - _model1->GetOldPosition());
		Vec3 normalizedMove = Vec3(_model2->GetPosition() - _model1->GetPosition());
		if (_frameTime > 0)
			normalizedStart = (1 - _frameTime)*normalizedStart + _frameTime*normalizedMove;
		normalizedMove -= normalizedStart;

		float r1 = _model1->GetRadius(), r2 = _model2->GetRadius();
		float moveLengthSq = lengthSq(normalizedMove);
		
		//p-q formula with d being the discriminant
		float p = -dot(normalizedStart, normalizedMove) / moveLengthSq;
		float d = p*p - (lengthSq(normalizedStart) - (r1 + r2)*(r1 + r2)) / moveLengthSq;
		// no collision at all
		if (d < 0){
			return false;
		}
		d = sqrt(d);
		// moving away from earlier (hypothetical) collision
		if (p + d < 0)
			return false;
		// is inside
		if (p-d < 0 && p+d>=0){
			_out=_frameTime;
			return true;
		}
		// collision in this frame
		if (p - d >= 0 && p - d < 1){
			if (_frameTime>0)
				_out = _frameTime + (1 - _frameTime)*(p - d);
			else
				_out = p - d;
			return true;
		}
		return false;
	}
		

	// ********************************************************************* //
	IntersectionIdentifier::IntersectionIdentifier(Voxel::Model * _model) :
		m_model(_model)
	{
	}

	// ********************************************************************* //
	void Universe::RecomputeIntersectionIndices(IntersectionIdentifier& _ident)
	{
		// Take the models old and new bounding sphere to compute new axis
		// projections.
		FixVec3 minPos = min(_ident.Model()->GetOldPosition(), _ident.Model()->GetPosition());
		FixVec3 maxPos = max(_ident.Model()->GetOldPosition(), _ident.Model()->GetPosition());
		Fix radius(_ident.Model()->GetRadius());
		m_xAxis[_ident.Begin(0)].value = minPos[0] - radius;
		m_yAxis[_ident.Begin(1)].value = minPos[1] - radius;
		m_zAxis[_ident.Begin(2)].value = minPos[2] - radius;
		m_xAxis[_ident.End(0)].value = maxPos[0] + radius;
		m_yAxis[_ident.End(1)].value = maxPos[1] + radius;
		m_zAxis[_ident.End(2)].value = maxPos[2] + radius;
	}

	// ********************************************************************* //
	void Universe::RecomputeIntersectionIndices()
	{
		// First update the intervals themselves.
		for(int i = 0; i < m_models.size(); i++)
			RecomputeIntersectionIndices(m_models[i]);
		// Then resort the arrays.
		bool (*pred)(const Interval&, const Interval&) =
			[](const Interval& _i0, const Interval& _i1) {
				return _i0.value < _i1.value;
			};
		std::sort(m_xAxis.begin(), m_xAxis.end(), pred);
		std::sort(m_yAxis.begin(), m_yAxis.end(), pred);
		std::sort(m_zAxis.begin(), m_zAxis.end(), pred);

		// Finally repair the indices from the identifiers
		for(int i = 0; i < m_models.size() * 2; i++)
		{
			if( m_xAxis[i].isBegin )	// current entry a start point?
				m_xAxis[i].ident->Begin(0) = i;
			else m_xAxis[i].ident->End(0) = i;
			if( m_yAxis[i].isBegin )	// current entry a start point?
				m_yAxis[i].ident->Begin(1) = i;
			else m_yAxis[i].ident->End(1) = i;
			if( m_zAxis[i].isBegin )	// current entry a start point?
				m_zAxis[i].ident->Begin(2) = i;
			else m_zAxis[i].ident->End(2) = i;
		}
		m_axisChanged = false;
	}

	// ********************************************************************* //
	bool Universe::IntersectIn(const IntersectionIdentifier* _i0,
							   const IntersectionIdentifier* _i1,
							   int _axis)
	{
		Assert( !m_axisChanged, "Somebody added or deleted an object. Axis invariants do not hold!" );
		// Since all values are sorted two intervals overlap if the indices overlap.
		if(_i0->Begin(_axis) >= _i1->Begin(_axis))
			return _i0->Begin(_axis) <= _i1->End(_axis);
		else return _i1->Begin(_axis) <= _i0->End(_axis);
	}
}