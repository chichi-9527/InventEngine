#include "common.slang"

[[vk::binding(0, 1)]] SamplerState gGlobalSampler;
[[vk::binding(2, 1)]] Texture2D gTextures[];

struct VSOutput {
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

[shader("vertex")]
VSOutput vertexMain(uint32_t vertexID : SV_VertexID) {
    VSOutput output;
    
    output.uv = float2((vertexID << 1) & 2, vertexID & 2);
    output.position = float4(output.uv * 2.0 - 1.0, 0.0, 1.0);
    output.uv.y = 1.0 - output.uv.y; // 修正 Vulkan 的 Y 軸朝向
    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
    // 透過 Push Constant 傳入的動態主畫面貼圖 ID 進行採樣
    float4 sceneColor = gTextures[pc.finalColorBufferId].Sample(gGlobalSampler, input.uv);
    
    // 執行電影級 ACES Tone Mapping 曲線擬合，提升畫面質感
    float3 x = sceneColor.rgb;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    float3 filmColor = saturate((x * (a * x + b)) / (x * (c * x + d) + e));
    
    //return float4(filmColor, sceneColor.a);
    return sceneColor;
}