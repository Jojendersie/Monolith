#pragma once

/// \brief A game state is an instance with a number of handling methods which
///		are called by the main loop.
class IGameState
{
public:
	/// \brief Update the movement, damages... of all objects
	/// \param [in] _fTime Total time since game start in seconds.
	/// \param [in] _fDeltaTime Time since last Update call.
	virtual void Update( double _fTime, double _fDeltaTime ) = 0;

	/// \brief Render one frame.
	/// \param [in] _fTime Total time since game start in seconds.
	/// \param [in] _fDeltaTime Time since last Render call.
	virtual void Render( double _fTime, double _fDeltaTime ) = 0;

	// TODO: Input
};

typedef IGameState* IGameStateP;

/// \brief The game consists of a main-loop and several different game-states.
class Monolith
{
public:
	/// \brief Create a single threaded variant with limited frame rate.
	Monolith( float _fTargetFrameRate );

	/// \brief Create a multi threaded variant with a limited frame rate for
	///		each thread.
	Monolith( float _fTargetRenderFR, float _fTargetUpdateFR, float _fTargetSoundFR );

	/// \brief Delete all created resources
	~Monolith();

	/// \brief Start the game. This method will return after the game closed.
	void Run();

private:
	IGameStateP m_ppGameStates[1];	///< MainMenu, NewGame, Main, 
	int m_iCurrentGameState;
	bool m_bSingleThreaded;
	bool m_bRunning;

	double m_fTime;					///< Total time since run in seconds
	double m_fTargetRenderFRInv;
};

/// \brief Game state for the game itself.
class GSMain: public IGameState
{
public:
	virtual void Update( double _fTime, double _fDeltaTime );
	virtual void Render( double _fTime, double _fDeltaTime );
};