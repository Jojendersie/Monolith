#pragma once


#include "parpredecs.hpp"
#include "complexalloc.hpp"
#include "enginetypes.hpp"

#include <vector>
#include <string>
#include <memory>

namespace NaReTi{

	typedef std::pair<std::string, par::ComplexType*> TypeAlias;

	class Module
	{
	public:
		Module(const std::string& _name);
		~Module();

		par::ComplexType* getType(const std::string& _name);
		TypeAlias* getTypeAlias(const std::string& _name);
		
		/* Structure that holds function querys.
		 */
		struct FuncMatch
		{
			FuncMatch(par::Function& _func) : function(&_func), diff(0){}
			par::Function* function;
			int diff; // count of params and args that match

			// allow sorting
			bool operator<(FuncMatch& _oth)
			{
				return diff < _oth.diff;
			}
		};
		/*search for a function with the given signature
		 * _name the name of the function 
		 * _begin, _end range on a param stack with the given arguments
		 * _funcQuery Destination where partly matches(same name, param count) are stored
		 * @ret A function with perfect match or nullptr
		 */
		par::Function* getFunction(const std::string& _name, 
			const std::vector<par::ASTExpNode*>::iterator& _begin,
			const std::vector<par::ASTExpNode*>::iterator& _end,
			std::vector<FuncMatch>& _funcQuery);

		// simpler function search that will return the first name match
		par::Function* getFunction(const std::string& _name, bool _external = false);

		// get a local var from this module
		par::VarSymbol* getGlobalVar(const std::string& _name);

		struct ExportVarIterator
		{
			ExportVarIterator(std::vector< par::VarSymbol* >& _variables);

			ExportVarIterator& operator++();
			VariableHandle get() const;
			operator bool() { return m_iterator != m_variables.end(); };
		private:
			std::vector< par::VarSymbol* >::iterator m_iterator;
			std::vector< par::VarSymbol* >& m_variables;
		};
		ExportVarIterator getExportVars();

		utils::DetorAlloc& getAllocator() { return m_allocator; }

		//links an as external declared func symbol in this module to the given ptr.
		bool linkExternal(const std::string& _name, void* _funcPtr);

		// flag that shows whether this module had any breaking changes since the last compile
		void setChanged(bool _b){ m_hasChanged = _b; }
		bool hasChanged() const { return m_hasChanged; }

		//their lifetime equals the module's
		//use a stack allocator for all related buffers if possible
		std::vector < par::ComplexType* > m_types;
		std::vector < par::GenericType* > m_genericTypes;
		std::vector < par::Function* > m_functions;
		std::vector < TypeAlias > m_typeAlias;
		par::ASTCode* m_text;

		std::string m_name;

		std::vector < Module* > m_dependencies;
	protected:
		bool m_hasChanged; // needs a recompile

		// allocator for the ast-nodes
		utils::DetorAlloc m_allocator;
	};
}