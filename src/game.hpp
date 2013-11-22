#pragma once

#include <chrono>

#include "graphic/uniformbuffer.hpp"
#include "graphic/effect.hpp"
#include "graphic/samplerstate.hpp"
#include "predeclarations.hpp"
#include <jofilelib.hpp>


/// \brief A game state is an instance with a number of handling methods which
///		are called by the main loop.
class IGameState
{
public:
	virtual ~IGameState() {}

	/// \brief Update the movement, damages... of all objects
	/// \param [in] _time Total time since game start in seconds.
	/// \param [in] _deltaTime Time since last Update call.
	virtual void Update( double _time, double _deltaTime ) = 0;

	/// \brief Render one frame.
	/// \param [in] _time Total time since game start in seconds.
	/// \param [in] _deltaTime Time since last Render call.
	virtual void Render( double _time, double _deltaTime ) = 0;

	// TODO: more Input

	/// \brief The input update is called synchronized with the renderer.
	/// \details This method must recompute camera matrices and other things
	///		which relay on input but are used in rendering too.
	virtual void UpdateInput() = 0;

	/// \brief React to mouse move input.
	/// \details Do not update anything accessed by the renderer. Try to
	///		deferr them till UpdateInput().
	/// \param [in] _dx Delta x. Change of position in x direction
	virtual void MouseMove( double _dx, double _dy ) = 0;

	/// \brief handle vertical and horizontal scroll events.
	virtual void Scroll( double _dx, double _dy ) {}

	Monolith* m_parent;
};

typedef IGameState* IGameStateP;

/// \brief The game consists of a main-loop and several different game-states.
class Monolith
{
public:
	/// \brief Create a single threaded variant with limited frame rate.
	Monolith( float _targetFrameRate );

	/// \brief Create a multi threaded variant with a limited frame rate for
	///		each thread.
	Monolith( float _targetRenderFR, float _targetUpdateFR, float _targetSoundFR );

	/// \brief Delete all created resources
	~Monolith();

	/// \brief Start the game. This method will return after the game closed.
	void Run();

	/// \brief Global content like fonts and render effects
	struct _Content {
		_Content();
		~_Content();

		Graphic::Effect voxelRenderEffect;
		Graphic::UniformBuffer objectUBO;
		Graphic::UniformBuffer cameraUBO;
		Graphic::SamplerState pointSampler;
		Graphic::SamplerState linearSampler;

		Graphic::Texture* voxelTextures;
	} content;

	Jo::Files::MetaFileWrapper Config;
private:
	IGameStateP m_gameStates[1];	///< MainMenu, NewGame, Main, 
	int m_currentGameState;
	bool m_singleThreaded;
	bool m_running;

	double m_time;					///< Total time since run in seconds
	std::chrono::microseconds m_microSecPerFrame;

	void BuildDefaultConfig();
};

/// \brief Game state for the game itself.
class GSMain: public IGameState
{
public:
	/// \brief Create main state specific content.
	GSMain();
	~GSMain();

	virtual void Update( double _time, double _deltaTime ) override;
	virtual void Render( double _time, double _deltaTime ) override;
	virtual void UpdateInput() override;
	virtual void MouseMove( double _dx, double _dy ) override;
	virtual void Scroll( double _dx, double _dy ) override;
private:
	Generators::Asteroid* m_astTest;
	Graphic::Font* m_fontTest;
	Graphic::Texture* m_textures;
	Input::Camera* m_camera;
};