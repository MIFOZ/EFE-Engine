#ifndef EFE_GUI_TYPES_H
#define EFE_GUI_TYPES_H

#include "math/MathTypes.h"
#include "Graphics/GraphicsTypes.h"
#include "input/InputTypes.h"

namespace efe
{
	enum eGuiMouseButton
	{
		eGuiMouseButton_Left =			0x00000001,
		eGuiMouseButton_Middle =		0x00000002,
		eGuiMouseButton_Right =			0x00000004,
		eGuiMouseButton_LastEnum = 4
	};

	enum eGuiMaterial
	{
		eGuiMaterial_Diffuse,
		eGuiMaterial_Alpha,
		eGuiMaterial_FontNormal,
		eGuiMaterial_Additive,
		eGuiMaterial_Modulative,
		eGuiMaterial_Inverse,
		eGuiMaterial_LastEnum
	};

	enum eWidgetType
	{
		eWidgetType_Root,
		eWidgetType_Window,
		eWidgetType_Button,
		eWidgetType_Frame,
		eWidgetType_Label,
		eWidgetType_Slider,
		eWidgetType_TextBox,
		eWidgetType_CheckBox,
		eWidgetType_Image,
		eWidgetType_ListBox,
		eWidgetType_ComboBox,

		eWidgetType_User,
		eWidgetType_LastEnum
	};

	enum eGuiSkinFont
	{
		eGuiSkinFont_Default,
		eGuiSkinFont_Disabled,
		eGuiSkinFont_WindowLabel,

		eGuiSkinFont_LastEnum
	};

	enum eGuiSkinAttribute
	{
		eGuiSkinAttribute_WindowLabelTextOffset,
		eGuiSkinAttribute_ButtonPressedContentOffset,
		eGuiSkinAttribute_SliderButtonSize,

		eGuiSkinAttribute_ListBoxSliderWidth,

		eGuiSkinAttribute_ComboBoxButtonWidth,
		eGuiSkinAttribute_ComboBoxSliderWidth,

		eGuiSkinAttribute_LastEnum
	};

	enum eGuiSkinGfx
	{
		eGuiSkinGfx_PointerNormal,
		eGuiSkinGfx_PointerText,

		eGuiSkinGfx_WindowBorderRight,
		eGuiSkinGfx_WindowBorderLeft,
		eGuiSkinGfx_WindowBorderUp,
		eGuiSkinGfx_WindowBorderDown,

		eGuiSkinGfx_WindowCornerLU,
		eGuiSkinGfx_WindowCornerRU,
		eGuiSkinGfx_WindowCornerRD,
		eGuiSkinGfx_WindowCornerLD,

		eGuiSkinGfx_WindowLabel,
		eGuiSkinGfx_WindowBackground,

		eGuiSkinGfx_FrameBorderRight,
		eGuiSkinGfx_FrameBorderLeft,
		eGuiSkinGfx_FrameBorderUp,
		eGuiSkinGfx_FrameBorderDown,

		eGuiSkinGfx_FrameCornerLU,
		eGuiSkinGfx_FrameCornerRU,
		eGuiSkinGfx_FrameCornerRD,
		eGuiSkinGfx_FrameCornerLD,

		eGuiSkinGfx_FrameBackground,

		eGuiSkinGfx_CheckBoxEnabledUnchecked,
		eGuiSkinGfx_CheckBoxEnabledChecked,
		eGuiSkinGfx_CheckBoxDisabledUnchecked,
		eGuiSkinGfx_CheckBoxDisabledChecked,

		eGuiSkinGfx_TextBoxBackground,
		eGuiSkinGfx_TextBoxSelectedTextBack,
		eGuiSkinGfx_TextBoxMarker,

		eGuiSkinGfx_ListBoxBackground,

		eGuiSkinGfx_ComboBoxButtonIcon,

		eGuiSkinGfx_ComboBoxBorderRight,
		eGuiSkinGfx_ComboBoxBorderLeft,
		eGuiSkinGfx_ComboBoxBorderUp,
		eGuiSkinGfx_ComboBoxBorderDown,

		eGuiSkinGfx_ComboBoxCornerLU,
		eGuiSkinGfx_ComboBoxCornerRU,
		eGuiSkinGfx_ComboBoxCornerRD,
		eGuiSkinGfx_ComboBoxCornerLD,

		eGuiSkinGfx_ComboBoxBackground,

		eGuiSkinGfx_SliderVertArrowUp,
		eGuiSkinGfx_SliderVertArrowDown,
		eGuiSkinGfx_SliderVertBackground,

		eGuiSkinGfx_SliderHoriArrowUp,
		eGuiSkinGfx_SliderHoriArrowDown,
		eGuiSkinGfx_SliderHoriBackground,

		eGuiSkinGfx_ButtonUpBorderRight,
		eGuiSkinGfx_ButtonUpBorderLeft,
		eGuiSkinGfx_ButtonUpBorderUp,
		eGuiSkinGfx_ButtonUpBorderDown,

		eGuiSkinGfx_ButtonUpCornerLU,
		eGuiSkinGfx_ButtonUpCornerRU,
		eGuiSkinGfx_ButtonUpCornerRD,
		eGuiSkinGfx_ButtonUpCornerLD,

		eGuiSkinGfx_ButtonUpBackground,

		eGuiSkinGfx_ButtonDownBorderRight,
		eGuiSkinGfx_ButtonDownBorderLeft,
		eGuiSkinGfx_ButtonDownBorderUp,
		eGuiSkinGfx_ButtonDownBorderDown,

		eGuiSkinGfx_ButtonDownCornerLU,
		eGuiSkinGfx_ButtonDownCornerRU,
		eGuiSkinGfx_ButtonDownCornerRD,
		eGuiSkinGfx_ButtonDownCornerLD,

		eGuiSkinGfx_ButtonDownBackground,

		eGuiSkinGfx_LastEnum
	};

	enum eWidgetSliderOrientation
	{
		eWidgetSliderOrientation_Horisontal,
		eWidgetSliderOrientation_Vertical,
		eWidgetSliderOrientation_LastEnum
	};

	enum eGuiMessage
	{
		eGuiMessage_MouseMove,
		eGuiMessage_MouseDown,
		eGuiMessage_MouseUp,
		eGuiMessage_MouseDoubleClick,
		eGuiMessage_MouseEnter,
		eGuiMessage_MouseLeave,

		eGuiMessage_GotFocus,
		eGuiMessage_LostFocus,

		eGuiMessage_OnDraw,

		eGuiMessage_ButtonPressed,

		eGuiMessage_TextChange,

		eGuiMessage_CheckChange,

		eGuiMessage_KeyPress,

		eGuiMessage_SliderMove,

		eGuiMessage_SelectionChange,

		eGuiMessage_LastEnum
	};

	struct cGuiMessageData
	{
		cGuiMessageData(){}
		cGuiMessageData(const cVector2f &a_vPos, const cVector2f &a_vRel)
		{
			m_vPos = a_vPos;
			m_vRel = a_vRel;
		}
		cGuiMessageData(const cVector2f &a_vPos, const cVector2f &a_vRel, int a_lVal)
		{
			m_vPos = a_vPos;
			m_vRel = a_vRel;
			m_lVal = a_lVal;
		}
		cGuiMessageData(int a_lVal)
		{
			m_lVal = a_lVal;
		}
		cGuiMessageData(float a_fVal)
		{
			m_fVal = a_fVal;
		}
		cGuiMessageData(const cKeyPress &a_KeyPress)
		{
			m_KeyPress = a_KeyPress;
		}

		cVector2f		m_vPos;
		cVector2f		m_vRel;
		int				m_lVal;
		cKeyPress		m_KeyPress;
		float			m_fVal;
		void			*m_pData;
		eGuiMessage		m_Message;
	};

	class iWidget;

	typedef bool (*tGuiCallbackFunc)(void*,iWidget*,cGuiMessageData&);

	#define kGuiCallbackDeclarationEnd(FuncName) \
		static bool FuncName##_static_gui(void *a_pObject, iWidget *a_pWidget, cGuiMessageData &a_Data);

	#define kGuiCallbackDeclaredFuncEnd(ThisClass,FuncName) \
		bool ThisClass::FuncName##_static_gui(void *a_pObject, iWidget *a_pWidget, cGuiMessageData &a_Data) \
		{\
			return ((ThisClass*)a_pObject)->FuncName(a_pWidget, a_Data); \
		}

	#define kGuiCallbackFuncEnd(ThisClass,FuncName) \
		static bool FuncName##_static_gui(void *a_pObject, iWidget *a_pWidget, cGuiMessageData &a_Data) \
		{\
			return ((ThisClass*)a_pObject)->FuncName(a_pWidget, a_Data); \
		}

	#define kGuiCallback(FuncName) &FuncName##_static_gui

	typedef std::list<iWidget*> tWidgetList;
	typedef tWidgetList::iterator tWidgetListIt;

	class cGuiGfxElement;

	typedef std::list<cGuiGfxElement*> tGuiGfxElementList;
	typedef tGuiGfxElementList::iterator tGuiGfxElementListIt;
};

#endif