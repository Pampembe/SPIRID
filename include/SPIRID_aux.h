#ifndef SPIRID_AUX_H_INCLUDED
#define SPIRID_AUX_H_INCLUDED

#include <ostream>
#include <cmath> //for standard math functions and to define constants related to pi
#include <algorithm>


#define fp_type float //standard floating point type for calculations
#define SQRT sqrtf //sqrt for float numbers
#define ACOS acosf //acos for float numbers
#define ASIN asinf //asin for float numbers
#define ATAN atanf //atan for float numbers
#define SIN  sinf //sin for float numbers
#define COS  cosf //cos for float numbers
#define SIGN  signf //cos for float numbers
#define LDEXP ldexpf //multiply float by 2^exp
#define ABS std::abs
/*
#define fp_type double //standard floating point type for calculations
#define SQRT sqrt //sqrt for double numbers
#define ACOS acos //acos for double numbers
#define ASIN asin //asin for double numbers
#define ATAN atan //atan for double numbers
#define SIN  sin //sin for double numbers
#define COS  cos //cos for double numbers
#define LDEXP ldexp //multiply double by 2^exp
#define ABS std::abs

#define fp_type long double //standard floating point type for calculations
#define SQRT sqrtl //sqrt for long double numbers
#define ACOS acosl //acos for long double numbers
#define ASIN asinl //asin for long double numbers
#define ATAN atanl //atan for long double numbers
#define SIN  sinl //sin for long double numbers
#define COS  cosl //cos for long double numbers
#define LDEXP ldexpl //multiply long double by 2^exp
#define ABS std::abs
*/

#define scaleExp_type unsigned long long

namespace SPIRID
{
//constants related to pi
const fp_type pi = M_PI;
const fp_type pi_2 = M_PI_2;
const fp_type two_pi = 2*M_PI;



//floating point type allowing arbitrarily small numbers by including an (unsigned negative) scaling factor
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
	inline scaledFP(fp_type m = 0, scaleExp_type e = 0) : mantissa(m), scaleExponent(e) {}; //standard constructor

	//get mantissa and exponent
	inline fp_type getMantissa() const {
		return mantissa;
	};
	inline scaleExp_type getExponent() const {
		return scaleExponent;
	};

	inline bool operator < (const scaledFP& y) const
	{
		if (y.scaleExponent > scaleExponent)
		{
			return (LDEXP(mantissa,y.scaleExponent-scaleExponent) < y.mantissa);
		}
		return (mantissa < LDEXP(y.mantissa,scaleExponent-y.scaleExponent));
	}
	inline bool operator > (const scaledFP& y) const {
		return !(operator < (y));
	};
	inline bool operator != (const scaledFP& y) const
	{
		if (y.scaleExponent > scaleExponent)
		{
			return (LDEXP(mantissa,y.scaleExponent-scaleExponent) != y.mantissa);
		}
		return (mantissa != LDEXP(y.mantissa,scaleExponent-y.scaleExponent));
	}
	inline bool operator == (const scaledFP& y) const
	{
		return !(operator != (y));
	}

	scaledFP operator + (const scaledFP&) const;
	scaledFP operator - (const scaledFP&) const;
	scaledFP operator * (const scaledFP&) const;
	scaledFP operator / (const scaledFP&) const;

	scaledFP abs() const;
	fp_type toFPType() const; //convert to standard float
};
//standard output for scaledFP
std::ostream& operator << (std::ostream& out, const scaledFP&);





//class angle: automatically converts a number to a given unit (Rad/Deg/Pi) on output
class angle
{

public:
	template<typename number>
	inline static number convertFromRadian(number X) {
		return X*unitScale;
	};

	static fp_type unitScale; // to convert radians/deg/...: radian-->unitScale=1; deg-->unitScale=180/Pi
	static std::string unitSymbol; //after switching unit the output functions adds a symbol

	//functions to change unit (global)
	static void unitPi();
	static void unitRad();
	static void unitDeg();
};





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
	inline sPolar(const sPolar& Q) : theta(Q.theta), phi(Q.phi) {};
	inline sPolar() : theta(0), phi(0) {};
	const sPolar& set(fp_type t, fp_type p);
	inline fp_type getTheta() const {
		return theta;
	};
	inline fp_type getPhi() const {
		return phi;
	};

	//geodesic distance between two points on the sphere
	static fp_type distance(const sPolar& P1, const sPolar& P2);
	//interior angle at Q a triangle in polar coordinates
	static fp_type interiorAngle(
	    const sPolar& P,
	    const sPolar& Q,
	    const sPolar& R);
	//orientation of a triangle in polar coordinates
	static unsigned short orientation(
	    const sPolar& P,
	    const sPolar& Q,
	    const sPolar& R);
	//oriented area of a triangle in polar coordinates
	static fp_type orientedArea(
	    const sPolar& P,
	    const sPolar& Q,
	    const sPolar& R);
};
//output for polar coordinates
std::ostream& operator << (std::ostream& out, const sPolar&);


}

#endif //SPIRID_AUX_H_INCLUDED
