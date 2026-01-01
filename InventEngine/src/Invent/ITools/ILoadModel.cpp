#include "IEpch.h"
#include "ILoadModel.h"

#include "3D/IMesh.h"
#include "IComponent/InventComponent.h"
#include "ITexture/ITexture.h"

#include "IEngine.h"
#include "IBase/IWindow.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static Assimp::Importer importer;

namespace INVENT
{
	template<bool HasTextureCoord, bool HasNormal, bool HasVertexColor, bool HasTangentAndBitangent>
	static void process_mesh(std::vector<IMesh>& meshes, const aiMesh* mesh, const aiScene* scene, unsigned int textureCoordID = 0, unsigned int vertexColorID = 0)
	{
		size_t materialID = mesh->mMaterialIndex;
		auto mesh_comp = meshes[materialID - 1].GetMesh();

		unsigned int offset = (unsigned int)mesh_comp->Vertexes.size();

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			auto& vertex = mesh_comp->Vertexes.emplace_back();
			// position
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;

			if constexpr (HasTextureCoord)
			{
				vertex.HasTexCoords = 1.0f;
				vertex.TexCoords = { mesh->mTextureCoords[textureCoordID][i].x, mesh->mTextureCoords[textureCoordID][i].y };
			}
			if constexpr (HasNormal)
			{
				vertex.HasNormal = 1.0f;
			}
			if constexpr (HasVertexColor)
			{
				
			}
			if constexpr (HasTangentAndBitangent)
			{
				vertex.HasTangent = 1.0f;
				vertex.HasBitangent = 1.0f;
			}


		}
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
				mesh_comp->Indeices.emplace_back() = mesh->mFaces[i].mIndices[j] + offset;
		}

	};

	void ILoadModel::Load(std::vector<IMesh>& meshes, const std::string& path)
	{
		meshes.clear();

		auto scene = importer.ReadFile(path, aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (!scene)
		{
			INVENT_LOG_ERROR(importer.GetErrorString());
			return;
		}

		unsigned int material_num = scene->mNumMaterials;
		meshes.resize(size_t(material_num - 1));
		unsigned int MaterialID = 1U;
		for (auto& imesh : meshes)
		{
			auto& textureIDs = imesh.GetMesh()->TextureIDs;
			const auto& name = scene->mMaterials[MaterialID]->GetName();
			if (scene->mMaterials[MaterialID]->GetTextureCount(aiTextureType_DIFFUSE))
			{
				aiString path;
				scene->mMaterials[MaterialID]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
				textureIDs[0] = ITexture2DManagement::Instance().CreateTextureDynamic(std::string(name.C_Str()) + "_diffuse", path.C_Str());
			}
			
		}


		aiNode* root_node = scene->mRootNode;
		std::queue<aiNode*> nodes;
		nodes.push(root_node);

		while (!nodes.empty())
		{
			auto& node = nodes.front();
			for (unsigned int i = 0; i < node->mNumMeshes; ++i)
			{
				auto mesh = scene->mMeshes[node->mMeshes[i]];
				if (mesh->GetNumUVChannels() && mesh->HasNormals() && mesh->GetNumColorChannels() && mesh->HasTangentsAndBitangents())
				{
					process_mesh<true, true, true, true>(meshes, mesh, scene);
				}
				
			}

			for (unsigned int i = 0; i < node->mNumChildren; ++i)
			{
				nodes.push(node->mChildren[i]);
			}
			nodes.pop();
		}

	}
}
