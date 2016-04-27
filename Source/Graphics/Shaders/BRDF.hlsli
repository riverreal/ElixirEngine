/*------------------------------------------------------
	Radix Engine 2016
	Bidirectional Reflectance Distribution Function

	**************Cook-Torrance BRDF*****************
-------------------------------------------------------*/

#define PI 3.14159265359f

//Distribution function
//GGX --->>            0
//Blinn --->>          1
#define SPECULAR_D 0

//Geometry term
//Smith/Schlick --->>  0
//Smith/GGX --->>      1
#define SPECULAR_G 0

//Fresnel
//None --->>           0
//Schlick --->>        1
#define SPECULAR_F 1

#if SPECULAR_G == 0
	//K term for schlick geometry term
	//Default Schlick --->>    0
	//Crytek model --->>       1
	//Disney --->>             2   <--- only for analytic light sources
	#define K_MODEL 0
#endif

