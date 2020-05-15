#define SCALAR  float
#define VECTOR2 float2
#define VECTOR3 float3
#define VECTOR4 float4
#define MATRIX4x4 float4x4
#include "../../src/clmagic/calculation/cg/rendering.hlsl"

/*- - - - - - - - - - - - - - - - - - - - - - - - VS - - - - - - - -  -- - - - - - - - - -  - - - - - - - - -*/
cbuffer PER_PASS : register(b0)
{                    // offset:
    MATRIX4x4 Mview; // 0*SCALAR
    MATRIX4x4 Mproj; // 16*SCALAR
    VECTOR3   Peye ;  // 32*SCALAR 
    SCALAR __pand0;
    VECTOR2 ResoLution; // 36*SCALAR 
    SCALAR  TotaLTime;
    SCALAR  DeLtaTime;
}

cbuffer PER_OBJECT : register(b1) {
    MATRIX4x4 Mworld;
};

void VS(float3 Position : POSITION, float3 NormaL : NORMAL, float2 Texcoord : TEXCOORD,
    out float4 Pps : SV_POSITION,
    out float3 N : NORMAL,
    out float3 P : POSITIONT,
    out float2 UV : TEXCOORD)
{
    UV  = Texcoord;
    float4 Pws = mul(Mworld, float4x1(Position, 1));
    P   = Pws.xyz;
    N   = mul((float3x3) Mworld, NormaL);
    Pps = mul(mul(Mproj, Mview), Pws);
}


/*- - - - - - - - - - - - - - - - - - - - - - - - PS - - - - - - - -  -- - - - - - - - - -  - - - - - - - - -*/

Texture2D    DiffuseMap    : register(t0);
SamplerState LinearSampler : register(s0);

cbuffer PER_MATERIAL : register(b2) {
    surface Mtl;
};

cbuffer RENDERING : register(b3) {
    // ...
    environment  Env;
    // Lights
    Light_source Lights[3];
};

SCALAR fdir(Light_source Light, VECTOR3 L) {
    SCALAR thetaS = acos(dot(Light.direction, -L));
    return saturate((Light.penumbra - thetaS) / (Light.penumbra - Light.umbra + 0.01));
}



float4 PS(float4 Pps : SV_POSITION, float3 N : NORMAL, float3 P : POSITIONT, float2 UV : TEXCOORD) : SV_TARGET {
    VECTOR3 unlit = VECTOR3(0,0,0);
    VECTOR3 lit   = VECTOR3(0,0,0);
    
    VECTOR3 Pss2 = DiffuseMap.Sample(LinearSampler, UV).xyz;
    
    VECTOR3 V  = Peye - P;
    SCALAR  Vd = length(V);
            V /= Vd;
    for (int i = 0; i != 3; ++i) {
        VECTOR3 L  = Lights[i].position - P;
        SCALAR  Ld = length(L);
                L /= Ld;
        
        SCALAR  I   = max(0, Lights[i].intensity - Env.particles_ratio * (Ld + Vd));
        SCALAR  att = I / Lights[i].intensity;
        if (att < 0.01) continue;
        att *= fdir(Lights[i], L);
        if (att < 0.01)  continue;
        
        VECTOR3 CLight = Lights[i].color * att;
        
        if (false) {
            lit += sin(TotaLTime * 50.0) * tri_Ace_BRDF(Mtl.subsurface_albedo, 1 - Mtl.roughness_x, Mtl.reflect_value, N, V, L) * CLight * saturate(dot(N, L));
        } else {
            lit += tri_Ace_BRDF(Pss2, 1 - Mtl.roughness_x, Mtl.reflect_value, N, V, L) * CLight * saturate(dot(N, L));
        }
    }
    lit *= 3.14159;
    
    return float4(unlit + lit, 1.0f);
}