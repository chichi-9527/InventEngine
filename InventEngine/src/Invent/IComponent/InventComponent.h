#ifndef _INVENTCOMPONENT_
#define _INVENTCOMPONENT_

#include <glm/glm.hpp>

#include <vector>

namespace INVENT
{
	struct IActionComponent 
	{
		IActionComponent() = default;
		IActionComponent(const IActionComponent&) = default;

		virtual void Update(float delta) = 0;
	};

	struct WorldPositionComponent
	{
		glm::vec3 WorldPosition;

		WorldPositionComponent() = default;
		WorldPositionComponent(const WorldPositionComponent&) = default;
		WorldPositionComponent(const glm::vec3& position)
			: WorldPosition(position){}
	};

	struct RelativePositionComponent
	{
		glm::vec3 RelativePosition;

		RelativePositionComponent() = default;
		RelativePositionComponent(const RelativePositionComponent&) = default;
		RelativePositionComponent(const glm::vec3& position)
			: RelativePosition(position){}
	};

	struct RotationComponent
	{
		glm::vec3 Rotation;

		RotationComponent() = default;
		RotationComponent(const RotationComponent&) = default;
		RotationComponent(const glm::vec3& rotation)
			: Rotation(rotation)
		{}
	};

	struct WorldRotationComponent
	{
		glm::vec3 Rotation;

		WorldRotationComponent() = default;
		WorldRotationComponent(const WorldRotationComponent&) = default;
		WorldRotationComponent(const glm::vec3& rotation)
			: Rotation(rotation)
		{}
	};

	struct ScaleComponent
	{
		glm::vec3 Scale;

		ScaleComponent() = default;
		ScaleComponent(const ScaleComponent&) = default;
		ScaleComponent(const glm::vec3& scale)
			: Scale(scale)
		{}
	};

	struct MeshVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		// 法线
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		// 切线
		glm::vec3 Tangent;
		// 双切线
		glm::vec3 Bitangent;

		bool HasNormal = false;
		bool HasTexCoords = false;
		bool HasTangent = false;
		bool HasBitangent = false;

		MeshVertex()
			: Position(0.0f)
			, Color(1.0f)
			, Normal(0.0f)
			, TexCoords(0.0f)
			, Tangent(0.0f)
			, Bitangent(0.0f)
		{}
	};

	struct MeshComponent
	{
		std::vector<MeshVertex> Vertexes;
		/*
		* size != 6 时无效  textureID == 0 时无此贴图
		* 0 diffuse 漫反射贴图
		* 1 normal 法线贴图
		* 2 specular 镜面反射贴图
		* 3 emission 自发光贴图
		* 4 roughness 粗糙度贴图
		* 5 ambient occlusion 环境光遮蔽贴图
		*/
		std::vector<size_t> TextureIDs;
		std::vector<unsigned int> Indeices;

		MeshComponent()
		{
			TextureIDs = { 0,0,0,0,0,0 };
		}
		MeshComponent(const MeshComponent&) = default;
	};


}




#endif // !_INVENTCOMPONENT_
