#include "../math/math.hpp"
#include "scenegraph.hpp"

struct FireRayInfo
{
	FireRayInfo(const Math::WorldRay& _ray, float _dam) :
		ray(_ray),
		damage(_dam)
	{}
	Math::WorldRay ray;

	float damage;
};

class FireManager
{
public:
	FireManager(SceneGraph& _graph);

	/// \brief Processes a ray shot in the scene
	/// \return distance of the hit
	float FireRay(const FireRayInfo& _info);

	void Process(float _deltaTime);
private:
	SceneGraph& m_sceneGraph;

	std::vector<FireRayInfo> m_fireInfos;
};

extern FireManager* g_fireManager;