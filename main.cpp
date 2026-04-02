/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <cmath>

#include <SPIRID.h>
using namespace SPIRID;

int main()
{
	angle::unitPi();


	size_t level = 4;
	size_t fullSearchLevel = level;
	if (level > 5) fullSearchLevel = level-5;
	else fullSearchLevel = 0;
//	funcGraphPoint<> F(sGrid::searchMinPoint(level,SPIRID::sGrid::test));
    sGrid minFace;
    unsigned short minLocation;
    scaledFP minValue(0,0);
    sGrid::minNodeSearch(level,SPIRID::sGrid::test, minFace, minLocation, minValue);

	std::cout << std::endl;
    std::cout << "standard grid minimum search algorithm result:";
	std::cout << std::endl;
	std::cout << minFace << minLocation << " - " << minFace.toPolar(level,minLocation) << ": " << minValue << std::endl;
//	std::cout << F.dPoint.first << F.dPoint.second << " - " << F.dPoint.first.toPolar(level,F.dPoint.second) << ": " << F.fValue << std::endl;
	std::cout << std::endl;


    std::cout << "full grid search inside level " << fullSearchLevel << " face around previous found minimum node at level " << level;
	std::cout << std::endl;

/*	sGrid TMP(F.dPoint.first);
	sGrid TMP2(F.dPoint.first);
	unsigned short loc2 = F.dPoint.second;

	scaledFP minValue = F.fValue;
	scaledFP dummy = F.fValue;
*/
	sGrid TMP(minFace);
	sGrid TMP2(minFace);
	unsigned short loc2 = minLocation;

//	scaledFP minValue = minValue;
	scaledFP dummy = minValue;

	sGrid::subGridScanner sTMP(TMP.begin(level,fullSearchLevel));
	while (sTMP != TMP.end())
	{
		dummy = sGrid::test(level,*sTMP,1);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=*sTMP;
			loc2=1;
		};
		dummy = sGrid::test(level,*sTMP,2);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=*sTMP;
			loc2=2;
		};
		dummy = sGrid::test(level,*sTMP,3);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=*sTMP;
			loc2=3;
		};
		++sTMP;
	}
	std::cout << TMP2 << loc2 << " minValue: " << minValue << std::endl;
    std::cout << "node values in the same face: " << std::endl;
	std::cout << TMP2 << 1 << " Value: " << sGrid::test(level,TMP2,1) << std::endl;
	std::cout << TMP2 << 2 << " Value: " << sGrid::test(level,TMP2,2) << std::endl;
	std::cout << TMP2 << 3 << " Value: " << sGrid::test(level,TMP2,3) << std::endl;
    std::cout << "node values in neighbor faces: " << std::endl;
	std::cout << TMP2.neighborFace(level,1) << 1 << " Value: " << sGrid::test(level,TMP2.neighborFace(level,1),1) << std::endl;
	std::cout << TMP2.neighborFace(level,1) << 2 << " Value: " << sGrid::test(level,TMP2.neighborFace(level,1),2) << std::endl;
	std::cout << TMP2.neighborFace(level,1) << 3 << " Value: " << sGrid::test(level,TMP2.neighborFace(level,1),3) << std::endl;
	std::cout << TMP2.neighborFace(level,2) << 1 << " Value: " << sGrid::test(level,TMP2.neighborFace(level,2),1) << std::endl;
	std::cout << TMP2.neighborFace(level,2) << 2 << " Value: " << sGrid::test(level,TMP2.neighborFace(level,2),2) << std::endl;
	std::cout << TMP2.neighborFace(level,2) << 3 << " Value: " << sGrid::test(level,TMP2.neighborFace(level,2),3) << std::endl;
	std::cout << TMP2.neighborFace(level,3) << 1 << " Value: " << sGrid::test(level,TMP2.neighborFace(level,3),1) << std::endl;
	std::cout << TMP2.neighborFace(level,3) << 2 << " Value: " << sGrid::test(level,TMP2.neighborFace(level,3),2) << std::endl;
	std::cout << TMP2.neighborFace(level,3) << 3 << " Value: " << sGrid::test(level,TMP2.neighborFace(level,3),3) << std::endl;




    level = 0;
    sGrid X(std::vector<unsigned short>({0}));
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({1});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    level = 1;
    X = std::vector<unsigned short>({0,3});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,0});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,2});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({1,3});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({1,0});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({1,2});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;

    level = 3;
    X = std::vector<unsigned short>({0,3,2,0});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,3,2,3});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,3,0,1});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,3,0,0});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,3,0,3});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,3,2,1});
	std::cout << X << "[1,2,3]: (" << SPIRID::sGrid::test(level,X,1) << ","  << SPIRID::sGrid::test(level,X,2) << "," << SPIRID::sGrid::test(level,X,3) << ")";
	std::cout << std::endl;



    X = std::vector<unsigned short>({0,3,2,3,0});
	std::cout << std::endl;
	std::cout << X << 3 << ": " << SPIRID::sGrid::test(4,X,3);
	std::cout << std::endl;
	std::cout << X << 2 << ": " << SPIRID::sGrid::test(4,X,2);
	std::cout << std::endl;
	std::cout << X << 1 << ": " << SPIRID::sGrid::test(4,X,1);
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,3,2,3});
	std::cout << X << 3 << ": " << SPIRID::sGrid::test(3,X,3);
	std::cout << std::endl;
	std::cout << X << 2 << ": " << SPIRID::sGrid::test(3,X,2);
	std::cout << std::endl;
	std::cout << X << 1 << ": " << SPIRID::sGrid::test(3,X,1);
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,3,2,0});
	std::cout << X << 3 << ": " << SPIRID::sGrid::test(3,X,3);
	std::cout << std::endl;
	std::cout << X << 2 << ": " << SPIRID::sGrid::test(3,X,2);
	std::cout << std::endl;
	std::cout << X << 1 << ": " << SPIRID::sGrid::test(3,X,1);
	std::cout << std::endl;
    X = std::vector<unsigned short>({0,3,2,1});
	std::cout << X << 3 << ": " << SPIRID::sGrid::test(3,X,3);
	std::cout << std::endl;
	std::cout << X << 2 << ": " << SPIRID::sGrid::test(3,X,2);
	std::cout << std::endl;
	std::cout << X << 1 << ": " << SPIRID::sGrid::test(3,X,1);
	std::cout << std::endl;
    X = sGrid(std::vector<unsigned short>({0,3,0,1,0}));
	std::cout << X << 3 << ": " << SPIRID::sGrid::test(4,X,3);
	std::cout << std::endl;
	std::cout << X << 2 << ": " << SPIRID::sGrid::test(4,X,2);
	std::cout << std::endl;
	std::cout << X << 1 << ": " << SPIRID::sGrid::test(4,X,1);
	std::cout << std::endl;
    X = sGrid(std::vector<unsigned short>({0,3,0,1}));
	std::cout << X << 3 << ": " << SPIRID::sGrid::test(3,X,3);
	std::cout << std::endl;
	std::cout << X << 2 << ": " << SPIRID::sGrid::test(3,X,2);
	std::cout << std::endl;
	std::cout << X << 1 << ": " << SPIRID::sGrid::test(3,X,1);
	std::cout << std::endl;






#ifdef DEBUG
    std::cout << "generate new sGrid point from sPolar point: " << std::endl;
    sPolar polarPoint(80*pi/91, 54*pi/41);
    sGrid polarToGrid(polarPoint);
    std::cout << "polar: " << polarPoint << " grid: " << polarToGrid << "-" << polarToGrid.toPolar();
    std::cout << " difference: (" << polarPoint.getTheta()-polarToGrid.toPolar().getTheta();
    std::cout << "," << polarPoint.getPhi()-polarToGrid.toPolar().getPhi() << ")" << std::endl;



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
	std::cout << "{" << F0.calcFaceGeometry(refLevel).SinSqE[0];
	std::cout << "," << F0.calcFaceGeometry(refLevel).SinSqE[1];
	std::cout << "," << F0.calcFaceGeometry(refLevel).SinSqE[2] << "}";
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
	std::list<sGrid> starF = sGrid({0,0,0,1,1,1,1}).nodeNeighborFaces(2,2);
	std::list<std::pair<sGrid, unsigned short> > starN = sGrid({0,0,0,1,1,1,1}).nodeNeighborNodes(2,2);
	std::list<std::pair<sGrid, unsigned short> > starE = sGrid({0,0,0,1,1,1,1}).nodeConnectedEdges(2,2);
	std::list<std::pair<sGrid, unsigned short> > starO = sGrid({0,0,0,1,1,1,1}).nodeOuterRingEdges(2,2);
	std::cout << "Nodes           : ";
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

	starF = sGrid({0,0,0,1,1,1,1}).nodeNeighborFaces(2,3);
	starN = sGrid({0,0,0,1,1,1,1}).nodeNeighborNodes(2,3);
	starE = sGrid({0,0,0,1,1,1,1}).nodeConnectedEdges(2,3);
	starO = sGrid({0,0,0,1,1,1,1}).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : ";
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

	starF = sGrid({0,0,0,0,0,0,1}).nodeNeighborFaces(2,3);
	starN = sGrid({0,0,0,0,0,0,1}).nodeNeighborNodes(2,3);
	starE = sGrid({0,0,0,0,0,0,1}).nodeConnectedEdges(2,3);
	starO = sGrid({0,0,0,0,0,0,1}).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : ";
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

	starF = sGrid({0,0,0,0,0,0,1}).nodeNeighborFaces(2,2);
	starN = sGrid({0,0,0,0,0,0,1}).nodeNeighborNodes(2,2);
	starE = sGrid({0,0,0,0,0,0,1}).nodeConnectedEdges(2,2);
	starO = sGrid({0,0,0,0,0,0,1}).nodeOuterRingEdges(2,2);
	std::cout << "Nodes           : ";
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

	starF = sGrid({0,0,0,1,0,1,1}).nodeNeighborFaces(2,1);
	starN = sGrid({0,0,0,1,0,1,1}).nodeNeighborNodes(2,1);
	starE = sGrid({0,0,0,1,0,1,1}).nodeConnectedEdges(2,1);
	starO = sGrid({0,0,0,1,0,1,1}).nodeOuterRingEdges(2,1);
	std::cout << "Nodes           : ";
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

	starF = sGrid({0,0,0,1,0,1,1}).nodeNeighborFaces(2,3);
	starN = sGrid({0,0,0,1,0,1,1}).nodeNeighborNodes(2,3);
	starE = sGrid({0,0,0,1,0,1,1}).nodeConnectedEdges(2,3);
	starO = sGrid({0,0,0,1,0,1,1}).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : ";
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

	starF = sGrid({0,0,0,0,0,1,1}).nodeNeighborFaces(2,3);
	starN = sGrid({0,0,0,0,0,1,1}).nodeNeighborNodes(2,3);
	starE = sGrid({0,0,0,0,0,1,1}).nodeConnectedEdges(2,3);
	starO = sGrid({0,0,0,0,0,1,1}).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : ";
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
