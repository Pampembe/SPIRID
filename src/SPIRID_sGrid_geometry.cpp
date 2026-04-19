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





//calculate length of one edge of a face
SPIRID::scaledFP
SPIRID::sGrid::edgeLength(size_t level, unsigned short edgeCode) const
{
	faceGeometry faceGeom(calcFaceGeometry(level));

	if (level>=minScaleSinXToX) return scaledFP(SQRT( faceGeom[edgeCode] ), level);
	return scaledFP( LDEXP( ASIN( LDEXP( SQRT(faceGeom[edgeCode]), -level) ), level), level );
}

//calculate geometry (scaled edge lengths) of the face
SPIRID::sGrid::faceGeometry
SPIRID::sGrid::calcFaceGeometry(size_t level) const
{
	//edge lengths E as SinSqE = 2^(2*level)*Sin(E)^2 --> level 0: E=Pi/2, SinSqE=1; level 1, E=Pi/4: SinSqE=2
	faceGeometry faceGeom({1,1,1});
	size_t max_level_it = std::min(level,minDepthFlatApprox);

	for (size_t level_it=0; level_it<max_level_it; level_it++)
	{
		stepupFaceGeometryFrom(level_it, at(level_it+1), faceGeom);
	}

	return faceGeom;
}
//update geometry (scaled edge lengths) of a face: step-up to the next grid level
void
SPIRID::sGrid::stepupFaceGeometryFrom(
    size_t level,
    unsigned short fCode,
    faceGeometry& faceGeom)
{
	scaleExp_type scaleSq = 2*level;

	fp_type SinaSq = faceGeom.SinaSq;
	fp_type SinbSq = faceGeom.SinbSq;
	fp_type SincSq = faceGeom.SincSq;

	fp_type ScaledSina2Sq = calcEdgeBisection(scaleSq,SinaSq);
	fp_type ScaledSinb2Sq = calcEdgeBisection(scaleSq,SinbSq);
	fp_type ScaledSinc2Sq = calcEdgeBisection(scaleSq,SincSq);

	switch (fCode)
	{
	case 0 :
	{
		faceGeom.SinaSq = (        2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scaleSq) - 2)
		                           - 2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq + ScaledSinc2Sq,-scaleSq) - 6)
		                           + 4*(SinaSq + SinbSq + SincSq - ScaledSinc2Sq)
		                  )/
		                  ((LDEXP(ScaledSinb2Sq,-scaleSq) - 4)*(LDEXP(ScaledSinc2Sq,-scaleSq) - 4));
		faceGeom.SinbSq = (   - 2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq - ScaledSinc2Sq,-scaleSq) + 2)
		                      - 2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scaleSq) - 6)
		                      + 4*(SinaSq + SinbSq + SincSq - ScaledSinc2Sq))/
		                  ((LDEXP(ScaledSina2Sq,-scaleSq) - 4)*(LDEXP(ScaledSinc2Sq,-scaleSq) - 4));
		faceGeom.SincSq = (      2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq - ScaledSinc2Sq,-scaleSq) - 2)
		                         - 2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scaleSq) + 2)
		                         + 4*(3*ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/
		                  ((LDEXP(ScaledSina2Sq,-scaleSq) - 4)*(LDEXP(ScaledSinb2Sq,-scaleSq) - 4));
		break;
	}
	case 1 :
	{
		// 4*SinmaSq == (2*ScaledSinb2Sq*(-2 + ScaledSinc2Sq) - 2*ScaledSina2Sq*(-6 + ScaledSinb2Sq + ScaledSinc2Sq) + 4*(-ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/((-4 + ScaledSinb2Sq)*(-4 + ScaledSinc2Sq))
		faceGeom.SinaSq = (        2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scaleSq) - 2)
		                           - 2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq + ScaledSinc2Sq,-scaleSq) - 6)
		                           + 4*(SinaSq + SinbSq + SincSq - ScaledSinc2Sq)
		                  )/
		                  ((LDEXP(ScaledSinb2Sq,-scaleSq) - 4)*(LDEXP(ScaledSinc2Sq,-scaleSq) - 4));
		faceGeom.SinbSq = ScaledSinb2Sq;
		faceGeom.SincSq = ScaledSinc2Sq;
		break;
	}
	case 2 :
	{
		faceGeom.SinaSq = ScaledSina2Sq;
		// 4*SinmbSq == (-2*ScaledSina2Sq*(2 + ScaledSinb2Sq - ScaledSinc2Sq) - 2*ScaledSinb2Sq*(-6 + ScaledSinc2Sq) + 4*(-ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/((-4 + ScaledSina2Sq)*(-4 + ScaledSinc2Sq))
		faceGeom.SinbSq = (   - 2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq - ScaledSinc2Sq,-scaleSq) + 2)
		                      - 2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scaleSq) - 6)
		                      + 4*(SinaSq + SinbSq + SincSq - ScaledSinc2Sq))/
		                  ((LDEXP(ScaledSina2Sq,-scaleSq) - 4)*(LDEXP(ScaledSinc2Sq,-scaleSq) - 4));
		faceGeom.SincSq = ScaledSinc2Sq;
		break;
	}
	case 3 :
	{
		// 4*SinmcSq == (2*ScaledSina2Sq*(-2 + ScaledSinb2Sq - ScaledSinc2Sq) - 2*ScaledSinb2Sq*(2 + ScaledSinc2Sq) + 4*(3*ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/((-4 + ScaledSina2Sq)*(-4 + ScaledSinb2Sq))
		faceGeom.SinaSq = ScaledSina2Sq;
		faceGeom.SinbSq = ScaledSinb2Sq;
		faceGeom.SincSq = (      2*ScaledSina2Sq*(LDEXP(ScaledSinb2Sq - ScaledSinc2Sq,-scaleSq) - 2)
		                         - 2*ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scaleSq) + 2)
		                         + 4*(3*ScaledSinc2Sq + SinaSq + SinbSq + SincSq))/
		                  ((LDEXP(ScaledSina2Sq,-scaleSq) - 4)*(LDEXP(ScaledSinb2Sq,-scaleSq) - 4));
		break;
	}
	default:
	{
		faceGeom.SinaSq = 0;
		faceGeom.SinbSq = 0;
		faceGeom.SincSq = 0;
		break;
	}
	}
	return;
}

//calculate interior angle of a face at a single node
fp_type
SPIRID::sGrid::interiorAngle(
    size_t level,
    unsigned short nodeCode,
    const faceGeometry& faceGeom)
{
	fp_type SinaSq,SinbSq,SincSq;
	switch (nodeCode)
	{
	case 1:
	{
		SinaSq = faceGeom[1];
		SinbSq = faceGeom[2];
		SincSq = faceGeom[3];
		break;
	}
	case 2:
	{
		SinaSq = faceGeom[2];
		SinbSq = faceGeom[1];
		SincSq = faceGeom[3];
		break;
	}
	case 3:
	{
		SinaSq = faceGeom[3];
		SinbSq = faceGeom[1];
		SincSq = faceGeom[2];
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

	return interiorAngle_fromSinSqe(2*level,SinbSq,SincSq,SinaSq);
}
//calculate interior angle of geometry
fp_type SPIRID::sGrid::interiorAngle_fromSinSqe(
    scaleExp_type scaleSq,
    fp_type SinbSq,
    fp_type SincSq,
    fp_type SinaSq)
{
	fp_type ScaledSina2Sq = calcEdgeBisection(scaleSq, SinaSq);
	fp_type ScaledSinb2Sq = calcEdgeBisection(scaleSq, SinbSq);
	fp_type ScaledSinc2Sq = calcEdgeBisection(scaleSq, SincSq);

	return ACOS( (ScaledSinc2Sq - ScaledSina2Sq - ScaledSinb2Sq*(LDEXP(ScaledSinc2Sq,-scaleSq-1) - 1) ) /
	             (2*SQRT(SinbSq*SincSq)) );
}
//calculate interior angle of geometry
fp_type SPIRID::sGrid::interiorAngle_fromSinSqeHalf(
    scaleExp_type scaleSq, //scaleSq==2*level+2
    fp_type SinSqbHalf,
    fp_type SinSqcHalf,
    fp_type SinSqaHalf)
{
	fp_type SinbSq = (SinSqbHalf-SinSqbHalf*LDEXP(SinSqbHalf, -scaleSq));
	fp_type SincSq = (SinSqcHalf-SinSqcHalf*LDEXP(SinSqcHalf, -scaleSq));
	return ACOS( (SinSqcHalf - SinSqaHalf - SinSqbHalf*(LDEXP(SinSqcHalf,-scaleSq+1) - 1) ) /
	             (2*SQRT(SinbSq*SincSq)) );
}

//calculate area of a face
SPIRID::scaledFP SPIRID::sGrid::area(size_t level, const faceGeometry& faceGeom)
{
	scaleExp_type scaleSq = 2*level;

	fp_type FourSinE1HalfSq = calcEdgeBisection(scaleSq,faceGeom[1]);
	fp_type FourSinE2HalfSq = calcEdgeBisection(scaleSq,faceGeom[2]);
	fp_type FourSinE3HalfSq = calcEdgeBisection(scaleSq,faceGeom[3]);

	if (scaleSq>=minScaleSinXToX)
	{
		return scaledFP(LDEXP(SQRT( sinSqQuarterArea(level, FourSinE1HalfSq, FourSinE2HalfSq, FourSinE3HalfSq) ),2),scaleSq);
	}
	return scaledFP(LDEXP(ASIN(LDEXP(SQRT(sinSqQuarterArea(level, FourSinE1HalfSq, FourSinE2HalfSq, FourSinE3HalfSq)),-scaleSq)), scaleSq+2), scaleSq);
}
//calculate area of a triangle given edge lengths E in terms of 4*Sin(E/2)^2
fp_type
SPIRID::sGrid::sinSqQuarterArea(
    size_t level,
    fp_type FourSinE1HalfSq,
    fp_type FourSinE2HalfSq,
    fp_type FourSinE3HalfSq)
{
	/* Area of a spherical triangle:
	    tan(area/4) = sqrt( tan(s/2)*tan((s-a)/2)*tan((s-b)/2)*tan((s-c)/2) )
	    where s=(a+b+c)/2

	   By trigonometric formulas this can be rewritten as
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

//calculate the edge length E of a triangle (as Sin(E/2)), given side-angle-side data (side lengths as Sin(a)^2)
fp_type
SPIRID::sGrid::calcSinEdgeHalfSAS(
    scaleExp_type level,
    fp_type SinaSq,
    fp_type cosDelta,
    fp_type SinbSq)
{
	/* Calculation method here differs from calcSinSqEdgeSAS:
	        At a==b and delta==0 the calculated edge length becomes zero and the numerical result is dominated by rounding errors.
	        Since calcSinSqEdgeSAS returns the square of a length, we may loose accuracy by taking the square root of rounding errors.
	        Here we use a different calculation method and return an actual length (instead of the length squared)
	        If the calculated edge length is d then the return value here is 2^level * Sin(d/2).

	    From spherical cosine law:
	    Sin(d/2)^2 = termL^2 + termA^2
	    termL = |Sin(a/2)*Cos(b/2) - Cos(delta)*Cos(a/2)*Sin(b/2)|
	    termA = |Cos(a/2)*Sin(b/2)*Sin(delta)|
	*/
	scaleExp_type scaleSq = 2*level;

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
//calculate the edge length E of a triangle (as 2^(level+2)*Sin^2(E/2)), given side-angle-side data ( side lengths L as 2^(level+2)*Sin^2(L/2) )
fp_type
SPIRID::sGrid::robustCalcSinEdgeHalfSAS(
    scaleExp_type scaleSq, //scaleSq is 2*level+2
    fp_type SinaHalfSq,
    fp_type cosDelta,
    fp_type SinbHalfSq)
{
	/* Calculation method here differs from calcSinSqEdgeHalfSAS:
	        At a==b and delta==0 the calculated edge length becomes zero and the numerical result is dominated by rounding errors.
	        Since calcSinSqEdgeHalfSAS returns the square of a length, we may loose accuracy by taking the square root of rounding errors.
	        Here we use a different calculation method and return the Sine of half the edge length (instead of half the length squared)
	        If the calculated edge length is d then the return value here is 2^(level+1) * Sin(d/2).

	    From spherical cosine law:
	    Sin(d/2)^2 = termL^2 + termA^2
	    termL = |Sin(a/2)*Cos(b/2) - Cos(delta)*Cos(a/2)*Sin(b/2)|
	    termA = |Cos(a/2)*Sin(b/2)*Sin(delta)|
	*/
	fp_type termL = ABS( SQRT(SinaHalfSq * (1-LDEXP(SinbHalfSq,-scaleSq)))
	                     - cosDelta * SQRT( SinbHalfSq * (1-LDEXP(SinaHalfSq,-scaleSq)) ) );
	fp_type termA = SQRT( (1-cosDelta*cosDelta) * (1-LDEXP(SinaHalfSq,-scaleSq)) * SinbHalfSq);

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





//transform sGrid coordinates to polar coordinates
SPIRID::sPolar
SPIRID::sGrid::toPolar(size_t level, unsigned short location) const
{
	sPolar ThetaPhi(toLocalPolar(0,3,1,level,location));

	//nodes at level0 with nodeCode 3 are the north and south pole
	//the edges with edgeCode 1 are the prime and antimeridian
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
	inFacePolarSH ThetaPhiSH(calcFacePolarSH(refNodeLevel,nodeCode,edgeCode,level,location));

	//for inFacePolarSH data type: the distance is stored as 2^(2*level+2)*Sin(d/2)^2 --> convert to actual local azimuthal angle
	if (refNodeLevel>=minScaleSinXToX)
	{
		ThetaPhiSH.SinSqdHalf = LDEXP(SQRT(ThetaPhiSH.SinSqdHalf),-refNodeLevel);
	}
	else
	{
		ThetaPhiSH.SinSqdHalf = 2*ASIN(LDEXP(SQRT(ThetaPhiSH.SinSqdHalf),-refNodeLevel-1));
	}

	return sPolar(ThetaPhiSH.SinSqdHalf,ThetaPhiSH.angle);


	inFacePolar ThetaPhi(calcFacePolar(refNodeLevel,nodeCode,edgeCode,level,location));

	//for inFacePolar data type: the distance is stored as 2^(2*level)*Sin(d)^2 --> convert to actual local azimuthal angle
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

//for a point (pointLevel, location): calculate angle and scaled distance to a reference node at lower level
SPIRID::sGrid::inFacePolar
SPIRID::sGrid::calcFacePolar(
    size_t refNodeLevel, // gridlevel of the reference node
    unsigned short nodeCode, // reference node in the face
    unsigned short edgeCode, // reference edge in the face
    size_t pointLevel, // gridlevel of the grid point for angle and distance calculation
    unsigned short location // location==0: crossing point of side bisections; location==1,2,3: a node
) const
{
	if (pointLevel<refNodeLevel) return {0,0};

	//calculation for points at very high level can be truncated, because changes are below the calculation accuracy
	return calcFacePolar(refNodeLevel, nodeCode, edgeCode,
	                     calcFaceGeometry(refNodeLevel),
	                     std::min(pointLevel, refNodeLevel+accuracyBits),
	                     location);
}
//for a point (pointLevel, location): calculate angle and scaled distance to a reference node at lower level
SPIRID::sGrid::inFacePolar
SPIRID::sGrid::calcFacePolar(
    size_t refNodeLevel, // gridlevel of the reference node
    unsigned short nodeCode, // reference node in the face
    unsigned short edgeCode, // reference edge in the face
    const faceGeometry& faceGeom, // scaled edge lengths of the face at refNodeLevel
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
				SinaSq = faceGeom[2];
				SinbSq = faceGeom[3];
				SincSq = faceGeom[1];
			}
			else if (edgeCode == 3)
			{
				SinaSq = faceGeom[3];
				SinbSq = faceGeom[2];
				SincSq = faceGeom[1];
			}
			else return {0,0};
		}
		else if (nodeCode == 2)
		{
			if (edgeCode == 1)
			{
				SinaSq = faceGeom[1];
				SinbSq = faceGeom[3];
				SincSq = faceGeom[2];
			}
			else if (edgeCode == 3)
			{
				SinaSq = faceGeom[3];
				SinbSq = faceGeom[1];
				SincSq = faceGeom[2];
			}
			else return {0,0};
		}
		else if (nodeCode == 3)
		{
			if (edgeCode == 1)
			{
				SinaSq = faceGeom[1];
				SinbSq = faceGeom[2];
				SincSq = faceGeom[3];
			}
			else if (edgeCode == 2)
			{
				SinaSq = faceGeom[2];
				SinbSq = faceGeom[1];
				SincSq = faceGeom[3];
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
			gammasca = interiorAngle_fromSinSqe(2*refNodeLevel,SinaSq,SinbSq,SincSq);
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

			//relevant edge lengths in the sub-face with code "nodeCode"
			fp_type SinaSq = calcEdgeBisection(refScale, faceGeom[edgeCode])/4;
			fp_type SinbSq = calcEdgeBisection(refScale, faceGeom[otherCode])/4;

			//distance and angle to the reference node within the center sub-face
			inFacePolar subFacePolar(calcFacePolar(nextLevel, edgeCode, nodeCode, newFaceGeom, pointLevel, location));
			//distance to the new reference node within the center sub-face
			fp_type SineSq = subFacePolar.SinDistSq/4;

			//angle between original reference edge and the connection line between the point and the new reference node inside the center sub-face
			fp_type delta    = subFacePolar.angle + interiorAngle_fromSinSqe(refScale,SinaSq,newFaceGeom[nodeCode]/4,SinbSq);
			fp_type tmpAngle = COS(delta);

			//distance to the original reference node
			fp_type SindSq = calcSinSqEdgeSAS(refScale,SinaSq,tmpAngle,SineSq);

			//using spherical sine law is unstable near 90 degree angle --> using cosine law & interiorAngle instead
			tmpAngle = SIN(delta)*SQRT(SineSq/SindSq);
			if (tmpAngle > 0.95) tmpAngle = interiorAngle_fromSinSqe(refScale,SinaSq,SindSq,SineSq);
			else tmpAngle = ASIN(tmpAngle);

			return {SindSq,tmpAngle};
		}
		else if (fCode == edgeCode)
		{
			scaleExp_type refScale = 2*refNodeLevel;
			unsigned short otherCode = newCode(edgeCode,nodeCode);

			fp_type SinaSq = faceGeom[otherCode];
			inFacePolar subFacePolar(calcFacePolar(nextLevel, edgeCode, otherCode, newFaceGeom, pointLevel, location));

			fp_type SinfSq   = subFacePolar.SinDistSq/4;
			fp_type delta    = subFacePolar.angle;
			//std::cout << SinaSq << "," << SinfSq << std::endl;
			//std::cout << calcEdgeBisection(refScale,SinaSq) << "," << calcEdgeBisection(refScale,SinfSq) << "," << delta << std::endl;

			fp_type SindSq = calcSinSqEdgeSAS(refScale,SinaSq,COS(delta),SinfSq);

			//using spherical sine law is unstable near 90 degree angle --> cannot happen in this case
			return {SindSq,interiorAngle(refNodeLevel,nodeCode,faceGeom) - ASIN(SIN(delta)*SQRT(SinfSq/SindSq))};
		}
		else
		{
			scaleExp_type refScale = 2*refNodeLevel;
			unsigned short otherCode = newCode(edgeCode,nodeCode);

			fp_type SinaSq = faceGeom[edgeCode];
			inFacePolar subFacePolar(calcFacePolar(nextLevel, otherCode, edgeCode, newFaceGeom, pointLevel, location));

			fp_type SinfSq    = subFacePolar.SinDistSq/4;
			fp_type delta     = subFacePolar.angle;

			fp_type SindSq = calcSinSqEdgeSAS(refScale,SinaSq,COS(delta),SinfSq);

			//using spherical sine law is unstable near 90 degree angle --> cannot happen in this case
			return {SindSq,ASIN(SIN(delta)*SQRT(SinfSq/SindSq))};
		}
	}
	return {0,0};
}
//assuming flat geometry approximation: for point (pointLevel, location) calculate angle and scaled distance to a particular node
SPIRID::sGrid::inFacePolar
SPIRID::sGrid::calcFlatFacePolar(
    size_t refNodeLevel, // gridlevel of the reference node
    unsigned short nodeCode, // reference node in the face
    unsigned short edgeCode, // reference edge in the face
    const faceGeometry& faceGeom, // scaled edge lengths of the face at refNodeLevel
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
			aSq = faceGeom[2];
			bSq = faceGeom[3];
			cSq = faceGeom[1];
		}
		else if (edgeCode == 3)
		{
			aSq = faceGeom[3];
			bSq = faceGeom[2];
			cSq = faceGeom[1];
		}
		else return {0,0};
	}
	else if (nodeCode == 2)
	{
		if (edgeCode == 1)
		{
			aSq = faceGeom[1];
			bSq = faceGeom[3];
			cSq = faceGeom[2];
		}
		else if (edgeCode == 3)
		{
			aSq = faceGeom[3];
			bSq = faceGeom[1];
			cSq = faceGeom[2];
		}
		else return {0,0};
	}
	else if (nodeCode == 3)
	{
		if (edgeCode == 1)
		{
			aSq = faceGeom[1];
			bSq = faceGeom[2];
			cSq = faceGeom[3];
		}
		else if (edgeCode == 2)
		{
			aSq = faceGeom[2];
			bSq = faceGeom[1];
			cSq = faceGeom[3];
		}
		else return {0,0};
	}
	else return {0,0};

	fp_type x = 0.5;
	fp_type y = 0.5;

	int direction = 1;
	fp_type axisOffset = 0.5;
	unsigned short faceCode;
	for (size_t levelIterator=1; levelIterator<=levelSteps; levelIterator++)
	{
		axisOffset /= 2;
		faceCode = at(refNodeLevel+levelIterator);
		if (faceCode == 0)
		{
			direction *= -1;
		}
		else if (faceCode == nodeCode)
		{
			x -= direction*axisOffset;
			y -= direction*axisOffset;
		}
		else if (faceCode == edgeCode)
		{
			y += direction*axisOffset;
		}
		else
		{
			x += direction*axisOffset;
		}
	}

	if (location == nodeCode)
	{
		x -= direction*axisOffset;
		y -= direction*axisOffset;
	}
	else if (location == edgeCode)
	{
		y += direction*axisOffset;
	}
	else if (location == newCode(nodeCode,edgeCode))
	{
		x += direction*axisOffset;
	}

	/*  In the polar angle calculation formula we need to divide by the azimuthal angle.
	    The azimuthal angle becomes zero at the reference node --> need to distinguish this case.
	*/
	if (x==0 && y==0) return {0,0};

	// calculation of the azimuthal angle d (as d^2)
	fp_type dSq = (aSq*(2*x-y)*(x+y) + bSq*(2*y-x)*(x+y) - cSq*(2*x-y)*(2*y-x))*4/9;

	/*  Calculation of the polar angle delta:
	    Two formulas can be derived for Sin(delta) or Cos(delta).
	    The former is more stable and accurate for small delta, the latter is better used for large delta.
	    Cos(delta) = ( a^2*x + (2*y-x)/3 * (b^2-c^2) ) / (a*d)
	    Sin^2(delta) = ( (2*y-x) / (3*a*d) )^2 * [(a^2+b^2+c^2)^2 - 2*(a^4+b^4+c^4)]
	*/
	fp_type delta = 0;
	if (y>x)
	{
		delta = ACOS((aSq*x+(2*y-x)*(bSq-cSq)/3)/SQRT(dSq*aSq));
	}
	else
	{
		delta = (aSq+bSq+cSq);
		delta = ASIN( (2*y-x)/3 * SQRT( (delta*delta - 2*aSq*aSq - 2*bSq*bSq - 2*cSq*cSq) / (aSq*dSq) ) );
	}

	return {dSq, delta};
}


//for a point (pointLevel, location): calculate angle and scaled distance to a reference node at lower level
SPIRID::sGrid::inFacePolarSH
SPIRID::sGrid::calcFacePolarSH(
    size_t refNodeLevel, // gridlevel of the reference node
    unsigned short nodeCode, // reference node in the face
    unsigned short edgeCode, // reference edge in the face
    size_t pointLevel, // gridlevel of the grid point for angle and distance calculation
    unsigned short location // location==0: crossing point of side bisections; location==1,2,3: a node
) const
{
	if (pointLevel<refNodeLevel) return {0,0};

	//calculation for points at very high level can be truncated, because changes are below the calculation accuracy
	return calcFacePolarSH(refNodeLevel, nodeCode, edgeCode,
	                     calcFaceGeometry(refNodeLevel),
	                     std::min(pointLevel, refNodeLevel+accuracyBits),
	                     location);
}
//for a point (pointLevel, location): calculate angle and distance to a reference node at lower level
//distance d as 2^(2*level+2)*Sin^2(d/2)
SPIRID::sGrid::inFacePolarSH
SPIRID::sGrid::calcFacePolarSH(
    size_t refNodeLevel, // gridlevel of the reference node
    unsigned short nodeCode, // reference node in the face
    unsigned short edgeCode, // reference edge in the face
    const faceGeometry& faceGeom, // scaled edge lengths of the face at refNodeLevel
    size_t pointLevel, // gridlevel of the grid point for angle and distance calculation
    unsigned short location // location==0: crossing point of side bisections; location==1,2,3: a node
) const
{
	if (refNodeLevel>=minDepthFlatApprox) return calcFlatFacePolarSH(refNodeLevel, nodeCode, edgeCode, faceGeom, pointLevel, location);
	if (refNodeLevel==pointLevel)
	{
		fp_type SinaSq, SinbSq, SincSq;
		//assign edge lengths and check that nodeCode and edgeCode have allowed values
		if (nodeCode == 1)
		{
			if (edgeCode == 2)
			{
				SinaSq = faceGeom[2];
				SinbSq = faceGeom[3];
				SincSq = faceGeom[1];
			}
			else if (edgeCode == 3)
			{
				SinaSq = faceGeom[3];
				SinbSq = faceGeom[2];
				SincSq = faceGeom[1];
			}
			else return {0,0};
		}
		else if (nodeCode == 2)
		{
			if (edgeCode == 1)
			{
				SinaSq = faceGeom[1];
				SinbSq = faceGeom[3];
				SincSq = faceGeom[2];
			}
			else if (edgeCode == 3)
			{
				SinaSq = faceGeom[3];
				SinbSq = faceGeom[1];
				SincSq = faceGeom[2];
			}
			else return {0,0};
		}
		else if (nodeCode == 3)
		{
			if (edgeCode == 1)
			{
				SinaSq = faceGeom[1];
				SinbSq = faceGeom[2];
				SincSq = faceGeom[3];
			}
			else if (edgeCode == 2)
			{
				SinaSq = faceGeom[2];
				SinbSq = faceGeom[1];
				SincSq = faceGeom[3];
			}
			else return {0,0};
		}
		else return {0,0};

		//if location == nodeCode then distance is 0 and we define gamma=0
		//fp_type SindSq = 0;
		fp_type SinSqdHalf = 0;
		fp_type gammasca = 0;
		if (location == 0) // point is at the intersection of side bisection lines
		{
			scaleExp_type refScale = 2*refNodeLevel;

			fp_type ScaledSina2Sq = calcEdgeBisection(refScale, SinaSq);
			fp_type ScaledSinb2Sq = calcEdgeBisection(refScale, SinbSq);
			fp_type ScaledSinc2Sq = calcEdgeBisection(refScale, SincSq);

			//SindcSq == (ScaledSina2Sq*(-2 + ScaledSinb2Sq) - 2*(ScaledSinb2Sq - ScaledSinc2Sq + SinaSq + SinbSq))/(2.*(-9 + ScaledSina2Sq + ScaledSinb2Sq + ScaledSinc2Sq))
			SinSqdHalf = calcEdgeBisection(
			                 refScale,
			                 (    ScaledSina2Sq*(LDEXP(ScaledSinb2Sq,-refScale) - 2)
			                      - (ScaledSinb2Sq - ScaledSinc2Sq + SinaSq + SinbSq)*2)/
			                 ((LDEXP(ScaledSina2Sq + ScaledSinb2Sq + ScaledSinc2Sq,-refScale) - 9)*2)
			             );

			//Cosgammasca == SQRT(Power(ScaledSina2Sq*(-2 + ScaledSinb2Sq) - 2*(ScaledSinb2Sq - ScaledSinc2Sq + 2*SinaSq),2)/(-2*ScaledSina2Sq*(-2 + ScaledSinb2Sq)*SinaSq + 4*SinaSq*(ScaledSinb2Sq - ScaledSinc2Sq + SinaSq + SinbSq)))/2.
			gammasca = ACOS( ( 2*(ScaledSinb2Sq - ScaledSinc2Sq + 2*SinaSq)
			                   - ScaledSina2Sq*(LDEXP(ScaledSinb2Sq,-refScale) - 2) )/
			                 SQRT(   (8*ScaledSina2Sq*(2-LDEXP(ScaledSinb2Sq,-refScale))*SinaSq
			                          + 16*SinaSq*(ScaledSinb2Sq - ScaledSinc2Sq + SinaSq + SinbSq)))
			               );
		}
		else if (location == newCode(nodeCode,edgeCode)) //point is the other node on the reference edge
		{
			SinSqdHalf = calcEdgeBisection(2*refNodeLevel, SinaSq);
		}
		else if (location == edgeCode) //point is the node which is not on the reference edge
		{
			scaleExp_type refScale = 2*refNodeLevel;

			SinSqdHalf = calcEdgeBisection(refScale, SinbSq);
			gammasca   = interiorAngle_fromSinSqe(refScale,SinaSq,SinbSq,SincSq);
		}

		return {SinSqdHalf,gammasca};
	}
	else
	{
		size_t nextLevel = refNodeLevel+1;
		unsigned short fCode = at(nextLevel);
		faceGeometry newFaceGeom(faceGeom);
		stepupFaceGeometryFrom(refNodeLevel,fCode,newFaceGeom);

		if (fCode == nodeCode)
		{
			inFacePolarSH subFacePolar(calcFacePolarSH(nextLevel,nodeCode,edgeCode,newFaceGeom,pointLevel,location));
			subFacePolar.SinSqdHalf /= 4;

			return subFacePolar;
		}
		else if (fCode == 0)
		{
			scaleExp_type refScale = 2*refNodeLevel;
			scaleExp_type nextScale = refScale+2;
			unsigned short otherCode = newCode(edgeCode,nodeCode);

			//relevant edge lengths in the sub-face with code "nodeCode"
			fp_type SinaSq = calcEdgeBisection(refScale, faceGeom[edgeCode])/4;
			fp_type SinbSq = calcEdgeBisection(refScale, faceGeom[otherCode])/4;
			fp_type SinSqaHalf = calcEdgeBisection(refScale, SinaSq);

			//distance and angle to the reference node within the center sub-face
			inFacePolarSH subFacePolar(calcFacePolarSH(nextLevel, edgeCode, nodeCode, newFaceGeom, pointLevel, location));
			//distance to the new reference node within the center sub-face
			fp_type SinSqeHalf = subFacePolar.SinSqdHalf/4;

			//angle between original reference edge and the connection line between the point and the new reference node inside the center sub-face
			fp_type delta    = subFacePolar.angle + interiorAngle_fromSinSqe(refScale,SinaSq,newFaceGeom[nodeCode]/4,SinbSq);
			fp_type tmpAngle = COS(delta);

			//distance to the original reference node
			fp_type SinSqdHalf = calcSinSqEdgeHalfSAS(nextScale,SinSqaHalf,tmpAngle,SinSqeHalf);

			//using spherical sine law is unstable near 90 degree angle --> using cosine law & interiorAngle instead
			tmpAngle = SIN(delta)*SQRT((SinSqeHalf-SinSqeHalf*LDEXP(SinSqeHalf,-nextScale))/(SinSqdHalf-SinSqdHalf*LDEXP(SinSqdHalf,-nextScale)));
			if (tmpAngle > 0.95) tmpAngle = interiorAngle_fromSinSqeHalf(nextScale,SinSqaHalf,SinSqdHalf,SinSqeHalf);
			else tmpAngle = ASIN(tmpAngle);

			return {SinSqdHalf,tmpAngle};
		}
		else if (fCode == edgeCode)
		{
			scaleExp_type refScale = 2*refNodeLevel+2;
			unsigned short otherCode = newCode(edgeCode,nodeCode);

			fp_type SinSqaHalf = newFaceGeom[otherCode];
			inFacePolarSH subFacePolar(calcFacePolarSH(nextLevel, edgeCode, otherCode, newFaceGeom, pointLevel, location));

			fp_type SinSqeHalf = subFacePolar.SinSqdHalf/4;
			fp_type delta    = subFacePolar.angle;

			fp_type SinSqdHalf = calcSinSqEdgeHalfSAS(refScale,SinSqaHalf,COS(delta),SinSqeHalf);

			//using spherical sine law is unstable near 90 degree angle --> cannot happen in this case
			return {SinSqdHalf,
			            interiorAngle(refNodeLevel,nodeCode,faceGeom) 
			            - ASIN(SIN(delta) * SQRT((SinSqeHalf-SinSqeHalf*LDEXP(SinSqeHalf,-refScale))/(SinSqdHalf-SinSqdHalf*LDEXP(SinSqdHalf,-refScale))))};
		}
		else
		{
			scaleExp_type refScale = 2*refNodeLevel;
			scaleExp_type nextScale = refScale+2;
			unsigned short otherCode = newCode(edgeCode,nodeCode);

			fp_type SinSqaHalf = calcEdgeBisection(refScale, faceGeom[edgeCode]);
			inFacePolarSH subFacePolar(calcFacePolarSH(nextLevel, otherCode, edgeCode, newFaceGeom, pointLevel, location));

			fp_type SinSqeHalf    = subFacePolar.SinSqdHalf/4;
			fp_type delta     = subFacePolar.angle;

			fp_type SinSqdHalf = calcSinSqEdgeHalfSAS(nextScale,SinSqaHalf,COS(delta),SinSqeHalf);

			//using spherical sine law is unstable near 90 degree angle --> cannot happen in this case
			return {SinSqdHalf,ASIN(SIN(delta) * SQRT((SinSqeHalf-SinSqeHalf*LDEXP(SinSqeHalf,-nextScale))/(SinSqdHalf-SinSqdHalf*LDEXP(SinSqdHalf,-nextScale))))};
		}
	}
	return {0,0};
}



//calculate the distance between two points: both can be node or center point, determined by location
//for a distance d: return value is Sin(d/2), bool reference "mirror" is used to determine if it is the distance to the mirror point
SPIRID::scaledFP
SPIRID::sGrid::sinDistanceHalf(size_t levelP1, const sGrid& P1, unsigned short locationP1,
                               size_t levelP2, const sGrid& P2, unsigned short locationP2,
                               bool& mirror)
{
	//start by finding the highest common node for P1 and P2
	pointPairRefNode commonN(findHighestRefNode(levelP1, P1, levelP2, P2));

	//if no reference node then we calculate the distance to a mirrored point
	mirror = false;
	if (commonN.nodeCode1 == 0) //means no reference node
	{
		mirror = true;
		sGrid P3(P2);
		commonN = findHighestRefNode(levelP1, P1, levelP2, P3.mirror());
	}

	//calculate distance & angle from points to reference nodes
	size_t levelN1 = commonN.level1;
	size_t levelN2 = commonN.level2;
	inFacePolarSH localP1(P1.calcFacePolarSH(levelN1, commonN.nodeCode1, commonN.edgeCode1, levelP1, locationP1));
	inFacePolarSH localP2(P2.calcFacePolarSH(levelN2, commonN.nodeCode2, commonN.edgeCode2, levelP2, locationP2));
	fp_type SinSqaHalf = localP1.SinSqdHalf;
	fp_type SinSqbHalf = localP2.SinSqdHalf;

	//calculate angle between the connections P1-referenceNode-P2
	fp_type cosGapAngle = COS(localP1.angle + commonN.signAngle2*localP2.angle + commonN.gapAngle);

	//check for level differences (occurs when one of the points is closer to the reference node)
	size_t levelOffset = 0;
	size_t minLevel = levelN1;
	if (levelN1>levelN2)
	{
		levelOffset=levelN1-levelN2;
		minLevel = levelN2;
		SinSqaHalf = LDEXP(SinSqaHalf,-levelOffset*2);
	}
	else
	{
		levelOffset=levelN2-levelN1;
		SinSqbHalf = LDEXP(SinSqbHalf,-levelOffset*2);
	}

	//calculate 2^minLevel*Sin(result/2) using special form of spherical cosine law
	fp_type interimResult = robustCalcSinEdgeHalfSAS(2*minLevel+2,SinSqaHalf,cosGapAngle,SinSqbHalf);

	return scaledFP(interimResult, minLevel);


/*
	//start by finding the highest common node for P1 and P2
	pointPairRefNode commonN(findHighestRefNode(levelP1, P1, levelP2, P2));

	//if no reference node then we calculate the distance to a mirrored point
	mirror = false;
	if (commonN.nodeCode1 == 0) //means no reference node
	{
		mirror = true;
		sGrid P3(P2);
		commonN = findHighestRefNode(levelP1, P1, levelP2, P3.mirror());
	}

	//calculate distance & angle from points to reference nodes
	size_t levelN1 = commonN.level1;
	size_t levelN2 = commonN.level2;
	inFacePolar localP1(P1.calcFacePolar(levelN1, commonN.nodeCode1, commonN.edgeCode1, levelP1, locationP1));
	inFacePolar localP2(P2.calcFacePolar(levelN2, commonN.nodeCode2, commonN.edgeCode2, levelP2, locationP2));
	fp_type SinaSq = localP1.SinDistSq;
	fp_type SinbSq = localP2.SinDistSq;

	//calculate angle between the connections P1-referenceNode-P2
	fp_type cosGapAngle = COS(localP1.angle + commonN.signAngle2*localP2.angle + commonN.gapAngle);

	//check for level differences (occurs when one of the points is closer to the reference node)
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

	return scaledFP(interimResult, minLevel);
*/
    
}
//distance calculation for points: return value is the actual distance in [0,pi]
SPIRID::scaledFP
SPIRID::sGrid::distance(size_t levelP1, const sGrid& P1, unsigned short locationP1,
                        size_t levelP2, const sGrid& P2, unsigned short locationP2)
{
	bool mirror = false;
	scaledFP sinDHalf = sinDistanceHalf(levelP1,P1,locationP1,
	                                    levelP2,P2,locationP2,
	                                    mirror);
	fp_type interimResult = sinDHalf.getMantissa();
	scaleExp_type level = sinDHalf.getExponent();

	if (level<minScaleSinXToX)
	{
		interimResult = ASIN(LDEXP(interimResult,-level)); //interimResult == result/2

		if (mirror) // if the point was mirrored then it is most appropriate to return a result without scaling
		{
			interimResult = pi-LDEXP(interimResult,1);
			level = 0;
		}
		else interimResult = LDEXP(interimResult,1+level);
	}
	else
	{
		interimResult = LDEXP(interimResult,1);  //interimResult == 2^level*result (assuming Sin(x)~x)

		if (mirror)
		{
			interimResult = pi-LDEXP(interimResult,-level);
			level = 0;
		}
	}

	return scaledFP(interimResult, level);
}
