#include "Header.hlsl"

/*- - - - - - - - - - - - - - - - - - - - - - - - VS - - - - - - - -  -- - - - - - - - - -  - - - - - - - - -*/

cbuffer PER_OBJECT : register(b0) {
    float4x4 WorldMatrix;
};

cbuffer PER_FRAME : register(b1)
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

void VS(float3 Position : POSITION, float3 Normal : NORMAL, float2 Texcoord : TEXCOORD,
    out float4 Pps : SV_POSITION,
    out float3 N : NORMAL,
    out float3 P : POSITIONT)
{
    float4 Pws = mul(WorldMatrix, float4x1(Position, 1));
    P   = Pws.xyz;
    N   = mul((float3x3)WorldMatrix, Normal);
    Pps = mul(mul(ProjMatrix, ViewMatrix), Pws);
}


/*- - - - - - - - - - - - - - - - - - - - - - - - PS - - - - - - - -  -- - - - - - - - - -  - - - - - - - - -*/

struct light_source 
{
    vector3 color;
    scalar  intensity;
    vector3 position;
    // non_physic_attribs
    scalar  umbra;
    vector3 direction;
    scalar  penumbra;
};

struct environment 
{
    scalar  Ap; // ratio of particles, absorbed light-intensity per 1[m]
	vector3 Cp; // color of particles
};

cbuffer RENDERING : register(b2) {
    // ...
    environment  Env;
    // lights
    light_source Lights[1];
    // surface
    vector3 Pss; // subsurface_albedo or diffuse_color
    vector3 F0; // reflectance_value or Fresnel_value
    scalar  r; // roughness
};

scalar fdir(light_source Light, vector3 l) {
    scalar thetaS = dot(Light.position, -l);
    return saturate((Light.penumbra - thetaS) / (Light.umbra - Light.penumbra));
}

float4 PS(float4 Pps : SV_POSITION, float3 N : NORMAL, float3 P : POSITIONT) : SV_TARGET {
    vector3 unlit = vector3(0.1);
    
    //vector3 lit = vector3(0, 0, 0);
    //for (int i = 0; i != 1; ++i) {
    //    vector3 l = Lights[i].position - P;
    //    vector3 Clight = Lights[i].color * fdir(Lights[i], l);
    //            Clight *= Lights[i].intensity - Env.Ap * 1.0;
        
    //    scalar  n_dot_l = dot(N, l);
    //    vector3 Cspec   = F0 + (1 - F0) * pow(n_dot_l, 5);// Schlick-Fresnel
    //    vector3 Cdiff   = (1 - Cspec) * (Pss / 3.141592);// Lambert-surface

    //    lit += Cdiff * Clight * saturate(dot(N, l));
    //}
    //lit /= 3.14159;
    
    //if (Env.Ap < 0.1)
    //{
        return float4(1, 0, 0, 1);
    //}
        //return float4(unlit + lit, 1.0f);
}