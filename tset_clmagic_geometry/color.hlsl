
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbPerPass : register(b3)
{
    float4x4 view_matrix; // 64
    float4x4 proj_matrix; // 128
    float4 eye_position; // 144
    float4 resolution; // 160
    float total_time = 0.f; // 164
    float delta_time = 0.f;
}

struct VertexOut
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VS(float3 Pos : POSITION,
             float4 Color : COLOR)
{
    VertexOut Vout;
    
    Vout.Pos =
        mul(proj_matrix,
            mul(view_matrix,
                mul(gWorld, float4x1(Pos, 1))));
    Vout.Color = Color;
    return (Vout);
}

float4 PS(VertexOut Pin) : SV_Target
{
    //return float4(1, 0, 0, 1);
    return (Pin.Color);
}