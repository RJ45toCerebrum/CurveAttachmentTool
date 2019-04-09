#include "stdafx.h"
#include "ClosestPointOnMesh.h"

//std::stringstream ss;
//ss << "Number of points: " << N;
//MGlobal::displayWarning(ss.str().c_str());

const char kMeshFlag[] = "mesh";
const char kMeshFlagShort[] = "m";
const char kPointsFlag[] = "points";
const char kPointsFlagShort[] = "p";
const char kWorldSpace[] = "worldSpace";
const char kWorldSpaceShort[] = "ws";
const char kHelp[] = "help";
const char kHelpShort[] = "h";

ClosestPointOnMesh::ClosestPointOnMesh() {}
ClosestPointOnMesh::~ClosestPointOnMesh() {}
void* ClosestPointOnMesh::creator() {
	return new ClosestPointOnMesh;
}
bool ClosestPointOnMesh::isUndoable() const {
	return false;
}

bool ClosestPointOnMesh::hasSyntax() const {
	return true;
}

MSyntax ClosestPointOnMesh::syntaxCreator()
{
	MSyntax syntax;
	syntax.addFlag(kMeshFlagShort, kMeshFlag, MSyntax::kString);
	syntax.addFlag(kPointsFlagShort, kPointsFlag, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(kWorldSpaceShort, kWorldSpace, MSyntax::kBoolean);
	syntax.makeFlagMultiUse(kPointsFlag);

	syntax.addFlag(kHelpShort, kHelp, MSyntax::kBoolean);

	return syntax;
}

// Gather the inputs into the command....
MStatus ClosestPointOnMesh::doIt(const MArgList& argList)
{
	MStatus status;
	MArgDatabase argDB(syntax(), argList, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	if (argDB.isFlagSet(kHelp)) 
	{
		std::stringstream ss;
		ss << "Help: \n\t" << "mesh = string\n\t" << "worldSpace = boolean\n\t"
		   << "points = [[float, float, float], ...]\n\n";
		ss << "Return: [polygonID1, p1.x, p1.y, p1.z, polygonID2, p2.x, p2.y, p2.z,....]\n\t"
		   << "The polygonID is the index of the polygon that contains closest point and the next 3 values are the \n"
		   << "coordinates of the closest point for input point. This repeats for every input point...";

		setResult(MString(ss.str().c_str()));
		return MS::kSuccess;
	}

	// Get the mesh dag
	if (argDB.isFlagSet(kMeshFlag))
	{
		MString transformName;
		CHECK_MSTATUS_AND_RETURN_IT(argDB.getFlagArgument(kMeshFlag, 0, transformName));
		// Get the dag for the transform
		CHECK_MSTATUS_AND_RETURN_IT(MGlobal::selectByName(transformName, MGlobal::ListAdjustment::kAddToList));
		MSelectionList selection;
		MGlobal::getActiveSelectionList(selection, true);
		if (selection.length() > 0)
		{
			MDagPath dag;
			CHECK_MSTATUS_AND_RETURN_IT(selection.getDagPath(selection.length() - 1, dag));
			if (dag.isValid() && dag.hasFn(MFn::kMesh))
				this->meshDag = dag;
			else 
			{
				MGlobal::displayError("the name provided by the mesh falg must be transform of a polygonal mesh");
				return MS::kFailure;
			}
			selection.remove(selection.length() - 1);
			MGlobal::setActiveSelectionList(selection, MGlobal::kReplaceList);
		}
	}

	// Set the worldspace flag
	if (argDB.isFlagSet(kWorldSpace)) {
		CHECK_MSTATUS_AND_RETURN_IT(argDB.getFlagArgument(kWorldSpace, 0, this->worldSpace));
	}

	// Get all the points input into the command
	if (argDB.isFlagSet(kPointsFlag)) 
	{
		MArgList pointArgList;
		unsigned int N = argDB.numberOfFlagUses(kPointsFlag);

		for (unsigned int i = 0; i < N; i++)
		{
			argDB.getFlagArgumentList(kPointsFlag, i, pointArgList);

			MPoint p;
			p.x = pointArgList.asDouble(i * 3, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			p.y = pointArgList.asDouble(i * 3 + 1, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			p.z = pointArgList.asDouble(i * 3 + 2, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			this->points.append(p);
		}
	}

	return this->redoIt();
}

MStatus ClosestPointOnMesh::redoIt() 
{
	MStatus status;
	MMatrix W = this->meshDag.inclusiveMatrix();
	CHECK_MSTATUS_AND_RETURN_IT(this->meshDag.extendToShape());
	MObject oMesh = this->meshDag.node();
	MString seq = this->meshDag.fullPathName();
	MMeshIntersector intersector;

	status = intersector.create(oMesh, W);
	if (!status) 
	{
		MGlobal::displayError(status.errorString());
		return MS::kFailure;
	}

	MPointOnMesh meshPoint;
	MPoint p;
	int polyID, triangleIndex;
	BaryCoord bc;
	for (unsigned int i = 0; i < this->points.length(); i++)
	{
		// the point returned is in object space
		intersector.getClosestPoint(this->points[i], meshPoint);
		p = meshPoint.getPoint();
		if (this->worldSpace)
			p = p * W;
		
		// calc. the barycentric coordinates
		polyID = meshPoint.faceIndex();
		triangleIndex = meshPoint.triangleIndex();
		bc = this->getBarycentricCoords(triangleIndex, polyID, p);
		
		// output the results....
		appendToResult(polyID);
		appendToResult(triangleIndex);
		appendPoint(p);
		p.x = bc.a; p.y = bc.b; p.z = bc.g;
		appendPoint(p);
	}

	return MS::kSuccess;
}

BaryCoord ClosestPointOnMesh::getBarycentricCoords(const int triangleIndex, const int polyID, const MFloatPoint& p)
{
	BaryCoord bc;
	MFnMesh mesh(meshDag);

	MIntArray vertIDs;
	int triangleIndices[3];
	MPoint p0, p1, p2;
	mesh.getPolygonTriangleVertices(polyID, triangleIndex, triangleIndices);

	if (this->worldSpace)
	{
		mesh.getPoint(triangleIndices[0], p0, MSpace::kWorld);
		mesh.getPoint(triangleIndices[1], p1, MSpace::kWorld);
		mesh.getPoint(triangleIndices[2], p2, MSpace::kWorld);
		bc.setBaryCoord(p0, p1, p2, p);
	}
	else
	{
		mesh.getPoint(triangleIndices[0], p0, MSpace::kObject);
		mesh.getPoint(triangleIndices[1], p1, MSpace::kObject);
		mesh.getPoint(triangleIndices[2], p2, MSpace::kObject);
		bc.setBaryCoord(p0, p1, p2, p);
	}

	return bc;
}

inline void ClosestPointOnMesh::appendPoint(const MFloatPoint& p) {
	appendToResult(p.x); appendToResult(p.y); appendToResult(p.z);
}