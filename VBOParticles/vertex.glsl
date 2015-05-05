attribute vec4 inColor;

varying vec4 vColor;

void main()
{
	vColor = inColor;
	
	float f = gl_Vertex.x;
	
	float r = sin(f);
	
	mat4 m = mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	
	);
	
	//gl_Position = ftransform();
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	
}