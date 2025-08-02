#ifndef _INVENT2DANIMATIONCOMPONENT_
#define _INVENT2DANIMATIONCOMPONENT_

#include "2D/ISquare2dActor.h"

namespace INVENT
{
	struct AnimationComponent
	{
		struct SpriteTextureColorInit
		{
			SpriteTextureColorInit(const glm::vec4& color)
				: Color(color)
			{
				TextureId = 0;
				TextureCoordIndex = { 0,0,false };
				Flip = { false, false };
			}

			SpriteTextureColorInit(ITexture2DManagement::TextureID texture_id, const ITexture2D::_UInt2& texture_coord_index, const std::pair<bool, bool>& flip)
				: TextureId(texture_id)
				, TextureCoordIndex(texture_coord_index)
				, Flip(flip)
			{
				Color = { 1.0f,1.0f,1.0f,1.0f };
			}

			SpriteTextureColorInit(const glm::vec4& color, ITexture2DManagement::TextureID texture_id, const ITexture2D::_UInt2& texture_coord_index, const std::pair<bool, bool>& flip)
				: Color(color)
				, TextureId(texture_id)
				, TextureCoordIndex(texture_coord_index)
				, Flip(flip)
			{}

			glm::vec4 Color = { 1.0f,1.0f,1.0f,1.0f };
			ITexture2DManagement::TextureID TextureId = 0;
			ITexture2D::_UInt2 TextureCoordIndex = { 0,0,false };
			std::pair<bool, bool> Flip = { false, false };

		};

		struct AnimationSequence
		{
			std::vector<SpriteTextureColorInit> SpriteTextureColorInits;
			float FrameRate = 0.0f;

		};

		AnimationComponent() = default;
		AnimationComponent(const AnimationComponent&) = default;
		AnimationComponent(ISquare2dActor* actor);

		ISquare2dActor* Actor = nullptr;
		std::vector<AnimationSequence> AnimationSequences;
		size_t NowPlayingAnimationSequence = 0;

		void Start();
		void Shutdown();

		size_t NowActorAnimationFrameShow = 0;
		float FirstPlayAnimationSequenceTime = 0.0f;

	};

	class AnimationManagement
	{
	public:
		static void Start();
		static void Shutdown();

		static void AddAnimationComponent(AnimationComponent* component);
		static void EraseAnimationComponent(AnimationComponent* component);

	};

}

#endif // !_INVENT2DANIMATIONCOMPONENT_

