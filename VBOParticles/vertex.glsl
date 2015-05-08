attribute vec3 inNormal;
attribute vec4 inColor;
attribute vec3 inTranslation;
attribute float inRotation;
attribute vec4 inVertex;
uniform vec3 camPos;
uniform mat4 inScale;

varying vec4 vColor;
varying vec3 vNormal;
//varying vec4 vPos;
varying vec3 vDir;


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
	
	vec4 n = normalize(vec4(inNormal, 0.0));
	
	// Pass color to fragment Shader
	vColor = inColor;

	// Rotation Matrix
	mat4 r = rotY(inRotation);
	
	// Translation Matrix
	mat4 t = translation(inTranslation.x, inTranslation.y, inTranslation.z);

	// Rotation -> Translation -> Model -> View matrix, all at once
	mat4 mvtr = gl_ModelViewMatrix * t * r;

	vNormal = vec3(normalize(t * r * n));

	vDir = vec3((t * r * inVertex)) - camPos;

	gl_Position = gl_ProjectionMatrix * mvtr * inVertex;
	
}