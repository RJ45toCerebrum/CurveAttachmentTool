#pragma once
#include <maya/MPxCommand.h>
#include <maya/MArgDatabase.h>
#include <maya/MSyntax.h>
#include <maya/MDagPath.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MArgList.h>
#include <maya/MFnMesh.h>
#include <maya/MMeshIntersector.h>
#include <maya/MFloatPointArray.h>
#include <sstream>


struct BaryCoord
{
	double a = -1.0, b = -1.0, g = -1.0;

	BaryCoord& operator=(const BaryCoord& other) {
		a = other.a;
		b = other.b;
		g = other.g;
		return *this;
	}

	MPoint getPointFromCoordinates(const MPoint& p0, const MPoint& p1, const MPoint& p2) const {
		return a * p0 + b * p1 + g * p2;
	}

	void setBaryCoord(const MPoint& a, const MPoint& b, const MPoint& c, const MPoint& p) 
	{
		MVector ab, ac, pa, pb, pc;
		ab = (b - a); ac = (c - a); pa = (a - p); pb = (b - p); pc = (c - p);
		double totalArea = 0.50 * (ab ^ ac).length();

		double abp = 0.50 * (pa ^ pb).length();
		double acp = 0.50 * (pa ^ pc).length();
		double bcp = 0.50 * (pb ^ pc).length();

		this->a = bcp / totalArea;
		this->b = acp / totalArea;
		this->g = abp / totalArea;

		if (!valid()) {
			this->a = -1.0; this->b = -1.0; this->g = -1.0;
		}
	}

	bool valid()
	{
		double sum = (a + b + g);
		if ((sum - 1.0) > 0.001)
			return false;
		return true;
	}
};


class ClosestPointOnMesh : public MPxCommand
{
public:
	ClosestPointOnMesh();
	virtual ~ClosestPointOnMesh();
	static void* creator();
	static MSyntax syntaxCreator();
	bool isUndoable() const;

	virtual bool hasSyntax() const;
	virtual MStatus doIt(const MArgList &args);
	virtual MStatus redoIt();
	BaryCoord getBarycentricCoords(const int triangleIndex, const int polyID, const MFloatPoint& p);
	void appendPoint(const MFloatPoint& p);

private:
	MDagPath meshDag;
	MPointArray points;
	bool worldSpace = false;
};

