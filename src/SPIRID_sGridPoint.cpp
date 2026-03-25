#include <SPIRID_sGridPoint.h>

#include <stack>

const SPIRID::sGridPoint SPIRID::sGridPoint::North = sGridPoint({0,0,0},3);
const SPIRID::sGridPoint SPIRID::sGridPoint::South = sGridPoint({1,1,1},3);

SPIRID::sGridPoint::sGridPoint(const std::vector<bool>& P, unsigned short location) : sGridPoint(P)
{
	setLocation(location);
}

SPIRID::sGridPoint
SPIRID::sGridPoint::searchPoint(scaledFP (*minFunc)(const sGridPoint&))
{
	return sGridPoint(North);
}
SPIRID::sGridPoint
SPIRID::sGridPoint::localSearchPoint(size_t level, const sGridPoint& P0, scaledFP minFunctionValue, scaledFP (*minFunc)(const sGridPoint&))
{
	unsigned short nodeCode0 = P0.location();
	unsigned short nodeCode1 = nextNode(nodeCode0);
	unsigned short nodeCode2 = newCode(nodeCode0,nodeCode1);

	//collect list of nodes to be checked
	std::stack<sGridPoint> neighborPoints;
	neighborPoints.push(sGridPoint(P0).setLocation(nodeCode1));
	neighborPoints.push(sGridPoint(P0).setLocation(nodeCode2));

	bool orientationMatch = true;

	neighborPoints.push(neighborPoints.top());
	neighborPoints.top().assignNeighborFace(level, nodeCode1, neighborPoints.top(), orientationMatch).setLocation(nodeCode1);

	neighborPoints.push(neighborPoints.top());
	// which next neighbor to be added depends on whether orientation is matched
	if (orientationMatch)
	{
		neighborPoints.top().assignNeighborFace(level, nodeCode0, neighborPoints.top(), orientationMatch).setLocation(nodeCode0);

		neighborPoints.push(neighborPoints.top());
		if (orientationMatch)
		{
			neighborPoints.top().assignNeighborFace(level, nodeCode2, neighborPoints.top(), orientationMatch).setLocation(nodeCode2);

			neighborPoints.push(neighborPoints.top());
			if (orientationMatch)
			{
				neighborPoints.top().assignNeighborFace(level, nodeCode1, neighborPoints.top(), orientationMatch).setLocation(nodeCode1);
			}
			else
			{
				neighborPoints.top().assignNeighborFace(level, nodeCode0, neighborPoints.top(), orientationMatch).setLocation(nodeCode0);
			}
		}
		else
		{
			neighborPoints.top().assignNeighborFace(level, nodeCode1, neighborPoints.top(), orientationMatch).setLocation(nodeCode1);

            //by construction of the grid, orientation in the previous step must be matched
			neighborPoints.push(neighborPoints.top());
			neighborPoints.top().assignNeighborFace(level, nodeCode2, neighborPoints.top(), orientationMatch).setLocation(nodeCode2);
		}
	}
	else
	{
		neighborPoints.top().assignNeighborFace(level, nodeCode2, neighborPoints.top(), orientationMatch).setLocation(nodeCode2);

		if (orientationMatch)
		{
			neighborPoints.push(neighborPoints.top());
			neighborPoints.top().assignNeighborFace(level, nodeCode0, neighborPoints.top(), orientationMatch).setLocation(nodeCode0);

            //by construction of the grid, orientation in the previous step must be matched
			neighborPoints.push(neighborPoints.top());
			neighborPoints.top().assignNeighborFace(level, nodeCode2, neighborPoints.top(), orientationMatch).setLocation(nodeCode1);
		}
		//if orientationMatch is two times false then by construction of the grid there are only four adjacent nodes
	}

    scaledFP functionResult(0,0);
    sGridPoint P(P0);
    while (!neighborPoints.empty())
    {
        functionResult = minFunc(neighborPoints.top());
        std::cout << neighborPoints.top() << neighborPoints.top().location() << " functionResult: " << functionResult << std::endl;
        if (functionResult < minFunctionValue)
        {
            P.set(neighborPoints.top());
            minFunctionValue = functionResult;
        }
        neighborPoints.pop();
    }

	return P;
}

/*
std::ostream& operator << (std::ostream& out, const SPIRID::sGridPoint& P)
{
    out << (SPIRID::sGrid)P << "[" << P.location() << "]";
    return out;
}
*/