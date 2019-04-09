#pragma once
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MAngle.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>


class QuaternionProduct : public MPxNode
{
public:
	QuaternionProduct();
	virtual ~QuaternionProduct();
	static void* creator();
	static MStatus initialize();

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);

private:
	MQuaternion getQuaternion(MDataHandle& hLeftQuaternion, bool left);

public:
	static MTypeId id;
	static MObject qL;
	static MObject qLX, qLY, qLZ, qLW;
	static MObject qR;
	static MObject qRX, qRY, qRZ, qRW;
	static MObject qrInverse;
	static MObject parentInverse;
	// OUT ATTRIBUTES...
	static MObject outEulerRotation;
	static MObject eulerX, eulerY, eulerZ;
	static MObject outQuaternion;
	static MObject qx, qy, qz, qw;
};

