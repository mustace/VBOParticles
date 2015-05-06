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
	vColor = inColor;
	
	mat4 t = translation(inTranslation.x, inTranslation.y, inTranslation.z);

	mat4 r = rotY(inRotation);

	vec4 pos = t * (r * gl_Vertex); 
	
	//gl_Position = ftransform();
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	
}