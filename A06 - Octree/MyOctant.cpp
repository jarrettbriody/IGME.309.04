#include "MyOctant.h"

using namespace Simplex;

static uint m_uOctantCount = 0;
static MyOctant* root = nullptr;
static uint m_uMaxLevel = 0;
static uint m_uIdealEntityCount = 0;

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	m_uMaxLevel = a_nMaxLevel;
	m_uLevel = 0;
	m_uIdealEntityCount = a_nIdealEntityCount;
	root = this;
	m_pRoot = this;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();
	float maxSize = 0.0f;
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		vector3 currentMax = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMaxGlobal();
		vector3 currentMin = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMinGlobal();
		if (glm::abs(currentMax.x) > maxSize) maxSize = glm::abs(currentMax.x);
		if (glm::abs(currentMax.y) > maxSize) maxSize = glm::abs(currentMax.y);
		if (glm::abs(currentMax.z) > maxSize) maxSize = glm::abs(currentMax.z);

		if (glm::abs(currentMin.x) > maxSize) maxSize = glm::abs(currentMin.x);
		if (glm::abs(currentMin.y) > maxSize) maxSize = glm::abs(currentMin.y);
		if (glm::abs(currentMin.z) > maxSize) maxSize = glm::abs(currentMin.z);

		m_EntityList.push_back(i);
	}
	m_fSize = maxSize;
	m_v3Center = vector3(0.0f, 0.0f, 0.0f);
	m_v3Min = m_v3Center + vector3(-m_fSize, -m_fSize, -m_fSize);
	m_v3Max = m_v3Center + vector3(m_fSize, m_fSize, m_fSize);

	m_uOctantCount = 1;
	m_uID = m_uOctantCount;
	ConstructTree(m_uMaxLevel);
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
	m_v3Max = m_v3Center + vector3(m_fSize, m_fSize, m_fSize);
	m_v3Min = m_v3Center + vector3(-m_fSize, -m_fSize, -m_fSize);
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();
	m_uOctantCount++;
	m_uID = m_uOctantCount;
	//std::cout << m_fSize << std::endl << m_v3Center.x << " " << m_v3Center.y << " " << m_v3Center.z << std::endl << std::endl;
	//std::cout << m_uOctantCount << std::endl;
}

Simplex::MyOctant::~MyOctant(void)
{
	if (!this->IsLeaf()) {
		for (int i = 0; i < 8; i++)
		{
			delete m_pChild[i];
		}
	}
}

float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	MyRigidBody* other = m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody();
	bool bColliding = true;
	
	if (this->m_v3Max.x < other->GetMinGlobal().x) //this to the right of other
		bColliding = false;
	if (this->m_v3Min.x > other->GetMaxGlobal().x) //this to the left of other
		bColliding = false;

	if (this->m_v3Max.y < other->GetMinGlobal().y) //this below of other
		bColliding = false;
	if (this->m_v3Min.y > other->GetMaxGlobal().y) //this above of other
		bColliding = false;

	if (this->m_v3Max.z < other->GetMinGlobal().z) //this behind of other
		bColliding = false;
	if (this->m_v3Min.z > other->GetMaxGlobal().z) //this in front of other
		bColliding = false;

	if (bColliding) {
		m_pEntityMngr->GetEntity(a_uRBIndex)->AddDimension(m_uID);
	}

	return bColliding;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	m_pMeshMngr->GenerateWireCube(m_fSize, a_v3Color);
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(this->m_v3Center), a_v3Color);
	if (!IsLeaf()) {
		for (int i = 0; i < 8; i++)
		{
			this->m_pChild[i]->Display(a_v3Color);
		}
	}
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
}

void Simplex::MyOctant::ClearEntityList(void)
{
}

void Simplex::MyOctant::Subdivide(void)
{
	float newSize = m_fSize/2;
	m_pChild[0] = new MyOctant(m_v3Center + vector3(newSize, newSize, newSize), newSize);
	m_pChild[1] = new MyOctant(m_v3Center + vector3(newSize, newSize, -newSize), newSize);
	m_pChild[2] = new MyOctant(m_v3Center + vector3(-newSize, newSize, -newSize), newSize);
	m_pChild[3] = new MyOctant(m_v3Center + vector3(-newSize, newSize, newSize), newSize);
	m_pChild[4] = new MyOctant(m_v3Center + vector3(-newSize, -newSize, newSize), newSize);
	m_pChild[5] = new MyOctant(m_v3Center + vector3(newSize, -newSize, newSize), newSize);
	m_pChild[6] = new MyOctant(m_v3Center + vector3(newSize, -newSize, -newSize), newSize);
	m_pChild[7] = new MyOctant(m_v3Center + vector3(-newSize, -newSize, -newSize), newSize);
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i]->m_pParent = this;
		//std::cout << m_pChild[i]->m_v3Center.x << " " << m_pChild[i]->m_v3Center.y << " " << m_pChild[i]->m_v3Center.z << std::endl;
	}
	m_uChildren = 8;
}

MyOctant* Simplex::MyOctant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
}

MyOctant* Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	return m_uChildren == 0;
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	if (this->m_uLevel >= a_nMaxLevel) {
		//std::cout << "level exceed limit" << std::endl;
		for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
		{
			if (this->IsColliding(i)) {
				root->m_lChild.push_back(this);
				this->m_EntityList.push_back(i);
				//std::cout << "collision " << i << std::endl;
			}
		}
		return;
	}
	else {
		//std::cout << "subdividing" << std::endl;
		//std::cout << "Current: " << this->m_uLevel << std::endl << "Max    : " << a_nMaxLevel << std::endl;
		this->Subdivide();
		for (int i = 0; i < 8; i++)
		{
			MyOctant* oneChild = this->GetChild(i);
			oneChild->m_uLevel = this->m_uLevel + 1;
			oneChild->ConstructTree(a_nMaxLevel);
		}
	}
}

std::vector<MyOctant*> Simplex::MyOctant::GetPopulatedLeaves(void)
{
	return m_lChild;
}

std::vector<uint> Simplex::MyOctant::GetEntityIndices(void)
{
	return m_EntityList;
}
