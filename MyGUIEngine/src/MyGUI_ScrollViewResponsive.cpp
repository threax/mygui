/*
 * This source file is part of MyGUI. For the latest info, see http://mygui.info/
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "MyGUI_Precompiled.h"
#include "MyGUI_ContainerResponsive.h"
#include "MyGUI_ScrollViewResponsive.h"
#include "MyGUI_Gui.h"

namespace MyGUI
{
	ScrollViewResponsive::ScrollViewResponsive()
		:rowColumns(12),
		horizontalMaxSize(Gui::getInstancePtr()->scalePreserve(300)),
		padding(Gui::getInstancePtr()->scalePreserve(2), Gui::getInstancePtr()->scalePreserve(2))
	{
	}

	ScrollViewResponsive::~ScrollViewResponsive()
	{

	}

	void ScrollViewResponsive::setPropertyOverride(const std::string& _key, const std::string& _value)
	{

		if (_key == "RowColumns")
			rowColumns = utility::parseValue<int>(_value);

		else if (_key == "HorizontalMaxSize")
			horizontalMaxSize = Gui::getInstancePtr()->scalePreserve(utility::parseValue<int>(_value));

		else if (_key == "Padding")
			padding = Gui::getInstance().scalePreserve(utility::parseValue<IntSize>(_value));

		else
		{
			Base::setPropertyOverride(_key, _value);
			return;
		}

		eventChangeProperty(this, _key, _value);
	}

	/** Set widget size */
	void ScrollViewResponsive::setSize(const IntSize& _value)
	{
		ScrollView::setSize(_value);
		repositionChildren();
	}
	/** Set widget position and size */
	void ScrollViewResponsive::setCoord(const IntCoord& _value)
	{
		ScrollView::setCoord(_value);
		repositionChildren();
	}

	void ScrollViewResponsive::repositionChildren()
	{
		int childCount = getChildCount();
		IntCoord viewCoord = getViewCoord();
		int width = viewCoord.width - viewCoord.left;
		int currentY = 0;

		if (width < horizontalMaxSize) //Vertical Layout
		{
			for (int i = 0; i < childCount; ++i)
			{
				ContainerResponsive* child = getChildAt(i)->castType<ContainerResponsive>(false);
				if (child != nullptr)
				{
					child->setCoord(0, currentY, width, child->getHeight());
					currentY = child->getBottom() + padding.height;
				}
			}
		}
		else //Horizontal Layout
		{
			int rowStart = 0;
			int columnCount = 0;
			for (int i = 0; i < childCount; ++i)
			{
				ContainerResponsive* child = getChildAt(i)->castType<ContainerResponsive>(false);
				if (child != nullptr)
				{
					columnCount += child->getColumnCount();
					if (columnCount > rowColumns)
					{
						//Build previous row
						currentY = buildRow(rowStart, i, width, currentY) + padding.height;
						
						//Start defining new row
						rowStart = i;
						columnCount = child->getColumnCount();
					}
				}
			}
			//Build any remaining row elements
			currentY = buildRow(rowStart, childCount, width, currentY);
		}

		//Set final scroll view size
		setCanvasSize(width, currentY);
	}

	int ScrollViewResponsive::buildRow(int rowStart, int rowEnd, int width, int currentY)
	{
		int count = rowEnd - rowStart;
		int totalPadding = (count + 1) * padding.width;
		int paddedWidth = width - totalPadding;
		int lowestY = currentY;
		//Build previous row
		int previousWidgetRight = padding.width;
		for (int j = rowStart; j < rowEnd; j++)
		{
			ContainerResponsive* rowChild = getChildAt(j)->castType<ContainerResponsive>(false);
			if (rowChild != nullptr)
			{
				int itemWidth = (int)((float)rowChild->getColumnCount() / rowColumns * paddedWidth);
				rowChild->setCoord(previousWidgetRight, currentY, itemWidth, rowChild->getHeight());
				previousWidgetRight = rowChild->getRight() + padding.width;
				if (rowChild->getBottom() > lowestY)
				{
					lowestY = rowChild->getBottom();
				}
			}
		}
		return lowestY;
	}

} // namespace MyGUI
