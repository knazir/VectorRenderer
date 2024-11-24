struct PSInput
{
    float4 position : SV_POSITION;  // Transformed position (not used here)
    float4 color    : COLOR;        // Input color
};

float4 main(PSInput input) : SV_TARGET
{
    // Output the color as-is
    return input.color;
}

