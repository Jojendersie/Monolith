#pragma once

#include "../voxel/model.hpp"
#include "../voxel/chunk.hpp"
#include "../math/math.hpp"

using namespace Math;

#include <vector>

namespace Physics {
	/// \brief Data structure to find potential colliding objects.
	/// \details The intersection test is using the separating axis theorem.
	///     This class stores the intervals and access indices to find the
	///     list positions on the projection axis.
	class IntersectionIdentifier
	{
	public:
		/// \brief Create with an model reference.
		/// \details Never call this function manually! It requires a
		///		registration in the universe. So the universe is the only one
		///		who should call this.
		explicit IntersectionIdentifier(Voxel::Model * _model);

		Voxel::Model* Model()			{ return m_model; }

		uint32_t Begin(int _axis) const { return m_beginIndices[_axis]; }
		uint32_t End(int _axis) const	{ return m_endIndices[_axis]; }
		uint32_t& Begin(int _axis)		{ return m_beginIndices[_axis]; }
		uint32_t& End(int _axis)		{ return m_endIndices[_axis]; }
	private:
		Voxel::Model* m_model;
		Vector<3, uint32_t> m_beginIndices;
		Vector<3, uint32_t> m_endIndices;
	};

	/// \brief Object for organizing and handling Models
	class Universe{
	public:
		Universe();
		~Universe();

		void AddModel(Voxel::Model * _model);
		void AddCelestial(Voxel::Model * _model);
//		void RemoveModel(Voxel::Model _model);
		void Update(double _deltaTime);
		const std::vector<IntersectionIdentifier>& getModels();
		float m_GravConst = 1.e-4f;		//real: m_GravConst=6.67384e-11;
	private:
		float collisionCheck(Voxel::Model* _model1, Voxel::Model* _model2, FixVec3 _start1, FixVec3 _start2, FixVec3 _end1, FixVec3 _end2);
		std::vector<IntersectionIdentifier> m_models;
		std::vector<Voxel::Model *> m_celestials;

		/// \brief Internal entry for the intersection finding step.
		struct Interval
		{
			IntersectionIdentifier* ident;
			Fix value;				///< Current interval boundary
			bool isBegin;			///< This is the smaller of two points from the interval

			/// \brief The constructor leaves the value undefined. This will
			///		be done in the next regular update.
			Interval(IntersectionIdentifier* _ident, bool _begin) :
				ident(_ident), isBegin(_begin) {}
		};

		std::vector<Interval> m_xAxis;  ///< 2 times number of models interval entries
		std::vector<Interval> m_yAxis;  ///< 2 times number of models interval entries
		std::vector<Interval> m_zAxis;  ///< 2 times number of models interval entries
		bool m_axisChanged;
    
		/// \brief Recompute the interval structure for intersection detection for
		///		all models.
		void RecomputeIntersectionIndices();

		/// \brief Recompute the intervals for the linked model.
		void RecomputeIntersectionIndices(IntersectionIdentifier& _ident);

		/// \brief Check if two projections intersect on a certain axis.
		/// \details Also returns true if the intervals touch only.
		/// \param _axis [in] Index for the axis: x=0, y=1, z=2
		bool IntersectIn(const IntersectionIdentifier* _i0,
			             const IntersectionIdentifier* _i1,
						 int _axis);
	};
/*	class CollisionInformation{
	public:
		CollisionInformation(float _time, FixVec3 _position){ m_time = _time; m_position = _position; }
		float m_time;
		FixVec3 m_position;
	};*/
}