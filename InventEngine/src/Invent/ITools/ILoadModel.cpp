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
				vertex.Normal = { mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z };
			}
			if constexpr (HasVertexColor)
			{
				vertex.Color = { mesh->mColors[vertexColorID][i].r, mesh->mColors[vertexColorID][i].g,mesh->mColors[vertexColorID][i].b,mesh->mColors[vertexColorID][i].a };
			}
			if constexpr (HasTangentAndBitangent)
			{
				vertex.HasTangent = 1.0f;
				vertex.HasBitangent = 1.0f;
				vertex.Tangent = { mesh->mTangents[i].x,mesh->mTangents[i].y,mesh->mTangents[i].z };
				vertex.Bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y,mesh->mBitangents[i].z };
			}


		}

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
			{
				mesh_comp->Indeices.emplace_back() = mesh->mFaces[i].mIndices[j] + offset;
			}
		}

	};

	void ILoadModel::Load(std::vector<IMesh>& meshes, const std::string& path)
	{
		meshes.clear();

		auto startcount = path.find_last_of("/\\") + 1;
		auto lastcount = path.find_last_of('.');
		std::string model_name = path.substr(startcount, lastcount - startcount);

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
		aiString aipath;
		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			auto& textureIDs = meshes[i].GetMesh()->TextureIDs;
			const auto& name = scene->mMaterials[i + 1]->GetName();
			std::string meshName = model_name + "_" + name.C_Str();

			if (scene->mMaterials[i + 1]->GetTextureCount(aiTextureType_DIFFUSE))
			{
				
				scene->mMaterials[i + 1]->GetTexture(aiTextureType_DIFFUSE, 0, &aipath);
				textureIDs[0] = ITexture2DManagement::Instance().CreateTextureDynamic(meshName + "_diffuse", path + "/../" + aipath.C_Str());

			}
			if (scene->mMaterials[i + 1]->GetTextureCount(aiTextureType_HEIGHT))
			{
				scene->mMaterials[i + 1]->GetTexture(aiTextureType_HEIGHT, 0, &aipath);
				textureIDs[1] = ITexture2DManagement::Instance().CreateTextureDynamic(meshName + "_normal", path + "/../" + aipath.C_Str());
			}
			if (scene->mMaterials[i + 1]->GetTextureCount(aiTextureType_SPECULAR))
			{
				scene->mMaterials[i + 1]->GetTexture(aiTextureType_SPECULAR, 0, &aipath);
				textureIDs[2] = ITexture2DManagement::Instance().CreateTextureDynamic(meshName + "_specular", path + "/../" + aipath.C_Str());
			}
			if (scene->mMaterials[i + 1]->GetTextureCount(aiTextureType_EMISSIVE))
			{
				scene->mMaterials[i + 1]->GetTexture(aiTextureType_EMISSIVE, 0, &aipath);
				textureIDs[3] = ITexture2DManagement::Instance().CreateTextureDynamic(meshName + "_emission", path + "/../" + aipath.C_Str());
			}
			if (scene->mMaterials[i + 1]->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
			{
				scene->mMaterials[i + 1]->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &aipath);
				textureIDs[4] = ITexture2DManagement::Instance().CreateTextureDynamic(meshName + "_roughness", path + "/../" + aipath.C_Str());
			}

			if (scene->mMaterials[i + 1]->GetTextureCount(aiTextureType_AMBIENT))
			{
				scene->mMaterials[i + 1]->GetTexture(aiTextureType_AMBIENT, 0, &aipath);
				textureIDs[5] = ITexture2DManagement::Instance().CreateTextureDynamic(meshName + "_ao", path + "/../" + aipath.C_Str());
			}
			else if (scene->mMaterials[i + 1]->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION))
			{
				scene->mMaterials[i + 1]->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &aipath);
				textureIDs[5] = ITexture2DManagement::Instance().CreateTextureDynamic(meshName + "_ao", path + "/../" + aipath.C_Str());
			}
		
		}


		aiNode* root_node = scene->mRootNode;
		std::queue<aiNode*> nodes;
		nodes.push(root_node);

		unsigned int textureCoordID = 0;
		unsigned int vertexColorID = 0;
		auto getID = [](aiMesh* aimesh, unsigned int& tID, unsigned int& cID) {
			for (int i = AI_MAX_NUMBER_OF_TEXTURECOORDS - 1; i >= 0; --i)
			{
				if (aimesh->mTextureCoords[i]) tID = i;
				if (aimesh->mColors[i]) cID = i;
			}
			};

		while (!nodes.empty())
		{
			auto node = nodes.front();
			for (unsigned int i = 0; i < node->mNumMeshes; ++i)
			{
				auto mesh = scene->mMeshes[node->mMeshes[i]];
				getID(mesh, textureCoordID, vertexColorID);

				if (mesh->GetNumUVChannels() && mesh->HasNormals() && mesh->GetNumColorChannels() && mesh->HasTangentsAndBitangents())
				{
					process_mesh<true, true, true, true>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->GetNumUVChannels() && mesh->HasNormals() && mesh->GetNumColorChannels())
				{
					process_mesh<true, true, true, false>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->GetNumUVChannels() && mesh->HasNormals() && mesh->HasTangentsAndBitangents())
				{
					process_mesh<true, true, false, true>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->GetNumUVChannels() && mesh->GetNumColorChannels() && mesh->HasTangentsAndBitangents())
				{
					process_mesh<true, false, true, true>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->HasNormals() && mesh->GetNumColorChannels() && mesh->HasTangentsAndBitangents())
				{
					process_mesh<false, true, true, true>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->GetNumUVChannels() && mesh->HasNormals())
				{
					process_mesh<true, true, false, false>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->GetNumUVChannels() && mesh->GetNumColorChannels())
				{
					process_mesh<true, false, true, false>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->GetNumUVChannels() && mesh->HasTangentsAndBitangents())
				{
					process_mesh<true, false, false, true>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->HasNormals() && mesh->GetNumColorChannels())
				{
					process_mesh<false, true, true, false>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->HasNormals() && mesh->HasTangentsAndBitangents())
				{
					process_mesh<false, true, false, true>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->GetNumColorChannels() && mesh->HasTangentsAndBitangents())
				{
					process_mesh<false, false, true, true>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->GetNumUVChannels())
				{
					process_mesh<true, false, false, false>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->HasNormals())
				{
					process_mesh<false, true, false, false>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->GetNumColorChannels())
				{
					process_mesh<false, false, true, false>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else if (mesh->HasTangentsAndBitangents())
				{
					process_mesh<false, false, false, true>(meshes, mesh, scene, textureCoordID, vertexColorID);
				}
				else
				{
					process_mesh<false, false, false, false>(meshes, mesh, scene, textureCoordID, vertexColorID);
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
