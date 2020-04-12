
struct common_light_source {
    float3 power;
    float  radius;
    float3 direction;
    float  umbra;
    float3 position;
    float  penumbra;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 WorldMatrix;
};

cbuffer cbMaterial : register(b1)
{
    float4 Diffuse;// [surface, alpha]
    float3 FresnelR0;
    float  Roughness;
}

cbuffer cbPerPass : register(b2)
{                        // offset:
    float4x4 ViewMatrix; //   0
    float4x4 ProjMatrix; //  64
    float4x4 TempMatrix; 
    float3 EyePosition ; // 128 
    float4 Resolution;   // 144 
    float4 AmbientLight; // 160
    float TotalTime = 0.f; // 176
    float DeltaTime = 0.f; // 180
}

#define DirectionalLightsCount 1
cbuffer cbDirectionalLights : register(b3) {
    common_light_source DirectionalLights[DirectionalLightsCount];
};

#define PointLightsCount 1
cbuffer cbDirectionalLights : register(b4) {
    common_light_source PointLights[PointLightsCount];
};

#define SpotLightsCount 1
cbuffer cbDirectionalLights : register(b5) {
    common_light_source SpotLights[SpotLightsCount];
};


void VS(float3 Position : POSITION, 
    float3 Normal   : NORMAL, 
    float2 Texcoord : TEXCOORD, 
    out float4 Pps : SV_POSITION,
    out float3 N : NORMAL, 
    out float3 P : POSITIONT) {
    
    float4 Pws = mul(WorldMatrix, float4x1(Position, 1));
    P = Pws.xyz;
    N = mul((float3x3)WorldMatrix, Normal);
    Pps = mul(mul(ProjMatrix, ViewMatrix), Pws);
}

float3 directial_klight(common_light_source light) {
    return light.power;
}

float fdist(common_light_source light, float distance) {
    const float rate = distance / light.radius;
    return saturate(1.0 - rate * rate);
}

float3 point_klight(common_light_source light, float distance) {
    return light.power * fdist(light, distance);
}

float fdir(common_light_source light, float3 L) {
    float t = saturate( (dot(light.direction, -L) - light.umbra) / (light.penumbra - light.umbra) );
    return t * t * (3.0 - 2.0 * t);
}

float3 spot_klight(common_light_source light, float distance, float3 L) {
    return light.power * fdist(light, distance) * fdir(light, L);
}


void ShadowVS(float3 Pin : POSITION, float3 Nin : NORMAL, float2 UVin : TEXCOORD, out float4 Pps : SV_POSITION) {
    float4x4 PVSW = mul(mul(mul(ProjMatrix, ViewMatrix), TempMatrix), WorldMatrix);
    //float4x4 PVW = mul(mul(ProjMatrix, ViewMatrix), WorldMatrix);
    Pps = mul(PVSW, float4x1(Pin, 1));
}

float4 ShadowPS() : SV_Target {
    return float4(1, 0, 0, 1);
}

float4 PS(float4 Pps : SV_POSITION, float3 N : NORMAL, float3 P : POSITIONT) : SV_Target
{
    N = normalize(N);
    float3 V = normalize(EyePosition - P);
    
    float3 Ccool = float3(0, 0, 0.55)  + 0.25 * Diffuse.xyz;
    float3 Cwarn = float3(0.3, 0.3, 0) + 0.25 * Diffuse.xyz;
    float3 Chighlight = float3(1, 1, 1);
    
    // unlit + klight * lit
    // unlit + dot(L, N)*lit
    //float3 L = normalize(float3(1, 1, 0));
    //float  t = (saturate(dot(N, L)) + 1) / 2;
    //float3 r = reflect(-L, N);
    //float  s = saturate(100 * dot(r, V) - 97);
    //return float4(lerp(lerp(Ccool, Cwarn, t), Chighlight, s), 1);
    
    float3 result = float3(0, 0, 0);
    int i = 0;
    for (; i < DirectionalLightsCount; ++i)
    {
        float3 L = -DirectionalLights[i].direction;
        float t = (dot(N, L) + 1) / 2;
        float3 r = reflect(-L, N);
        float s = saturate(100 * dot(r, V) - 97);
        
        result = lerp(lerp(Ccool, Cwarn, t), Chighlight, s);
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
    
    //for (i = 0; i < SpotLightsCount; ++i) {
    //    float3 L = PointLights[i].position - P;
    //    float  d = length(L);
    //    L /= d;
    //    float3 r = reflect(-L, N);
    //    float  s = saturate(100 * dot(r, V) - 97);
        
    //    result += dot(N, L) * spot_klight(SpotLights[i], d, L) * lerp(Cwarn, Chighlight, s);
    //}
    
    
    return float4(result, 1);
}