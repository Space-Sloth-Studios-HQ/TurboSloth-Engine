struct VertexInput {
    [vk::location(0)] float3 position : POSITION;
    [vk::location(1)] float3 color    : COLOR;
    [vk::location(2)] float3 normal   : NORMAL;
};

struct VertexOutput {
    float3 color;
    float4 sv_position : SV_Position;
    float3 worldNormal;
};

struct PushConstants {
    float4x4 projection;
    float4x4 view;
    float4x4 model;
};
[vk::push_constant] ConstantBuffer<PushConstants> pc;

static const float3 lightDir = normalize(float3(0.5, 1.0, 0.3));

[shader("vertex")]
VertexOutput vertMain(VertexInput input) {
    VertexOutput output;
    float4x4 mvp = mul(mul(pc.projection, pc.view), pc.model);
    output.sv_position = mul(mvp, float4(input.position, 1.0));
    output.color = input.color;
    output.worldNormal = mul((float3x3)pc.model, input.normal);
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target
{
    float ndot1 = max(dot(normalize(inVert.worldNormal), lightDir), 0.0);
    float3 color = inVert.color * (0.15 + 0.85 * ndot1);
    return float4(color, 1.0);
}