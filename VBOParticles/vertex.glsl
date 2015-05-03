attribute vec4 inColor;

varying vec4 vColor;

void main()
{
	vColor = inColor;

	gl_Position = ftransform();
	//gl_Position = gl_ModelViewMatrix * gl_ProjectionMatrix * gl_Vertex;
}