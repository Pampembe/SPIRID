#ifndef SPIRID_H_INCLUDED
#define SPIRID_H_INCLUDED

#include <iostream>

#include <vector> // grid based coordinate system uses vector<bool> to label points
#include <list>   // for member function return values (like the list of neighbors)

#include <SPIRID_aux.h>


//SPIRID - SPherical CoordInate gRID
namespace SPIRID
{
class sGrid;
//output for grid coordinates as face codes
std::ostream& operator << (std::ostream& out, const sGrid&);
//triangle grid based coordinates of the sphere (radius==1)
/*  Each octant of the sphere is considered a spherical triangle.
    Every spherical triangle is then successively divided into four sub-triangles.
    Nomenclature:
        Face: spherical triangle in the resulting grid
        Node/Edge: nodes and edges of a face
        Point: A point inside a face, further characterized by a location code
            location 0: the point where side bisection lines intersect
            location 1,2,3: the respective nodes of the face
        Level: number of face divisions to get to a certain face (initial octant is level 0)
*/
class sGrid
{
public:
	/*  A face is represented by a gridCode with depth+1 digits:
	        1st digit (0..7): grid level 0, identifies the octant of the sphere
	        nth digit (0..3): grid level n-1, identifies the sub-faces after successive divisions
	        depth+1: number of digits of the gridCode
	    A gridCode of depth D also represents D faces with smaller depths through truncation of the last digits
	    A gridCode of depth D also represents infinitly many faces with higher grid levels through appending zeros after digit D+1
	*/

	//standard constructors
	inline sGrid(size_t depth = 0) : gridCode(2*depth+3) {};
	inline sGrid(const sGrid& P) : gridCode(P.gridCode) {};

	//constructor using a list of face codes
	sGrid(const std::vector<unsigned short>& faceCodes);
	//internally we represent all faceCodes in a single bit array
	sGrid(const std::vector<bool>& bitCode);


	//depth of the gridCode & resize
	inline size_t depth() const {
		return (gridCode.size()-3)/2; //internally gridCode is a vector<bool> gridCode of length 3+2*depth --> make sure gridCode.size() always >=3
	};
	inline void resize(size_t newDepth) {
		gridCode.resize(3+2*newDepth);
	};





protected:
	//get & set face code: (0..7) at level 0, (0..3) at other levels
	unsigned short operator [] (size_t level) const;
	const sGrid& set(size_t level, unsigned short faceCode);
public:
	unsigned short at(size_t level) const;
	const sGrid& setExtend(size_t level, unsigned short faceCode);
	//set all face codes in level and above to zero
	const sGrid& reset(size_t level);
	//copy from another sGrid
	inline sGrid& copy(const sGrid& Q)
	{
		gridCode = Q.gridCode;
		return *this;
	};

	//mirror a face to the opposite side of the sphere
	inline sGrid& mirror() {
		gridCode[0]=!gridCode[0];
		gridCode[1]=!gridCode[1];
		gridCode[2]=!gridCode[2];
		return *this;
	};



	/* special points on the sphere (represented as pairs of <sGrid, nodeCode (0..3)> ) */
	static const std::pair<sGrid,unsigned short> NorthOct0;
	static const std::pair<sGrid,unsigned short> WestOct0;
	static const std::pair<sGrid,unsigned short> SouthOct7;
	static const std::pair<sGrid,unsigned short> EastOct7;



	//comparison operators
	inline bool operator == (const sGrid& P) const
	{
		return gridCode == P.gridCode;
	};
	inline bool operator != (const sGrid& P) const
	{
		return !(operator == (P));
	}





public:
	/* grid topology related functions */
	//stepping to the neighbor face (across a single edge)
	sGrid neighborFace(size_t level, unsigned short edgeCode) const;
	//as above, includes check whether neighbor face has the same orientation
	sGrid neighborFace(size_t level, unsigned short edgeCode, bool& orientationMatch) const;
private:
	//assign the neighbor of the current face to an existing sGrid object, assuming its depth is already at level
	static sGrid& assignNeighborFace(size_t level, unsigned short edgeCode, sGrid& neighbor, bool& orientationMatch);

public:
	std::list<sGrid> neighborFaces(size_t level) const;
	/* nodes and edges in the grid are characterized by a pair of (sGrid, code 1..3) */
	std::list<sGrid> edgeNeighborFaces(size_t level, unsigned short edgeCode) const;
	std::list<sGrid> nodeNeighborFaces(size_t level, unsigned short nodeCode) const;
	std::list<std::pair<sGrid, unsigned short> > nodeNeighborNodes(size_t level, unsigned short nodeCode) const;
	std::list<std::pair<sGrid, unsigned short> > nodeConnectedEdges(size_t level, unsigned short nodeCode) const;
	std::list<std::pair<sGrid, unsigned short> > nodeOuterRingEdges(size_t level, unsigned short nodeCode) const;

	// subGridScanner can be used to scan through all sub-faces at scannerLevel in a given list of faces at gridMinLevel
	class subGridScanner
	{
		size_t scannerLevel;
		size_t gridMinLevel;

		std::list<sGrid> scanFaces;
		std::list<sGrid>::iterator currentFace;
		std::list<sGrid>::iterator endIterator;

		inline bool stepToNextFace()
		{
			return stepToNextFace(scannerLevel);
		};
		bool stepToNextFace(size_t gridLevel);

	public:
		subGridScanner(const std::list<sGrid>&, size_t scanLevel, size_t minLevel = 1);
		inline subGridScanner(const subGridScanner& X) :
			scannerLevel(X.scannerLevel),
			gridMinLevel(X.gridMinLevel),
			scanFaces(X.scanFaces),
			currentFace(scanFaces.begin()),
			endIterator(scanFaces.end())
		{
			reset();
		};
		subGridScanner& operator = (const subGridScanner& X);
		void reset();

		inline subGridScanner& operator ++()
		{
			if (!stepToNextFace()) ++currentFace;
			return *this;
		};
		inline subGridScanner& operator ++(int n)
		{
			for ( int m = 0; m<n && currentFace != endIterator; m++) operator ++();
			return *this;
		};
		bool operator == (const subGridScanner& s2) const;
		inline bool operator != (const subGridScanner& s2) const
		{
			return !(operator == (s2));
		};
		inline sGrid& operator *() const
		{
			return *currentFace;
		}
		inline sGrid* operator ->() const
		{
			return currentFace.operator ->();
		}
	};
	inline subGridScanner begin() const
	{
		return begin(depth(),1);
	}
	subGridScanner begin(size_t scanLevel, size_t minLevel = 1) const;
	subGridScanner end() const;

protected:
	// auxiliary functions to get new node, edge & face codes (independent of the grid)
	inline static unsigned short nextNode(unsigned short code) {
		return newNodeCodes[code];
	};
	inline static unsigned short nextFace(unsigned short code) {
		return newFaceCodes[code];
	};
	inline static unsigned short newCode(unsigned short code1, unsigned short code2, unsigned short code3 = 0) {
		return newCodes[code1+code2+code3];
	};
private:
	/* constant arrays used in auxiliary functions above */
	static const unsigned short newNodeCodes[4];
	static const unsigned short newFaceCodes[4];
	static const unsigned short newCodes[7];





private:
	/* calculation of the distance between two points P1 & P2:
	    first step is to find out which is the highest grid level where the faces of P1 and P2 have a common node
	        the grid level can be different for P1 & P2 (if one of the points is closer to the common node)
	    then we calculate the distance of P1 & P2 to this common node and the angle between the respective connections
	    finally we use the spherical cosine law to calculate the distance
	*/
	//data type used for information about the common node and the angle between the corresponding faces
	struct pointPairRefNode
	{
		// {0,0,0, 0,0,0, 0,0} means no common node --> calculate pi - distance to a mirrored point

		// grid level, nodeCode & edgeCode for common node in face P1
		size_t level1;
		unsigned short nodeCode1;
		unsigned short edgeCode1;

		// grid level, nodeCode & edgeCode for common node in face P2
		size_t level2;
		unsigned short nodeCode2;
		unsigned short edgeCode2;

		//the angle between edgeCode1 and edgeCode2 at the common node
		fp_type gapAngle;

		/* For point distances we calculate local polar coordinates first.
		   Depending on the face arrangement we need either the sum or the difference of the local azimuthal angles phi.
		   --> final formula is phi1 + gapAngle + signAngle2*phi1	*/
		short signAngle2;

		//output pointPairRefNode data
		std::ostream& print(std::ostream&);
	};
	//main function to get common node data (including gapAngle): this is independent on whether we refer to the center point of a node of the face
	static pointPairRefNode findHighestRefNode(size_t level1, const sGrid& P1, size_t level2, const sGrid& P2);
	//as above, point level is the depth of the face
	static pointPairRefNode findHighestRefNode(const sGrid& P1, const sGrid& P2)
	{
		return findHighestRefNode(P1.depth(), P1, P2.depth(), P2);
	};
	//auxliary function used when P1 and P2 are in the same face at all lower levels (level-1)
	static pointPairRefNode sameFaceStepupTo(size_t level, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2);
	//auxliary function used when the faces of P1 and P2 at level-1 share a common edge
	static pointPairRefNode commonEdgeStepupTo(size_t level, unsigned short edgeCode, bool orientationMatch, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2);
	//auxiliary function: assuming faceCode at startlevel-1 equals nodeCode: until which level nodeCode repeats?
	inline static size_t lastLevelAtNode(size_t startLevel, unsigned short nodeCode, size_t endLevel, const sGrid& P)
	{
		while (P.at(startLevel)==nodeCode && startLevel<=endLevel) {
			startLevel++;
		};
		return startLevel-1;
	};





public:
	//calculation accuracy (accuracyBits cannot be larger than digits of fp_type)
	static void setAccuracyBits(size_t);
	inline static size_t getAccuracyBits() {
		return accuracyBits;
	};



	/* geometry related functions */
	/* Each point at every grid level corresponds to a
	   face (faceCodes 0..3) with edges and nodes (codes 1..3).
	   We can calculate edge lengths, interior angles and area for that face.
	*/
	//get the orientation of a face
	inline signed short orientation() const
	{
		signed short fOrientation = 1;
		if (gridCode[0]) fOrientation *= -1;
		if (gridCode[1]) fOrientation *= -1;
		if (gridCode[2]) fOrientation *= -1;
		return fOrientation;
	}
	//calculate the area of a face
	inline angle area(size_t level) const
	{
		return area(level, calcFaceGeometry(level));
	};
	//calculate an edge length (code 1..3)
	angle edgeLength(size_t level, unsigned short edgeCode) const;
	//calculate the interior angle of a face at a node (1..3)
	inline angle interiorAngle(size_t level, unsigned short nodeCode) const
	{
		return angle( interiorAngle(level, nodeCode, calcFaceGeometry(level)) );
	};
private:
	/* auxiliary functions used to calculate face geometries */
	//data type used for geometry data of faces: lengths of the three edges E as Sin(E)^2*2^(2*level)
	struct faceGeometry {
		fp_type SinaSq;
		fp_type SinbSq;
		fp_type SincSq;

		inline fp_type operator[] (unsigned short edgeCode) const
		{
			switch (edgeCode)
			{
			case 1:
			{
				return SinaSq;
				break;
			}
			case 2:
			{
				return SinbSq;
				break;
			}
			case 3:
			{
				return SincSq;
				break;
			}
			default:
			{
				return 0;
				break;
			}
			}
		}
	};
public:
	//calculate face geometry data at a certain level
	faceGeometry calcFaceGeometry(size_t level = -1) const;
private:
	//calculate face geometry data at the next grid level given pre-calculated geometry data at the current level
	static void stepupFaceGeometryFrom(size_t currentLevel, unsigned short nextFaceCode, faceGeometry& currentFaceGeometry);
	//calculate the interior angle of a face at a node (1..3) given pre-calculated geometry data as Sin(edge lengths)^2
	static fp_type interiorAngle(size_t level, unsigned short nodeCode, const faceGeometry&);
	//calculate the interior angle at node 3 of a face given pre-calculated geometry data (edge lengths as scaleFactor*Sin(E)^2)
	static fp_type interiorAngle(scaleExp_type scale, fp_type SinE1Sq, fp_type SinE2Sq, fp_type SinE3Sq);

	//calculate the area of a face given pre-calculated geometry data as Sin(edge lengths)^2
	static angle area(size_t level, const faceGeometry&);
	//calculate the area of a face given pre-calculated geometry
	//      output: 2^(2*level)*Sin(area/4)^2
	//      input: edge lengths L as 2^(2*(level+1))*Sin[L/2]^2)
	static fp_type sinSqQuarterArea(size_t level,
	                                fp_type FourSinE1HalfSq,
	                                fp_type FourSinE2HalfSq,
	                                fp_type FourSinE3HalfSq);

	/* auxiliary functions for special geometry related calculations */
	//calculate third edge length d in a spherical triangle, given side-angle-side
	//      return value is 2^scale*Sin(d/2); input lengths L as 2^(2*scale)*Sin(L)^2) and angle delta as Cos(delta)
	static fp_type calcSinEdgeHalfSAS(scaleExp_type scale, fp_type SinaSq, fp_type cosDelta, fp_type SinbSq);
	//calculate third edge length d in a spherical triangle, given side-angle-side (all edge lengths L as 2^scale*Sin(L)^2)
	//      return value is 2^scaleSq*Sin(d)^2, i.e., does not distinguish between d and pi - d
	//      input lengths L as 2^(scaleSq)*Sin(L)^2) and angle delta as Cos(delta)
	inline static fp_type calcSinSqEdgeSAS(scaleExp_type scaleSq, fp_type SinaSq, fp_type cosDelta, fp_type SinbSq)
	{
		return SinaSq + SinbSq - (cosDelta*cosDelta+1) * SinaSq*LDEXP(SinbSq,-scaleSq)
		       - 2*cosDelta * SQRT(SinaSq*SinbSq * (1-LDEXP(SinaSq,-scaleSq))*(1-LDEXP(SinbSq,-scaleSq)));
	};
	//for edge bisections: calculate 4*Sin(a/2)^2 from Sin(a)^2
	inline static fp_type calcEdgeBisection(scaleExp_type scaleSq, fp_type SinaSq)
	{
		return (2*SinaSq)/(1 + SQRT(1 - LDEXP(SinaSq,-scaleSq)));
	};






public:
	//convertion functions from and to polar coordinates
	sGrid(const sPolar& P, size_t level = accuracyBits);
	sPolar toPolar(size_t level, unsigned short location = 0) const;
	inline sPolar toPolar() const {
		return toPolar(depth(),0);
	};
	inline sPolar toPolar(unsigned short location) const {
		return toPolar(depth(),location);
	};

	//for a point at level within a face at refNodeLevel: returns distance to a node and angle to an edge
	sPolar toLocalPolar(
	    size_t refNodeLevel,
	    unsigned short refNodeCode,
	    unsigned short refEdgeCode,
	    size_t level,
	    unsigned short location = 0) const;
	inline sPolar toLocalPolar(size_t refNodeLevel, unsigned short nodeCode, unsigned short edgeCode) const
	{
		return toLocalPolar(refNodeLevel, nodeCode, edgeCode, depth(), 0);
	};
private:
	/* auxiliary functions used to calculate local polar coordinates inside a face */
	// data type for distance and direction data (local polar coordinates):
	//      distance d as Sin(d)^2*2^(2*level), angle in standard radians
public:
	struct inFacePolar
	{
		//distance between a point and a node (stored as as Sin(d)^2*2^(2*level))
		fp_type SinDistSq;
		//angle between an edge and the connection between a point and a node
		fp_type angle;
	};
	//calculate local polar coordinates of a point (pointLevel, location) with respect to nodeCode & edgeCode at refNodeLevel
	inFacePolar calcFacePolar(
	    size_t refNodeLevel,
	    unsigned short nodeCode,
	    unsigned short edgeCode,
	    size_t pointLevel,
	    unsigned short location = 0) const;
	//calculate local polar coordinates of a point (pointLevel, location) with respect to nodeCode & edgeCode at refNodeLevel, given pre-calculated geometry data
	inFacePolar calcFacePolar(
	    size_t refNodeLevel,
	    unsigned short nodeCode,
	    unsigned short edgeCode,
	    const faceGeometry&,
	    size_t pointLevel,
	    unsigned short location = 0) const;
	//flat geometry approximation:
	//calculate local polar coordinates of a point (pointLevel, location) with respect to nodeCode & edgeCode at refNodeLevel, given pre-calculated geometry data
	inFacePolar calcFlatFacePolar(
	    size_t refNodeLevel,
	    unsigned short nodeCode,
	    unsigned short edgeCode,
	    const faceGeometry&,
	    size_t pointLevel,
	    unsigned short location = 0) const;





public:
	/* functions to determine relations between multiple points on the sphere */
	//calculate the distance between two points: both can be node or center point, determined by location
	//for a distance d: return value is Sin(d/2), bool reference "mirror" is used to determine if it is the distance to the mirror point
	static scaledFP sinDistanceHalf(size_t level1, const sGrid& P1, unsigned short location1,
	                                size_t level2, const sGrid& P2, unsigned short location2,
	                                bool& mirror);
	//return value is the actual distance in [0,pi]
	static angle distance(size_t level1, const sGrid& P1, unsigned short location1,
	                      size_t level2, const sGrid& P2, unsigned short location2);
	//as above, but restricted to center points
	inline static angle distance(size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
	{
		return distance(level1, P1, 0, level2, P2, 0);
	}
	//as above, point level is the depth of the face
	inline static angle distance(const sGrid& P1, const sGrid& P2)
	{
		return distance(P1.depth(), P1, P2.depth(), P2);
	}

	class distanceToPolar
	{
		sPolar refPoint;

	public:
		inline distanceToPolar(const sPolar& P) : refPoint(P) {};
		inline scaledFP operator() (size_t level, const sGrid& P, unsigned short location)
		{
			return sPolar::distance(refPoint, P.toPolar(level, location));
		}
	};





private:
	//internal representation of the gridCode
	std::vector<bool> gridCode;

	//calculation accuracy (accuracyBits cannot be larger than digits of fp_type)
	static size_t accuracyBits;
	//if accuracy is A then the difference between flat and spherical triangles above level A/2 is negligible
	static size_t minDepthFlatApprox;
	//if accuracy is A then the difference Sin(x)-x for numbers below 2^(-A/3) is negligible
	static size_t minScaleSinXToX;






public:

template <typename gridFunction>
static void
searchMinPoint(
    size_t maxLevel,
    gridFunction minFunc,
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

		/*	    std::cout << *tmp << minFunc(1,*tmp,1) << " ";
			    std::cout << *tmp << minFunc(1,*tmp,2) << " ";
			    std::cout << *tmp << minFunc(1,*tmp,3);
		    	std::cout << std::endl;
		*/
	}
/*
	std::cout << "next level minimum: " << minPoint.first << minPoint.second << ": " << minFunc(1,minPoint.first,minPoint.second);
	std::cout << std::endl;
*/
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
/*
		std::cout << std::endl;
		std::cout << "level " << level_it+1 << " search result: [";
		std::cout << refMinFace;
		std::cout << "" << refMinLocation;
		std::cout << "]" << refMinValue;
		std::cout << " - " << refEdgeCode;
		std::cout << "," << refEdgeOrientationMatch;
		std::cout << " - " << refEdgeNeighborFace;
		std::cout << std::endl;
*/
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
/*
		std::cout << "next level minimum: " << minPoint.first << minPoint.second << ": " << minFunc(level_it+2,minPoint.first,minPoint.second);
		std::cout << std::endl;
*/
	}
	resultFace = refMinFace;
	resultLocation = refMinLocation;
	resultValue = refMinValue;
	return;
//	return {{refMinFace,refMinLocation},refMinValue};

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

#include <SPIRID_sGrid_searches.h>

};





} // namespace SPIRID

#endif //SPIRID_H_INCLUDED
