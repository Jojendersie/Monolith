#include "model.hpp"
#include "chunk.hpp"
#include <cstdlib>
#include "input/camera.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/content.hpp"
#include "exceptions.hpp"
#include "timer.hpp"

using namespace Math;

namespace RenderStat {
	extern int g_numVoxels;
	extern int g_numChunks;
}

namespace Voxel {

	Model::Model() :
		m_numVoxels(0),
		m_mass(0.0f),
		m_center(0.0f),
		m_boundingSphereRadius(0.0f),
		m_voxelTree(this),
		m_chunks(),
		m_inertiaMoment(
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f),
		m_inertiaX(0.0f),
		m_inertiaY(0.0f),
		m_inertiaZ(0.0f),
		m_inertiaXY(0.0f),
		m_inertiaXZ(0.0f),
		m_inertiaYZ(0.0f),
		m_inertiaYZR(0.0f),
		m_inertiaXZR(0.0f),
		m_inertiaXYR(0.0f),
		m_angularVelocity(1.f, 0.f, 0.f, 0.f),
		m_acceleration(0.f, 0.f, 0.f)
	{
		auto x = IVec3(3) * 0.5f;
	}

	Model::~Model()
	{
	}

	// ********************************************************************* //
	struct DecideToDraw: public Model::ModelData::SVOProcessor
	{
		const Input::Camera& camera;					// Required for culling and LOD
		Model::ModelData* model;						// Operate on this data.
		std::unordered_map<Math::IVec4, Chunk>* chunks;	// Create or find chunks here.
		const Math::Mat4x4& modelView;

		DecideToDraw(const Input::Camera& _camera,
				Model::ModelData* _model,
				std::unordered_map<Math::IVec4, Chunk>* _chunks,
				const Math::Mat4x4& _modelView) :
			camera(_camera), model(_model), chunks(_chunks),
			modelView(_modelView)
		{}

		bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
		{
	//		if( !IsSolid( _type ) ) return false;

			// Compute a world space position
			float chunkLength = float(1 << _position[3]);
			Sphere boundingSphere(Vec3(_position[0] * chunkLength, _position[1] * chunkLength, _position[2] * chunkLength), 0.87f * chunkLength);
			boundingSphere.m_center += chunkLength * 0.5f;
			boundingSphere.m_center = boundingSphere.m_center * modelView;

			// View frustum culling
			if( !camera.IsVisible( boundingSphere ) )
				return false;

			// LOD - calculate a target level. If the current level is less or
			// equal the target draw. Where targetLOD is the level of the
			// required root node level. The resulting chunk will be up to 5
			// levels more tessellated
			//float detailResolution = 0.31f * log( lengthSq(boundingSphere.m_center - camera.GetPosition()) );
			float detailResolution = 0.021f * sq(log( lengthSq(boundingSphere.m_center) ));
			//float detailResolution = 0.045f * pow(log( lengthSq(boundingSphere.m_center - camera.GetPosition()) ), 1.65f);
			int targetLOD = max(2, Math::ceil(detailResolution));
			if( _position[3] <= targetLOD )
			{
				// For very far objects a chunk might be too detailed. In this case
				// a coarser level is used (usually 5 -> 32^3 chunks)
				int levels = max(0, 5 - (targetLOD - _position[3]));
				// Encode in position -> part of the key / hash
				IVec4 position(_position);
				position[3] |= levels << 16;
				auto chunk = chunks->find(position);
				if( chunk == chunks->end() )
				{
					// Chunk does not exist -> create
					ChunkBuilder builder; // TEMP -> in job verschieben
					chunk = chunks->insert(
						std::make_pair(position, std::move(Chunk(model, _position, levels)))
						).first;
					builder.RecomputeVertexBuffer(chunk->second);
				}// else Assert( !_node->Data().IsDirty(), "Node-data was not changed."); //if( _node->Data().IsDirty() )
					//builder->RecomputeVertexBuffer(chunk->second);
				// There are empty inner chunks
				if( chunk->second.NumVoxels() > 0 )
				{
					RenderStat::g_numVoxels += chunk->second.NumVoxels();
					RenderStat::g_numChunks++;
					chunk->second.Draw( modelView, camera.GetProjection() );
				}
				return false;
			}
			return true;
		}
	};

	// ********************************************************************* //
	void Model::Draw( const Input::Camera& _camera )
	{
		// Delete all invalid and old chunks
		ClearChunkCache();

		// Create a new model space transformation
		Math::Mat4x4 modelView;
		GetModelMatrix( modelView, _camera );

		// Iterate through the octree and render chunks depending on the lod.
		DecideToDraw param( _camera, &this->m_voxelTree, &this->m_chunks, modelView );
		m_voxelTree.Traverse( param );
	}

	// ********************************************************************* //
	VoxelType Model::Get( const Math::IVec3& _position, int _level ) const
	{
		auto node = m_voxelTree.Get(_position, _level);
		if( node ) return node->Data().type;
		
		return VoxelType::UNDEFINED;
	}



	// ********************************************************************* //
	Math::Mat4x4& Model::GetModelMatrix( Math::Mat4x4& _out, const Math::Transformation& _reference ) const
	{
		_out = Mat4x4::Translation(-m_center) * GetTransformation(_reference);
		return _out;
	}

	Math::Mat4x4& Model::GetModelMatrix( Math::Mat4x4& _out, const Input::Camera& _reference ) const
	{
		_out = Mat4x4::Translation(-m_center) * GetTransformation(_reference.RenderState());
		return _out;
	}


	// ********************************************************************* //
	void Model::UpdateMass( const Math::IVec4& _position, const Component& _oldType, const Component& _newType )
	{
		// Compute real volume from logarithmic size
		int size = 1 << _position[3];
		Vec3 voxelCenter = Math::IVec3(_position) + size * 0.5f;
		float voxelSurface = size * size / 6.0f;
		size = size * size * size;

		// Remove the old voxel
		if( TypeInfo::GetMass(_oldType.type) > 0.0f )
		{
			float oldMass = TypeInfo::GetMass(_oldType.type) * size;
			m_center = (m_center * m_mass - voxelCenter * oldMass) / (m_mass - oldMass);
			m_mass -= oldMass;
			m_numVoxels -= size;

			//update inertia helper variables
			m_inertiaX -= oldMass*voxelCenter[0];
			m_inertiaY -= oldMass*voxelCenter[1];
			m_inertiaZ -= oldMass*voxelCenter[2];
			m_inertiaXY -= oldMass*voxelCenter[0] * voxelCenter[1];
			m_inertiaXZ -= oldMass*voxelCenter[0] * voxelCenter[2];
			m_inertiaYZ -= oldMass*voxelCenter[1] * voxelCenter[2];
			float xSq = voxelCenter[0] * voxelCenter[0];
			float ySq = voxelCenter[1] * voxelCenter[1];
			float zSq = voxelCenter[2] * voxelCenter[2];
			float rSq = voxelSurface;
			m_inertiaXYR -= oldMass*(xSq + ySq + rSq);
			m_inertiaXZR -= oldMass*(xSq + zSq + rSq);
			m_inertiaYZR -= oldMass*(ySq + zSq + rSq);

		}

		// Add new voxel
		if( TypeInfo::GetMass(_newType.type) )
		{
			float newMass = TypeInfo::GetMass(_newType.type) * size;
			m_center = (m_center * m_mass + voxelCenter * newMass) / (m_mass + newMass);
			m_mass += newMass;
			m_numVoxels += size;

			//update inertia helper variables
			m_inertiaX += newMass*voxelCenter[0];
			m_inertiaY += newMass*voxelCenter[1];
			m_inertiaZ += newMass*voxelCenter[2];
			m_inertiaXY += newMass*voxelCenter[0] * voxelCenter[1];
			m_inertiaXZ += newMass*voxelCenter[0] * voxelCenter[2];
			m_inertiaYZ += newMass*voxelCenter[1] * voxelCenter[2];
			float xSq = voxelCenter[0] * voxelCenter[0];
			float ySq = voxelCenter[1] * voxelCenter[1];
			float zSq = voxelCenter[2] * voxelCenter[2];
			float rSq = voxelSurface;
			m_inertiaXYR += newMass*(xSq + ySq + rSq);
			m_inertiaXZR += newMass*(xSq + zSq + rSq);
			m_inertiaYZR += newMass*(ySq + zSq + rSq);
		}

		// calculate new moment of inertia
		{
			float x0 = m_center[0];
			float y0 = m_center[1];
			float z0 = m_center[2];
			float xSq = x0 * x0 * m_mass;
			float ySq = y0 * y0 * m_mass;
			float zSq = z0 * z0 * m_mass;
			float xy = x0 * y0 * m_mass;
			float xz = x0 * z0 * m_mass;
			float yz = y0 * z0 * m_mass;
			float xxi = 2 * x0 * m_inertiaX;
			float yyi = 2 * y0 * m_inertiaY;
			float zzi = 2 * z0 * m_inertiaZ;

			float I11 = ySq + zSq - yyi - zzi + m_inertiaYZR;
			float I22 = xSq + zSq - xxi - zzi + m_inertiaXZR;
			float I33 = xSq + ySq - xxi - yyi + m_inertiaXYR;

			float I12 = -xy + x0*m_inertiaY + y0*m_inertiaX - m_inertiaXY;
			float I13 = -xz + x0*m_inertiaZ + z0*m_inertiaX - m_inertiaXZ;
			float I23 = -yz + y0*m_inertiaZ + z0*m_inertiaY - m_inertiaYZ;

			m_inertiaMoment=Mat3x3(I11,I12,I13,I12,I22,I23,I13,I23,I33);
		}
		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume
		m_boundingSphereRadius = Math::max(m_boundingSphereRadius, Math::length(Math::Vector<3,float>(m_center) - Math::Vector<3,int>(_position)) );
	}

	// ********************************************************************* //
    void Model::UpdatePhysics()
	{
		// Do nothing if nothing changed
		if(!m_voxelTree.Get(IVec3(0,0,0), m_voxelTree.GetRootLevel())->Data().IsPhysicsDirty())
			return;

		TimeQuerySlot timer;
		TimeQuery(timer);

		struct CheckAndUpdatePhysics: public Model::ModelData::SVOProcessor
		{
			Model* model;						// Operate on this data.

			CheckAndUpdatePhysics(Model* _model) :
				model(_model)
			{}

			bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
			{
				if( _node->Data().IsPhysicsDirty() )
				{
					// Do things which can be updated locally
				}

				// Update tensor for each real element.
				if( !_node->Children() )
				{
					// Compute the position in relation to the mass center
					float r = float(1 << _position[3]);
					float x = _position[0] - model->GetCenter()[0] + r * 0.5f;
					float y = _position[1] - model->GetCenter()[1] + r * 0.5f;
					float z = _position[2] - model->GetCenter()[2] + r * 0.5f;
					r *= r / 6.f;
					model->m_inertiaMoment += Voxel::TypeInfo::GetMass(_node->Data().type)
						* Mat3x3(y*y + z*z+r, -x*y,      -x*z,
						         -x*y,      x*x + z*z+r, -y*z,
								 -x*z,      -y*z,      x*x + y*y+r);
				}

				_node->Data().dirtyPhysics = 0;
				return true;
			}
		};

		// Reset the inertia tensor
		m_inertiaMoment = Mat3x3(0.0f);
		m_voxelTree.Traverse( CheckAndUpdatePhysics(this) );

		double end = TimeQuery(timer);
		LOG_LVL1("Time for UpdatePhysics: " + std::to_string((end) * 1000.0));
	}


	// ********************************************************************* //
	bool Model::RayCast( const Math::WorldRay& _ray, int _targetLevel, ModelData::HitResult& _hit ) const
	{
		// Convert ray to model space
		Ray ray( _ray, *this );
		ray.m_origin += GetCenter();
		// TODO: Mat4x4::Scaling(m_scale) translation relevant?
		return m_voxelTree.RayCast(ray, _targetLevel, _hit);
	}

	// ********************************************************************* //
	void Model::ClearChunkCache()
	{
		for( auto it = m_chunks.begin(); it != m_chunks.end(); )
		{
			// Delete a chunk if it is not used in the last time
			Chunk& chunk = it->second;
			if( chunk.IsNotUsedLately() )
				it = m_chunks.erase( it );
			else {
				// Get the node to check if it is dirty
				Model::ModelData::SVON* node = m_voxelTree.Get( IVec3(chunk.m_root), chunk.m_root[3] );
				if( !node || node->Data().IsDirty() )
					it = m_chunks.erase( it );
				// Increase iterator only if nothing was deleted - deleting sets
				// the iterator to the next element anyway.
				else ++it;
			}
		}
	}


	// ********************************************************************* //
	struct StoreModelTree: public Model::ModelData::SVOProcessor
	{
		Jo::Files::IFile& file;

		StoreModelTree(Jo::Files::IFile& _file) :
			file(_file)
		{}

		bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
		{
			if( _node->Children() )
			{
				// Store in a mask which children are defined
				uint8_t mask = 0;
				for( int i = 0; i < 8; ++i )
					if( _node->GetChild(i) )
						mask |= 1 << i;
				file.WriteU8( mask );

				// Then iterate over whatever comes
			} else {
				// This is a leaf and contains a component
				// First store the mask byte with all 0 (no children)
				file.WriteU8( 0 );
				file.WriteU8( (uint8_t)_node->Data().type );
			}
			return true;
		}
	};

	enum struct ModelChunkTypes: uint8_t {
		END_MODEL,
		WORLD_LOCATION,
		COMPONENT_TREE
	};

	// ********************************************************************* //
	void Model::Save( Jo::Files::IFile& _file )
	{
		if(!_file.CanWrite()) throw InvalidSaveGame( _file, "To save a model the file must be opened for writing!" );

		_file.WriteU8( (uint8_t)ModelChunkTypes::WORLD_LOCATION );
			_file.Write( &m_position, sizeof(FixVec3) );
			_file.Write( &m_rotation, sizeof(Quaternion) );

		_file.WriteU8( (uint8_t)ModelChunkTypes::COMPONENT_TREE );
			_file.Write( &m_voxelTree.GetRootPosition(), sizeof(IVec3) );
			_file.WriteI32( m_voxelTree.GetRootLevel() );
			StoreModelTree proc( _file );
			m_voxelTree.Traverse( proc );

		_file.WriteU8( (uint8_t)ModelChunkTypes::END_MODEL );
	}

	// ********************************************************************* //
	void recursiveLoad( Model* _model, const Jo::Files::IFile& _file, const IVec3& _position, int _level )
	{
		uint8_t mask;
		_file.Read( 1, &mask );
		if( mask )
		{
			// There are children
			for( int i = 0; i < 8; ++i )
			{
				if( mask & (1 << i) )
				{
					recursiveLoad( _model, _file, _position * 2 + IVec3(CHILD_OFFSETS[i]), _level-1 );
				}
			}
		} else {
			// Now data comes
			VoxelType type;
			_file.Read( sizeof(VoxelType), &type );
			_model->Set( _position, _level, type );
		}
	}

	void Model::Load( const Jo::Files::IFile& _file )
	{
		Assert(m_numVoxels == 0, "Cannot load into a partially filled model!");

		ModelChunkTypes chunkType;
		_file.Read( 1, &chunkType );
		while(chunkType != ModelChunkTypes::END_MODEL)
		{
			switch( chunkType )
			{
			case ModelChunkTypes::WORLD_LOCATION:
				_file.Read( sizeof(FixVec3), &m_position );
				_file.Read( sizeof(Quaternion), &m_rotation );
				break;
			case ModelChunkTypes::COMPONENT_TREE:
				IVec3 root; int level;
				_file.Read( sizeof(IVec3), &root[0] );
				_file.Read( sizeof(int), &level );
				recursiveLoad( this, _file, root, level );
				break;
			}
			_file.Read( 1, &chunkType );
		}
	}
};