struct VertexInput {
    [vk::location(0)] float3 position : POSITION;
    [vk::location(1)] float3 color    : COLOR;
};

struct VertexOutput {
    float3 color;
    float4 sv_position : SV_Position;
};

struct PushConstants {
    float4x4 mvp;
};
[vk::push_constant] ConstantBuffer<PushConstants> pc;

[shader("vertex")]
VertexOutput vertMain(VertexInput input) {
    VertexOutput output;
    output.sv_position = mul(pc.mvp, float4(input.position, 1.0));
    output.color = input.color;
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target
{
    float3 color = inVert.color;
    return float4(color, 1.0);
}