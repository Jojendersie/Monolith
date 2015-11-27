#include <functional>
#include <vector>

#include "predeclarations.hpp"
#include "..\managment\controller.hpp"
#include "..\..\graphic\interface\messagebox.hpp"

namespace Gameplay{

/*	struct DialogChoice
	{
		std::string m_description;
		
		bool m_isPermanent;
		bool m_alreadyUsed;

		std::function<bool()> m_condition;
		std::function<void()> m_talk;
	};

	class Dialog
	{
	public:
		/// \brief A simple always true condition.
		static bool NoCondition() { return true; };

		//global dialog vars
		static Controller& m_self;
		static Controller& m_other;

		Dialog(MessageBox& _msgBox,Controller& _self, Controller& _other);

		/// \brief Adds an option to the dialog box.
		void AddChoice(const std::string& _description, std::function<void()> _talk, std::function<bool()> _condition = NoCondition, bool _isPermanent = false);

		/// \brief Shows all currently avialable choices.
		/// \details Takes all choices created with AddChoice() since the last call
		///			to ClearChoices().
		void ShowChoices();

		/// \brief Clears the currently avialable choices.
		/// \details Excludes init choices.
		void ClearChoices();
	private:
		/// \brief Called on the start of a dialog to handle initial options.
		virtual void Init() abstract;

		void Say(Controller& _source, const std::string& _text);

	//	Graphic::Hud& m_hud;
		MessageBox& m_messageBox;

		std::vector < DialogChoice& > m_currentChoices;
	};
	*/
}