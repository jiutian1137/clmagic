
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 0
#define NUM_SPOT_LIGHTS 0

#include "LightingUtil.hlsl"

cbuffer cbPerObject : register(b0)
{
    float4x4 WorldMatrix;
};

cbuffer cbMaterial : register(b1)
{
    float4 Diffuse;
    float3 FresnelR0;
    float  Roughness;
}

cbuffer cbPerPass : register(b2)
{                        // offset:
    float4x4 ViewMatrix; //   0
    float4x4 ProjMatrix; //  64
    float3 EyePosition ; // 128 
    float4 Resolution;   // 144 
    float4 AmbientLight; // 160
    float TotalTime = 0.f; // 176
    float DeltaTime = 0.f; // 180
    
    Light Lights[MaxLights];// 184
}

struct VertexOut {
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
};

VertexOut VS(float3 Pos : POSITION, 
             float3 Normal : NORMAL,
             float2 Texcoord : TEXCOORD) {
    VertexOut Vout;
    
    float4 _Position_world = mul(WorldMatrix, float4x1(Pos, 1));
    Vout.PosW    = _Position_world.xyz;
    Vout.NormalW = mul((float3x3)WorldMatrix, Normal);
    Vout.PosH    = mul(mul(ProjMatrix, ViewMatrix), _Position_world);
    
    return (Vout);
}

float4 PS(VertexOut Pin) : SV_Target
{
    Pin.NormalW = normalize(Pin.NormalW);
    float3 _Toeye = normalize(EyePosition - Pin.PosW);
    
    const float _Shininess = 1.f - Roughness;
    Material _Mtl;
    _Mtl.DiffuseAlbedo = Diffuse;
    _Mtl.FresnelR0     = FresnelR0;
    _Mtl.Shininess     = _Shininess;
    
    float4 _AmbientLight = AmbientLight * Diffuse;
    float4 _DirectLight  = ComputeLighting(Lights, _Mtl, Pin.PosW, Pin.NormalW, _Toeye, 1.f);
    
    if (Lights[0].Strength.z == 1.f)
    {
        return float4(1, 0, 0, 1);
    }
    else
    {
        return float4(_AmbientLight.xyz + _DirectLight.xyz, Diffuse.a);
    }
    //return float4(1, 0, 0, 1);
}