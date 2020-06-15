struct environment {
    SCALAR  particles_ratio; // absorbed Light-intensity per 1[m]
    VECTOR3 particles_color;
};

struct Light_source {
    VECTOR3 color;
    SCALAR  intensity;
    VECTOR3 position;
    // non_physic_attribs
    SCALAR  umbra;
    VECTOR3 direction;
    SCALAR  penumbra;
};

struct surface {
	VECTOR3 subsurface_albedo;
    SCALAR  roughness_x;
	VECTOR3 reflect_value;
    SCALAR  roughness_y; // anisotripic
};


SCALAR is_positive(SCALAR x) {
    // x > 0 ? 1 : 0
    return (x > 0 ? 1 : 0);
}

VECTOR3 Schlick_Fresnel(VECTOR3 F0, SCALAR cos_theta) {
    return F0 + (1 - F0) * pow(1 - cos_theta, 5);
}


VECTOR3 Lambertian_BRDF(VECTOR3 Pss){
    // diffuse
    SCALAR PI = 3.14159265358979323846;
    return Pss / PI;
}

//------------------------------------------
// Copyright (c) <<SIGGRAPH 2010 Course: Physically-Based Shading Models in Film and Game Production>>.<<Practical Implementation of Physically-Based Shading Models at tri-Ace (Yoshiharu Gotanda)>>
// All Rights Reserved
//------------------------------------------
VECTOR3 tri_Ace_BRDF(VECTOR3 Pss, SCALAR s, VECTOR3 F0, VECTOR3 N, VECTOR3 E, VECTOR3 L) {
    /* isotripic-BRDF, diffuse + reflectance
    VECTOR3 Pss = subsurface_albedo;
    SCALAR  n   = surface_shininess;
    VECTOR3 f0  = surface_reflectance;
    VECTOR3 N   = surface_normal;
    VECTOR3 E   = eye_vector;
    VECTOR3 L   = light_vector;*/
    SCALAR  PI = 3.14159265358979323846;
    VECTOR3 H  = normalize(E + L);
    
    VECTOR3 Fdiff = F0 + (1 - F0) * pow(1 - max(0, dot(N, L)), 5); // Shlick_Fresnel
    VECTOR3 Fspec = F0 + (1 - F0) * pow(1 - max(0, dot(E, H)), 5); // Shlick_Fresnel
    return (Pss/PI)*(1-Fdiff) +               (s + 2)              *  Fspec * pow(max(0,dot(N, H)),s)    
                                /*------------------------------*/  /*-------------------------------*/
                                   / (4*PI * (2 - pow(2,-s/2)))        / max(dot(N, L), dot(N, E));
}

SCALAR GGX_normal_distribution(SCALAR nm, SCALAR a2) {
    /*
    SCALAR nm = dot(N, M);
    SCALAR a2 = pow(roughness * roughness, 2) */
    SCALAR PI = 3.14159265358979323846;
    SCALAR Nx = a2;
    SCALAR Dx = PI * pow(1 + nm*nm * (a2 - 1), 2);
    return Nx / Dx;
}

SCALAR GGX_Smith_height_correlated_masking_shadowing(SCALAR muO, SCALAR muI, SCALAR a2) {
    /*
    SCALAR muO = dot(N, do);
    SCALAR muI = dot(N, di);
    SCALAR a2  = pow(_Roughness * _Roughness, 2); */
    const SCALAR muO2 = muO * muO;
    const SCALAR muI2 = muI * muI;
    const SCALAR Nx   = 0.5;
    const SCALAR Dx   = muO * sqrt(a2 + muI2 - a2 * muI2) + muI * sqrt(a2 + muO2 - a2 * muO2);
    return Nx / Dx;
}

VECTOR3 GGX_BRDF(VECTOR3 v_out, VECTOR3 v_in, VECTOR3 n, VECTOR3 F0, SCALAR r) {
    // isotripic-BRDF, reflectance
    SCALAR mu_out = dot(v_out, n);
    SCALAR mu_in  = dot(v_in, n);
    //if (mu_out < 0 || mu_in < 0) {// backface or backlight
    //    return VECTOR3(0, 0, 0);
    //}
    VECTOR3 h  = normalize(v_out + v_in);
    SCALAR  a2 = pow(r, 2);
    return Schlick_Fresnel(F0, dot(v_in, h))
         * GGX_normal_distribution(dot(n, h), a2)
         * GGX_Smith_height_correlated_masking_shadowing(mu_out, mu_in, a2);
}