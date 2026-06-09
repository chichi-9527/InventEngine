#include "common.slang"

[[vk::binding(0, 0)]] ConstantBuffer<FrameData> cbFrame;
[[vk::binding(1, 0)]] StructuredBuffer<PointLightData> gPointLights;
[[vk::binding(0, 1)]] SamplerState gGlobalSampler;
[[vk::binding(1, 1)]] StructuredBuffer<MeshMaterialData> gMaterials;
[[vk::binding(2, 1)]] Texture2D gTextures[];
[[vk::binding(0, 2)]] StructuredBuffer<Vertex> gGlobalVertices;

[SpecializationConstant]
const int BLEND_MODE = 0;
[SpecializationConstant]
const int PRESET_EFFECT = 0;

struct VSOutput {
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
    float3 worldNormal : NORMAL;
    float3 worldPos    : POSITION;
    nointerpolation uint32_t materialID : TEXCOORD1;
};

[shader("vertex")]
VSOutput vertexMain(uint32_t vertexID : SV_VertexID) {
    VSOutput output;
    uint32_t globalVertexIndex = vertexID + pc.baseVertexOffset;
    Vertex v = gGlobalVertices[globalVertexIndex];
    
    output.position = mul(cbFrame.offscreenViewProjection, float4(v.Position, 1.0));
    output.uv = v.UV;
    output.worldNormal = v.Normal;
    output.worldPos = v.Position;
    output.materialID = pc.meshID;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
    MeshMaterialData mat = gMaterials[input.materialID];

    float4 albedo = gTextures[mat.diffuseTextureId].Sample(gGlobalSampler, input.uv);
    if (BLEND_MODE == 1 && albedo.a < 0.5) discard;

    float3 normalDetail = gTextures[mat.normalTextureId].Sample(gGlobalSampler, input.uv).rgb * 2.0 - 1.0;
    float3 emission = gTextures[mat.emissionTextureId].Sample(gGlobalSampler, input.uv).rgb;
    float shadow = 1.0; 

     float3 litColor = CalculateGlobalLighting(
        albedo.rgb, normalDetail, input.worldNormal, input.worldPos, 
        emission, shadow, cbFrame, gPointLights
    );
    
    // 處理預設特殊效果修改
    float4 finalColor =  float4(litColor, albedo.a);
    if (PRESET_EFFECT == 1) { // 灰階
        float gray = dot(finalColor.rgb, float3(0.299, 0.587, 0.114));
        finalColor = float4(gray, gray, gray, finalColor.a);
    } 
    else if (PRESET_EFFECT == 2) { // 反相
        finalColor = float4(1.0 - finalColor.rgb, finalColor.a);
    }
    
    return finalColor;
}