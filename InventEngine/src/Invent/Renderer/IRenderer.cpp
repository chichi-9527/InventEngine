#include "IEpch.h"
#include "IRenderer.h"

#include "IRenderer2D.h"

#include "Invent/3D/IMesh.h"
#include "IBuffer.h"
#include "IUniformBuffer.h"
#include "IShaderStorageBuffer.h"
#include "Invent/IComponent/InventComponent.h"


#define INVENT_MAX_VERTEX_RENDER_ONCE  20000 * 6
#define INVENT_MAX_INDEX_RENDER_ONCE  INVENT_MAX_VERTEX_RENDER_ONCE * 4
#define INVENT_MAX_TEXTURE_RENDER_ONCE  32
#define INVENT_MAX_MESH_A_RANDER  1000

namespace INVENT
{
	std::unique_ptr<IRenderer::SceneData> IRenderer::_scene_data = std::make_unique<IRenderer::SceneData>();

	void IRenderer::Init()
	{
		IRendererCommend::Init();
		IRenderer2D::Init();
		IRenderer::_init_renderer_data();
	}

	void IRenderer::Shutdown()
	{
		IRenderer::_free_renderer_data();
		IRenderer2D::Shutdown();
	}

	void IRenderer::Submit(const IShader* shader, const std::shared_ptr<IVertexArray> vertex_array, const glm::mat4& transfrom)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", _scene_data->ViewProjectionMatrix);
		shader->SetMat4("u_Transfrom", transfrom);

		IRendererCommend::DrawIndexed(vertex_array, vertex_array->GetIndexBuffer()->GetCount());
	}

	// 每个 Mesh
	struct DrawElementsIndirectCommand 
	{
		unsigned int count; // 索引数量 index count
		unsigned int instanceCount; // 实例数量 always = 1；优化批量渲染时更改代码
		unsigned int firstIndex; // 第一个索引在IBO中的位置（注意：是索引，不是字节偏移）
		unsigned int baseVertex; // 基顶点（顶点索引的偏移）
		unsigned int baseInstance; // 基实例（模型ID）

		DrawElementsIndirectCommand()
			: count(0)
			, instanceCount(1)
			, firstIndex(0)
			, baseVertex(0)
			, baseInstance(0)
		{}
	};

	struct InstanceData
	{
		glm::mat4 modleMartrix;
		glm::vec4 TextureIDs;

		InstanceData()
			: modleMartrix(1.0f)
			, TextureIDs(0.0f)
		{}
	};

	struct RanderMeshVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoords;
		glm::vec3 TextureIDs;

		RanderMeshVertex()
			: Position(0.0f)
			, TexCoords(0.0f)
			, TextureIDs(0.0f)
		{}
	};
	//struct RanderMeshVertex
	//{
	//	glm::vec3 Position;
	//	glm::vec4 Color;
	//	// 法线
	//	glm::vec3 Normal;
	//	glm::vec2 TexCoords;
	//	// 切线
	//	glm::vec3 Tangent;
	//	// 双切线
	//	glm::vec3 Bitangent;

	//	RanderMeshVertex()
	//		: Position(0.0f)
	//		, Color(1.0f)
	//		, Normal(0.0f)
	//		, TexCoords(0.0f)
	//		, Tangent(0.0f)
	//		, Bitangent(0.0f)
	//	{}
	//};

	struct RendererData
	{
		// once default render
		std::shared_ptr<IVertexArray> VertexArray;
		std::shared_ptr<IVertexBuffer> VertexBuffer;
		std::shared_ptr<IIndexBuffer> IndexBuffer;
		IShader* Shader = nullptr;
		ITexture2D* WhiteTexture = nullptr;

		unsigned int VertexCount = 0;
		unsigned int IndexCount = 0;
		unsigned int BaseInstance = 0;

		unsigned int* Indices = nullptr;
		unsigned int IndexOffset = 0;

		std::array<ITextureBase*, INVENT_MAX_TEXTURE_RENDER_ONCE> TextureArray;
		size_t TextureSlotIndex = 0;

		RanderMeshVertex* VertexBuffers = nullptr;
		RanderMeshVertex* VertexBufferBack = nullptr;

		unsigned int ModelID = 0;

		std::vector<InstanceData> Instances;
		std::vector<DrawElementsIndirectCommand> Cmds;

		struct CameraData
		{
			glm::mat4 ViewProjection{ 1.0f };
			glm::mat4 ViewProjection2D{ 1.0f };
		};
		CameraData CameraBuffer;

		std::shared_ptr<IUniformBuffer> CameraUniformBuffer;
		std::shared_ptr<IDrawIndirectBuffer> DrawIndirectBuffer;
		std::shared_ptr<IShaderStorageBuffer> ShaderStorageBuffer;

		RendererData()
		{
			TextureArray.fill(nullptr);
		}
	};

	static RendererData renderer_data;

	void IRenderer::_init_renderer_data()
	{
		renderer_data.VertexArray = IVertexArray::CreatePtr();
		renderer_data.VertexBuffer = IVertexBuffer::CreatePtr(INVENT_MAX_VERTEX_RENDER_ONCE * sizeof(RanderMeshVertex));
		renderer_data.VertexBuffer->SetLayout({
			{IShaderDataType::Float3, "a_Position"},
			{IShaderDataType::Float2, "a_TexCoord"},
			{IShaderDataType::Float3, "a_TextureIDs"}
			});
		renderer_data.VertexArray->AddVertexBuffer(renderer_data.VertexBuffer);

		renderer_data.Shader = IShaderManagement::GetDefault3DShader();
		renderer_data.WhiteTexture = ITexture2DManagement::GetWhiteTexture();

		renderer_data.TextureArray[0] = renderer_data.WhiteTexture;
		renderer_data.TextureSlotIndex = 1;

		renderer_data.VertexBuffers = new RanderMeshVertex[INVENT_MAX_VERTEX_RENDER_ONCE];
		
		renderer_data.IndexBuffer = IIndexBuffer::CreatePtr(INVENT_MAX_INDEX_RENDER_ONCE);
		renderer_data.VertexArray->SetIndexBuffer(renderer_data.IndexBuffer);

		renderer_data.Indices = new unsigned int[INVENT_MAX_INDEX_RENDER_ONCE];
		
		renderer_data.CameraUniformBuffer = IUniformBuffer::CreatePtr(sizeof(RendererData::CameraData));
		renderer_data.DrawIndirectBuffer = IDrawIndirectBuffer::CreatePtr((unsigned int)(INVENT_MAX_MESH_A_RANDER * sizeof(DrawElementsIndirectCommand)));
		renderer_data.ShaderStorageBuffer = IShaderStorageBuffer::CreatePtr((unsigned int)(INVENT_MAX_MESH_A_RANDER * sizeof(InstanceData)));

	}

	void IRenderer::BeginRender(const ICamera* camera)
	{
		renderer_data.CameraBuffer.ViewProjection = camera ? camera->GetViewProjectionMatrix() : glm::mat4(1.0f);
		renderer_data.CameraBuffer.ViewProjection2D = glm::mat4(1.0f);
		renderer_data.CameraUniformBuffer->SetData(&renderer_data.CameraBuffer, sizeof(RendererData::CameraData));
		renderer_data.CameraUniformBuffer->Bind(0);

		StartARender();
		
	}

	void IRenderer::EndRender()
	{
		Rendering();
		renderer_data.CameraUniformBuffer->UnBind(0);
	}

	void IRenderer::_free_renderer_data()
	{
		if (renderer_data.Indices)
		{
			delete[] renderer_data.Indices;
		}
		if (renderer_data.VertexBuffers)
		{
			delete[] renderer_data.VertexBuffers;
		}
	}

	void IRenderer::StartARender()
	{
		renderer_data.VertexCount = 0;
		renderer_data.IndexCount = 0;
		renderer_data.BaseInstance = 0;
		renderer_data.VertexBufferBack = renderer_data.VertexBuffers;
		renderer_data.Instances.clear();

		renderer_data.TextureSlotIndex = 1;
	}

	void IRenderer::NextARender()
	{
		Rendering();
		StartARender();
	}

	void IRenderer::Rendering()
	{
		// 
		if (renderer_data.IndexCount)
		{
			unsigned int data_size = (unsigned int)((unsigned char*)renderer_data.VertexBufferBack - (unsigned char*)renderer_data.VertexBuffers);
			renderer_data.VertexBuffer->SetData((void*)renderer_data.VertexBuffers, data_size);

			renderer_data.IndexBuffer->SetData((void*)renderer_data.Indices, renderer_data.IndexCount * sizeof(unsigned int));

			for (unsigned int i = 0; i < renderer_data.TextureSlotIndex; ++i)
			{
				renderer_data.TextureArray[i]->BindUnit(i);
			}
			renderer_data.Shader->Bind();
			renderer_data.ShaderStorageBuffer->Bind(0);
			renderer_data.ShaderStorageBuffer->SetData((void*)renderer_data.Instances.data(), unsigned int(renderer_data.Instances.size() * sizeof(InstanceData)));
			renderer_data.DrawIndirectBuffer->Bind();
			renderer_data.DrawIndirectBuffer->SetData((void*)renderer_data.Cmds.data(), unsigned int(renderer_data.Cmds.size() * sizeof(DrawElementsIndirectCommand)));
			IRendererCommend::MultiDrawElementsIndirect(renderer_data.VertexArray, renderer_data.BaseInstance);
		}

	}

	void IRenderer::DrawMesh(IMesh* mesh, const glm::mat4& model_martrix)
	{
		auto mesh_comp = mesh->GetMesh();

		if (mesh_comp->Vertexes.size() == 0)
		{
			return;
		}

		if (renderer_data.VertexCount + mesh_comp->Vertexes.size() > INVENT_MAX_VERTEX_RENDER_ONCE
			 || renderer_data.IndexCount + mesh_comp->Indeices.size() > INVENT_MAX_INDEX_RENDER_ONCE
			 || renderer_data.TextureSlotIndex + mesh_comp->TextureIDs.size() - std::count(mesh_comp->TextureIDs.begin(), mesh_comp->TextureIDs.end(), 0) > INVENT_MAX_TEXTURE_RENDER_ONCE
			 || renderer_data.BaseInstance >= INVENT_MAX_MESH_A_RANDER)
		{
			NextARender();
		}

		auto& instance = renderer_data.Instances.emplace_back();
		instance.modleMartrix = model_martrix;
		// diffuse
		if (size_t t_id = mesh_comp->TextureIDs[0])
		{
			auto texture = ITexture2DManagement::Instance()[t_id];
			if (texture && texture->IsValid)
			{
				float texture_index = .0f;
				for (size_t i = 1; i < renderer_data.TextureSlotIndex; ++i)
				{
					if (renderer_data.TextureArray[i] == texture)
						texture_index = (float)i; break;
				}
				if (texture_index == 0.0f)
				{
					texture_index = (float)renderer_data.TextureSlotIndex;
					renderer_data.TextureArray[renderer_data.TextureSlotIndex] = texture;
					renderer_data.TextureSlotIndex++;
				}
				instance.TextureIDs.x = texture_index;
			}
		}
		// 
		


		auto& cmd = renderer_data.Cmds.emplace_back();
		cmd.count = unsigned int(mesh_comp->Indeices.size());
		cmd.firstIndex = renderer_data.IndexCount;
		cmd.baseVertex = renderer_data.VertexCount;
		cmd.baseInstance = renderer_data.BaseInstance++;

		

		for (auto& mesh_vertex : mesh_comp->Vertexes)
		{
			renderer_data.VertexBufferBack->Position = model_martrix * glm::vec4(mesh_vertex.Position, 1.0f);
			renderer_data.VertexBufferBack->TexCoords = mesh_vertex.TexCoords;
			renderer_data.VertexBufferBack->TextureIDs = instance.TextureIDs;
			renderer_data.VertexBufferBack++;
		}

		for (unsigned int i = 0; i < mesh_comp->Indeices.size(); ++i)
		{
			renderer_data.Indices[renderer_data.IndexCount + i] = mesh_comp->Indeices[i] + renderer_data.VertexCount;
		}

		renderer_data.IndexCount += (unsigned int)mesh_comp->Indeices.size();
		renderer_data.VertexCount += (unsigned int)mesh_comp->Vertexes.size();



	}



}