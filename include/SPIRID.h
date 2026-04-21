#ifndef SPIRID_H_INCLUDED
#define SPIRID_H_INCLUDED

#include <iostream>

#include <vector> // grid based coordinate system uses vector<bool> to label points
#include <list>   // for member function return values (like the list of neighbors)

#include <limits> //to get the accuracy of fp_type

#include <SPIRID_aux.h>

//#define fastSearch_ConsistencyCheckLimit 1e-6 //perform consistency checks within fastMinSearch algorithm
//#define hybridSearch_ConsistencyCheckLimit 1e-6 //perform consistency checks within hybridMinSearch algorithm

//SPIRID - SPherical CoordInate gRID
namespace SPIRID
{
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
	/******************** convertion functions from and to polar coordinates ********************/
	// convert sGrid coordinates into polar coordinates
	inline sPolar toPolar() const
	{
		return toPolar(depth(),0);
	};
	// convert sGrid coordinates into polar coordinates:
	// location==0 returns the face center polar coordinates
	// location in 1,2,3 returns the face node polar coordinates
	inline sPolar toPolar(unsigned short location) const
	{
		return toPolar(depth(),location);
	};
	// convert sGrid coordinates into polar coordinates: as above and allows to specify the grid level separately
	sPolar toPolar(size_t level, unsigned short location = 0) const;

	// calculate the distance of a point in sGrid coordinates to a fixed point in polar coordinates
	// functor is needed to calculate sGrid coordinates from polar coordiantes
	class polarDistanceToRef
	{
		sPolar refPoint;

	public:
		inline polarDistanceToRef(const sPolar& P) : refPoint(P) {};
		inline scaledFP operator() (size_t level, const sGrid& P, unsigned short location)
		{
			return sPolar::distance(refPoint, P.toPolar(level, location));
		}
	};
	// sGrid constructor from standard polar coordinates (theta, phi), theta in [0,pi], phi in [0,2pi)
	// optional arguments:
	// level - the depth (highest grid level) of the grid point
	// minSearchFunction: different methods can be used to find the sGrid point with best fitting polar coordiantes
	sGrid(const sPolar& P,
	      size_t level = accuracyBits,
	      sGrid (*minSearchFunction)(polarDistanceToRef, size_t) = fastMinSearch<polarDistanceToRef>);





	/******************** geometry calculations in sGrid coordinates for points on the unit sphere ********************/
	//function to calculate the distance between two points (in [0,pi])
	static scaledFP distance(size_t level1, const sGrid& P1, unsigned short location1,
	                         size_t level2, const sGrid& P2, unsigned short location2);
	//as above, but restricted to face center points
	inline static scaledFP distance(size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
	{
		return distance(level1, P1, 0, level2, P2, 0);
	}
	//as above, point level is the depth of the face
	inline static scaledFP distance(const sGrid& P1, const sGrid& P2)
	{
		return distance(P1.depth(), P1, P2.depth(), P2);
	}


private:
	//calculate the distance between two points: both can be node or center point, determined by location
	//for a distance d: return value is Sin(d/2), i.e. not scaled by grid level,
	// but an implicit grid level can be determined from the exponent of the return value
	//bool reference "mirror" becomes true when one of the point was mirrored for the calculation,
	// in that case the actual distance is d=pi-2*ASIN(return value)
	static scaledFP sinDistanceHalf(size_t level1, const sGrid& P1, unsigned short location1,
	                                size_t level2, const sGrid& P2, unsigned short location2,
	                                bool& mirror);





public:
	/******************** standard constructors & basic grid point property read-out and manipulation ********************/
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
	//constructor using a bit array (faceCodes can be represented by a bit array of size 3+2*depth)
	sGrid(const std::vector<bool>& bitCode);


	//depth of the gridCode & resize
	inline size_t depth() const
	{
		return (gridCode.size()-3)/2; //internally gridCode is a vector<bool> gridCode of length 3+2*depth --> make sure gridCode.size() always >=3
	};
	inline void resize(size_t newDepth)
	{
		gridCode.resize(3+2*newDepth);
	};
	//set all face codes in level and above to zero
	const sGrid& reset(size_t level);
	// reduce depth by cutting all tailing 0 (sGrid code with smaller depth still implicitly represents the same point)
	void trunc();


	//comparison operators
	inline bool operator == (const sGrid& P) const
	{
		return gridCode == P.gridCode;
	};
	inline bool operator != (const sGrid& P) const
	{
		return !(operator == (P));
	}


	// get the face code at a given level (returns 0 if level > depth)
	unsigned short at(size_t level) const;
	// set the face code at a given level, if depth<level resize to level first
	const sGrid& setExtend(size_t level, unsigned short faceCode);
private:
	//get & set face code: (0..7) at level 0, (0..3) at other levels (no checking of depth)
	unsigned short operator [] (size_t level) const;
	const sGrid& set(size_t level, unsigned short faceCode);



public:
	//mirror a face to the opposite side of the sphere
	inline sGrid& mirror()
	{
		gridCode[0]=!gridCode[0];
		gridCode[1]=!gridCode[1];
		gridCode[2]=!gridCode[2];
		return *this;
	};


	/* special points on the sphere (represented as pairs of <sGrid, nodeCode (0..3)> ) */
	static const std::pair<sGrid,unsigned short> NorthOct0; //Octant 0, nodeCode 3
	static const std::pair<sGrid,unsigned short> EastOct0; //Octant 0, nodeCode 1
	static const std::pair<sGrid,unsigned short> SouthOct7; //Octant 7, nodeCode 3
	static const std::pair<sGrid,unsigned short> WestOct7; //Octant 7, nodeCode 1





	/******************** calculate geometric properties of single grid elements ********************/
	/* Each point at every grid level corresponds to a
	   face (faceCodes 0..3) with edges and nodes (codes 1..3).
	   We can calculate edge lengths, interior angles and area for that face.
	*/
	//calculate the area of a face
	inline scaledFP area(size_t level) const
	{
		return area(level, calcFaceGeometry(level));
	};
	//calculate the interior angle of a face at a node (1..3)
	inline fp_type interiorAngle(size_t level, unsigned short nodeCode) const
	{
		return interiorAngle(level, nodeCode, calcFaceGeometry(level));
	};
	//get the orientation of a face
	inline signed short orientation() const
	{
		signed short fOrientation = -1;
		if (gridCode[0]) fOrientation *= -1;
		if (gridCode[1]) fOrientation *= -1;
		if (gridCode[2]) fOrientation *= -1;
		return fOrientation;
	}
	//calculate an edge length (code 1..3)
	scaledFP edgeLength(size_t level, unsigned short edgeCode) const;


	/******************** auxiliary functions used to calculate face geometries ********************/
	//data type used for face geometry data: lengths of the three edges E as Sin(E)^2*2^(2*level)
	struct faceGeometry
	{
		fp_type SinaSq;
		fp_type SinbSq;
		fp_type SincSq;

		inline fp_type operator[] (unsigned short edgeCode) const
		{
			switch (edgeCode)
			{
			case 1:
				return SinaSq;
			case 2:
				return SinbSq;
			case 3:
				return SincSq;
			default:
				return 0;
			}
		}
	};
	//calculate face geometry data at a certain level
	faceGeometry calcFaceGeometry(size_t level = -1) const;

	//calculate the interior angle of a face at a node (1..3) given pre-calculated geometry data as Sin(edge lengths)^2
	static fp_type interiorAngle(size_t level, unsigned short nodeCode, const faceGeometry&);
	//calculate the area of a face given pre-calculated geometry data as Sin(edge lengths)^2
	static scaledFP area(size_t level, const faceGeometry&);

private:
	//calculate face geometry data at the next grid level given pre-calculated geometry data at the current level
	static void stepupFaceGeometryFrom(size_t currentLevel, unsigned short nextFaceCode, faceGeometry& currentFaceGeometry);


	/************************* auxiliary functions with basic formulas used in geometry calculations *************************/
	//calculate Sin(a/2)^2 from Sin(a)^2: input 2^scaleSq*Sin(a)^2, output 2^(scaleSq+2)*Sin(a/2)^2 ( scaleSq=2*level)
	inline static fp_type calcSinSqLengthHalf(scaleExp_type scaleSq, fp_type SinaSq)
	{
		return (2*SinaSq)/(1 + SQRT(1 - LDEXP(SinaSq,-scaleSq)));
	};
	//calculate the interior angle at node 3 of a triangle with edge lengths E1..3, given as 2^scale*Sin(E)^2 (scale = 2*level )
	static fp_type interiorAngle_fromSinSqe(scaleExp_type scale,
	                                        fp_type SinE1Sq,
	                                        fp_type SinE2Sq,
	                                        fp_type SinE3Sq);
	//calculate the interior angle at node 3 of a triangle with edge lengths E1..3, given as 2^scale*Sin^2(E/2) (scale = 2*level+2 )
	static fp_type interiorAngle_fromSinSqeHalf(scaleExp_type scale,
	        fp_type SinSqE1Half,
	        fp_type SinSqE2Half,
	        fp_type SinSqE3Half);
	//calculate the area of a triangle given edge lengths
	//      output: 2^(2*level+4)*Sin(area/4)^2
	//      input: edge lengths L as 2^(2*(level+1))*Sin[L/2]^2)
	//      scale: 2*leve+2
	static fp_type sinSqQuarterArea(scaleExp_type scale,
	                                fp_type sinSqE1Half,
	                                fp_type sinSqE2Half,
	                                fp_type sinSqE3Half);
	//calculate third edge length d in a spherical triangle, given side-angle-side (all edge lengths L as 2^scaleSq*Sin(L)^2, scaleSq=2*level)
	//      return value is 2^scaleSq*Sin(d)^2, i.e., does not distinguish between d and pi - d
	//      input lengths L as 2^scaleSq*Sin(L)^2 and angle delta as Cos(delta)
	inline static fp_type calcSinSqEdgeSAS(scaleExp_type scaleSq,
	                                       fp_type SinaSq,
	                                       fp_type cosDelta,
	                                       fp_type SinbSq)
	{
		return SinaSq + SinbSq - (cosDelta*cosDelta+1) * SinaSq*LDEXP(SinbSq,-scaleSq)
		       - 2*cosDelta * SQRT(SinaSq*SinbSq * (1-LDEXP(SinaSq,-scaleSq))*(1-LDEXP(SinbSq,-scaleSq)));
	};
	//calculate third edge length d in a spherical triangle, given side-angle-side (all edge lengths L as 2^scaleSq*Sin(L/2)^2, scaleSq=2*level+2)
	//      return value is 2^scaleSq*Sin(d/2)^2
	//      input lengths L as 2^scaleSq*Sin(L/2)^2 and angle delta as Cos(delta)
	inline static fp_type calcSinSqEdgeHalfSAS(
	    scaleExp_type scaleSq, //scaleSq is 2*level+2
	    fp_type SinSqaHalf,
	    fp_type cosDelta,
	    fp_type SinSqbHalf)
	{
		return SinSqaHalf + SinSqbHalf - SinSqaHalf*LDEXP(SinSqbHalf,-scaleSq+1)
		       - 2*cosDelta*SQRT( SinSqaHalf*SinSqbHalf * (1-LDEXP(SinSqaHalf,-scaleSq))*(1-LDEXP(SinSqbHalf,-scaleSq)) );
	};
	//calculate third edge length d in a spherical triangle, given side-angle-side ( scaleSq=2*level+2 )
	//      return value is 2^(scaleSq/2)*Sin(d/2)
	//      input lengths L as 2^(scaleSq)*Sin(L/2)^2 and angle delta as Cos(delta)
	//near a==b and delta==0 then the result is dominated by rounding errors,
	//this function applies a modified calculation formula to minimize impact of rounding errors
	static fp_type calcSinEdgeHalfSAS(scaleExp_type scaleSq,
	                                  fp_type SinSqaHalf,
	                                  fp_type cosDelta,
	                                  fp_type SinSqbHalf);





public:
	/******************** calculate local polar coordinates inside a grid face ********************/
	//for a point at level within a face at refNodeLevel: returns distance to a node and angle to an edge
	sPolar toLocalPolar(
	    size_t refNodeLevel,
	    unsigned short refNodeCode,
	    unsigned short refEdgeCode,
	    size_t level,
	    unsigned short location = 0) const;
	//as above, with level=depth and location=0
	inline sPolar toLocalPolar(size_t refNodeLevel, unsigned short nodeCode, unsigned short edgeCode) const
	{
		return toLocalPolar(refNodeLevel, nodeCode, edgeCode, depth(), 0);
	};

private:
	/* auxiliary functions used to calculate local polar coordinates inside a face */
	// data type for distance and direction data (local polar coordinates):
	//      distance d as 2^(2*level+2)*Sin^2(d/2), angle in standard radians
	struct inFacePolar
	{
		//distance between a point and a node (stored as as Sin^2(d/2)*2^(2*level+2))
		fp_type SinSqdHalf;
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




private:
	/******************** auxiliary functions for the calculation of distances on the sphere in grid coordinates ********************/
	/* calculation of the distance between two points P1 & P2:
	    first step is to find out which is the highest grid level where the faces of P1 and P2 have a common node (refNode)
	        the grid level can be different for P1 & P2 (if one of the points is closer to the refNode)
	    then we calculate the distance of P1 & P2 to this refNode and the angle between the respective connections
	    finally we use the spherical cosine law to calculate the distance
	*/
	//data type used for information about the refNode and the angle between the corresponding faces
	struct pointPairRefNode
	{
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

		// {0,0,0, 0,0,0, 0,0} means no common node in any grid level --> will check again a mirrored point

		//output pointPairRefNode data
		std::ostream& print(std::ostream&);
	};

	//main function to get reference node data (including gapAngle):
	// this is independent on whether we refer to the center point or a node of the face
	static pointPairRefNode findHighestRefNode(size_t level1, const sGrid& P1, size_t level2, const sGrid& P2);
	//as above, point level is the depth of the face
	static pointPairRefNode findHighestRefNode(const sGrid& P1, const sGrid& P2)
	{
		return findHighestRefNode(P1.depth(), P1, P2.depth(), P2);
	};

	//auxliary function for pointPairRefNode search:
	// used when P1 and P2 are in the same face at all lower levels (level-1)
	static pointPairRefNode sameFaceStepupTo(size_t level, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2);
	//auxliary function for pointPairRefNode search:
	// used when the faces of P1 and P2 at level-1 share a common edge
	static pointPairRefNode commonEdgeStepupTo(size_t level, unsigned short edgeCode, bool orientationMatch, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2);
	//auxliary function for pointPairRefNode search:
	// assuming faceCode at startlevel-1 equals nodeCode: until which level nodeCode repeats?
	inline static size_t lastLevelAtNode(size_t startLevel, unsigned short nodeCode, size_t endLevel, const sGrid& P)
	{
		while (P.at(startLevel)==nodeCode && startLevel<=endLevel)
		{
			startLevel++;
		};
		return startLevel-1;
	};





private:
	/******************** actual code of a grid point and calculation accuracy settings ********************/
	//internal representation of the gridCode
	std::vector<bool> gridCode;

	//calculation accuracy (accuracyBits cannot be larger than digits of fp_type)
	static size_t accuracyBits;
	//if accuracy is A then the difference between flat and spherical triangles above level A/2 is negligible
	static size_t minDepthFlatApprox;
	//if accuracy is A then the difference Sin(x)-x for numbers below 2^(-A/3) is negligible
	static size_t minScaleSinXToX;

public:
	//calculation accuracy (accuracyBits not larger than digits of fp_type)
	static void setAccuracyBits(size_t = std::numeric_limits<fp_type>::digits);
	inline static size_t getAccuracyBits()
	{
		return accuracyBits;
	};





public:
	/****************************** grid topology related functions ******************************/
	//stepping to the neighbor face (across a single edge)
	inline sGrid neighborFace(unsigned short edgeCode) const
	{
	    return neighborFace(depth(), edgeCode);
	};
	//as above, includes check whether neighbor face has the same orientation
	inline sGrid neighborFace(unsigned short edgeCode, bool& orientationMatch) const
	{
	    return neighborFace(depth(), edgeCode, orientationMatch);
	};
	//stepping to the neighbor face (across a single edge)
	sGrid neighborFace(size_t level, unsigned short edgeCode) const;
	//as above, includes check whether neighbor face has the same orientation
	sGrid neighborFace(size_t level, unsigned short edgeCode, bool& orientationMatch) const;
private:
	//assign the neighbor of the current face to an existing sGrid object, assuming its depth is already at level
	static sGrid& assignNeighborFace(size_t level, unsigned short edgeCode, sGrid& neighbor, bool& orientationMatch);


public:
	// given two nodes with code1 != code2 in {1,2,3} it is sometimes useful to get the third remaining code
	inline static unsigned short otherNodeCode(unsigned short code1, unsigned short code2)
	{
		return 3 - ((code1 + code2) % 3);
	};
private:
	// auxiliary functions to get new node, edge & face codes (independent of the grid)
	// code -> 1+code%3
	inline static unsigned short nextNode(unsigned short code)
	{
		return newNodeCodes[code];
	};
	// code -> (code+1)%4
	inline static unsigned short nextFace(unsigned short code)
	{
		return newFaceCodes[code];
	};
	// given two or three nodes, edges or faces with code1 != code2 != code3 != code1 in {0,1,2,3}
	// get a new code in {1,2,3} or {0,1,2,3}
	inline static unsigned short newCode(unsigned short code1, unsigned short code2, unsigned short code3 = 0)
	{
		return newCodes[code1+code2+code3];
	};
	/* constant arrays used in auxiliary functions above */
	static const unsigned short newNodeCodes[4];
	static const unsigned short newFaceCodes[4];
	static const unsigned short newCodes[7];



public:
	// get the list of (three) neighbors of a given face
	std::list<sGrid> neighborFaces(size_t level) const;
	// get the list of (two) neighbor faces for a given edge
	std::list<sGrid> edgeNeighborFaces(size_t level, unsigned short edgeCode) const;
	// get the list of (four or six) neighbor faces for a given node
	std::list<sGrid> nodeNeighborFaces(size_t level, unsigned short nodeCode) const;
	// nodes and edges in the grid are characterized by a pair of (sGrid, code 1..3)
	// get a list of nodes (four or six) which are directly connected by edges at level to a given center node
	std::list<std::pair<sGrid, unsigned short> > nodeNeighborNodes(size_t level, unsigned short nodeCode) const;
	// get a list of edges (four or six) which connect to a given center node at level
	std::list<std::pair<sGrid, unsigned short> > nodeConnectedEdges(size_t level, unsigned short nodeCode) const;
	// get a list of edges (four or six) which at level connect the nodeNeighborNodes above among each other
	std::list<std::pair<sGrid, unsigned short> > nodeOuterRingEdges(size_t level, unsigned short nodeCode) const;



	// subGridScanner can be used to similar as an iterator
	// to scan through all sub-faces at scannerLevel in a given list of faces at gridMinLevel
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
		void reset();

		subGridScanner& operator = (const subGridScanner& X);
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





public:
	/******************** grid minimum search algorithms and auxiliary functions ********************/

	// for a fixed face, get the node where the function becomes minimal, return value is the minimum funtion value
	template<typename gridFunction>
	static scaledFP minNodeInFace(
	    gridFunction minFunc, // function to minimize
	    size_t level, // grid level of the search
	    const sGrid& minFace, // the face where to find the minimum node
	    unsigned short& minNode // input not relevant, output: minimum node code
	)
	{
		scaledFP minValue = minFunc(level, minFace, 1);
		minNode = 1;
		scaledFP dummyValue = minFunc(level, minFace, 2);
		if (dummyValue < minValue)
		{
			minValue = dummyValue;
			minNode = 2;
		}
		dummyValue = minFunc(level, minFace, 3);
		if (dummyValue < minValue)
		{
			minValue = dummyValue;
			minNode = 3;
		}
		return minValue;
	}
	// for a fixed node: return the adjacent face with minimal function value at the face center
	template<typename gridFunction>
	static sGrid minNeighborFace(
	    gridFunction minFunc, // function to minimize
	    size_t level, // grid level of the search
	    const sGrid& minFace, // the face where to find the minimum node
	    unsigned short minNode, // input not relevant, output: minimum node code
	    scaledFP& minValue
	)
	{
		std::list<sGrid> neighborFaces(minFace.nodeNeighborFaces(level,minNode));

		std::list<sGrid>::iterator it = neighborFaces.begin();
		std::list<sGrid>::iterator it_end = neighborFaces.end();
		std::list<sGrid>::iterator min_it = it;
		scaledFP dummyValue = minFunc(level, *it, 0);
		minValue = dummyValue;
		++it;
		for (; it != it_end; ++it)
		{
			dummyValue = minFunc(level, *it, 0);
			if (dummyValue < minValue)
			{
				min_it = it;
				minValue = dummyValue;
			}
		}
		return *min_it;

	}


	/******************** robust grid search algorithm and auxiliary functions ********************/
	// search for the face at grid level maxLevel where the function minFunc becomes minimal
	// this procedure returns the face with minimal center value, but not the function value
	template<typename gridFunction>
	static sGrid minSearch(
	    gridFunction minFunc, // function to minimize
	    size_t maxLevel = accuracyBits // highest grid level of the search
	)
	{
		scaledFP minValue;

		return minSearch(minFunc, maxLevel, minValue);
	}
	// search for the face at grid level maxLevel where the function minFunc becomes minimal
	// this procedure returns the face with minimal center value, and the function value at this location through minValue
	template<typename gridFunction>
	static sGrid minSearch(
	    gridFunction minFunc, // function to minimize
	    size_t maxLevel, // highest grid level of the search
	    scaledFP& minValue // input irrelevant, output: minimum face center value found
	)
	{
		sGrid searchFace;
		unsigned short searchLocation;
		minNodeSearch(minFunc, maxLevel, searchFace, searchLocation);

		return minNeighborFace(minFunc, maxLevel, searchFace, searchLocation, minValue);
	}
	// search for the node at grid level maxLevel where the function minFunc becomes minimal
	// this procedure returns function value at that node and corresponding face & node code through minFace & minNode
	template<typename gridFunction>
	static scaledFP minNodeSearch(
	    gridFunction minFunc, // function to minimize
	    size_t maxLevel, // highest grid level of the search
	    sGrid& minFace, // input not relevant, output: the minimum face
	    unsigned short& minNode // input not relevant, output: the minimum node
	)
	{
		minFace = NorthOct0.first;
		minNode = 3;
		scaledFP minValue = minFunc(0, minFace, minNode);

		scaledFP dummyValue = minFunc(0, minFace, 2);
		if (dummyValue < minValue)
		{
			minValue = dummyValue;
			minNode = 2;
		}
		dummyValue = minFunc(0, minFace, 1);
		if (dummyValue < minValue)
		{
			minValue = dummyValue;
			minNode = 1;
		}

		dummyValue = minFunc(0, SouthOct7.first, 3);
		if (dummyValue < minValue)
		{
			minValue = dummyValue;
			minNode = 3;
			minFace = SouthOct7.first;
		}
		dummyValue = minFunc(0, SouthOct7.first, 2);
		if (dummyValue < minValue)
		{
			minValue = dummyValue;
			minNode = 2;
			minFace = SouthOct7.first;
		}
		dummyValue = minFunc(0, SouthOct7.first, 1);
		if (dummyValue < minValue)
		{
			minValue = dummyValue;
			minNode = 1;
			minFace = SouthOct7.first;
		}

		for (size_t level_it = 0; level_it < maxLevel; ++level_it)
		{
			minNodeSearchAux_StepToNextLevel(minFunc, level_it, minFace, minNode, minValue);
		}

		return minValue;
	}
	// search minimum node at maxLevel, around starting point defined by (startLevel, minFace, minLocation)
	template<typename gridFunction>
	static scaledFP minNodeSearchAux_LocalMinNode(
	    gridFunction minFunc, // function to minimize
	    size_t startLevel, // grid level where to start the search
	    size_t maxLevel, // highest grid level of the search
	    sGrid& minFace, // input: starting face of the search, output: minimum face
	    unsigned short& minLocation // input: starting location (node or face center) of the search, output: minimum node code
	)
	{
		scaledFP minValue;
		if (minLocation < 1 || minLocation > 3) minValue = minNodeSearchAux_InFaceMinNode(minFunc, startLevel, minFace, minLocation);
		else minValue = minFunc(startLevel, minFace, minLocation);

		for (size_t level_it = startLevel; level_it < maxLevel; ++level_it)
		{
			minNodeSearchAux_StepToNextLevel(minFunc, level_it, minFace, minLocation, minValue);
		}

		return minValue;
	}
private:
	// search minimum node auxiliary function:
	// for a minimum node at level (minFace, minNode, minValue) search within the neighbor faces for the the minimum node in the next level
	template<typename gridFunction>
	static void minNodeSearchAux_StepToNextLevel(
	    gridFunction minFunc, // function to minimize
	    size_t level, // old (lower) grid level of the search
	    sGrid& minFace, // input: starting face of the search, output: minimum face
	    unsigned short& minNode, // input: starting node of the search, output: minimum node code
	    scaledFP& minValue // input: function value at the starting node, output: minimum node value at the next level
	)
	{
		scaledFP dummyValue = minValue;

		std::list<std::pair<sGrid, unsigned short> > outerRing(minFace.nodeOuterRingEdges(level,minNode));
		level++;
		minFace.setExtend(level,minNode);
		std::list<std::pair<sGrid, unsigned short> > innerRing(minFace.nodeNeighborNodes(level,minNode));

		std::list<std::pair<sGrid, unsigned short> >::iterator it = outerRing.begin();
		std::list<std::pair<sGrid, unsigned short> >::iterator end = outerRing.end();
		for (; it != end; ++it)
		{
			it -> first.setExtend(level,0);
			dummyValue = minFunc(level,it->first,it->second);
			if (dummyValue < minValue)
			{
				minValue = dummyValue;
				minFace = it -> first;
				minNode = it -> second;
			}
		}

		it = innerRing.begin();
		end = innerRing.end();
		for (; it != end; ++it)
		{
			dummyValue = minFunc(level,it->first,it->second);
			if (dummyValue < minValue)
			{
				minValue = dummyValue;
				minFace = it -> first;
				minNode = it -> second;
			}
		}

		return;
	}



public:
	/******************** fast grid search algorithm and auxiliary functions ********************/
	/* reduces number of function evaluations by 50%, but has higher risk of not finding the real minimum */

	// search for the face at grid level maxLevel where the function minFunc becomes minimal
	// this procedure returns the face with minimal center value, but not the function value
	template<typename gridFunction>
	static sGrid fastMinSearch(
	    gridFunction minFunc, // function to minimize
	    size_t maxLevel = accuracyBits // highest grid level of the search
	)
	{
		scaledFP minValue;
		return fastMinSearch(minFunc, maxLevel, minValue);
	}
	// search for the face at grid level maxLevel where the function minFunc becomes minimal
	// this procedure returns the face with minimal center value, and the function value at this location through minValue
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

		scaledFP dummyValue = minFunc(0, SouthOct7.first, SouthOct7.second);
		if (dummyValue < minValue)
		{
			std::swap(minValue, dummyValue);
			minFace = SouthOct7.first;
			minNode = SouthOct7.second;
		}

		unsigned short otherNodeCode = nextNode(minNode);
		unsigned short neighborEdgeCode = nextNode(otherNodeCode);
		bool neighborOrientationMatch = true;
		sGrid minFaceNeighbor = minFace.neighborFace(0, neighborEdgeCode, neighborOrientationMatch);

		fastMinEdgeSearch(
		    minFunc, 0, maxLevel,
		    minFace, minNode, minValue,
		    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch, otherNodeCode);

		return minNeighborFace(minFunc, maxLevel, minFace, minNode, minValue);

		/* checking only the two edge neighbor faces seems not enough to get the real minimum face, especially at lower grid levels

		minValue = minFunc(maxLevel, minFace, 0);
		dummyValue = minFunc(maxLevel, minFaceNeighbor, 0);

		if (dummyValue < minValue)
		{
			minValue = dummyValue;
			minFace = minFaceNeighbor;
		}

		return minFace;
		*/
	}
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

		minValue = minFunc(startLevel, minFace, minLocation);

		if (neighborEdgeCode < 1 || neighborEdgeCode > 3 || neighborEdgeCode == minLocation) neighborEdgeCode = nextNode(minLocation);

		sGrid minFaceNeighbor;
		bool neighborOrientationMatch;
		unsigned short otherNodeCode;

		fastMinEdgeSearch(
		    minFunc, startLevel, maxLevel,
		    minFace, minLocation, minValue,
		    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch, otherNodeCode);
	}
private:
	// fast minimum edge search, starting at a defined NODE, returning the minimum nearby edge
	// no checks of consistent node and edge codes are done here
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

		fastMinEdgeSearchAux(
		    minFunc, startLevel, maxLevel,
		    minFace, minLocation, minValue,
		    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
		    otherNodeCode);
	}
	// fast minimum edge search auxiliary function:
	// initialize first proper minimum edge and step through the grid levels
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
	// fast minimum edge search auxiliary function:
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

		unsigned short minNodeRight = otherNodeCode;
		unsigned short otherNodeRight = minNode;
		if (!neighborOrientationMatch) std::swap(minNodeRight, otherNodeRight);

#ifdef fastSearch_ConsistencyCheckLimit

		// consistency checks
		scaledFP funcValueLimit = scaledFP(fastSearch_ConsistencyCheckLimit,0);
		if (minNode == otherNodeCode || otherNodeCode == neighborEdgeCode || neighborEdgeCode == minNode ||
		        minNode < 1  || otherNodeCode < 1 || neighborEdgeCode < 1 ||
		        minNode > 3  || otherNodeCode > 3 || neighborEdgeCode > 3)
		{
			std::cerr << "fast grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - node codes not as expected: (" << minNode << "," << otherNodeCode << "," << neighborEdgeCode << ")" << std::endl;
		}
		bool testEdgeOrientationMatched = true;
		sGrid testFace = minFace.neighborFace(nextLevel-1, neighborEdgeCode, testEdgeOrientationMatched);
		if (minFaceNeighbor != testFace)
		{
			std::cerr << "fast grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - minFaceNeighbor not matched";
			minFaceNeighbor.print(std::cerr);
			testFace.print(std::cerr);
			std::cerr << std::endl;
		}
		if (testEdgeOrientationMatched != neighborOrientationMatch)
		{
			std::cerr << "fast grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - minFaceNeighbor edge orientation not as expected";
			std::cerr << " input: " << neighborOrientationMatch;
			std::cerr << " calculation: " << testEdgeOrientationMatched << " ";
			minFace.print(std::cerr);
			minFaceNeighbor.print(std::cerr);
			std::cerr << std::endl;
		}
		scaledFP testValue = minFunc(nextLevel-1, minFace, minNode);
		if ((minValue - testValue).abs() > funcValueLimit)
		{
			std::cerr << "fast grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - minimum value not as expected: " << minValue << "-" << testValue;
			std::cerr << " difference: " << minValue-testValue;
			std::cerr << " relative difference: " << (minValue - testValue)/(minValue + testValue) << std::endl;
		}
		testValue = minFunc(nextLevel-1, minFace, otherNodeCode);
		if ((otherNodeValue - testValue).abs() > funcValueLimit)
		{
			std::cerr << "fast grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - other node value not as expected: " << otherNodeValue << "-" << testValue;
			std::cerr << " difference: " << otherNodeValue-testValue;
			std::cerr << " relative difference: " << (otherNodeValue - testValue)/(otherNodeValue + testValue) << std::endl;
		}
		if (otherNodeValue < minValue)
		{
			std::cerr << "fast grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - otherNodeValue < minValue: " << otherNodeValue << "<" << minValue << std::endl;
		}
		testValue = minFunc(nextLevel-1, minFaceNeighbor, minNodeRight);
		if ((minValue - testValue).abs() > funcValueLimit)
		{
			std::cerr << "fast grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - min node value in neighborface not as expected: " << minValue << "-" << testValue;
			std::cerr << " difference: " << minValue-testValue;
			std::cerr << " relative difference: " << (minValue - testValue)/(minValue + testValue) << std::endl;
		}
		testValue = minFunc(nextLevel-1, minFaceNeighbor, otherNodeRight);
		if ((otherNodeValue - testValue).abs() > funcValueLimit)
		{
			std::cerr << "fast grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - other node value in neighborface not as expected: " << otherNodeValue << "-" << testValue;
			std::cerr << " difference: " << otherNodeValue-testValue;
			std::cerr << " relative difference: " << (otherNodeValue - testValue)/(otherNodeValue + testValue) << std::endl;
		}

#endif //fastSearch_ConsistencyCheckLimit

		minFace.setExtend(nextLevel, minNode);
		scaledFP centerNodeValue = minFunc(nextLevel, minFace, otherNodeCode);

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
			minFace.set(nextLevel, 0);
			minFaceNeighbor.setExtend(nextLevel, 0);

			fastMinEdgeSearchAux_MidEdgeSmallerCase(
			    minFunc, nextLevel,
			    minFace, minNode, minValue,
			    otherNodeCode, otherNodeValue, centerNodeValue,
			    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
			    minNodeRight, otherNodeRight
			);
		}
	}
	// fast minimum edge search auxiliary function:
	// search for function minimum within nodes around lower level minimum edge end point
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
		scaledFP outerNode5Value = minFunc(level, minFaceNeighbor, nodeCode2); // note that this is outerNode #3 in case of only four adjacent faces

		/* variables for minimum node inside outer ring */
		scaledFP outerRingMinValue = otherNodeValue;
		unsigned short outerRingMinIndex = 6;

		if (outerNode1Value < outerRingMinValue)
		{
			outerRingMinIndex = 1;
			outerRingMinValue = outerNode1Value;
		}
		if (outerNode2Value < outerRingMinValue)
		{
			outerRingMinIndex = 2;
			outerRingMinValue = outerNode2Value;
		}
		if (outerNode5Value < outerRingMinValue)
		{
			outerRingMinIndex = 5;
			outerRingMinValue = outerNode5Value;
		}

		/* check the case of only four adjacent faces first */
		if (!(neighborOrientationMatch || edgeAOrientationMatch))
		{
			switch (outerRingMinIndex)
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

					fastMinEdgeSearchAux_LeftOuterNodeCheck(
					    level,
					    minFace, minNode, minValue,
					    otherNodeCode, otherNodeValue,
					    neighborEdgeCode, outerNode2Value,
					    minFaceNeighbor, neighborOrientationMatch);
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
				//otherNodeCode = nodeCode1; -> no change

				if (outerNode1Value < outerNode5Value)
				{
					minFace = leftFaceA;
					minFaceNeighbor = leftFaceA.neighborFace(level, neighborEdgeCode);

					fastMinEdgeSearchAux_LeftOuterNodeCheck(
					    level,
					    minFace, minNode, minValue,
					    otherNodeCode, otherNodeValue,
					    neighborEdgeCode, outerNode1Value,
					    minFaceNeighbor, neighborOrientationMatch);
				}
				else
				{
					minFaceNeighbor = leftFaceA;
					minFace = leftFaceA.neighborFace(level, neighborEdgeCode);

					fastMinEdgeSearchAux_LeftOuterNodeCheck(
					    level,
					    minFace, minNode, minValue,
					    otherNodeCode, otherNodeValue,
					    neighborEdgeCode, outerNode5Value,
					    minFaceNeighbor, neighborOrientationMatch);
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

					fastMinEdgeSearchAux_LeftOuterNodeCheck(
					    level,
					    minFace, minNode, minValue,
					    otherNodeCode, otherNodeValue,
					    neighborEdgeCode, outerNode2Value,
					    minFaceNeighbor, neighborOrientationMatch);
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
			outerRingMinIndex = 3;
			outerRingMinValue = outerNode3Value;
		}
		if (outerNode4Value < outerRingMinValue)
		{
			outerRingMinIndex = 4;
			outerRingMinValue = outerNode4Value;
		}

		switch (outerRingMinIndex)
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
					//minNode = nodeCode0; -> no change
					otherNodeCode = nodeCode2;
				}

				minFaceNeighbor = minFace;
				minFace = leftFaceA;

				fastMinEdgeSearchAux_LeftOuterNodeCheck(
				    level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode2Value,
				    minFaceNeighbor, neighborOrientationMatch);
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

					// minNode = nodeCode0; -> no change
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
				//otherNodeCode = nodeCode1; -> no change

				fastMinEdgeSearchAux_LeftOuterNodeCheck(
				    level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode1Value,
				    minFaceNeighbor, neighborOrientationMatch);
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
					//otherNodeCode = nodeCode1; -> no change
				}

				fastMinEdgeSearchAux_LeftOuterNodeCheck(
				    level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode3Value,
				    minFaceNeighbor, neighborOrientationMatch);
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

				fastMinEdgeSearchAux_LeftOuterNodeCheck(
				    level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode2Value,
				    minFaceNeighbor, neighborOrientationMatch);
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
						neighborEdgeCode = minNodeRight;
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

				fastMinEdgeSearchAux_LeftOuterNodeCheck(
				    level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode4Value,
				    minFaceNeighbor, neighborOrientationMatch);
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

				fastMinEdgeSearchAux_LeftOuterNodeCheck(
				    level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode3Value,
				    minFaceNeighbor, neighborOrientationMatch);
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

				fastMinEdgeSearchAux_LeftOuterNodeCheck(
				    level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode5Value,
				    minFaceNeighbor, neighborOrientationMatch);
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

				fastMinEdgeSearchAux_LeftOuterNodeCheck(
				    level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode4Value,
				    minFaceNeighbor, neighborOrientationMatch);
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
	// fast  minimum edge search auxiliary function:
	// search for function minimum within nodes around lower level minimum edge mid point
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
		//assume the face code at level for minFace and minFaceNeighbor is already set to 0
		//minFace.set(level,0);
		//minFaceNeighbor.setExtend(level,0);

		unsigned short nodeCode0 = otherNodeCode;
		unsigned short nodeCode1 = minNode;
		unsigned short nodeCode2 = neighborEdgeCode;

		scaledFP leftCenterFaceFValueA = minFunc(level,minFace,nodeCode0);
		scaledFP leftCenterFaceFValueB = minFunc(level,minFace,nodeCode1);
		scaledFP rightCenterFaceFValueA = minFunc(level,minFaceNeighbor,otherNodeRight);
		scaledFP rightCenterFaceFValueB = minFunc(level,minFaceNeighbor,minNodeRight);

		// outer ring minimum search:
		// edges connecting to the other node of the level-1 minimum edge cannot become the new minmum edge
		unsigned short outerRingMinIndex = 6; // this is the case where the level-1 minimum node is the outer ring minimum
		scaledFP outerRingMinValue = minValue;
		if (leftCenterFaceFValueA < outerRingMinValue)
		{
			outerRingMinIndex = 1;
			outerRingMinValue = leftCenterFaceFValueA;
		}
		if (leftCenterFaceFValueB < outerRingMinValue)
		{
			outerRingMinIndex = 2;
			outerRingMinValue = leftCenterFaceFValueB;
		}
		if (rightCenterFaceFValueA < outerRingMinValue)
		{
			outerRingMinIndex = 5;
			outerRingMinValue = rightCenterFaceFValueA;
		}
		if (rightCenterFaceFValueB < outerRingMinValue)
		{
			outerRingMinIndex = 4;
			outerRingMinValue = rightCenterFaceFValueB;
		}

		switch (outerRingMinIndex)
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
	/* fast minimum edge search auxiliary function:
	    starting from a face, minFace, where one node was a center node during the search (minNode),
	    and the 2nd node has the smallest function value inside the outer ring (at level) of this center node:
	    re-arrange correct minimum nodes, edges and faces such that on output minNode is the final (local) minimum
	    assumption: 3rd function value inside minFace < 3rd function value inside minFaceNeighbor
	*/
	static void fastMinEdgeSearchAux_LeftOuterNodeCheck(
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

		if (outerNodeMinValue < centerValue) // outer ring value is the minimum
		{
			unsigned short minNode = outerMinNode;

			if (leftOuterValue < centerValue) // outer edge is the actual the minimum edge
			{
				//for output: centerNode -> minNode, outerMinNode -> leftOuterNode, neighborEdgeCode(==leftOuterNode) --> centerNode
				outerMinNode = leftOuterNode;
				leftOuterNode = centerNode;
				centerNode = minNode;

				centerValue = outerNodeMinValue;
				outerNodeMinValue = leftOuterValue;

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
		return;
	}



public:
	/******************** hybrid grid search algorithm and auxiliary functions ********************/
	/* reduces number of function evaluations compared to standard minSearch and is more robust than fastMinSearch */

	// search for the face at grid level maxLevel where the function minFunc becomes minimal
	// this procedure returns the face with minimal center value, but not the function value
	template<typename gridFunction>
	static sGrid hybridMinSearch(
	    gridFunction minFunc, // function to minimize
	    size_t maxLevel = accuracyBits // highest grid level of the search
	)
	{
		scaledFP minValue;
		return hybridMinSearch(minFunc, maxLevel, minValue);
	}
	// search for the face at grid level maxLevel where the function minFunc becomes minimal
	// this procedure returns the face with minimal center value, and the function value at this location through minValue
	template<typename gridFunction>
	static sGrid hybridMinSearch(
	    gridFunction minFunc, // function to minimize
	    size_t maxLevel, // highest grid level of the search
	    scaledFP& minValue // only output: minimum face value found
	)
	{
		sGrid minFace = NorthOct0.first;
		unsigned short minNode = NorthOct0.second;
		minValue = minFunc(0, minFace, minNode);

		scaledFP dummyValue = minFunc(0, SouthOct7.first, SouthOct7.second);
		if (dummyValue < minValue)
		{
			minValue = dummyValue;
			minFace = SouthOct7.first;
			minNode = SouthOct7.second;
		}

		unsigned short otherNodeCode = nextNode(minNode);
		unsigned short neighborEdgeCode = nextNode(otherNodeCode);
		bool neighborOrientationMatch = true;
		sGrid minFaceNeighbor = minFace.neighborFace(0, neighborEdgeCode, neighborOrientationMatch);

		hybridMinEdgeSearch(
		    minFunc, 0, maxLevel,
		    minFace, minNode, minValue,
		    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch, otherNodeCode);

		return minNeighborFace(minFunc, maxLevel, minFace, minNode, minValue);
	}
	// minimum edge search, starting at a defined location (node or face center), returning the minimum nearby edge
	template<typename gridFunction>
	static void hybridMinEdgeSearch(
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

		minValue = minFunc(startLevel, minFace, minLocation);

		if (neighborEdgeCode < 1 || neighborEdgeCode > 3 || neighborEdgeCode == minLocation) neighborEdgeCode = nextNode(minLocation);

		sGrid minFaceNeighbor;
		bool neighborOrientationMatch;
		unsigned short otherNodeCode;

		hybridMinEdgeSearch(
		    minFunc, startLevel, maxLevel,
		    minFace, minLocation, minValue,
		    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch, otherNodeCode);
	}
private:
	// hybrid minimum edge search, starting at a defined NODE, returning the minimum nearby edge
	// no checks of consistent node and edge codes are done here
	template<typename gridFunction>
	static void hybridMinEdgeSearch(
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

		hybridMinEdgeSearchAux(
		    minFunc, startLevel, maxLevel,
		    minFace, minLocation, minValue,
		    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
		    otherNodeCode);
	}
	// hybrid minimum edge search auxiliary function:
	// initialize first proper minimum edge and step through the grid levels
	template<typename gridFunction>
	static void hybridMinEdgeSearchAux(
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

		hybridMinEdgeSearchAux_EdgeEndSmallerCase(
		    minFunc, startLevel,
		    minFace, minNode, minValue,
		    otherNodeCode, otherNodeValue,
		    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
		    minNodeRight, otherNodeRight);

		for (size_t level_it = 0; level_it < maxLevel; )
		{
			++level_it;
			hybridMinEdgeSearchAux_StepToNextLevel(
			    minFunc, level_it,
			    minFace, minNode, minValue,
			    otherNodeCode, otherNodeValue,
			    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch);
		}
	}
	// hybrid minimum edge search auxiliary function:
	// check whether function value is smaller at the edge end or mid point and search new minimum edge around there
	template<typename gridFunction>
	static void hybridMinEdgeSearchAux_StepToNextLevel(
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

		unsigned short minNodeRight = otherNodeCode;
		unsigned short otherNodeRight = minNode;
		if (!neighborOrientationMatch) std::swap(minNodeRight, otherNodeRight);

#ifdef hybridSearch_ConsistencyCheckLimit

		// consistency checks
		scaledFP funcValueLimit = scaledFP(hybridSearch_ConsistencyCheckLimit,0);
		if (minNode == otherNodeCode || otherNodeCode == neighborEdgeCode || neighborEdgeCode == minNode ||
		        minNode < 1  || otherNodeCode < 1 || neighborEdgeCode < 1 ||
		        minNode > 3  || otherNodeCode > 3 || neighborEdgeCode > 3)
		{
			std::cerr << "hybrid grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - node codes not as expected: (" << minNode << "," << otherNodeCode << "," << neighborEdgeCode << ")" << std::endl;
		}
		bool testEdgeOrientationMatched = true;
		sGrid testFace = minFace.neighborFace(nextLevel-1, neighborEdgeCode, testEdgeOrientationMatched);
		if (minFaceNeighbor != testFace)
		{
			std::cerr << "hybrid grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - minFaceNeighbor not matched";
			minFaceNeighbor.print(std::cerr);
			testFace.print(std::cerr);
			std::cerr << std::endl;
		}
		if (testEdgeOrientationMatched != neighborOrientationMatch)
		{
			std::cerr << "hybrid grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - minFaceNeighbor edge orientation not as expected";
			std::cerr << " input: " << neighborOrientationMatch;
			std::cerr << " calculation: " << testEdgeOrientationMatched << " ";
			minFace.print(std::cerr);
			minFaceNeighbor.print(std::cerr);
			std::cerr << std::endl;
		}
		scaledFP testValue = minFunc(nextLevel-1, minFace, minNode);
		if ((minValue - testValue).abs() > funcValueLimit)
		{
			std::cerr << "hybrid grid search inconsistency, ";
			minFace.print(std::cout);
			std::cerr << minNode << " level " << nextLevel-1;
			std::cerr << " - minimum value not as expected: " << minValue << "-" << testValue;
			std::cerr << " difference: " << minValue-testValue;
			std::cerr << " relative difference: " << (minValue - testValue)/(minValue + testValue) << std::endl;
		}
		testValue = minFunc(nextLevel-1, minFace, otherNodeCode);
		if ((otherNodeValue - testValue).abs() > funcValueLimit)
		{
			std::cerr << "hybrid grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - other node value not as expected: " << otherNodeValue << "-" << testValue;
			std::cerr << " difference: " << otherNodeValue-testValue;
			std::cerr << " relative difference: " << (otherNodeValue - testValue)/(otherNodeValue + testValue) << std::endl;
		}
		if (otherNodeValue < minValue)
		{
			std::cerr << "hybrid grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - otherNodeValue < minValue: " << otherNodeValue << "<" << minValue << std::endl;
		}
		testValue = minFunc(nextLevel-1, minFaceNeighbor, minNodeRight);
		if ((minValue - testValue).abs() > funcValueLimit)
		{
			std::cerr << "hybrid grid search inconsistency, ";
			minFaceNeighbor.print(std::cout);
			std::cerr << minNodeRight << " level " << nextLevel-1;
			std::cerr << " - min node value in neighborface not as expected: " << minValue << "-" << testValue;
			std::cerr << " difference: " << minValue-testValue;
			std::cerr << " relative difference: " << (minValue - testValue)/(minValue + testValue) << std::endl;
		}
		testValue = minFunc(nextLevel-1, minFaceNeighbor, otherNodeRight);
		if ((otherNodeValue - testValue).abs() > funcValueLimit)
		{
			std::cerr << "hybrid grid search inconsistency, level " << nextLevel-1;
			std::cerr << " - other node value in neighborface not as expected: " << otherNodeValue << "-" << testValue;
			std::cerr << " difference: " << otherNodeValue-testValue;
			std::cerr << " relative difference: " << (otherNodeValue - testValue)/(otherNodeValue + testValue) << std::endl;
		}

#endif //hybridSearch_ConsistencyCheckLimit

		minFace.setExtend(nextLevel, minNode);
		scaledFP centerNodeValue = minFunc(nextLevel, minFace, otherNodeCode);

		if (minValue < centerNodeValue)
		{
			//std::cout << "hybrid grid search, level " << nextLevel << ": end node smaller" << std::endl;
			minFaceNeighbor.setExtend(nextLevel, minNodeRight);
			otherNodeValue = centerNodeValue;

			hybridMinEdgeSearchAux_EdgeEndSmallerCase(
			    minFunc, nextLevel,
			    minFace, minNode, minValue,
			    otherNodeCode, otherNodeValue,
			    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
			    minNodeRight, otherNodeRight
			);
		}
		else
		{
			//std::cout << "hybrid grid search, level " << nextLevel << ": mid edge smaller" << std::endl;
			minFace.set(nextLevel, 0);
			minFaceNeighbor.setExtend(nextLevel, 0);

			hybridMinEdgeSearchAux_MidEdgeSmallerCase(
			    minFunc, nextLevel,
			    minFace, minNode, minValue,
			    otherNodeCode, otherNodeValue, centerNodeValue,
			    neighborEdgeCode, minFaceNeighbor, neighborOrientationMatch,
			    minNodeRight, otherNodeRight
			);
		}
	}
	// hybrid minimum edge search auxiliary function:
	// search for function minimum within nodes around lower level minimum edge end point
	template<typename gridFunction>
	static void hybridMinEdgeSearchAux_EdgeEndSmallerCase(
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
		scaledFP outerNode5Value = minFunc(level, minFaceNeighbor, nodeCode2); // note that this is outerNode #3 in case of only four adjacent faces

		/* variables for minimum node inside outer ring */
		scaledFP outerRingMinValue = otherNodeValue;
		unsigned short outerRingMinIndex = 6;

		if (outerNode1Value < outerRingMinValue)
		{
			outerRingMinIndex = 1;
			outerRingMinValue = outerNode1Value;
		}
		if (outerNode2Value < outerRingMinValue)
		{
			outerRingMinIndex = 2;
			outerRingMinValue = outerNode2Value;
		}
		if (outerNode5Value < outerRingMinValue)
		{
			outerRingMinIndex = 5;
			outerRingMinValue = outerNode5Value;
		}

		/* check the case of only four adjacent faces first */
		if (!(neighborOrientationMatch || edgeAOrientationMatch))
		{
			switch (outerRingMinIndex)
			{
			case 1:
			{
				if (outerNode2Value < otherNodeValue)
				{
					otherNodeValue = outerRingMinValue;

					// minNode = nodeCode0; -> no change
					otherNodeCode = nodeCode2;
					neighborEdgeCode = nodeCode1;
					//neighborOrientationMatch unchanged (false)

					minFaceNeighbor = minFace;
					minFace = leftFaceA;

					hybridMinEdgeSearchAux_LeftOuterNodeCheck(
					    minFunc, level,
					    minFace, minNode, minValue,
					    otherNodeCode, otherNodeValue,
					    neighborEdgeCode, outerNode2Value,
					    minFaceNeighbor, neighborOrientationMatch);
				}
				else
				{
					bool outerRingEdgeOrientationMatch = true;
					sGrid outerRingNeighbor = minFace.neighborFace(level, nodeCode0, outerRingEdgeOrientationMatch);
					scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, nodeCode0);

					if (outerRingNeighborNewValue < minValue)
					{
						//outer ring edge cannot be the minimum edge
						minFace = outerRingNeighbor;

						if (outerRingNeighborNewValue < outerRingMinValue)
						{
							minValue = outerRingNeighborNewValue;
							otherNodeValue = outerRingMinValue;

							if (!outerRingEdgeOrientationMatch)
							{
								neighborEdgeCode = nodeCode1;
								//minNode = nodeCode0; -> no change
								otherNodeCode = nodeCode2;
							}
							//else
							//{
							//neighborEdgeCode = nodeCode2; -> no change
							//minNode = nodeCode0; -> no change
							//otherNodeCode = nodeCode1; -> no change
							//}
						}
						else
						{
							minValue = outerRingMinValue;
							otherNodeValue = outerRingNeighborNewValue;

							if (outerRingEdgeOrientationMatch)
							{
								//neighborEdgeCode = nodeCode2; -> no change
								minNode = nodeCode1;
								otherNodeCode = nodeCode0;
							}
							else
							{
								neighborEdgeCode = nodeCode1;
								minNode = nodeCode2;
								otherNodeCode = nodeCode0;
							}
						}
						minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
					}
					else
					{
						minFaceNeighbor = leftFaceA;
						// minFace unchanged;
						neighborEdgeCode = nodeCode1;

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

					hybridMinEdgeSearchAux_LeftOuterNodeCheck(
					    minFunc, level,
					    minFace, minNode, minValue,
					    otherNodeCode, otherNodeValue,
					    neighborEdgeCode, outerNode1Value,
					    minFaceNeighbor, neighborOrientationMatch);
				}
				else
				{
					minFaceNeighbor = leftFaceA;
					minFace = leftFaceA.neighborFace(level, neighborEdgeCode);

					hybridMinEdgeSearchAux_LeftOuterNodeCheck(
					    minFunc, level,
					    minFace, minNode, minValue,
					    otherNodeCode, otherNodeValue,
					    neighborEdgeCode, outerNode5Value,
					    minFaceNeighbor, neighborOrientationMatch);
				}
				break;
			}
			case 5:
			{
				if (outerNode2Value < otherNodeValue)
				{
					otherNodeValue = outerRingMinValue;

					// minNode = nodeCode0; -> no change
					otherNodeCode = nodeCode2;
					neighborEdgeCode = nodeCode1;
					//neighborOrientationMatch unchanged (false)

					// minFaceNeighbor unchanged
					minFace = minFaceNeighbor.neighborFace(level, neighborEdgeCode);

					hybridMinEdgeSearchAux_LeftOuterNodeCheck(
					    minFunc, level,
					    minFace, minNode, minValue,
					    otherNodeCode, otherNodeValue,
					    neighborEdgeCode, outerNode2Value,
					    minFaceNeighbor, neighborOrientationMatch);
				}
				else
				{
					bool outerRingEdgeOrientationMatch = true;
					sGrid outerRingNeighbor = minFaceNeighbor.neighborFace(level, nodeCode0, outerRingEdgeOrientationMatch);
					scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, nodeCode0);

					if (outerRingNeighborNewValue < minValue)
					{
						//outer ring edge cannot be the minimum edge
						minFace = outerRingNeighbor;

						if (outerRingNeighborNewValue < outerRingMinValue)
						{
							minValue = outerRingNeighborNewValue;
							otherNodeValue = outerRingMinValue;

							if (!outerRingEdgeOrientationMatch)
							{
								neighborEdgeCode = nodeCode1;
								//minNode = nodeCode0; -> no change
								otherNodeCode = nodeCode2;
							}
							//else
							//{
							//neighborEdgeCode = nodeCode2; -> no change
							//minNode = nodeCode0; -> no change
							//otherNodeCode = nodeCode1; -> no change
							//}
						}
						else
						{
							minValue = outerRingMinValue;
							otherNodeValue = outerRingNeighborNewValue;

							if (outerRingEdgeOrientationMatch)
							{
								//neighborEdgeCode = nodeCode2; -> no change
								minNode = nodeCode1;
								otherNodeCode = nodeCode0;
							}
							else
							{
								neighborEdgeCode = nodeCode1;
								minNode = nodeCode2;
								otherNodeCode = nodeCode0;
							}
						}
						minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
					}
					else
					{
						neighborEdgeCode = nodeCode1;
						//neighborOrientationMatch unchanged (false)

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
				}
				break;
			}
			case 6:
			{
				if (outerNode5Value < outerNode1Value)
				{
					bool outerRingEdgeOrientationMatch = true;
					sGrid outerRingNeighbor = minFaceNeighbor.neighborFace(level, nodeCode0, outerRingEdgeOrientationMatch);
					scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, nodeCode0);

					if (outerRingNeighborNewValue < minValue)
					{
						minFace = outerRingNeighbor;

						minValue = outerRingNeighborNewValue;
						//otherNodeValue unchanged

						if (outerRingEdgeOrientationMatch)
						{
							//minNode = nodeCode0; -> no change
							otherNodeCode = nodeCode2;
							neighborEdgeCode = nodeCode1;
						}
						//else
						//{
						//minNode = nodeCode0; -> no change
						//otherNodeCode = nodeCode1; -> no change
						//neighborEdgeCode = nodeCode2; -> no change

						//}
						minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
					}
					//else no change
				}
				else
				{
					bool outerRingEdgeOrientationMatch = true;
					sGrid outerRingNeighbor = minFace.neighborFace(level, nodeCode0, outerRingEdgeOrientationMatch);
					scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, nodeCode0);

					if (outerRingNeighborNewValue < minValue)
					{
						minFace = outerRingNeighbor;

						minValue = outerRingNeighborNewValue;
						//otherNodeValue unchanged

						if (outerRingEdgeOrientationMatch)
						{
							//minNode = nodeCode0; -> no change
							otherNodeCode = nodeCode2;
							neighborEdgeCode = nodeCode1;
						}
						//else
						//{
						//minNode = nodeCode0; -> no change
						//otherNodeCode = nodeCode1; -> no change
						//neighborEdgeCode = nodeCode2; -> no change

						//}
						minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
					}
					//else no change
				}
				break;
			}
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
			outerRingMinIndex = 3;
			outerRingMinValue = outerNode3Value;
		}
		if (outerNode4Value < outerRingMinValue)
		{
			outerRingMinIndex = 4;
			outerRingMinValue = outerNode4Value;
		}
		/*
		std::cout << "level: " << level << " - " << outerRingMinIndex << " - " << edgeAOrientationMatch;
		minFace.print(std::cout);
		leftFaceA.print(std::cout);
		std::cout << " minValue: " << minValue;
		std::cout << " outerRingMinValue: " << outerRingMinValue;
		std::cout << std::endl;
		*/
		switch (outerRingMinIndex)
		{
		case 1:
		{
			//std::cout << " outerNode2Value: " << outerNode2Value;
			//std::cout << " otherNodeValue: " << otherNodeValue << std::endl;
			if (outerNode2Value < otherNodeValue)
			{
				neighborEdgeCode = nodeCode1;
				neighborOrientationMatch = edgeAOrientationMatch;

				//minValue unchanged for now, will update within hybridMinEdgeSearch_LeftOuterNodeCheck
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

				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode2Value,
				    minFaceNeighbor, neighborOrientationMatch);
			}
			else
			{
				bool outerRingEdgeOrientationMatch = true;
				sGrid outerRingNeighbor = minFace.neighborFace(level, nodeCode0, outerRingEdgeOrientationMatch);
				scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, nodeCode0);

				if (outerRingNeighborNewValue < minValue)
				{
					minFace = outerRingNeighbor;

					minValue = outerRingNeighborNewValue;
					otherNodeValue = outerNode1Value;

					if (!outerRingEdgeOrientationMatch)
					{
						//minNode = nodeCode0; -> no change
						otherNodeCode = nodeCode2;
						neighborEdgeCode = nodeCode1;
					}
					//else
					//{
					//minNode = nodeCode0; -> no change
					//otherNode = nodeCode1; -> no change
					//neighborEdgeCode = nodeCode2; -> no change
					//}
					minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
				}
				else
				{
					neighborEdgeCode = nodeCode1;
					neighborOrientationMatch = edgeAOrientationMatch;

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
			}
			break;
		}
		case 2:
		{
			neighborEdgeCode = edgeCodeBLeft;
			neighborOrientationMatch = edgeBOrientationMatch;

			//minValue unchanged for now, will update within hybridMinEdgeSearchAux_LeftOuterNodeCheck
			otherNodeValue = outerRingMinValue;

			if (outerNode1Value < outerNode3Value)
			{
				minFace = leftFaceA;
				minFaceNeighbor = leftFaceB;

				minNode = nodeCode2;
				if (!edgeAOrientationMatch) minNode = nodeCode0;
				//otherNodeCode = nodeCode1; -> no change

				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode1Value,
				    minFaceNeighbor, neighborOrientationMatch);
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
					//otherNodeCode = nodeCode1; -> no change
				}

				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode3Value,
				    minFaceNeighbor, neighborOrientationMatch);
			}
			break;
		}
		case 3:
		{
			// neighborOrientationMatch unchanged

			//minValue unchanged for now, will update within hybridMinEdgeSearchAux_LeftOuterNodeCheck
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

				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode2Value,
				    minFaceNeighbor, neighborOrientationMatch);
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
						neighborEdgeCode = minNodeRight;
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

				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode4Value,
				    minFaceNeighbor, neighborOrientationMatch);
			}
			break;
		}
		case 4:
		{
			neighborOrientationMatch = edgeAOrientationMatch;

			//minValue unchanged for now, will update within hybridMinEdgeSearchAux_LeftOuterNodeCheck
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

				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode3Value,
				    minFaceNeighbor, neighborOrientationMatch);
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

				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode5Value,
				    minFaceNeighbor, neighborOrientationMatch);
			}
			break;
		}
		case 5:
		{
			if (outerNode4Value < otherNodeValue)
			{
				neighborEdgeCode = otherNodeRight;
				neighborOrientationMatch = edgeBOrientationMatch;

				//minValue unchanged for now, will update within hybridMinEdgeSearchAux_LeftOuterNodeCheck
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

				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, outerNode4Value,
				    minFaceNeighbor, neighborOrientationMatch);
			}
			else
			{
				bool outerRingEdgeOrientationMatch = true;
				sGrid outerRingNeighbor = minFaceNeighbor.neighborFace(level, minNodeRight, outerRingEdgeOrientationMatch);
				scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, minNodeRight);

				if (outerRingNeighborNewValue < minValue)
				{
					minValue = outerRingNeighborNewValue;
					otherNodeValue = outerNode5Value;

					minFace = outerRingNeighbor;

					minNode = minNodeRight;
					if (outerRingEdgeOrientationMatch)
					{
						otherNodeCode = otherNodeRight;
						neighborEdgeCode = nodeCode2;
					}
					else
					{
						otherNodeCode = nodeCode2;
						neighborEdgeCode = otherNodeRight;
					}
					minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
				}
				else
				{
					neighborEdgeCode = otherNodeRight;
					neighborOrientationMatch = edgeBOrientationMatch;

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
			}
			break;
		}
		case 6:
		{
			if (outerNode5Value < outerNode1Value)
			{
				bool outerRingEdgeOrientationMatch = true;
				sGrid outerRingNeighbor = minFaceNeighbor.neighborFace(level, minNodeRight, outerRingEdgeOrientationMatch);
				scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, minNodeRight);

				if (outerRingNeighborNewValue < minValue)
				{
					minFace = outerRingNeighbor;

					minValue = outerRingNeighborNewValue;
					// otherNodeValue unchanged

					minNode = minNodeRight;
					if (outerRingEdgeOrientationMatch)
					{
						otherNodeCode = nodeCode2;
						neighborEdgeCode = otherNodeRight;
					}
					else
					{
						otherNodeCode = otherNodeRight;
						//neighborEdgeCode = nodeCode2; -> no change
					}
					minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
				}
				else
				{
					std::swap(minFace, minFaceNeighbor);

					minNode = minNodeRight;
					otherNodeCode = otherNodeRight;

					//neighborEdgeCode = nodeCode2; -> no change
					//neighborOrientationMatch unchanged

					//minValue & otherNodeValue unchanged
				}
			}
			else
			{
				bool outerRingEdgeOrientationMatch = true;
				sGrid outerRingNeighbor = minFace.neighborFace(level, minNode, outerRingEdgeOrientationMatch);
				scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, minNode);

				if (outerRingNeighborNewValue < minValue)
				{
					minFace = outerRingNeighbor;
					minValue = outerRingNeighborNewValue;
					//otherNodeValue unchanged

					//minNode = nodeCode0; -> no change
					if (outerRingEdgeOrientationMatch)
					{
						otherNodeCode = nodeCode2;
						neighborEdgeCode = nodeCode1;
					}
					//else
					//{
					//otherNodeCode = nodeCode1; -> no change
					//neighborEdgeCode = nodeCode2; -> no change
					//}
					minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
				}
				//else no changes
			}
			break;
		}
		default:
		{
			break;
		}
		}
		return;
	}
	// hybrid minimum edge search auxiliary function:
	// search for function minimum within nodes around lower level minimum edge mid point
	template<typename gridFunction>
	static void hybridMinEdgeSearchAux_MidEdgeSmallerCase(
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
		//assume the face code at level for minFace and minFaceNeighbor is already set to 0
		//minFace.set(level,0);
		//minFaceNeighbor.setExtend(level,0);

		unsigned short nodeCode0 = otherNodeCode;
		unsigned short nodeCode1 = minNode;
		unsigned short nodeCode2 = neighborEdgeCode;

		scaledFP leftCenterFaceFValueA = minFunc(level,minFace,nodeCode0);
		scaledFP leftCenterFaceFValueB = minFunc(level,minFace,nodeCode1);
		scaledFP rightCenterFaceFValueA = minFunc(level,minFaceNeighbor,otherNodeRight);
		scaledFP rightCenterFaceFValueB = minFunc(level,minFaceNeighbor,minNodeRight);

		// outer ring minimum search:
		// edges connecting to the other node of the level-1 minimum edge cannot become the new minmum edge
		unsigned short outerRingMinIndex = 6; // this is the case where the level-1 minimum node is the outer ring minimum
		scaledFP outerRingMinValue = minValue;
		if (leftCenterFaceFValueA < outerRingMinValue)
		{
			outerRingMinIndex = 1;
			outerRingMinValue = leftCenterFaceFValueA;
		}
		if (leftCenterFaceFValueB < outerRingMinValue)
		{
			outerRingMinIndex = 2;
			outerRingMinValue = leftCenterFaceFValueB;
		}
		if (rightCenterFaceFValueA < outerRingMinValue)
		{
			outerRingMinIndex = 5;
			outerRingMinValue = rightCenterFaceFValueA;
		}
		if (rightCenterFaceFValueB < outerRingMinValue)
		{
			outerRingMinIndex = 4;
			outerRingMinValue = rightCenterFaceFValueB;
		}
		//std::cout << outerRingMinIndex << std::endl;

		switch (outerRingMinIndex)
		{
		case 1:
		{
			if (leftCenterFaceFValueB < minValue)
			{
				// minFace.set(level, 0) already done
				minFaceNeighbor = minFace;
				minFaceNeighbor.set(level,nodeCode1);
				neighborOrientationMatch = true;
				// outerRingNeighbor already checked at level-1

				// otherNodeCode = nodeCode0; -> no change
				minNode = nodeCode2;
				neighborEdgeCode = nodeCode1;

				// outerRingMinValue may be smaller than centerValue, this will be checked by hybridMinEdgeSearchAux_LeftOuterNodeCheck
				minValue = centerNodeValue;
				otherNodeValue = outerRingMinValue;

				// now check whether center node or outer ring minimum is smaller
				// can use fast_LeftOuterNodeCheck, because outerRingNeighbor was already checked at level-1
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

				bool outerRingEdgeOrientationMatch = true;
				sGrid outerRingNeighbor = minFace.neighborFace(level, nodeCode0, outerRingEdgeOrientationMatch);
				scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, nodeCode0);

				if (outerRingNeighborNewValue < centerNodeValue)
				{
					minFace = outerRingNeighbor;

					if (outerRingNeighborNewValue < outerRingMinValue)
					{
						minValue = outerRingNeighborNewValue;
						otherNodeValue = outerRingMinValue;

						minNode = nodeCode0;

						if (outerRingEdgeOrientationMatch)
						{
							otherNodeCode = nodeCode1;
							// neighborEdgeCode = nodeCode2; -> no change
						}
						else
						{
							otherNodeCode = nodeCode2;
							neighborEdgeCode = nodeCode1;
						}

						minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
					}
					else
					{
						minValue = outerRingMinValue;
						otherNodeValue = outerRingNeighborNewValue;

						//otherNodeCode = nodeCode0; -> no change

						if (!outerRingEdgeOrientationMatch)
						{
							minNode = nodeCode2;
							neighborEdgeCode = nodeCode1;
						}
						//else {
						//	minNode = nodeCode1; -> no change
						//	neighborEdgeCode = nodeCode2; -> no change
						//}
						minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
					}
				}
				else
				{
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
					}
					else
					{
						otherNodeCode = nodeCode2;
						minNode = nodeCode0;
						minValue = centerNodeValue;
						otherNodeValue = outerRingMinValue;
					}
					/*
							    std::cout << "minValue < otherNodeValue: " << minValue << "<" << otherNodeValue << std::endl;
							    minFace.print(std::cout);
							    std::cout << otherNodeCode << ":" << minFunc(level, minFace, otherNodeCode) << std::endl;
							    minFaceNeighbor.print(std::cout);
							    std::cout << minNode << ":" << minFunc(level, minFaceNeighbor, minNode) << std::endl;
					*/
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
				// outerRingNeighbor already checked at level-1

				otherNodeCode = nodeCode1;
				minNode = nodeCode2;
				neighborEdgeCode = nodeCode0;

				// outerRingMinValue may be smaller than centerValue, this will be checked by hybridMinEdgeSearchAux_LeftOuterNodeCheck
				minValue = centerNodeValue;
				otherNodeValue = outerRingMinValue;

				// now check whether center node or outer ring minimum is smaller
				// can use fast_LeftOuterNodeCheck, because outerRingNeighbor was already checked at level-1
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

				bool outerRingEdgeOrientationMatch = true;
				sGrid outerRingNeighbor = minFace.neighborFace(level, nodeCode1, outerRingEdgeOrientationMatch);
				scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, nodeCode1);

				if (outerRingNeighborNewValue < centerNodeValue)
				{
					minFace = outerRingNeighbor;

					if (outerRingNeighborNewValue < outerRingMinValue)
					{
						minValue = outerRingNeighborNewValue;
						otherNodeValue = outerRingMinValue;

						// minNode = nodeCode1; -> no change

						if (!outerRingEdgeOrientationMatch)
						{
							otherNodeCode = nodeCode2;
							neighborEdgeCode = nodeCode0;
						}
						//else
						//{
						// otherNodeCode = nodeCode0; -> no change
						// neighborEdgeCode = nodeCode2; -> no change
						//}
						minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
					}
					else
					{
						minValue = outerRingMinValue;
						otherNodeValue = outerRingNeighborNewValue;

						otherNodeCode = nodeCode1;
						if (outerRingEdgeOrientationMatch)
						{
							minNode = nodeCode0;
							//neighborEdgeCode = nodeCode2; -> no change
						}
						else
						{
							minNode = nodeCode2;
							neighborEdgeCode = nodeCode0;
						}
						minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
					}
				}
				else
				{

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
			}
			break;
		}
		//case 3: not possible, checked in level-1
		case 4:
		{
			if (rightCenterFaceFValueA < otherNodeValue)
			{
				minFace = minFaceNeighbor;
				// minFace.set(level, 0) already done
				minFaceNeighbor.set(level,otherNodeRight);
				neighborOrientationMatch = true;
				// outerRingNeighbor already checked at level-1

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

				bool outerRingEdgeOrientationMatch = true;
				sGrid outerRingNeighbor = minFace.neighborFace(level, minNodeRight, outerRingEdgeOrientationMatch);
				scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, minNodeRight);

				if (outerRingNeighborNewValue < centerNodeValue)
				{
					minFace = outerRingNeighbor;
					if (outerRingNeighborNewValue < outerRingMinValue)
					{
						minValue = outerRingNeighborNewValue;
						otherNodeValue = outerRingMinValue;

						minNode = minNodeRight;
						if (outerRingEdgeOrientationMatch)
						{
							otherNodeCode = otherNodeRight;
							neighborEdgeCode = nodeCode2;
						}
						else
						{
							otherNodeCode = nodeCode2;
							neighborEdgeCode = otherNodeRight;
						}
					}
					else
					{
						minValue = outerRingMinValue;
						otherNodeValue = outerRingNeighborNewValue;

						otherNodeCode = minNodeRight;
						if (outerRingEdgeOrientationMatch)
						{
							minNode = otherNodeRight;
							neighborEdgeCode = nodeCode2;
						}
						else
						{
							minNode = nodeCode2;
							neighborEdgeCode = otherNodeRight;
						}
					}
					minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
				}
				else
				{
					// minFaceNeighbor.set(level,0); already done
					neighborOrientationMatch = true;

					neighborEdgeCode = otherNodeRight;

					if (outerRingMinValue < centerNodeValue)
					{
						minNode = nodeCode2;
						otherNodeCode = minNodeRight;
						minValue = outerRingMinValue;
						otherNodeValue = centerNodeValue;
					}
					else
					{
						minNode = minNodeRight;
						otherNodeCode = nodeCode2;
						minValue = centerNodeValue;
						otherNodeValue = outerRingMinValue;
					}
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
				// outerRingNeighbor already checked at level-1

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

				bool outerRingEdgeOrientationMatch = true;
				sGrid outerRingNeighbor = minFace.neighborFace(level, otherNodeRight, outerRingEdgeOrientationMatch);
				scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, otherNodeRight);

				if (outerRingNeighborNewValue < centerNodeValue)
				{
					minFace = outerRingNeighbor;
					if (outerRingNeighborNewValue < outerRingMinValue)
					{
						minValue = outerRingNeighborNewValue;
						otherNodeValue = outerRingMinValue;

						minNode = otherNodeRight;
						if (outerRingEdgeOrientationMatch)
						{
							otherNodeCode = minNodeRight;
							neighborEdgeCode = nodeCode2;
						}
						else
						{
							otherNodeCode = nodeCode2;
							neighborEdgeCode = minNodeRight;
						}
					}
					else
					{
						minValue = outerRingMinValue;
						otherNodeValue = outerRingNeighborNewValue;
					}
					minFaceNeighbor = minFace.neighborFace(level, neighborEdgeCode, neighborOrientationMatch);
				}
				else
				{
					// minFaceNeighbor.set(level,0); already done
					neighborOrientationMatch = true;

					neighborEdgeCode = minNodeRight;

					if (outerRingMinValue < centerNodeValue)
					{
						minNode = nodeCode2;
						otherNodeCode = otherNodeRight;
						minValue = outerRingMinValue;
						otherNodeValue = centerNodeValue;
					}
					else
					{
						minNode = otherNodeRight;
						otherNodeCode = nodeCode2;
						minValue = centerNodeValue;
						otherNodeValue = outerRingMinValue;
					}
				}
			}
			break;
		}
		case 6:
		{
			minFace.set(level, minNode);
			minFaceNeighbor.set(level, minNodeRight);
			//neighborEdgeCode & neighborOrientationMatch unchanged
			otherNodeValue = minValue;
			minValue = centerNodeValue;

			if (rightCenterFaceFValueA < leftCenterFaceFValueA)
			{
				std::swap(minFace, minFaceNeighbor);
				minNode = otherNodeRight;
				otherNodeCode = minNodeRight;

				// now check whether center node or outer ring minimum is smaller
				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, rightCenterFaceFValueA,
				    minFaceNeighbor, neighborOrientationMatch);
			}
			else
			{
				minNode = nodeCode0;
				otherNodeCode = nodeCode1;

				// now check whether center node or outer ring minimum is smaller
				hybridMinEdgeSearchAux_LeftOuterNodeCheck(
				    minFunc, level,
				    minFace, minNode, minValue,
				    otherNodeCode, otherNodeValue,
				    neighborEdgeCode, leftCenterFaceFValueA,
				    minFaceNeighbor, neighborOrientationMatch);
			}


			break;
		}
		default:
		{
			break;
		}
		}
	}
	/* hybrid minimum edge search auxiliary function:
	    starting from a face (minFace), where one node was a center node during the search (centerNode),
	    and the 2nd node has the smallest function value inside the outer ring (at level) of this center node:
	    calculate one new function value outside the outer ring around the center in the face adjacent to the relevant outer edge
	    re-arrange correct minimum nodes, edges and faces such that on output centerNode is the final (local) minimum
	    assumption: 3rd function value inside minFace < 3rd function value inside minFaceNeighbor
	*/
	template<typename gridFunction>
	static void hybridMinEdgeSearchAux_LeftOuterNodeCheck(
	    gridFunction minFunc,
	    size_t level, // the grid level of the minimum search
	    sGrid& minFace, // face including the center node and the outer ring minimum node --> becomes true minimum face on output
	    unsigned short& centerNode, // center node inside minFace on input --> becomes true minimum node in the end
	    scaledFP& centerValue, // input: function value at the center node, output: actual minimum at this level
	    unsigned short& outerMinNode, // input: the nodeCode of the outer ring minimum within minFace, output the other nodeCode of the minimum edge
	    scaledFP& outerNodeMinValue, // input: outer ring minimum value, output: function value at the non-minimum end node of the minimum edge
	    unsigned short& leftOuterNode, // input: edge code connecting the center node to the outer ring minimum node --> becomes the minimum edge on output
	    const scaledFP& leftOuterValue, // function value at the 3rd node of minFace
	    sGrid& minFaceNeighbor, // neighbor face next to minFace across the relevant edge
	    bool& neighborOrientationMatch // whether orientation of minFace and minFaceNeighbor is matched
	)
	{
		bool outerRingEdgeOrientationMatch = true;
		sGrid outerRingNeighbor = minFace.neighborFace(level, centerNode, outerRingEdgeOrientationMatch);
		scaledFP outerRingNeighborNewValue = minFunc(level, outerRingNeighbor, centerNode);

		if (outerRingNeighborNewValue < centerValue)
		{
			if (leftOuterValue < outerRingNeighborNewValue) // outer ring edge is the actual the minimum edge
			{
				//minFace unchanged
				minFaceNeighbor = outerRingNeighbor;
				neighborOrientationMatch = outerRingEdgeOrientationMatch;

				unsigned short minNode = outerMinNode;

				//for output: centerNode -> minNode, outerMinNode -> leftOuterNode, neighborEdgeCode(==leftOuterNode) --> centerNode
				outerMinNode = leftOuterNode;
				leftOuterNode = centerNode;
				centerNode = minNode;

				outerNodeMinValue = leftOuterValue;
				centerValue = outerNodeMinValue;
			}
			else //new minimum edge connects from outer ring minimum to the new node outside the outer ring
			{
				minFace = outerRingNeighbor;
				if (outerRingNeighborNewValue < outerNodeMinValue) // minimum in at the new node outside the outer ring
				{
					centerValue = outerRingNeighborNewValue;
					//outerNodeMinValue -> no change

					if (outerRingEdgeOrientationMatch)
					{
						//centerNode -> no change
						std::swap(outerMinNode, leftOuterNode);
					}
					//else
					//{
					//centerNode -> no change
					//outerMinNode -> no change
					// leftOuterNode -> no change
					//}
					minFaceNeighbor = minFace.neighborFace(level, leftOuterNode, neighborOrientationMatch);
				}
				else // minimum in the outer ring
				{
					centerValue = outerNodeMinValue;
					outerNodeMinValue = outerRingNeighborNewValue;

					if (outerRingEdgeOrientationMatch)
					{
						unsigned short minNode = leftOuterNode;
						leftOuterNode = outerMinNode;
						outerMinNode = centerNode;
						centerNode = minNode;
					}
					else
					{
						std::swap(outerMinNode, centerNode);
						//leftOuterNode -> no change
					}
					minFaceNeighbor = minFace.neighborFace(level, leftOuterNode, neighborOrientationMatch);
				}
			}
		}
		else
		{
			if (outerNodeMinValue < centerValue) // outer ring value is the minimum
			{
				unsigned short minNode = outerMinNode;

				if (leftOuterValue < centerValue) // outer edge is the actual the minimum edge
				{
					//for output: centerNode -> minNode, outerMinNode -> leftOuterNode, neighborEdgeCode(==leftOuterNode) --> centerNode
					outerMinNode = leftOuterNode;
					leftOuterNode = centerNode;
					centerNode = minNode;

					centerValue = outerNodeMinValue;
					outerNodeMinValue = leftOuterValue;

					minFaceNeighbor = outerRingNeighbor;
					neighborOrientationMatch = outerRingEdgeOrientationMatch;
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
		}
		return;
	}



public:
	void print(std::ostream& out) const;
};
//output for grid coordinates as face codes
std::ostream& operator << (std::ostream& out, const sGrid&);

} // namespace SPIRID

#endif //SPIRID_H_INCLUDED
