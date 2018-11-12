#include "Simplex\Physics\Octant.h"

using namespace Simplex;

void Octant::Init()
{
	//m_EntityList = m_pEntityMngr.entity
}

void Octant::Swap(Octant& other)
{

}

Octant::Octant(uint a_nMaxLevel = 2, uint a_nIdealEntityCount = 5)
{
	// when does m_pMeshMngr get initialized
	// same for EntityMngr
	//
	// what is a leaf?

	if (m_uOctantCount == 0)
	{
		m_pRoot = this;
	}

	Octant::Init();
	m_uOctantCount++;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

}

Octant::Octant(vector3 a_v3Center, float a_fSize)
{

}

Octant::Octant(Octant const& other)
{

}

Octant& Octant::operator=(Octant const& other)
{

}

Octant::~Octant() { Release(); }

float Octant::GetSize()
{
	return m_fSize;
}

vector3 Octant::GetCenterGlobal()
{
	return m_v3Center;
}

vector3 Octant::GetMinGlobal()
{
	return m_v3Min;
}

vector3 Octant::GetMaxGlobal()
{
	return m_v3Max;
}

bool Octant::IsColliding(uint a_uRBIndex)
{

}

void Octant::Display(vector3 a_v3Color = C_YELLOW)
{

}

void Octant::DisplayLeafs(vector3 a_v3Color = C_YELLOW)
{

}

void Octant::ClearEntityList()
{
	for each(Octant* child in m_pChild)
	{
		SafeDelete(child);
	}
	m_pChild[8];
}

void Octant::Subdivide()
{
	
}

Octant* Octant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
}

Octant* Octant::GetParent()
{
	return m_pParent;
}

bool Octant::IsLeaf()
{
	if (m_pChild[0] == nullptr) return true;
	else return false;
}

bool Octant::ContainsMoreThan(uint a_Entities)
{
	if (m_EntityList.size() >= a_Entities) return true;
	else return false;
}

void Octant::KillBranches()
{
	for each(Octant* child in m_pChild)
	{
		if (!child->IsLeaf()) { KillBranches(); }
		else { SafeDelete(child); }
	}
}

void Octant::ConstructTree(uint a_MaxLevel = 3)
{

}

uint Octant::GetOctantCount()
{
	return m_uOctantCount;
}

void Octant::AssignIDtoEntity()
{

}

void Octant::Release()
{
	SafeDelete(m_pParent);
	for each (Octant* child in m_pChild)
	{
		SafeDelete(child);
	}
	for each (Octant* child in m_lChild)
	{
		SafeDelete(child);
	}
	SafeDelete(m_pRoot);
}

void Octant::ConstructList()
{

}