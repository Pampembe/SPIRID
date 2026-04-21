#include <cmath>
#include <SPIRID.h>

//sGrid constructor: convert polar coordinates to sGrid coordinates
SPIRID::sGrid::sGrid(
    const sPolar& P,
    size_t level,
    sGrid (*minSearchFunction)(polarDistanceToRef, size_t)
) : gridCode()
{
	polarDistanceToRef polarDistanceToP(P);
	*this = minSearchFunction(polarDistanceToP, level);
}

//sGrid constructor from a vector of face codes
SPIRID::sGrid::sGrid(
    const std::vector<unsigned short>& faceCodes
) : gridCode(std::max(size_t(3),size_t(1+2*faceCodes.size())))
{
	for (size_t it = 0; it < faceCodes.size(); ++it)
	{
		set(it, faceCodes[it]);
	}
}
//sGrid constructor from bit array
SPIRID::sGrid::sGrid(
    const std::vector<bool>& bitCode
) : gridCode(bitCode)
{
	size_t size = gridCode.size();
	if (size<3) gridCode.resize(3);
	if (size%2==0) gridCode.resize(size+1);
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
SPIRID::sGrid::set(size_t level, unsigned short faceCode)
{
	if (level==0)
	{
		switch (faceCode)
		{
		case 0 :
		{
			gridCode[0]=false;
			gridCode[1]=false;
			gridCode[2]=false;
			break;
		}
		case 1 :
		{
			gridCode[0]=true;
			gridCode[1]=false;
			gridCode[2]=false;
			break;
		}
		case 2 :
		{
			gridCode[0]=false;
			gridCode[1]=true;
			gridCode[2]=false;
			break;
		}
		case 3 :
		{
			gridCode[0]=true;
			gridCode[1]=true;
			gridCode[2]=false;
			break;
		}
		case 4 :
		{
			gridCode[0]=false;
			gridCode[1]=false;
			gridCode[2]=true;
			break;
		}
		case 5 :
		{
			gridCode[0]=true;
			gridCode[1]=false;
			gridCode[2]=true;
			break;
		}
		case 6 :
		{
			gridCode[0]=false;
			gridCode[1]=true;
			gridCode[2]=true;
			break;
		}
		case 7 :
		{
			gridCode[0]=true;
			gridCode[1]=true;
			gridCode[2]=true;
			break;
		}
		default:
		{
			gridCode[0]=true;
			gridCode[1]=true;
			gridCode[2]=true;
			break;
		}
		}
	}
	else
	{
		switch (faceCode)
		{
		case 0 :
		{
			gridCode[2*level+1]=false;
			gridCode[2*level+2]=false;
			break;
		}
		case 1 :
		{
			gridCode[2*level+1]=true;
			gridCode[2*level+2]=false;
			break;
		}
		case 2 :
		{
			gridCode[2*level+1]=false;
			gridCode[2*level+2]=true;
			break;
		}
		case 3 :
		{
			gridCode[2*level+1]=true;
			gridCode[2*level+2]=true;
			break;
		}
		default:
		{
			gridCode[2*level+1]=false;
			gridCode[2*level+2]=false;
			break;
		}
		}
	}
	return *this;
}
//set all grid codes in higher levels to zero (starting at level)
const SPIRID::sGrid&
SPIRID::sGrid::reset(size_t level)
{
	size_t start = 2*level+1;
	if (level == 0) start = 0;
	std::fill(gridCode.begin()+start, gridCode.end(), false);
	return *this;
}
//cut tailing zero grid codes
void
SPIRID::sGrid::trunc()
{
	size_t it = depth();
	while (it > 0 && operator [](it) == 0) {
		--it;
	}
	resize(it);
	return;
}



//special points
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::NorthOct0 = {SPIRID::sGrid(std::vector<bool>({0,0,0})),3};
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::EastOct0  = {SPIRID::sGrid(std::vector<bool>({0,0,0})),1};
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::SouthOct7 = {SPIRID::sGrid(std::vector<bool>({1,1,1})),3};
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::WestOct7  = {SPIRID::sGrid(std::vector<bool>({1,1,1})),1};





//collect neighbor faces, edge & nodes
SPIRID::sGrid
SPIRID::sGrid::neighborFace(size_t level, unsigned short edgeCode) const
{
	bool orientationMatch = true;
	return neighborFace(level, edgeCode, orientationMatch);
}
SPIRID::sGrid
SPIRID::sGrid::neighborFace(size_t level, unsigned short edgeCode, bool& orientationMatch) const
{
	sGrid neighbor(*this);
	neighbor.resize(level);
	orientationMatch = true;
	return assignNeighborFace(level, edgeCode, neighbor, orientationMatch);
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


//auxiliary constants to get new codes for faces, edges & nodes
const unsigned short SPIRID::sGrid::newNodeCodes[4] = {4,2,3,1};
const unsigned short SPIRID::sGrid::newFaceCodes[4] = {1,2,3,0};
const unsigned short SPIRID::sGrid::newCodes[7] = {4,4,4,3,2,1,0};





//output operations for sGrid and pointPairRefNode
void SPIRID::sGrid::print(std::ostream& out) const
{
	out << "(F";
	for (size_t it=0; it<=depth(); it++) out << at(it);
	out << ")";
}
std::ostream& SPIRID::operator << (std::ostream& out, const sGrid& P)
{
	P.print(out);
	return out;
}
//output data of a pointPairRefNode
std::ostream& SPIRID::sGrid::pointPairRefNode::print(std::ostream& out)
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
