#include "MyOctant.h"

using namespace Simplex;

uint MyOctant::m_uOctantCount;
uint MyOctant::m_uMaxLevel;
uint MyOctant::m_uIdealEntityCount;
uint MyOctant::currentMaxLevel;
uint MyOctant::numCreated;
uint MyOctant::uintlayers;
uint MyOctant::uIntTotalOcts;

void MyOctant::Init()
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
}

MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();

	// get list of entities
	MyEntity** l_EntityList = m_pEntityMngr->GetEntityList();
	uint iEntityCount = m_pEntityMngr->GetEntityCount();
	std::vector<vector3> v3MaxMin_List;
	// get max values based on what entities the entity manager has
	for (uint i = 0; i < iEntityCount; i++)
	{
		MyRigidBody* pRG = l_EntityList[i]->GetRigidBody();
		m_EntityList.push_back(i);
		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();
		v3MaxMin_List.push_back(v3Min);
		v3MaxMin_List.push_back(v3Max);
	}

	for (GLuint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
	pRigidBody = new MyRigidBody(v3MaxMin_List);

	m_fSize = glm::length(pRigidBody->GetHalfWidth()) + 10;

	m_pRoot = this;
	m_uOctantCount = 1;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	currentMaxLevel = 0;
	m_uLevel = 0;
	m_uChildren = 0;
	for (GLuint i = 0; i < m_uMaxLevel; i++)
	{
		Subdivide();
	}
	Add();
	pRigidBody->MakeCube();
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	std::vector<vector3> v3MaxMin_List;
	v3MaxMin_List.push_back(a_v3Center - vector3(a_fSize));
	v3MaxMin_List.push_back(a_v3Center + vector3(a_fSize));
	pRigidBody = new MyRigidBody(v3MaxMin_List);
	
	m_uOctantCount++;
	m_uID = m_uOctantCount;
	
	m_uChildren = 0;
	m_fSize = a_fSize;
	m_v3Center = a_v3Center;
	m_v3Min = vector3(m_v3Center) + vector3(-a_fSize);
	m_v3Max = vector3(m_v3Center) + vector3(a_fSize);
}

MyOctant::MyOctant(MyOctant const& other)
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;
	m_fSize = other.m_fSize;
	m_pMeshMngr = other.m_pMeshMngr;
	m_pEntityMngr = other.m_pEntityMngr;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	*m_pParent = *other.m_pParent;
	for (GLuint i = 0; i < 8; i++)
	{
		*m_pChild[i] = *other.m_pChild[i];
	}
	m_EntityList = other.m_EntityList;
	*m_pRoot = *other.m_pRoot;
	m_lChild = other.m_lChild;
}

MyOctant& MyOctant::operator=(MyOctant const& other)
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;
	m_fSize = other.m_fSize;
	m_pMeshMngr = other.m_pMeshMngr;
	m_pEntityMngr = other.m_pEntityMngr;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	*m_pParent = *other.m_pParent;
	for (GLuint i = 0; i < 8; i++)
	{
		*m_pChild[i] = *other.m_pChild[i];
	}
	m_EntityList = other.m_EntityList;
	*m_pRoot = *other.m_pRoot;
	m_lChild = other.m_lChild;

	return (MyOctant)other;
}

MyOctant::~MyOctant() 
{ 
	if (this != nullptr && !IsLeaf())
	{
		KillBranches();
	}

	currentMaxLevel = 0;
	m_uOctantCount = 0;
}

float MyOctant::GetSize()
{
	return m_fSize;
}

vector3 MyOctant::GetCenterGlobal()
{
	return m_v3Center;
}

vector3 MyOctant::GetMinGlobal()
{
	return m_v3Min;
}

vector3 MyOctant::GetMaxGlobal()
{
	return m_v3Max;
}

bool MyOctant::IsColliding(uint a_uRBIndex)
{
	if (pRigidBody->IsColliding(m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody()))
	{
		//std::cout << "HELLOOOOO" << std::endl;
		return true;
	}
	
	return false;
}
void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
}

void MyOctant::Display(vector3 a_v3Color)
{
	//m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center) * glm::scale(vector3(m_fSize)), C_BLUE);
	pRigidBody->AddToRenderList();
	if (m_pChild[0])
	{
		for (GLuint i = 0; i < 8; i++)
		{
			m_pChild[i]->Display(C_BLUE);
		}
	}
}

void MyOctant::Subdivide()
{
	// subdivide about the each axis by 3
	// they should each know who their parents are
	// this will be an infinite loop if called in constructor, we must check if the object is the root
	if (IsLeaf()) // == m_pChildren[0] == nullptr
	{
		vector3 v3Center = pRigidBody->GetCenterLocal();
		vector3 v3CenterPoints[8];
		vector3 v3HalfWidth = pRigidBody->GetHalfWidth();
		float fSize = (v3HalfWidth.x) / 2.0f;
		float fCenters = fSize;

		v3CenterPoints[0] = vector3(v3Center + vector3(fCenters, fCenters, fCenters));
		v3CenterPoints[1] = vector3(v3Center + vector3(-fCenters, fCenters, fCenters));
		v3CenterPoints[2] = vector3(v3Center + vector3(-fCenters, -fCenters, fCenters));
		v3CenterPoints[3] = vector3(v3Center + vector3(fCenters, -fCenters, fCenters));

		v3CenterPoints[4] = vector3(v3Center + vector3(fCenters, fCenters, -fCenters));
		v3CenterPoints[5] = vector3(v3Center + vector3(-fCenters, fCenters, -fCenters));
		v3CenterPoints[6] = vector3(v3Center + vector3(-fCenters, -fCenters, -fCenters));
		v3CenterPoints[7] = vector3(v3Center + vector3(fCenters, -fCenters, -fCenters));

		m_uChildren = 8;

		for (int i = 0; i < 8; i++)
		{
			m_pChild[i] = new MyOctant(v3CenterPoints[i], fSize);
			m_pChild[i]->m_pParent = this;
			m_pChild[i]->SetParent();
			std::cout << m_pChild[i]->m_uID << std::endl;
		}
	}
	else
	{
		for (GLuint i = 0; i < 8; i++)
		{
			m_pChild[i]->Subdivide();
		}

	}
}

MyOctant* MyOctant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
}

void MyOctant::SetParent()
{
	m_pRoot = m_pParent->m_pRoot;
	m_uLevel = m_pParent->m_uLevel + 1;
}

MyOctant* MyOctant::GetParent()
{
	return m_pParent;
}

bool MyOctant::IsLeaf()
{
	if (m_pChild[0] == nullptr) return true;
	else return false;
}

void MyOctant::Add() 
{
	if (!IsLeaf()) // if m_pChild[0] != nullptr
	{
		for (GLuint i = 0; i < 8; i++)
		{
			m_pChild[i]->Add();
		}
	}
	else
	{
		m_pEntityMngr = MyEntityManager::GetInstance();
		uint iEntityCount = m_pEntityMngr->GetEntityCount();

		for (GLuint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
		{
			MyRigidBody* pRG = m_pEntityMngr->GetEntity(i)->GetRigidBody();
			if (pRG->IsColliding(pRigidBody))
			{
				m_pEntityMngr->GetEntity(i)->AddDimension(m_uID);
			}
		}
	}
}

bool MyOctant::ContainsMoreThan(uint a_Entities)
{
	if (m_EntityList.size() >= a_Entities) return true;
	else return false;
}

void MyOctant::KillBranches()
{
	for each(MyOctant* child in m_pChild)
	{
		if (!child->IsLeaf()) { child->KillBranches(); }
		else { SafeDelete(child); }
	}
}

void MyOctant::ConstructTree(uint a_MaxLevel)
{

}

void MyOctant::Swap(MyOctant& other)
{
	MyOctant tempOctant = MyOctant(other);

	// copy this Octant into the other
	other.m_uID = m_uID;
	other.m_uLevel = m_uLevel;
	other.m_uChildren = m_uChildren;
	other.m_fSize = m_fSize;
	other.m_pMeshMngr = m_pMeshMngr;
	other.m_pEntityMngr = m_pEntityMngr;
	other.m_v3Center = m_v3Center;
	other.m_v3Min = m_v3Min;
	other.m_v3Max = m_v3Max;
	*other.m_pParent = *m_pParent;

	for (GLuint i = 0; i < 8; i++)
	{
		*other.m_pChild[i] = *m_pChild[i];
	}
	other.m_EntityList = m_EntityList;
	*other.m_pRoot = *m_pRoot;
	other.m_lChild = m_lChild;

	// copy the tempOctant
	m_uID = tempOctant.m_uID;
	m_uLevel = tempOctant.m_uLevel;
	m_uChildren = tempOctant.m_uChildren;
	m_fSize = tempOctant.m_fSize;
	m_pMeshMngr = tempOctant.m_pMeshMngr;
	m_pEntityMngr = tempOctant.m_pEntityMngr;

	m_v3Center = tempOctant.m_v3Center;
	m_v3Min = tempOctant.m_v3Min;
	m_v3Max = tempOctant.m_v3Max;

	*m_pParent = *tempOctant.m_pParent;
	for (GLuint i = 0; i < 8; i++)
	{
		*m_pChild[i] = *tempOctant.m_pChild[i];
	}
	m_EntityList = tempOctant.m_EntityList;
	*m_pRoot = *tempOctant.m_pRoot;
	m_lChild = tempOctant.m_lChild;
}

void MyOctant::DisplayLeafs(vector3 a_v3Color)
{

}

void MyOctant::ClearEntityList()
{
	for each(MyOctant* child in m_pChild)
	{
		SafeDelete(child);
	}
	m_pChild[8];
}

uint MyOctant::GetOctantCount()
{
	return m_uOctantCount;
}

void MyOctant::AssignIDtoEntity()
{
	if (m_pParent != nullptr)
	{

	}
	else
		m_uID = 0;
}

void MyOctant::Release()
{
	SafeDelete(m_pParent);
	for each (MyOctant* child in m_pChild)
	{
		SafeDelete(child);
	}
	SafeDelete(m_pRoot);
}

void MyOctant::ConstructList()
{

}