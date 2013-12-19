#pragma once

namespace Math {

	// ******************************************************************************** //
	// A class for boxes
	class Polyhedron
	{
	public:
		//Constructors
		Polyhedron();
		Polyhedron(const Polyhedron _ph1, const Polyhedron _ph2);
		Polyhedron(const std::list<Polyhedron> _polyhedra);

		/// \brief true if the Polyhedron contains no internal points
		bool IsEmpty();
		/// \brief returns the central Point of the Polyhedron
		Math::Vec3 GetCenter(){ return Vec3(); }
	};

} // namespace Math
