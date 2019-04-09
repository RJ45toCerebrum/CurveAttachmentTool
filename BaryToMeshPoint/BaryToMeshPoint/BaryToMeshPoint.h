#pragma once
#include"stdafx.h"
#include <maya/MPxNode.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MPoint.h>
#include <maya/MFnMesh.h>
#include <maya/MMatrix.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MNodeMessage.h>
#include <vector>


struct VertIDs
{
	int id0 = -1;
	int id1 = -1;
	int id2 = -1;

	VertIDs& operator=(const VertIDs& vIDs)
	{
		id0 = vIDs.id0;
		id1 = vIDs.id1;
		id2 = vIDs.id2;
		return *this;
	}

};

struct BaryCoords
{
	float a = -1.0;
	float b = -1.0;
	float c = -1.0;

	BaryCoords& operator=(const BaryCoords& bc) {
		a = bc.a;
		b = bc.b;
		c = bc.c;
		return *this;
	}
};

/// Node computes takes barycentric coordinates as input
/// and outputs the worldspace point on the mesh
class BaryToMeshPoint : public MPxNode
{
public:
	BaryToMeshPoint();
	virtual ~BaryToMeshPoint();
	virtual void postConstructor();
	static void* creator();
	static MStatus initialize();

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	bool jumpToArrayElement(MArrayDataHandle& hArr, unsigned int index);

private:
	MStatus initNode(MDataBlock& data);
	void clearCache();
	static void attrChangedCallback(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData);
	
public:
	// INPUT ATTRIBUTES
	static MTypeId id;
	static MObject inMesh;
	static MObject inMeshWorldMatrix;
	static MObject inParentInverse;
	static MObject inInitialize;
	static MObject input;
	static MObject polygonID;
	static MObject triangleID;
	static MObject barycentricCoordinates;
	static MObject parentInverseID;
	// OUTPUT ATTRIBUTES
	static MObject outPoint;

private:
	bool startInitialization = false;
	bool initialized = false;
	std::vector<unsigned int> polygonIDs;
	std::vector<VertIDs> vertIDs;
	std::vector<BaryCoords> baryCoords;
	std::vector<int> parentInverseIDs;

	MCallbackId callbackID;
};

