#include "stdafx.h"
#include "BaryToMeshPoint.h"


MTypeId BaryToMeshPoint::id(0x0001212F);
MObject BaryToMeshPoint::input;
MObject BaryToMeshPoint::polygonID;
MObject BaryToMeshPoint::triangleID;
MObject BaryToMeshPoint::barycentricCoordinates;
MObject BaryToMeshPoint::parentInverseID;
MObject BaryToMeshPoint::inMesh;
MObject BaryToMeshPoint::inMeshWorldMatrix;
MObject BaryToMeshPoint::inParentInverse;
MObject BaryToMeshPoint::inInitialize;
MObject BaryToMeshPoint::outPoint;

const MString kInMesh("inMesh");
const MString kMeshWorldMatrix("meshWorldMatrix");
const MString kParentInverse("parentInverse");
const MString kInitialize("reinitialize");
const MString kInput("input");
const MString kPolygonID("polygonID");
const MString kTriangleID("triangleID");
const MString kBarycentricCoords("barycentricCoordinates");
const MString kParentInverseID("parentInverseID");
const MString kOutPoint("outPoint");


BaryToMeshPoint::BaryToMeshPoint() {}
BaryToMeshPoint::~BaryToMeshPoint() {
	MNodeMessage::removeCallback(callbackID);
}


void BaryToMeshPoint::postConstructor()
{
	MObject oThis = thisMObject();
	callbackID = MNodeMessage::addAttributeChangedCallback(oThis, attrChangedCallback, this);
}

void* BaryToMeshPoint::creator() {
	return new BaryToMeshPoint;
}

MStatus BaryToMeshPoint::initialize()
{
	MStatus status;
	MFnNumericAttribute nAttr;
	MFnCompoundAttribute cAttr;
	MFnTypedAttribute tAttr;
	MFnMatrixAttribute mAttr;

	inMesh = tAttr.create(kInMesh, kInMesh, MFnData::kMesh);
	tAttr.setWritable(true);
	tAttr.setReadable(false);
	addAttribute(inMesh);

	inMeshWorldMatrix = mAttr.create(kMeshWorldMatrix, kMeshWorldMatrix, MFnMatrixAttribute::kDouble);
	mAttr.setWritable(true);
	mAttr.setReadable(false);
	addAttribute(inMeshWorldMatrix);

	inInitialize = nAttr.create(kInitialize, kInitialize, MFnNumericData::kBoolean, 0);
	mAttr.setWritable(true);
	mAttr.setReadable(false);
	addAttribute(inInitialize);

	inParentInverse = mAttr.create(kParentInverse, kParentInverse, MFnMatrixAttribute::kDouble);
	mAttr.setArray(true);
	mAttr.setWritable(true);
	mAttr.setReadable(false);
	addAttribute(inParentInverse);

	{
		input = cAttr.create(kInput, kInput);
		cAttr.setArray(true);
		cAttr.setUsesArrayDataBuilder(true);
		cAttr.setReadable(false);
		cAttr.setWritable(true);

		polygonID = nAttr.create(kPolygonID, kPolygonID, MFnNumericData::kInt, 0);
		nAttr.setMin(0);
		nAttr.setReadable(false);
		nAttr.setWritable(true);
		cAttr.addChild(polygonID);
		//addAttribute(polygonID);

		triangleID = nAttr.create(kTriangleID, kTriangleID, MFnNumericData::kInt, 0);
		nAttr.setMin(0);
		nAttr.setReadable(false);
		nAttr.setWritable(true);
		cAttr.addChild(triangleID);
		//addAttribute(triangleID);

		barycentricCoordinates = nAttr.createPoint(kBarycentricCoords, kBarycentricCoords);
		nAttr.setDefault(-1.0, -1.0, -1.0);
		nAttr.setReadable(false);
		nAttr.setWritable(true);
		cAttr.addChild(barycentricCoordinates);
		//addAttribute(barycentricCoordinates);

		parentInverseID = nAttr.create(kParentInverseID, kParentInverseID, MFnNumericData::kInt);
		nAttr.setDefault(-2);
		nAttr.setMin(-2);
		cAttr.addChild(parentInverseID);

		addAttribute(input);
	}

	outPoint = nAttr.createPoint(kOutPoint, kOutPoint);
	nAttr.setArray(true);
	nAttr.setUsesArrayDataBuilder(true);
	nAttr.setWritable(false);
	nAttr.setReadable(true);
	addAttribute(outPoint);


	attributeAffects(inMesh, outPoint);
	attributeAffects(inMeshWorldMatrix, outPoint);
	attributeAffects(inParentInverse, outPoint);
	attributeAffects(input, outPoint);
	attributeAffects(polygonID, outPoint);
	attributeAffects(triangleID, outPoint);
	attributeAffects(parentInverseID, outPoint);
	attributeAffects(barycentricCoordinates, outPoint);
	return MS::kSuccess;
}

MStatus BaryToMeshPoint::compute(const MPlug& plug, MDataBlock& data)
{
	if (plug != outPoint)
		return MS::kUnknownParameter;

	if (!this->initialized)
	{
		if (this->startInitialization)
		{
			if (!this->initNode(data)) {
				this->initialized = false;
				this->startInitialization = false;
				return MS::kFailure;
			}
		}
		else
			return MS::kSuccess;
	}

	MDataHandle hMesh = data.inputValue(inMesh);
	MDataHandle hMeshWorldMatrix = data.inputValue(inMeshWorldMatrix);
	MFnMesh mesh(hMesh.asMesh());

	// Get the parent inverse
	MArrayDataHandle hParentInverseArr = data.inputValue(inParentInverse);
	MDataHandle hParentInverse;
	unsigned int numParentInverses = hParentInverseArr.elementCount();
	std::vector<MMatrix> piMatrices(numParentInverses);
	for (unsigned int i = 0; i < numParentInverses; i++)
	{
		CHECK_MSTATUS_AND_RETURN_IT(hParentInverseArr.jumpToArrayElement(i));
		hParentInverse = hParentInverseArr.inputValue();
		piMatrices[i] = hParentInverse.asMatrix();
	}

	MArrayDataHandle hOutputArr = data.outputArrayValue(outPoint);

	unsigned int N = this->baryCoords.size();
	MDataHandle hOutput;
	unsigned int polyID, triID;
	MPoint p0, p1, p2, p;
	for (unsigned int i = 0; i < N; i++)
	{
		polyID = this->polygonIDs[i];
		VertIDs ids = this->vertIDs[i];
		mesh.getPoint(ids.id0, p0, MSpace::kWorld);
		mesh.getPoint(ids.id1, p1, MSpace::kWorld);
		mesh.getPoint(ids.id2, p2, MSpace::kWorld);
		BaryCoords bc = this->baryCoords[i];

		p = p0 * bc.a + p1 * bc.b + p2 * bc.c;

		if(this->parentInverseIDs[i] > -1)
			p = p * piMatrices[this->parentInverseIDs[i]];

		if (this->jumpToArrayElement(hOutputArr, i)) {
			hOutput = hOutputArr.outputValue();
			hOutput.set3Float(p.x, p.y, p.z);
		}
		else
			MGlobal::displayInfo("unable to set output...");
	}

	hOutputArr.setAllClean();
	data.setClean(outPoint);
	return MS::kSuccess;
}

bool BaryToMeshPoint::jumpToArrayElement(MArrayDataHandle& hArr, unsigned int index)
{
	MStatus status = hArr.jumpToArrayElement(index);
	if (!status)
	{
		MArrayDataBuilder dataBuilder = hArr.builder();
		MDataHandle hNewHandle = dataBuilder.addElement(index, &status);
		if (!status)
			return false;

		if (!hArr.set(dataBuilder))
			return false;
	}

	return true;
}

MStatus BaryToMeshPoint::initNode(MDataBlock& data)
{
	MDataHandle hMesh = data.inputValue(inMesh);
	MFnMesh mesh(hMesh.asMesh());

	MArrayDataHandle hInputArr = data.inputArrayValue(input);
	unsigned int N = hInputArr.elementCount();
	this->polygonIDs.reserve(N);
	this->vertIDs.reserve(N);
	this->baryCoords.reserve(N);
	this->parentInverseIDs.reserve(N);


	MDataHandle hInput, hChild, hOutput;
	unsigned int polyID, triID;
	float3 outPoint;
	MPoint p0, p1, p2, p;
	int triangleVertIDs[3];
	for (unsigned int i = 0; i < N; i++)
	{
		if (!this->jumpToArrayElement(hInputArr, i))
			return MS::kFailure;

		hInput = hInputArr.inputValue();
		hChild = hInput.child(polygonID);
		polyID = hChild.asInt();
		this->polygonIDs.push_back(polyID);
		
		hChild = hInput.child(triangleID);
		triID = hChild.asInt();
		
		hChild = hInput.child(barycentricCoordinates);
		float3& bc = hChild.asFloat3();
		BaryCoords bary;
		bary.a = bc[0]; bary.b = bc[1]; bary.c = bc[2];
		this->baryCoords.push_back(bary);

		VertIDs vIDs;
		if (mesh.getPolygonTriangleVertices(polyID, triID, triangleVertIDs)) {
			vIDs.id0 = triangleVertIDs[0];
			vIDs.id1 = triangleVertIDs[1];
			vIDs.id2 = triangleVertIDs[2];
		}
		this->vertIDs.push_back(vIDs);

		hChild = hInput.child(parentInverseID);
		this->parentInverseIDs.push_back(hChild.asInt());
	}

	this->initialized = true;
	this->startInitialization = false;
	return MS::kSuccess;
}

void BaryToMeshPoint::clearCache()
{
	this->polygonIDs.clear();
	this->vertIDs.clear();
	this->baryCoords.clear();
	this->parentInverseIDs.clear();
}

void BaryToMeshPoint::attrChangedCallback(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData)
{
	if (msg != (MNodeMessage::AttributeMessage::kAttributeSet | MNodeMessage::AttributeMessage::kIncomingDirection))
		return;

	if (plug == inInitialize)
	{
		bool val;
		plug.getValue(val);

		if (val) {
			BaryToMeshPoint* ccm = (BaryToMeshPoint*)clientData;
			ccm->initialized = false;
			ccm->startInitialization = true;
		}
	}
}
