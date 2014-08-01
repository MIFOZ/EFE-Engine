#ifndef EFE_GUI_POP_UP_MESSAGE_BOX
#define EFE_GUI_POP_UP_MESSAGE_BOX

#include "gui/GuiPopUp.h"

namespace efe
{
	class cWidgetWindow;
	class cWidgetLabel;
	class cWidgetButton;

	class cGuiPopUpMessageBox : public iGuiPopUp
	{
	public:
		cGuiPopUpMessageBox(cGuiSet *a_pSet,
			const tWString &a_sLabel, const tWString &a_sText,
			const tWString &a_sButton1, const tWString &a_sButton2,
			void *a_pCallbackObject, tGuiCallbackFunc a_pCallback);
		~cGuiPopUpMessageBox();

	protected:
		bool ButtonPress(iWidget *a_pWidget, cGuiMessageData &a_Data);
		kGuiCallbackDeclarationEnd(ButtonPress);

		cWidgetWindow *m_pWindow;
		cWidgetButton *m_vButtons[2];
		cWidgetLabel *m_pLabel;

		iWidget *m_pPrevAttention;

		void *m_pCallbackObject;
		tGuiCallbackFunc m_pCallback;
	};
};

#endif