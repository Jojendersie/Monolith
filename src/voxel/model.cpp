#include "model.hpp"
#include "chunk.hpp"
#include <cstdlib>
#include "input/camera.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/content.hpp"
#include "exceptions.hpp"
#include "algorithm/hashmap.hpp"

//test
#include "../timer.hpp"
#include <iostream>

using namespace ei;
using namespace Math;

namespace RenderStat {
	extern int g_numVoxels;
	extern int g_numChunks;
}

namespace Voxel {

	// how strongly velocity follows rotation
	// physically correct would be 0
	const float ROTATE_VELOCITY_COUPLING = 0.7f;

	Model::Model() :
		m_numVoxels(0),
		m_mass(0.0f),
		m_center(0.0f),
		m_oldCenter(0.0f),
		m_boundingSphereRadius(0.0f),
		m_voxelTree(this),
		m_chunks(),
		m_rotateVelocity(false),
		m_angularVelocity(0.f)
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
		std::unordered_map<IVec4, Chunk>* chunks;	// Create or find chunks here.
		const Mat4x4& modelView;

		DecideToDraw(const Input::Camera& _camera,
				Model::ModelData* _model,
				std::unordered_map<IVec4, Chunk>* _chunks,
				const Mat4x4& _modelView) :
			camera(_camera), model(_model), chunks(_chunks),
			modelView(_modelView)
		{}

		bool PreTraversal(const IVec4& _position, Model::ModelData::SVON* _node)
		{
	//		if( !IsSolid( _type ) ) return false;

			// Compute a world space position
			float chunkLength = float(1 << _position[3]);
			Sphere boundingSphere(Vec3(_position[0] * chunkLength, _position[1] * chunkLength, _position[2] * chunkLength), 0.87f * chunkLength);
			boundingSphere.center += chunkLength * 0.5f;
			boundingSphere.center = transform( boundingSphere.center, modelView );

			// View frustum culling
			if( !camera.IsVisible( boundingSphere ) )
				return false;

			// LOD - calculate a target level. If the current level is less or
			// equal the target draw. Thereby targetLOD is the level of the
			// required root node level. The resulting chunk will be up to 5
			// levels more tessellated
			float detailResolution = 0.45f * log( lensq(boundingSphere.center) );
			//float detailResolution = 0.030f * sq(log( lensq(boundingSphere.center) ));
			//float detailResolution = 0.045f * pow(log( lengthSq(boundingSphere.m_center) ), 1.65f);
			int targetLOD = max(LOG_CHUNK_SIZE, (int)ceil(detailResolution));
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
		Mat4x4 modelView;
		GetModelMatrix( modelView, _camera );

		// Iterate through the octree and render chunks depending on the lod.
		DecideToDraw param( _camera, &this->m_voxelTree, &this->m_chunks, modelView );
		m_voxelTree.Traverse( param );
	}

	// ********************************************************************* //
	ComponentType Model::Get( const IVec3& _position ) const
	{
		auto node = m_voxelTree.Get(_position, 0);
		if( node ) return node->Data().type;
		
		return ComponentType::UNDEFINED;
	}

	// ********************************************************************* //
	void Model::Damage(const ei::IVec3& _position, uint32_t _damage)
	{
		Voxel& voxel = m_voxelTree.Get(_position, 0)->Data();

		if (_damage >= voxel.health)
		{
			Set(_position, ComponentType::UNDEFINED);
			m_hasTakenDamage = true;
		}
		voxel.health -= _damage;
	}

	// ********************************************************************* //
	Mat4x4& Model::GetModelMatrix( Mat4x4& _out, const Math::Transformation& _reference ) const
	{
		_out = GetTransformation(_reference) * translation(-m_center);
		return _out;
	}

	Mat4x4& Model::GetModelMatrix( Mat4x4& _out, const Input::Camera& _reference ) const
	{
		if( this == _reference.GetAttachedModel() )
		{
			_out = rotationH(_reference.Transformation().GetRotation())
				 * translation(_reference.GetReferencePosition())
				 * rotationH(m_rotation)
				 * translation(-m_center);
		} else {
			_out = GetTransformation(_reference.Transformation()) * translation(-m_center);
		}
		return _out;
	}


	// ********************************************************************* //
	void Model::Update( const IVec4& _position, const Voxel& _oldType, const Voxel& _newType )
	{
		// Compute real volume from logarithmic size
		int size = 1 << _position[3];
		float voxelSurface = size * size / 6.0f;
		Vec3 center = IVec3(_position) * size + size * 0.5f;
		int volume = size * size * size;

		// Remove the old voxel
		if( TypeInfo::GetMass(_oldType.type) > 0.0f )
		{
			float oldMass = TypeInfo::GetMass(_oldType.type) * volume;
			m_center = (m_center * m_mass - center * oldMass) / (m_mass - oldMass);
			m_mass -= oldMass;
			m_numVoxels -= volume;
		//	Assert(m_numVoxels >= 0, "Yeah!");

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

		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume
		// TODO remove Math::Vector if replaced by template
		m_boundingSphereRadius = max(m_boundingSphereRadius, 0.7f + len(Vec3(m_center) - Vec3(_position)) );
	}

	// ********************************************************************* //
	void Model::UpdateInertialTensor()
	{
/*		Vec3 xxm_yym_zzm = m_center * m_center * m_mass;
		Vec3 xym_xzm_yzm(m_center[0] * m_center[1] * m_mass,
			m_center[0] * m_center[2] * m_mass,
			m_center[2] * m_center[1] * m_mass);
		Vec3 x2i_y2i_z2i = m_center * 2 * m_inertiaX_Y_Z;
		xxm_yym_zzm -= x2i_y2i_z2i;

		float I11 = xxm_yym_zzm[1] + xxm_yym_zzm[2] + m_inertiaXYR_XZR_YZR[2];
		float I22 = xxm_yym_zzm[0] + xxm_yym_zzm[2] + m_inertiaXYR_XZR_YZR[1];
		float I33 = xxm_yym_zzm[0] + xxm_yym_zzm[1] + m_inertiaXYR_XZR_YZR[0];

		float I12 = -xym_xzm_yzm[0] + m_center[0] * m_inertiaX_Y_Z[1] + m_center[1] * m_inertiaX_Y_Z[0] - m_inertiaXY_XZ_YZ[0];
		float I13 = -xym_xzm_yzm[1] + m_center[0] * m_inertiaX_Y_Z[2] + m_center[2] * m_inertiaX_Y_Z[0] - m_inertiaXY_XZ_YZ[1];
		float I23 = -xym_xzm_yzm[2] + m_center[1] * m_inertiaX_Y_Z[2] + m_center[2] * m_inertiaX_Y_Z[1] - m_inertiaXY_XZ_YZ[2];
	
		m_inertiaTensor = Mat3x3(I11, I12, I13, I12, I22, I23, I13, I23, I33);
		m_inertiaTensorInverse = invert(m_inertiaTensor);*/

		//incremental update does not work when changes are skipped
		ComputeInertia();
	}

	// ********************************************************************* //
	void Model::Simulate(float _deltaTime)
	{
		m_position += FixVec3( m_velocity * _deltaTime );
		// Create a delta Quaternion from rotation velocity
		// http://physicsforgames.blogspot.de/2010/02/quaternions.html
		Quaternion deltaRot;
		Vec3 theta = m_angularVelocity * _deltaTime;
		float thetaMagSq = lensq(theta);
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

		Rotate( deltaRot );
		// Also rotate the velocity, while this is not physical plausible it increases
		// the playability extreme.
		
		if (m_rotateVelocity)
		{
			
			deltaRot *= ROTATE_VELOCITY_COUPLING;
			m_velocity = transform(m_velocity, deltaRot);
		}
	}

	// ********************************************************************* //
	bool Model::RayCast( const Math::WorldRay& _ray, int _targetLevel, ModelData::HitResult& _hit, float& _distance ) const
	{
		// Convert ray to model space
		Ray ray = _ray.GetRelativeRay(*this);
		ray.origin += GetCenter();
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
	void Model::EvtCollision(Model& _other)
	{
		if (m_onCollision) m_onCollision(_other);
	}


	// ********************************************************************* //
	struct StoreModelTree: public Model::ModelData::SVOProcessor
	{
		Jo::Files::IFile& file;

		StoreModelTree(Jo::Files::IFile& _file) :
			file(_file)
		{}

		bool PreTraversal(const IVec4& _position, const Model::ModelData::SVON* _node)
		{
			if( _node->Children() )
			{
				// Store in a mask which children are defined
				uint8 mask = 0;
				for( int i = 0; i < 8; ++i )
					if( _node->GetChild(i) )
						mask |= 1 << i;
				file.WriteU8( mask );

				// Then iterate over whatever comes
			} else {
				// This is a leaf and contains a component
				// First store the mask byte with all 0 (no children)
				file.WriteU8( 0 );
				file.Write( &_node->Data().type, sizeof(ComponentType) );
				file.WriteU8( _node->Data().rotation );
				file.WriteU8( _node->Data().sysAssignment );
			}
			return true;
		}
	};

	enum struct ModelChunkTypes: uint8 {
		END_MODEL,
		WORLD_LOCATION,
		COMPONENT_TREE
	};

	// ********************************************************************* //
	void Model::Save( Jo::Files::IFile& _file ) const
	{
		if(!_file.CanWrite()) throw InvalidSaveGame( _file, "To save a model the file must be opened for writing!" );

		_file.WriteU8( (uint8)ModelChunkTypes::WORLD_LOCATION );
			_file.Write( &m_position, sizeof(FixVec3) );
			_file.Write( &m_rotation, sizeof(Quaternion) );

		_file.WriteU8( (uint8)ModelChunkTypes::COMPONENT_TREE );
			_file.Write( &m_voxelTree.GetRootPosition(), sizeof(IVec3) );
			_file.WriteI32( m_voxelTree.GetRootSize() );
			StoreModelTree proc( _file );
			m_voxelTree.Traverse( proc );

		_file.WriteU8( (uint8)ModelChunkTypes::END_MODEL );
	}

	// ********************************************************************* //
	void recursiveLoad( Model* _model, const Jo::Files::IFile& _file, const IVec3& _position, int _level )
	{
		uint8 mask;
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
			// Now the data follows
			Voxel voxel;
			_file.Read( sizeof(ComponentType), &voxel.type );
			voxel = Voxel(voxel.type); // Reinitialize with correct type information
			_file.Read( 1, &voxel.rotation );
			_file.Read( 1, &voxel.sysAssignment );
			_model->Set( _position, voxel );
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
				UpdateMatrices();
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
		ComputeInertia();
	//	UpdateInertialTensor();
		m_oldCenter = m_center;
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
	void Model::ComputeInertia()
	{
		/*struct MassProcessor: public Model::ModelData::SVOProcessor
		{
			MassProcessor() : center(0.0f), mass(0.0f), n(0) {}

			bool PreTraversal(const Math::IVec4& _position, const Model::ModelData::SVON* _node)
			{
				if( !_node->Children() )
				{
					// This is a component add it if it is a functional unit
					float m = TypeInfo::GetMass(_node->Data().type);
					center += m * (Vec3((float)_position[0], (float)_position[1], (float)_position[2]) + 0.5f);
					mass += m;
					++n;
				}
				return true;
			}

			Vec3 center;
			float mass;
			int n;
		};

		MassProcessor mproc;
		m_voxelTree.Traverse( mproc );
		m_mass = mproc.mass;
		m_center = mproc.center / m_mass;*/

		// https://de.wikipedia.org/wiki/Tr%C3%A4gheitstensor
		// Iterate over the tree, compute values for each element and reassign to systems.
		struct InertiaProcessor: public Model::ModelData::SVOProcessor
		{
			InertiaProcessor(const Vec3& _center) : newInertia(0.0f), center(_center) {}

			bool PreTraversal(const IVec4& _position, const Model::ModelData::SVON* _node)
			{
				if( !_node->Children() )
				{
					// This is a component add it if it is a functional unit
					float m = TypeInfo::GetMass(_node->Data().type);
					// TODO: optimize redundancy if compiler does not
					Vec3 p = Vec3((float)_position[0], (float)_position[1], (float)_position[2]) - center + 0.5f;
					// A voxel is handled as a single point and not as a box.
					// If it is set at the origin the tensor would be 0. Therefore I added
					// an regularization which is effectively the tensor of a box.
					float regularization = m * 2.0f / 3.0f;
					newInertia += m * Mat3x3(
						p[1]*p[1] + p[2]*p[2], -p[0] * p[1], -p[0] * p[2],
						-p[0] * p[1], p[0]*p[0] + p[2]*p[2], -p[1] * p[2],
						-p[0] * p[2], -p[1] * p[2], p[0]*p[0] + p[1]*p[1]
					);
				}
				return true;
			}

			Mat3x3 newInertia;
		private:
			const Vec3& center;
		};
		
		InertiaProcessor proc(m_center);
		m_voxelTree.Traverse( proc );
		m_inertiaTensor = proc.newInertia;
		m_inertiaTensorInverse = invert(proc.newInertia);
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

			bool PreTraversal(const IVec4& _position, ModelData::SVON* _node,
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

	// ********************************************************************* //
	struct ComputeFlatArray : public Model::ModelData::SVOProcessor
	{
		struct VoxelMark
		{
			VoxelMark(const IVec3& _pos, const Voxel* _vox)
				: position(_pos), voxel(_vox){}
			IVec3 position;
			const Voxel* voxel;
		};

		ComputeFlatArray()
		{
		}

		void flattenTree(const Model::ModelData& _tree, int _voxelCount)
		{
			m_voxelMarks.reserve(_voxelCount);

			voxelCount = 0;
			_tree.Traverse(*this);
		}

		bool extractModel(std::function<void(const VoxelMark& _mark)> _action)
		{
			// try to begin with the main computer in case of a ship
			// because the first model found remains
			auto it = m_voxelMarks.find(IVec3(2012, 2012, 2012));
			// computer is gone or this is not a ship
			if (!it)
			{
				it = m_voxelMarks.first();
			}
			//whole model is gone
		//	if (it != m_voxelMarks.end() && it->second->type == ComponentType::UNDEFINED)
				int brk = 123;
			if (!it || it.data()->type == ComponentType::UNDEFINED) return false;

			voxelCount = 0;
			static std::vector<VoxelMark> stack;
			stack.reserve(100);

			stack.emplace_back(it.key(), it.data());
			m_voxelMarks.remove(it);

			do{
				auto vox = stack.back(); 
				stack.pop_back();
				
				//push neighbors
				it = m_voxelMarks.find(vox.position + IVec3(1, 0, 0));
				if (it){ stack.emplace_back(it.key(), it.data()); m_voxelMarks.remove(it); }
				it = m_voxelMarks.find(vox.position + IVec3(-1, 0, 0));
				if (it){ stack.emplace_back(it.key(), it.data()); m_voxelMarks.remove(it); }
				it = m_voxelMarks.find(vox.position + IVec3(0, 1, 0));
				if (it){ stack.emplace_back(it.key(), it.data()); m_voxelMarks.remove(it); }
				it = m_voxelMarks.find(vox.position + IVec3(0, -1, 0));
				if (it){ stack.emplace_back(it.key(), it.data()); m_voxelMarks.remove(it); }
				it = m_voxelMarks.find(vox.position + IVec3(0, 0, 1));
				if (it){ stack.emplace_back(it.key(), it.data()); m_voxelMarks.remove(it); }
				it = m_voxelMarks.find(vox.position + IVec3(0, 0, -1));
				if (it){ stack.emplace_back(it.key(), it.data()); m_voxelMarks.remove(it); }

				++voxelCount;
				_action(vox);
			} while (stack.size());

			return true;
		}

		bool PreTraversal(const ei::IVec4& _position, const Model::ModelData::SVON* _node)	
		{
			if (!_position[3])
			{
			//	m_voxelMarks.emplace(IVec3(_position), &_node->Data());
				m_voxelMarks.add(IVec3(_position), &_node->Data());
			}
			return true;
		}

		int voxelCount;
		IVec3 offset;

		HashMap<IVec3, const Voxel*> m_voxelMarks;
	//	std::unordered_map<IVec3, const Voxel*> m_voxelMarks;

	};

	// ********************************************************************* //
	void Model::UpdateCenter(const ei::Vec3& _shift)
	{
		Translate(m_rotationMatrix * _shift);
		m_oldCenter = m_center;
	}

	// ********************************************************************* //
	std::vector<Model*> Model::UpdateCohesion()
	{
		static ComputeFlatArray flatVoxels;
		std::vector<Model*> models;
		TimeQuerySlot slot = 42;
		Vec3 center = m_center; // store old center before any changes happen

		if (!m_hasTakenDamage) return models;
		m_hasTakenDamage = false;

		TimeQuery(slot);
		if (m_numVoxels <= 0)
		{
			Delete();
			return models;
		}

		TimeQuery(slot);
		
		flatVoxels.flattenTree(m_voxelTree, m_numVoxels);

			// main part
		flatVoxels.extractModel([&](const ComputeFlatArray::VoxelMark& _mark){});
		
		double t = TimeQuery(slot);
	//	std::cout << "hashmap: " << t << std::endl;
		//always shift to update the changed center of mass
		UpdateCenter(m_center - m_oldCenter);
		UpdateInertialTensor();
		//the model is still fully connected
		// sometimes an undefined voxel is counted too
		if (flatVoxels.voxelCount >= m_numVoxels)
		{
			//still requires a physics update
			
			return models;
		}
		
		Model* model;
		//extract other parts
		while(true){
			model = new Model();
			bool ret = flatVoxels.extractModel([&](const ComputeFlatArray::VoxelMark& _mark)
			{
				model->Set(_mark.position, *_mark.voxel);
				this->Set(_mark.position, ComponentType::UNDEFINED);
			});
			if (!ret)
			{
				delete model;
				break;
			}
			models.push_back(model);
		}

		float angularVelLen = len(m_angularVelocity);
		//the main model needs a physics update as well
		models.push_back(this);
		for (auto mod : models)
		{
			// update properties in this order
			// UpdateInertialTensor() depends on UpdateCenter() depends on SetRotation()
			mod->SetPosition(m_position);
			mod->SetRotation(m_rotation);
			// the center of mass changes but position in the world should not
			Vec3 shift = mod->m_center - center;
			mod->UpdateCenter(shift);
			mod->UpdateInertialTensor();
			// calculate movement of the pieces
			mod->AddVelocity(m_velocity - mod->GetVelocity());
			mod->AddVelocity(cross(GetAngularVelocity(), shift));
			//simulate some explosion in the center of mass, only to make some visible change
		//	mod->AddVelocity(Vec3(10.f, 0.f, 0.f));
		//	AddVelocity(m_rotationMatrix * shift * 0.3f);
			// this is probably not correct
			float l = len(shift);
			if (angularVelLen * l != 0.f)
			{
				float angle = acos(ei::dot(m_angularVelocity, shift) / (angularVelLen * l));
				angle /= PI * 0.5f;
				angle = angle < 1.f ? 1.f - angle : (angle - floor(angle));
				mod->m_angularVelocity = m_angularVelocity * angle;
				Assert(angle >= 0.f, "What???");
			}
			else mod->m_angularVelocity = Vec3(0.f);
		}
		models.pop_back(); // remove this again

		return std::move(models);
	}
};