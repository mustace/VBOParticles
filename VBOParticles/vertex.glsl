attribute vec4 inColor;
attribute vec3 inTranslation;
attribute float inRotZ;

varying vec4 vColor;

void main()
{
	vColor = inColor;
	
	float f = gl_Vertex.x;
	
	float r = sin(f);
	
	float x = inTranslation.x;
	float y = inTranslation.y;
	float z = inTranslation.z;

	float s = sin(inRotZ);

	mat4 t = mat4(
		  s, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		  x,   y,   z, 1.0
	);

	vec4 pos = t * gl_Vertex; 
	
	//gl_Position = ftransform();
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	
}