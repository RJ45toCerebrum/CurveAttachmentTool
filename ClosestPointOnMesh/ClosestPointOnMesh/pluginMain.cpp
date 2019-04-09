#include "stdafx.h"
#include "ClosestPointOnMesh.h"
#include <maya/MFnPlugin.h>

MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, "Tyler Heers", "1.0", "Any");

	status = plugin.registerCommand("closestPointOnMesh", ClosestPointOnMesh::creator, ClosestPointOnMesh::syntaxCreator);
	if (!status) {
		status.perror("failed to register command: closestPointOnMesh");
		return status;
	}

	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterCommand("closestPointOnMesh");
	if (!status) {
		status.perror("failed to de-register command: closestPointOnMesh");
		return status;
	}

	return status;
}