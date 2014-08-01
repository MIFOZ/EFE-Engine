#ifndef EFE_WIDGET_BASE_CLASSES_H
#define EFE_WIDGET_BASE_CLASSES_H

#include "gui/GuiTypes.h"

namespace efe
{
	class iWidgetItemContainer
	{
	public:
		void AddItem(const tWString &a_sItem);
		void RemoveItem(int a_lX);
		void RemoveItem(const tWString &a_sItem);

		const tWString &GetItem(int a_lX);
		void SetItem(int a_lX, const tWString &a_sItem);
		int GetItemNum() const;
		bool HasItem(const tWString &a_sItem);

	protected:
		virtual void UpdateProperties() = 0;

		tWStringVec m_vItems;
	};
};

#endif