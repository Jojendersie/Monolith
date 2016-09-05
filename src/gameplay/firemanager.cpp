#include "firemanager.hpp"
#include "../voxel/voxel.hpp"

FireManager* g_fireManager;

FireManager::FireManager(SceneGraph& _graph)
	:m_sceneGraph(_graph)
{
}

void FireManager::FireRay(const FireRayInfo& _info)
{
	m_fireInfos.push_back(_info);
}

void FireManager::Process(float _deltaTime)
{
	for (auto info : m_fireInfos)
	{
		Voxel::Model::ModelData::HitResult hit;


		auto hitObj = m_sceneGraph.RayQuery(info.ray, hit);
		if (hitObj){
			Voxel::Model* model = static_cast<Voxel::Model*>(&hitObj);
			model->Set( hit.position, Voxel::ComponentType::UNDEFINED );
		}
	}

	m_fireInfos.clear();
}