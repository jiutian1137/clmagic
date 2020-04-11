
struct common_light_source
{
    float3 power;
    float radius;
    float3 direction;
    float umbra;
    float3 position;
    float penumbra;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 WorldMatrix;
};

cbuffer cbMaterial : register(b1)
{
    float4 Diffuse; // [surface, alpha]
    float3 FresnelR0;
    float Roughness;
}

cbuffer cbPerPass : register(b2)
{ // offset:
    float4x4 ViewMatrix; //   0
    float4x4 ProjMatrix; //  64
    float3 EyePosition; // 128 
    float4 Resolution; // 144 
    float4 AmbientLight; // 160
    float TotalTime = 0.f; // 176
    float DeltaTime = 0.f; // 180
}

#define DirectionalLightsCount 1
cbuffer cbDirectionalLights : register(b3)
{
    common_light_source DirectionalLights[DirectionalLightsCount];
};

#define PointLightsCount 1
cbuffer cbDirectionalLights : register(b4)
{
    common_light_source PointLights[PointLightsCount];
};

#define SpotLightsCount 1
cbuffer cbDirectionalLights : register(b5)
{
    common_light_source SpotLights[SpotLightsCount];
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
};

void VS(float3 Position : POSITION,
    float3 Normal : NORMAL,
    float2 Texcoord : TEXCOORD,
    out float4 Pps : SV_POSITION,
    out float3 N : NORMAL,
    out float3 P : POSITIONT)
{
    
    float4 Pws = mul(WorldMatrix, float4x1(Position, 1));
    P = Pws.xyz;
    N = mul((float3x3) WorldMatrix, Normal);
    Pps = mul(mul(ProjMatrix, ViewMatrix), Pws);
}

float3 directial_klight(common_light_source light)
{
    return light.power;
}

float fdist(common_light_source light, float distance)
{
    const float rate = distance / light.radius;
    return 1.0 - (rate * rate);
}

float3 point_klight(common_light_source light, float distance)
{
    return light.power * fdist(light, distance);
}

float fdir(common_light_source light, float3 L)
{
    return (dot(light.direction, -L) - light.umbra)
         /*----------------------------------------*/
                   / (light.penumbra - light.umbra);
}

float3 spot_klight(common_light_source light, float distance, float3 L)
{
    return light.power * fdist(light, distance) * fdir(light, L);
}

float4 PS(float4 Pps : SV_POSITION, float3 N : NORMAL, float3 P : POSITIONT) : SV_Target
{
    N = normalize(N);
    float3 V = normalize(EyePosition - P);
    
    float3 Ccool = float3(0, 0, 0.55) + 0.25 * Diffuse.xyz;
    float3 Cwarn = float3(0.3, 0.3, 0) + 0.25 * Diffuse.xyz;
    float3 Chighlight = float3(2, 2, 2);
    
    // unlit + klight * lit
    // unlit + dot(L, N)*lit
    //return float4(lerp(lerp(Ccool, Cwarn, t), Chighlight, s), 1);
    float3 unlit = Ccool * 0.2;
    
    float3 result = unlit;
    int i = 0;
    for (; i < DirectionalLightsCount; ++i)
    {
        float3 L = -DirectionalLights[i].direction;
        float3 r = reflect(-L, N);
        float s = saturate(100 * dot(r, V) - 97);
        
        result += dot(N, L) * directial_klight(DirectionalLights[i]) * lerp(Cwarn, Chighlight, s);
    }
    
    for (i = 0; i < PointLightsCount; ++i)
    {
        float3 L = PointLights[i].position - P;
        float d = length(L);
        L /= d;
        float3 r = reflect(-L, N);
        float s = saturate(100 * dot(r, V) - 97);
        
        result += dot(N, L) * point_klight(PointLights[i], d) * lerp(Cwarn, Chighlight, s);
    }
    
    for (i = 0; i < SpotLightsCount; ++i)
    {
        float3 L = PointLights[i].position - P;
        float d = length(L);
        L /= d;
        float3 r = reflect(-L, N);
        float s = saturate(100 * dot(r, V) - 97);
        
        result += dot(N, L) * spot_klight(PointLights[i], d, L) * lerp(Cwarn, Chighlight, s);
    }
    
    
    return float4(result, 1);
    //float4 _Unlit = AmbientLight * Diffuse;
    //float4 _DirectLight = ComputeLighting(Lights, _Mtl, Pin.PosW, Pin.NormalW, _Toeye, 1.f);
    //return float4(_Unlit.xyz + _DirectLight.xyz, Diffuse.a);
    //if (Lights[0].Strength.z == 1.f)
    //{
    //    return float4(1, 0, 0, 1);
    //}
    //else
    //{
    //    return float4(_AmbientLight.xyz + _DirectLight.xyz, Diffuse.a);
    //}    
}