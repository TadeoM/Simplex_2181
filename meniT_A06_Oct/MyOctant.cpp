#include "MyOctant.h"

using namespace Simplex;

void MyOctant::Init()
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	std::vector<MyEntity*> l_EntityList = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_EntityList.size();
	std::vector<vector3> v3MaxMin_List;
	for (uint i = 0; i < iEntityCount; i++)
	{
		MyRigidBody* pRG = l_EntityList[i]->GetRigidBody();
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
}

void MyOctant::Swap(MyOctant& other)
{

}

MyOctant::MyOctant(uint a_nMaxLevel = 2, uint a_nIdealEntityCount = 5)
{
	// when does m_pMeshMngr get initialized
	// same for EntityMngr
	//
	// what is a leaf?

	if (m_uOctantCount == 0)
	{
		m_pRoot = this;
	}

	Init();
	m_uOctantCount++;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	m_uID = 0;
	m_uLevel = 0;
	m_uChildren = 0;
	m_fSize = a_fSize;
	m_v3Center = a_v3Center;
	m_v3Min = vector3(-a_fSize / 2);
	m_v3Max = vector3(a_fSize);
	m_pParent;
	
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
}

MyOctant::~MyOctant() { Release(); }

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
	std::vector<MyEntity*> l_EntityList = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_EntityList.size();
	for (uint i = 0; i < iEntityCount; i++)
	{
		MyRigidBody* pRB = l_EntityList[i]->GetRigidBody();
		if (pRB->IsColliding(pRigidBody))
		{
			l_EntityList[i]->AddDimension(m_uID);
		}
	}
}

void MyOctant::Display(vector3 a_v3Color = C_YELLOW)
{
	pRigidBody->AddToRenderList();
	//m_pMeshMngr->AddWireCubeToRenderList(glm::scale(vector3(70)), C_BLUE);
}

void MyOctant::DisplayLeafs(vector3 a_v3Color = C_YELLOW)
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

void MyOctant::Subdivide()
{
	// subdivide about the each axis by 3
	// they should each know who their parents are
	// this will be an infinite loop if called in constructor, we must check if the object is the root
	for (GLuint i = 0; i < 8; i++)
	{
		m_pChild[i] = new MyOctant();
	}
}

MyOctant* MyOctant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
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

void MyOctant::ConstructTree(uint a_MaxLevel = 3)
{

}

uint MyOctant::GetOctantCount()
{
	return m_uOctantCount;
}

void MyOctant::AssignIDtoEntity()
{

}

void MyOctant::Release()
{
	SafeDelete(m_pParent);
	for each (MyOctant* child in m_pChild)
	{
		SafeDelete(child);
	}
	for each (MyOctant* child in m_lChild)
	{
		SafeDelete(child);
	}
	SafeDelete(m_pRoot);
}

void MyOctant::ConstructList()
{

}