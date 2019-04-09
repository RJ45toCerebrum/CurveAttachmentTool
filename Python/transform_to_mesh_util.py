'''
Required plugins:
    faceOrientation.mll, baryToMeshPoint, closestPointOnMesh
'''


RAD_TO_DEG = 57.29582


class ClosestPointData(object):
    def __init__(self):
        self.closestPolyID = -1
        self.closestTriangleID = -1
        self.closestPoint = []
        self.barycentricCoords = []

'''
This function uses the command closestPointOnMesh from the plugin
in order to get all the closest point data from the list of points
to the mesh such as: closest point in world space, barycentric coordinate,
triangle id, and polygon id...

    param pts: list of pymel Vector's
    param M: the transform of the mesh
'''
def getClosestPointData(pts, M):    
    points = []
    for p in pts:
        points.append([p.x, p.y, p.z])
    
    result = pm.closestPointOnMesh(mesh=M.name(), points=points, ws=True)
    
    L = len(result)
    if L % 8 != 0:
        return  
    
    cpData = [] 
    for i in range(0, L, 8):
        cp = ClosestPointData()
        cp.closestPolyID = result[i]
        cp.closestTriangleID = result[i+1]
        cp.closestPoint.extend([result[i+2],result[i+3],result[i+4]])
        cp.barycentricCoords.extend([result[i+5],result[i+6],result[i+7]])
        cpData.append(cp)
    
    return cpData


def getTransformPositions(transforms, space='world'):
    pts = []
    for t in transforms:
        pts.append(t.getTranslation(space=space))
    return pts


'''
This is the main function to be called:
    param M: the transform of the mesh object to attach the transforms to
    param transforms: the list of transforms to attach to the mesh
'''
def attachTransformsToMesh(M, transforms):
    mesh = M.getShape()
    # 1) get the closest point data
    pts = getTransformPositions(transforms)
    cpData = getClosestPointData(pts, M)
    
    # 2) create the face orientation node and the baryToMesh point node
    #    and set their inMesh and worldMatrix attrs
    foNode = pm.createNode('faceOrientation')
    pm.connectAttr(mesh.worldMesh[0], foNode.worldMesh)
    pm.connectAttr(M.worldMatrix[0], foNode.worldMatrix)
    btmNode = pm.createNode('baryToMeshPoint')
    pm.connectAttr(M.worldMesh[0], btmNode.inMesh)
    pm.connectAttr(M.worldMatrix[0], btmNode.meshWorldMatrix)
    
    # 3) initialize the values for the baryToMeshPoint and faceOrientation nodes
    PS = {}
    np = 0
    for i in range(len(cpData)):
        cp = cpData[i]
        
        # find the relative orientation of transform to the polygon face
        qfArr = pm.polyFaceOrientation(mt=M.name(), pd=cp.closestPolyID, td=cp.closestTriangleID)
        qf  = dt.Quaternion(qfArr[0], qfArr[1], qfArr[2], qfArr[3])
        qr = transforms[i].getRotation(space='world', quaternion=True) * qf.inverse()
        # set the faceOrientation polygon and triangle IDs...
        foNode.input[i].polyID.set(cp.closestPolyID)
        foNode.input[i].triID.set(cp.closestTriangleID)
        
        # finalRotation = qr * qf    needs re-evaluation after face triangle changes
        finalRotationNode = pm.createNode('quaternionProduct')
        finalRotationNode.inputQuaternion1.set(qr.x, qr.y, qr.z, qr.w)
        pm.connectAttr(foNode.outQuaternion[i].qx, finalRotationNode.inputQuaternion2.qRx)
        pm.connectAttr(foNode.outQuaternion[i].qy, finalRotationNode.inputQuaternion2.qRy)
        pm.connectAttr(foNode.outQuaternion[i].qz, finalRotationNode.inputQuaternion2.qRz)
        pm.connectAttr(foNode.outQuaternion[i].qw, finalRotationNode.inputQuaternion2.qRw)
        
        # make sure the out euler rotation in in proper space...
        pm.connectAttr(transforms[i].parentInverseMatrix[0], finalRotationNode.parentInverse)
        
        # now connect the out euler rotation
        eulerAttr = finalRotationNode.eulerRotation
        pm.connectAttr(eulerAttr.eulerX, transforms[i].rotate.rotateX)
        pm.connectAttr(eulerAttr.eulerY, transforms[i].rotate.rotateY)
        pm.connectAttr(eulerAttr.eulerZ, transforms[i].rotate.rotateZ)
        
        # now init baryToMesh node plugs
        parents = transforms[i].getAllParents()
        if len(parents) > 0:
            if not parents[0] in PS:
                PS[parents[0]] = np
                np += 1
        
        input = btmNode.input[i]
        input.polygonID.set(cp.closestPolyID)
        input.triangleID.set(cp.closestTriangleID)
        a, b, c = cp.barycentricCoords
        input.barycentricCoordinates.set(a, b, c)
        if len(parents) > 0:
            input.parentInverseID.set( PS[parents[0]] )
        else:
            input.parentInverseID.set( -1 ) 
        
        outcp = btmNode.outPoint[i]
        pm.connectAttr(outcp, transforms[i].translate)

    
    # 4) finish initializing the btm parent inverse array
    piPlug = btmNode.parentInverse
    for t in PS:
        pm.connectAttr(t.inverseMatrix, piPlug[PS[t]])
    
    # 5) dont forget to initialize the bary to point node
    btmNode.reinitialize.set(True)
    return btmNode, foNode
# ------------------------------------------------------

# this is justa debug util for printing quaternion
# in euler angles instead...
def printQuaternion(q):
    euler = dt.EulerRotation()
    euler.assign(q)
    print euler.x * RAD_TO_DEG, euler.y * RAD_TO_DEG, euler.z * RAD_TO_DEG 








