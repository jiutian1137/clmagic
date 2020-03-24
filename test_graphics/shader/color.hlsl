
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
};

struct VertexOut
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VS(float3 Pos : POSITION0, 
             float4 Color : COLOR)
{
    VertexOut Vout;
    
    Vout.Pos = mul(gWorldViewProj, float4x1(Pos, 1));
    //if (abs(gWorldViewProj[2][3] - 4.f) <= 0.02)
    //{
    //    Vout.Color = float4(1.f, 1.f, 1.f, 1.f);
    //}
    //else
    //{
    Vout.Color = Color;
    //}
    return (Vout);
}

float4 PS(VertexOut Pin) : SV_Target
{
    return (Pin.Color);
}