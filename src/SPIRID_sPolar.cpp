#include <cmath>
#include <SPIRID_aux.h>

//haversine formula for the distance
fp_type
SPIRID::sPolar::distance(
    const sPolar& P1,
    const sPolar& P2
)
{
	fp_type sinDeltaThetaHalf  = SIN((P1.theta - P2.theta)/2);
	fp_type sinDeltaPhiHalf    = SIN((P1.phi   - P2.phi  )/2);
	fp_type sinTheta1plus2Half = SIN((P1.theta + P2.theta)/2);

	fp_type distance = 2*ASIN(SQRT(sinDeltaThetaHalf*sinDeltaThetaHalf + sinDeltaPhiHalf*sinDeltaPhiHalf*(sinTheta1plus2Half*sinTheta1plus2Half-sinDeltaThetaHalf*sinDeltaThetaHalf)));

	/* result accuracy becomes worse for large distances and unstable near pi
	    therefore we calculate the distance to a mirror point and substract from pi in the end
	    drawback: may need to execute calculation twice
	*/
	if (distance > pi_2)
	{
		sinDeltaThetaHalf  = SIN((P1.theta + P2.theta - pi)/2);
		sinDeltaPhiHalf    = SIN((P1.phi   - P2.phi -   pi)/2);
		sinTheta1plus2Half = SIN((P1.theta + pi - P2.theta)/2);

		distance = pi - 2*ASIN(SQRT(sinDeltaThetaHalf*sinDeltaThetaHalf + sinDeltaPhiHalf*sinDeltaPhiHalf*(sinTheta1plus2Half*sinTheta1plus2Half-sinDeltaThetaHalf*sinDeltaThetaHalf)));
	}

	return distance;
}

//interior angle at Q a triangle in polar coordinates
fp_type SPIRID::sPolar::interiorAngle(
    const sPolar& P,
    const sPolar& Q,
    const sPolar& R
)
{
	fp_type dPQ = distance(P,Q);
	fp_type dPR = distance(P,R);
	fp_type dQR = distance(Q,R);

	return ACOS( (COS(dPR)-COS(dPQ)*COS(dQR)) / (SIN(dPQ)*SIN(dPR)) );
}
//orientation of a triangle in polar coordinates
unsigned short SPIRID::sPolar::orientation(
    const sPolar& P,
    const sPolar& Q,
    const sPolar& R
)
{
	fp_type Px = SIN(P.theta)*COS(P.phi);
	fp_type Py = SIN(P.theta)*SIN(P.phi);
	fp_type Pz = COS(P.theta);

	fp_type Qx = SIN(Q.theta)*COS(Q.phi);
	fp_type Qy = SIN(Q.theta)*SIN(Q.phi);
	fp_type Qz = COS(Q.theta);

	fp_type Rx = SIN(R.theta)*COS(R.phi);
	fp_type Ry = SIN(R.theta)*SIN(R.phi);
	fp_type Rz = COS(R.theta);

	if (std::signbit(Px*(Qy*Rz-Qz*Ry)+Py*(Qz*Rx-Qx*Rz)+Pz*(Qx*Ry-Qy*Rx))) return -1;
	return 1;
}

//oriented area of a triangle in polar coordinates
fp_type SPIRID::sPolar::orientedArea(
    const sPolar& P,
    const sPolar& Q,
    const sPolar& R
)
{
	fp_type Px = SIN(P.theta)*COS(P.phi);
	fp_type Py = SIN(P.theta)*SIN(P.phi);
	fp_type Pz = COS(P.theta);

	fp_type Qx = SIN(Q.theta)*COS(Q.phi);
	fp_type Qy = SIN(Q.theta)*SIN(Q.phi);
	fp_type Qz = COS(Q.theta);

	fp_type Rx = SIN(R.theta)*COS(R.phi);
	fp_type Ry = SIN(R.theta)*SIN(R.phi);
	fp_type Rz = COS(R.theta);

	return 2*ATAN(
	           (Px*(Qy*Rz-Qz*Ry) + Py*(Qz*Rx-Qx*Rz) + Pz*(Qx*Ry-Qy*Rx)) /
	           (1 + Px*Qx+Py*Qy+Pz*Qz + Px*Rx+Py*Ry+Pz*Rz + Qx*Rx+Qy*Ry+Qz*Rz));
}



const SPIRID::sPolar& SPIRID::sPolar::set(fp_type t, fp_type p)
{
	theta = t;
	phi = p;
	normalize();
	return *this;
}
void SPIRID::sPolar::normalize()
{
	theta = std::fmod(theta,two_pi);
	if (theta<0) theta+=two_pi;
	if (theta>pi)
	{
		theta=two_pi-theta;
		phi+=pi;
	}

	phi = std::fmod(phi,two_pi);
	if (phi<0) phi+=two_pi;
}



std::ostream& SPIRID::operator << (std::ostream& out, const sPolar& P)
{
	out << "(" << angle::convertFromRadian(P.getTheta()) << angle::unitSymbol << "," << angle::convertFromRadian(P.getPhi()) << angle::unitSymbol << ")";
	return out;
}
