#include "stdafx.h"
#include "QuaternionProduct.h"


MTypeId QuaternionProduct::id(0x0000BB4F);
MObject QuaternionProduct::qL;
MObject QuaternionProduct::qLX, QuaternionProduct::qLY, QuaternionProduct::qLZ, QuaternionProduct::qLW;
MObject QuaternionProduct::qR;
MObject QuaternionProduct::qRX, QuaternionProduct::qRY, QuaternionProduct::qRZ, QuaternionProduct::qRW;
MObject QuaternionProduct::qrInverse;
MObject QuaternionProduct::parentInverse;
// OUTPUT
MObject QuaternionProduct::outEulerRotation;
MObject QuaternionProduct::eulerX, QuaternionProduct::eulerY, QuaternionProduct::eulerZ;
MObject QuaternionProduct::outQuaternion;
MObject QuaternionProduct::qx, QuaternionProduct::qy, QuaternionProduct::qz, QuaternionProduct::qw;

const MString kLeftQuaternion("inputQuaternion1");
const MString kLeftQuaternionShort("iq1");
const MString kRightQuaternion("inputQuaternion2");
const MString kRightQuaternionShort("iq2");
const MString kInverseQuat2("inverseQuat2");
const MString kParentInverse("parentInverse");
const MString kEulerRotation("eulerRotation");
const MString kEulerX("eulerX");  const MString kEulerY("eulerY");  const MString kEulerZ("eulerZ");
const MString kQuaternion("quaternion");


QuaternionProduct::QuaternionProduct() {}
QuaternionProduct::~QuaternionProduct(){}
void* QuaternionProduct::creator() {
	return new QuaternionProduct;
}


MStatus QuaternionProduct::initialize()
{
	MStatus stat;
	MFnNumericAttribute nAttr;
	MFnMatrixAttribute mAttr;
	MFnCompoundAttribute cAttr;
	MFnUnitAttribute uAttr;
	
	// left quaternion
	{
		qL = cAttr.create(kLeftQuaternion, kLeftQuaternionShort);
		cAttr.setWritable(true);
		cAttr.setReadable(false);

		qLX = nAttr.create("qLx", "qLx", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qLX));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qLX));

		qLY = nAttr.create("qLy", "qLy", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qLY));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qLY));

		qLZ = nAttr.create("qLz", "qLz", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qLZ));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qLZ));

		qLW = nAttr.create("qLw", "qLw", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qLW));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qLW));

		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qL));
	}

	// right quaternion
	{
		qR = cAttr.create(kRightQuaternion, kRightQuaternionShort);
		cAttr.setWritable(true);
		cAttr.setReadable(false);

		qRX = nAttr.create("qRx", "qRx", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qRX));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qRX));

		qRY = nAttr.create("qRy", "qRy", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qRY));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qRY));

		qRZ = nAttr.create("qRz", "qRz", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qRZ));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qRZ));

		qRW = nAttr.create("qRw", "qRw", MFnNumericData::kDouble);
		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qRW));
		CHECK_MSTATUS_AND_RETURN_IT(cAttr.addChild(qRW));

		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(qR));
	}

	parentInverse = mAttr.create(kParentInverse, kParentInverse, MFnMatrixAttribute::kDouble);
	mAttr.setWritable(true);
	mAttr.setReadable(false);
	addAttribute(parentInverse);

	qrInverse = nAttr.create(kInverseQuat2, kInverseQuat2, MFnNumericData::kBoolean, 0);
	nAttr.setWritable(true);
	nAttr.setReadable(false);
	addAttribute(qrInverse);

	// output quaternion
	{
		outQuaternion = cAttr.create(kQuaternion, kQuaternion);
		cAttr.setReadable(true);
		cAttr.setWritable(false);

		qx = nAttr.create("qx", "qx", MFnNumericData::kDouble);
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

		CHECK_MSTATUS_AND_RETURN_IT(addAttribute(outQuaternion));
	}

	// output euler angles
	{
		outEulerRotation = cAttr.create(kEulerRotation, kEulerRotation);
		cAttr.setWritable(false);
		cAttr.setReadable(true);

		MAngle angle(0);
		eulerX = uAttr.create(kEulerX, kEulerX, angle);
		addAttribute(eulerX);
		cAttr.addChild(eulerX);

		eulerY = uAttr.create(kEulerY, kEulerY, angle);
		addAttribute(eulerY);
		cAttr.addChild(eulerY);

		eulerZ = uAttr.create(kEulerZ, kEulerZ, angle);
		addAttribute(eulerZ);
		cAttr.addChild(eulerZ);

		addAttribute(outEulerRotation);
	}

	// relations...
	MObject inputQuaternion[] = { qLX, qLY, qLZ, qLW, qRX, qRY, qRZ, qRW };
	MObject attr;
	for (unsigned int i = 0; i < 8; i++) 
	{
		attr = inputQuaternion[i];
		CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(attr, eulerX));
		CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(attr, eulerY));
		CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(attr, eulerZ));
		CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(attr, qx));
		CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(attr, qy));
		CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(attr, qz));
		CHECK_MSTATUS_AND_RETURN_IT(attributeAffects(attr, qw));
	}

	attributeAffects(qrInverse, eulerX);  attributeAffects(qrInverse, eulerY);  attributeAffects(qrInverse, eulerZ);
	attributeAffects(qrInverse, qx);  attributeAffects(qrInverse, qy);  attributeAffects(qrInverse, qz);

	attributeAffects(parentInverse, eulerX);  attributeAffects(parentInverse, eulerY);  attributeAffects(parentInverse, eulerZ);
	attributeAffects(parentInverse, qx);  attributeAffects(parentInverse, qy);  attributeAffects(parentInverse, qz);

	return MS::kSuccess;
}

MStatus QuaternionProduct::compute(const MPlug& plug, MDataBlock& data)
{
	if (plug != qx && plug != eulerX && plug != outQuaternion && plug != outEulerRotation)
		return MS::kUnknownParameter;

	MDataHandle hParentInverse = data.inputValue(parentInverse);
	MMatrix M(hParentInverse.asMatrix());
	MDataHandle hQrInverse = data.inputValue(qrInverse);
	bool inverse = hQrInverse.asBool();

	MDataHandle hLeftQuaternion = data.inputValue(qL);
	MQuaternion leftQuaternion(getQuaternion(hLeftQuaternion, true));
	MDataHandle hRightQuaternion = data.inputValue(qR);
	MQuaternion rightQuaternion(getQuaternion(hRightQuaternion, false));

	if (inverse)
		rightQuaternion.invertIt();

	MQuaternion q = leftQuaternion * rightQuaternion;

	// get it into proper space
	MTransformationMatrix T(M);
	MQuaternion qp = T.rotation();
	MQuaternion qf = q * qp;
	MEulerRotation ef = qf.asEulerRotation();

	// set the quaternion
	{
		MDataHandle hOutQuaternion = data.outputValue(outQuaternion);
		MDataHandle hQuaternionChild = hOutQuaternion.child(qx);
		hQuaternionChild.setDouble(qf.x);
		hQuaternionChild.setClean();
		
		hQuaternionChild = hOutQuaternion.child(qy);
		hQuaternionChild.setDouble(qf.y);
		hQuaternionChild.setClean();

		hQuaternionChild = hOutQuaternion.child(qz);
		hQuaternionChild.setDouble(qf.z);
		hQuaternionChild.setClean();

		hQuaternionChild = hOutQuaternion.child(qw);
		hQuaternionChild.setDouble(qf.w);
		hQuaternionChild.setClean();

		hOutQuaternion.setClean();
	}

	// set out euler angles
	{
		MDataHandle hOutEuler = data.outputValue(outEulerRotation);
		MDataHandle hEulerChild = hOutEuler.child(eulerX);
		MAngle angle(ef.x);
		hEulerChild.setMAngle(angle);
		hEulerChild.setClean();

		hEulerChild = hOutEuler.child(eulerY);
		angle = ef.y;
		hEulerChild.setMAngle(angle);
		hEulerChild.setClean();

		hEulerChild = hOutEuler.child(eulerZ);
		angle = ef.z;
		hEulerChild.setMAngle(angle);
		hEulerChild.setClean();

		hOutEuler.setClean();
	}

	data.setClean(outEulerRotation);
	data.setClean(outQuaternion);
	return MS::kSuccess;
}

MQuaternion QuaternionProduct::getQuaternion(MDataHandle& hQuaternion, bool left)
{
	MDataHandle hQuaternionChild;
	double x, y, z, w;
	if (left) 
	{
		hQuaternionChild = hQuaternion.child(qLX);
		x = hQuaternionChild.asDouble();

		hQuaternionChild = hQuaternion.child(qLY);
		y = hQuaternionChild.asDouble();

		hQuaternionChild = hQuaternion.child(qLZ);
		z = hQuaternionChild.asDouble();

		hQuaternionChild = hQuaternion.child(qLW);
		w = hQuaternionChild.asDouble();
	}
	else
	{
		hQuaternionChild = hQuaternion.child(qRX);
		x = hQuaternionChild.asDouble();

		hQuaternionChild = hQuaternion.child(qRY);
		y = hQuaternionChild.asDouble();

		hQuaternionChild = hQuaternion.child(qRZ);
		z = hQuaternionChild.asDouble();

		hQuaternionChild = hQuaternion.child(qRW);
		w = hQuaternionChild.asDouble();
	}
	
	return MQuaternion(x, y, z, w);
}
