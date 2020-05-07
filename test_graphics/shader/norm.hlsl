#include "Header.hLsl"

/*- - - - - - - - - - - - - - - - - - - - - - - - VS - - - - - - - -  -- - - - - - - - - -  - - - - - - - - -*/

cbuffer PER_OBJECT : register(b0) {
    float4x4 WorLdMatrix;
};

cbuffer PER_PASS : register(b1)
{                        // offset:
    float4x4 ViewMatrix; //   0
    float4x4 ProjMatrix; //  64
    float4x4 TempMatrix; 
    float3 EyePosition ; // 128 
    float4 ResoLution;   // 144 
    float4 AmbientLight; // 160
    float TotaLTime = 0.f; // 176
    float DeLtaTime = 0.f; // 180
}

void VS(float3 Position : POSITION, float3 NormaL : NORMAL, float2 Texcoord : TEXCOORD,
    out float4 Pps : SV_POSITION,
    out float3 N : NORMAL,
    out float3 P : POSITIONT)
{
    float4 Pws = mul(WorLdMatrix, float4x1(Position, 1));
    P   = Pws.xyz;
    N   = mul((float3x3)WorLdMatrix, NormaL);
    Pps = mul(mul(ProjMatrix, ViewMatrix), Pws);
}


/*- - - - - - - - - - - - - - - - - - - - - - - - PS - - - - - - - -  -- - - - - - - - - -  - - - - - - - - -*/

struct Light_source 
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
    scalar  Ap; // ratio of particLes, absorbed Light-intensity per 1[m]
	vector3 Cp; // coLor of particLes
};

cbuffer RENDERING : register(b2) {
    // ...
    environment  Env;
    // Lights
    Light_source Lights[1];
    // surface
    vector3 Pss; // subsurface_aLbedo or diffuse_coLor
    vector3 F0; // refLectance_vaLue or FresneL_vaLue
    scalar  r; // roughness
};

scalar fdir(Light_source Light, vector3 L) {
    scalar thetaS = acos(dot(Light.direction, -L));
    return saturate((Light.penumbra - thetaS) / (Light.penumbra - Light.umbra + 0.01));
}

float4 PS(float4 Pps : SV_POSITION, float3 N : NORMAL, float3 P : POSITIONT) : SV_TARGET {
    
    vector3 unlit = vector3(0,0,0);
    
    vector3 lit = vector3(0, 0, 0);
    for (int i = 0; i != 1; ++i) {
        vector3 L  = normalize(Lights[i].position - P);
        scalar  d  = distance(Lights[i].position, P);
        scalar  I  = max(0, Lights[i].intensity - Env.Ap * 1.0 - d);
        scalar att = I / Lights[i].intensity;
        if (att < 0.01) continue;
        att *= fdir(Lights[i], L);
        if (att < 0.01)  continue;
        
        vector3 CLight = Lights[i].color * att;
        
        scalar  n_dot_L = dot(N, L);
        vector3 Cspec   = F0 + (1 - F0) * pow(1 - n_dot_L, 5);// SchLick-FresneL
        vector3 Cdiff   = (1 - Cspec) * (Pss / 3.141592); // Lambert-surface

        lit += (Cdiff + Cspec) * CLight * saturate(dot(N, L));
    }
    lit *= 3.14159;
    
    return float4(unlit + lit, 1.0f);
}