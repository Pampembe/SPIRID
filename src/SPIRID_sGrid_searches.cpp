#include <SPIRID.h>

//main function to find a reference node of two faces
SPIRID::sGrid::pointPairRefNode
SPIRID::sGrid::findHighestRefNode(
    size_t level1,
    const sGrid& P1,
    size_t level2,
    const sGrid& P2)
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
SPIRID::sGrid::sameFaceStepupTo(
    size_t level,
    size_t level1,
    const sGrid& P1,
    size_t level2,
    const sGrid& P2)
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
SPIRID::sGrid::commonEdgeStepupTo(
    size_t level,
    unsigned short edgeCode,
    bool orientationMatch,
    size_t level1,
    const sGrid& P1,
    size_t level2,
    const sGrid& P2)
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





std::list<SPIRID::sGrid>
SPIRID::sGrid::neighborFaces(size_t level) const
{
	std::list<SPIRID::sGrid> neighbors;
	neighbors.push_back(*this);
	neighbors.back().resize(level);
	neighbors.push_back(neighborFace(level,1));
	neighbors.push_back(neighborFace(level,2));
	neighbors.push_back(neighborFace(level,3));
	return neighbors;
};
std::list<SPIRID::sGrid>
SPIRID::sGrid::edgeNeighborFaces(size_t level, unsigned short edgeCode) const
{
	std::list<SPIRID::sGrid> neighbors;
	neighbors.push_back(*this);
	neighbors.back().resize(level);
	neighbors.push_back(neighborFace(level,edgeCode));
	return neighbors;
}
std::list<SPIRID::sGrid>
SPIRID::sGrid::nodeNeighborFaces(size_t level, unsigned short nodeCode) const
{
	std::list<SPIRID::sGrid> neighbors;
	neighbors.push_back(*this);
	neighbors.back().resize(level);

	unsigned short edgeCodeLeft = nextNode(nodeCode);
	unsigned short edgeCodeRight = nextNode(edgeCodeLeft);

	bool orientationMatchLeft = true;
	bool orientationMatchRight = true;
	bool orientationMatchOther = true;

	neighbors.push_back(neighborFace(level,edgeCodeLeft,orientationMatchLeft));

	if (orientationMatchLeft)
	{
		neighbors.push_back(neighbors.back().neighborFace(level,nodeCode,orientationMatchOther));
		if (orientationMatchOther)
		{
			neighbors.push_back(neighbors.back().neighborFace(level,edgeCodeRight,orientationMatchRight));
		}
		else
		{
			neighbors.push_back(neighbors.back().neighborFace(level,edgeCodeLeft,orientationMatchRight));
		}

		neighbors.push_back(neighborFace(level,edgeCodeRight));
		if (orientationMatchRight)
		{
			neighbors.push_back(neighbors.back().neighborFace(level,nodeCode));
		}
		else
		{
			neighbors.push_back(neighbors.back().neighborFace(level,edgeCodeLeft));
		}
	}
	else
	{
		neighbors.push_back(neighbors.back().neighborFace(level,edgeCodeRight,orientationMatchOther));

		if (orientationMatchOther)
		{
			neighbors.push_back(neighbors.back().neighborFace(level,nodeCode));
			neighbors.push_back(neighborFace(level,edgeCodeRight,orientationMatchRight));
			//orientationMatchRight must be true by grid construction
			neighbors.push_back(neighbors.back().neighborFace(level,nodeCode));
		}
		else // orientationMatchOther is false, i.e., there are only four neighbor faces
		{
			neighbors.push_back(neighborFace(level,edgeCodeRight,orientationMatchRight));
		}
	}

	return neighbors;
}
std::list<std::pair<SPIRID::sGrid, unsigned short> >
SPIRID::sGrid::nodeNeighborNodes(size_t level, unsigned short nodeCode) const
{
	std::list<std::pair<SPIRID::sGrid, unsigned short> > neighborNodes;

	unsigned short edgeCodeLeft = nextNode(nodeCode);
	unsigned short edgeCodeRight = nextNode(edgeCodeLeft);

	bool orientationMatchLeft = true;
	bool orientationMatchRight = true;
	bool orientationMatchOther = true;

	neighborNodes.push_back({*this, edgeCodeRight});
	neighborNodes.back().first.resize(level);
	neighborNodes.push_back({neighborNodes.back().first.neighborFace(level,edgeCodeLeft,orientationMatchLeft), edgeCodeLeft});

	if (orientationMatchLeft)
	{
		neighborNodes.push_back({neighborNodes.back().first.neighborFace(level,nodeCode,orientationMatchOther), nodeCode});

		if (orientationMatchOther)
		{
			neighborNodes.push_back({neighborNodes.back().first.neighborFace(level,edgeCodeRight,orientationMatchRight), edgeCodeRight});
		}
		else
		{
			neighborNodes.push_back({neighborNodes.back().first.neighborFace(level,edgeCodeLeft,orientationMatchRight), edgeCodeLeft});
		}

		neighborNodes.push_back({neighborNodes.front().first.neighborFace(level,edgeCodeRight), edgeCodeRight});
		neighborNodes.push_back({neighborNodes.front().first, edgeCodeLeft});
	}
	else
	{
		neighborNodes.push_back({neighborNodes.back().first.neighborFace(level,edgeCodeRight,orientationMatchOther), edgeCodeRight});

		if (orientationMatchOther)
		{
			neighborNodes.push_back({neighborNodes.back().first.neighborFace(level,nodeCode), nodeCode});
			neighborNodes.push_back({neighborNodes.front().first.neighborFace(level,edgeCodeRight,orientationMatchRight), edgeCodeRight});
			//orientationMatchRight must be true by grid construction
			neighborNodes.push_back({neighborNodes.front().first, edgeCodeLeft});
		}
		else // orientationMatchOther is false, i.e., there are only four neighbor faces
		{
			neighborNodes.push_back({neighborNodes.front().first, edgeCodeLeft});
		}
	}


	return neighborNodes;
}
std::list<std::pair<SPIRID::sGrid, unsigned short> >
SPIRID::sGrid::nodeConnectedEdges(size_t level, unsigned short nodeCode) const
{
	std::list<std::pair<SPIRID::sGrid, unsigned short> > connectedEdges;

	unsigned short edgeCodeLeft = nextNode(nodeCode);
	unsigned short edgeCodeRight = nextNode(edgeCodeLeft);

	connectedEdges.push_back({*this, edgeCodeLeft});
	connectedEdges.back().first.resize(level);

	bool orientationMatchLeft = true;
	bool orientationMatchRight = true;
	bool orientationMatchOther = true;

	connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,edgeCodeLeft,orientationMatchLeft), nodeCode});
	if (orientationMatchLeft)
	{
		connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,nodeCode,orientationMatchOther), edgeCodeRight});

		if (orientationMatchOther)
		{
			connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,edgeCodeRight,orientationMatchRight), edgeCodeLeft});
			if (!orientationMatchRight) connectedEdges.back().second = nodeCode;
		}
		else
		{
			connectedEdges.back().second = edgeCodeLeft;
			connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,edgeCodeLeft,orientationMatchRight), edgeCodeRight});
		}
		if (orientationMatchRight)
		{
			connectedEdges.push_back({connectedEdges.front().first.neighborFace(level,edgeCodeRight), nodeCode});
		}
		else
		{
			connectedEdges.push_back({connectedEdges.front().first.neighborFace(level,edgeCodeRight), edgeCodeLeft});
		}
	}
	else
	{
		connectedEdges.back().second = edgeCodeRight;
		connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,edgeCodeRight,orientationMatchOther), nodeCode});

		if (orientationMatchOther)
		{
			connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,nodeCode), edgeCodeLeft});
			connectedEdges.push_back({connectedEdges.front().first.neighborFace(level,edgeCodeRight), nodeCode});
		}
		else // orientationMatchOther is false, i.e., there are only four neighbor faces
		{
			connectedEdges.back().second = edgeCodeLeft;
		}
	}
	connectedEdges.push_back({connectedEdges.front().first, edgeCodeRight});

	return connectedEdges;
}
std::list<std::pair<SPIRID::sGrid, unsigned short> >
SPIRID::sGrid::nodeOuterRingEdges(size_t level, unsigned short nodeCode) const
{
	std::list<std::pair<SPIRID::sGrid, unsigned short> > connectedEdges;

	unsigned short edgeCodeLeft = nextNode(nodeCode);
	unsigned short edgeCodeRight = nextNode(edgeCodeLeft);

	connectedEdges.push_back({*this, nodeCode});
	connectedEdges.back().first.resize(level);

	bool orientationMatchLeft = true;
	bool orientationMatchRight = true;
	bool orientationMatchOther = true;

	connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,edgeCodeLeft,orientationMatchLeft), edgeCodeRight});
	if (orientationMatchLeft)
	{
		connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,nodeCode,orientationMatchOther), edgeCodeLeft});

		if (orientationMatchOther)
		{
			connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,edgeCodeRight,orientationMatchRight), nodeCode});
			if (orientationMatchRight)
			{
				connectedEdges.push_back({connectedEdges.front().first.neighborFace(level,edgeCodeRight), edgeCodeLeft});
				connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,nodeCode), edgeCodeRight});
			}
			else
			{
				connectedEdges.push_back({connectedEdges.front().first.neighborFace(level,edgeCodeRight), edgeCodeLeft});
				connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,edgeCodeLeft), edgeCodeRight});
			}
		}
		else
		{
			connectedEdges.back().second = edgeCodeRight;
			connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,edgeCodeLeft,orientationMatchRight), nodeCode});

			connectedEdges.push_back({connectedEdges.front().first.neighborFace(level,edgeCodeRight), edgeCodeLeft});
			connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,nodeCode), edgeCodeLeft});
		}
	}
	else
	{
		connectedEdges.back().second = nodeCode;
		connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,edgeCodeRight,orientationMatchOther), edgeCodeLeft});

		if (orientationMatchOther)
		{
			connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,nodeCode), edgeCodeRight});
			connectedEdges.push_back({connectedEdges.front().first.neighborFace(level,edgeCodeRight), edgeCodeLeft});
			connectedEdges.push_back({connectedEdges.back().first.neighborFace(level,nodeCode), edgeCodeRight});
		}
		else // orientationMatchOther is false, i.e., there are only four neighbor faces
		{
			connectedEdges.back().second = nodeCode;
			connectedEdges.push_back({connectedEdges.front().first.neighborFace(level,edgeCodeRight), nodeCode});
		}
	}


	return connectedEdges;
}



//iterator-like class to step through a list of sGrid coordinates
bool
SPIRID::sGrid::subGridScanner::stepToNextFace(size_t gridLevel)
{
	if (gridLevel < gridMinLevel) return false;
	if (gridLevel == gridMinLevel)
	{
		if (gridLevel == 0)
		{
			if (currentFace->operator[](0) == 7) return false;
			else
			{
				currentFace->set(0,currentFace->operator[](0)+1);
				return true;
			}
		}

		if (currentFace->operator[](gridLevel) == 3) return false;
		else
		{
			currentFace->set(gridLevel,currentFace->operator[](gridLevel)+1);
			return true;
		}
	}
	else
	{
		if (currentFace->operator[](gridLevel) == 3)
		{
			currentFace->set(gridLevel,0);
			return stepToNextFace(gridLevel-1);
		}
		else
		{
			currentFace->set(gridLevel,currentFace->operator[](gridLevel)+1);
			return true;
		}
	}
}
SPIRID::sGrid::subGridScanner::subGridScanner(const std::list<sGrid>& faceList, size_t scanLevel, size_t minLevel) :
	scannerLevel(scanLevel),
	gridMinLevel(minLevel),
	scanFaces(faceList),
	currentFace(scanFaces.begin()),
	endIterator(scanFaces.end())
{
	for (currentFace = scanFaces.begin(); currentFace != endIterator; ++currentFace)
	{
		currentFace -> resize(scanLevel);
		currentFace -> reset(minLevel);
	}
	currentFace = scanFaces.begin();
}
void
SPIRID::sGrid::subGridScanner::reset()
{
	for (currentFace = scanFaces.begin(); currentFace != endIterator; ++currentFace)
	{
		currentFace -> reset(gridMinLevel);
	}
	currentFace = scanFaces.begin();
}
SPIRID::sGrid::subGridScanner&
SPIRID::sGrid::subGridScanner::operator = (const subGridScanner& X)
{
	scannerLevel = X.scannerLevel;
	gridMinLevel = X.gridMinLevel;
	scanFaces = X.scanFaces;
	currentFace = scanFaces.begin();
	endIterator = scanFaces.end();
	reset();
	return *this;
}
bool
SPIRID::sGrid::subGridScanner::operator == (const subGridScanner& s2) const
{
	if (currentFace == endIterator && s2.currentFace == s2.endIterator) return true;
	if (currentFace == endIterator || s2.currentFace == s2.endIterator) return false;
	return *currentFace == *s2.currentFace;
}



SPIRID::sGrid::subGridScanner
SPIRID::sGrid::begin(size_t scanLevel, size_t minLevel) const
{
	std::list<sGrid> faces;
	faces.push_back(*this);
	return subGridScanner(faces,scanLevel,minLevel);
}
SPIRID::sGrid::subGridScanner
SPIRID::sGrid::end() const
{
	std::list<sGrid> faces;
	subGridScanner s(faces,0,1);
	return s;
}
