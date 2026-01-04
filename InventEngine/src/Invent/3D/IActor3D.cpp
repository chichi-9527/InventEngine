#include "IEpch.h"
#include "IActor3D.h"

#include "ITools/ILoadModel.h"

#include "IComponent/InventComponent.h"

namespace INVENT
{
	IActor3D::IActor3D()
		: IActor()
		, _model_matrice(1.0f)
	{
		meshes.clear();
		this->AddComponent<WorldRotationComponent>(glm::vec3(0.0f));
		this->AddComponent<ScaleComponent>(glm::vec3(1.0f));
	}

	IActor3D::~IActor3D()
	{}

	void IActor3D::SetWorldPosition(const glm::vec3& position)
	{
		IActor::SetWorldPosition(position);
		RecalculateModelMatrix();
	}

	void IActor3D::LoadModel(const std::string& path)
	{
		ILoadModel::Load(this->meshes, path);
	}

	void IActor3D::SetWorldRotation(const glm::vec3& rotation)
	{
		this->GetComponent<WorldRotationComponent>()->Rotation = rotation;
		RecalculateModelMatrix();
	}
	const glm::vec3& IActor3D::GetWorldRotation()
	{
		return this->GetComponent<WorldRotationComponent>()->Rotation;
	}

	void IActor3D::SetScale(const glm::vec3& scale)
	{
		this->GetComponent<ScaleComponent>()->Scale = scale;
		RecalculateModelMatrix();
	}

	const glm::vec3& IActor3D::GetScale()
	{
		return this->GetComponent<ScaleComponent>()->Scale;
	}

	void IActor3D::RecalculateModelMatrix()
	{
		_model_matrice = glm::translate(glm::mat4(1.0f), this->GetWorldPosition());
		
		auto& rotation = this->GetComponent<WorldRotationComponent>()->Rotation;
		_model_matrice = glm::rotate(_model_matrice, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		_model_matrice = glm::rotate(_model_matrice, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		_model_matrice = glm::rotate(_model_matrice, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		_model_matrice = glm::scale(_model_matrice, this->GetComponent<ScaleComponent>()->Scale);
	}
}