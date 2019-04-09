#pragma once
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MQuaternion.h>

/// Node that is used to get a triangles orientation in space
/// Input: Mesh, Mesh World Matrix, Triangle vertex id's
/// Output: Quaternion describing the world space face orientation.
class FaceOrientationNode : public MPxNode
{
public:
	FaceOrientationNode();
	virtual ~FaceOrientationNode();
	static void* creator();
	static MStatus initialize();

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	void createMatrix(const MVector& b0, const MVector& b1, const MVector& b2, MMatrix& M);

public:
	static MTypeId id;
	static MObject inInput;
	static MObject inV0, inV1, inV2;
	static MObject inWorldMesh;
	static MObject inWorldMatrix;
	static MObject outFaceOrientation;
	static MObject qx, qy, qz, qw;
};

