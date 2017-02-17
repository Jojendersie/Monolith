#include "../math/math.hpp"
#include "scenegraph.hpp"

struct FireRayInfo
{
	FireRayInfo(const Math::WorldRay& _ray, int _damage, float _range) :
		ray(_ray),
		damage(_damage),
		range(_range)
	{}
	Math::WorldRay ray;
	float range;
	int damage;
};

class FireManager
{
public:
	FireManager(SceneGraph& _graph);

	/// \brief Processes a ray shot in the scene
	/// \return distance of the hit
	float FireRay(const FireRayInfo& _info);

	/// \brief Spawns a projectile in the world
	void FireProjectile(const FireRayInfo& _info);

	void Process(float _deltaTime);
private:
	SceneGraph& m_sceneGraph;

	std::vector<FireRayInfo> m_fireInfos;
};

extern FireManager* g_fireManager;