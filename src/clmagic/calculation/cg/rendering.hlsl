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


//------------------------------------------
// Copyright (c) <<SIGGRAPH 2010 Course: Physically-Based Shading Models in Film and Game Production>>.<<Practical Implementation of Physically-Based Shading Models at tri-Ace (Yoshiharu Gotanda)>>
// All Rights Reserved
//------------------------------------------
/* isotripic-BRDF
 VECTOR3 Pss = subsurface_albedo
 SCALAR  n   = surface_shininess;
 VECTOR3 f0  = surface_reflectance;
 VECTOR3 N   = surface_normal;
 VECTOR3 E   = eye_vector;
 VECTOR3 L   = light_vector;*/
VECTOR3 tri_Ace_BRDF(VECTOR3 Pss, SCALAR n, VECTOR3 F0, VECTOR3 N, VECTOR3 E, VECTOR3 L) {
    SCALAR  Pi = 3.14159;
    VECTOR3 H  = normalize(E + L);
    
    VECTOR3 Fdiff = F0 + (1 - F0) * pow(1 - max(0, dot(N, L)), 5);
    VECTOR3 Fspec = F0 + (1 - F0) * pow(1 - max(0, dot(E, H)), 2);
    return (Pss/Pi)*(1-Fdiff) +               (n + 2)              *  Fspec * pow(max(0,dot(N, H)),n)    
                                /*------------------------------*/  /*-------------------------------*/
                                   / (4*Pi * (2 - pow(2,-n/2)))        / max(dot(N, L), dot(N, E));
}