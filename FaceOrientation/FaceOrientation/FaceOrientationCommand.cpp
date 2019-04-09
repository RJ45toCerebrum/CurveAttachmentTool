#include "stdafx.h"
#include "FaceOrientationCommand.h"

const char* const kPolyID = "polyID";
const char* const kPolyIDShort = "pd";
const char* const kTriangleID = "triangleID";
const char* const kTriangleIDShort = "td";
const char* const kMeshTransform = "meshTransform";
const char* const kMeshTransformShort = "mt";

FaceOrientationCommand::FaceOrientationCommand() {}
FaceOrientationCommand::~FaceOrientationCommand() {}
void* FaceOrientationCommand::creator() {
	return new FaceOrientationCommand;
}

// command syntax creation
MSyntax FaceOrientationCommand::createSyntax()
{
	MSyntax newSyntax;
	newSyntax.addFlag(kPolyIDShort, kPolyID, MSyntax::MArgType::kUnsigned);
	newSyntax.makeFlagMultiUse(kPolyID);
	newSyntax.addFlag(kTriangleIDShort, kTriangleID, MSyntax::MArgType::kUnsigned);
	newSyntax.makeFlagMultiUse(kTriangleID);
	newSyntax.addFlag(kMeshTransformShort, kMeshTransform, MSyntax::MArgType::kString);
	return newSyntax;
}

//command input parsing
MStatus FaceOrientationCommand::doIt(const MArgList& args)
{
	MStatus stat;
	MArgDatabase argDB(syntax(), args);

	if (!argDB.isFlagSet(kPolyID) || !argDB.isFlagSet(kTriangleID) || !argDB.isFlagSet(kMeshTransform))
		return MS::kFailure;

	unsigned int numPolyIDs = argDB.numberOfFlagUses(kPolyID);
	unsigned int numTriIDs = argDB.numberOfFlagUses(kTriangleID);
	if (numPolyIDs != numTriIDs) {
		MGlobal::displayError("the number of triangle and polygon ids must match...");
		return MS::kFailure;
	}

	this->polyID.resize(numPolyIDs);
	this->triID.resize(numTriIDs);
	MArgList polyIDList, triIDList;
	int pID = -1, L = -1, triID = -1;
	for (unsigned int i = 0; i < numPolyIDs; i++)
	{
		CHECK_MSTATUS_AND_RETURN_IT(argDB.getFlagArgumentList(kPolyID, i, polyIDList));
		CHECK_MSTATUS_AND_RETURN_IT(argDB.getFlagArgumentList(kTriangleID, i, triIDList));
		L = polyIDList.length();
		this->polyID[i] = polyIDList.asInt(L - 1);
		this->triID[i] = triIDList.asInt(L - 1);
	}

	// get the mesh dag
	MString meshTransformStr;
	CHECK_MSTATUS_AND_RETURN_IT(argDB.getFlagArgument(kMeshTransform, 0, meshTransformStr));
	CHECK_MSTATUS_AND_RETURN_IT(MGlobal::selectByName(meshTransformStr, MGlobal::kAddToList));
	MSelectionList selection;
	MGlobal::getActiveSelectionList(selection, true);
	CHECK_MSTATUS_AND_RETURN_IT(selection.getDagPath(selection.length() - 1, this->meshDag));
	selection.remove(selection.length() - 1);
	MGlobal::setActiveSelectionList(selection);

	if (!this->meshDag.hasFn(MFn::kMesh))
		return MS::kFailure;

	return this->redoIt();
}

// command execution
MStatus FaceOrientationCommand::redoIt()
{
	MFnMesh meshFn(this->meshDag);
	unsigned int N = this->polyID.size();

	double MArr[4][4];
	int pID, tID;
	int triangleVIDs[3];
	MPoint p0, p1, p2;
	MVector v, w, n;
	MTransformationMatrix T;
	for (unsigned int i = 0; i < N; i++)
	{
		pID = this->polyID[i];	tID = this->triID[i];
		CHECK_MSTATUS_AND_RETURN_IT(meshFn.getPolygonTriangleVertices(pID, tID, triangleVIDs));
		meshFn.getPoint(triangleVIDs[0], p0, MSpace::kWorld);
		meshFn.getPoint(triangleVIDs[1], p1, MSpace::kWorld);
		meshFn.getPoint(triangleVIDs[2], p2, MSpace::kWorld);
		
		v = p1 - p0;
		w = p2 - p0;
		n = v ^ w;
		n.normalize();
		v.normalize();
		w = n ^ v;

		MArr[0][0] = v.x; MArr[0][1] = v.y; MArr[0][2] = v.z; MArr[0][3] = 0;
		MArr[1][0] = n.x; MArr[1][1] = n.y; MArr[1][2] = n.z; MArr[1][3] = 0;
		MArr[2][0] = w.x; MArr[2][1] = w.y; MArr[2][2] = w.z; MArr[2][3] = 0;
		MArr[3][0] = 0  ; MArr[3][1] = 0  ; MArr[3][2] = 0  ; MArr[3][3] = 1;

		MMatrix M(MArr);
		T = M;
		MQuaternion q = T.rotation();
		appendToResult(q.x); appendToResult(q.y); appendToResult(q.z); appendToResult(q.w);
	}

	return MS::kSuccess;
}

bool FaceOrientationCommand::isUndoable() const {
	return false;
}

bool FaceOrientationCommand::hasSyntax() const {
	return true;
}
