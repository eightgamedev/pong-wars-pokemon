#pragma once

namespace CustomizedSimpleGUI
{

	constexpr int32 UnitSize = 36;
	constexpr ColorF BackgroundColor{ 1.0 };
	constexpr ColorF DisabledBackgroundColor{ 0.92 };
	constexpr ColorF ButtonMouseOverColor{ 0.9, 0.95, 1.0 };
	constexpr ColorF ActiveTextColor{ 0.11 };
	constexpr ColorF DisabledTextColor{ 0.67 };
	constexpr int32 FontYOffset = -1;
	constexpr double RoundSize = 4.8;
	constexpr ColorF FrameColor{ 0.67 };

	constexpr ColorF ListBoxSelectedColor{ 0.2, 0.4, 0.8 };
	constexpr int32 ScrollBarWidth = 18;
	constexpr ColorF ListBoxSelectedDisabledColor{ 0.75, 0.85, 1.0 };
	constexpr int32 ListBoxFrameThickness = 1;
	constexpr int32 FrameThickness = 1;
	constexpr int32 DisplayRegionFrameThickness = 2;

	[[nodiscard]]
	inline constexpr ColorF GetTextColor(bool enabled) noexcept
	{
		return (enabled ? ActiveTextColor : DisabledTextColor);
	}

}
