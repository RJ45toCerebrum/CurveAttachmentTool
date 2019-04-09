#include "stdafx.h"
#include "BaryToMeshPoint.h"
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>


MStatus initializePlugin(MObject obj)
{
	MFnPlugin pluginFn(obj, "Tyler Heers", "1.0");
	MStatus status = pluginFn.registerNode("baryToMeshPoint", BaryToMeshPoint::id, BaryToMeshPoint::creator, BaryToMeshPoint::initialize, MPxNode::kDependNode);
	if (!status)
	{
		MGlobal::displayInfo("Error registering curveCVsToMesh node...");
		return MS::kFailure;
	}

	return MS::kSuccess;
}


MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin pluginFn(obj);
	MStatus status = pluginFn.deregisterNode(BaryToMeshPoint::id);
	if (!status)
	{
		MGlobal::displayInfo("Error de-registering curveCVsToMesh node...");
		return MS::kFailure;
	}

	return MS::kSuccess;
}