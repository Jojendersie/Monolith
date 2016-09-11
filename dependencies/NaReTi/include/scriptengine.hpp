#include "enginetypes.hpp"
#include "module.hpp"
#include "moduleloader.hpp"
#include <assert.h>

#pragma once

namespace codeGen{
	class Compiler;
	class Optimizer;
}
namespace par{
	class Parser;
	class GenericsParser;
}

namespace lang{
	struct BasicModule;
}


namespace NaReTi{

	class ScriptEngine
	{
	public:
		ScriptEngine(const std::string& _scriptLoc);
		~ScriptEngine();

		// Loads the module if it is not found.
		Module* getModule(const std::string& _name);
		//compiles a given source file and adds the module
		//the name is the file name without ending.
		// @param _dest when provided symbols are added to this module
		bool loadModule(const std::string& _fileName, NaReTi::Module* _dest = nullptr);
		bool loadModule(NaReTi::Module* _module);

		// Removes a module with the given name and all it's symbols to release it's memory.
		// This should only be used if no other module depends on the module.
		// @param _keepBinary If true no compiled functions are removed
		bool unloadModule(const std::string& _moduleName, bool _keepBinary = false);

		// Deletes and loads a module.
		// Use this function if you want to recompile a module.
		bool reloadModule(const std::string& _moduleName);
		bool reloadModule(NaReTi::Module* _module);

		//Creates a new and empty module to build extern.
		Module& createModule(const std::string& _moduleName);

		//returns a handle to the function with the given name if existent
		FunctionHandle getFuncHndl(const std::string& _name) const;

		//call without template params(just for convenience)
		void call(FunctionHandle _hndl) const;
		
		template< typename _Ret, typename... _Args>
		_Ret call(FunctionHandle _hndl, _Args... _args) const
		{
			assert((bool)_hndl);

			//magic typecasting to the given function type
			auto func = (_Ret (*) (_Args...))_hndl.ptr;
			//the call
			return func(_args...);
		};

		Config& config() { return m_config; }
	private:
		Config m_config;
		ModuleLoader m_moduleLoader;

		std::string extractName(const std::string& _fullName);

		lang::BasicModule* m_basicModule;

		par::Parser* m_parser;
		par::GenericsParser* m_genericsParser;

		codeGen::Compiler* m_compiler;
		codeGen::Optimizer* m_optimizer;

		std::vector< FunctionHandle > m_nativeFunctions;
		std::vector< std::unique_ptr<Module> > m_modules;
	};

}
