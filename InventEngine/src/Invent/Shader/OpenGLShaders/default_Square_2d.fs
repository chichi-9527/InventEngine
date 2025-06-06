#version 450 core

layout (location = 0) out vec4 color; // Output color to be used in the fragment shader.

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
	
}

layout (location = 0) in VertexOutput Input; // Input from vertex shader, contains color, texture coordinates, and texture index.

layout (binding = 0) uniform sampler2D u_Textures[32]; // 32 textures, each with 4 channels (RGBA) and 2D dimensions (width and height)


void main()
{
    vec4 texColor = Input.Color; // Get the color from the vertex shader.
    
    switch (int(Input.TexIndex)) // Switch statement to select the correct texture based on the texture index.
    {
        case 0: texColor *= texture(u_Textures[0], Input.TexCoord); break; // Multiply the color by the texture at index 0.
        case 1: texColor *= texture(u_Textures[1], Input.TexCoord); break; // Multiply the color by the texture at index 1.
        case 2: texColor *= texture(u_Textures[2], Input.TexCoord); break; // Multiply the color by the texture at index 2.
        case 3: texColor *= texture(u_Textures[3], Input.TexCoord); break; // Multiply the color by the texture at index 3.
        case 4: texColor *= texture(u_Textures[4], Input.TexCoord); break; // Multiply the color by the texture at index 4.
        case 5: texColor *= texture(u_Textures[5], Input.TexCoord); break; // Multiply the color by the texture at index 5.
        case 6: texColor *= texture(u_Textures[6], Input.TexCoord); break; // Multiply the color by the texture at index 6.
        case 7: texColor *= texture(u_Textures[7], Input.TexCoord); break; // Multiply the color by the texture at index 7.
        case 8: texColor *= texture(u_Textures[8], Input.TexCoord); break; // Multiply the color by the texture at index 8.
        case 9: texColor *= texture(u_Textures[9], Input.TexCoord); break; // Multiply the color by the texture at index 9.
        case 10: texColor *= texture(u_Textures[10], Input.TexCoord); break; // Multiply the color by the texture at index 10.
        case 11: texColor *= texture(u_Textures[11], Input.TexCoord); break; // Multiply the color by the texture at index 11.
        case 12: texColor *= texture(u_Textures[12], Input.TexCoord); break; // Multiply the color by the texture at index 12.
        case 13: texColor *= texture(u_Textures[13], Input.TexCoord); break; // Multiply the color by the texture at index 13.
        case 14: texColor *= texture(u_Textures[14], Input.TexCoord); break; // Multiply the color by the texture at index 14.
        case 15: texColor *= texture(u_Textures[15], Input.TexCoord); break; // Multiply the color by the texture at index 15.
        case 16: texColor *= texture(u_Textures[16], Input.TexCoord); break; // Multiply the color by the texture at index 16.
        case 17: texColor *= texture(u_Textures[17], Input.TexCoord); break; // Multiply the color by the texture at index 17.
        case 18: texColor *= texture(u_Textures[18], Input.TexCoord); break; // Multiply the color by the texture at index 18.
        case 19: texColor *= texture(u_Textures[19], Input.TexCoord); break; // Multiply the color by the texture at index 19.
        case 20: texColor *= texture(u_Textures[20], Input.TexCoord); break; // Multiply the color by the texture at index 20.
        case 21: texColor *= texture(u_Textures[21], Input.TexCoord); break; // Multiply the color by the texture at index 21.
        case 22: texColor *= texture(u_Textures[22], Input.TexCoord); break; // Multiply the color by the texture at index 22.
        case 23: texColor *= texture(u_Textures[23], Input.TexCoord); break; // Multiply the color by the texture at index 23.
        case 24: texColor *= texture(u_Textures[24], Input.TexCoord); break; // Multiply the color by the texture at index 24.
        case 25: texColor *= texture(u_Textures[25], Input.TexCoord); break; // Multiply the color by the texture at index 25.
        case 26: texColor *= texture(u_Textures[26], Input.TexCoord); break; // Multiply the color by the texture at index 26.
        case 27: texColor *= texture(u_Textures[27], Input.TexCoord); break; // Multiply the color by the texture at index 27.
        case 28: texColor *= texture(u_Textures[28], Input.TexCoord); break; // Multiply the color by the texture at index 28.
        case 29: texColor *= texture(u_Textures[29], Input.TexCoord); break; // Multiply the color by the texture at index 29.
        case 30: texColor *= texture(u_Textures[30], Input.TexCoord); break; // Multiply the color by the texture at index 30.
        case 31: texColor *= texture(u_Textures[31], Input.TexCoord); break; // Multiply the color by the texture at index 31.
    }
    
    color = texColor; // Set the output color to the color multiplied by the texture.

}