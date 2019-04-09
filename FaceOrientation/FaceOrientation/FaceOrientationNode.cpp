#include "stdafx.h"
#include "FaceOrientationNode.h"

// Attributes
MTypeId FaceOrientationNode::id(0x0000AB1D);
MObject FaceOrientationNode::inInput;
MObject FaceOrientationNode::inV0, FaceOrientationNode::inV1, FaceOrientationNode::inV2;
MObject FaceOrientationNode::inWorldMesh;
MObject FaceOrientationNode::inWorldMatrix;
MObject FaceOrientationNode::outFaceOrientation;
MObject FaceOrientationNode::qx, FaceOrientationNode::qy, FaceOrientationNode::qz, FaceOrientationNode::qw;

const MString kInput("input");
const MString kV0("v0");
const MString kV1("v1");
const MString kV2("v2");
const MString kWorldMesh("worldMesh");
const MString kWorldMatrix("worldMatrix");
const MString kOutFaceOrientation("outQuaternion");


FaceOrientationNode::FaceOrientationNode() {}
FaceOrientationNode::~FaceOrientationNode() {}

void* FaceOrientationNode::creator() {
	return new FaceOrientationNode;
}

MStatus FaceOrientationNode::initialize()
{
	MStatus stat;
	MFnCompoundAttribute cAttr, qAttr;
	MFnMatrixAttribute mAttr;
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;

	{	
		inInput = cAttr.create(kInput, kInput);
		cAttr.setArray(true);
		cAttr.setReadable(false);

		inV0 = nAttr.create(kV0, kV0, MFnNumericData::kInt, -2);
		nAttr.setReadable(false);
		nAttr.setMin(-2);
		cAttr.addChild(inV0);
		addAttribute(inV0);

		inV1 = nAttr.create(kV1, kV1, MFnNumericData::kInt, -2);
		nAttr.setReadable(false);
		nAttr.setMin(-2);
		cAttr.addChild(inV1);
		addAttribute(inV1);

		inV2 = nAttr.create(kV2, kV2, MFnNumericData::kInt, -2);
		nAttr.setReadable(false);
		nAttr.setMin(-2);
		cAttr.addChild(inV2);
		addAttribute(inV2);

		addAttribute(inInput);
	}

	{
		inWorldMesh = tAttr.create(kWorldMesh, kWorldMesh, MFnData::kMesh);
		tAttr.setWritable(true);
		tAttr.setReadable(false);
		addAttribute(inWorldMesh);

		inWorldMatrix = mAttr.create(kWorldMatrix, kWorldMatrix, MFnMatrixAttribute::kDouble);
		mAttr.setWritable(true);
		mAttr.setReadable(false);
		addAttribute(inWorldMatrix);
	}

	{
		outFaceOrientation = cAttr.create(kOutFaceOrientation, kOutFaceOrientation, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		cAttr.setArray(true);
		cAttr.setUsesArrayDataBuilder(true);
		cAttr.setReadable(true);
		cAttr.setWritable(false);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(outFaceOrientation));

		qx = nAttr.create("qx", "qx", MFnNumericData::kDouble, 0.0, &stat);
		CHECK_MSTATUS_AND_RETURN_IT(stat);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qx));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qx));

		qy = nAttr.create("qy", "qy", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qy));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qy));

		qz = nAttr.create("qz", "qz", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qz));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qz));

		qw = nAttr.create("qw", "qw", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qw));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qw));
	}

	// relations
	CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(inWorldMesh, outFaceOrientation));
	CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(inWorldMatrix, outFaceOrientation));
	CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(inInput, outFaceOrientation));
	CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(inWorldMesh, qx));
	CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(inWorldMatrix, qx));
	CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(inInput, qx));
	return MS::kSuccess;
}

MStatus FaceOrientationNode::compute(const MPlug& plug, MDataBlock& data)
{
	if (plug != outFaceOrientation && plug != qx)
		return MS::kUnknownParameter;

	MDataHandle hWorldMesh = data.inputValue(inWorldMesh);
	MObject oMesh = hWorldMesh.asMesh();
	if (oMesh.isNull())
		return MS::kSuccess;

	MDataHandle hWorldMatrix = data.inputValue(inWorldMatrix);
	MMatrix W(hWorldMatrix.asMatrix());

	MArrayDataHandle hInputArr = data.inputArrayValue(inInput);
	MArrayDataHandle hOutputArr = data.outputArrayValue(outFaceOrientation);
	unsigned int N = hInputArr.elementCount();
	unsigned int NO = hOutputArr.elementCount();
	if (NO < N)
	{
		MArrayDataBuilder builder = hOutputArr.builder();
		CHECK_MSTATUS_AND_RETURN_IT(builder.growArray(N));
		hOutputArr.set(builder);
	}

	MFnMesh meshFn(oMesh);

	MDataHandle hInput, hInputChild, hOutput, hOutputChild;
	int v0, v1, v2;
	MPoint p0, p1, p2;
	MVector b0, b1, normal;
	MTransformationMatrix T;
	MMatrix M;
	MQuaternion q;
	for (unsigned int i = 0; i < N; i++)
	{
		// Gather the vertex id's for each input
		CHECK_MSTATUS_AND_RETURN_IT(hInputArr.jumpToArrayElement(i));
		hInput = hInputArr.inputValue();
		
		hInputChild = hInput.child(inV0);
		v0 = hInputChild.asInt();
		if (v0 < 0)
			continue;
		
		hInputChild = hInput.child(inV1);
		v1 = hInputChild.asInt();
		if (v1 < 0)
			continue;

		hInputChild = hInput.child(inV2);
		v2 = hInputChild.asInt();
		if (v2 < 0)
			continue;

		meshFn.getPoint(v0, p0, MSpace::kWorld);
		meshFn.getPoint(v1, p1, MSpace::kWorld);
		meshFn.getPoint(v2, p2, MSpace::kWorld);

		// The meet of the Algorithm:
		/* We need to create a Matrix with:
			column[0] being the vector from vertex0 --> vertex1 of triangle
			column[1] being the normal of the triangle
			column[2] being the cross product of those,
			column[3] being the world space translation
			Note that we need to normalize each vector, which makes its orthonormal basis;
			Also, in maya (arrrgghhhh =( ) matrix multiplication is row order, and
			therefore we need to transpose this matrix..
		*/
		b0 = p1 - p0;  b1 = p2 - p0;
		normal = b0 ^ b1;
		b0.normalize();
		normal.normalize();
		b1 = b0 ^ normal;

		this->createMatrix(b0, normal, b1, M);
		T = M;
		q = T.rotation();

		// output the quaternion
		CHECK_MSTATUS_AND_RETURN_IT(hOutputArr.jumpToArrayElement(i));
		hOutput = hOutputArr.outputValue();
		
		hOutputChild = hOutput.child(qx);
		hOutputChild.setDouble(q.x);
		hOutputChild = hOutput.child(qy);
		hOutputChild.setDouble(q.y);
		hOutputChild = hOutput.child(qz);
		hOutputChild.setDouble(q.z);
		hOutputChild = hOutput.child(qw);
		hOutputChild.setDouble(q.w);
	}

	// tell maya that the output plugs have been computed and are fully up to date
	hOutputArr.setAllClean();
	data.setClean(outFaceOrientation);
	data.setClean(qx);  data.setClean(qy);  data.setClean(qz);  data.setClean(qw);
	return MS::kSuccess;
}

void FaceOrientationNode::createMatrix(const MVector& b0, const MVector& b1, const MVector& b2, MMatrix& M)
{
	double MArr[4][4];
	MArr[0][0] = b0.x; MArr[0][1] = b0.y; MArr[0][2] = b0.z; MArr[0][3] = 0;
	MArr[1][0] = b1.x; MArr[1][1] = b1.y; MArr[1][2] = b1.z; MArr[1][3] = 0;
	MArr[2][0] = b2.x; MArr[2][1] = b2.y; MArr[2][2] = b2.z; MArr[2][3] = 0;
	MArr[3][0] = 0; MArr[3][1] = 0; MArr[3][2] = 0; MArr[3][3] = 1;
	M = MArr;
}
