#include <limits>
#include <SPIRID_aux.h>

const size_t SPIRID::scaledFP::FPDigits = std::numeric_limits<fp_type>::digits;
const scaleExp_type SPIRID::scaledFP::FPMaxExponent = -std::numeric_limits<fp_type>::min_exponent;

/*
SPIRID::scaledFP
SPIRID::scaledFP::operator + (const scaledFP& X) const
{
	if (X.scaleExponent > scaleExponent)
	{
		return {mantissa + LDEXP(X.mantissa,scaleExponent-X.scaleExponent), scaleExponent};
	}
	return {X.mantissa + LDEXP(mantissa,X.scaleExponent-scaleExponent), X.scaleExponent};
};
*/
SPIRID::scaledFP
SPIRID::scaledFP::operator - (const scaledFP& X) const
{
	if (X.scaleExponent > scaleExponent)
	{
		return {mantissa - LDEXP(X.mantissa,scaleExponent-X.scaleExponent), scaleExponent};
	}
	return {X.mantissa - LDEXP(mantissa,X.scaleExponent-scaleExponent), X.scaleExponent};
};

SPIRID::scaledFP::operator fp_type() const
{
	return LDEXP(mantissa,-scaleExponent);
};

std::ostream& SPIRID::operator << (std::ostream& out, const SPIRID::scaledFP& x)
{
	out << x.getMantissa();
	if(x.getExponent() != 0) out << "/2^" << x.getExponent();
	return out;

	/*
		size_t printExponent = alpha.getExponent();
		int exponent=0;
		bool exponentSignChange = false;
		fp_type mantissa = frexp(alpha.getMantissa()*angle::unitScale,&exponent);
		if (exponent<=0)
		{
			printExponent -= exponent;
		}
		else
		{
			if (exponent<=printExponent)
			{
				printExponent -= exponent;
			}
			else
			{
				printExponent = exponent - printExponent;
				exponentSignChange = true;
			}
		}

	if (exponentSignChange)
		{
	        out << mantissa << SPIRID::angle::unitSymbol << "*2^" << printExponent << std::endl;
		}
		else
		{
	        out << mantissa << SPIRID::angle::unitSymbol << "/2^" << printExponent << std::endl;
		}

		fp_type mantissaModifier = log10(2)*printExponent;
		printExponent = round(mantissaModifier);

		if (exponentSignChange)
		{
			mantissa *= pow(10,mantissaModifier-printExponent);
	        if (printExponent==0)
	        {
			    out << mantissa << SPIRID::angle::unitSymbol;
	        }
	        else
	        {
			    out << mantissa << "E" << printExponent << SPIRID::angle::unitSymbol;
	        }
		}
		else
		{
			mantissa /= pow(10,mantissaModifier-printExponent);
	        if (printExponent==0)
	        {
			    out << mantissa << SPIRID::angle::unitSymbol;
	        }
	        else
	        {
	    		out << mantissa << "E-" << printExponent << SPIRID::angle::unitSymbol;
	        }
		}
		return out;
	*/
}
