cbuffer Transform : register(b0)
{
    float4x4 WorldViewProj;         // Combined world-view-projection matrix
}

struct VSInput
{
    float3 position : POSITION;     // Vertex position (in object space)
    float4 color    : COLOR;        // Vertex color
};

struct PSInput
{
    float4 position : SV_POSITION;  // Transformed position (in clip space)
    float4 color    : COLOR;        // Passed color
};

PSInput main(VSInput input)
{
    PSInput output;
    
    // Transform the vertex position to clip space
    output.position = mul(WorldViewProj, float4(input.position, 1.0f));
    
    // Pass the color through to the pixel shader
    output.color = input.color;
    
    return output;
}