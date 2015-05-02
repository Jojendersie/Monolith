#pragma once

#include "voxel/model.hpp"
#include "computersystem.hpp"

/// \brief A ship is a model with additional simulation and control options.
/// \details A standard ship design contains a single computer with one
///		instance of each other system as sub-system
class Ship: public Voxel::Model
{
public:
	Ship();
protected:
	Mechanics::ComputerSystem m_primarySystem;
};