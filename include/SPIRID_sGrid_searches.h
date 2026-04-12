#ifndef SPIRID_SGRID_SEARCHES_H_INCLUDED
#define SPIRID_SGRID_SEARCHES_H_INCLUDED




/*
public:
template <typename gridFunction>
static sGrid
searchMinPoint(
    gridFunction minFunc,
    size_t maxLevel)
{
    sGrid resultFace;
    unsigned short resultLocation;
    scaledFP resultValue;

    searchMinPoint(minFunc, maxLevel, resultFace, resultLocation,resultValue);

    return resultFace;
}
*/
/*
public:
template <typename gridFunction>
static void
searchMinPoint(
    gridFunction minFunc,
    size_t maxLevel,
    sGrid& resultFace,
    unsigned short& resultLocation,
    scaledFP& resultValue)
{
    std::cout << std::endl;
    std::cout << "level0: [";
    std::cout << NorthOct0.first;
    std::cout << "" << NorthOct0.second;
    std::cout << "]" << minFunc(0,NorthOct0.first,NorthOct0.second);
    std::cout << " ";
    std::cout << "[";
    std::cout << WestOct0.first;
    std::cout << "" << WestOct0.second;
    std::cout << "]" << minFunc(0,WestOct0.first,WestOct0.second);
    std::cout << " ";
    std::cout << "[";
    std::cout << EastOct7.first;
    std::cout << "" << EastOct7.second;
    std::cout << "]" << minFunc(0,EastOct7.first,EastOct7.second);
    std::cout << " ";
    std::cout << "[";
    std::cout << SouthOct7.first;
    std::cout << "" << SouthOct7.second;
    std::cout << "]" << minFunc(0,SouthOct7.first,SouthOct7.second);
    std::cout << " ";
    std::cout << "[";
    std::cout << NorthOct0.first;
    std::cout << "" << 1;
    std::cout << "]" << minFunc(0,NorthOct0.first,1);
    std::cout << " ";
    std::cout << "[";
    std::cout << SouthOct7.first;
    std::cout << "" << 1;
    std::cout << "]" << minFunc(0,SouthOct7.first,1);
    std::cout << " ";
    std::cout << std::endl;

    sGrid refPoint(NorthOct0.first);

    sGrid refMinFace(NorthOct0.first);
    unsigned short refMinLocation = NorthOct0.second;
    unsigned short refEdgeCode = 1;
    bool refEdgeOrientationMatch = false;

    scaledFP refMinValue = minFunc(0, refMinFace, refMinLocation);
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
    std::cout << " :: minimum point value: " << minFunc(0,refMinFace, refMinLocation);
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
    std::cout << "refNode neighbor nodes: ";

    std::list<std::pair<sGrid, unsigned short> > neighborNodes(refMinFace.nodeNeighborNodes(0, refMinLocation));
    for (std::list<std::pair<sGrid, unsigned short> >::iterator it = neighborNodes.begin(); it != neighborNodes.end(); ++it)
    {
        std::cout << "[" << it->first << it->second << "]" << minFunc(0,it->first, it->second) << "; ";
    }
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

        	    std::cout << *tmp << minFunc(1,*tmp,1) << " ";
        	    std::cout << *tmp << minFunc(1,*tmp,2) << " ";
        	    std::cout << *tmp << minFunc(1,*tmp,3);
            	std::cout << std::endl;

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
        std::cout << "refNode neighbor nodes: ";

        neighborNodes = refMinFace.nodeNeighborNodes(level_it+1, refMinLocation);
        for (std::list<std::pair<sGrid, unsigned short> >::iterator it = neighborNodes.begin(); it != neighborNodes.end(); ++it)
        {
            std::cout << "[" << it->first << it->second << "]" << minFunc(level_it+1,it->first, it->second) << "; ";
        }
        std::cout << std::endl;

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
        minimum = refMinValue+scaledFP(1.,0);
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
    resultFace = refMinFace;
    resultLocation = refMinLocation;
    resultValue = refMinValue;
    return;
//	return {{refMinFace,refMinLocation},refMinValue};


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


}
*/


/*
static unsigned short
minIndexLocalSearch(
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
*/
/*
static unsigned short
min2ndIndexLocalSearch(
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
*/
/*
template <typename gridFunction>
static void
searchLocalMinAtLevel(
    size_t level,
    sGrid& refMinFace, // we search for a local minimum around a reference node, this is the corresponding grid face
    unsigned short& refMinLocation, // the location of the reference node inside refMinFace
    scaledFP& refMinValue, // the function value of minFunc at the reference node
    unsigned short& refEdgeCode, // a reference edge inside refMinFace that connects to the reference node
    sGrid& refEdgeNeighborFace, // neighbor face next to refMinFace across refEdge
    bool& refEdgeOrientationMatch, // whether orientation of left and right face of refEdge is matched
    const scaledFP& refMin2ndValue, // the function value at the 2nd node of refEdge
    gridFunction minFunc // function to minimize
)
{

    std::cout << "AtLevel search: ";
    std::cout << std::endl;

        std::cout << std::endl;
    	std::cout << "AtLevel search: ";
    	std::cout << std::endl;

    	std::cout << "Start: level: " << level;
    	std::cout << " refMin: [" << refMinFace << refMinLocation << "]" << refMinValue;
    	std::cout << " - " << refEdgeCode << "," << refEdgeOrientationMatch << " - ";
    	std::cout << " neighbor: " << refEdgeNeighborFace << "]" << refMin2ndValue;
    	std::cout << std::endl;

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
    //std::cout << refMinFace << leftNewFaceA << refEdgeNeighborFace << std::endl;

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

    std::cout << "refMinValue: " << refMinValue;
    std::cout << " outerValues: {" << *outerValues[0] << "," << *outerValues[1] << "," << *outerValues[2] << "," << *outerValues[3] << "," << *outerValues[4] << "," << *outerValues[5] << "}";
    std::cout << " minIndex: " << minIndex;
    std::cout << " min2ndIndex: " << min2ndIndex;
    std::cout << std::endl;

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
            refEdgeNeighborFace = *neighborFaces[2];
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
*/
template <typename gridFunction>
static void
searchLocalMinNextLevel(
    size_t lowerLevel,
    sGrid& refMinFace, // the face where a local minimum of minFunc was found (at lowerLevel)
    unsigned short& refMinLocation, // the location inside face where the local minimum of minFunc was found (at lowerLevel)
    scaledFP& refMinValue, // the minimum function value of minFunc (at nodes in lowerLevel)
    unsigned short& refEdgeCode, // the edge inside the minimum face connecting points with smallest and 2nd smallest minFunc values
    sGrid& refEdgeNeighborFace, // neighbor face next to refMinPoint across refEdge
    bool& refEdgeOrientationMatch, // whether orientation of left and right face of refEdge is matched
    gridFunction minFunc // function to minimize
)
{
    /*
    std::cout << "NextLevel search: ";
    std::cout << std::endl;

    */
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
    /*
    	std::cout << "lowerLevel: " << lowerLevel << std::endl;
    	std::cout << " refMin: [" << refMinFace << refMinLocation << "]" << refMinValue;
    	std::cout << ": (" << minFunc(lowerLevel,refMinFace,nodeCode0) << "," << minFunc(lowerLevel,refMinFace,nodeCode1) << "," << minFunc(lowerLevel,refMinFace,nodeCode2) << ") ";
    	std::cout << " - " << refEdgeCode << "," << refEdgeOrientationMatch << " - " << refMinFace.neighborFace(lowerLevel,refEdgeCode) << std::endl;
    	std::cout << " refNeighbor: " << refEdgeNeighborFace;
    	std::cout << ": (" << minFunc(lowerLevel,refEdgeNeighborFace,nodeCode0Right) << "," << minFunc(lowerLevel,refEdgeNeighborFace,nodeCode1Right) << "," << minFunc(lowerLevel,refEdgeNeighborFace,nodeCode2) << ") ";
    	std::cout << std::endl;
    */

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











// minimum edge search auxiliary function:
// given an outer edge turned out to be the minimum edge: check again the new function value inside the outer neighborFace
template<typename gridFunction>
static void fastMinEdgeSearchAux_OuterEdgeCase(
    gridFunction minFunc, // function to minimize
    size_t level, // the grid level of the minimum search
    sGrid& minFace, // face including the minimum edge (assuming that edge is inside an outer ring around a certain center node)
    unsigned short& minNode, // minimum node inside minFace on input --> becomes true minimum in the end
    scaledFP& minValue, // input: minimum function value inside minFace, output: actual minimum at this level
    unsigned short& otherNodeCode, // the nodeCode within minFace of the other (non-minimum) end node of the relevant edge
    scaledFP& otherNodeValue, // the function value at the other (non-minimum) end node of the relevant edge
    unsigned short& neighborEdgeCode, // the edge code of the relevant edge
    sGrid& minFaceNeighbor, // neighbor face next to minFace across the relevant edge
    bool& neighborOrientationMatch // whether orientation of minFace and minFaceNeighbor is matched
)
{
    scaledFP newValue = minFunc(level, minFaceNeighbor, neighborEdgeCode);

    if (newValue < otherNodeValue) //another edge inside minFaceNeighbor becomes the new minimum edge
    {
        //assign correct node codes inside minFaceNeighbor
        if (neighborOrientationMatch) std::swap(minNode, otherNodeCode); //switch node codes from minFace to minFaceNeighbor
        std::swap(neighborEdgeCode, otherNodeCode); //switch previous to new minimum edge

        minFace = minFaceNeighbor;
        minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);

        if (newValue < minValue)
        {
            std::swap(minNode, otherNodeCode);

            otherNodeValue = minValue;
            minValue = newValue;
        }
        else
        {
            otherNodeValue = newValue;
        }
    }
    // nothing to be done for newValue >= otherNodeValue
    return;
}
#endif // SPIRID_SGRID_SEARCHES_H_INCLUDED
