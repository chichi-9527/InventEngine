#include "common.slang"

[[vk::binding(0,0)]] ConstantBuffer<FrameData> cbFrame;

[[vk::binding(0,1)]] SamplerState gGlobalSampler;
[[vk::binding(1,1)]] StructuredBuffer<MeshMaterialData> gMeshMaterials;
[[vk::binding(2,1)]] Texture2D gTexture[];

[[vk::binding(0,2)]] StructuredBuffer<Vertex> gGlobalVertices;

[SpecializationConstant]
const int SHADOW_BLEND_MODE = 0;

struct VSOutput
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
    nointerpolation uint32_t MaterialID : TEXCOORD1;
};

[shader("vertex")]
VSOutput vertexMain(uint32_t vertexID : SV_VertexID)
{
    VSOutput output;
    Vertex vertex = gGlobalVertices[vertexID + pc.baseVertexOffset];

    output.Position = mul(cbFrame.shadowMatrix, float4(vertex.Position, 1.0f));
    output.UV = vertex.UV;
    output.MaterialID = pc.meshID;
    return output;
}

[shader("fragment")]
void fragmentMain(VSOutput input)
{
    if(SHADOW_BLEND_MODE == 1){
        MeshMaterialData material = gMeshMaterials[input.MaterialID];
        float alpha = gTexture[material.diffuseTextureId].Sample(gGlobalSampler, input.UV).a;
        if(alpha < 0.5f){
            discard;
        }
    }
}