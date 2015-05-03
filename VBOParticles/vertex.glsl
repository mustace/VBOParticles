void main()
{
	gl_Position = ftransform();
	//gl_Position = gl_ModelViewMatrix * gl_ProjectionMatrix * gl_Vertex;
}