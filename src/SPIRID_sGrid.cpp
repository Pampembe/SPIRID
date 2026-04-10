#include <cmath>
#include <SPIRID.h>

//sGrid constructor from bit array
SPIRID::sGrid::sGrid(const std::vector<bool>& bitCode) : gridCode(bitCode)
{
    size_t size = gridCode.size();
    if (size<3) gridCode.resize(3);
    if (size%2==0) gridCode.resize(size+1);
}
//sGrid constructor from a vector of face codes
SPIRID::sGrid::sGrid(const std::vector<unsigned short>& faceCodes) : gridCode(1+2*faceCodes.size())
{
    for (size_t it = 0; it <= faceCodes.size(); ++it)
    {
        set(it, faceCodes[it]);
    }
}

//sGrid constructor: convert polar coordinates to sGrid coordinates
SPIRID::sGrid::sGrid(
    const sPolar& P,
    size_t level,
    sGrid (*minSearchFunction)(polarDistanceToRef, size_t)
) : gridCode()
{
    polarDistanceToRef polarDistanceToP(P);
//	*this = minSearch<polarDistanceToRef>(polarDistanceToP, level);
//	*this = searchMinPoint<polarDistanceToRef>(polarDistanceToP, level);
    *this = minSearchFunction(polarDistanceToP, level);
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

const SPIRID::sGrid&
SPIRID::sGrid::reset(size_t level)
{
    size_t start = 2*level+1;
    if (level == 0) start = 0;
    std::fill(gridCode.begin()+start, gridCode.end(), false);
    return *this;
}



//special points
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::NorthOct0 = {SPIRID::sGrid(std::vector<bool>({0,0,0})),3};
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::WestOct0  = {SPIRID::sGrid(std::vector<bool>({0,0,0})),2};
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::SouthOct7 = {SPIRID::sGrid(std::vector<bool>({1,1,1})),3};
const std::pair<SPIRID::sGrid,unsigned short> SPIRID::sGrid::EastOct7  = {SPIRID::sGrid(std::vector<bool>({1,1,1})),2};





//auxiliary constants to get new codes for faces, edges & nodes
const unsigned short SPIRID::sGrid::newNodeCodes[4] = {4,2,3,1};
const unsigned short SPIRID::sGrid::newFaceCodes[4] = {1,2,3,0};
const unsigned short SPIRID::sGrid::newCodes[7] = {4,4,4,3,2,1,0};

//collect neighbor faces, edge & nodes
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
    //neighborNodes.push_back({*this, nodeCode});
    //neighborNodes.back().first.resize(level);

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















//output a complete face code
std::ostream& SPIRID::operator << (std::ostream& out, const sGrid& P)
{
    out << "(F";
    for (size_t it=0; it<=P.depth(); it++) out << P.at(it);
    out << ")";
    return out;
}
//output data of a common node
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
