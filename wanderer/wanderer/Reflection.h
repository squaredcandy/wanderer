#pragma once


namespace Wanderer::Engine::Reflection
{
	template<typename T> std::string ReflectHandleNumber(const char * name, 
		std::string& format, T var)
	{
		char ret[50];
		sprintf_s(ret, format.c_str(), name, var);
		return std::string{ ret };
	}

	template<typename T> std::string ReflectHandleString(const char * name,
		std::string& format, T var)
	{
		char ret[50];
		sprintf_s(ret, format.c_str(), name, var);
		return std::string{ ret };
	}

	template<typename T> std::string Reflect(const char * name, T var)
	{
		std::string format{ "%s: " };

		// Floating Point Numbers
		if (typeid(T) == typeid(float) || typeid(T) == typeid(double))
		{
			format.append("%.3f");
			return ReflectHandleNumber(name, format, var);
		}
		// Whole Numbers
		else if (typeid(T) == typeid(int) || typeid(T) == typeid(unsigned int))
		{
			format.append("%d");
			return ReflectHandleNumber(name, format, var);
		}
		// Strings
		else if (typeid(T) == typeid(const char *))
		{
			format.append("%s");
			return ReflectHandleString(name, format, var);
		}
		

		return std::string{ "" };
	}

}

#define GETVARVALUE(A) Wanderer::Engine::Reflection::Reflect(#A, A)