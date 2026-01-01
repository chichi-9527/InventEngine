#include "IEpch.h"
#include "IMesh.h"

#include "IComponent/InventComponent.h"

namespace INVENT
{
	IMesh::IMesh()
		: IObjectBase()
	{
		this->AddComponent<MeshComponent>().PreExpansion((size_t)2 << 11);
	}

	IMesh::~IMesh()
	{
		this->RemoveComponent<MeshComponent>();
	}

	MeshComponent* IMesh::GetMesh()
	{
		return this->GetComponent<MeshComponent>();
	}
}
