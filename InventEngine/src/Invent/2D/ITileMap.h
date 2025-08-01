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
			TileSpriteTextureColorInit()
				
			{}

			glm::vec4 Color;

		};

		void DynamicInit(const std::initializer_list<TileSpriteTextureColorInit>& inits);

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

		glm::vec2 _sprite_size;
		glm::vec3 _world_rotation;
	};
}

#endif // !_ITILEMAP_
