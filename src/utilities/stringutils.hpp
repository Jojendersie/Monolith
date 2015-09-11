#pragma once

#include <string>

namespace StringUtils
{
	template < typename _T >
	std::string ToConstDigit(_T _num, unsigned int _digits, unsigned int _numDigits = 0)
	{
		std::string ret = std::to_string(_num);


		auto i = _numDigits ? _numDigits : ret.size();
		ret.resize(_digits);

		//copy string to the end
		auto begin = _digits - i;
		for (size_t j = begin; j < _digits; ++j)
			ret[j] = ret[j - begin];

		//fill begining with spaces
		for (size_t j = 0; j < begin; ++j)
			ret[j] = ' ';

		return ret;
	}


	/// \brief Returns _num as string with up to _digits after the point.
	template < typename _T >
	std::string ToFixPoint(_T _num, unsigned int _digits)
	{
		std::string ret = std::to_string(_num);

		size_t i = ret.find('.');

		ret.resize(i + _digits + 1);

		return ret;
	}

} 