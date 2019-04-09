#include "stdafx.h"
#include "FaceOrientationCommand.h"
#include "FaceOrientationNode.h"
#include "QuaternionProduct.h"
#include <maya/MFnPlugin.h>

MStatus initializePlugin(MObject obj)
{
	MFnPlugin plugin(obj, "Tyler Heers", "1.0", "Any");
	MStatus stat = plugin.registerCommand("polyFaceOrientation", FaceOrientationCommand::creator, FaceOrientationCommand::createSyntax);
	if (!stat) {
		MGlobal::displayError("An error occurred while trying to register polyFaceOrientation command...");
		return stat;
	}

	stat = plugin.registerNode("faceOrientation", FaceOrientationNode::id, FaceOrientationNode::creator, FaceOrientationNode::initialize, MPxNode::kDependNode);
	if (!stat) {
		MGlobal::displayError("An error occurred while trying to register faceOrientation node...");
		return stat;
	}

	stat = plugin.registerNode("quaternionProduct", QuaternionProduct::id, QuaternionProduct::creator, QuaternionProduct::initialize, MPxNode::kDependNode);
	if (!stat) {
		MGlobal::displayError("An error occurred while trying to register faceOrientation node...");
		return stat;
	}

	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);
	MStatus stat = plugin.deregisterCommand("polyFaceOrientation");
	if (!stat) {
		MGlobal::displayError("An error occurred while trying to deregister polyFaceOrientation command...");
		return stat;
	}

	stat = plugin.deregisterNode(FaceOrientationNode::id);
	if (!stat) {
		MGlobal::displayError("An error occurred while trying to deregister faceOrientation node...");
		return stat;
	}

	stat = plugin.deregisterNode(QuaternionProduct::id);
	if (!stat) {
		MGlobal::displayError("An error occurred while trying to deregister quaternionProduct node...");
		return stat;
	}

	return MS::kSuccess;
}
