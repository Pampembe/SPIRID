/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <cmath>

//#define USE_FLOAT
//#define USE_DOUBLE
//#define USE_LONG_DOUBLE

#include <SPIRID.h>
using namespace SPIRID;

//#define DEBUG

class test
{
public:
	scaledFP operator() (size_t level, const sGrid& P, unsigned short location) const
	{
		sPolar TP(P.toPolar(level,location));
//		return scaledFP(sPolar::distance(P.toPolar(level,location),sPolar(pi/5,pi/3)),0);
//		return scaledFP(sPolar::distance(P.toPolar(level,location),sPolar(pi/8,5*pi/8)),0);
//		return scaledFP(sPolar::distance(P.toPolar(level,location),sPolar(80*pi/91,54*pi/41)),0);
		return scaledFP(SQRT((fp_type(TP.getTheta())-pi/8.)*(fp_type(TP.getTheta())-pi/8.) + (fp_type(TP.getPhi())-5*pi/8.)*(fp_type(TP.getPhi())-5*pi/8.)),0);
	};
};


int main()
{
	angle::unitDeg();


	size_t level = 4;
	size_t fullSearchLevel = level;
	if (level > 5) fullSearchLevel = level-5;
	else fullSearchLevel = 0;

	sGrid minFace;
	unsigned short minLocation;
	scaledFP minValue(0,0);
	test testFunc;
	unsigned short minEdge = 1;
//	minValue = sGrid::minNodeSearch(testFunc, level, minFace, minLocation);
//	sGrid::searchMinPoint(testFunc, level, minFace, minLocation, minValue);
	sGrid::fastMinEdgeSearch(testFunc, 0, level, minFace, minLocation, minEdge, minValue);
	scaledFP minValue2 = testFunc(level, minFace, sGrid::otherNodeCode(minEdge, minLocation));


	std::cout << std::endl;
	std::cout << "grid minimum search algorithm result: ";
	std::cout << std::endl;
	std::cout << minFace << minLocation << "," << minEdge << " - " << minFace.toPolar(level,minLocation) << ": " << minValue;
	std::cout << " -- " << sGrid::otherNodeCode(minEdge, minLocation) << " - " << minFace.toPolar(level,sGrid::otherNodeCode(minEdge, minLocation)) << ": " << minValue2;
	std::cout << std::endl;
	std::cout << "node values of adjacent nodes: " << std::endl;
	std::list<std::pair<sGrid, unsigned short> > minFaceNeighborNodes = minFace.nodeNeighborNodes(level, minLocation);
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = minFaceNeighborNodes.begin(); it != minFaceNeighborNodes.end(); ++it)
	{
		std::cout << it->first << it->second << " Value: " << testFunc(level, it->first, it->second) << " - " << it->first.toPolar(level,it->second) << std::endl;
	}
	std::cout << std::endl;


	std::cout << "full grid search inside level " << fullSearchLevel << " face around previous found minimum node at level " << level;
	std::cout << std::endl;

	sGrid TMP(minFace);
	sGrid TMP2(minFace);
	unsigned short loc = minLocation;

	scaledFP dummy = minValue;

	sGrid::subGridScanner sTMP(TMP.begin(level,fullSearchLevel));
	while (sTMP != TMP.end())
	{
		dummy = testFunc(level,*sTMP,1);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=*sTMP;
			loc=1;
		};
		dummy = testFunc(level,*sTMP,2);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=*sTMP;
			loc=2;
		};
		dummy = testFunc(level,*sTMP,3);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=*sTMP;
			loc=3;
		};
		++sTMP;
	}
	std::cout << TMP2 << loc << " minValue: " << minValue << " - " << TMP2.toPolar(level,loc) << std::endl;

	std::cout << "node values of adjacent nodes: " << std::endl;
	std::list<std::pair<sGrid, unsigned short> > TMP2neighborNodes = TMP2.nodeNeighborNodes(level, loc);
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = TMP2neighborNodes.begin(); it != TMP2neighborNodes.end(); ++it)
	{
		std::cout << it->first << it->second << " Value: " << testFunc(level, it->first, it->second) << " - " << it->first.toPolar(level,it->second) << std::endl;
	}
	std::cout << "node values in the same face: " << std::endl;
	std::cout << TMP2 << 1 << " Value: " << testFunc(level,TMP2,1) << std::endl;
	std::cout << TMP2 << 2 << " Value: " << testFunc(level,TMP2,2) << std::endl;
	std::cout << TMP2 << 3 << " Value: " << testFunc(level,TMP2,3) << std::endl;
	std::cout << "node values in neighbor faces: " << std::endl;
	std::cout << TMP2.neighborFace(level,1) << 1 << " Value: " << testFunc(level,TMP2.neighborFace(level,1),1) << std::endl;
	std::cout << TMP2.neighborFace(level,1) << 2 << " Value: " << testFunc(level,TMP2.neighborFace(level,1),2) << std::endl;
	std::cout << TMP2.neighborFace(level,1) << 3 << " Value: " << testFunc(level,TMP2.neighborFace(level,1),3) << std::endl;
	std::cout << TMP2.neighborFace(level,2) << 1 << " Value: " << testFunc(level,TMP2.neighborFace(level,2),1) << std::endl;
	std::cout << TMP2.neighborFace(level,2) << 2 << " Value: " << testFunc(level,TMP2.neighborFace(level,2),2) << std::endl;
	std::cout << TMP2.neighborFace(level,2) << 3 << " Value: " << testFunc(level,TMP2.neighborFace(level,2),3) << std::endl;
	std::cout << TMP2.neighborFace(level,3) << 1 << " Value: " << testFunc(level,TMP2.neighborFace(level,3),1) << std::endl;
	std::cout << TMP2.neighborFace(level,3) << 2 << " Value: " << testFunc(level,TMP2.neighborFace(level,3),2) << std::endl;
	std::cout << TMP2.neighborFace(level,3) << 3 << " Value: " << testFunc(level,TMP2.neighborFace(level,3),3) << std::endl;
	std::cout << std::endl;





	std::cout << "generate new sGrid points from sPolar point - single point, arbitrary coordinates: " << std::endl;
	fp_type theta = 80*pi/91;
	fp_type phi = 54*pi/41;

	sPolar polarPoint(theta, phi);
	sGrid polarToGrid(polarPoint, sGrid::getAccuracyBits(), sGrid::fastMinSearch<sGrid::polarDistanceToRef>);

	std::cout << "polar: " << polarPoint << " grid: " << polarToGrid << "-" << polarToGrid.toPolar();
	std::cout << " difference: (" << polarPoint.getTheta()-polarToGrid.toPolar().getTheta();
	std::cout << "," << polarPoint.getPhi()-polarToGrid.toPolar().getPhi() << ")";
	std::cout << "," << sPolar::distance(polarPoint,polarToGrid.toPolar()) << std::endl;
	std::cout << std::endl;

	level = sGrid::getAccuracyBits();
	level = 16;

	size_t pointCount = 1000;
	std::srand(0);
	std::cout << "generate " << pointCount << " new sGrid points from sPolar point - random coordinates" << std::endl;
	std::vector<sPolar> polarPoints(pointCount);
    std::for_each(polarPoints.begin(), polarPoints.end(), [](sPolar& P) {P = sPolar(pi*rand()/RAND_MAX, two_pi*rand()/RAND_MAX);});

    auto t1 = std::chrono::high_resolution_clock::now();
	std::vector<sGrid> gridPointsFastSearch(pointCount);
	for (size_t it = 0; it < pointCount; ++it)
	{
	    gridPointsFastSearch[it] = sGrid(polarPoints[it], level, sGrid::fastMinSearch<sGrid::polarDistanceToRef>);
	}
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fastSearchTime = t2 - t1;

    t1 = std::chrono::high_resolution_clock::now();
	std::vector<sGrid> gridPointsRobustSearch(pointCount);
	for (size_t it = 0; it < pointCount; ++it)
	{
	    gridPointsRobustSearch[it] = sGrid(polarPoints[it], level, sGrid::minSearch<sGrid::polarDistanceToRef>);
	}
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> robustSearchTime = t2 - t1;

	fp_type maxDifference = 0;
	size_t max_it_fastSearch = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
		if ( maxDifference < sPolar::distance(polarPoints[it],gridPointsFastSearch[it].toPolar()) )
		{
			maxDifference = sPolar::distance(polarPoints[it],gridPointsFastSearch[it].toPolar());
			max_it_fastSearch = it;
		}
	}
	std::cout << "sGrid point with highest distance to given sPolar using fastMinSearch - execution time: " << fastSearchTime.count() << "ms" << std::endl;
	std::cout << "polar[" << max_it_fastSearch << "]: " << polarPoints[max_it_fastSearch] << " grid: " << gridPointsFastSearch[max_it_fastSearch] << "-" << gridPointsFastSearch[max_it_fastSearch].toPolar();
	std::cout << " difference: (" << polarPoints[max_it_fastSearch].getTheta()-gridPointsFastSearch[max_it_fastSearch].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_fastSearch].getPhi()-gridPointsFastSearch[max_it_fastSearch].toPolar().getPhi() << ")";
	std::cout << "," << sPolar::distance(polarPoints[max_it_fastSearch],gridPointsFastSearch[max_it_fastSearch].toPolar()) << std::endl;

	maxDifference = 0;
	max_it_fastSearch = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
		if ( maxDifference < sPolar::distance(polarPoints[it],gridPointsRobustSearch[it].toPolar()) )
		{
			maxDifference = sPolar::distance(polarPoints[it],gridPointsRobustSearch[it].toPolar());
			max_it_fastSearch = it;
		}
	}
	std::cout << "sGrid point with highest distance to given sPolar using minSearch - execution time: " << robustSearchTime.count() << "ms" << std::endl;
	std::cout << "polar[" << max_it_fastSearch << "]: " << polarPoints[max_it_fastSearch] << " grid: " << gridPointsRobustSearch[max_it_fastSearch] << "-" << gridPointsRobustSearch[max_it_fastSearch].toPolar();
	std::cout << " difference: (" << polarPoints[max_it_fastSearch].getTheta()-gridPointsRobustSearch[max_it_fastSearch].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_fastSearch].getPhi()-gridPointsRobustSearch[max_it_fastSearch].toPolar().getPhi() << ")";
	std::cout << "," << sPolar::distance(polarPoints[max_it_fastSearch],gridPointsRobustSearch[max_it_fastSearch].toPolar()) << std::endl;

	scaledFP maxDifference2 = 0;
	max_it_fastSearch = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
		if ( maxDifference2 < sGrid::distance(gridPointsFastSearch[it],gridPointsRobustSearch[it]) )
		{
			maxDifference2 = sGrid::distance(gridPointsFastSearch[it],gridPointsRobustSearch[it]);
			max_it_fastSearch = it;
		}
	}
	std::cout << "sGrid point with highest difference between fastMinSearch and minSearch: " << std::endl;
	std::cout << "polar[" << max_it_fastSearch << "]: " << polarPoints[max_it_fastSearch] << std::endl;
	std::cout << "fast search  : " << gridPointsFastSearch[max_it_fastSearch] << "-" << gridPointsFastSearch[max_it_fastSearch].toPolar();
	std::cout << " difference to polar: (" << polarPoints[max_it_fastSearch].getTheta()-gridPointsFastSearch[max_it_fastSearch].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_fastSearch].getPhi()-gridPointsFastSearch[max_it_fastSearch].toPolar().getPhi() << ")";
	std::cout << "," << sPolar::distance(polarPoints[max_it_fastSearch],gridPointsFastSearch[max_it_fastSearch].toPolar()) << std::endl;
	std::cout << "robust search: " << gridPointsRobustSearch[max_it_fastSearch] << "-" << gridPointsRobustSearch[max_it_fastSearch].toPolar();
	std::cout << " difference to polar: (" << polarPoints[max_it_fastSearch].getTheta()-gridPointsRobustSearch[max_it_fastSearch].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_fastSearch].getPhi()-gridPointsRobustSearch[max_it_fastSearch].toPolar().getPhi() << ")";
	std::cout << "," << sPolar::distance(polarPoints[max_it_fastSearch],gridPointsRobustSearch[max_it_fastSearch].toPolar()) << std::endl;
	std::cout << "difference between grid points: (" << gridPointsFastSearch[max_it_fastSearch].toPolar().getTheta()-gridPointsRobustSearch[max_it_fastSearch].toPolar().getTheta();
	std::cout << "," << gridPointsFastSearch[max_it_fastSearch].toPolar().getPhi()-gridPointsRobustSearch[max_it_fastSearch].toPolar().getPhi() << ")";
	std::cout << "," << sGrid::distance(gridPointsFastSearch[max_it_fastSearch],gridPointsRobustSearch[max_it_fastSearch]) << std::endl;



#ifdef DEBUG

	std::cout << "checking polar coordinates in Octant0: ";
	std::cout << std::endl;
	sGrid O0(std::vector<unsigned short>({0}));
	std::cout << "O0 center:   " << O0.toPolar(0,0);
	std::cout << " O0 front:   " << O0.toPolar(0,1);
	std::cout << "          O0 west:   " << O0.toPolar(0,2);
	std::cout << "            O0 north:   " << O0.toPolar(0,3);
	std::cout << std::endl;
	O0.setExtend(1,0);
	std::cout << "O00 center:  " << O0.toPolar(1,0);
	std::cout << " O00 west:   " << O0.toPolar(1,1);
	std::cout << "           O00 front: " << O0.toPolar(1,2);
	std::cout << "         O00 south:  " << O0.toPolar(1,3);
	std::cout << std::endl;
	O0.setExtend(2,0);
	std::cout << "O000 center: " << O0.toPolar(2,0);
	std::cout << " O000 front: " << O0.toPolar(2,1);
	std::cout << " O000 west: " << O0.toPolar(2,2);
	std::cout << " O000 north: " << O0.toPolar(2,3);
	std::cout << std::endl;
	std::cout << std::endl;

	std::cout << "checking edge lengths of the same edge in different faces: ";
	std::cout << std::endl;
	sGrid F0(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,2,1,1,3,3}));
	sGrid F1(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,2,1,0,2,2}));
	sGrid F2(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,0,3,3,1,1}));
	sGrid F3(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,2,1,1,3,0}));
	std::cout << "F0: " << F0;
	std::cout << std::endl;
	std::cout << "F1: " << F1;
	std::cout << std::endl;
	std::cout << "F2: " << F2;
	std::cout << std::endl;
	std::cout << "F3: " << F3;
	std::cout << std::endl;
	std::cout << "level 30 F0 edgeLength 1: " << F0.edgeLength(30,1);
	std::cout << " edgeLength 2: " << F0.edgeLength(30,2);
	std::cout << " edgeLength 3: " << F0.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "level 30 F1 edgeLength 1: " << F1.edgeLength(30,1);
	std::cout << " edgeLength 2: " << F1.edgeLength(30,2);
	std::cout << " edgeLength 3: " << F1.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "level 30 F2 edgeLength 1: " << F2.edgeLength(30,1);
	std::cout << " edgeLength 2: " << F2.edgeLength(30,2);
	std::cout << " edgeLength 3: " << F2.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "level 30 F3 edgeLength 1: " << F3.edgeLength(30,1);
	std::cout << " edgeLength 2: " << F3.edgeLength(30,2);
	std::cout << " edgeLength 3: " << F3.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "lengths of same edges in different faces: ";
	std::cout << std::endl;
	std::cout << "F0 edge1 - F1 edge1: " << F0.edgeLength(30,1) - F1.edgeLength(30,1);
	std::cout << std::endl;
	std::cout << "F0 edge2 - F2 edge2: " << F0.edgeLength(30,2) - F2.edgeLength(30,2);
	std::cout << std::endl;
	std::cout << "F0 edge3 - F3 edge3: " << F0.edgeLength(30,3) - F3.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "lengths of different nearby edges: ";
	std::cout << std::endl;
	std::cout << "F2 edge1 - F1 edge1: " << F2.edgeLength(30,1) - F1.edgeLength(30,1);
	std::cout << std::endl;
	std::cout << "F2 edge3 - F1 edge3: " << F2.edgeLength(30,3) - F1.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "F3 edge1 - F2 edge1: " << F3.edgeLength(30,1) - F2.edgeLength(30,1);
	std::cout << std::endl;
	std::cout << "F3 edge3 - F2 edge3: " << F3.edgeLength(30,3) - F2.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "F1 edge1 - F3 edge1: " << F1.edgeLength(30,1) - F3.edgeLength(30,1);
	std::cout << std::endl;
	std::cout << "polar coordinates of the same node in different faces: ";
	std::cout << std::endl;
	sPolar F0p = F0.toPolar(30,2);
	sPolar F1p = F1.toPolar(30,3);
	std::cout << "[F0,2] polar: " << F0p;
	std::cout << " [F1,3] polar: " << F1p;
	std::cout << " difference: (" << F0p.getTheta()-F1p.getTheta() << "," << F0p.getPhi()-F1p.getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,3] polar: " << F0.toPolar(30,3);
	std::cout << " [F1,2] polar: " << F1.toPolar(30,2);
	std::cout << " difference: (" << F0.toPolar(30,3).getTheta()-F1.toPolar(30,2).getTheta() << "," << F0.toPolar(30,3).getPhi()-F1.toPolar(30,2).getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,1] polar: " << F0.toPolar(30,1);
	std::cout << " [F2,3] polar: " << F2.toPolar(30,3);
	std::cout << " difference: (" << F0.toPolar(30,1).getTheta()-F2.toPolar(30,3).getTheta() << "," << F0.toPolar(30,1).getPhi()-F2.toPolar(30,3).getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,3] polar: " << F0.toPolar(30,3);
	std::cout << " [F2,1] polar: " << F2.toPolar(30,1);
	std::cout << " difference: (" << F0.toPolar(30,3).getTheta()-F2.toPolar(30,1).getTheta() << "," << F0.toPolar(30,3).getPhi()-F2.toPolar(30,1).getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,1] polar: " << F0.toPolar(30,1);
	std::cout << " [F3,2] polar: " << F3.toPolar(30,2);
	std::cout << " difference: (" << F0.toPolar(30,1).getTheta()-F3.toPolar(30,2).getTheta() << "," << F0.toPolar(30,1).getPhi()-F3.toPolar(30,2).getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,2] polar: " << F0.toPolar(30,2);
	std::cout << " [F3,1] polar: " << F3.toPolar(30,1);
	std::cout << " difference: (" << F0.toPolar(30,2).getTheta()-F3.toPolar(30,1).getTheta() << "," << F0.toPolar(30,2).getPhi()-F3.toPolar(30,1).getPhi() << ")";
	std::cout << std::endl;
	std::cout << std::endl;

	size_t refLevel = 29;
	std::cout << "high level nodes polar coordinates in flat and spherical geometry: ";
	std::cout << "{" << F0.calcFaceGeometry(refLevel)[1];
	std::cout << "," << F0.calcFaceGeometry(refLevel)[2];
	std::cout << "," << F0.calcFaceGeometry(refLevel)[3] << "}";
	std::cout << std::endl;
	sGrid::inFacePolar testSpher(F0.calcFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,0));
	sGrid::inFacePolar testFlat(F0.calcFlatFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,0));
	std::cout << "[F0,0] level 30 local polar (1,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (1,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (2,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (2,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (3,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (3,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (1,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (1,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (2,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (2,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (3,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (3,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (1,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (1,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (2,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (2,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (3,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (3,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (1,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (1,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (2,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (2,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (3,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (3,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	std::cout << std::endl;



	std::cout << "collecting neighbor faces & nodes for special points: ";
	std::cout << std::endl;
	std::list<sGrid> starF = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeNeighborFaces(2,2);
	std::list<std::pair<sGrid, unsigned short> > starN = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeNeighborNodes(2,2);
	std::list<std::pair<sGrid, unsigned short> > starE = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeConnectedEdges(2,2);
	std::list<std::pair<sGrid, unsigned short> > starO = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeOuterRingEdges(2,2);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,1,1,1,1})) << 2 << " ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeNeighborFaces(2,3);
	starN = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeNeighborNodes(2,3);
	starE = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeConnectedEdges(2,3);
	starO = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,1,1,1,1})) << 3 << " ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeNeighborFaces(2,3);
	starN = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeNeighborNodes(2,3);
	starE = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeConnectedEdges(2,3);
	starO = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,0,0,0,1})) << 3 << " ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeNeighborFaces(2,2);
	starN = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeNeighborNodes(2,2);
	starE = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeConnectedEdges(2,2);
	starO = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeOuterRingEdges(2,2);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,0,0,0,1})) << 2 << " ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeNeighborFaces(2,1);
	starN = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeNeighborNodes(2,1);
	starE = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeConnectedEdges(2,1);
	starO = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeOuterRingEdges(2,1);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,1,0,1,1})) << 1 << " ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeNeighborFaces(2,3);
	starN = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeNeighborNodes(2,3);
	starE = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeConnectedEdges(2,3);
	starO = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,1,0,1,1})) << 3 << " ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid(std::vector<bool>({0,0,0,0,0,1,1})).nodeNeighborFaces(2,3);
	starN = sGrid(std::vector<bool>({0,0,0,0,0,1,1})).nodeNeighborNodes(2,3);
	starE = sGrid(std::vector<bool>({0,0,0,0,0,1,1})).nodeConnectedEdges(2,3);
	starO = sGrid(std::vector<bool>({0,0,0,0,0,1,1})).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,0,0,1,1})) << 3 << " ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;
	std::cout << std::endl;



	std::cout << "Comparing distances between grid coordinates and polar coordinates (up to level 4):" << std::endl;
	//initialize Q as all faces in levels 0,1,2,3
	sGrid Q[512];
	for (size_t it0 = 0; it0<8; it0++)
	{
		for (size_t it1 = 0; it1<4; it1++)
		{
			for (size_t it2 = 0; it2<4; it2++)
			{
				for (size_t it3 = 0; it3<4; it3++)
				{
					Q[it0+8*it1+32*it2+128*it3].setExtend(0,it0);
					Q[it0+8*it1+32*it2+128*it3].setExtend(1,it1);
					Q[it0+8*it1+32*it2+128*it3].setExtend(2,it2);
					Q[it0+8*it1+32*it2+128*it3].setExtend(3,it3);
				}
			}
		}
	}

	size_t l1_max=3;
	size_t l2_max=4;
	size_t i_max=450;
	size_t j_max=453;

	/*
	        fp_type difference = 0;
			fp_type max_difference = 0;
			for (size_t l1=0; l1<5; l1++)
			{
				for (size_t l2=0; l2<5; l2++)
				{
					for (size_t i = 0; i<512; i++)
					{
						for (size_t j = 0; j<512; j++)
						{
							difference = std::abs(SPIRID::sPolar::distance(Q[i].toPolar(l1),Q[j].toPolar(l2)) - SPIRID::sGrid::distance(l1,Q[i],l2,Q[j]));
							if (std::isnan(difference)) std::cerr << "nan" << std::endl;
							if( difference > max_difference)
							{
								max_difference = difference;
								i_max = i;
								j_max = j;
								l1_max = l1;
								l2_max = l2;
							}
						}
					}
				}
			}
	*/

	std::cout << "(" << l1_max << "," << l2_max << ")";
	std::cout << "(" << i_max << "," << j_max << "): ";
	std::cout << Q[i_max] << Q[i_max].toPolar(l1_max) << Q[j_max] << Q[j_max].toPolar(l2_max) << ": ";
	std::cout << "polar: " << SPIRID::sPolar::distance(Q[i_max].toPolar(l1_max),Q[j_max].toPolar(l2_max)) << ",";
	std::cout << " grid: " << SPIRID::sGrid::distance(l1_max,Q[i_max],l2_max,Q[j_max]) << ",";
	std::cout << " difference: " << SPIRID::sPolar::distance(Q[i_max].toPolar(l1_max),Q[j_max].toPolar(l2_max)) - SPIRID::sGrid::distance(l1_max,Q[i_max],l2_max,Q[j_max]);
	std::cout << std::endl;
	std::cout << std::endl;

#endif //DEBUG

	return 0;
}
