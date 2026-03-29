#include <cmath>
#include <limits>
#include <SPIRID.h>

//sGrid constructors
SPIRID::sGrid::sGrid(const std::vector<bool>& bitCode) : gridCode(bitCode)
{
	size_t size = gridCode.size();
	if (size<3) gridCode.resize(3);
	if (size%2==0) gridCode.resize(size+1);
}
SPIRID::sGrid::sGrid(const std::vector<unsigned short>& faceCodes) : gridCode(1+2*faceCodes.size())
{
	for (size_t it = 0; it <= faceCodes.size(); ++it)
	{
		set(it, faceCodes[it]);
	}
}

//get faceCode at a certain level
unsigned short
SPIRID::sGrid::at(size_t level) const
{
	if (2*level+3>gridCode.size()) return 0;
	else return operator [] (level);
};
unsigned short
SPIRID::sGrid::operator [] (size_t level) const
{
	if (level==0) return gridCode[0]+2*gridCode[1]+4*gridCode[2];
	return gridCode[2*level+1]+2*gridCode[2*level+2];
};
//set faceCode at a certain level
const SPIRID::sGrid&
SPIRID::sGrid::setExtend(size_t level, unsigned short faceCode)
{
	if (2*level+3>gridCode.size()) gridCode.resize(2*level+3);
	return set(level, faceCode);
}
const SPIRID::sGrid&
SPIRID::sGrid::reset(size_t level)
{
	size_t start = 2*level+1;
	if (level == 0) start = 0;
	std::fill(gridCode.begin()+start, gridCode.end(), false);
	return *this;
}
const SPIRID::sGrid&
SPIRID::sGrid::set(size_t level, unsigned short faceCode)
{
	if (level==0)
	{
		switch (faceCode)
		{
		case 0 : {
			gridCode[0]=false;
			gridCode[1]=false;
			gridCode[2]=false;
			return *this;
		}
		case 1 : {
			gridCode[0]=true;
			gridCode[1]=false;
			gridCode[2]=false;
			return *this;
		}
		case 2 : {
			gridCode[0]=false;
			gridCode[1]=true;
			gridCode[2]=false;
			return *this;
		}
		case 3 : {
			gridCode[0]=true;
			gridCode[1]=true;
			gridCode[2]=false;
			return *this;
		}
		case 4 : {
			gridCode[0]=false;
			gridCode[1]=false;
			gridCode[2]=true;
			return *this;
		}
		case 5 : {
			gridCode[0]=true;
			gridCode[1]=false;
			gridCode[2]=true;
			return *this;
		}
		case 6 : {
			gridCode[0]=false;
			gridCode[1]=true;
			gridCode[2]=true;
			return *this;
		}
		case 7 : {
			gridCode[0]=true;
			gridCode[1]=true;
			gridCode[2]=true;
			return *this;
		}
		default: {
			gridCode[0]=true;
			gridCode[1]=true;
			gridCode[2]=true;
			return *this;
		}
		}
	}
	else
	{
		switch (faceCode)
		{
		case 0 : {
			gridCode[2*level+1]=false;
			gridCode[2*level+2]=false;
			return *this;
		}
		case 1 : {
			gridCode[2*level+1]=true;
			gridCode[2*level+2]=false;
			return *this;
		}
		case 2 : {
			gridCode[2*level+1]=false;
			gridCode[2*level+2]=true;
			return *this;
		}
		case 3 : {
			gridCode[2*level+1]=true;
			gridCode[2*level+2]=true;
			return *this;
		}
		default: {
			gridCode[2*level+1]=true;
			gridCode[2*level+2]=true;
			return *this;
		}
		}
	}
}

//special points
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::NorthOct0 = {SPIRID::sGrid(std::vector<bool>({0,0,0})),3};
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::WestOct0  = {SPIRID::sGrid(std::vector<bool>({0,0,0})),2};
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::SouthOct7 = {SPIRID::sGrid(std::vector<bool>({1,1,1})),3};
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::EastOct7  = {SPIRID::sGrid(std::vector<bool>({1,1,1})),2};



//accuracy settings
size_t SPIRID::sGrid::accuracyBits       = std::numeric_limits<fp_type>::digits;
size_t SPIRID::sGrid::minDepthFlatApprox = SPIRID::sGrid::accuracyBits/2+1;
size_t SPIRID::sGrid::minScaleSinXToX    = SPIRID::sGrid::accuracyBits/3+1;
void SPIRID::sGrid::setAccuracyBits(size_t bitCount)
{
	if (bitCount>std::numeric_limits<fp_type>::digits)
	{
		std::cerr << "warning: floating point type supports only " << std::numeric_limits<fp_type>::digits << " binary digits." << std::endl;
		bitCount = std::numeric_limits<fp_type>::digits;
	}
	SPIRID::sGrid::accuracyBits = bitCount;
	SPIRID::sGrid::minDepthFlatApprox = SPIRID::sGrid::accuracyBits/2+1;
	SPIRID::sGrid::minScaleSinXToX    = SPIRID::sGrid::accuracyBits/3+1;
}






SPIRID::sGrid
SPIRID::sGrid::neighborFace(size_t level, unsigned short edgeCode) const
{
	bool orientationMatch = true;
	return neighborFace(level, edgeCode, orientationMatch);
};
SPIRID::sGrid
SPIRID::sGrid::neighborFace(size_t level, unsigned short edgeCode, bool& orientationMatch) const
{
	sGrid neighbor(*this);
	neighbor.resize(level);
	orientationMatch = true;
	return assignNeighborFace(level, edgeCode, neighbor, orientationMatch);
};
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
	neighborNodes.push_back({*this, nodeCode});
	neighborNodes.back().first.resize(level);

	unsigned short edgeCodeLeft = nextNode(nodeCode);
	unsigned short edgeCodeRight = nextNode(edgeCodeLeft);

	bool orientationMatchLeft = true;
	bool orientationMatchRight = true;
	bool orientationMatchOther = true;

	neighborNodes.push_back({neighborNodes.back().first, edgeCodeRight});
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

void
SPIRID::sGrid::subGridScanner::reset()
{
	for (currentFace = scanFaces.begin(); currentFace != endIterator; ++currentFace)
	{
		currentFace -> reset(gridMinLevel);
	}
	currentFace = scanFaces.begin();
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
bool
SPIRID::sGrid::subGridScanner::operator == (const subGridScanner& s2) const
{
	if (currentFace == endIterator && s2.currentFace == s2.endIterator) return true;
	if (currentFace == endIterator || s2.currentFace == s2.endIterator) return false;
	return *currentFace == *s2.currentFace;
}














//main function to find the common node of two faces
SPIRID::sGrid::commonNode
SPIRID::sGrid::findHighestCommonNode(size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
{
	//ensure level1 <= level2
	if (level1 > level2)
	{
		commonNode N(findHighestCommonNode(level2, P2, level1, P1));
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
	// cases 1,2,4 --> unique common node at level0 already, 1: N1, 2: N2, 4: N3
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
SPIRID::sGrid::commonNode
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

	faceGeometry faceGeom(P1.calcFaceGeometry(level-1));
	stepupFaceGeometryFrom(level-1, 0, faceGeom); //need interior angles from center face (code 0)

	unsigned short edgeCode = newCode(fCode1,fCode2);

	return {lastLevelAtNode(nextLevel,fCode2,level1,P1),fCode2,edgeCode,
	        lastLevelAtNode(nextLevel,fCode1,level2,P2),fCode1,edgeCode,
	        pi,1}; // using that the angle between the relevant edges in P1 and P2 is pi and cos(Pi+x)==Cos(Pi-x)
}
SPIRID::sGrid::commonNode
SPIRID::sGrid::commonEdgeStepupTo(size_t level, unsigned short edgeCode, bool orientationMatch, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
{
	unsigned short f1NodeLeft  = nextNode(edgeCode);
	unsigned short f1NodeRight = newCode(edgeCode,f1NodeLeft);

	unsigned short f2NodeLeft  = f1NodeLeft;
	unsigned short f2NodeRight = f1NodeRight;

	if (orientationMatch) //if face orientation matches then common edge orientation is opposite
	{
		f2NodeLeft  = f1NodeRight;
		f2NodeRight = f1NodeLeft;
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
	return {0,0,0,0,0,0,0};
}



SPIRID::sGrid&
SPIRID::sGrid::assignNeighborFace(
    size_t level,
    unsigned short edgeCode,
    sGrid& neighbor,
    bool& orientationMatch)
{
	unsigned short fCode = neighbor[level];

	if (level==0)
	{
		orientationMatch = false;
		neighbor.gridCode[edgeCode-1] = !neighbor.gridCode[edgeCode-1];
		return neighbor;
	}

	if (fCode == 0)
	{
		neighbor.set(level, edgeCode);
	}
	else if (fCode == edgeCode)
	{
		neighbor.set(level, 0);
	}
	else
	{
		assignNeighborFace(level-1, edgeCode, neighbor, orientationMatch);
		if (orientationMatch) neighbor.set(level, newCode(fCode,edgeCode));
		else neighbor.set(level, fCode);
	}

	return neighbor;
}






std::list<std::pair<SPIRID::sGrid,unsigned short> >&
SPIRID::sGrid::collectNeighborNodesTo(
    std::list<std::pair<sGrid,unsigned short> >& neighborList,
    size_t level,
    const sGrid& P0,
    unsigned short location)
{
	unsigned short nodeCode0 = location;
	unsigned short nodeCode1 = nextNode(nodeCode0);
	unsigned short nodeCode2 = newCode(nodeCode0,nodeCode1);

	neighborList.push_front({sGrid(P0),nodeCode1});
	neighborList.push_front({sGrid(P0),nodeCode2});

	bool orientationMatch = true;

	neighborList.push_front(neighborList.front());
	assignNeighborFace(level, nodeCode1, neighborList.front().first, orientationMatch);
	neighborList.front().second = nodeCode1;

	neighborList.push_front(neighborList.front());
	// which next neighbor to be added depends on whether orientation is matched
	if (orientationMatch)
	{
		assignNeighborFace(level, nodeCode0, neighborList.front().first, orientationMatch);
		neighborList.front().second = nodeCode0;

		neighborList.push_front(neighborList.front());
		if (orientationMatch)
		{
			assignNeighborFace(level, nodeCode2, neighborList.front().first, orientationMatch);
			neighborList.front().second = nodeCode2;

			neighborList.push_front(neighborList.front());
			if (orientationMatch)
			{
				assignNeighborFace(level, nodeCode1, neighborList.front().first, orientationMatch);
				neighborList.front().second = nodeCode1;
			}
			else
			{
				//need to reset orientationMatch, otherwise result will not be the neighbor face
				orientationMatch = true;
				assignNeighborFace(level, nodeCode0, neighborList.front().first, orientationMatch);
				neighborList.front().second = nodeCode0;
			}
		}
		else
		{
			//need to reset orientationMatch, otherwise result will not be the neighbor face
			orientationMatch = true;
			assignNeighborFace(level, nodeCode1, neighborList.front().first, orientationMatch);
			neighborList.front().second = nodeCode1;

			//by construction of the grid, orientation in the previous step must be matched
			neighborList.push_front(neighborList.front());
			assignNeighborFace(level, nodeCode2, neighborList.front().first, orientationMatch);
			neighborList.front().second = nodeCode2;
		}
	}
	else
	{
		//need to reset orientationMatch, otherwise result will not be the neighbor face
		orientationMatch = true;
		assignNeighborFace(level, nodeCode2, neighborList.front().first, orientationMatch);
		neighborList.front().second = nodeCode2;

		if (orientationMatch)
		{
			neighborList.push_front(neighborList.front());
			assignNeighborFace(level, nodeCode0, neighborList.front().first, orientationMatch);
			neighborList.front().second = nodeCode0;

			//by construction of the grid, orientation in the previous step must be matched
			neighborList.push_front(neighborList.front());
			assignNeighborFace(level, nodeCode1, neighborList.front().first, orientationMatch);
			neighborList.front().second = nodeCode1;
		}
		//if orientationMatch is two times false then by construction of the grid there are only four adjacent nodes
	}
	/*	std::cout << "neighbor nodes: ";
		for (std::list<std::pair<SPIRID::sGrid,unsigned short> >::const_iterator it = neighborList.begin(); it != neighborList.end(); ++it)
		{
		    std::cout << it->first << it->second << " ";
		}
	    std::cout << P0 << location << " ";
		std::cout << std::endl;
	*/
	return neighborList;
}
/*
std::list<std::pair<SPIRID::sGrid,unsigned short> >&
SPIRID::sGrid::collectNeighborPointsTo(
    std::list<std::pair<sGrid,unsigned short> >& neighborList,
    size_t level,
    const sGrid& P0,
    unsigned short location)
{
	unsigned short nodeCode0 = location;
	unsigned short nodeCode1 = nextNode(nodeCode0);
	unsigned short nodeCode2 = newCode(nodeCode0,nodeCode1);

	collectNeighborNodesTo(neighborList, level, P0, location);

	bool orientationMatch = true;

	neighborList.push_front(neighborList.front());
	assignNeighborFace(level, nodeCode2, neighborList.front().first, orientationMatch);

	for (std::list<std::pair<sGrid,unsigned short> >::iterator it = neighborList.begin(); it !=neighborList.end(); ++it)
	{
		neighborList.push_front(*it);
		it->second = 0;
	}

	return neighborList;
}
*/
/*
SPIRID::funcGraphPoint<>&
SPIRID::sGrid::getMinPoint(
    const std::list<std::pair<sGrid,unsigned short> >& pointList,
    size_t level,
    funcGraphPoint<>& reference,
    scaledFP (*minFunc)(size_t, const sGrid&, unsigned short))
{
	for (std::list<std::pair<sGrid,unsigned short> >::const_iterator it = pointList.begin(); it!=pointList.end(); ++it)
	{
		scaledFP funcVal = minFunc(level, it->first, it->second);
		sPolar pol((it->first).toPolar(level,it->second));
		std::cout << "getMinPoint: ";
		std::cout << it->first;
		std::cout << it->second;
		std::cout << "(" << pol.getTheta()-1*pi/8 << "," << pol.getPhi()-1*pi/8 << ")" << ":";
		std::cout << funcVal << std::endl;

		if (funcVal < reference.fValue)
		{
			reference.dPoint = *it;
			reference.fValue = funcVal;
		}
	}
	std::cout << "result: ";
	std::cout << reference.dPoint.first;
	std::cout << reference.dPoint.second << ":";
	std::cout << reference.fValue << std::endl;
	return reference;
}
SPIRID::funcGraphPoint<>&
SPIRID::sGrid::localSearchMinNode(
    size_t level,
    funcGraphPoint<>& reference,
    scaledFP (*minFunc)(size_t, const sGrid&, unsigned short))
{
	std::list<std::pair<sGrid,unsigned short> > neighborList;
	collectNeighborNodesTo(neighborList,
	                       level,
	                       reference.dPoint.first,
	                       reference.dPoint.second);

	getMinPoint(neighborList, level, reference, minFunc);
	return reference;
}
*/
SPIRID::funcGraphPoint<>
SPIRID::sGrid::searchMinPoint(size_t maxLevel, scaledFP (*minFunc)(size_t, const sGrid&, unsigned short))
{
	sGrid refPoint(NorthOct0.first);

	sGrid refMinFace(NorthOct0.first);
	unsigned short refMinLocation = NorthOct0.second;
	unsigned short refEdgeCode = 1;
	bool refEdgeOrientationMatch = false;

	scaledFP refMinValue = minFunc(0, refMinFace, 3);
	scaledFP refMin2ndValue = minFunc(0, WestOct0.first, WestOct0.second);

	if (refMin2ndValue < refMinValue) // west is smaller than north
	{
		scaledFP dummyValue = minFunc(0, EastOct7.first, EastOct7.second);
		if (dummyValue < refMin2ndValue) // east is smaller than west and north
		{
			refMinFace = EastOct7.first;
			refMinLocation = EastOct7.second;
			refMin2ndValue = refMinValue; //2nd node becomes north
			refMinValue = dummyValue;
		}
		else // west is smaller than north and east
		{
			refMinFace = WestOct0.first;
			refMinLocation = WestOct0.second;
			std::swap(refMinValue,refMin2ndValue);
		}
	}
	else // north is smaller than west
	{
		scaledFP dummyValue = minFunc(0, SouthOct7.first, SouthOct7.second);
		if (dummyValue < refMinValue) // south is smaller than west and north
		{
			refMinFace = SouthOct7.first;
			refMinLocation = SouthOct7.second;
			// refMin2ndValue unchanged: 2nd node becomes west
			refMinValue = dummyValue;
		}
		else // north is smaller than south and west
		{
			refMinFace = NorthOct0.first;
			refMinLocation = NorthOct0.second;
			// refMinValue & refMin2ndValue unchanged
		}

	}
	sGrid refEdgeNeighborFace(refMinFace.neighborFace(0,refEdgeCode));
	//refMinFace.resize(maxLevel);
	//refEdgeNeighborFace.resize(maxLevel);

	std::cout << std::endl;
	std::cout << "initial setup: [";
	std::cout << refMinFace;
	std::cout << "" << refMinLocation;
	std::cout << "]" << refMinValue;
	std::cout << " - " << refEdgeCode;
	std::cout << "," << refEdgeOrientationMatch;
	std::cout << " - " << refEdgeNeighborFace;
	std::cout << ", " << refMin2ndValue;
	std::cout << std::endl;

	searchLocalMinAtLevel(0,
	                      refMinFace, refMinLocation, refMinValue,
	                      refEdgeCode, refEdgeNeighborFace, refEdgeOrientationMatch,
	                      refMin2ndValue, minFunc);

	std::cout << std::endl;
	std::cout << "level 0 search result: [";
	std::cout << refMinFace;
	std::cout << "" << refMinLocation;
	std::cout << "]" << refMinValue;
	std::cout << " - " << refEdgeCode;
	std::cout << "," << refEdgeOrientationMatch;
	std::cout << " - " << refEdgeNeighborFace;
	std::cout << std::endl;

//	std::cout << "next level nodes:" << std::endl;
	sGrid::subGridScanner tmp(refMinFace.nodeNeighborFaces(0,refMinLocation),1,1);
	scaledFP minimum = refMinValue;
	std::pair<sGrid, unsigned short> minPoint = {refMinFace, refMinLocation};
	for (; tmp != refMinFace.end(); ++tmp)
	{
		if (minFunc(1,*tmp,1) < minimum)
		{
			minimum = minFunc(1,*tmp,1);
			minPoint = {*tmp,1};
		}

		if (minFunc(1,*tmp,2) < minimum)
		{
			minimum = minFunc(1,*tmp,2);
			minPoint = {*tmp,2};
		}

		if (minFunc(1,*tmp,3) < minimum)
		{
			minimum = minFunc(1,*tmp,3);
			minPoint = {*tmp,3};
		}

		/*	    std::cout << *tmp << minFunc(1,*tmp,1) << " ";
			    std::cout << *tmp << minFunc(1,*tmp,2) << " ";
			    std::cout << *tmp << minFunc(1,*tmp,3);
		    	std::cout << std::endl;
		*/
	}
	std::cout << "next level minimum: " << minPoint.first << minPoint.second << ": " << minFunc(1,minPoint.first,minPoint.second);
	std::cout << std::endl;

	for (size_t level_it = 0; level_it < maxLevel; level_it++)
	{
		refMinFace.setExtend(level_it+1,refMinLocation);
		refEdgeNeighborFace.resize(level_it+1);
		searchLocalMinNextLevel(level_it,
		                        refMinFace, refMinLocation, refMinValue,
		                        refEdgeCode, refEdgeNeighborFace, refEdgeOrientationMatch,
		                        minFunc);
		std::cout << std::endl;
		std::cout << "level " << level_it+1 << " search result: [";
		std::cout << refMinFace;
		std::cout << "" << refMinLocation;
		std::cout << "]" << refMinValue;
		std::cout << " - " << refEdgeCode;
		std::cout << "," << refEdgeOrientationMatch;
		std::cout << " - " << refEdgeNeighborFace;
		std::cout << std::endl;

//	std::cout << "next level nodes:" << std::endl;
		tmp = sGrid::subGridScanner(refMinFace.nodeNeighborFaces(level_it+1,refMinLocation),level_it+2,level_it+2);
		minimum = refMinValue+1;
		minPoint = {refMinFace, refMinLocation};
		for (; tmp != refMinFace.end(); ++tmp)
		{
			if (minFunc(level_it+2,*tmp,1) < minimum)
			{
				minimum = minFunc(level_it+2,*tmp,1);
				minPoint = {*tmp,1};
			}

			if (minFunc(level_it+2,*tmp,2) < minimum)
			{
				minimum = minFunc(level_it+2,*tmp,2);
				minPoint = {*tmp,2};
			}

			if (minFunc(level_it+2,*tmp,3) < minimum)
			{
				minimum = minFunc(level_it+2,*tmp,3);
				minPoint = {*tmp,3};
			}
		}
		std::cout << "next level minimum: " << minPoint.first << minPoint.second << ": " << minFunc(level_it+2,minPoint.first,minPoint.second);
		std::cout << std::endl;

	}
	return {{refMinFace,refMinLocation},refMinValue};

	/*
		funcGraphPoint<> reference = {{refPoint, 3}, minFunc(0,refPoint,3)};

		scaledFP newFuncValue = minFunc(0,SouthOct7.first,3);
		if (newFuncValue < reference.fValue)
		{
			reference.dPoint = SouthOct7;
			reference.fValue = newFuncValue;
		}
		for (size_t level_it = 0; level_it < maxLevel; )
		{
			localSearchMinNode(level_it,reference,minFunc);
			reference.dPoint.first.setExtend(++level_it,reference.dPoint.second);
		}
		localSearchMinNode(maxLevel,reference,minFunc);

		return reference;
	*/

}


unsigned short
SPIRID::sGrid::minIndexLocalSearch(
    const scaledFP& centerValue,
    unsigned short outerValCount,
    const scaledFP** outerValues)
{
	//total number of outerValues is outerValCount, but we can assume that the last element of outValues is larger than centerValue
	outerValCount--;
	unsigned short minIndex = 0;
	scaledFP minimum = centerValue;
	for (size_t it = 0; it < outerValCount; it++)
	{
		if (*outerValues[it] < minimum)
		{
			minimum = *outerValues[it];
			minIndex = it+1;
		}
	}
	return minIndex;
}
unsigned short SPIRID::sGrid::min2ndIndexLocalSearch(
    unsigned short min1stIndex,
    const scaledFP& centerValue,
    unsigned short outerValCount,
    const scaledFP** outerValues)
{
	unsigned short min2ndIndex = 0;

	if (min1stIndex == 0)
	{
		scaledFP minimum2nd = *outerValues[0];
		for (size_t it = 1; it < outerValCount; it++)
		{
			if (*outerValues[it] < minimum2nd)
			{
				minimum2nd = *outerValues[it];
				min2ndIndex = it;
			}
		}
		return min2ndIndex+1;
	}
	else
	{
		unsigned short connectedOuterEdgeIndex1 = min1stIndex-2; //shift index by 1 to come out at the right position of outerValues
		unsigned short connectedOuterEdgeIndex2 = min1stIndex;
		if (min1stIndex==1) connectedOuterEdgeIndex1 = outerValCount-1;
		//min1stIndex cannot be the last outerValues
		//if (min1stIndex==outerValCount) connectedOuterEdgeIndex2 = 1;

		scaledFP minimum2nd = centerValue;

		if (*outerValues[connectedOuterEdgeIndex1] < minimum2nd)
		{
			minimum2nd = *outerValues[connectedOuterEdgeIndex1];
			min2ndIndex = connectedOuterEdgeIndex1+1;
		}
		if (*outerValues[connectedOuterEdgeIndex2] < minimum2nd)
		{
			minimum2nd = *outerValues[connectedOuterEdgeIndex2];
			min2ndIndex = connectedOuterEdgeIndex2+1;
		}
		return min2ndIndex;
	}
}
void SPIRID::sGrid::searchLocalMinAtLevel(
    size_t level,
    sGrid& refMinFace, // we search for a local minimum around a reference node, this is the corresponding grid face
    unsigned short& refMinLocation, // the location of the reference node inside refMinFace
    scaledFP& refMinValue, // the function value of minFunc at the reference node
    unsigned short& refEdgeCode, // a reference edge inside refMinFace that connects to the reference node
    sGrid& refEdgeNeighborFace, // neighbor face next to refMinFace across refEdge
    bool& refEdgeOrientationMatch, // whether orientation of left and right face of refEdge is matched
    const scaledFP& refMin2ndValue, // the function value at the 2nd node of refEdge
    scaledFP (*minFunc)(size_t, const sGrid&, unsigned short) // function to minimize
)
{
		std::cout << "AtLevel search: ";
	    std::cout << std::endl;
	/*
	    std::cout << std::endl;
		std::cout << "AtLevel search: ";
		std::cout << std::endl;

		std::cout << "Start: level: " << level;
		std::cout << " refMin: [" << refMinFace << refMinLocation << "]" << refMinValue;
		std::cout << " - " << refEdgeCode << "," << refEdgeOrientationMatch << " - ";
		std::cout << " neighbor: " << refEdgeNeighborFace << "]" << refMin2ndValue;
		std::cout << std::endl;
	*/
	// node codes at the left side of refEdge (inside the reference face)
	unsigned short nodeCode0 = refMinLocation; // node code of the reference node
	unsigned short nodeCode2 = refEdgeCode; // node code of the 3rd node in the face, which is not on refEdge
	unsigned short nodeCode1 = newCode(nodeCode0,nodeCode2); // node code of the 2nd on refEdge

	// node codes at the other side of refEdge
	unsigned short nodeCode0Right = nodeCode1;
	unsigned short nodeCode1Right = nodeCode0;
	if (!refEdgeOrientationMatch) std::swap(nodeCode0Right,nodeCode1Right);

	// two new function values in the faces adjacent to refEdge
	scaledFP leftNodeAFValue(minFunc(level, refMinFace, nodeCode2));
	scaledFP rightNodeAFValue(minFunc(level, refEdgeNeighborFace, nodeCode2));

	//placeholders for two unknown orientations
	bool edgeAOrientationMatch = true;
	bool edgeBOrientationMatch = true;

	// 1st new face & function value at the left side of the face
	sGrid leftNewFaceA(refMinFace.neighborFace(level, nodeCode1, edgeAOrientationMatch));
	scaledFP leftNodeBFValue(minFunc(level, leftNewFaceA, nodeCode1));

	const scaledFP* outerValues[6] = {&leftNodeAFValue,&leftNodeBFValue,&rightNodeAFValue,&refMin2ndValue,&rightNodeAFValue,&refMin2ndValue};
	sGrid* neighborFaces[6] = {&refMinFace,&leftNewFaceA,nullptr,&refEdgeNeighborFace,nullptr,&refEdgeNeighborFace};

	unsigned short faceCount = 6;
	// two edges with non-matched orientation can only happen at the nodes with four adjacent faces
	if (!(edgeAOrientationMatch || refEdgeOrientationMatch)) faceCount = 4;

	// create two new faces and function values if faceCount==6
	if (faceCount == 6)
	{
		neighborFaces[2] = new sGrid(leftNewFaceA);
		if (edgeAOrientationMatch)
		{
			assignNeighborFace(level, nodeCode0, *neighborFaces[2], edgeBOrientationMatch);
			outerValues[2] = new scaledFP(minFunc(level,*neighborFaces[2],nodeCode0));
		}
		else
		{
			assignNeighborFace(level, nodeCode2, *neighborFaces[2], edgeBOrientationMatch);
			outerValues[2] = new scaledFP(minFunc(level,*neighborFaces[2],nodeCode2));
		}

		neighborFaces[4] = new sGrid(refEdgeNeighborFace.neighborFace(level, nodeCode1Right));
		outerValues[3] = new scaledFP(minFunc(level,*neighborFaces[4],nodeCode1Right));
	}

	if (faceCount == 6)
	{
		unsigned short oppositeNodeCode;
		if (edgeAOrientationMatch) oppositeNodeCode = nodeCode0;
		else oppositeNodeCode = nodeCode2;
	std::cout << "search nodes: ";
		        std::cout << " refMinPoint: [" << refMinFace << refMinLocation << "]" << refMinValue << " " << minFunc(level,refMinFace,refMinLocation) << std::endl;
		        std::cout << " neighbors[0]: [" << *neighborFaces[0] << nodeCode2 << "]" << *outerValues[0] << " " << minFunc(level,*neighborFaces[0],nodeCode2) << std::endl;
				std::cout << " neighbors[1]: [" << *neighborFaces[1] << nodeCode1 << "]" << *outerValues[1] << " " << minFunc(level,*neighborFaces[1],nodeCode1) << std::endl;
				std::cout << " neighbors[2]: [" << *neighborFaces[2] << oppositeNodeCode << "]" << *outerValues[2] << " " << minFunc(level,*neighborFaces[2],oppositeNodeCode) << std::endl;
				std::cout << " neighbors[3]: [" << *neighborFaces[3] << nodeCode1Right << "]" << *outerValues[3] << " " << minFunc(level,*neighborFaces[4],nodeCode1Right) << std::endl;
				std::cout << " neighbors[4]: [" << *neighborFaces[4] << nodeCode2 << "]" << *outerValues[4] << " " << minFunc(level,*neighborFaces[5],nodeCode2) << std::endl;
				std::cout << " neighbors[5]: [" << *neighborFaces[5] << nodeCode1Right << "]" << *outerValues[5] << " " << minFunc(level,*neighborFaces[5],nodeCode1Right) << std::endl;
	}
	else
	{
	std::cout << "search nodes: ";
		        std::cout << " refMinPoint: [" << refMinFace << refMinLocation << "]" << refMinValue << " " << minFunc(level,refMinFace,refMinLocation) << std::endl;
		        std::cout << " neighbors[0]: [" << *neighborFaces[0] << nodeCode2 << "]" << *outerValues[0] << " " << minFunc(level,*neighborFaces[0],nodeCode2) << std::endl;
				std::cout << " neighbors[1]: [" << *neighborFaces[1] << nodeCode1 << "]" << *outerValues[1] << " " << minFunc(level,*neighborFaces[1],nodeCode1) << std::endl;
				std::cout << " neighbors[3]: [" << *neighborFaces[3] << nodeCode2 << "]" << *outerValues[2] << " " << minFunc(level,*neighborFaces[3],nodeCode2) << std::endl;
				std::cout << " neighbors[5]: [" << *neighborFaces[5] << nodeCode1Right << "]" << *outerValues[3] << " " << minFunc(level,*neighborFaces[5],nodeCode1Right) << std::endl;
	}

	//search for the overal minimum node and the smallest node connected to that
	unsigned short minIndex = minIndexLocalSearch(refMinValue,faceCount,outerValues);
	unsigned short min2ndIndex = min2ndIndexLocalSearch(minIndex,refMinValue,faceCount,outerValues);

	if (minIndex == 0 || min2ndIndex == 0) //minimum edge connects to center point
	{
		min2ndIndex += minIndex; // the sum determines the new minimum edge

		if (min2ndIndex == 1)
		{
			if (minIndex == 1)
			{
				refMinLocation = nodeCode2;
				refMinValue = *outerValues[0];
			} // if minIndex == 0 then minimum location and value are unchanged
			refEdgeOrientationMatch = edgeAOrientationMatch;
			refEdgeCode = nodeCode1;
			// refMinFace is unchanged
			refEdgeNeighborFace = leftNewFaceA;
		}
		else if (min2ndIndex == faceCount)
		{
			// assume minIndex != faceCount, because by assumption refMin2ndValue>refMinValue
			// hence, no changes here
			// refEdgeOrientationMatch is unchanged
			// refEdgeCode is unchanged
			// refMinFace is unchanged
			// refEdgeNeighborFace is unchanged
		}
		else if (min2ndIndex == faceCount/2) // the last face at the right side was not assigned before
		{
			if (minIndex == 0)
			{
				refMinLocation = nodeCode1;
				if (faceCount == 4) refMinLocation = nodeCode0;
				else if (!edgeBOrientationMatch) refMinLocation = nodeCode2;
				// refMinValue is unchanged
			}
			else
			{
				refMinLocation = nodeCode0;
				if (faceCount == 4) refMinLocation = nodeCode1;
				else if (!edgeAOrientationMatch) refMinLocation = nodeCode2;
				refMinValue = *outerValues[min2ndIndex-1];
			}
			// refEdgeOrientationMatch is unchanged
			refEdgeCode = nodeCode2;
			if (faceCount == 6)
			{
				if (!edgeAOrientationMatch) refEdgeCode = nodeCode0;
				else if (!edgeBOrientationMatch) refEdgeCode = nodeCode1;
			}
			refMinFace = *neighborFaces[min2ndIndex-1];
			refEdgeNeighborFace = refMinFace.neighborFace(level,refEdgeCode);
		}
		else if (min2ndIndex == faceCount/2 + 1) // the last face at the right side has not been assigned before
		{
			if (minIndex == 0)
			{
				refMinLocation = nodeCode0;
				if (faceCount == 6)
				{
					if (!edgeAOrientationMatch) refMinLocation = nodeCode2;
					else if (!refEdgeOrientationMatch) refMinLocation = nodeCode1;
				}
				// refMinValue is unchanged
			}
			else
			{
				refMinLocation = nodeCode2;
				if (faceCount == 6)
				{
					if (!edgeAOrientationMatch) refMinLocation = nodeCode0;
					else if (!refEdgeOrientationMatch) refMinLocation = nodeCode1;
				}
				refMinValue = *outerValues[min2ndIndex-1];
			}
			refEdgeOrientationMatch = edgeAOrientationMatch;
			refEdgeCode = nodeCode0Right;
			if (faceCount == 4) refEdgeCode = nodeCode1;
			else if (!edgeBOrientationMatch)  refEdgeCode = nodeCode2;
			refEdgeNeighborFace = *neighborFaces[min2ndIndex];
			refMinFace = refEdgeNeighborFace.neighborFace(level,refEdgeCode);
		}
		else if (min2ndIndex == 2)
		{
			if (minIndex == 0)
			{
				refMinLocation = nodeCode2;
				if (!edgeAOrientationMatch) refMinLocation = nodeCode0;
				// refMinValue is unchanged
			}
			else
			{
				refMinLocation = nodeCode1;
				refMinValue = leftNodeBFValue;
			}
			refEdgeCode = nodeCode0;
			if (!edgeAOrientationMatch) refEdgeCode = nodeCode2;
			refEdgeOrientationMatch = edgeBOrientationMatch;
			refMinFace = leftNewFaceA;
			refEdgeNeighborFace = *neighborFaces[3];
		}
		else // if (min2ndIndex == 5)
		{
			if (minIndex == 0)
			{
				refMinLocation = nodeCode2;
				if (!edgeBOrientationMatch) refEdgeCode = nodeCode0Right;
				// refMinValue is unchanged
			}
			else
			{
				refMinLocation = nodeCode0Right;
				if (!edgeBOrientationMatch) refMinLocation = nodeCode2;
				refMinValue = *outerValues[4]; //min2ndIndex-1 == 4
			}
			refEdgeCode = nodeCode1Right;
			refEdgeOrientationMatch = edgeBOrientationMatch;
			refMinFace = *neighborFaces[4];
			// refEdgeNeighborFace is unchanged
		}
	}
	else  //minimum edge is one of the outer edges
	{
		// assign the correct neighborFace code to min2ndIndex
		if (std::max(minIndex,min2ndIndex)-std::min(minIndex,min2ndIndex) > 1) min2ndIndex = 0;
		else min2ndIndex = std::min(minIndex,min2ndIndex);

		if (faceCount == 4)
		{
			refMinValue = *outerValues[minIndex-1];
			if (minIndex == 1 || minIndex == 3) refMinLocation = nodeCode2;
			else refMinLocation = nodeCode1;
			refEdgeCode = nodeCode0;
		}
		else
		{
			switch (min2ndIndex)
			{
			case 0:
			{
				refEdgeCode = nodeCode0;

				if (minIndex == 1)
				{
					refMinLocation = nodeCode2;
					refMinValue = *outerValues[0];
				}
				else
				{
					refMinLocation = nodeCode1;
					refMinValue = refMin2ndValue;
				}
				break;
			}
			case 1:
			{
				if (edgeAOrientationMatch)
				{
					refEdgeCode = nodeCode2;

					if (minIndex == 2)
					{
						refMinLocation = nodeCode1;
						refMinValue = *outerValues[1];
					}
					else
					{
						refMinLocation = nodeCode0;
						refMinValue = *outerValues[0];
					}
				}
				else
				{
					refEdgeCode = nodeCode0;

					if (minIndex == 2)
					{
						refMinLocation = nodeCode1;
						refMinValue = *outerValues[1];
					}
					else
					{
						refMinLocation = nodeCode2;
						refMinValue = *outerValues[0];
					}
				}
				break;
			}
			case 2:
			{
				if (edgeBOrientationMatch)
				{
					refEdgeCode = nodeCode1;

					if (minIndex == 3)
					{
						if (edgeAOrientationMatch) refMinLocation = nodeCode0;
						else refMinLocation = nodeCode2;
						refMinValue = *outerValues[2];
					}
					else
					{
						if (edgeAOrientationMatch) refMinLocation = nodeCode2;
						else refMinLocation = nodeCode0;
						refMinValue = *outerValues[1];
					}
				}
				else
				{
					refEdgeCode = nodeCode2;

					if (minIndex == 3)
					{
						refMinLocation = nodeCode0;
						refMinValue = *outerValues[2];
					}
					else
					{
						refMinLocation = nodeCode1;
						refMinValue = *outerValues[1];
					}
				}
				break;
			}
			case 3:
			{
				if (edgeAOrientationMatch)
				{
					refEdgeCode = nodeCode1Right;

					if (minIndex == 4)
					{
						if (edgeBOrientationMatch) refMinLocation = nodeCode0Right;
						else refMinLocation = nodeCode2;
						refMinValue = *outerValues[3];
					}
					else
					{
						if (edgeBOrientationMatch) refMinLocation = nodeCode0Right;
						else refMinLocation = nodeCode2;
						refMinValue = *outerValues[2];
					}
				}
				else
				{
					refEdgeCode = nodeCode2;

					if (minIndex == 4)
					{
						refMinLocation = nodeCode0;
						refMinValue = *outerValues[3];
					}
					else
					{
						refMinLocation = nodeCode1;
						refMinValue = *outerValues[2];
					}
				}
				break;
			}
			case 4:
			{
				if (edgeBOrientationMatch)
				{
					refEdgeCode = nodeCode2;

					if (minIndex == 5)
					{
						refMinLocation = nodeCode0Right;
						refMinValue = *outerValues[4];
					}
					else
					{
						refMinLocation = nodeCode1Right;
						refMinValue = *outerValues[3];
					}
				}
				else
				{
					refEdgeCode = nodeCode1;

					if (minIndex == 5)
					{
						refMinLocation = nodeCode2;
						refMinValue = *outerValues[4];
					}
					else
					{
						refMinLocation = nodeCode0;
						refMinValue = *outerValues[3];
					}
				}
				break;
			}
			case 5:
			{
				refEdgeCode = nodeCode0Right;

				if (minIndex == 6)
				{
					refMinLocation = nodeCode1Right;
					refMinValue = *outerValues[5];
				}
				else
				{
					refMinLocation = nodeCode2;
					refMinValue = *outerValues[4];
				}
				break;
			}
			}
		}
		refMinFace = *neighborFaces[min2ndIndex];
		refEdgeNeighborFace = refMinFace;
		assignNeighborFace(level,refEdgeCode,refEdgeNeighborFace,refEdgeOrientationMatch);
	}

		std::cout << "End: level: " << level;
		std::cout << " refMin: [" << refMinFace << refMinLocation << "]" << refMinValue;
		std::cout << " - " << refEdgeCode << "," << refEdgeOrientationMatch << " - ";
		std::cout << " neighbor: " << refEdgeNeighborFace << "]" << refMin2ndValue;
		std::cout << std::endl;

	if (faceCount == 6)
	{
		delete neighborFaces[2];
		delete neighborFaces[4];

		delete outerValues[2];
		delete outerValues[3];
	}

	return;
}


void SPIRID::sGrid::searchLocalMinNextLevel(
    size_t lowerLevel,
    sGrid& refMinFace, // the face where a local minimum of minFunc was found (at lowerLevel)
    unsigned short& refMinLocation, // the location inside face where the local minimum of minFunc was found (at lowerLevel)
    scaledFP& refMinValue, // the minimum function value of minFunc (at nodes in lowerLevel)
    unsigned short& refEdgeCode, // the edge inside the minimum face connecting points with smallest and 2nd smallest minFunc values
    sGrid& refEdgeNeighborFace, // neighbor face next to refMinPoint across refEdge
    bool& refEdgeOrientationMatch, // whether orientation of left and right face of refEdge is matched
    scaledFP (*minFunc)(size_t, const sGrid&, unsigned short)
)
{
		std::cout << "NextLevel search: ";
		std::cout << std::endl;

	size_t nextLevel = lowerLevel+1;
	refMinFace.resize(nextLevel);
	refEdgeNeighborFace.resize(nextLevel);

	// node codes of the various nodes
	unsigned short nodeCode0 = refMinLocation; // node code of the reference node
	unsigned short nodeCode2 = refEdgeCode; // node code of the 3rd node in the face, which is not on refEdge
	unsigned short nodeCode1 = newCode(nodeCode0,nodeCode2); // node code of the 2nd on refEdge

	unsigned short nodeCode0Right = nodeCode1; // node codes at the other side of refEdge
	unsigned short nodeCode1Right = nodeCode0;
	if (!refEdgeOrientationMatch) std::swap(nodeCode0Right,nodeCode1Right);

		std::cout << "lowerLevel: " << lowerLevel << std::endl;
		std::cout << " refMin: [" << refMinFace << refMinLocation << "]" << refMinValue;
		std::cout << ": (" << minFunc(lowerLevel,refMinFace,nodeCode0) << "," << minFunc(lowerLevel,refMinFace,nodeCode1) << "," << minFunc(lowerLevel,refMinFace,nodeCode2) << ") ";
		std::cout << " - " << refEdgeCode << "," << refEdgeOrientationMatch << " - " << refMinFace.neighborFace(lowerLevel,refEdgeCode) << std::endl;
		std::cout << " refNeighbor: " << refEdgeNeighborFace;
		std::cout << ": (" << minFunc(lowerLevel,refEdgeNeighborFace,nodeCode0Right) << "," << minFunc(lowerLevel,refEdgeNeighborFace,nodeCode1Right) << "," << minFunc(lowerLevel,refEdgeNeighborFace,nodeCode2) << ") ";
		std::cout << std::endl;


	// 1st step: calculate function value at the middle point of refEdge
	sGrid nextLevelFirstFace(refMinFace);
	nextLevelFirstFace.set(nextLevel,nodeCode0); //assume sufficient capacity for the gridCode
	scaledFP midPointFValue = minFunc(nextLevel, nextLevelFirstFace, nodeCode1);

	// use different ways to other neighbor faces depending function values
	if (refMinValue < midPointFValue)
	{
		/*  search new minimum around refMinPoint in the next grid level
		    use searchLocalMinAtLevel function
		*/
		refMinFace.set(nextLevel,nodeCode0);
		refEdgeNeighborFace.set(nextLevel,nodeCode0Right);
		searchLocalMinAtLevel(nextLevel,
		                      refMinFace, refMinLocation, refMinValue,
		                      refEdgeCode, refEdgeNeighborFace, refEdgeOrientationMatch,
		                      midPointFValue, minFunc);
	}
	else
	{
		/*  search minimum around the midpoint of refEdge
		    the midpoint has always six adjacent faces (since it exists only in nextLevel)
		    refEdge may or may not switch orientation, other edges must have orientation matched on both sides
		    function values at four new nodes need to be calculated:
		        5th node is the 2nd node of refEdge at lowerLevel
		        --> we know that minFunc value there is larger than at midpoint and at refMinPoint
		    we can construct all new faces by step-up from refMinPoint or refEdgeNeighborFace (no neighbor search needed)
		 */
		sGrid leftCenterFace(refMinFace);
		leftCenterFace.set(nextLevel,0); //assume sufficient capacity for the gridCode
		scaledFP leftCenterFaceFValueA( minFunc(nextLevel, leftCenterFace, nodeCode1) );
		scaledFP leftCenterFaceFValueB( minFunc(nextLevel, leftCenterFace, nodeCode0) );

		sGrid rightCenterFace(refEdgeNeighborFace);
		rightCenterFace.set(nextLevel,0); //assume sufficient capacity for the gridCode
		scaledFP rightCenterFaceFValueA( minFunc(nextLevel, rightCenterFace, nodeCode1Right) );
		scaledFP rightCenterFaceFValueB( minFunc(nextLevel, rightCenterFace, nodeCode0Right) );
/*
				std::cout << " lower level values: (" << refMinValue << "," << minFunc(lowerLevel,refMinFace,nodeCode1) << ")";
				std::cout << std::endl;
				std::cout << " leftCenterFace : " << leftCenterFace << " (" << nodeCode2 << "," << nodeCode0 << "," << nodeCode1 << ") (";
				std::cout << midPointFValue << "," << leftCenterFaceFValueA << "," << leftCenterFaceFValueB << ")";
				std::cout << std::endl;
				std::cout << " rightCenterFace: " << rightCenterFace << " ("<< nodeCode2 << "," << nodeCode0Right << "," << nodeCode1Right << ") (";
				std::cout << midPointFValue << "," << rightCenterFaceFValueA << "," << rightCenterFaceFValueB << ")";
				std::cout << std::endl;
*/

		//search node with minimal function value
		unsigned short minIndex = 0;
		scaledFP minValue = midPointFValue;
		if (leftCenterFaceFValueA < minValue)
		{
			minIndex = 1;
			minValue = leftCenterFaceFValueA;
		}
		if (leftCenterFaceFValueB < minValue)
		{
			minIndex = 2;
			minValue = leftCenterFaceFValueB;
		}
		if (rightCenterFaceFValueA < minValue)
		{
			minIndex = 5;
			minValue = rightCenterFaceFValueA;
		}
		if (rightCenterFaceFValueB < minValue)
		{
			minIndex = 4;
			minValue = rightCenterFaceFValueB;
		}

		if (minIndex == 0) // minimum in the middle of the reference edge
		{
			// search node with 2nd smallest function value
			minIndex = 6;
			minValue = refMinValue;
			if (leftCenterFaceFValueA < minValue)
			{
				minIndex = 1;
				minValue = leftCenterFaceFValueA;
			}
			if (leftCenterFaceFValueB < minValue)
			{
				minIndex = 2;
				minValue = leftCenterFaceFValueB;
			}
			if (rightCenterFaceFValueA < minValue)
			{
				minIndex = 5;
				minValue = rightCenterFaceFValueA;
			}
			if (rightCenterFaceFValueB < minValue)
			{
				minIndex = 4;
				minValue = rightCenterFaceFValueB;
			}
/*
			std::cout << "minIndex 2: " << minIndex;
			std::cout << std::endl;
*/
			// assign correct return values
			refMinValue = midPointFValue;
			if (minIndex == 6)
			{
				refMinFace = nextLevelFirstFace;
				refMinLocation = nodeCode1;
				// refMinValue already assigned
				// refEdgeCode unchanged
				refEdgeNeighborFace.set(nextLevel,nodeCode0Right);
				// refEdgeOrientationMatch unchanged
				return;
			}
			if (minIndex == 1)
			{
				refMinFace = nextLevelFirstFace;
				refMinLocation = nodeCode1;
				// refMinValue already assigned
				refEdgeCode = nodeCode0;
				refEdgeNeighborFace = leftCenterFace;
				refEdgeOrientationMatch = true;
				return;
			}
			if (minIndex == 2)
			{
				refEdgeNeighborFace = refMinFace.set(nextLevel,nodeCode1);
				refMinFace = leftCenterFace;
				refMinLocation = nodeCode2;
				// refMinValue already assigned
				refEdgeCode = nodeCode1;
				refEdgeOrientationMatch = true;
				return;
			}
			if (minIndex == 4)
			{
				refMinFace = rightCenterFace;
				refMinLocation = nodeCode2;
				// refMinValue already assigned
				refEdgeCode = nodeCode1Right;
				refEdgeNeighborFace.set(nextLevel,nodeCode1Right);
				refEdgeOrientationMatch = true;
				return;
			}
			if (minIndex == 5)
			{
				refMinFace = rightCenterFace;
				refMinLocation = nodeCode2;
				// refMinValue already assigned
				refEdgeCode = nodeCode0Right;
				refEdgeNeighborFace.set(nextLevel,nodeCode0Right);
				refEdgeOrientationMatch = true;
				return;
			}
		}
		else if (minIndex == 1) // minimum is nodeA in the left center face
		{
			if (refMinValue < midPointFValue)
			{
				if (leftCenterFaceFValueB < refMinValue)
				{
					refMinFace.set(nextLevel,nodeCode2);
                    refMinLocation = nodeCode0;
					// refMinValue will be assigned in the end
					refEdgeCode = nodeCode2;
					refEdgeOrientationMatch = true;
					refEdgeNeighborFace  = leftCenterFace;
				}
				else
				{
					refMinFace = nextLevelFirstFace;
					refMinLocation = nodeCode2;
					// refMinValue will be assigned in the end
					refEdgeCode = nodeCode1;
					refEdgeNeighborFace = nextLevelFirstFace.neighborFace(nextLevel,refEdgeCode,refEdgeOrientationMatch);
				}
			}
			else
			{
				if (leftCenterFaceFValueB < midPointFValue)
				{
					refMinFace.set(nextLevel,nodeCode2);
                    refMinLocation = nodeCode0;
					// refMinValue will be assigned in the end
					refEdgeCode = nodeCode2;
					refEdgeOrientationMatch = true;
					refEdgeNeighborFace  = leftCenterFace;
				}
				else
				{
					refMinFace = nextLevelFirstFace;
                    refMinLocation = nodeCode2;
					// refMinValue will be assigned in the end
					refEdgeCode = nodeCode0;
					refEdgeOrientationMatch = true;
					refEdgeNeighborFace = leftCenterFace;
				}
			}
			refMinValue = leftCenterFaceFValueA;
		}
		else if (minIndex == 2) // minimum is nodeB in the left center face
		{
			refMinFace = leftCenterFace;
			refMinLocation = nodeCode0;
			refMinValue = leftCenterFaceFValueB;
			refEdgeOrientationMatch = true;

			if (leftCenterFaceFValueA < midPointFValue)
			{
				// refMinFace, refMinLocation, refMinValue & refEdgeOrientationMatch already assigned
				refEdgeCode = nodeCode2;
				refEdgeNeighborFace = leftCenterFace.set(nextLevel,nodeCode2);
			}
			else
			{
				// refMinFace, refMinLocation, refMinValue & refEdgeOrientationMatch already assigned
				refEdgeCode = nodeCode1;
				refEdgeNeighborFace = leftCenterFace.set(nextLevel,nodeCode1);
			}
		}
		else if (minIndex == 4) // minimum is nodeA in the right center face
		{
			refMinFace = rightCenterFace;
			refMinLocation = nodeCode0Right;
			refMinValue = rightCenterFaceFValueB;
			refEdgeOrientationMatch = true;

			if (rightCenterFaceFValueA < midPointFValue)
			{
				// refMinFace, refMinLocation, refMinValue & refEdgeOrientationMatch already assigned
				refEdgeCode = nodeCode2;
				refEdgeNeighborFace.set(nextLevel,nodeCode2);
			}
			else
			{
				// refMinFace, refMinLocation, refMinValue & refEdgeOrientationMatch already assigned
				refEdgeCode = nodeCode1Right;
				refEdgeNeighborFace.set(nextLevel,nodeCode1Right);
			}

		}
		else if (minIndex == 5) // minimum is nodeB in the right center face
		{
			refMinFace = rightCenterFace;
			refMinLocation = nodeCode1Right;

			if (refMinValue < midPointFValue)
			{
				if (rightCenterFaceFValueB < refMinValue)
				{
                    refMinFace = rightCenterFace;
                    refMinLocation = nodeCode1Right;
					// refMinValue will be assigned in the end
					refEdgeCode = nodeCode2;
                    refEdgeOrientationMatch = true;
					refEdgeNeighborFace.set(nextLevel,nodeCode2);
				}
				else
				{
					refMinFace = refEdgeNeighborFace.set(nextLevel,nodeCode0Right);
					refMinLocation = nodeCode2;
					// refMinValue will be assigned in the end
					refEdgeCode = nodeCode1Right;
					refEdgeNeighborFace = refMinFace.neighborFace(nextLevel,refEdgeCode,refEdgeOrientationMatch);
				}
			}
			else
			{
				if (rightCenterFaceFValueB < midPointFValue)
				{
                    refMinFace = rightCenterFace;
                    refMinLocation = nodeCode1Right;
					// refMinValue will be assigned in the end
					refEdgeCode = nodeCode2;
                    refEdgeOrientationMatch = true;
					refEdgeNeighborFace.set(nextLevel,nodeCode2);
				}
				else
				{
                    refMinFace = rightCenterFace;
                    refMinLocation = nodeCode1Right;
					// refMinValue will be assigned in the end
					refEdgeCode = nodeCode0Right;
                    refEdgeOrientationMatch = true;
					refEdgeNeighborFace.set(nextLevel,nodeCode0Right);
				}
			}
			refMinValue = rightCenterFaceFValueA;
		}
	}
	return;
}



const unsigned short SPIRID::sGrid::newNodeCodes[4] = {4,2,3,1};
const unsigned short SPIRID::sGrid::newFaceCodes[4] = {1,2,3,0};
const unsigned short SPIRID::sGrid::newCodes[7] = {4,4,4,3,2,1,0};



//output a complete face code
std::ostream& SPIRID::operator << (std::ostream& out, const sGrid& P)
{
	out << "(F";
	for (size_t it=0; it<=P.depth(); it++) out << P.at(it);
	out << ")";
	return out;
}
//output data of a common node
std::ostream& SPIRID::sGrid::commonNode::print(std::ostream& out)
{
	out << "{L" << level1;
	out << ",N" << nodeCode1;
	out << ",E" << edgeCode1;
	out << "; L" << level2;
	out << ",N" << nodeCode2;
	out << ",E" << edgeCode2;
	out << "; " << gapAngle;
	out << "," << signAngle2;
	out << "}";

	return out;
}
