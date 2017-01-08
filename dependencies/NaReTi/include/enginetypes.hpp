#pragma once

#include <string>

namespace NaReTi{

	/* types used by the script engine
	 */

	typedef void basicFunc(void);

	/* A function handle that can be used to let the script engine
	 * call the associated function.
	 */
	struct FunctionHandle
	{
		friend class ScriptEngine;

		FunctionHandle() : ptr(nullptr){}

		FunctionHandle(basicFunc* _ptr)
		{
			ptr = _ptr;
		}
		
		template< typename _T>
		FunctionHandle(_T* _ptr)
		{
			ptr = (void*)_ptr;
		}

		operator bool() const
		{
			return ptr != nullptr;
		}

	private:
		void* ptr;
	};

	struct VariableHandle
	{
		const void* ptr;
		const std::string& name;
	private:
		friend class Module;
		VariableHandle(void* _ptr, const std::string& _name): ptr(_ptr), name(_name){};
	};

	/* Configuration options
	*/

	enum OptimizationLvl
	{
		None,
		//const folding, var substitution, inlining
		Basic
	};

	struct LoaderConfig
	{
		//path where the script files are found
		std::string scriptLocation;
	};

	struct Config : public LoaderConfig
	{
		OptimizationLvl optimizationLvl;
	};
}