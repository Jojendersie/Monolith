#pragma once

#include "gamestatebase.hpp"
#include "math/vector.hpp"
#include "utilities/scopedpointer.hpp"
#include <hybridarray.hpp>

/// \brief Game state to edit voxel models.
class GSEditor: public IGameState
{
public:
	/// \brief Create main state specific content.
	GSEditor(Monolith* _game);
	~GSEditor();

	void OnBegin();
	virtual void OnEnd() override;

	virtual void Simulate( double _deltaTime ) override;
	virtual void Render( double _deltaTime ) override;
	virtual void MouseMove( double _dx, double _dy ) override;
	virtual void Scroll( double _dx, double _dy ) override;
	virtual void KeyDown( int _key, int _modifiers ) override;
	virtual void KeyRelease(int _key) override;
	virtual void KeyClick( int _key ) override;
	virtual void KeyDoubleClick( int _key ) override;
private:
	Graphic::Hud* m_hud;
	Input::Camera* m_modelCamera;		///< Standard camera for the model view
	Graphic::Marker::Box* m_redBox;		///< Marker for regions that can be deleted, or for invalid positions
	Graphic::Marker::Box* m_greenBox;	///< Marker where the new voxel would be added
	//ScopedPtr<Graphic::Marker::SphericalFunction> m_thrustFunction;	///< Visualization for the maximum engine thrust

	ScopedPtr<Voxel::Model> m_model;	///< The model which is currently edited
	std::mutex m_criticalModelWork;		///< Locked if a model is replaced or similar
	Jo::HybridArray<ScopedPtr<Voxel::Model>> m_deleteList;	///< Deletion must be done by the one who has the context

	Voxel::VoxelType m_currentType;		///< The type of the voxel which is painted
	bool m_rayHits;						///< The hit information is up to date and filled
	bool m_deletionMode;				///< Deletion mode for the selected voxel
	bool m_validPosition;				///< False if the target voxel cannot be edited (coordinates or other conditions)
	Math::IVec3 m_lvl0Position;			///< Information about the current target voxel

	/// \brief Method to create a new model as copy or from scratch.
	/// TODO: Pool for object templates
	/// \details The new model is not saved automatically. If leaving the
	///		editor there is a request to do that.
	/// \param [in] _copyFrom A model which should be copied to be edited. If
	///		this is nullptr a new model with one computer core is created.
	void CreateNewModel( const Voxel::Model* _copyFrom = nullptr );

	/// \brief Check if the target position can be edited.
	/// \details The result is saved in m_validPosition;
	void ValidatePosition();
};