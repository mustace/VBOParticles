attribute vec3 inNormal;
attribute vec4 inColor;
attribute vec3 inTranslation;
attribute float inRotation;

varying vec4 vColor;

mat4 translation(float x, float y, float z) {

	return mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		  x,   y,   z, 1.0
	);

}

mat4 rotX(float r) {

	return mat4( 
		1.0,     0.0,    0.0, 0.0,
		0.0,  cos(r), sin(r), 0.0,
		0.0, -sin(r), cos(r), 0.0,
		0.0,     0.0,    0.0, 1.0
	);

}

mat4 rotY(float r) {

	return mat4( 
		cos(r), 0.0, -sin(r), 0.0,
		   0.0, 1.0,     0.0, 0.0,
		sin(r), 0.0,  cos(r), 0.0,
		   0.0, 0.0,     0.0, 1.0
	);

}


mat4 rotZ(float r) {

	return 	mat4(
		 cos(r), sin(r), 0.0, 0.0,
		-sin(r), cos(r), 0.0, 0.0,
		    0.0,    0.0, 1.0, 0.0,
		    0.0,    0.0, 0.0, 1.0
	);

}

void main()
{

	
	vec4 n = vec4(inNormal, 1.0);

	vColor = mix(inColor, n, 0.5);

	mat4 r = rotY(inRotation);
	

	//mat4 t = translation(inTranslation.x, inTranslation.y, inTranslation.z);

	
	vec4 pos = vec4(inTranslation,0.0) + (r * gl_Vertex); 
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	
}