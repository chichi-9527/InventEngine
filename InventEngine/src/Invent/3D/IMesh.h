#ifndef _IMESH_
#define _IMESH_

#include "IBase/IObjectBase.h"

#include <string>
#include <vector>

namespace INVENT
{
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

		float HasNormal = 0.0f;
		float HasTexCoords = 0.0f;
		float HasTangent = 0.0f;
		float HasBitangent = 0.0f;

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
			Vertexes.clear();
			Indeices.clear();
		}
		MeshComponent(const MeshComponent&) = default;

		void PreExpansion(size_t capacity)
		{
			Vertexes.reserve(capacity);
			Indeices.reserve(capacity);
		}
	};

	class IMesh
	{
		friend class ILoadModel;
	public:
		IMesh();
		virtual ~IMesh();

		MeshComponent* GetMesh();

		const std::string& Name() const { return _name; }
		void SetName(const std::string& name) { _name = name; }
	private:
		std::string _name;
		MeshComponent _mesh_comp;
	};


	class IModelManagement
	{
	public:
		~IModelManagement();

		typedef size_t ModelID;

		static IModelManagement& Instance();

		ModelID LoadModel(const std::string& path);
		/*
		* path: in
		* name: out
		*/
		ModelID LoadModel(const std::string& path, std::string& name);

		/*
		* name : in 
		* path : in
		*/
		ModelID LoadModelInName(const std::string& name, const std::string& path);

		void UnLoadModel(ModelID id);

		std::vector<IMesh>* GetMeshes(const std::string& name);
		std::vector<IMesh>* GetMeshes(ModelID id);

		std::vector<IMesh>* operator[](ModelID id)
		{
			return GetMeshes(id);
		}



	private:
		IModelManagement();
	};


}



#endif // !_IMESH_