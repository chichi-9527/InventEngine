
// 顶点结构体
struct Vertex
{
    float3 Position;
    float3 Normal;
    float2 UV;
};

// light struct
struct PointLightData
{
    float3 Position;
    float Radius;
    float3 Color;
    float Intensity;
};

// global material struct
struct MeshMaterialData
{
    uint32_t diffuseTextureId;
    uint32_t normalTextureId; // 法线贴图
    uint32_t specularTextureId; // 镜面贴图
    uint32_t emissionTextureId; // 发光贴图

    uint32_t shadowMapTextureId; // 阴影贴图
    uint32_t blendMode; // 0: 不透明, 1: 半透明, 2: 透明

    uint32_t padding[2];
};

struct FrameData
{
    float4x4 mainViewProjection;
    float4x4 offscreenViewProjection;
    float4x4 uiViewProjection;
    float4x4 shadowMatrix;

    // 平行光数据
    float3 LightDirection;
    float3 LightColor;

    // 点光数据
    uint32_t pointLightCount;

    uint32_t padding;
    float4x4 padding1;
};

struct PushConstants
{
    uint32_t meshID;
    uint32_t baseVertexOffset;
    uint32_t finalColorBufferId;
};

[[vk::push_constant]] PushConstants pc;

float3 CalculateGlobalLighting(
    float3 albedo,
    float3 normalDetail,
    float3 worldNormal,
    float3 worldPosition,
    float3 emission,
    float shadow,
    FrameData frameData,
    StructuredBuffer<PointLightData> pointLightData
)
{
    float3 blenderNormal = normalize(worldNormal + normalDetail * 0.5f);
    float3 totalLight = float3(0.0f);

    // 平行光
    float diffuse = max(0.0f, dot(blenderNormal, normalize(frameData.LightDirection)));
    totalLight += albedo * frameData.LightColor * diffuse * shadow;

    // 点光
    for(uint32_t i = 0; i < frameData.pointLightCount; i++){
        PointLightData pointLight = pointLightData[i];
        float dist = distance(worldPosition, pointLight.Position);

        if(dist > pointLight.Radius){
            continue;
        }

        float attenuation = saturate(1.0f - (dist * dist));
        attenuation *= attenuation;

        float3 lightDir = normalize(pointLight.Position - worldPosition);
        float pointDiff = max(0.0f, dot(blenderNormal, lightDir));

        totalLight += albedo * pointLight.Color * (pointDiff * pointLight.Intensity * attenuation);

    }

    return totalLight + emission;

}