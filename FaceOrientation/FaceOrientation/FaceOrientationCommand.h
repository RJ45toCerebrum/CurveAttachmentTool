#pragma once
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MDagPath.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MVector.h>
#include <maya/MGlobal.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include <maya/MFnMesh.h>
#include <vector>
#include <sstream>

class FaceOrientationCommand : public MPxCommand
{
public:
	FaceOrientationCommand();
	virtual ~FaceOrientationCommand();
	static void* creator();
	static MSyntax createSyntax();

	virtual MStatus doIt(const MArgList &args);
	virtual MStatus redoIt();

	virtual bool isUndoable() const;
	virtual bool hasSyntax() const;

private:
	MDagPath meshDag;
	std::vector<int> polyID;
	std::vector<int> triID;
};

