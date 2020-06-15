
#define LIGHTS_COUNT 3

#ifdef __cplusplus

#include "../../src/clmagic/math.h"
#include "../../src/clmagic/directX12/directX12.h"

namespace norm_hlsl{
    using _Ts = float;
    using _Tb = float;
    constexpr clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR; 

    struct varying{
        VECTOR3 position;
        VECTOR3 normal;
        VECTOR2 texcoord;
    };

    inline d3d12::shader_input make_input_decs() {
        d3d12::shader_input _Varyings;
		_Varyings.push_back("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		_Varyings.push_back("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		_Varyings.push_back("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
        return std::move(_Varyings);
    }

    enum{
        INDEX_FRAME        = 0,// b0
        INDEX_TRANSFORM    = 1,// b1
        INDEX_SUBSTANCE    = 2,// b2
        INDEX_DIFF_TEXTURE = 3 // t0
    };

    struct uniform_frame : public clmagic::uniform {
        using _Mybase = clmagic::uniform;

        struct _My_data_type {
            MATRIX4x4 view_matrix;
            MATRIX4x4 proj_matrix;
            VECTOR3 eye_position;
            SCALAR _pand0;
            VECTOR3 eye_direction;
            SCALAR _pand1;
            VECTOR2 resolution;
            SCALAR total_time;
            SCALAR delta_time;
            ENVIRONMENT  env;
            LIGHT_SOURCE light_sources[LIGHTS_COUNT];
        };

	    uniform_frame() = default;
	    uniform_frame(ID3D12Device& _Device) { 
            _My_data = d3d12::dynamic_buffer<_My_data_type>(_Device, 1); 
        }

	    virtual void release() override {
		    _My_data.release();
		    _Mybase::release();
	    }

	    virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
		    _Cmdlist.SetGraphicsRootConstantBufferView(INDEX_FRAME, _My_data->GetGPUVirtualAddress());
		    return _Cmdlist;
	    }

        void map(void** _Pptr){
            _My_data.map();
            *_Pptr = _My_data.begin();
        }
        
        void unmap(void** _Pptr) {
            *_Pptr = nullptr;
            _My_data.unmap();
        }

	    d3d12::dynamic_buffer<_My_data_type> _My_data;// b0
    };

    using uniform_transform = d3d12::matrix_transform<INDEX_TRANSFORM, _Ts, _Tb, _Major>;
    
    using uniform_surface = d3d12::surface_substance<INDEX_SUBSTANCE, INDEX_DIFF_TEXTURE, 0, 0, _Ts, _Tb>;

    inline d3d12::shader_signature make_shader_signature(ID3D12Device& _Device){
        std::vector<D3D12_ROOT_PARAMETER> _Parameters(4);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[INDEX_FRAME], 0/*b0*/);
		CD3DX12_ROOT_PARAMETER::InitAsConstants(_Parameters[INDEX_TRANSFORM], 16, 1/*b1*/);
		CD3DX12_ROOT_PARAMETER::InitAsConstants(_Parameters[INDEX_SUBSTANCE], 8, 2/*b2*/);
		CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(_Parameters[INDEX_DIFF_TEXTURE], 1, &CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0/*t0*/));
		
		std::vector<D3D12_STATIC_SAMPLER_DESC> _Samplers(1);
		_Samplers[0] = CD3DX12_STATIC_SAMPLER_DESC(0/*s0*/, D3D12_FILTER_ANISOTROPIC);
		
		return d3d12::shader_signature(_Device, _Parameters, _Samplers);
    }
}

#else
    
#define SCALAR  float
#define VECTOR2 float2
#define VECTOR3 float3
#define VECTOR4 float4
#define MATRIX4x4 float4x4
#include "../../src/clmagic/calculation/cg/rendering.hlsl"

/*- - - - - - - - - - - - - - - - - - - - - - - - VS - - - - - - - -  -- - - - - - - - - -  - - - - - - - - -*/
cbuffer FRAME : register(b0)
{                    // offset:
    MATRIX4x4 Mview; // 0*SCALAR
    MATRIX4x4 Mproj; // 16*SCALAR
    VECTOR3   Peye ;  // 32*SCALAR 
    SCALAR __pand0;
    VECTOR3   eye_direction;
    SCALAR __pand1;
    VECTOR2 Resolution; // 36*SCALAR 
    SCALAR  TotalTime;
    SCALAR  DeltaTime;
     // ...
    environment Env;
    // Lights
    Light_source Lights[LIGHTS_COUNT];
}

cbuffer TRANSFORM : register(b1) {
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

cbuffer SUBSTANCE : register(b2) {
    VECTOR3 subsurface_albedo;
    SCALAR roughness_x;
    VECTOR3 reflect_value;
    SCALAR roughness_y; // anisotripic
};

SCALAR fdir(Light_source Light, VECTOR3 L) {
    SCALAR thetaS = acos(dot(Light.direction, -L));
    return saturate((Light.penumbra - thetaS) / (Light.penumbra - Light.umbra + 0.01));
}


VECTOR3 compute_direction_with_length(VECTOR3 p0, VECTOR3 p1, out SCALAR _Out_length) {
    VECTOR3 direction  = p1 - p0;
           _Out_length = length(direction);
            direction /= _Out_length;
    return direction;
}

VECTOR3 compute_direction_with_length_cosine(VECTOR3 p0, VECTOR3 p1, VECTOR3 n, out SCALAR _Out_length, out SCALAR _Out_cosine) {
    VECTOR3 direction = compute_direction_with_length(p0, p1, _Out_length);
    _Out_cosine = dot(direction, n);
    return direction;
}

float4 PS(float4 Pps : SV_POSITION, float3 N : NORMAL, float3 P : POSITIONT, float2 UV : TEXCOORD) : SV_TARGET {
    VECTOR3 unlit = VECTOR3(0,0,0);
    VECTOR3 lit   = VECTOR3(0,0,0);
    
    VECTOR3 Pss2 = DiffuseMap.Sample(LinearSampler, UV).xyz;
    
    // 1. check backface
    SCALAR  o_len, o_mu;
    VECTOR3 o_vector = compute_direction_with_length_cosine(P, Peye, N, o_len/*out*/, o_mu/*out*/);
    if (o_mu < 0) {// back face
        discard;
    }
    
    for (int i = 0; i != LIGHTS_COUNT; ++i)
    {
        // 2. check backlight
        SCALAR  i_len, i_mu;
        VECTOR3 i_vector = compute_direction_with_length_cosine(P, Lights[i].position, N, i_len, i_mu);
        if (i_mu < 0) {// back light
            continue;
        }
        
        // 3. check energy ratio
        SCALAR I   = max(0, Lights[i].intensity - Env.particles_ratio * (o_len + i_len));
        SCALAR att = I / Lights[i].intensity;
        if (att < 0.01){// energy exhaust 
            continue;
        }
        
        // 4. check spot shadowing
        att *= fdir(Lights[i], i_vector);
        if (att < 0.01) {// not in spot-cone
            continue;
        }
        VECTOR3 CLight = Lights[i].color * att;
        
        
        // tri_Ace diffuse+reflectance
        //VECTOR3 BRDF = tri_Ace_BRDF(Pss2, 1 - roughness_x, reflect_value, N, o_vector, i_vector);
       
        // Lambertian-diffuse + GGX-reflectance
        VECTOR3 BRDF = Lambertian_BRDF(Pss2) * (1 - Schlick_Fresnel(reflect_value, dot(i_vector, N)))
                     + GGX_BRDF(o_vector, i_vector, N, reflect_value, roughness_x * roughness_x);
        
        lit += BRDF * CLight * i_mu;
    }
    lit *= 3.14159;
    
    return float4(unlit + lit, 1.0f);
}

#endif