#include <cmath>
#include <limits>
#include <SPIRID.h>

//accuracy settings
size_t SPIRID::sGrid::accuracyBits       = std::numeric_limits<fp_type>::digits;
size_t SPIRID::sGrid::minDepthFlatApprox = SPIRID::sGrid::accuracyBits/2+1;
size_t SPIRID::sGrid::minScaleSinXToX    = SPIRID::sGrid::accuracyBits/3+1;
void SPIRID::sGrid::setAccuracyBits(size_t bitCount)
{
	if (bitCount>std::numeric_limits<fp_type>::digits)
	{
		std::cerr << "warning: floating point type supports only " << std::numeric_limits<fp_type>::digits << " binary digits." << std::endl;
		bitCount = std::numeric_limits<fp_type>::digits;
	}
	SPIRID::sGrid::accuracyBits = bitCount;
	SPIRID::sGrid::minDepthFlatApprox = SPIRID::sGrid::accuracyBits/2+1;
	SPIRID::sGrid::minScaleSinXToX    = SPIRID::sGrid::accuracyBits/3+1;
}



//sGrid constructors
SPIRID::sGrid::sGrid(const std::vector<bool>& P) : gridCode(P)
{
	size_t size = gridCode.size();
	if (size<3) gridCode.resize(3);
	if (size%2==0) gridCode.resize(size+1);
}



//get faceCode at a certain level
unsigned short SPIRID::sGrid::at(size_t level) const
{
	if (2*level+3>gridCode.size()) return 0;
	else return operator [] (level);
};
unsigned short SPIRID::sGrid::operator [] (size_t level) const
{
	if (level==0) return gridCode[0]+2*gridCode[1]+4*gridCode[2];
	return gridCode[2*level+1]+2*gridCode[2*level+2];
};
//set faceCode at a certain level
void SPIRID::sGrid::setExtend(size_t level, unsigned short faceCode)
{
	if (2*level+3>gridCode.size()) gridCode.resize(2*level+3);
	set(level, faceCode);
}
void SPIRID::sGrid::set(size_t level, unsigned short faceCode)
{
	if (level==0)
	{
		switch (faceCode)
		{
		case 0 : {
			gridCode[0]=false;
			gridCode[1]=false;
			gridCode[2]=false;
			return;
		}
		case 1 : {
			gridCode[0]=true;
			gridCode[1]=false;
			gridCode[2]=false;
			return;
		}
		case 2 : {
			gridCode[0]=false;
			gridCode[1]=true;
			gridCode[2]=false;
			return;
		}
		case 3 : {
			gridCode[0]=true;
			gridCode[1]=true;
			gridCode[2]=false;
			return;
		}
		case 4 : {
			gridCode[0]=false;
			gridCode[1]=false;
			gridCode[2]=true;
			return;
		}
		case 5 : {
			gridCode[0]=true;
			gridCode[1]=false;
			gridCode[2]=true;
			return;
		}
		case 6 : {
			gridCode[0]=false;
			gridCode[1]=true;
			gridCode[2]=true;
			return;
		}
		case 7 : {
			gridCode[0]=true;
			gridCode[1]=true;
			gridCode[2]=true;
			return;
		}
		default: {
			gridCode[0]=true;
			gridCode[1]=true;
			gridCode[2]=true;
			return;
		}
		}
	}
	else
	{
		switch (faceCode)
		{
		case 0 : {
			gridCode[2*level+1]=false;
			gridCode[2*level+2]=false;
			return;
		}
		case 1 : {
			gridCode[2*level+1]=true;
			gridCode[2*level+2]=false;
			return;
		}
		case 2 : {
			gridCode[2*level+1]=false;
			gridCode[2*level+2]=true;
			return;
		}
		case 3 : {
			gridCode[2*level+1]=true;
			gridCode[2*level+2]=true;
			return;
		}
		default: {
			gridCode[2*level+1]=true;
			gridCode[2*level+2]=true;
			return;
		}
		}
	}
}



//calculate geometry (scaled edge lengths) of the face
SPIRID::sGrid::faceGeometry SPIRID::sGrid::calcFaceGeometry(size_t level) const
{
	//edge length a as SinaSq = 2^(2*level)*Sin(a)^2 --> level 0: a=Pi/2, SinaSq=1; level 1, a=Pi/4: SinaSq=2
	faceGeometry faceGeom({1,1,1});
	size_t max_level_it = std::min(level,minDepthFlatApprox);
	unsigned short fCode;

	for (size_t level_it=0; level_it<max_level_it; level_it++)
	{
		fCode = at(level_it+1);
		stepupFaceGeometryFrom(level_it, fCode, faceGeom);
	};
	return faceGeom;
}
//update geometry (scaled edge lengths) of a face: step-up to the next grid level
void SPIRID::sGrid::stepupFaceGeometryFrom(size_t level, unsigned short fCode, faceGeometry& faceGeom)
{
	scaleExp_type scale = 2*level;

	fp_type SinaSq = faceGeom.SinSqE[0];
	fp_type SinbSq = faceGeom.SinSqE[1];
	fp_type SincSq = faceGeom.SinSqE[2];

	fp_type ScaledSina2Sq = calcEdgeBisection(scale,SinaSq);
	fp_type ScaledSinb2Sq = calcEdgeBisection(scale,SinbSq);
	fp_type ScaledSinc2Sq = calcEdgeBisection(scale,SincSq);

	switch (fCode)
	{
	case 0 :
	{
		faceGeom.SinSqE[0] = (        2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scale) - 2)
		                              - 2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq + ScaledSinc2Sq,-scale) - 6)
		                              + 4*(SinaSq + SinbSq + SincSq - ScaledSinc2Sq)
		                     )/
		                     ((LDEXP(ScaledSinb2Sq,-scale) - 4)*(LDEXP(ScaledSinc2Sq,-scale) - 4));
		faceGeom.SinSqE[1] = (   - 2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq - ScaledSinc2Sq,-scale) + 2)
		                         - 2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scale) - 6)
		                         + 4*(SinaSq + SinbSq + SincSq - ScaledSinc2Sq))/
		                     ((LDEXP(ScaledSina2Sq,-scale) - 4)*(LDEXP(ScaledSinc2Sq,-scale) - 4));
		faceGeom.SinSqE[2] = (      2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq - ScaledSinc2Sq,-scale) - 2)
		                            - 2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scale) + 2)
		                            + 4*(3*ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/
		                     ((LDEXP(ScaledSina2Sq,-scale) - 4)*(LDEXP(ScaledSinb2Sq,-scale) - 4));
		break;
	}
	case 1 :
	{
		// 4*SinmaSq == (2*ScaledSinb2Sq*(-2 + ScaledSinc2Sq) - 2*ScaledSina2Sq*(-6 + ScaledSinb2Sq + ScaledSinc2Sq) + 4*(-ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/((-4 + ScaledSinb2Sq)*(-4 + ScaledSinc2Sq))
		faceGeom.SinSqE[0] = (        2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scale) - 2)
		                              - 2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq + ScaledSinc2Sq,-scale) - 6)
		                              + 4*(SinaSq + SinbSq + SincSq - ScaledSinc2Sq)
		                     )/
		                     ((LDEXP(ScaledSinb2Sq,-scale) - 4)*(LDEXP(ScaledSinc2Sq,-scale) - 4));
		faceGeom.SinSqE[1] = ScaledSinb2Sq;
		faceGeom.SinSqE[2] = ScaledSinc2Sq;
		break;
	}
	case 2 :
	{
		faceGeom.SinSqE[0] = ScaledSina2Sq;
		// 4*SinmbSq == (-2*ScaledSina2Sq*(2 + ScaledSinb2Sq - ScaledSinc2Sq) - 2*ScaledSinb2Sq*(-6 + ScaledSinc2Sq) + 4*(-ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/((-4 + ScaledSina2Sq)*(-4 + ScaledSinc2Sq))
		faceGeom.SinSqE[1] = (   - 2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq - ScaledSinc2Sq,-scale) + 2)
		                         - 2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scale) - 6)
		                         + 4*(SinaSq + SinbSq + SincSq - ScaledSinc2Sq))/
		                     ((LDEXP(ScaledSina2Sq,-scale) - 4)*(LDEXP(ScaledSinc2Sq,-scale) - 4));
		faceGeom.SinSqE[2] = ScaledSinc2Sq;
		break;
	}
	case 3 :
	{
		// 4*SinmcSq == (2*ScaledSina2Sq*(-2 + ScaledSinb2Sq - ScaledSinc2Sq) - 2*ScaledSinb2Sq*(2 + ScaledSinc2Sq) + 4*(3*ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/((-4 + ScaledSina2Sq)*(-4 + ScaledSinb2Sq))
		faceGeom.SinSqE[0] = ScaledSina2Sq;
		faceGeom.SinSqE[1] = ScaledSinb2Sq;
		faceGeom.SinSqE[2] = (      2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq - ScaledSinc2Sq,-scale) - 2)
		                            - 2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scale) + 2)
		                            + 4*(3*ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/
		                     ((LDEXP(ScaledSina2Sq,-scale) - 4)*(LDEXP(ScaledSinb2Sq,-scale) - 4));
		break;
	}
	default: // defaul is 0
	{
		faceGeom.SinSqE[0] = 0;
		faceGeom.SinSqE[1] = 0;
		faceGeom.SinSqE[2] = 0;
		break;
	}
	}
}



//calculate length of one edge of a face
SPIRID::angle SPIRID::sGrid::edgeLength(size_t level, unsigned short edgeCode) const
{
	faceGeometry faceGeom(calcFaceGeometry(level));

	if (level>=minScaleSinXToX) return angle(SQRT(faceGeom.SinSqE[edgeCode-1]),level);
	return angle(ASIN(LDEXP(SQRT(faceGeom.SinSqE[edgeCode-1]),-level)),0);
}
//calculate interior angle of a face at a single node
fp_type SPIRID::sGrid::interiorAngle(size_t level, unsigned short nodeCode, const faceGeometry& faceGeom)
{
	fp_type SinaSq,SinbSq,SincSq;
	switch (nodeCode)
	{
	case 1:
	{
		SinaSq = faceGeom.SinSqE[0];
		SinbSq = faceGeom.SinSqE[1];
		SincSq = faceGeom.SinSqE[2];
		break;
	}
	case 2:
	{
		SinaSq = faceGeom.SinSqE[1];
		SinbSq = faceGeom.SinSqE[0];
		SincSq = faceGeom.SinSqE[2];
		break;
	}
	case 3:
	{
		SinaSq = faceGeom.SinSqE[2];
		SinbSq = faceGeom.SinSqE[0];
		SincSq = faceGeom.SinSqE[1];
		break;
	}
	default:
	{
		return 0;
		break;
	}
	}

	if (level>=SPIRID::sGrid::minDepthFlatApprox)
	{
		return ACOS((SinbSq+SincSq-SinaSq)/(SQRT(SinbSq*SincSq)*2));
	}

	return interiorAngle(2*level,SinbSq,SincSq,SinaSq);
}
//calculate interior angle of geometry
fp_type SPIRID::sGrid::interiorAngle(scaleExp_type scale, fp_type SinbSq, fp_type SincSq, fp_type SinaSq)
{
	fp_type ScaledSina2Sq = calcEdgeBisection(scale, SinaSq);
	fp_type ScaledSinb2Sq = calcEdgeBisection(scale, SinbSq);
	fp_type ScaledSinc2Sq = calcEdgeBisection(scale, SincSq);

	return ACOS( (2*ScaledSinc2Sq - 2*ScaledSina2Sq - ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scale) - 2) ) /
	             (4*SQRT(SinbSq*SincSq)) );
}
SPIRID::angle SPIRID::sGrid::area(size_t level, const faceGeometry& faceGeom)
{
	scaleExp_type scaleSq = 2*level;

	fp_type FourSinE1HalfSq = calcEdgeBisection(scaleSq,faceGeom.SinSqE[0]);
	fp_type FourSinE2HalfSq = calcEdgeBisection(scaleSq,faceGeom.SinSqE[1]);
	fp_type FourSinE3HalfSq = calcEdgeBisection(scaleSq,faceGeom.SinSqE[2]);

	if (scaleSq>=minScaleSinXToX)
	{
		return angle(LDEXP(SQRT(sinSqQuarterArea(level, FourSinE1HalfSq, FourSinE2HalfSq, FourSinE3HalfSq)),2),scaleSq);
	}
	return angle(LDEXP(ASIN(LDEXP(SQRT(sinSqQuarterArea(level, FourSinE1HalfSq, FourSinE2HalfSq, FourSinE3HalfSq)),-scaleSq)),scaleSq+2),scaleSq);
}
fp_type
SPIRID::sGrid::sinSqQuarterArea(size_t level,
                                fp_type FourSinE1HalfSq,
                                fp_type FourSinE2HalfSq,
                                fp_type FourSinE3HalfSq)
{
	/* Area of a spherical triangle:
	    tan(area/4) = sqrt( tan(s/2)*tan((s-a)/2)*tan((s-b)/2)*tan((s-c)/2) )
	    where s=(a+b+c)/2

	   By trigonometric formulas this becomes
	    sin(area/4)^2 = [(sin(a/4)^2+sin(b/4)^2+sin(c/4)^2)^2
	                    - 2*(sin(a/4)^4+sin(b/4)^4+sin(c/4)^4)
	                    - 4*sin(a/4)^2*sin(b/4)^2*sin(c/4)^2] /
	                    [(1-2*sin(a/4)^2) * (1-2*sin(b/4)^2) * (1-2*sin(c/4)^2)]
	*/
	//Input values are already given as 4*Sin(e/2)^2. Thus, these values need to be scaled with one level higher.
	scaleExp_type scaleSq = 2*(level+1);
	//calculate 16*Sin(e/4)^2
	fp_type ScaledSinSqQuarterA = calcEdgeBisection(scaleSq,FourSinE1HalfSq);
	fp_type ScaledSinSqQuarterB = calcEdgeBisection(scaleSq,FourSinE2HalfSq);
	fp_type ScaledSinSqQuarterC = calcEdgeBisection(scaleSq,FourSinE3HalfSq);

	fp_type sum = (ScaledSinSqQuarterA+ScaledSinSqQuarterB+ScaledSinSqQuarterC); //scale: 16*2^(2*level) == 2^(scaleSq+2)

	return LDEXP( (sum*sum //scale: 16^2 * 2^(4*level) == 2^(4*(level+2))
	               - 2*( ScaledSinSqQuarterA*ScaledSinSqQuarterA
	                     +ScaledSinSqQuarterB*ScaledSinSqQuarterB
	                     +ScaledSinSqQuarterC*ScaledSinSqQuarterC )
	               - LDEXP(ScaledSinSqQuarterA*ScaledSinSqQuarterB*ScaledSinSqQuarterC,-scaleSq) // 4*term with scaleSq+2 == term with scaleSq
	              ) /
	              ( ( 1 - LDEXP(ScaledSinSqQuarterA,-scaleSq-1))
	                *(1 - LDEXP(ScaledSinSqQuarterB,-scaleSq-1))
	                *(1 - LDEXP(ScaledSinSqQuarterC,-scaleSq-1))
	              ), -8);
}



//for a particular sGrid point: calculate angle and scaled distance to a particular node
SPIRID::sGrid::inFacePolar
SPIRID::sGrid::calcFacePolar(
    size_t refNodeLevel, // gridlevel of the node and edge
    unsigned short nodeCode, // node of the face to consider for distance
    unsigned short edgeCode, // edge of the face to consider for angle
    size_t pointLevel, // gridlevel of the grid point for angle and distance calculation
    unsigned short location // location==0: crossing point of side bisections; location==1,2,3: a node
) const
{
	if (pointLevel<refNodeLevel) return {0,0};

	return calcFacePolar(refNodeLevel, nodeCode, edgeCode,
	                     calcFaceGeometry(refNodeLevel),
	                     std::min(pointLevel, refNodeLevel+accuracyBits),
	                     location);
}
//for a point (pointLevel, location): calculate angle and scaled distance to a particular node
SPIRID::sGrid::inFacePolar
SPIRID::sGrid::calcFacePolar(
    size_t refNodeLevel, // gridlevel of the reference node
    unsigned short nodeCode, // node of the face to consider for distance
    unsigned short edgeCode, // edge of the face to consider for angle
    const faceGeometry& faceGeom, // scaled edge lengths of the face
    size_t pointLevel, // gridlevel of the grid point for angle and distance calculation
    unsigned short location // location==0: crossing point of side bisections; location==1,2,3: a node
) const
{
	if (refNodeLevel>=minDepthFlatApprox) return calcFlatFacePolar(refNodeLevel, nodeCode, edgeCode, faceGeom, pointLevel, location);
	if (refNodeLevel==pointLevel)
	{
		fp_type SinaSq, SinbSq, SincSq;
		//assign edge lengths and check that nodeCode and edgeCode have allowed values
		if (nodeCode == 1)
		{
			if (edgeCode == 2)
			{
				SinaSq = faceGeom.SinSqE[1];
				SinbSq = faceGeom.SinSqE[2];
				SincSq = faceGeom.SinSqE[0];
			}
			else if (edgeCode == 3)
			{
				SinaSq = faceGeom.SinSqE[2];
				SinbSq = faceGeom.SinSqE[1];
				SincSq = faceGeom.SinSqE[0];
			}
			else return {0,0};
		}
		else if (nodeCode == 2)
		{
			if (edgeCode == 1)
			{
				SinaSq = faceGeom.SinSqE[0];
				SinbSq = faceGeom.SinSqE[2];
				SincSq = faceGeom.SinSqE[1];
			}
			else if (edgeCode == 3)
			{
				SinaSq = faceGeom.SinSqE[2];
				SinbSq = faceGeom.SinSqE[0];
				SincSq = faceGeom.SinSqE[1];
			}
			else return {0,0};
		}
		else if (nodeCode == 3)
		{
			if (edgeCode == 1)
			{
				SinaSq = faceGeom.SinSqE[0];
				SinbSq = faceGeom.SinSqE[1];
				SincSq = faceGeom.SinSqE[2];
			}
			else if (edgeCode == 2)
			{
				SinaSq = faceGeom.SinSqE[1];
				SinbSq = faceGeom.SinSqE[0];
				SincSq = faceGeom.SinSqE[2];
			}
			else return {0,0};
		}
		else return {0,0};

		//if location == nodeCode then distance is 0 and we define gamma=0
		fp_type SindSq = 0;
		fp_type gammasca = 0;
		if (location == 0) // point is at the intersection of side bisection lines
		{
			scaleExp_type refScale = 2*refNodeLevel;

			fp_type ScaledSina2Sq = calcEdgeBisection(refScale, SinaSq);
			fp_type ScaledSinb2Sq = calcEdgeBisection(refScale, SinbSq);
			fp_type ScaledSinc2Sq = calcEdgeBisection(refScale, SincSq);

			//SindcSq == (ScaledSina2Sq*(-2 + ScaledSinb2Sq) - 2*(ScaledSinb2Sq - ScaledSinc2Sq + SinaSq + SinbSq))/(2.*(-9 + ScaledSina2Sq + ScaledSinb2Sq + ScaledSinc2Sq))
			SindSq = (      ScaledSina2Sq*(LDEXP(ScaledSinb2Sq,-refScale) - 2)
			                - (ScaledSinb2Sq - ScaledSinc2Sq + SinaSq + SinbSq)*2)/
			         ((LDEXP(ScaledSina2Sq + ScaledSinb2Sq + ScaledSinc2Sq,-refScale) - 9)*2);

			//Cosgammasca == SQRT(Power(ScaledSina2Sq*(-2 + ScaledSinb2Sq) - 2*(ScaledSinb2Sq - ScaledSinc2Sq + 2*SinaSq),2)/(-2*ScaledSina2Sq*(-2 + ScaledSinb2Sq)*SinaSq + 4*SinaSq*(ScaledSinb2Sq - ScaledSinc2Sq + SinaSq + SinbSq)))/2.
			gammasca = ACOS( ( 2*(ScaledSinb2Sq - ScaledSinc2Sq + 2*SinaSq)
			                   - ScaledSina2Sq*(LDEXP(ScaledSinb2Sq,-refScale) - 2) )/
			                 SQRT(   (8*ScaledSina2Sq*(2-LDEXP(ScaledSinb2Sq,-refScale))*SinaSq
			                          + 16*SinaSq*(ScaledSinb2Sq - ScaledSinc2Sq + SinaSq + SinbSq)))
			               );
		}
		else if (location == newCode(nodeCode,edgeCode)) //point is the other node on the reference edge
		{
			SindSq = SinaSq;
		}
		else if (location == edgeCode) //point is the node which is not on the reference edge
		{
			SindSq = SinbSq;
			gammasca = interiorAngle(2*refNodeLevel,SinaSq,SinbSq,SincSq);
		}
		return {SindSq,gammasca};
	}
	else
	{
		size_t nextLevel = refNodeLevel+1;
		unsigned short fCode = at(nextLevel);
		faceGeometry newFaceGeom(faceGeom);
		stepupFaceGeometryFrom(refNodeLevel,fCode,newFaceGeom);

		if (fCode == nodeCode)
		{
			inFacePolar subFacePolar(calcFacePolar(nextLevel,nodeCode,edgeCode,newFaceGeom,pointLevel,location));
			subFacePolar.SinDistSq /= 4;
			return subFacePolar;
		}
		else if (fCode == 0)
		{
			scaleExp_type refScale = 2*refNodeLevel;
			unsigned short otherCode = newCode(edgeCode,nodeCode);

			//lengths of the edges in the sub-face with code "nodeCode"
			fp_type SinaSq = calcEdgeBisection(refScale, faceGeom.SinSqE[edgeCode-1])/4;
			fp_type SinbSq = calcEdgeBisection(refScale, faceGeom.SinSqE[otherCode-1])/4;

			//distance and angle to the reference node within the center sub-face
			inFacePolar subFacePolar(calcFacePolar(nextLevel, edgeCode, nodeCode, newFaceGeom, pointLevel, location));

			//distance to the new reference node within the center sub-face
			fp_type SineSq    = subFacePolar.SinDistSq/4;
			//angle between original reference edge and connection line point-new reference node
			fp_type delta     = subFacePolar.angle + interiorAngle(refScale,SinaSq,newFaceGeom.SinSqE[nodeCode-1]/4,SinbSq);
			fp_type cosDelta  = COS(delta);

			//distance to the original reference node
			fp_type SindSq = calcSinSqEdgeSAS(refScale,SinaSq,cosDelta,SineSq);

			return {SindSq,ASIN(SIN(delta)*SQRT(SineSq/SindSq))};
		}
		else if (fCode == edgeCode)
		{
			scaleExp_type refScale = 2*refNodeLevel;
			unsigned short otherCode = newCode(edgeCode,nodeCode);

			fp_type SinaSq = faceGeom.SinSqE[otherCode-1];
			inFacePolar subFacePolar(calcFacePolar(nextLevel, edgeCode, otherCode, newFaceGeom, pointLevel, location));

			fp_type SinfSq    = subFacePolar.SinDistSq/4;
			fp_type delta     = subFacePolar.angle;
			fp_type cosDelta  = COS(delta);

			fp_type SindSq = calcSinSqEdgeSAS(refScale,SinaSq,cosDelta,SinfSq);

			return {SindSq,interiorAngle(refNodeLevel,nodeCode,faceGeom)-ASIN(SIN(delta)*SQRT(SinfSq/SindSq))};
		}
		else
		{
			scaleExp_type refScale = 2*refNodeLevel;
			unsigned short otherCode = newCode(edgeCode,nodeCode);

			fp_type SinaSq = faceGeom.SinSqE[edgeCode-1];
			inFacePolar subFacePolar(calcFacePolar(nextLevel, otherCode, edgeCode, newFaceGeom, pointLevel, location));

			fp_type SinfSq    = subFacePolar.SinDistSq/4;
			fp_type delta     = subFacePolar.angle;
			fp_type cosDelta  = COS(delta);

			fp_type SindSq = calcSinSqEdgeSAS(refScale,SinaSq,cosDelta,SinfSq);

			return {SindSq,ASIN(SIN(delta)*SQRT(SinfSq/SindSq))};
		}
	}
	return {0,0};
}
//assuming flat geometry approximation: for point (pointLevel, location) calculate angle and scaled distance to a particular node
SPIRID::sGrid::inFacePolar
SPIRID::sGrid::calcFlatFacePolar(
    size_t refNodeLevel, // gridlevel of the node and edge
    unsigned short nodeCode, // node of the face to consider for distance
    unsigned short edgeCode, // edge of the face to consider for angle
    const faceGeometry& faceGeom, // scaled edge lengths of the face
    size_t pointLevel, // gridlevel of the grid point for angle and distance calculation
    unsigned short location // location==0: crossing point of side bisections; location==1,2,3: a node
) const
{
	size_t levelSteps = std::min(accuracyBits,std::max(std::min(pointLevel,depth()),refNodeLevel)-refNodeLevel);
	//assign edge lengths and check that nodeCode and edgeCode have allowed values
	fp_type aSq, bSq, cSq;
	if (nodeCode == 1)
	{
		if (edgeCode == 2)
		{
			aSq = faceGeom.SinSqE[1];
			bSq = faceGeom.SinSqE[2];
			cSq = faceGeom.SinSqE[0];
		}
		else if (edgeCode == 3)
		{
			aSq = faceGeom.SinSqE[2];
			bSq = faceGeom.SinSqE[1];
			cSq = faceGeom.SinSqE[0];
		}
		else return {0,0};
	}
	else if (nodeCode == 2)
	{
		if (edgeCode == 1)
		{
			aSq = faceGeom.SinSqE[0];
			bSq = faceGeom.SinSqE[2];
			cSq = faceGeom.SinSqE[1];
		}
		else if (edgeCode == 3)
		{
			aSq = faceGeom.SinSqE[2];
			bSq = faceGeom.SinSqE[0];
			cSq = faceGeom.SinSqE[1];
		}
		else return {0,0};
	}
	else if (nodeCode == 3)
	{
		if (edgeCode == 1)
		{
			aSq = faceGeom.SinSqE[0];
			bSq = faceGeom.SinSqE[1];
			cSq = faceGeom.SinSqE[2];
		}
		else if (edgeCode == 2)
		{
			aSq = faceGeom.SinSqE[1];
			bSq = faceGeom.SinSqE[0];
			cSq = faceGeom.SinSqE[2];
		}
		else return {0,0};
	}
	else return {0,0};

	fp_type x = 0.5;
	fp_type y = 0.5;

	int direction = -1;
	fp_type axisOffset = 0.25;
	unsigned short faceCode;
	for (size_t levelIterator=1; levelIterator<=levelSteps; levelIterator++)
	{
		faceCode = at(refNodeLevel+levelIterator);
		if (faceCode == 0)
		{
			direction *= -1;
		}
		else if (faceCode == nodeCode)
		{
			x += direction*axisOffset;
			y += direction*axisOffset;
		}
		else if (faceCode == edgeCode)
		{
			y -= direction*axisOffset;
		}
		else
		{
			x -= direction*axisOffset;
		}
		axisOffset /= 2;
	}

	if (location == nodeCode)
	{
		axisOffset = LDEXP(0.5,refNodeLevel-pointLevel);
		direction = 1-2*((refNodeLevel-pointLevel)%2);
		x -= direction*axisOffset;
		y -= direction*axisOffset;
	}
	else if (location == edgeCode)
	{
		axisOffset = LDEXP(0.5,refNodeLevel-pointLevel);
		direction = 1-2*((refNodeLevel-pointLevel)%2);
		y += direction*axisOffset;
	}
	else if (location == newCode(nodeCode,edgeCode))
	{
		axisOffset = LDEXP(0.5,refNodeLevel-pointLevel);
		direction = 1-2*((refNodeLevel-pointLevel)%2);
		x += direction*axisOffset;
	}

	if (x==0 && y==0) return {0,0};

	fp_type dSq = (aSq*(2*x-y)*(x+y) + bSq*(2*y-x)*(x+y) - cSq*(2*x-y)*(2*y-x))*4/9;
	return {dSq,ACOS((aSq*x+(2*y-x)*(bSq-cSq)/3)/(SQRT(dSq*aSq)))};
}



//transform sGrid coordinates to polar coordinates
SPIRID::sPolar SPIRID::sGrid::toPolar(size_t level, unsigned short location) const
{
	sPolar ThetaPhi(toLocalPolar(0,3,1,level,location));

	switch (operator[](0))
	{
	case 0:
	{
		return {ThetaPhi.getTheta(),ThetaPhi.getPhi()};
		break;
	}
	case 1:
	{
		return {ThetaPhi.getTheta(),two_pi-ThetaPhi.getPhi()};
		break;
	}
	case 2:
	{
		return {ThetaPhi.getTheta(),pi-ThetaPhi.getPhi()};
		break;
	}
	case 3:
	{
		return {ThetaPhi.getTheta(),ThetaPhi.getPhi()+pi};
		break;
	}
	case 4:
	{
		return {pi-ThetaPhi.getTheta(),ThetaPhi.getPhi()};
		break;
	}
	case 5:
	{
		return {pi-ThetaPhi.getTheta(),two_pi-ThetaPhi.getPhi()};
		break;
	}
	case 6:
	{
		return {pi-ThetaPhi.getTheta(),pi-ThetaPhi.getPhi()};
		break;
	}
	case 7:
	{
		return {pi-ThetaPhi.getTheta(),ThetaPhi.getPhi()+pi};
		break;
	}
	default:
	{
		return {0,0};
		break;
	}
	}
	return {0,0};
}
//calculate local polar coordinates within a face
SPIRID::sPolar
SPIRID::sGrid::toLocalPolar(size_t refNodeLevel,
                            unsigned short nodeCode,
                            unsigned short edgeCode,
                            size_t level,
                            unsigned short location) const
{
	//consider node at level0 nodeCode3 the north south pole, edgeCode1 the prime and antimeridian
	inFacePolar ThetaPhi(calcFacePolar(refNodeLevel,nodeCode,edgeCode,level,location));

	//in the localPolar data type the distance is stored as 2^(2*level)*Sin(d)^2 --> convert to actual angle
	if (refNodeLevel>=minScaleSinXToX)
	{
		ThetaPhi.SinDistSq = LDEXP(SQRT(ThetaPhi.SinDistSq),-refNodeLevel);
	}
	else
	{
		ThetaPhi.SinDistSq = ASIN(LDEXP(SQRT(ThetaPhi.SinDistSq),-refNodeLevel));
	}

	return sPolar(ThetaPhi.SinDistSq,ThetaPhi.angle);
}



//main function to find the common node of two faces
SPIRID::sGrid::commonNode SPIRID::sGrid::findHighestCommonNode(size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
{
	//ensure level1 <= level2
	if (level1 > level2)
	{
		commonNode N(findHighestCommonNode(level2, P2, level1, P1));
		std::swap(N.level1,N.level2);
		std::swap(N.nodeCode1,N.nodeCode2);
		std::swap(N.edgeCode1,N.edgeCode2);
		return N;
	}

	//bit2: north/south, bit1: front/back, bit0: east/west
	unsigned short level0 =     (P1.gridCode[0]==P2.gridCode[0])
	                            + 2*(P1.gridCode[1]==P2.gridCode[1])
	                            + 4*(P1.gridCode[2]==P2.gridCode[2]);

	switch (level0)
	{
	// case 0 --> no common node
	case 0:
	{
		return {0,0,0, 0,0,0, 0,0};
		break;
	}
	// case 7 --> same face
	case 7:
	{
		return sameFaceStepupTo(1,level1,P1,level2,P2);
		break;
	}
	// cases 1,2,4 --> unique common node at level0 already, 1: N1, 2: N2, 4: N3
	case 1:
	{
		return {lastLevelAtNode(1,1,level1,P1),1,2,
		        lastLevelAtNode(1,1,level2,P2),1,3,
		        pi_2,1};
		break;
	}
	case 2:
	{
		return {lastLevelAtNode(1,2,level1,P1),2,3,
		        lastLevelAtNode(1,2,level2,P2),2,1,
		        pi_2,1};
		break;
	}
	case 4:
	{
		return {lastLevelAtNode(1,3,level1,P1),3,1,
		        lastLevelAtNode(1,3,level2,P2),3,2,
		        pi_2,1};
		break;
	}
	// case 3,5,6 --> common edge
	case 3:
	{
		return commonEdgeStepupTo(1, 3, false, level1, P1, level2, P2);
		break;
	}
	case 5:
	{
		return commonEdgeStepupTo(1, 2, false, level1, P1, level2, P2);
		break;
	}
	case 6:
	{
		return commonEdgeStepupTo(1, 1, false, level1, P1, level2, P2);
		break;
	}
	}
	return {0,0,0, 0,0,0, 0,0};
}
SPIRID::sGrid::commonNode
SPIRID::sGrid::sameFaceStepupTo(size_t level, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
{
	// assume level1 <= level2
	if (level1 < level) //P1 and P2 are the same face at level-1 --> means level1==level-1
	{
		if (level2 < level) //means level1==level-1==level2 and P1==P2
		{
			return {level1,1,2, level2,1,2, 0,-1};
		}

		unsigned short fCode2 = P2.at(level);
		if (fCode2==0) //P2 has higher levels, but no common node
		{
			return {level1,1,2, level1,1,2, 0,-1};
		}
		else
		{
			unsigned short edgeCode = nextNode(fCode2);
			return {level1,fCode2,edgeCode,
			        lastLevelAtNode(level+1,fCode2,level2,P2),fCode2,edgeCode,
			        0,-1};
		}
	}
	// level2<level not possible anymore, because level1<=level2

	//now level1 & level2 >= level
	unsigned short fCode1 = P1.at(level);
	unsigned short fCode2 = P2.at(level);
	size_t nextLevel = level+1;

	if (fCode1==fCode2)
	{
		return sameFaceStepupTo(nextLevel, level1, P1, level2, P2);
	}
	else if (fCode1 == 0)
	{
		return commonEdgeStepupTo(nextLevel, fCode2, true, level1, P1, level2, P2);
	}
	else if (fCode2 == 0)
	{
		return commonEdgeStepupTo(nextLevel, fCode1, true, level1, P1, level2, P2);
	}

	faceGeometry faceGeom(P1.calcFaceGeometry(level-1));
	stepupFaceGeometryFrom(level-1, 0, faceGeom); //need interior angles from center face (code 0)

	unsigned short edgeCode = newCode(fCode1,fCode2);

	return {lastLevelAtNode(nextLevel,fCode2,level1,P1),fCode2,edgeCode,
	        lastLevelAtNode(nextLevel,fCode1,level2,P2),fCode1,edgeCode,
	        pi,1}; // using that the angle between the relevant edges in P1 and P2 is pi and cos(Pi+x)==Cos(Pi-x)
}
SPIRID::sGrid::commonNode
SPIRID::sGrid::commonEdgeStepupTo(size_t level, unsigned short edgeCode, bool orientationMatch, size_t level1, const sGrid& P1, size_t level2, const sGrid& P2)
{
	unsigned short f1NodeLeft  = nextNode(edgeCode);
	unsigned short f1NodeRight = newCode(edgeCode,f1NodeLeft);

	unsigned short f2NodeLeft  = f1NodeLeft;
	unsigned short f2NodeRight = f1NodeRight;

	if (orientationMatch) //if face orientation matches then common edge orientation is opposite
	{
		f2NodeLeft  = f1NodeRight;
		f2NodeRight = f1NodeLeft;
	}

	// assume level1 <= level2
	if (level1 < level) //P1 and P2 are the same face at level-1 --> means level1==level-1
	{
		if (level2 < level) //means level1==level-1==level2
		{
			return {level1,f1NodeLeft,edgeCode,
			        level2,f2NodeLeft,edgeCode,
			        0,1};
		}

		unsigned short fCode2 = P2.at(level);
		if (fCode2==0 || fCode2==edgeCode) //P2 has higher levels, but no common node
		{
			return {level-1,f1NodeLeft,edgeCode,
			        level-1,f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode2==f2NodeLeft)
		{
			return {level-1,f1NodeLeft,edgeCode,
			        lastLevelAtNode(level+1,fCode2,level2,P2),f2NodeLeft,edgeCode,
			        0,1};
		}
		if (fCode2==f2NodeRight)
		{
			return {level-1,f1NodeRight,edgeCode,
			        lastLevelAtNode(level+1,fCode2,level2,P2),f2NodeRight,edgeCode,
			        0,1};
		}
	}
	// level2<level not possible anymore, because level1<=level2

	unsigned short fCode1 = P1.at(level);
	unsigned short fCode2 = P2.at(level);

	if (fCode1 == edgeCode) //no common node for sub-faces --> return one of the edge nodes
	{
		if (fCode2 == edgeCode || fCode2 == 0)
		{
			return {level-1,f1NodeLeft,edgeCode,
			        level-1,f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode2 == f2NodeLeft)
		{
			return {level-1,f1NodeLeft,edgeCode,
			        lastLevelAtNode(level,f2NodeLeft,level,P2),f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode2 == f2NodeRight)
		{
			return {level-1,f1NodeRight,edgeCode,
			        lastLevelAtNode(level,f2NodeRight,level,P2),f2NodeRight,edgeCode,
			        0,1};
		}
	}

	if (fCode2 == edgeCode) //no common node for sub-faces --> return one of the edge nodes
	{
		if (fCode1 == 0)
		{
			return {level-1,f1NodeLeft,edgeCode,
			        level-1,f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode1 == f1NodeLeft)
		{
			return {lastLevelAtNode(level,f1NodeLeft,level,P1),f1NodeLeft,edgeCode,
			        level-1,f2NodeLeft,edgeCode,
			        0,1};
		}
		else if (fCode1 == f1NodeRight)
		{
			return {lastLevelAtNode(level,f1NodeRight,level,P1),f1NodeRight,edgeCode,
			        level-1,f2NodeRight,edgeCode,
			        0,1};
		}
	}

	if (fCode1 == 0)
	{
		faceGeometry faceGeom1(P1.calcFaceGeometry(level-1));

		if (fCode2 == 0) //common node is in both sub-faces the one touching the common edge
		{
			faceGeometry faceGeom2(P2.calcFaceGeometry(level-1));

			stepupFaceGeometryFrom(level-1, f1NodeLeft, faceGeom1);
			stepupFaceGeometryFrom(level-1, f2NodeLeft, faceGeom2);

			return {lastLevelAtNode(level+1,edgeCode,level1,P1),edgeCode,f1NodeLeft,
			        lastLevelAtNode(level+1,edgeCode,level2,P2),edgeCode,f2NodeLeft,
			        interiorAngle(level, f1NodeRight, faceGeom1) + interiorAngle(level, f2NodeRight, faceGeom2),1};
		}
		else if (fCode2 == f2NodeLeft)
		{
			stepupFaceGeometryFrom(level-1, f1NodeLeft, faceGeom1);

			return {lastLevelAtNode(level+1,edgeCode,level1,P1),edgeCode,f1NodeLeft,
			        lastLevelAtNode(level+1,f2NodeRight,level2,P2),f2NodeRight,edgeCode,
			        interiorAngle(level, f1NodeRight, faceGeom1),1};
		}
		else if (fCode2 == f2NodeRight) //one edge of the sub-face face2 is half of the common edge
		{
			stepupFaceGeometryFrom(level-1, f1NodeRight, faceGeom1);

			return {lastLevelAtNode(level+1,edgeCode,level1,P1),edgeCode,f1NodeRight,
			        lastLevelAtNode(level+1,f2NodeLeft,level2,P2),f2NodeLeft,edgeCode,
			        interiorAngle(level, f1NodeLeft, faceGeom1),1};
		}
	}

	if (fCode2 == 0)
	{
		faceGeometry faceGeom2(P2.calcFaceGeometry(level-1));

		if (fCode1 == f1NodeLeft)
		{
			stepupFaceGeometryFrom(level-1, f2NodeLeft, faceGeom2);

			return {lastLevelAtNode(level+1,f1NodeRight,level1,P1),f1NodeRight,edgeCode,
			        lastLevelAtNode(level+1,edgeCode,level2,P2),edgeCode,f2NodeLeft,
			        interiorAngle(level, f2NodeRight, faceGeom2),1};
		}
		else if (fCode1 == f1NodeRight)
		{
			stepupFaceGeometryFrom(level-1, f2NodeRight, faceGeom2);

			return {lastLevelAtNode(level+1,f1NodeLeft,level1,P1),f1NodeLeft,edgeCode,
			        lastLevelAtNode(level+1,edgeCode,level2,P2),edgeCode,f2NodeRight,
			        interiorAngle(level, f2NodeLeft, faceGeom2),1};
		}
	}

	if (fCode1 == f1NodeLeft)
	{
		if (fCode2 == f2NodeLeft)
		{
			return commonEdgeStepupTo(level+1, edgeCode, orientationMatch, level1, P1, level2, P2);
		}
		else if (fCode2 == f2NodeRight)
		{
			return {lastLevelAtNode(level+1,f1NodeRight,level1,P1),f1NodeRight,edgeCode,
			        lastLevelAtNode(level+1,f2NodeLeft,level2,P2),f2NodeLeft,edgeCode,
			        pi,-1}; //add angles as alpha1 + sign(gamma)*(alpha2+gamma)
		}
	}
	else if (fCode1 == f1NodeRight)
	{
		if (fCode2 == f2NodeRight)
		{
			return commonEdgeStepupTo(level+1, edgeCode, orientationMatch, level1, P1, level2, P2);
		}
		else if (fCode2 == f2NodeLeft)
		{
			return {lastLevelAtNode(level+1,f1NodeLeft,level1,P1),f1NodeLeft,edgeCode,
			        lastLevelAtNode(level+1,f2NodeRight,level2,P2),f2NodeRight,edgeCode,
			        pi,-1}; //add angles as alpha1 + sign(gamma)*(alpha2+gamma)
		}
	}
	return {0,0,0,0,0,0,0};
}



SPIRID::sGrid&
SPIRID::sGrid::assignNeighborFace(size_t level, unsigned short edgeCode, sGrid& neighbor, bool& orientationMatch) const
{
	unsigned short fCode = operator[](level);

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


//distance calculation for points
SPIRID::angle SPIRID::sGrid::distance(size_t levelP1, const sGrid& P1, unsigned short locationP1,
                                      size_t levelP2, const sGrid& P2, unsigned short locationP2)
{
	//start by finding the highest common node for P1 and P2
	commonNode commonN(findHighestCommonNode(levelP1, P1, levelP2, P2));

	//if no common node then we calculate the distance to a mirrored point
	bool mirror = false;
	if (commonN.nodeCode1 == 0) //means no common node
	{
		mirror = true;
		sGrid P3(P2);
		commonN = findHighestCommonNode(levelP1, P1, levelP2, P3.mirror());
	}

	//calculate distance & angle from points to common nodes
	size_t levelN1 = commonN.level1;
	size_t levelN2 = commonN.level2;
	inFacePolar localP1(P1.calcFacePolar(levelN1, commonN.nodeCode1, commonN.edgeCode1, levelP1, locationP1));
	inFacePolar localP2(P2.calcFacePolar(levelN2, commonN.nodeCode2, commonN.edgeCode2, levelP2, locationP2));
	fp_type SinaSq = localP1.SinDistSq;
	fp_type SinbSq = localP2.SinDistSq;

	//calculate angle between the connections P1-commonNode-P2
	fp_type cosGapAngle = COS(localP1.angle + commonN.signAngle2*localP2.angle + commonN.gapAngle);

	//check for level differences (occurs when one of the points is closer to the common node)
	size_t levelOffset = 0;
	size_t minLevel = levelN1;
	if (levelN1>levelN2)
	{
		levelOffset=levelN1-levelN2;
		minLevel = levelN2;
		SinaSq = LDEXP(SinaSq,-levelOffset*2);
	}
	else
	{
		levelOffset=levelN2-levelN1;
		SinbSq = LDEXP(SinbSq,-levelOffset*2);
	}

	//calculate 2^minLevel*Sin(result/2) using special form of spherical cosine law
	fp_type interimResult = calcSinEdgeHalfSAS(minLevel,SinaSq,cosGapAngle,SinbSq);

	if (minLevel<minScaleSinXToX)
	{
		interimResult = ASIN(LDEXP(interimResult,-minLevel)); //interimResult == result/2

		if (mirror) // if the point was mirrored then it is most appropriate to return a result without scaling
		{
			interimResult = pi-LDEXP(interimResult,1);
			minLevel = 0;
		}
		else interimResult = LDEXP(interimResult,1+minLevel);
	}
	else
	{
		interimResult = LDEXP(interimResult,1);  //interimResult == 2^minLevel*result (assuming Sin(x)~x)

		if (mirror)
		{
			interimResult = pi-LDEXP(interimResult,-minLevel);
			minLevel = 0;
		}
	}

	return angle(interimResult,minLevel);
}

fp_type SPIRID::sGrid::calcSinEdgeHalfSAS(scaleExp_type scale, fp_type SinaSq, fp_type cosDelta, fp_type SinbSq)
{
	/* Calculation method here differs from calcSinSqEdgeSAS:
	        At a==b and delta==0 the calculated edge length becomes zero and the numerical result is dominated by rounding errors.
	        Since calcSinSqEdgeSAS returns the square of a length, we loose accuracy by taking the square root of rounding errors.
	        Here we modify the calculation and return an actual length (instead of the length squared)
	        If the calculated edge length is d then the return value here is 2^scale * Sin(d/2).

	    From spherical cosine law:
	    Sin(d/2)^2 = termL^2 + termA^2
	    termL = |Sin(a/2)*Cos(b/2) - Cos(delta)*Cos(a/2)*Sin(b/2)|
	    termA = |Cos(a/2)*Sin(b/2)*Sin(delta)|
	*/
	scaleExp_type scaleSq = 2*scale;

	fp_type ScaledSina2Sq = calcEdgeBisection(scaleSq,SinaSq); //==4*Sin(a/2)^2
	fp_type ScaledSinb2Sq = calcEdgeBisection(scaleSq,SinbSq); //==4*Sin(b/2)^2

	fp_type termL = ABS(SQRT(ScaledSina2Sq*(1-LDEXP(ScaledSinb2Sq,-scaleSq-2)))
	                    - cosDelta*SQRT(ScaledSinb2Sq*(1-LDEXP(ScaledSina2Sq,-scaleSq-2))));
	fp_type termA = SQRT((1-cosDelta*cosDelta)*(1-LDEXP(ScaledSina2Sq,-scaleSq-2))*ScaledSinb2Sq);

	fp_type result = 0;
	if (termA == 0) result = termL;
	else if (termL == 0) result = termA;
	else if (termA < termL)
	{
		result = termL * SQRT((1+termA*termA/(termL*termL)));
	}
	else
	{
		result = termA * SQRT((1+termL*termL/(termA*termA)));
	}
	return LDEXP(result,-1);
}




const unsigned short SPIRID::sGrid::newNodeCodes[4] = {4,2,3,1};
const unsigned short SPIRID::sGrid::newFaceCodes[4] = {1,2,3,0};
const unsigned short SPIRID::sGrid::newCodes[7] = {4,4,4,3,2,1,0};



//output a complete face code
std::ostream& SPIRID::operator << (std::ostream& out, const sGrid& P)
{
	out << "(F";
	for (size_t it=0; it<=P.depth(); it++) out << P.at(it);
	out << ")";
	return out;
}
//output data of a common node
std::ostream& SPIRID::sGrid::commonNode::print(std::ostream& out)
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
