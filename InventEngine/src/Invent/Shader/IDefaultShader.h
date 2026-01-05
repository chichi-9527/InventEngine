#ifndef _IDEFAULTSHADER_
#define _IDEFAULTSHADER_

namespace INVENT
{
	class IDefaultShader 
	{
		friend class IShaderManagement;

//layout(std140, binding = 0) uniform Camera
//{
//	mat4 u_ViewProjection;
//};

		// Default 2D square shader
		constexpr static const char* DefaultSquare2DVertexShader = R"(
			#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_ViewProjection2D;
};

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	Output.TexIndex = a_TexIndex;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
		)";

		constexpr static const char* DefaultSquare2DFragmentShader = R"(
			#version 460 core

layout(location = 0) out vec4 color;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
};

layout (location = 0) in VertexOutput Input;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	vec4 texColor = Input.Color;

	switch(int(Input.TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], Input.TexCoord); break;
		case  1: texColor *= texture(u_Textures[ 1], Input.TexCoord); break;
		case  2: texColor *= texture(u_Textures[ 2], Input.TexCoord); break;
		case  3: texColor *= texture(u_Textures[ 3], Input.TexCoord); break;
		case  4: texColor *= texture(u_Textures[ 4], Input.TexCoord); break;
		case  5: texColor *= texture(u_Textures[ 5], Input.TexCoord); break;
		case  6: texColor *= texture(u_Textures[ 6], Input.TexCoord); break;
		case  7: texColor *= texture(u_Textures[ 7], Input.TexCoord); break;
		case  8: texColor *= texture(u_Textures[ 8], Input.TexCoord); break;
		case  9: texColor *= texture(u_Textures[ 9], Input.TexCoord); break;
		case 10: texColor *= texture(u_Textures[10], Input.TexCoord); break;
		case 11: texColor *= texture(u_Textures[11], Input.TexCoord); break;
		case 12: texColor *= texture(u_Textures[12], Input.TexCoord); break;
		case 13: texColor *= texture(u_Textures[13], Input.TexCoord); break;
		case 14: texColor *= texture(u_Textures[14], Input.TexCoord); break;
		case 15: texColor *= texture(u_Textures[15], Input.TexCoord); break;
		case 16: texColor *= texture(u_Textures[16], Input.TexCoord); break;
		case 17: texColor *= texture(u_Textures[17], Input.TexCoord); break;
		case 18: texColor *= texture(u_Textures[18], Input.TexCoord); break;
		case 19: texColor *= texture(u_Textures[19], Input.TexCoord); break;
		case 20: texColor *= texture(u_Textures[20], Input.TexCoord); break;
		case 21: texColor *= texture(u_Textures[21], Input.TexCoord); break;
		case 22: texColor *= texture(u_Textures[22], Input.TexCoord); break;
		case 23: texColor *= texture(u_Textures[23], Input.TexCoord); break;
		case 24: texColor *= texture(u_Textures[24], Input.TexCoord); break;
		case 25: texColor *= texture(u_Textures[25], Input.TexCoord); break;
		case 26: texColor *= texture(u_Textures[26], Input.TexCoord); break;
		case 27: texColor *= texture(u_Textures[27], Input.TexCoord); break;
		case 28: texColor *= texture(u_Textures[28], Input.TexCoord); break;
		case 29: texColor *= texture(u_Textures[29], Input.TexCoord); break;
		case 30: texColor *= texture(u_Textures[30], Input.TexCoord); break;
		case 31: texColor *= texture(u_Textures[31], Input.TexCoord); break;
	}
	color = texColor;
}
		)";

		// End Default 2D square shader


		constexpr static const char* DefaultTextVertexShader = R"(
		#version 460 core
		layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_ViewProjection2D;
};

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	Output.TexIndex = a_TexIndex;

	gl_Position = u_ViewProjection2D * vec4(a_Position, 1.0);
}
		)";
		constexpr static const char* DefaultTextFragmentShader = R"(
		#version 460 core
		
layout(location = 0) out vec4 color;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
};

layout (location = 0) in VertexOutput Input;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	vec4 sampled = Input.Color;

	switch(int(Input.TexIndex))
	{
		case  0: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 0], Input.TexCoord).r); break;
		case  1: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 1], Input.TexCoord).r); break;
		case  2: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 2], Input.TexCoord).r); break;
		case  3: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 3], Input.TexCoord).r); break;
		case  4: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 4], Input.TexCoord).r); break;
		case  5: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 5], Input.TexCoord).r); break;
		case  6: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 6], Input.TexCoord).r); break;
		case  7: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 7], Input.TexCoord).r); break;
		case  8: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 8], Input.TexCoord).r); break;
		case  9: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[ 9], Input.TexCoord).r); break;
		case 10: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[10], Input.TexCoord).r); break;
		case 11: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[11], Input.TexCoord).r); break;
		case 12: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[12], Input.TexCoord).r); break;
		case 13: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[13], Input.TexCoord).r); break;
		case 14: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[14], Input.TexCoord).r); break;
		case 15: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[15], Input.TexCoord).r); break;
		case 16: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[16], Input.TexCoord).r); break;
		case 17: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[17], Input.TexCoord).r); break;
		case 18: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[18], Input.TexCoord).r); break;
		case 19: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[19], Input.TexCoord).r); break;
		case 20: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[20], Input.TexCoord).r); break;
		case 21: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[21], Input.TexCoord).r); break;
		case 22: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[22], Input.TexCoord).r); break;
		case 23: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[23], Input.TexCoord).r); break;
		case 24: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[24], Input.TexCoord).r); break;
		case 25: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[25], Input.TexCoord).r); break;
		case 26: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[26], Input.TexCoord).r); break;
		case 27: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[27], Input.TexCoord).r); break;
		case 28: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[28], Input.TexCoord).r); break;
		case 29: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[29], Input.TexCoord).r); break;
		case 30: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[30], Input.TexCoord).r); break;
		case 31: sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures[31], Input.TexCoord).r); break;
	}
		color = Input.Color * sampled;
}  
		)";

		constexpr static const char* Default3DVertexShader = R"(
			#version 460 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec3 a_TextureIDs;

struct InstanceData
{
    mat4 normalMatrix;
    vec4 TextureIDs;
};

layout(std430, binding = 0) buffer InstanceDataBuffer
{
    InstanceData instances[];
};

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
    mat4 u_ViewProjection2D;
};

struct VertexOutput
{
    vec2 TexCoord;
    float diffuseTextureID;
};

layout (location = 0) flat out VertexOutput Output;

void main()
{
    InstanceData data = instances[gl_InstanceID];
    
    Output.diffuseTextureID = data.TextureIDs.r;
    Output.TexCoord = a_TexCoord;
    
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
		)";

		constexpr static const char* Default3DFragmentShader = R"(
			#version 460 core
layout (binding = 0) uniform sampler2D u_Textures[32];

struct VertexOutput
{
    vec2 TexCoord;
    float diffuseTextureID;
};

layout (location = 0) flat in VertexOutput Input;

layout (location = 0) out vec4 FragColor;

void main()
{
    int texID = int(Input.diffuseTextureID);
    texID = clamp(texID, 0, 31);
    
    vec4 diffuse = texture(u_Textures[ texID], Input.TexCoord);

    diffuse.rgb = pow(diffuse.rgb, vec3(1.0/2.2));
    
    FragColor = diffuse;
}
		)";
		
	};
		/*constexpr static const char* Default3DVertexShader = R"(
			#version 460 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec2 a_TexCoords;
layout (location = 4) in vec3 a_Tangent;
layout (location = 5) in vec3 a_Bitangent;

struct InstanceData
{
	mat4 normalMatrix;
	float diffuseTextureID;
	float normalTextureID;
	float specularTextureID;
	float emissionTextureID;
	float roughnessTextureID;
	float aoTextureID;
};

layout(std430, binding = 0) buffer InstanceDataBuffer
{
	InstanceData instances[];
};

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_ViewProjection2D;
	vec3 u_LightColor;
	vec3 u_CameraPosition;
};

struct VertexOutput
{
	vec3 LightColor;
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
	mat3 TBN;
	vec3 CameraPos;
	float diffuseTextureID;
	float normalTextureID;
	float specularTextureID;
	float emissionTextureID;
	float roughnessTextureID;
	float aoTextureID;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	InstanceData data = instances[gl_BaseInstance];

	Output.diffuseTextureID = data.diffuseTextureID;
	Output.normalTextureID = data.normalTextureID;
	Output.specularTextureID = data.specularTextureID;
	Output.emissionTextureID = data.emissionTextureID;
	Output.roughnessTextureID = data.roughnessTextureID;
	Output.aoTextureID = data.aoTextureID;
	
	Output.LightColor = u_LightColor;
    Output.TexCoord = a_TexCoords;
	Output.Normal = normalize(mat3(data.normalMatrix) * a_Normal);
	Output.FragPos = a_Position;
	Output.CameraPos = u_CameraPosition;
	
	vec3 T = normalize(mat3(data.normalMatrix) * a_Tangent);
	vec3 N = normalize(mat3(data.normalMatrix) * a_Normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	Output.TBN = mat3(T, B, N);
	
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
		)";

		constexpr static const char* Default3DFragmentShader = R"(
			#version 460 core
layout (binding = 0) uniform sampler2D u_Textures[32];

uniform vec3 uLightPos;
uniform float uShininess;
uniform vec3 uEmissionColor;
uniform float uEmissionStrength;

struct VertexOutput
{
	vec3 LightColor;
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
	mat3 TBN;
	vec3 CameraPos;
	float diffuseTextureID;
	float normalTextureID;
	float specularTextureID;
	float emissionTextureID;
	float roughnessTextureID;
	float aoTextureID;
};

layout (location = 0) in VertexOutput Input;

vec3 getNormalFromMap(int normalTextureID)
{
	vec3 tangentNormal = texture(u_Textures[normalTextureID], Input.TexCoord).xyz * 2.0 - 1.0;
	return normalize(Input.TBN * tangentNormal);
}

layout (location = 0) out vec4 FragColor;

void main()
{
	
    vec4 diffuse = texture(u_Textures[int(Input.diffuseTextureID)], Input.TexCoord);
    float specularIntensity = texture(u_Textures[int(Input.specularTextureID)], Input.TexCoord).r;
    vec3 emission = texture(u_Textures[int(Input.emissionTextureID)], Input.TexCoord).rgb;
    float roughness = texture(u_Textures[int(Input.roughnessTextureID)], Input.TexCoord).r;
    float ao = texture(u_Textures[int(Input.aoTextureID)], Input.TexCoord).r;
	
    vec3 normal;
    if (Input.normalTextureID >= 0) {
        normal = getNormalFromMap(int(Input.normalTextureID));
    } else {
        normal = normalize(Input.Normal);
    }
    
    vec3 lightDir = normalize(uLightPos - Input.FragPos);
    vec3 viewDir = normalize(Input.CameraPos - Input.FragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = diff * Input.LightColor * diffuse.rgb;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess * (1.0 - roughness));
    vec3 specularColor = spec * specularIntensity * Input.LightColor;
    
    vec3 emissionColor = emission * uEmissionColor * uEmissionStrength;
    
    vec3 ambientColor = vec3(0.1) * diffuse.rgb * ao;
    
    vec3 result = ambientColor + diffuseColor + specularColor + emissionColor;
    
    FragColor = vec4(result, diffuse.a);
}
		)";
		
	};*/
}

#endif // !_IDEFAULTSHADER_
