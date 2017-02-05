#include "firemanager.hpp"
#include "../voxel/voxel.hpp"

FireManager* g_fireManager;

FireManager::FireManager(SceneGraph& _graph)
	:m_sceneGraph(_graph)
{
}

float FireManager::FireRay(const FireRayInfo& _info)
{
//	m_fireInfos.push_back(_info);
	Voxel::Model::ModelData::HitResult hit;


	auto hitObj = m_sceneGraph.RayQuery(_info.ray, hit, _info.range);
	if (hitObj){
		Voxel::Model* model = static_cast<Voxel::Model*>(&hitObj);
		model->Damage(hit.position, (uint32_t)_info.damage);

		Math::FixVec3 pos = model->GetPosition() + Math::FixVec3(model->GetRotationMatrix() * (hit.position + 0.5f - model->GetCenter()));

		return ei::len(ei::Vec3(_info.ray.origin - pos));
	}

	return 100.f; // max length
}

void FireManager::Process(float _deltaTime)
{
}