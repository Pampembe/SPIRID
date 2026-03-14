#ifndef SPIRID_H
#define SPIRID_H

#include <iostream> //for I/O operations

#include <vector> //grid based coordinate system uses vector<bool> to label points

#include <cmath> //for standard math functions and to define constants related to pi
#define fp_type float //standard floating point type for calculations
#define SQRT sqrtf //sqrt for float numbers
#define ACOS acosf //acos for float numbers
#define ASIN asinf //asin for float numbers
#define SIN  sinf //sin for float numbers
#define COS  cosf //cos for float numbers
#define LDEXP ldexpf //multiply float by 2^exp
#define ABS std::abs

#define scaleExp_type unsigned long long



//SPIRID - SPherical CoordInate gRID
namespace SPIRID
{


//constants related to pi
const fp_type pi = M_PI;
const fp_type pi_2 = M_PI_2;
const fp_type two_pi = 2*M_PI;






//floating point type allowing arbitrarily small numbers by including a scaling factor
class scaledFP
{
	//actual number is mantissa * 2^(-scaleExponent)
	fp_type mantissa;
	scaleExp_type scaleExponent;

	//mantissa digits of the fp_type
	static const size_t FPDigits;
	//the maximum negative exponent supported by fp_type (-min_exponent)
	static const scaleExp_type FPMaxExponent;

public:
	inline scaledFP(fp_type m, scaleExp_type e = 0) : mantissa(m), scaleExponent(e) {}; //standard constructor

	//get mantissa and exponent
	inline fp_type getMantissa() const {
		return mantissa;
	};
	inline scaleExp_type getExponent() const {
		return scaleExponent;
	};

	operator fp_type() const; //convert to standard float
};
//standard output for scaledFP
std::ostream& operator << (std::ostream& out, const scaledFP&);






//class angle: automatically converts to a given unit (Rad/Deg/Pi) on output
class angle : public scaledFP
{

public:
	inline angle(fp_type m, scaleExp_type e = 0) : scaledFP(m,e) {}; //standard constructor

	static fp_type unitScale; // to convert radians/deg/...: radian-->unitScale=1; deg-->unitScale=180/Pi
	static std::string unitSymbol; //after switching unit the output functions adds a symbol

	//functions to change unit (global)
	static void unitPi();
	static void unitRad();
	static void unitDeg();
};
//output for anlge
std::ostream& operator << (std::ostream& out, const angle&);






//polar coordinates of the sphere (polar and azimuthal angle, radius==1)
class sPolar
{
	fp_type theta; // Polar angle in [0,Pi]
	fp_type phi;   // Azimuthal angle in [0,2Pi)
	void normalize(); // shift theta into [0,Pi] and phi into [0,2Pi)

public:
	inline sPolar(fp_type t, fp_type p) : theta(t), phi(p) {
		normalize();
	};
	void set(fp_type t, fp_type p);
	inline angle getTheta() const {
		return angle(theta);
	};
	inline angle getPhi() const {
		return angle(phi);
	};

	static angle distance(const sPolar& P1, const sPolar& P2);
};
//output for polar coordinates
std::ostream& operator << (std::ostream& out, const sPolar&);






//triangle grid based coordinates of the sphere (radius==1)
/*  Each octant of the sphere is considered a spherical triangle.
    Every spherical triangle is then successively divided into four sub-triangles.
    Nomenclature:
        Face: spherical triangle in the resulting grid
        Node/Edge: nodes and edges of a face
        Point: A point inside a face, further characterized by a location code
            location 0: the point where side bisection lines intersect
            location 1,2,3: the respective nodes of the face
        Level: number of face division to get to a certain face (initial octant is level 0)
*/
class sGrid
{
public:
	//standard constructors
	inline sGrid(size_t depth = 0) : gridCode(2*depth+3) {};
	inline sGrid(const sGrid& P) : gridCode(P.gridCode) {};
	sGrid(const std::vector<bool>& P);

	/*  A face is represented by a gridCode with depth+1 digits:
	        1st digit (0..7): grid level 0, identifies the octant of the sphere
	        nth digit (0..3): grid level n-1, identifies the sub-faces after successive divisions
	        depth+1: number of digits of the gridCode
	    A gridCode of depth D also represents D faces with smaller depths through truncation of the last digits
	    A gridCode of depth D also represents infinitly many faces with higher grid levels through appending zeros after digit D+1
	*/
	//depth of the gridCode & resize
	inline size_t depth() const {
		return (gridCode.size()-3)/2; //internally gridCode is a vector<bool> gridCode of length 3+2*depth --> make sure gridCode.size() always >=3
	};
	inline void resize(size_t newDepth) {
		gridCode.resize(3+2*newDepth);
	};
	//get & set face code: (0..7) at level 0, (0..3) at other levels
	unsigned short at(size_t level) const;
	void setExtend(size_t level, unsigned short faceCode);

	//stepping through the grid at a certain level
	sGrid neighborFace(size_t level, unsigned short edgeCode) const
	{
		sGrid neighbor(*this);
		neighbor.resize(level);
		bool orientationMatch = true;
		return assignNeighborFace(level, edgeCode, neighbor, orientationMatch);
	};

	/* Each point at every grid level corresponds to a
	   face (faceCodes 0..3) with edges and nodes (codes 1..3).
	   We can calculate edge lengths, interior angles and area for that face.
	*/
	//calculate the area of a face
	angle area(size_t level) const
	{
	    return area(level, calcFaceGeometry(level));
	};
	//calculate an edge length (code 1..3)
	angle edgeLength(size_t level, unsigned short edgeCode) const;
	//calculate the interior angle of a face at a node (1..3)
	inline angle interiorAngle(size_t level, unsigned short nodeCode) const
	{
		return angle(interiorAngle(level, nodeCode, calcFaceGeometry(level)));
	};

	//convertion functions from and to polar coordinates
	sGrid(const sPolar& P);
	sPolar toPolar(size_t level, unsigned short location = 0) const;
	inline sPolar toPolar() const {
		return toPolar(depth(),0);
	};
	inline sPolar toPolar(unsigned short location) const {
		return toPolar(depth(),location);
	};
	//for a point at level within a face at refNodeLevel: returns distance to a node and angle to an edge
	sPolar toLocalPolar(size_t refNodeLevel, unsigned short nodeCode, unsigned short edgeCode, size_t level, unsigned short location = 0) const;
	inline sPolar toLocalPolar(size_t refNodeLevel, unsigned short nodeCode, unsigned short edgeCode) const
	{
		return toLocalPolar(refNodeLevel, nodeCode, edgeCode, depth(), 0);
	};

	//calculate the distance between two points
	static angle distance(size_t level1, const sGrid& P1, unsigned short location1,
	                      size_t level2, const sGrid& P2, unsigned short location2);
	inline static angle distance(size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
	{
		return distance(level1, P1, 0, level2, P2, 0);
	}
	inline static angle distance(const sGrid& P1, const sGrid& P2)
	{
		return distance(P1.depth(), P1, P2.depth(), P2);
	};

	//mirror a point to the opposite side of the sphere
	inline sGrid& mirror() {
		gridCode[0]=!gridCode[0];
		gridCode[1]=!gridCode[1];
		gridCode[2]=!gridCode[2];
		return *this;
	};

	//calculation accuracy (accuracyBits cannot be larger than digits of fp_type)
	static void setAccuracyBits(size_t);
	inline static size_t getAccuracyBits() {
		return accuracyBits;
	};

protected:
	//get and set a face code (0..3) or (0..7) at a certain level
	unsigned short operator [] (size_t level) const;
	void set(size_t level, unsigned short faceCode);

private:
	//internal representation of the gridCode
	std::vector<bool> gridCode;

	//calculation accuracy (accuracyBits cannot be larger than digits of fp_type)
	static size_t accuracyBits;
	//if accuracy is A then the difference between flat and spherical triangles above level A/2 is negligible
	static size_t minDepthFlatApprox;
	//if accuracy is A then the difference Sin(x)-x for numbers below 2^(-A/3) is negligible
	static size_t minScaleSinXToX;



	//assign the neighbor of the current face to an existing sGrid object, assuming its depth is already at level
	sGrid& assignNeighborFace(size_t level, unsigned short edgeCode, sGrid& neighbor, bool& orientationMatch) const;



	/* calculation of the distance between two points P1 & P2:
	    first step is to find out which is the highest grid level where the faces of P1 and P2 have a common node
	        the grid level can be different for P1 & P2 (if one of the points is closer to the common node)
	    then we calculate the distance of P1 & P2 to this common node and the angle between the respective connections
	    finally we use the spherical cosine law to calculate the distance
	*/
	//data type used for information about the common node and the angle between the corresponding faces
	struct commonNode
	{
		// {0,0,0, 0,0,0, 0,0} means no common node --> calculate distance to a mirrored point

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

		//output commonNode data
		std::ostream& print(std::ostream&);
	};
	//function to get common node data (including gapAngle): this is independent of the location of the point inside the face
	static commonNode findHighestCommonNode(const sGrid& P1, const sGrid& P2)
	{
		return findHighestCommonNode(P1.depth(), P1, P2.depth(), P2);
	};
	static commonNode findHighestCommonNode(size_t level1, const sGrid& P1, size_t level2, const sGrid& P2);
	//auxliary function used when P1 and P2 are in the same face at level-1
	static commonNode sameFaceStepupTo(size_t level, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2);
	//auxliary function used when the faces of P1 and P2 at level-1 share a common edge
	static commonNode commonEdgeStepupTo(size_t level, unsigned short edgeCode, bool orientationMatch, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2);
	//auxiliary function: assuming faceCode at startlevel-1 equals nodeCode: until which level nodeCode repeats?
	inline static size_t lastLevelAtNode(size_t startLevel, unsigned short nodeCode, size_t endLevel, const sGrid& P)
	{
		while (P.at(startLevel)==nodeCode && startLevel<=endLevel) {
			startLevel++;
		};
		return startLevel-1;
	};



	/* auxiliary functions for geometry calculations */
	//calculate third edge length d in a spherical triangle, given side-angle-side
	//      return value is 2^scale*Sin(d/2); input lengths L as 2^(2*scale)*Sin(L)^2) and angle alpha as Cos(alpha)
	static fp_type calcSinEdgeHalfSAS(scaleExp_type scale, fp_type SinaSq, fp_type cosDelta, fp_type SinbSq);
	//calculate third edge length d in a spherical triangle, given side-angle-side (all edge lengths L as 2^scale*Sin(L)^2)
	//the return value does not distinguish between angle and pi-angle, since the return value is basically sin(angle)^2
	inline static fp_type calcSinSqEdgeSAS(scaleExp_type scale, fp_type SinaSq, fp_type cosDelta, fp_type SinbSq)
	{
		return SinaSq + SinbSq - (cosDelta*cosDelta+1) * SinaSq*LDEXP(SinbSq,-scale)
		       - 2*cosDelta * SQRT(SinaSq*SinbSq * (1-LDEXP(SinaSq,-scale))*(1-LDEXP(SinbSq,-scale)));
	};
	//for edge bisections: calculate 4*Sin(a/2)^2 from Sin(a)^2
	inline static fp_type calcEdgeBisection(scaleExp_type scale, fp_type SinaSq)
	{
		return (2*SinaSq)/(1 + SQRT(1 - LDEXP(SinaSq,-scale)));
	};
	//calculate the area of a face given pre-calculated geometry and edge bisection (as 2^(2*(level+1))*Sin[half edge lengths]^2)
	static fp_type sinSqQuarterArea(size_t level,
	                                fp_type FourSinE1HalfSq,
	                                fp_type FourSinE2HalfSq,
	                                fp_type FourSinE3HalfSq);



	/* functions used to calculate face geometries */
	//data type used for geometry data of faces: lengths of the three edges E as Sin(E)^2*2^(2*level)
	struct faceGeometry {
		fp_type SinSqE[3]; // edgeCode-1
	};
	//calculate face geometry data at a certain level
	faceGeometry calcFaceGeometry(size_t level = -1) const;
	//calculate face geometry data at the next level given pre-calculated geometry data at the current level
	static void stepupFaceGeometryFrom(size_t currentLevel, unsigned short nextFaceCode, faceGeometry& currentFaceGeometry);
	//calculate the interior angle of a face at a node (1..3) given pre-calculated geometry data as Sin(edge lengths)^2
	static fp_type interiorAngle(size_t level, unsigned short nodeCode, const faceGeometry&);
	//calculate the interior angle at node 3 of a face given pre-calculated geometry data (edge lengths as scaleFactor*Sin(E)^2)
	static fp_type interiorAngle(scaleExp_type scale, fp_type SinE1Sq, fp_type SinE2Sq, fp_type SinE3Sq);
	//calculate the area of a face given pre-calculated geometry data as Sin(edge lengths)^2
	static angle area(size_t level, const faceGeometry&);



	/* functions used to calculate local polar coordinates inside a face */
	// data type for distance and direction data (local polar coordinates): distance d as Sin(d)^2*2^(2*level), angle as standard fp_type
	struct inFacePolar
	{
		//distance between a point and a node (stored as as Sin(d)^2*2^(2*level))
		fp_type SinDistSq;
		//angle between an edge and the connection between a point and a node
		fp_type angle;
	};
	//calculate local polar coordinates of a point (pointLevel, location) with respect to nodeCode & edgeCode at refNodeLevel
	inFacePolar calcFacePolar(size_t refNodeLevel, unsigned short nodeCode, unsigned short edgeCode, size_t pointLevel, unsigned short location = 0) const;
	//calculate local polar coordinates of a point (pointLevel, location) with respect to nodeCode & edgeCode at refNodeLevel, given pre-calculated geometry data
	inFacePolar calcFacePolar(size_t refNodeLevel, unsigned short nodeCode, unsigned short edgeCode, const faceGeometry&, size_t pointLevel, unsigned short location = 0) const;
	//flat geometry approximation: calculate local polar coordinates of a point (pointLevel, location) with respect to nodeCode & edgeCode at refNodeLevel, given pre-calculated geometry data
	inFacePolar calcFlatFacePolar(size_t refNodeLevel, unsigned short nodeCode, unsigned short edgeCode, const faceGeometry&, size_t pointLevel, unsigned short location = 0) const;



	/* auxiliary functions and constant arrays used to navigate through the grid topology */
	static const unsigned short newNodeCodes[4];
	static const unsigned short newFaceCodes[4];
	static const unsigned short newCodes[7];
	inline static unsigned short nextNode(unsigned short code) {
		return newNodeCodes[code];
	};
	inline static unsigned short nextFace(unsigned short code) {
		return newFaceCodes[code];
	};
	inline static unsigned short newCode(unsigned short code1, unsigned short code2, unsigned short code3 = 0) {
		return newCodes[code1+code2+code3];
	};
};
//output for grid coordinates as face codes
std::ostream& operator << (std::ostream& out, const sGrid&);



} // namespace SPIRID

#endif //SPIRID_H