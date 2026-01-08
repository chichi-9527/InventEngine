#include "IEpch.h"
#include "IMesh.h"

#include "ITools/ILoadModel.h"

namespace INVENT
{
	IMesh::IMesh()
	{
		_mesh_comp.PreExpansion((size_t)2 << 13);
	}

	IMesh::~IMesh()
	{
	}

	MeshComponent* IMesh::GetMesh()
	{
		return &_mesh_comp;
	}




	/////////////////////////// IModelManagement /////////////////////////////////////////////////

	static std::vector<std::pair<std::string, std::vector<IMesh>>> IMeshesVector;
	static std::unordered_map<std::string, IModelManagement::ModelID> IMeshesMap;
	static std::mutex modelMutex;

	IModelManagement::~IModelManagement()
	{
		IMeshesVector.clear();
	}

	IModelManagement::IModelManagement()
	{
		IMeshesVector.reserve((size_t)2 << 5);
		IMeshesMap.reserve((size_t)2 << 5);
	}

	IModelManagement& IModelManagement::Instance()
	{
		static IModelManagement m;
		return m;
	}

	IModelManagement::ModelID IModelManagement::LoadModel(const std::string& path)
	{
		auto startcount = path.find_last_of("/\\") + 1;
		auto lastcount = path.find_last_of('.');
		std::string name = path.substr(startcount, lastcount - startcount);

		return LoadModelInName(name, path);
	}

	IModelManagement::ModelID IModelManagement::LoadModel(const std::string& path, std::string& name)
	{
		auto startcount = path.find_last_of("/\\") + 1;
		auto lastcount = path.find_last_of('.');
		name = path.substr(startcount, lastcount - startcount);

		return LoadModelInName(name, path);
	}

	IModelManagement::ModelID IModelManagement::LoadModelInName(const std::string& name, const std::string& path)
	{
		if (IMeshesMap.find(name) != IMeshesMap.end())
		{
			return IMeshesMap[name];
		}

		std::lock_guard<std::mutex> lock(modelMutex);

		IModelManagement::ModelID id = IMeshesVector.size();

		auto& pair_imeshes = IMeshesVector.emplace_back();
		pair_imeshes.first = name;
		ILoadModel::Load(pair_imeshes.second, path);

		return IMeshesMap[name] = id;
	}

	void IModelManagement::UnLoadModel(ModelID id)
	{
		if (id >= IMeshesVector.size()) return;
		std::lock_guard<std::mutex> lock(modelMutex);

		IMeshesMap.erase(IMeshesVector[id].first);
		IMeshesVector[id] = IMeshesVector.back();
		IMeshesVector.pop_back();
	}

	std::vector<IMesh>* IModelManagement::GetMeshes(const std::string& name)
	{
		if (IMeshesMap.find(name) != IMeshesMap.end())
		{
			return GetMeshes(IMeshesMap[name]);
		}
		return nullptr;
	}

	std::vector<IMesh>* IModelManagement::GetMeshes(ModelID id)
	{
		if (id >= IMeshesVector.size()) return nullptr;
		return &(IMeshesVector[id].second);
	}


}
