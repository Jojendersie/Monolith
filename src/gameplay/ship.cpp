#include "ship.hpp"

using namespace Math;


Ship::Ship() :
	m_computerSystemAllocation(256, false),
	m_primarySystem(*this, "CentralComputer"),
	m_targetVelocity(0.0f),
	m_centralComputerPosition(2012, 2012, 2012)
{
	Model::Set(m_centralComputerPosition, Voxel::ComponentType::COMPUTER);
}

// ********************************************************************* //
/*Ship::Ship(Voxel::Model* _model, const Math::IVec3& _centralComputer) :
	m_primarySystem("CentralComputer"),
	m_model( _model ),
	m_modelHandle( _model ),
	m_centralComputerPosition(_centralComputer),
	m_targetVelocity( 0.0f ),
	m_targetRotation( _model->GetRotation() )
{
	// Make sure there is a computer
	m_model->Set(m_centralComputerPosition, Voxel::ComponentType::COMPUTER);
	// TODO OnAdd for all components!
}*/

// ********************************************************************* //
Ship::~Ship()
{
}

// ********************************************************************* //
void Ship::AddComponent(const Math::IVec3& _position, Voxel::ComponentType _type)
{
	Model::Set(_position, _type);
	// Add to systems after insertion
	//m_primarySystem.OnAdd(_position, _type);
	// Report the change to each neighbor
	// TODO: first sample the area, than call OnChange(area)
}

// ********************************************************************* //
void Ship::RemoveComponent(const Math::IVec3& _position)
{
	if( _position == m_centralComputerPosition )
		throw "You cant remove the central computer! This leads to unimplemented handling (conversion to model/game over...)";
	Voxel::ComponentType type = Model::Get(_position);
	Model::Set(_position, Voxel::ComponentType::UNDEFINED);
	//m_primarySystem.OnRemove(_position, type);
	// Report the change to each neighbor
	// TODO: first sample the area, than call OnChange(area)
}

// ********************************************************************* //
void Ship::Save( Jo::Files::IFile& _file ) const
{
	Model::Save(_file);
	// Store ship state
	_file.Write(&m_targetVelocity, sizeof(Math::Vec3));
	_file.Write(&m_targetAngularVelocity, sizeof(Math::Vec3));
	// TODO: store precomputed system data
}

// ********************************************************************* //
void Ship::Load( const Jo::Files::IFile& _file )
{
	Model::Load(_file);
	// Load ship state
	_file.Read(sizeof(float), &m_targetVelocity);
	_file.Read(sizeof(Math::Vec3), &m_targetAngularVelocity);
	// Compute system data
	ComputeParameters();
}

// ********************************************************************* //
void Ship::Simulate(float _deltaTime)
{
	Mechanics::SystemRequierements requirements;
	// Get movement directions in object space
	Vec3 deltaAngularVelMod = m_targetAngularVelocity - GetRotationMatrix() * Model::m_angularVelocity;
	Vec3 deltaVelMod = m_targetVelocity - GetRotationMatrix() * Model::m_velocity;
	// Estimate the required forces to reach the target velocities
	requirements.torque = m_inertiaTensor * (deltaAngularVelMod / _deltaTime);
	requirements.thrust = (m_mass / _deltaTime) * deltaVelMod;

	// Do Energy management
	m_primarySystem.Estimate(_deltaTime, requirements);
	m_primarySystem.Process(_deltaTime, requirements);

	// Accelerate dependent on available torque and force.
	AddAngularVelocity( (_deltaTime * (m_inertiaTensorInverse * requirements.torque)) * GetRotationMatrix() );
	AddVelocity( ((_deltaTime / m_mass) * requirements.thrust) * GetRotationMatrix() );

	// Also simulate the physics
	Model::Simulate(_deltaTime);
}

// ********************************************************************* //
unsigned Ship::AllocNewSystemID()
{
	for(unsigned i=0; i<256; ++i)
		if(!m_computerSystemAllocation[i]) {
			m_computerSystemAllocation[i] = true;
			return i;
		}
	LOG_ERROR("Too many computer systems! No new id possible");
	return -1;
}

void Ship::ReleaseSystemID( unsigned _id )
{
	m_computerSystemAllocation[_id] = false;
}

// ********************************************************************* //
void Ship::ComputeParameters()
{
	ComputeInertia();
	// Delete the old state.
	m_primarySystem.ClearSystem();
	// Iterate over the tree, compute values for each element and reassign to systems.
	struct UpdateProcessor: public Model::ModelData::SVOProcessor
	{
		UpdateProcessor(Mechanics::ComputerSystem& _system) :
			m_system(_system)
		{
		}

		bool PreTraversal(const Math::IVec4& _position, const Model::ModelData::SVON* _node)
		{
			if( !_node->Children() )
			{
				// This is a component add it if it is a functional unit
				if(Voxel::TypeInfo::IsComputer(_node->Data().type)
					|| Voxel::TypeInfo::IsStorage(_node->Data().type)
					|| Voxel::TypeInfo::IsReactor(_node->Data().type)
					|| Voxel::TypeInfo::IsBattery(_node->Data().type)
					|| Voxel::TypeInfo::IsDrive(_node->Data().type)
					|| Voxel::TypeInfo::IsWeapon(_node->Data().type)
					|| Voxel::TypeInfo::IsShield(_node->Data().type)
					|| Voxel::TypeInfo::IsSensor(_node->Data().type))
					m_system.OnAdd(Math::IVec3(_position), _node->Data().type, _node->Data().sysAssignment);
			}
			return true;
		}
	private:
		Mechanics::ComputerSystem& m_system;
	};

	UpdateProcessor proc(m_primarySystem);
	m_voxelTree.Traverse( proc );
}