#include <SPIRID.h>

//main function to find a reference node of two faces
SPIRID::sGrid::pointPairRefNode
SPIRID::sGrid::findHighestRefNode(size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
{
	//ensure level1 <= level2
	if (level1 > level2)
	{
		pointPairRefNode N(findHighestRefNode(level2, P2, level1, P1));
		std::swap(N.level1,N.level2);
		std::swap(N.nodeCode1,N.nodeCode2);
		std::swap(N.edgeCode1,N.edgeCode2);
		return N;
	}

	//bit2: north/south, bit1: front/back, bit0: east/west
	unsigned short level0 =     (P1.gridCode[0]==P2.gridCode[0])
	                            + 2*(P1.gridCode[1]==P2.gridCode[1])
	                            + 4*(P1.gridCode[2]==P2.gridCode[2]);

	switch (level0)
	{
	// case 0 --> no common node
	case 0:
	{
		return {0,0,0, 0,0,0, 0,0};
		break;
	}
	// case 7 --> same face
	case 7:
	{
		return sameFaceStepupTo(1,level1,P1,level2,P2);
		break;
	}
	// cases 1,2,4 --> single reference node at level0 already, 1: N1, 2: N2, 4: N3
	case 1:
	{
		return {lastLevelAtNode(1,1,level1,P1),1,2,
		        lastLevelAtNode(1,1,level2,P2),1,3,
		        pi_2,1};
		break;
	}
	case 2:
	{
		return {lastLevelAtNode(1,2,level1,P1),2,3,
		        lastLevelAtNode(1,2,level2,P2),2,1,
		        pi_2,1};
		break;
	}
	case 4:
	{
		return {lastLevelAtNode(1,3,level1,P1),3,1,
		        lastLevelAtNode(1,3,level2,P2),3,2,
		        pi_2,1};
		break;
	}
	// case 3,5,6 --> common edge
	case 3:
	{
		return commonEdgeStepupTo(1, 3, false, level1, P1, level2, P2);
		break;
	}
	case 5:
	{
		return commonEdgeStepupTo(1, 2, false, level1, P1, level2, P2);
		break;
	}
	case 6:
	{
		return commonEdgeStepupTo(1, 1, false, level1, P1, level2, P2);
		break;
	}
	}
	return {0,0,0, 0,0,0, 0,0};
}
SPIRID::sGrid::pointPairRefNode
SPIRID::sGrid::sameFaceStepupTo(size_t level, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
{
	// assume level1 <= level2
	if (level1 < level) //P1 and P2 are the same face at level-1 --> means level1==level-1
	{
		if (level2 < level) //means level1==level-1==level2 and P1==P2
		{
			return {level1,1,2, level2,1,2, 0,-1};
		}

		unsigned short fCode2 = P2.at(level);
		if (fCode2==0) //P2 has higher levels, but no common node
		{
			return {level1,1,2, level1,1,2, 0,-1};
		}
		else
		{
			unsigned short edgeCode = nextNode(fCode2);
			return {level1,fCode2,edgeCode,
			        lastLevelAtNode(level+1,fCode2,level2,P2),fCode2,edgeCode,
			        0,-1};
		}
	}
	// level2<level not possible anymore, because level1<=level2

	//now level1 & level2 >= level
	unsigned short fCode1 = P1.at(level);
	unsigned short fCode2 = P2.at(level);
	size_t nextLevel = level+1;

	if (fCode1==fCode2)
	{
		return sameFaceStepupTo(nextLevel, level1, P1, level2, P2);
	}
	else if (fCode1 == 0)
	{
		return commonEdgeStepupTo(nextLevel, fCode2, true, level1, P1, level2, P2);
	}
	else if (fCode2 == 0)
	{
		return commonEdgeStepupTo(nextLevel, fCode1, true, level1, P1, level2, P2);
	}

	unsigned short edgeCode = newCode(fCode1,fCode2);

	return {lastLevelAtNode(nextLevel,fCode2,level1,P1),fCode2,edgeCode,
	        lastLevelAtNode(nextLevel,fCode1,level2,P2),fCode1,edgeCode,
	        pi,1}; // using that the angle between the relevant edges in P1 and P2 is pi and cos(Pi+x)==Cos(Pi-x)
}
SPIRID::sGrid::pointPairRefNode
SPIRID::sGrid::commonEdgeStepupTo(size_t level, unsigned short edgeCode, bool orientationMatch, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
{
	unsigned short f1NodeLeft  = nextNode(edgeCode);
	unsigned short f1NodeRight = newCode(edgeCode,f1NodeLeft);

	unsigned short f2NodeLeft  = f1NodeRight;
	unsigned short f2NodeRight = f1NodeLeft;

	if (!orientationMatch) //if face orientation matches then common edge node codes are exchanged
	{
		f2NodeLeft  = f1NodeLeft;
		f2NodeRight = f1NodeRight;
	}

	// assume level1 <= level2
	if (level1 < level) //P1 and P2 are the same face at level-1 --> means level1==level-1
	{
		if (level2 < level) //means level1==level-1==level2
		{
			return {level1,f1NodeLeft,edgeCode,
			        level2,f2NodeLeft,edgeCode,
			        0,1};
		}

		unsigned short fCode2 = P2.at(level);
		if (fCode2==0 || fCode2==edgeCode) //P2 has higher levels, but no common node
		{
			return {level-1,f1NodeLeft,edgeCode,
			        level-1,f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode2==f2NodeLeft)
		{
			return {level-1,f1NodeLeft,edgeCode,
			        lastLevelAtNode(level+1,fCode2,level2,P2),f2NodeLeft,edgeCode,
			        0,1};
		}
		if (fCode2==f2NodeRight)
		{
			return {level-1,f1NodeRight,edgeCode,
			        lastLevelAtNode(level+1,fCode2,level2,P2),f2NodeRight,edgeCode,
			        0,1};
		}
	}
	// level2<level not possible anymore, because level1<=level2

	unsigned short fCode1 = P1.at(level);
	unsigned short fCode2 = P2.at(level);

	if (fCode1 == edgeCode) //no common node for sub-faces --> return one of the edge nodes
	{
		if (fCode2 == edgeCode || fCode2 == 0)
		{
			return {level-1,f1NodeLeft,edgeCode,
			        level-1,f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode2 == f2NodeLeft)
		{
			return {level-1,f1NodeLeft,edgeCode,
			        lastLevelAtNode(level,f2NodeLeft,level,P2),f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode2 == f2NodeRight)
		{
			return {level-1,f1NodeRight,edgeCode,
			        lastLevelAtNode(level,f2NodeRight,level,P2),f2NodeRight,edgeCode,
			        0,1};
		}
	}

	if (fCode2 == edgeCode) //no common node for sub-faces --> return one of the edge nodes
	{
		if (fCode1 == 0)
		{
			return {level-1,f1NodeLeft,edgeCode,
			        level-1,f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode1 == f1NodeLeft)
		{
			return {lastLevelAtNode(level,f1NodeLeft,level,P1),f1NodeLeft,edgeCode,
			        level-1,f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode1 == f1NodeRight)
		{
			return {lastLevelAtNode(level,f1NodeRight,level,P1),f1NodeRight,edgeCode,
			        level-1,f2NodeRight,edgeCode,
			        0,1};
		}
	}

	if (fCode1 == 0)
	{
		faceGeometry faceGeom1(P1.calcFaceGeometry(level-1));

		if (fCode2 == 0) //common node is in both sub-faces the one touching the common edge
		{
			faceGeometry faceGeom2(P2.calcFaceGeometry(level-1));

			stepupFaceGeometryFrom(level-1, f1NodeLeft, faceGeom1);
			stepupFaceGeometryFrom(level-1, f2NodeLeft, faceGeom2);

			return {lastLevelAtNode(level+1,edgeCode,level1,P1),edgeCode,f1NodeLeft,
			        lastLevelAtNode(level+1,edgeCode,level2,P2),edgeCode,f2NodeLeft,
			        interiorAngle(level, f1NodeRight, faceGeom1) + interiorAngle(level, f2NodeRight, faceGeom2),1};
		}
		else if (fCode2 == f2NodeLeft)
		{
			stepupFaceGeometryFrom(level-1, f1NodeLeft, faceGeom1);

			return {lastLevelAtNode(level+1,edgeCode,level1,P1),edgeCode,f1NodeLeft,
			        lastLevelAtNode(level+1,f2NodeRight,level2,P2),f2NodeRight,edgeCode,
			        interiorAngle(level, f1NodeRight, faceGeom1),1};
		}
		else if (fCode2 == f2NodeRight) //one edge of the sub-face face2 is half of the common edge
		{
			stepupFaceGeometryFrom(level-1, f1NodeRight, faceGeom1);

			return {lastLevelAtNode(level+1,edgeCode,level1,P1),edgeCode,f1NodeRight,
			        lastLevelAtNode(level+1,f2NodeLeft,level2,P2),f2NodeLeft,edgeCode,
			        interiorAngle(level, f1NodeLeft, faceGeom1),1};
		}
	}

	if (fCode2 == 0)
	{
		faceGeometry faceGeom2(P2.calcFaceGeometry(level-1));

		if (fCode1 == f1NodeLeft)
		{
			stepupFaceGeometryFrom(level-1, f2NodeLeft, faceGeom2);

			return {lastLevelAtNode(level+1,f1NodeRight,level1,P1),f1NodeRight,edgeCode,
			        lastLevelAtNode(level+1,edgeCode,level2,P2),edgeCode,f2NodeLeft,
			        interiorAngle(level, f2NodeRight, faceGeom2),1};
		}
		else if (fCode1 == f1NodeRight)
		{
			stepupFaceGeometryFrom(level-1, f2NodeRight, faceGeom2);

			return {lastLevelAtNode(level+1,f1NodeLeft,level1,P1),f1NodeLeft,edgeCode,
			        lastLevelAtNode(level+1,edgeCode,level2,P2),edgeCode,f2NodeRight,
			        interiorAngle(level, f2NodeLeft, faceGeom2),1};
		}
	}

	if (fCode1 == f1NodeLeft)
	{
		if (fCode2 == f2NodeLeft)
		{
			return commonEdgeStepupTo(level+1, edgeCode, orientationMatch, level1, P1, level2, P2);
		}
		else if (fCode2 == f2NodeRight)
		{
			return {lastLevelAtNode(level+1,f1NodeRight,level1,P1),f1NodeRight,edgeCode,
			        lastLevelAtNode(level+1,f2NodeLeft,level2,P2),f2NodeLeft,edgeCode,
			        pi,-1}; //add angles as alpha1 + sign(gamma)*(alpha2+gamma)
		}
	}
	else if (fCode1 == f1NodeRight)
	{
		if (fCode2 == f2NodeRight)
		{
			return commonEdgeStepupTo(level+1, edgeCode, orientationMatch, level1, P1, level2, P2);
		}
		else if (fCode2 == f2NodeLeft)
		{
			return {lastLevelAtNode(level+1,f1NodeLeft,level1,P1),f1NodeLeft,edgeCode,
			        lastLevelAtNode(level+1,f2NodeRight,level2,P2),f2NodeRight,edgeCode,
			        pi,-1}; //add angles as alpha1 + sign(gamma)*(alpha2+gamma)
		}
	}
	return {0,0,0, 0,0,0, 0,0};
}




