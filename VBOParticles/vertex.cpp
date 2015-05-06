#include "vertex.h"

vertex vertex_normal(vertex* vs)
{
	// edges
	vertex e1, e2;
	// normal
	vertex n;

	// e1 is edge from v1 to v2.
	e1.x = vs[1].x - vs[0].x;
	e1.y = vs[1].y - vs[0].y;
	e1.z = vs[1].z - vs[0].z;

	// e2 is edge from v1 to v3.
	e2.x = vs[2].x - vs[0].x;
	e2.y = vs[2].y - vs[0].y;
	e2.z = vs[2].z - vs[0].z;

	// normal is e1 x e2.  (Note: Does not need to be unit length for glNormal.)
	n.x = (e1.y * e2.z) - (e1.z * e2.y);
	n.y = (e1.z * e2.x) - (e1.x * e2.z);
	n.z = (e1.x * e2.y) - (e1.y * e2.x);

	return n;
}

vertex vertex_add(vertex a, vertex b)
{
	vertex c;

	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;

	return c;
}