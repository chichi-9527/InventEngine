#ifndef _ITILEMAP_
#define _ITILEMAP_

#include "IActor2D.h"
#include "ISquare2dActor.h"

namespace INVENT
{
	class ITileMap : public IActor2D 
	{
	public:
		ITileMap();
		virtual ~ITileMap();


		struct TileSpriteTextureColorInit 
		{
			TileSpriteTextureColorInit(const glm::vec4& color)
				: Color(color)
			{
				TileMapTextureIndex = -1;
				TextureCoordIndex = { 0,0,false };
				Flip = { false, false };
			}

			TileSpriteTextureColorInit(int tile_map_texture_index, const ITexture2D::_UInt2& texture_coord_index, const std::pair<bool, bool>& flip)
				: TileMapTextureIndex(tile_map_texture_index)
				, TextureCoordIndex(texture_coord_index)
				, Flip(flip)
			{
				Color = { 1.0f,1.0f,1.0f,1.0f };
			}

			TileSpriteTextureColorInit(const glm::vec4& color, int tile_map_texture_index, const ITexture2D::_UInt2& texture_coord_index, const std::pair<bool, bool>& flip)
				: Color(color)
				, TileMapTextureIndex(tile_map_texture_index)
				, TextureCoordIndex(texture_coord_index)
				, Flip(flip)
			{}

			glm::vec4 Color = { 1.0f,1.0f,1.0f,1.0f };
			int TileMapTextureIndex = -1;
			ITexture2D::_UInt2 TextureCoordIndex = { 0,0,false };
			std::pair<bool, bool> Flip = { false, false };

		};

		void InitTileMapTextures(const std::initializer_list<ITexture2DManagement::TextureID>& textures) { _texture_indexs = textures; }

		void DynamicInit(const std::vector<TileSpriteTextureColorInit>& inits);

		// 旋转锚点在左上角
		virtual void SetWorldRotation(const glm::vec3& rotation);
		virtual const glm::vec3& GetWorldRotation() const { return _world_rotation; }

		void SetTileMapSize(unsigned int w, unsigned int h);
		const std::pair<unsigned int, unsigned int>& GetTileMapSize() const { return _tile_map_size; }
		size_t GetSpritesNum() const { return (size_t)_tile_map_size.first * _tile_map_size.second; }

		const ISquare2dActor* GetSprite(unsigned int w, unsigned int h) const;
		ISquare2dActor* GetSprite(unsigned int w, unsigned int h);

		void SetSpriteSize(const glm::vec2& size) { _sprite_size = size; }

		const std::vector<ISquare2dActor>& GetSquares() const { return _sprites; }
		std::vector<ISquare2dActor>& GetSquares() { return _sprites; }

		virtual void SetWorldPosition(const glm::vec3& position) override;

	private:
		void UpdateSpritesInformation();

	protected:
		std::vector<ISquare2dActor> _sprites;

	private:
		std::pair<unsigned int, unsigned int> _tile_map_size;

		std::vector<ITexture2DManagement::TextureID> _texture_indexs;

		glm::vec2 _sprite_size;
		glm::vec3 _world_rotation;
	};
}

#endif // !_ITILEMAP_
