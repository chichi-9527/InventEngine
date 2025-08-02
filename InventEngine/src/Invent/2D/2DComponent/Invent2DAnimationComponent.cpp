#include "Invent2DAnimationComponent.h"

#include "IEngine.h"

#include <chrono>

namespace INVENT
{
    AnimationComponent::AnimationComponent(ISquare2dActor* actor)
        : Actor(actor)
    {
    }

    void AnimationComponent::Start()
    {
        AnimationManagement::AddAnimationComponent(this);
    }

    void AnimationComponent::Shutdown()
    {
        AnimationManagement::EraseAnimationComponent(this);
    }


    /// <summary>
    /// management  
    /// </summary>
    /// 

    static std::vector<AnimationComponent*> AnimationComponents;
    static std::thread* AnimationThread = nullptr;
    static std::mutex AnimationCompMutex;
    static bool ThreadRunning = true;
    constexpr float FPS = 30.0f;
    constexpr float FrameDuration = 1000.0f / FPS;

    void AnimationManagement::Start()
    {
        AnimationThread = new std::thread([]() {
            while (ThreadRunning)
            {
                auto frame_start = std::chrono::high_resolution_clock::now();

                //////////////////// start update actor ////////////////////////////
                 
                {
                    std::lock_guard<std::mutex> lock(AnimationCompMutex);

                    for (size_t i = 0; i < AnimationComponents.size(); )
                    {
                        auto& comp = AnimationComponents[i];
                        if (comp->Actor == nullptr)
                        {
                            AnimationComponents[i] = AnimationComponents.back();
                            AnimationComponents.pop_back();
                            continue;
                        }

                        if (comp->FirstPlayAnimationSequenceTime == 0.0f)
                        {
                            comp->FirstPlayAnimationSequenceTime = std::chrono::duration<float, std::milli>(frame_start - IEngine::GetEngineStartTimePoint()).count();
                        }
                        else
                        {
                            if (comp->AnimationSequences.size() > comp->NowPlayingAnimationSequence)
                            {
                                auto& sequence = comp->AnimationSequences[comp->NowPlayingAnimationSequence];

                                float time_defrence = std::chrono::duration<float, std::milli>(frame_start - IEngine::GetEngineStartTimePoint()).count() - comp->FirstPlayAnimationSequenceTime;

                                float frame_time = 1000.0f / sequence.FrameRate;

                                size_t now_frame = ((size_t)(time_defrence / frame_time)) % sequence.SpriteTextureColorInits.size();

                                auto& sprite_init = sequence.SpriteTextureColorInits[now_frame];

                                comp->Actor->SetColor(sprite_init.Color);
                                comp->Actor->SetTextureID(sprite_init.TextureId);
                                comp->Actor->SetTextureCoordIndex(sprite_init.TextureCoordIndex);
                                comp->Actor->SetFlip(sprite_init.Flip.first, sprite_init.Flip.second);

                            }
                        }
                        

                        ++i;
                    }
                }



                //////////////////// end update actor ////////////////////////////

                auto frame_end = std::chrono::high_resolution_clock::now();

                float elapsed = std::chrono::duration<float, std::milli>(frame_end - frame_start).count();
                if (elapsed < FrameDuration)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds((int)(FrameDuration - elapsed)));
                }
            }
            });
    }

    void AnimationManagement::Shutdown()
    {
        if (AnimationThread->joinable())
            AnimationThread->join();
        ThreadRunning = false;
        delete AnimationThread;
        AnimationThread = nullptr;
    }

    void AnimationManagement::AddAnimationComponent(AnimationComponent* component)
    {
        std::lock_guard<std::mutex> lock(AnimationCompMutex);
        AnimationComponents.push_back(component);
    }

    void AnimationManagement::EraseAnimationComponent(AnimationComponent * component)
    {
        std::lock_guard<std::mutex> lock(AnimationCompMutex);
        AnimationComponents.erase(std::find(AnimationComponents.begin(), AnimationComponents.end(), component), AnimationComponents.end());
    }

}