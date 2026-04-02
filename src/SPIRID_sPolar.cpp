#include <cmath>
#include <SPIRID_aux.h>

const SPIRID::sPolar& SPIRID::sPolar::set(fp_type t, fp_type p)
{
	theta = t;
	phi = p;
	normalize();
	return *this;
}

void SPIRID::sPolar::normalize()
{
	theta = std::fmod(theta,2*M_PI);
	if (theta<0) theta+=2*M_PI;
	if (theta>M_PI)
	{
		theta=2*M_PI-theta;
		phi+=M_PI;
	}

	phi = std::fmod(phi,2*M_PI);
	if (phi<0) phi+=2*M_PI;
}

//haversine formula
SPIRID::angle SPIRID::sPolar::distance(const sPolar& P1, const sPolar& P2)
{
	fp_type sinDeltaThetaHalf  = SIN((P1.theta - P2.theta)/2);
	fp_type sinDeltaPhiHalf    = SIN((P1.phi   - P2.phi  )/2);
	fp_type sinTheta1plus2Half = SIN((P1.theta + P2.theta)/2);

	//For large distances near pi the calculation becomes numerically unstable. Therefore we calculate the offset to the mirror point.
	bool mirror = false;
	if (ABS(P1.phi - P2.phi) > pi_2)
	{
		sinDeltaThetaHalf  = SIN((P1.theta + P2.theta - pi)/2);
		sinDeltaPhiHalf    = SIN((P1.phi   - P2.phi - pi)/2);
		sinTheta1plus2Half = SIN((P1.theta + pi - P2.theta)/2);
		mirror = true;
	}
	fp_type distance = 2*ASIN(SQRT(sinDeltaThetaHalf*sinDeltaThetaHalf + sinDeltaPhiHalf*sinDeltaPhiHalf*(sinTheta1plus2Half*sinTheta1plus2Half-sinDeltaThetaHalf*sinDeltaThetaHalf)));
	if (mirror) distance = pi-distance;

	return angle(distance);
}


std::ostream& SPIRID::operator << (std::ostream& out, const sPolar& P)
{
	out << "(" << P.getTheta() << "," << P.getPhi() << ")";
	return out;
}
