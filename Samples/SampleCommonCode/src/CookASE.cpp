#include "CookASE.h"

#include <NxPhysics.h>
#include <stdio.h>

#include "Stream.h"
#include "cooking.h"
#include "MediaPath.h"

NxVec3 _offset;

static char* trimFront(char* c) {
	while(*c == ' ' || *c == '\t')
		c++;
	return c;
}

void readVertices(FILE* f, NxArray<NxVec3>& vertices, NxVec3 scale) {
	char line[512];

	while(true) {
		fgets(line, 512, f);
		char* l = trimFront(line);
		if (*l == '}') {
			return;
		} else if (strstr(l, "*MESH_VERTEX")) {
			float a,b,c;
			int i;
			sscanf(l, "*MESH_VERTEX %d %f %f %f", &i, &a, &b, &c);
			NxVec3 newV(a,c,-b);
			newV.x *= scale.x;
			newV.y *= scale.y;
			newV.z *= scale.z;
			vertices[i] = newV;
			vertices[i] += _offset;
		}
	}
}

void readFaces(FILE* f, NxArray<NxU32>& faces) {
	char line[512];

	while(true) {
		fgets(line, 512, f);
		char* l = trimFront(line);
		if (*l == '}') {
			return;
		} else if (strstr(l, "*MESH_FACE")) {
			int a,b,c;
			int i;
			sscanf(l, "*MESH_FACE %d: A: %d B: %d C: %d ", &i, &a, &b, &c);
			faces[3*i+0] = a;
			faces[3*i+1] = b;
			faces[3*i+2] = c;
		}
	}
}

void readMesh(FILE* f, NxArray<NxVec3>& vertices, NxArray<NxU32>& faces, NxVec3 scale) {
	char line[512];
	int nbVertices;
	int nbFaces;
	while(true) {
		fgets(line, 512, f);
		char* l = trimFront(line);
		if (*l == '}') {
			return;
		} else if (strstr(l, "*MESH_NUMVERTEX")) {
			sscanf(l, "*MESH_NUMVERTEX %d", &nbVertices);
			vertices.resize(nbVertices);
		} else if (strstr(l, "*MESH_NUMFACES")) {
			sscanf(l, "*MESH_NUMFACES %d", &nbFaces);
			faces.resize(nbFaces*3);
		} else if (strstr(l, "*MESH_VERTEX_LIST {")) {
			readVertices(f, vertices, scale);
		} else if (strstr(l, "*MESH_FACE_LIST {")) {
			readFaces(f, faces);
		}
	}
}

NxActor* CookASE(const std::string& filename, NxScene* scene, NxVec3 offset, NxVec3 scale) {
	_offset = offset;
	char buff[1024];
	//FILE *f = 0;
	FILE* f = fopen(FindMediaFile(filename.c_str(),buff), "rb");
	if (f == NULL) {
		printf("File not found: %s\n", filename.c_str());
		return NULL;
	}

	NxArray<NxVec3> vertices;
	NxArray<NxU32> faces;

	char line[512];
	int linenbr = 0;
	while (!feof(f)) {
		fgets(line, 512, f);
		char* l = trimFront(line);
		if (!strncmp(l, "*MESH {", 7)) {
			readMesh(f, vertices, faces, scale);
		} else {
			//printf("Line %4d: %s\n", linenbr++, l);
		}

	}

	fclose(f);

	// Build physical model
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices					= vertices.size();
	terrainDesc.numTriangles				= faces.size()/3;
	terrainDesc.pointStrideBytes			= sizeof(NxVec3);
	terrainDesc.triangleStrideBytes			= 3*sizeof(NxU32);
	terrainDesc.points						= &vertices[0].x;
	terrainDesc.triangles					= &faces[0];
	terrainDesc.flags						= NX_MF_HARDWARE_MESH;

	//terrainDesc.heightFieldVerticalAxis		= NX_Y;
	//terrainDesc.heightFieldVerticalExtent	= -1000.0f;
	MemoryWriteBuffer buf;
	bool status = CookTriangleMesh(terrainDesc, buf);
	NxTriangleMesh* terrainMesh	= scene->getPhysicsSDK().createTriangleMesh(MemoryReadBuffer(buf.data));
	//
	// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
	//


	NxTriangleMeshShapeDesc terrainMeshDesc;
	terrainMeshDesc.meshData = terrainMesh;


	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&terrainMeshDesc);
	
	NxActor* a = scene->createActor(ActorDesc);
	
	assert(a != NULL);
	assert(a->getNbShapes() == 1);
	NxShape* s = a->getShapes()[0];
	assert(s != NULL);
	NxTriangleMeshShape* ts = s->isTriangleMesh();
	assert(ts != NULL);
	NxTriangleMeshDesc* tmd= new NxTriangleMeshDesc();
	ts->getTriangleMesh().saveToDesc(*tmd);
	ts->userData = tmd;

	return a;
}

