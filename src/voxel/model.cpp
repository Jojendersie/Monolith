#include "model.hpp"
#include "chunk.hpp"
#include <cstdlib>
#include "input/camera.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/content.hpp"
#include "exceptions.hpp"

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
		m_chunks()
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
			int targetLOD = max(LOG_CHUNK_SIZE, Math::ceil(detailResolution));
			if( _position[3] <= targetLOD )
			{
				// For very far objects a chunk might be too detailed. In this case
				// a coarser level is used (usually 5 -> 32^3 chunks)
				int levels = max(0, LOG_CHUNK_SIZE - (targetLOD - _position[3]));
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
	ComponentType Model::Get( const Math::IVec3& _position ) const
	{
		auto node = m_voxelTree.Get(_position, 0);
		if( node ) return node->Data().type;
		
		return ComponentType::UNDEFINED;
	}



	// ********************************************************************* //
	Math::Mat4x4& Model::GetModelMatrix( Math::Mat4x4& _out, const Math::Transformation& _reference ) const
	{
		_out = Mat4x4::Translation(-m_center) * GetTransformation(_reference);
		return _out;
	}

	Math::Mat4x4& Model::GetModelMatrix( Math::Mat4x4& _out, const Input::Camera& _reference ) const
	{
		if( this == _reference.GetAttachedModel() )
		{
			_out = Mat4x4::Translation(-m_center)
				 * Mat4x4::Rotation(m_rotation)
				 * Mat4x4::Translation(_reference.GetReferencePosition())
				 * Mat4x4::Rotation(_reference.RenderState().GetRotation());
		} else {
			_out = Mat4x4::Translation(-m_center) * GetTransformation(_reference.RenderState());
		}
		return _out;
	}


	// ********************************************************************* //
	void Model::Update( const Math::IVec4& _position, const Voxel& _oldType, const Voxel& _newType )
	{
		// Compute real volume from logarithmic size
		int size = 1 << _position[3];
		float voxelSurface = size * size / 6.0f;
		Vec3 center = Math::IVec3(_position) * size + size * 0.5f;
		int volume = size * size * size;

		// Remove the old voxel
		if( TypeInfo::GetMass(_oldType.type) > 0.0f )
		{
			float oldMass = TypeInfo::GetMass(_oldType.type) * volume;
			m_center = (m_center * m_mass - center * oldMass) / (m_mass - oldMass);
			m_mass -= oldMass;
			m_numVoxels -= volume;

			// Update inertia helper variables
			m_inertiaX_Y_Z += oldMass * center;
			m_inertiaXY_XZ_YZ[0] += oldMass * center[0] * center[1];
			m_inertiaXY_XZ_YZ[1] += oldMass * center[0] * center[2];
			m_inertiaXY_XZ_YZ[2] += oldMass * center[1] * center[2];
			Vec3 centerSq = center * center; 
			m_inertiaXYR_XZR_YZR[0] += oldMass * (centerSq[0] + centerSq[1] + voxelSurface);
			m_inertiaXYR_XZR_YZR[1] += oldMass * (centerSq[0] + centerSq[2] + voxelSurface);
			m_inertiaXYR_XZR_YZR[2] += oldMass * (centerSq[1] + centerSq[2] + voxelSurface);
		}

		// Add new voxel
		if( TypeInfo::GetMass(_newType.type) )
		{
			float newMass = TypeInfo::GetMass(_newType.type) * volume;
			m_center = (m_center * m_mass + center * newMass) / (m_mass + newMass);
			m_mass += newMass;
			m_numVoxels += volume;

			// Update inertia helper variables
			m_inertiaX_Y_Z += newMass * center;
			m_inertiaXY_XZ_YZ[0] += newMass * center[0] * center[1];
			m_inertiaXY_XZ_YZ[1] += newMass * center[0] * center[2];
			m_inertiaXY_XZ_YZ[2] += newMass * center[1] * center[2];
			Vec3 centerSq = center * center; 
			m_inertiaXYR_XZR_YZR[0] += newMass * (centerSq[0] + centerSq[1] + voxelSurface);
			m_inertiaXYR_XZR_YZR[1] += newMass * (centerSq[0] + centerSq[2] + voxelSurface);
			m_inertiaXYR_XZR_YZR[2] += newMass * (centerSq[1] + centerSq[2] + voxelSurface);
		}

		Vec3 xxm_yym_zzm = m_center * m_center * m_mass;
		Vec3 xym_xzm_yzm(m_center[0] * m_center[1] * m_mass,
			m_center[0] * m_center[2] * m_mass,
			m_center[2] * m_center[1] * m_mass);
		Vec3 x2i_y2i_z2i = m_center * 2 * m_inertiaX_Y_Z;
		xxm_yym_zzm -= x2i_y2i_z2i;

		float I11 = xxm_yym_zzm[1] + xxm_yym_zzm[2] + m_inertiaXYR_XZR_YZR[2];
		float I22 = xxm_yym_zzm[0] + xxm_yym_zzm[2] + m_inertiaXYR_XZR_YZR[1];
		float I33 = xxm_yym_zzm[0] + xxm_yym_zzm[1] + m_inertiaXYR_XZR_YZR[0];

		float I12 = -xym_xzm_yzm[0] + m_center[0]*m_inertiaX_Y_Z[1] + m_center[1]*m_inertiaX_Y_Z[0] - m_inertiaXY_XZ_YZ[0];
		float I13 = -xym_xzm_yzm[1] + m_center[0]*m_inertiaX_Y_Z[2] + m_center[2]*m_inertiaX_Y_Z[0] - m_inertiaXY_XZ_YZ[1];
		float I23 = -xym_xzm_yzm[2] + m_center[1]*m_inertiaX_Y_Z[2] + m_center[2]*m_inertiaX_Y_Z[1] - m_inertiaXY_XZ_YZ[2];

		m_inertiaTensor = Mat3x3(I11,I12,I13,I12,I22,I23,I13,I23,I33);

		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume
		// TODO remove Math::Vector if replaced by template
		m_boundingSphereRadius = Math::max(m_boundingSphereRadius, Math::length(Math::Vector<3,float>(m_center) - Math::Vector<3,int>(_position)) );
	}

	// ********************************************************************* //
	void Model::Simulate(float _deltaTime)
	{
		m_position += FixVec3( m_velocity * _deltaTime );
		// Create a delta Quaternion from rotation velocity
		// http://physicsforgames.blogspot.de/2010/02/quaternions.html
		Quaternion deltaRot;
		Vec3 theta = m_angularVelocity * _deltaTime;
		float thetaMagSq = lengthSq(theta);
		float s;
		if(thetaMagSq * thetaMagSq / 24.0f < 1e-6f)
		{
			deltaRot.r = 1.0f - thetaMagSq / 2.0f;
			s = 1.0f - thetaMagSq / 6.0f;
		}
		else
		{
			float thetaMag = sqrt(thetaMagSq);
			deltaRot.r = cos(thetaMag);
			s = sin(thetaMag) / thetaMag;
		}
		deltaRot.i = theta[0] * s;
		deltaRot.j = theta[1] * s;
		deltaRot.k = theta[2] * s;

		m_rotation *= deltaRot;
	}

	// ********************************************************************* //
	bool Model::RayCast( const Math::WorldRay& _ray, int _targetLevel, ModelData::HitResult& _hit, float& _distance ) const
	{
		// Convert ray to model space
		Ray ray( _ray, *this );
		ray.m_origin += GetCenter();
		// TODO: Mat4x4::Scaling(m_scale) translation relevant?
		return m_voxelTree.RayCast(ray, _targetLevel, _hit, _distance);
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

		bool PreTraversal(const Math::IVec4& _position, const Model::ModelData::SVON* _node)
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
	void Model::Save( Jo::Files::IFile& _file ) const
	{
		if(!_file.CanWrite()) throw InvalidSaveGame( _file, "To save a model the file must be opened for writing!" );

		_file.WriteU8( (uint8_t)ModelChunkTypes::WORLD_LOCATION );
			_file.Write( &m_position, sizeof(FixVec3) );
			_file.Write( &m_rotation, sizeof(Quaternion) );

		_file.WriteU8( (uint8_t)ModelChunkTypes::COMPONENT_TREE );
			_file.Write( &m_voxelTree.GetRootPosition(), sizeof(IVec3) );
			_file.WriteI32( m_voxelTree.GetRootSize() );
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
			ComponentType type;
			_file.Read( sizeof(ComponentType), &type );
			_model->Set( _position, type );
		}
	}

	void Model::Load( const Jo::Files::IFile& _file )
	{
		if(m_numVoxels > 0) {
			// Clear
			m_voxelTree = ModelData(this);
			m_numVoxels = 0;
		}

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

		ComputeBoundingBox();
	}
	
	// ********************************************************************* //
	void Model::UpdateBoundingBox()
	{
		// Transform each octree corner to world space
		float size = float(1 << m_voxelTree.GetRootSize());
		Vec3 octMin = m_objectBBmin - m_center;//m_voxelTree.GetRootPosition() * size - m_center;
		Vec3 octMax = m_objectBBmax - m_center;//octMin + size;
		m_boundingBox.min = m_boundingBox.max = TransformInverse(octMin);
		FixVec3 tvec = TransformInverse(Vec3(octMin[0], octMin[1], octMax[2]));
		m_boundingBox.min = min(m_boundingBox.min, tvec); m_boundingBox.max = max(m_boundingBox.max, tvec);
		tvec = TransformInverse(Vec3(octMin[0], octMax[1], octMin[2]));
		m_boundingBox.min = min(m_boundingBox.min, tvec); m_boundingBox.max = max(m_boundingBox.max, tvec);
		tvec = TransformInverse(Vec3(octMin[0], octMax[1], octMax[2]));
		m_boundingBox.min = min(m_boundingBox.min, tvec); m_boundingBox.max = max(m_boundingBox.max, tvec);
		tvec = TransformInverse(Vec3(octMax[0], octMin[1], octMin[2]));
		m_boundingBox.min = min(m_boundingBox.min, tvec); m_boundingBox.max = max(m_boundingBox.max, tvec);
		tvec = TransformInverse(Vec3(octMax[0], octMin[1], octMax[2]));
		m_boundingBox.min = min(m_boundingBox.min, tvec); m_boundingBox.max = max(m_boundingBox.max, tvec);
		tvec = TransformInverse(Vec3(octMax[0], octMax[1], octMin[2]));
		m_boundingBox.min = min(m_boundingBox.min, tvec); m_boundingBox.max = max(m_boundingBox.max, tvec);
		tvec = TransformInverse(octMax);
		m_boundingBox.min = min(m_boundingBox.min, tvec); m_boundingBox.max = max(m_boundingBox.max, tvec);
	}

	// ********************************************************************* //
	void Model::ComputeBoundingBox()
	{
		struct ComputeBB: public ModelData::SVONeighborProcessor
		{
		public:
			IVec3 bbmin, bbmax; /// Finest level voxel positions

			ComputeBB() :
				bbmin(2147483647),
				bbmax(-2147483647)
			{
			}

			bool PreTraversal(const Math::IVec4& _position, ModelData::SVON* _node,
				const ModelData::SVON* _left, const ModelData::SVON* _right, const ModelData::SVON* _bottom,
				const ModelData::SVON* _top, const ModelData::SVON* _front, const ModelData::SVON* _back)
				//bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
			{
				// Stop in inner nodes (they cannot contribute to the bounding box)
				if( _left && _right && _bottom && _top && _front && _back )
					return false;
				// TODO: early out due to possible boundaries?

				// On finest level update minima and maxima
				if(_position[3] == 0)
				{
					// TODO: optimize
					bbmin = min(bbmin, IVec3(_position));
					bbmax = max(bbmax, IVec3(_position));
				}

				return true;
			}
		};

		ComputeBB proc;
		m_voxelTree.TraverseEx(proc);
		m_objectBBmin = proc.bbmin;
		m_objectBBmax = proc.bbmax;
	}
};