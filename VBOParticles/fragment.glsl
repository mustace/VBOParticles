uniform samplerCube cubemap;
uniform float reflecRate;

varying vec4 vColor;
varying vec3 vNormal;
//varying vec4 vPos;
varying vec3 vDir;


void main()
{

	//vec3 eyeToFrag = vDir;

	vec3 reflected = reflect(normalize(vDir), vNormal);

	// vPos.z = abs(vPos.z);

	

	// gl_FragColor = vec4(reflected, 1.0);
	gl_FragColor = mix(vec4(reflected, 1.0), texture(cubemap, reflected), reflecRate);
}