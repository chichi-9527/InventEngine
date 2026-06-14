

struct UIVertex 
{
    float2 position;
    float2 uv;
    float4 color;
};

[[vk::binding(0, 1)]] SamplerState gGlobalSampler;
[[vk::binding(2, 1)]] Texture2D gTextures[];
 
struct UIPushConstant
{
    uint32_t TextureID;
    uint32_t BaseVertexOffset;
}

[[vk::push_constant]] UIPushConstant pc;

[[vk::binding(0,3)]] StructuredBuffer<UIVertex> gGlobalVertices;

struct VSOutput 
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
    float4 color    : TEXCOORD1;
}

[shader("vertex")]
VSOutput vertexMain(uint32_t vertexID : SV_VertexID) 
{
    VSOutput output;
    uint32_t globalVertexIndex = vertexID + pc.BaseVertexOffset;
    UIVertex v = gGlobalVertices[globalVertexIndex];
    
    output.position = float4(v.position, 0.0, 1.0);
    output.uv = v.uv;
    output.color = v.color;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
    float4 sceneColor = gTextures[pc.TextureID].Sample(gGlobalSampler, input.uv);
    return sceneColor * input.color;
}