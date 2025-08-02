#include "IEpch.h"
#include "IRenderer2D.h"

#include "IRendererCommand.h"
#include "IVertexArray.h"
#include "IUniformBuffer.h"

#include "Shader/IShader.h"

#include "ITexture/ITexture.h"

#include <array>

#include <glm/glm.hpp>

#define INVENT_MAX_SQUARE_RENDER_ONCE 20000
#define INVENT_MAX_VERTEX_RENDER_ONCE INVENT_MAX_SQUARE_RENDER_ONCE * 4
#define INVENT_MAX_INDEX_RENDER_ONCE INVENT_MAX_SQUARE_RENDER_ONCE * 6
#define INVENT_MAX_TEXTURE_RENDER_ONCE 32


namespace INVENT
{
	struct SquareVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
	};

	struct Renderer2DData
	{
		std::shared_ptr<IVertexArray> SquareVertexArray;
		std::shared_ptr<IVertexBuffer> SquareVertexBuffer;
		IShader* SquareShader = nullptr;
		ITexture2D* WhiteTexture = nullptr;

		unsigned int SquareIndexCount = 0;
		// is not ivertexbuffer is all vertexs
		SquareVertex* SquareVertexBuffers = nullptr;
		SquareVertex* SquareVertexBufferBack = nullptr;

		float LineWidth = 2.0f;

		std::array<ITexture2D*, INVENT_MAX_TEXTURE_RENDER_ONCE> TextureArray;
		size_t TextureSlotIndex = 0;

		glm::vec4 SquareVertexPosition[4]{};

		struct CameraData 
		{
			glm::mat4 ViewProjection{ 1.0f };
		};
		CameraData CameraBuffer;

		std::shared_ptr<IUniformBuffer> CameraUniformBuffer;

		Renderer2DData()
		{
			TextureArray.fill(nullptr);
		}
	};

	static Renderer2DData renderer2d_data;


	void IRenderer2D::Init()
	{
		// square
		renderer2d_data.SquareVertexArray = IVertexArray::CreatePtr();
		renderer2d_data.SquareVertexBuffer = IVertexBuffer::CreatePtr(INVENT_MAX_VERTEX_RENDER_ONCE * sizeof(SquareVertex));
		renderer2d_data.SquareVertexBuffer->SetLayout({
			{IShaderDataType::Float3, "a_Position"},
			{IShaderDataType::Float4, "a_Color"},
			{IShaderDataType::Float2, "a_TexCoord"},
			{IShaderDataType::Float, "a_TexIndex"}
			});
		renderer2d_data.SquareVertexArray->AddVertexBuffer(renderer2d_data.SquareVertexBuffer);

		renderer2d_data.SquareShader = IShaderManagement::GetDefaultSquare2DShader();
		renderer2d_data.WhiteTexture = ITexture2DManagement::GetWhiteTexture();

		renderer2d_data.TextureArray[0] = renderer2d_data.WhiteTexture;
		renderer2d_data.TextureSlotIndex = 1;

		renderer2d_data.SquareVertexBuffers = new SquareVertex[INVENT_MAX_VERTEX_RENDER_ONCE];
		unsigned int* square_indices = new unsigned int[INVENT_MAX_INDEX_RENDER_ONCE];
		unsigned int offset = 0;
		for (unsigned int i = 0; i < INVENT_MAX_INDEX_RENDER_ONCE; i += 6)
		{
			square_indices[i + 0] = offset + 0;
			square_indices[i + 1] = offset + 1;
			square_indices[i + 2] = offset + 2;

			square_indices[i + 3] = offset + 2;
			square_indices[i + 4] = offset + 3;
			square_indices[i + 5] = offset + 0;

			offset += 4;
		}
		auto square_index_buffer = IIndexBuffer::CreatePtr(square_indices, INVENT_MAX_INDEX_RENDER_ONCE);
		renderer2d_data.SquareVertexArray->SetIndexBuffer(square_index_buffer);
		delete[] square_indices;


		// other instance need render 


		///////////////////
		renderer2d_data.SquareVertexPosition[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		renderer2d_data.SquareVertexPosition[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		renderer2d_data.SquareVertexPosition[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		renderer2d_data.SquareVertexPosition[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		renderer2d_data.CameraUniformBuffer = IUniformBuffer::CreatePtr(sizeof(Renderer2DData::CameraData), 0);
	}

	void IRenderer2D::Init(float line_width)
	{
		Init();
		renderer2d_data.LineWidth = line_width;
	}

	void IRenderer2D::Shutdown()
	{
		delete[] renderer2d_data.SquareVertexBuffers;
	}

	void IRenderer2D::BeginRender(const ICamera* camera)
	{
		renderer2d_data.CameraBuffer.ViewProjection = camera ? camera->GetViewProjectionMatrix() : glm::mat4(1.0f);
		renderer2d_data.CameraUniformBuffer->SetData(&renderer2d_data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartARender();
	}

	void IRenderer2D::EndRender()
	{
		Rendering();
	}

	void IRenderer2D::StartARender()
	{
		renderer2d_data.SquareIndexCount = 0;
		renderer2d_data.SquareVertexBufferBack = renderer2d_data.SquareVertexBuffers;

		renderer2d_data.TextureSlotIndex = 1;
	}

	void IRenderer2D::NextARender()
	{
		Rendering();
		StartARender();
	}

	void IRenderer2D::Rendering()
	{
		// 
		if (renderer2d_data.SquareIndexCount)
		{
			unsigned int data_size = (unsigned int)((unsigned char*)renderer2d_data.SquareVertexBufferBack - (unsigned char*)renderer2d_data.SquareVertexBuffers);
			renderer2d_data.SquareVertexBuffer->SetData((void*)renderer2d_data.SquareVertexBuffers, data_size);

			for (unsigned int i = 0; i < renderer2d_data.TextureSlotIndex; ++i)
			{
				renderer2d_data.TextureArray[i]->BindUnit(i);
			}
			renderer2d_data.SquareShader->Bind();
			IRendererCommend::DrawIndexed(renderer2d_data.SquareVertexArray, renderer2d_data.SquareIndexCount);
		}

		//

	}

	void IRenderer2D::DrawSquare(ISquare2dActor* actor)
	{
		//if (!actor) return;
		
		if (!actor->GetShader())
		{
			return;
		}

		if (renderer2d_data.SquareShader != actor->GetShader())
		{
			INVENT_LOG_WARNING(std::string("actor's shader is not default shader ,you need use youself renderer. ShaderName: ") + actor->GetShader()->Name()); return;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), actor->GetWorldPosition());
		auto& rotation = actor->GetWorldRotation();
		if (rotation.x)
			transform *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), { 1.0f, 0.0f, 0.0f });
		if (rotation.y)
			transform *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), { 0.0f, 1.0f, 0.0f });
		if (rotation.z)
			transform *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), { 0.0f, 0.0f, 1.0f });
		transform *= glm::scale(glm::mat4(1.0f), glm::vec3(actor->GetScale(), 1.0f));

		if (renderer2d_data.SquareIndexCount >= INVENT_MAX_INDEX_RENDER_ONCE)
			NextARender();

		auto texture = actor->GetTexture() ? actor->GetTexture() : ITexture2DManagement::Instance()[actor->GetTextureID()];
		float texture_index = .0f;

		if (texture)
		{
			if (texture->IsValid)
			{
				for (unsigned int i = 1; i < renderer2d_data.TextureSlotIndex; ++i)
				{
					if (renderer2d_data.TextureArray[i] == texture)
						texture_index = (float)i; break;
				}
			}
		}

		if (texture && texture_index == .0f)
		{
			if (texture->IsValid)
			{
				if (renderer2d_data.TextureSlotIndex >= INVENT_MAX_TEXTURE_RENDER_ONCE - 1)
					NextARender();

				texture_index = (float)renderer2d_data.TextureSlotIndex;
				renderer2d_data.TextureArray[renderer2d_data.TextureSlotIndex] = texture;
				renderer2d_data.TextureSlotIndex++;
			}
			
		}

		glm::vec2 texture_coords[4]{};
		
		if (texture)
		{
			if (texture->IsValid)
			{
				if (actor->GetTextureCoordIndex().is_valid && texture->GetBreakNum().is_valid)
				{
					glm::vec2 ld((float)actor->GetTextureCoordIndex().width / (float)texture->GetBreakWNum(), (float)actor->GetTextureCoordIndex().height / (float)texture->GetBreakHNum());
					glm::vec2 ru((float)(actor->GetTextureCoordIndex().width + 1) / (float)texture->GetBreakWNum(), (float)(actor->GetTextureCoordIndex().height + 1) / (float)texture->GetBreakHNum());
					texture_coords[0] = ld;
					texture_coords[1] = glm::vec2(ru.x, ld.y);
					texture_coords[2] = ru;
					texture_coords[3] = glm::vec2(ld.x, ru.y);
				}
				else
				{
					glm::vec2 ld = actor->GetTextureCoord()[0];
					glm::vec2 ru = actor->GetTextureCoord()[1];
					texture_coords[0] = ld;
					texture_coords[1] = glm::vec2(ru.x, ld.y);
					texture_coords[2] = ru;
					texture_coords[3] = glm::vec2(ld.x, ru.y);
				}

				if (actor->GetFlip().first)
				{
					std::swap(texture_coords[0], texture_coords[1]);
					std::swap(texture_coords[2], texture_coords[3]);
				}
				if (actor->GetFlip().second)
				{
					std::swap(texture_coords[0], texture_coords[3]);
					std::swap(texture_coords[2], texture_coords[1]);
				}

			}
			
		}

		constexpr size_t square_vertex_count = 4;

		for (size_t i = 0; i < square_vertex_count; ++i)
		{
			renderer2d_data.SquareVertexBufferBack->Position = transform * renderer2d_data.SquareVertexPosition[i];
			renderer2d_data.SquareVertexBufferBack->Color = actor->GetColor();
			renderer2d_data.SquareVertexBufferBack->TexCoord = texture_coords[i];
			renderer2d_data.SquareVertexBufferBack->TexIndex = texture_index;

			renderer2d_data.SquareVertexBufferBack++;
		}

		renderer2d_data.SquareIndexCount += 6;

	}

}