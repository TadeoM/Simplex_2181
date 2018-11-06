#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
}
void MyEntityManager::Release(void)
{
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	// return
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	// check if we have something at the index a_uIndex, if we do, return its model, otherwise, return nullptr
	if (m_entityList[a_uIndex]->GetModel() != nullptr)
		return m_entityList[a_uIndex]->GetModel();

	return nullptr;
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{

	for (int i = 0; i < m_entityList.size(); i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetModel();
		}
	}
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	// check if we have something at the index a_uIndex, if we do, return its model, otherwise, return nullptr
	if (m_entityList[a_uIndex]->GetRigidBody() != nullptr)
		return m_entityList[a_uIndex]->GetRigidBody();

	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	for (int i = 0; i < m_entityList.size(); i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetRigidBody();
		}
	}

	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	// check if we have something at the index a_uIndex, if we do, return its model, otherwise, return nullptr
	if (m_entityList[a_uIndex]->GetModel() != nullptr)
		return m_entityList[a_uIndex]->GetModelMatrix();

	return IDENTITY_M4;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	// pass in the a_m4ToWorld into the object that has the specified unique ID, using a for loop
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	// pass in the a_m4ToWorld for the object that has the specified index
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{

}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	MyEntity* newEntity = new MyEntity(a_sFileName, a_sUniqueID);

	m_entityList.push_back(newEntity);
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	// find the entity in the index
	// remove the entity at the specified index
	// delete the reference to the entity
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	// find the entity in the index with a for loop
	// remove the entity at the specified index
	// delete the reference to the entity
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	return "";
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	return nullptr;
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	// call the addToRenderList of the entity at a_uIndex
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	// loop through entity list until we find the entity with the correct unique ID
	// call the addToRenderList of the entity with the specified uniqueID
}