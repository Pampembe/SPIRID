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
// minimum edge search auxiliary function:
// return value: whether the new minimum edge is an outer edge
// starting from a face, minFace, where one node is a certain center node (minNode),
// and the 2nd node has the smallest function value inside the outer ring (at level) of this center node:
// re-arrange correct minimum nodes, edges and faces such that on output minNode is the final (local) minimum
// assumption: 3rd function value inside minFace < 3rd function value inside minFaceNeighbor
static bool fastMinEdgeSearchAux_LeftOuterNodeCheck(
    // gridFunction minFunc not needed: no function evaluation
    size_t level, // the grid level of the minimum search
    sGrid& minFace, // face including the center node and the outer ring minimum node --> becomes true minimum face in the end
    unsigned short& centerNode, // center node inside minFace on input --> becomes true minimum in the end
    scaledFP& centerValue, // input: function value at the center node, output: actual minimum at this level
    unsigned short& outerMinNode, // input: the nodeCode of the outer ring minimum within minFace, output the other nodeCode of the minimum edge
    scaledFP& outerNodeMinValue, // input: outer ring minimum value, output: function value at the non-minimum end node of the minimum edge
    unsigned short& leftOuterNode, // input: edge code connecting the center node to the outer ring minimum node --> becomes the minimum edge on output
    const scaledFP& leftOuterValue, // function value at the 3rd node of minFace
    sGrid& minFaceNeighbor, // neighbor face next to minFace across the relevant edge
    bool& neighborOrientationMatch // whether orientation of minFace and minFaceNeighbor is matched
)
{
    // outer ring minimum is outerNodeMinValue --> if minValue is still smaller, then nothing needs to change
    bool minEdgeIsOuterEdge = false;

    if (outerNodeMinValue < centerValue) // outer ring value is the minimum
    {
        unsigned short minNode = outerMinNode;

        if (leftOuterValue < centerValue) // outer edge is the actual the minimum edge
        {
            minEdgeIsOuterEdge = true;

            //for output: centerNode -> minNode, outerMinNode -> leftOuterNode, neighborEdgeCode(==leftOuterNode) --> centerNode
            outerMinNode = leftOuterNode;
            leftOuterNode = centerNode;
            centerNode = minNode;

            outerNodeMinValue = leftOuterValue;
            centerValue = outerNodeMinValue;

            minFaceNeighbor = minFace.neighborFace(level, leftOuterNode, neighborOrientationMatch);
        }
        else // minimum edge still connects to the center node, but centerValue the larger one
        {
            // no change to minFace, leftOuterNode==neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch
            // only exchange center node code and value with outer minimum node code and value
            outerMinNode = centerNode;
            centerNode = minNode;
            std::swap(centerValue, outerNodeMinValue);
        }
    }
    return minEdgeIsOuterEdge;
}
// minimum edge search auxiliary function:
// search for function minimum within nodes around edge mid point
template<typename gridFunction>
static void fastMinEdgeSearchAux_MidEdgeSmallerCase(
    gridFunction minFunc, // function to minimize
    size_t level, // the grid level to search for the minimum
    sGrid& minFace, // face including the minimum node; input: lower grid level, output: next grid level
    unsigned short& minNode, // node code at the minimum node inside minFace, aka nodeCode1; input: lower grid level, output: next grid level
    scaledFP& minValue, // minimum function value; input: lower grid level, output: next grid level
    unsigned short& otherNodeCode, // input: other node in level-1, output: at new level, aka nodeCode0
    scaledFP& otherNodeValue, // function value at this otherNode
    const scaledFP& centerNodeValue, // function value at the edge mid point, becomes other node value of the minimum edge at output
    unsigned short& neighborEdgeCode, // the edge between minFace and minFaceNeighbor (minimum node is one of the edge end points)
    sGrid& minFaceNeighbor, // neighbor face next to minFace; input: lower grid level, output: new grid level
    bool& neighborOrientationMatch, // whether orientation of minFace and minFaceNeighbor is matched
    const unsigned short& minNodeRight, // node code inside level-1 minFaceNeighbor of the minimum node of neighborEdge, aka nodeCode1Right
    const unsigned short& otherNodeRight // node code inside level-1 minFaceNeighbor of the other node of neighborEdge, aka nodeCode0Right
)
{
    //minFace was already moved to the next level to calculate the edge mid point function value
    minFace.set(level,0);
    minFaceNeighbor.setExtend(level,0);

    unsigned short nodeCode0 = otherNodeCode;
    unsigned short nodeCode1 = minNode;
    unsigned short nodeCode2 = neighborEdgeCode;

    scaledFP leftCenterFaceFValueA = minFunc(level,minFace,nodeCode0);
    scaledFP leftCenterFaceFValueB = minFunc(level,minFace,nodeCode1);
    scaledFP rightCenterFaceFValueA = minFunc(level,minFaceNeighbor,otherNodeRight);
    scaledFP rightCenterFaceFValueB = minFunc(level,minFaceNeighbor,minNodeRight);

    // outer ring minimum search:
    // edges connecting to the other node of the level-1 minimum edge cannot become the new minmum edge
    unsigned short outerMinIndex = 6; // this is the case where the level-1 minimum node is the outer ring minimum
    scaledFP outerRingMinValue = minValue;
    if (leftCenterFaceFValueA < outerRingMinValue)
    {
        outerMinIndex = 1;
        outerRingMinValue = leftCenterFaceFValueA;
    }
    if (leftCenterFaceFValueB < outerRingMinValue)
    {
        outerMinIndex = 2;
        outerRingMinValue = leftCenterFaceFValueB;
    }
    if (rightCenterFaceFValueA < outerRingMinValue)
    {
        outerMinIndex = 5;
        outerRingMinValue = rightCenterFaceFValueA;
    }
    if (rightCenterFaceFValueB < outerRingMinValue)
    {
        outerMinIndex = 4;
        outerRingMinValue = rightCenterFaceFValueB;
    }
/*
    std::cout << "fastMinEdgeSearchAux_MidEdgeSmallerCase: ";
    std::cout << " level: " << level;
    std::cout << " minFace: " << minFace;
    std::cout << " minNode: " << minNode;
    std::cout << " minValue: " << minValue;
    std::cout << " neighborEdgeCode: " << neighborEdgeCode;
    std::cout << " otherNodeCode: " << otherNodeCode;
    std::cout << " otherNodeValue: " << otherNodeValue;
    std::cout << " centerNodeValue: " << centerNodeValue;
    std::cout << " neighborOrientationMatch: " << neighborOrientationMatch;
    std::cout << " minFaceNeighbor: " << minFaceNeighbor;
    std::cout << " leftCenterFaceFValueA: " << leftCenterFaceFValueA;
    std::cout << " leftCenterFaceFValueB: " << leftCenterFaceFValueB;
    std::cout << " rightCenterFaceFValueA: " << rightCenterFaceFValueA;
    std::cout << " rightCenterFaceFValueB: " << rightCenterFaceFValueB;
    std::cout << " outerRingMinValue: " << outerRingMinValue;
    std::cout << " outerMinIndex: " << outerMinIndex;
    std::cout << std::endl;
*/
    switch (outerMinIndex)
    {
    case 1:
    {
        if (leftCenterFaceFValueB < minValue)
        {
            // minFace.set(level, 0) already done
            minFaceNeighbor = minFace;
            minFaceNeighbor.set(level,nodeCode1);
            neighborOrientationMatch = true;

            // otherNodeCode = nodeCode0; -> no change
            minNode = nodeCode2;
            neighborEdgeCode = nodeCode1;

            // outerRingMinValue may be smaller than centerValue, this will be checked by fastMinEdgeSearchAux_LeftOuterNodeCheck
            minValue = centerNodeValue;
            otherNodeValue = outerRingMinValue;

            // now check whether center node or outer ring minimum is smaller
            fastMinEdgeSearchAux_LeftOuterNodeCheck(
                level,
                minFace, minNode, minValue,
                otherNodeCode, otherNodeValue,
                neighborEdgeCode, leftCenterFaceFValueB,
                minFaceNeighbor, neighborOrientationMatch);
        }
        else
        {
            minFace.set(level, nodeCode1);
            minFaceNeighbor = minFace;
            minFaceNeighbor.set(level,0);
            neighborOrientationMatch = true;

            neighborEdgeCode = nodeCode1;

            if (outerRingMinValue < centerNodeValue)
            {
                // otherNodeCode = nodeCode0; -> no change
                minNode = nodeCode2;
                minValue = outerRingMinValue;
                otherNodeValue = centerNodeValue;
                // centerNodeValue -> otherNodeValue unchanged
            }
            else
            {
                otherNodeCode = nodeCode2;
                minNode = nodeCode0;
                minValue = centerNodeValue;
                otherNodeValue = outerRingMinValue;
            }
        }
        break;
    }
    case 2:
    {
        if (leftCenterFaceFValueA < otherNodeValue)
        {
            // minFace.set(level, 0) already done
            minFaceNeighbor = minFace;
            minFaceNeighbor.set(level,nodeCode0);
            neighborOrientationMatch = true;

            otherNodeCode = nodeCode1;
            minNode = nodeCode2;
            neighborEdgeCode = nodeCode0;

            // outerRingMinValue may be smaller than centerValue, this will be checked by fastMinEdgeSearchAux_LeftOuterNodeCheck
            minValue = centerNodeValue;
            otherNodeValue = outerRingMinValue;

            // now check whether center node or outer ring minimum is smaller
            fastMinEdgeSearchAux_LeftOuterNodeCheck(
                level,
                minFace, minNode, minValue,
                otherNodeCode, otherNodeValue,
                neighborEdgeCode, leftCenterFaceFValueA,
                minFaceNeighbor, neighborOrientationMatch);
        }
        else
        {
            minFace.set(level, nodeCode0);
            minFaceNeighbor = minFace;
            minFaceNeighbor.set(level,0);
            neighborOrientationMatch = true;

            neighborEdgeCode = nodeCode0;

            if (outerRingMinValue < centerNodeValue)
            {
                otherNodeCode = nodeCode1;
                minNode = nodeCode2;
                minValue = outerRingMinValue;
                otherNodeValue = centerNodeValue;
            }
            else
            {
                otherNodeCode = nodeCode2;
                // minNode = nodeCode1; -> no change
                minValue = centerNodeValue;
                otherNodeValue = outerRingMinValue;
            }
        }
        break;
    }
    case 4:
    {
        if (rightCenterFaceFValueA < otherNodeValue)
        {
            minFace = minFaceNeighbor;
            // minFace.set(level, 0) already done
            minFaceNeighbor.set(level,otherNodeRight);
            neighborOrientationMatch = true;

            otherNodeCode = minNodeRight;
            minNode = nodeCode2;
            neighborEdgeCode = otherNodeRight;

            // outerRingMinValue may be smaller than centerValue, this will be checked by fastMinEdgeSearchAux_LeftOuterNodeCheck
            minValue = centerNodeValue;
            otherNodeValue = outerRingMinValue;

            // now check whether center node or outer ring minimum is smaller
            fastMinEdgeSearchAux_LeftOuterNodeCheck(
                level,
                minFace, minNode, minValue,
                otherNodeCode, otherNodeValue,
                neighborEdgeCode, rightCenterFaceFValueA,
                minFaceNeighbor, neighborOrientationMatch);
        }
        else
        {
            minFace = minFaceNeighbor;
            minFace.set(level, otherNodeRight);
            // minFaceNeighbor.set(level,0); already done
            neighborOrientationMatch = true;

            neighborEdgeCode = otherNodeRight;

            if (outerRingMinValue < centerNodeValue)
            {
                otherNodeCode = minNodeRight;
                minNode = nodeCode2;
                minValue = outerRingMinValue;
                otherNodeValue = centerNodeValue;
            }
            else
            {
                otherNodeCode = nodeCode2;
                minNode = minNodeRight;
                minValue = centerNodeValue;
                otherNodeValue = outerRingMinValue;
            }
        }
        break;
    }
    case 5:
    {
        if (rightCenterFaceFValueB < minValue)
        {
            minFace = minFaceNeighbor;
            // minFace.set(level, 0) already done
            minFaceNeighbor.set(level,minNodeRight);
            neighborOrientationMatch = true;

            otherNodeCode = otherNodeRight;
            minNode = nodeCode2;
            neighborEdgeCode = minNodeRight;

            // outerRingMinValue may be smaller than centerValue, this will be checked by fastMinEdgeSearchAux_LeftOuterNodeCheck
            minValue = centerNodeValue;
            otherNodeValue = outerRingMinValue;

            // now check whether center node or outer ring minimum is smaller
            fastMinEdgeSearchAux_LeftOuterNodeCheck(
                level,
                minFace, minNode, minValue,
                otherNodeCode, otherNodeValue,
                neighborEdgeCode, rightCenterFaceFValueB,
                minFaceNeighbor, neighborOrientationMatch);
        }
        else
        {
            minFace = minFaceNeighbor;
            minFace.set(level, minNodeRight);
            // minFaceNeighbor.set(level,0); already done
            neighborOrientationMatch = true;

            neighborEdgeCode = minNodeRight;

            if (outerRingMinValue < centerNodeValue)
            {
                otherNodeCode = otherNodeRight;
                minNode = nodeCode2;
                minValue = outerRingMinValue;
                otherNodeValue = centerNodeValue;
            }
            else
            {
                otherNodeCode = nodeCode2;
                minNode = otherNodeRight;
                minValue = centerNodeValue;
                otherNodeValue = outerRingMinValue;
            }
        }
        break;
    }
    case 6:
    {
        minFace.set(level, minNode);
        minFaceNeighbor.set(level, minNodeRight);
        // neighborOrientationMatch unchanged

        otherNodeCode = nodeCode1;
        minNode = nodeCode0;
        // neighborEdgeCode = nodeCode2; -> no change

        otherNodeValue = minValue;
        minValue = centerNodeValue;
        break;
    }
    default:
    {
        break;
    }
    }
}
// minimum edge search auxiliary function:
// search for function minimum within nodes around edge end point
template<typename gridFunction>
static void fastMinEdgeSearchAux_EdgeEndSmallerCase(
    gridFunction minFunc, // function to minimize
    size_t level, // grid level of the search
    sGrid& minFace, // face adjacent to the relevant edge
    unsigned short& minNode, // node code inside minFace of the minimum point, aka nodeCode0
    scaledFP& minValue, // function value at the minimum node
    unsigned short& otherNodeCode, // node code at the other (non-minimum) end point of neighborEdge
    scaledFP& otherNodeValue, // function value at this otherNode
    unsigned short& neighborEdgeCode, // the edge between minFace and minFaceNeighbor (minimum node is one of the edge end points)
    sGrid& minFaceNeighbor, // neighbor face next to minFace; input: previous grid level, output: next grid level
    bool& neighborOrientationMatch, // whether orientation of minFace and minFaceNeighbor is matched
    const unsigned short& minNodeRight, // node code inside minFaceNeighbor of the minimum node of neighborEdge
    const unsigned short& otherNodeRight // node code inside minFaceNeighbor of the other node of neighborEdge
)
{
    unsigned short nodeCode0 = minNode;
    unsigned short nodeCode1 = otherNodeCode;
    unsigned short nodeCode2 = neighborEdgeCode;

    bool edgeAOrientationMatch = true;
    sGrid leftFaceA = minFace.neighborFace(level, nodeCode1, edgeAOrientationMatch);

    scaledFP outerNode1Value = minFunc(level, minFace, nodeCode2);
    scaledFP outerNode2Value = minFunc(level, leftFaceA, nodeCode1);
    scaledFP outerNode5Value = minFunc(level, minFaceNeighbor, nodeCode2); // will be treated as outerNode #3 in case of only four adjacent faces

    /* variables for minimum node inside outer ring */
    scaledFP outerRingMinValue = otherNodeValue;
    unsigned short outerMinIndex = 6;

    if (outerNode1Value < outerRingMinValue)
    {
        outerMinIndex = 1;
        outerRingMinValue = outerNode1Value;
    }
    if (outerNode2Value < outerRingMinValue)
    {
        outerMinIndex = 2;
        outerRingMinValue = outerNode2Value;
    }
    if (outerNode5Value < outerRingMinValue)
    {
        outerMinIndex = 5;
        outerRingMinValue = outerNode5Value;
    }

    /* check the case of only four adjacent faces first */
    if (!(neighborOrientationMatch || edgeAOrientationMatch))
    {
        switch (outerMinIndex)
        {
        case 1:
        {
            neighborEdgeCode = nodeCode1;
            //neighborOrientationMatch unchanged (false)

            if (outerNode2Value < otherNodeValue)
            {
                otherNodeValue = outerRingMinValue;

                // minNode = nodeCode0; -> no change
                otherNodeCode = nodeCode2;

                minFaceNeighbor = minFace;
                minFace = leftFaceA;

                if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                            level,
                            minFace, minNode, minValue,
                            otherNodeCode, otherNodeValue,
                            neighborEdgeCode, outerNode2Value,
                            minFaceNeighbor, neighborOrientationMatch))
                {
                    fastMinEdgeSearchAux_OuterEdgeCase(
                        minFunc, level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
                }
            }
            else
            {
                minFaceNeighbor = leftFaceA;
                // minFace unchanged;

                if (outerNode1Value < minValue)
                {
                    otherNodeValue = minValue;
                    minValue = outerNode1Value;

                    minNode = nodeCode2;
                    otherNodeCode = nodeCode0;
                }
                else
                {
                    // minValue unchanged
                    otherNodeValue = outerNode1Value;

                    // minNode = nodeCode0; unchanged
                    otherNodeCode = nodeCode2;
                }
            }
            break;
        }
        case 2:
        {
            //neighborEdgeCode = nodeCode2; -> no change
            //neighborOrientationMatch unchanged (false)

            otherNodeValue = outerRingMinValue;

            // minNode = nodeCode0; -> no change
            otherNodeCode = nodeCode1;

            if (outerNode1Value < outerNode5Value)
            {
                minFace = leftFaceA;
                minFaceNeighbor = leftFaceA.neighborFace(level, neighborEdgeCode);

                if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                            level,
                            minFace, minNode, minValue,
                            otherNodeCode, otherNodeValue,
                            neighborEdgeCode, outerNode1Value,
                            minFaceNeighbor, neighborOrientationMatch))
                {
                    fastMinEdgeSearchAux_OuterEdgeCase(
                        minFunc, level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
                }
            }
            else
            {
                minFaceNeighbor = leftFaceA;
                minFace = leftFaceA.neighborFace(level, neighborEdgeCode);

                if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                            level,
                            minFace, minNode, minValue,
                            otherNodeCode, otherNodeValue,
                            neighborEdgeCode, outerNode5Value,
                            minFaceNeighbor, neighborOrientationMatch))
                {
                    fastMinEdgeSearchAux_OuterEdgeCase(
                        minFunc, level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
                }
            }
            break;
        }
        case 5:
        {
            neighborEdgeCode = nodeCode1;
            //neighborOrientationMatch unchanged (false)

            if (outerNode2Value < otherNodeValue)
            {
                otherNodeValue = outerRingMinValue;

                // minNode = nodeCode0; -> no change
                otherNodeCode = nodeCode2;

                // minFaceNeighbor unchanged
                minFace = minFaceNeighbor.neighborFace(level, neighborEdgeCode);

                if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                            level,
                            minFace, minNode, minValue,
                            otherNodeCode, otherNodeValue,
                            neighborEdgeCode, outerNode2Value,
                            minFaceNeighbor, neighborOrientationMatch))
                {
                    fastMinEdgeSearchAux_OuterEdgeCase(
                        minFunc, level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
                }
            }
            else
            {
                minFace = minFaceNeighbor;
                minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode);

                if (outerNode5Value < minValue)
                {
                    otherNodeValue = minValue;
                    minValue = outerNode5Value;

                    minNode = nodeCode2;
                    otherNodeCode = nodeCode0;
                }
                else
                {
                    // minValue unchanged
                    otherNodeValue = outerNode5Value;

                    // minNode = nodeCode0; unchanged
                    otherNodeCode = nodeCode2;
                }
            }
            break;
        }
        //case 6: no changes
        default:
        {
            break;
        }
        }
        return;
    }

    bool edgeBOrientationMatch = true;
    unsigned short edgeCodeBLeft = nodeCode0;
    if (!edgeAOrientationMatch) edgeCodeBLeft = nodeCode2;

    sGrid leftFaceB = leftFaceA.neighborFace(level,edgeCodeBLeft,edgeBOrientationMatch);
    sGrid rightFaceA = minFaceNeighbor.neighborFace(level, otherNodeRight);

    scaledFP outerNode3Value = minFunc(level, leftFaceB, edgeCodeBLeft);
    scaledFP outerNode4Value = minFunc(level, rightFaceA, otherNodeRight);

    if (outerNode3Value < outerRingMinValue)
    {
        outerMinIndex = 3;
        outerRingMinValue = outerNode3Value;
    }
    if (outerNode4Value < outerRingMinValue)
    {
        outerMinIndex = 4;
        outerRingMinValue = outerNode4Value;
    }
/*
    std::cout << "fastMinEdgeSearchAux_EdgeEndSmallerCase: ";
    std::cout << " level: " << level;
    std::cout << " minFace: " << minFace;
    std::cout << " minNode: " << minNode;
    std::cout << " minValue: " << minValue;
    std::cout << " neighborEdgeCode: " << neighborEdgeCode;
    std::cout << " otherNodeCode: " << otherNodeCode;
    std::cout << " otherNodeValue: " << otherNodeValue;
    std::cout << " neighborOrientationMatch: " << neighborOrientationMatch;
    std::cout << " minFaceNeighbor: " << minFaceNeighbor;
    std::cout << " leftFaceA: " << leftFaceA;
    std::cout << " outerNode1Value: " << outerNode1Value;
    std::cout << " outerNode2Value: " << outerNode2Value;
    std::cout << " outerNode3Value: " << outerNode3Value;
    std::cout << " outerNode4Value: " << outerNode4Value;
    std::cout << " outerNode5Value: " << outerNode5Value;
    std::cout << " outerRingMinValue: " << outerRingMinValue;
    std::cout << " outerMinIndex: " << outerMinIndex;
    std::cout << std::endl;
*/
    switch (outerMinIndex)
    {
    case 1:
    {
        neighborEdgeCode = nodeCode1;
        neighborOrientationMatch = edgeAOrientationMatch;

        if (outerNode2Value < otherNodeValue)
        {
            //minValue unchanged for now, will update within fastMinEdgeSearchAux_LeftOuterNodeCheck
            otherNodeValue = outerRingMinValue;

            if (edgeAOrientationMatch)
            {
                minNode = nodeCode2;
                otherNodeCode = nodeCode0;
            }
            else
            {
                minNode = nodeCode0;
                otherNodeCode = nodeCode2;
            }

            minFaceNeighbor = minFace;
            minFace = leftFaceA;

            if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                        level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, outerNode2Value,
                        minFaceNeighbor, neighborOrientationMatch))
            {
                fastMinEdgeSearchAux_OuterEdgeCase(
                    minFunc, level,
                    minFace, minNode, minValue,
                    otherNodeCode, otherNodeValue,
                    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
            }
        }
        else
        {
            minFaceNeighbor = leftFaceA;
            // minFace unchanged;

            if (outerNode1Value < minValue)
            {
                otherNodeValue = minValue;
                minValue = outerNode1Value;

                minNode = nodeCode2;
                otherNodeCode = nodeCode0;
            }
            else
            {
                // minValue unchanged
                otherNodeValue = outerNode1Value;

                // minNode = nodeCode0; unchanged
                otherNodeCode = nodeCode2;
            }
        }
        break;
    }
    case 2:
    {
        neighborEdgeCode = edgeCodeBLeft;
        neighborOrientationMatch = edgeBOrientationMatch;

        //minValue unchanged for now, will update within fastMinEdgeSearchAux_LeftOuterNodeCheck
        otherNodeValue = outerRingMinValue;

        if (outerNode1Value < outerNode3Value)
        {
            minFace = leftFaceA;
            minFaceNeighbor = leftFaceB;

            minNode = nodeCode2;
            if (!edgeAOrientationMatch) minNode = nodeCode0;
            otherNodeCode = nodeCode1;

            if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                        level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, outerNode1Value,
                        minFaceNeighbor, neighborOrientationMatch))
            {
                fastMinEdgeSearchAux_OuterEdgeCase(
                    minFunc, level,
                    minFace, minNode, minValue,
                    otherNodeCode, otherNodeValue,
                    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
            }
        }
        else
        {
            minFace = leftFaceB;
            minFaceNeighbor = leftFaceA;

            if (edgeBOrientationMatch)
            {
                minNode = nodeCode1;
                if (edgeAOrientationMatch)
                {
                    otherNodeCode = nodeCode2;
                }
                else
                {
                    otherNodeCode = nodeCode0;
                }
            }
            else
            {
                minNode = nodeCode2;
                otherNodeCode = nodeCode1;
            }

            if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                        level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, outerNode3Value,
                        minFaceNeighbor, neighborOrientationMatch))
            {
                fastMinEdgeSearchAux_OuterEdgeCase(
                    minFunc, level,
                    minFace, minNode, minValue,
                    otherNodeCode, otherNodeValue,
                    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
            }
        }
        break;
    }
    case 3:
    {
        // neighborOrientationMatch unchanged

        //minValue unchanged for now, will update within fastMinEdgeSearchAux_LeftOuterNodeCheck
        otherNodeValue = outerRingMinValue;

        if (outerNode2Value < outerNode4Value)
        {
            if (edgeBOrientationMatch)
            {
                minNode = nodeCode1;
                if (edgeAOrientationMatch)
                {
                    otherNodeCode = nodeCode0;
                    //neighborEdgeCode = nodeCode2; -> no change
                }
                else
                {
                    otherNodeCode = nodeCode2;
                    neighborEdgeCode = nodeCode0;
                }
            }
            else
            {
                minNode = nodeCode2;
                otherNodeCode = nodeCode0;
                neighborEdgeCode = nodeCode1;
            }

            minFace = leftFaceB;
            minFaceNeighbor = leftFaceB.neighborFace(level, neighborEdgeCode);

            if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                        level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, outerNode2Value,
                        minFaceNeighbor, neighborOrientationMatch))
            {
                fastMinEdgeSearchAux_OuterEdgeCase(
                    minFunc, level,
                    minFace, minNode, minValue,
                    otherNodeCode, otherNodeValue,
                    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
            }
        }
        else
        {
            if (edgeAOrientationMatch)
            {
                minNode = otherNodeRight;
                if (edgeBOrientationMatch)
                {
                    otherNodeCode = minNodeRight;
                    //neighborEdgeCode = nodeCode2; -> no change
                }
                else
                {
                    otherNodeCode = nodeCode2;
                    neighborEdgeCode = otherNodeRight;
                }
            }
            else
            {
                minNode = nodeCode2;
                otherNodeCode = minNodeRight;
                neighborEdgeCode = otherNodeRight;
            }

            minFaceNeighbor = leftFaceB;
            minFace = leftFaceB.neighborFace(level, neighborEdgeCode);

            if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                        level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, outerNode4Value,
                        minFaceNeighbor, neighborOrientationMatch))
            {
                fastMinEdgeSearchAux_OuterEdgeCase(
                    minFunc, level,
                    minFace, minNode, minValue,
                    otherNodeCode, otherNodeValue,
                    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
            }
        }
        break;
    }
    case 4:
    {
        neighborOrientationMatch = edgeAOrientationMatch;

        //minValue unchanged for now, will update within fastMinEdgeSearchAux_LeftOuterNodeCheck
        otherNodeValue = outerRingMinValue;

        if (outerNode3Value < outerNode5Value)
        {
            if (edgeAOrientationMatch)
            {
                minNode = otherNodeRight;
                if (edgeBOrientationMatch)
                {
                    otherNodeCode = nodeCode2;
                    neighborEdgeCode = minNodeRight;
                }
                else
                {
                    otherNodeCode = minNodeRight;
                    neighborEdgeCode = nodeCode2;
                }
            }
            else
            {
                minNode = nodeCode2;
                otherNodeCode = nodeCode0;
                neighborEdgeCode = nodeCode1;
            }

            minFaceNeighbor = rightFaceA;
            minFace = rightFaceA.neighborFace(level, neighborEdgeCode);

            if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                        level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, outerNode3Value,
                        minFaceNeighbor, neighborOrientationMatch))
            {
                fastMinEdgeSearchAux_OuterEdgeCase(
                    minFunc, level,
                    minFace, minNode, minValue,
                    otherNodeCode, otherNodeValue,
                    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
            }
        }
        else
        {
            otherNodeCode = otherNodeRight;
            if (edgeBOrientationMatch)
            {
                minNode = nodeCode2;
                neighborEdgeCode = minNodeRight;
            }
            else
            {
                minNode = minNodeRight;
                neighborEdgeCode = nodeCode2;
            }

            minFace = rightFaceA;
            minFaceNeighbor = rightFaceA.neighborFace(level, neighborEdgeCode);

            if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                        level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, outerNode4Value,
                        minFaceNeighbor, neighborOrientationMatch))
            {
                fastMinEdgeSearchAux_OuterEdgeCase(
                    minFunc, level,
                    minFace, minNode, minValue,
                    otherNodeCode, otherNodeValue,
                    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
            }
        }
        break;
    }
    case 5:
    {
        neighborEdgeCode = otherNodeRight;
        neighborOrientationMatch = edgeBOrientationMatch;

        if (outerNode4Value < otherNodeValue)
        {
            //minValue unchanged for now, will update within fastMinEdgeSearchAux_LeftOuterNodeCheck
            otherNodeValue = outerRingMinValue;

            if (edgeBOrientationMatch)
            {
                minNode = nodeCode2;
                otherNodeCode = minNodeRight;
            }
            else
            {
                minNode = minNodeRight;
                otherNodeCode = nodeCode2;
            }

            // minFaceNeighbor unchanged
            minFace = rightFaceA;

            if (fastMinEdgeSearchAux_LeftOuterNodeCheck(
                        level,
                        minFace, minNode, minValue,
                        otherNodeCode, otherNodeValue,
                        neighborEdgeCode, outerNode4Value,
                        minFaceNeighbor, neighborOrientationMatch))
            {
                fastMinEdgeSearchAux_OuterEdgeCase(
                    minFunc, level,
                    minFace, minNode, minValue,
                    otherNodeCode, otherNodeValue,
                    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
            }
        }
        else
        {
            minFace = minFaceNeighbor;
            minFaceNeighbor = rightFaceA;

            if (outerNode5Value < minValue)
            {
                otherNodeValue = minValue;
                minValue = outerNode5Value;

                minNode = nodeCode2;
                otherNodeCode = minNodeRight;
            }
            else
            {
                // minValue unchanged
                otherNodeValue = outerNode5Value;

                minNode = minNodeRight;
                otherNodeCode = nodeCode2;
            }
        }
        break;
    }
    //case 6: no changes
    default:
    {
        break;
    }
    }
    return;
}
// minimum edge search auxiliary function:
// check whether function value is smaller at the edge end or mid point and search new minimum edge around there
template<typename gridFunction>
static void fastMinEdgeSearchAux_StepToNextLevel(
    gridFunction minFunc, // function to minimize
    size_t nextLevel, // the next grid level to search for the minimum
    sGrid& minFace, // face including the minimum node; input: previous grid level, output: next grid level
    unsigned short& minNode, // node code at the minimum node inside minFace; input: previous grid level, output: next grid level
    scaledFP& minValue, // minimum function value; input: previous grid level, output: next grid level
    unsigned short& otherNodeCode, // node code at the other (non-minimum) end point of neighborEdge
    scaledFP& otherNodeValue, // function value at this otherNode
    unsigned short& neighborEdgeCode, // the edge between minFace and minFaceNeighbor (minimum node is one of the edge end points)
    sGrid& minFaceNeighbor, // neighbor face next to minFace; input: previous grid level, output: next grid level
    bool& neighborOrientationMatch // whether orientation of minFace and minFaceNeighbor is matched
)
{
    minFace.setExtend(nextLevel, minNode);
    scaledFP centerNodeValue = minFunc(nextLevel, minFace, otherNodeCode);

    unsigned short minNodeRight = otherNodeCode;
    unsigned short otherNodeRight = minNode;
    if (!neighborOrientationMatch) std::swap(minNodeRight, otherNodeRight);

    if (minValue < centerNodeValue)
    {
        minFaceNeighbor.setExtend(nextLevel, minNodeRight);
        otherNodeValue = centerNodeValue;

        fastMinEdgeSearchAux_EdgeEndSmallerCase(
            minFunc, nextLevel,
            minFace, minNode, minValue,
            otherNodeCode, otherNodeValue,
            neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
            minNodeRight, otherNodeRight
        );
    }
    else
    {
        fastMinEdgeSearchAux_MidEdgeSmallerCase(
            minFunc, nextLevel,
            minFace, minNode, minValue,
            otherNodeCode, otherNodeValue, centerNodeValue,
            neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
            minNodeRight, otherNodeRight
        );
    }
}
// minimum edge search auxiliary function: initialize first proper minimum edge and step through the grid levels
template<typename gridFunction>
static void fastMinEdgeSearchAux(
    gridFunction minFunc, // function to minimize
    size_t startLevel, // grid level where to start the search
    size_t maxLevel, // highest grid level of the search
    sGrid& minFace, // input: starting face of the search, output: minimum face
    unsigned short& minNode, // input: starting node of the search, output: minimum node code
    scaledFP& minValue, // input: function value at the starting node, output: minimum node value found
    unsigned short& neighborEdgeCode, // edge code of the relevant edge
    sGrid& minFaceNeighbor, // neighbor face of minFace across the relevant edge: input at start, output final result
    bool& neighborOrientationMatch, // whether orientation of minFace and minFaceNeighbor is matched
    unsigned short& otherNodeCode // node code inside minFace of the other end (not minimum) of the relevant edge
)
{
    scaledFP otherNodeValue = minFunc(startLevel, minFace, otherNodeCode);

    if (otherNodeValue < minValue)
    {
        std::swap(minValue, otherNodeValue);
        std::swap(minNode, otherNodeCode);
    }

    unsigned short minNodeRight = otherNodeCode;
    unsigned short otherNodeRight = minNode;
    if (!neighborOrientationMatch) std::swap(minNodeRight, otherNodeRight);

    fastMinEdgeSearchAux_EdgeEndSmallerCase(
        minFunc, startLevel,
        minFace, minNode, minValue,
        otherNodeCode, otherNodeValue,
        neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
        minNodeRight, otherNodeRight);

    for (size_t level_it = 0; level_it < maxLevel; )
    {
        ++level_it;
        fastMinEdgeSearchAux_StepToNextLevel(
            minFunc, level_it,
            minFace, minNode, minValue,
            otherNodeCode, otherNodeValue,
            neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
    }
}
// minimum edge search, starting at a defined location (node or face center), returning the minimum nearby edge
template<typename gridFunction>
static void fastMinEdgeSearch(
    gridFunction minFunc, // function to minimize
    size_t startLevel, // grid level where to start the search
    size_t maxLevel, // highest grid level of the search
    sGrid& minFace, // input: starting face of the search, output: minimum face
    unsigned short& minLocation, // input: starting location (node or face center) of the search, output: minimum node code
    scaledFP& minValue, // input: function value at the starting point, output: minimum node value found
    unsigned short& neighborEdgeCode, // input: which edge to start from, output: edge code of the minimum edge
    sGrid& minFaceNeighbor, // only output: neighbor face of minFace across the relevant edge
    bool& neighborOrientationMatch, // only output: whether orientation of minFace and minFaceNeighbor is matched
    unsigned short& otherNodeCode // only output: node code inside minFace of the other end (not minimum) of the relevant edge
)
{
    otherNodeCode = newCode(minLocation, neighborEdgeCode);

    neighborOrientationMatch = true;
    minFaceNeighbor = minFace.neighborFace(startLevel, neighborEdgeCode, neighborOrientationMatch);
/*
    std::cout << "fastMinEdgeSearch: ";
    std::cout << " startLevel: " << startLevel;
    std::cout << " maxLevel: " << maxLevel;
    std::cout << " minFace: " << minFace;
    std::cout << " minLocation: " << minLocation;
    std::cout << " minValue: " << minValue;
    std::cout << " neighborEdgeCode: " << neighborEdgeCode;
    std::cout << " otherNodeCode: " << otherNodeCode;
    std::cout << " neighborOrientationMatch: " << neighborOrientationMatch;
    std::cout << " minFaceNeighbor: " << minFaceNeighbor;
    std::cout << std::endl;
*/
    fastMinEdgeSearchAux(
        minFunc, startLevel, maxLevel,
        minFace, minLocation, minValue,
        neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
        otherNodeCode);
}
public:
// minimum edge search, starting at a defined location (node or face center), returning the minimum nearby edge
template<typename gridFunction>
static void fastMinEdgeSearch(
    gridFunction minFunc, // function to minimize
    size_t startLevel, // grid level where to start the search
    size_t maxLevel, // highest grid level of the search
    sGrid& minFace, // input: starting face of the search, output: minimum face
    unsigned short& minLocation, // input: starting location (node or face center) of the search, output: minimum node code
    unsigned short& neighborEdgeCode, // input: which edge to start from, output: edge code of the minimum edge
    scaledFP& minValue // only output: minimum node value found
)
{
    if (minLocation < 1) minLocation = 1;
    if (minLocation > 3) minLocation = 3;

    if (neighborEdgeCode < 1 || neighborEdgeCode > 3 || neighborEdgeCode == minLocation) neighborEdgeCode = nextNode(minLocation);

    minValue = minFunc(startLevel, minFace, minLocation);

    sGrid minFaceNeighbor;
    bool neighborOrientationMatch;
    unsigned short otherNodeCode;

    fastMinEdgeSearch(
        minFunc, startLevel, maxLevel,
        minFace, minLocation, minValue,
        neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch, otherNodeCode);
}
// minimum face search, return minimum function value through reference minValue
template<typename gridFunction>
static sGrid fastMinSearch(
    gridFunction minFunc, // function to minimize
    size_t maxLevel, // highest grid level of the search
    scaledFP& minValue // only output: minimum face value found
)
{
    sGrid minFace = NorthOct0.first;
    unsigned short minNode = NorthOct0.second;
    minValue = minFunc(0, minFace, minNode);

    unsigned short otherNodeCode = nextNode(minNode);
    unsigned short neighborEdgeCode = nextNode(otherNodeCode);

    bool neighborOrientationMatch = true;
    sGrid minFaceNeighbor = minFace.neighborFace(0, neighborEdgeCode, neighborOrientationMatch);

    fastMinEdgeSearch(
        minFunc, 0, maxLevel,
        minFace, minNode, minValue,
        neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch, otherNodeCode);

    minValue = minFunc(maxLevel, minFace, 0);
    scaledFP rightCenterValue = minFunc(maxLevel, minFaceNeighbor, 0);

    if (rightCenterValue < minValue)
    {
        minValue = rightCenterValue;
        minFace = minFaceNeighbor;
    }

    return minFace;
}
// minimum face search without returning function value
template<typename gridFunction>
static sGrid fastMinSearch(
    gridFunction minFunc, // function to minimize
    size_t maxLevel = accuracyBits // highest grid level of the search
)
{
    scaledFP minValue;
    return fastMinSearch(minFunc, maxLevel, minValue);
}
#endif // SPIRID_SGRID_SEARCHES_H_INCLUDED
