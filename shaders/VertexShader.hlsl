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
    output.position = float4(input.position, 1.0);
    
    // Pass the color through to the pixel shader
    output.color = input.color;
    
    return output;
}