#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	m_uEntityCount = 0;
	m_entityList.clear();
}
void MyEntityManager::Release(void)
{
	for (int i = 0; i < m_uEntityCount; i++)
	{
		MyEntity* pEntity = m_entityList[i];
		SafeDelete(pEntity);
	}
	m_uEntityCount = 0;
	m_entityList.clear();
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
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (a_sUniqueID == m_entityList[i]->GetUniqueID())
			return i;
	}
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	if (m_entityList.size() == 0) return nullptr;

	if (a_uIndex >= m_uEntityCount) a_uIndex = m_uEntityCount - 1;

	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	MyEntity* someEntity = MyEntity::GetEntity(a_sUniqueID);
	if (someEntity)
	{
		return someEntity->GetModel();
	}
	else return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	if (m_entityList.size() == 0) return nullptr;

	if (a_uIndex >= m_uEntityCount) a_uIndex = m_uEntityCount - 1;

	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	MyEntity* someEntity = MyEntity::GetEntity(a_sUniqueID);
	if (someEntity)
	{
		return someEntity->GetRigidBody();
	}
	else return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	if (m_entityList.size() == 0) return IDENTITY_M4;

	if (a_uIndex >= m_uEntityCount) a_uIndex = m_uEntityCount - 1;

	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	MyEntity* someEntity = MyEntity::GetEntity(a_sUniqueID);
	if (someEntity)
	{
		return someEntity->GetModelMatrix();
	}
	else return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	MyEntity* someEntity = MyEntity::GetEntity(a_sUniqueID);
	if (someEntity)
	{
		someEntity->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	if (m_entityList.size() == 0) return;

	if (a_uIndex >= m_uEntityCount) a_uIndex = m_uEntityCount - 1;

	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	for (int i = 0; i < m_uEntityCount - 1; i++)
	{
		for (int j = i + 1; j < m_uEntityCount; j++)
		{
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	auto newEntity = new MyEntity(a_sFileName, a_sUniqueID);
	m_entityList.push_back(newEntity);
	m_uEntityCount++;
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	int count = 0;
	for (auto i = m_entityList.begin(); i < m_entityList.end(); i++)
	{
		if (count == a_uIndex) {
			m_entityList.erase(i);
			m_uEntityCount--;
			return;
		}
		count++;
	}
	
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	int index = GetEntityIndex(a_sUniqueID);
	RemoveEntity(index);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	if (m_entityList.size() == 0) return "";

	if (a_uIndex >= m_entityList.size()) a_uIndex = m_entityList.size() - 1;

	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	if (m_entityList.size() == 0) return nullptr;

	if (a_uIndex >= m_entityList.size()) a_uIndex = m_entityList.size() - 1;

	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	if (a_uIndex >= m_uEntityCount)
	{
		for (a_uIndex = 0; a_uIndex < m_uEntityCount; a_uIndex++)
		{
			m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
		}
	}
	else
	{
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	MyEntity* someEntity = MyEntity::GetEntity(a_sUniqueID);
	if (someEntity)
	{
		someEntity->AddToRenderList(a_bRigidBody);
	}
}