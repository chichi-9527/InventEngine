#include "common.slang"

[[vk::binding(0, 0)]] ConstantBuffer<FrameData> cbFrame;
[[vk::binding(1, 0)]] StructuredBuffer<PointLightData> gPointLights;

[[vk::binding(0, 1)]] SamplerState gGlobalSampler;
[[vk::binding(1, 1)]] StructuredBuffer<MeshMaterialData> gMaterials;
[[vk::binding(2, 1)]] Texture2D gTextures[]; 

[[vk::binding(0, 2)]] StructuredBuffer<Vertex> gGlobalVertices;

[SpecializationConstant]
const int BLEND_MODE = 0;

struct VSOutput {
    float4 position : SV_Position;
    float4 shadowCoord : TEXCOORD0;
    float2 uv       : TEXCOORD1;
    float3 worldNormal : NORMAL;
    float3 worldPos    : POSITION; // 傳遞世界座標以計算點光源距離
    nointerpolation uint32_t materialID : SV_InstanceID;
};

[shader("vertex")]
VSOutput vertexMain(uint32_t vertexID : SV_VertexID) {
    VSOutput output;
    uint32_t globalVertexIndex = vertexID + pc.baseVertexOffset;
    Vertex v = gGlobalVertices[globalVertexIndex];
    
    output.position = mul(cbFrame.mainViewProjection, float4(v.Position, 1.0));
    output.shadowCoord = mul(cbFrame.shadowMatrix, float4(v.Position, 1.0));
    output.uv = v.UV;
    output.worldNormal = v.Normal; 
    output.worldPos = v.Position; 
    output.materialID = pc.meshID;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
    MeshMaterialData mat = gMaterials[input.materialID];
    
    // 💡 1. 採樣基礎色彩（如果是魔法傳送門，此時 diffuseTexId 裝的就是 離屏 RT 的 ID！）
    float4 albedo = gTextures[mat.diffuseTextureId].Sample(gGlobalSampler, input.uv);
    
    if (BLEND_MODE == 1) {
        if (albedo.a < 0.5) discard;
    }
    
    float3 normalDetail = gTextures[mat.normalTextureId].Sample(gGlobalSampler, input.uv).rgb * 2.0 - 1.0;
    float4 specRough = gTextures[mat.specularTextureId].Sample(gGlobalSampler, input.uv);
    float3 emission = gTextures[mat.emissionTextureId].Sample(gGlobalSampler, input.uv).rgb;

    float3 projCoords = input.shadowCoord.xyz / input.shadowCoord.w;
    float closestDepth = gTextures[mat.shadowMapTextureId].Sample(gGlobalSampler, projCoords.xy).r;
    float shadow = projCoords.z > closestDepth ? 0.3 : 1.0;

    float3 finalColor = CalculateGlobalLighting(
        albedo.rgb, normalDetail, input.worldNormal, input.worldPos, 
        emission, shadow, cbFrame, gPointLights
    );
    
    return float4(finalColor, albedo.a);
}