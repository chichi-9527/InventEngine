#include "IEpch.h"
#include "ITileMap.h"

#include "IEngine.h"

namespace INVENT
{
	ITileMap::ITileMap()
		: IActor2D()
		, _tile_map_size(0,0)
		, _sprite_size(1.0f,1.0f)
		, _world_rotation(0.0f,0.0f,0.0f)
	{}

	ITileMap::~ITileMap()
	{}

	void ITileMap::DynamicInit(const std::vector<TileSpriteTextureColorInit>& inits)
	{
		INVENT::IEngine::InstancePtr()->GetIWindow()->GetThreadPool()->Submit(0, [this](const std::vector<TileSpriteTextureColorInit>& Inits) {
			if (Inits.size() != this->_sprites.size())
			{
				INVENT_LOG_ERROR("A tile map init size is error");
				return;
			}
			for (size_t i = 0; i < this->_sprites.size(); ++i)
			{
				this->_sprites[i].SetColor(Inits[i].Color);

				if (Inits[i].TileMapTextureIndex >= 0)
					this->_sprites[i].SetTextureID(_texture_indexs[Inits[i].TileMapTextureIndex]);

				if (Inits[i].TextureCoordIndex.is_valid)
					this->_sprites[i].SetTextureCoordIndex(Inits[i].TextureCoordIndex);

			}
			}, inits);
	}

	void ITileMap::SetWorldRotation(const glm::vec3& rotation)
	{
		_world_rotation = rotation;
		UpdateSpritesInformation();
	}

	void ITileMap::SetTileMapSize(unsigned int w, unsigned int h)
	{
		_tile_map_size = { w, h };
		_sprites.resize((size_t)(w) * h);
		UpdateSpritesInformation();
	}

	const ISquare2dActor* ITileMap::GetSprite(unsigned int w, unsigned int h) const
	{
		if (w < _tile_map_size.first && h < _tile_map_size.second)
		{
			return &_sprites[(size_t)(h)*_tile_map_size.first + w];
		}
		return nullptr;
	}

	ISquare2dActor* ITileMap::GetSprite(unsigned int w, unsigned int h)
	{
		if (w < _tile_map_size.first && h < _tile_map_size.second)
		{
			return &_sprites[(size_t)(h)*_tile_map_size.first + w];
		}
		return nullptr;
	}

	void ITileMap::SetWorldPosition(const glm::vec3& position)
	{
		IActor2D::SetWorldPosition(position);
		UpdateSpritesInformation();
	}

	void ITileMap::UpdateSpritesInformation()
	{
		if (((size_t)(_tile_map_size.first) * _tile_map_size.second) > _sprites.size())
		{
			INVENT_LOG_ERROR("tile map size error!");
			return;
		}

		glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(_world_rotation.x), glm::vec3(1, 0, 0));
		glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(_world_rotation.y), glm::vec3(0, 1, 0));
		glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(_world_rotation.z), glm::vec3(0, 0, 1));
		glm::mat4 rotation = rotZ * rotY * rotX;

		for (unsigned int i = 0; i < _tile_map_size.first; ++i)
		{
			for (unsigned int j = 0; j < _tile_map_size.second; ++j)
			{
				auto& sprite = _sprites[(size_t)j * _tile_map_size.first + (size_t)i];
				glm::vec4 rotated = rotation * glm::vec4(_sprite_size.x / 2.0f + (float)i * _sprite_size.x, -_sprite_size.y / 2.0f - (float)j * _sprite_size.y, 0.0f, 0.0f);
				sprite.SetWorldPosition(this->GetWorldPosition() + glm::vec3(rotated));
				sprite.SetWorldRotation(_world_rotation);
				sprite.SetScale(_sprite_size);
			}
		}

	}

}