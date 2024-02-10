# pragma once

# include <Siv3D.hpp>
# include "Type.hpp"
# include "Ball.hpp"
# include "Block.hpp"

void DrawGradientText(const Font& font, const String& text, const Vec2& pos, const ColorF& topColor, const ColorF& bottomColor);
const Polygon getRectFrame(const RectF& rect, const double thickness);


enum class State
{
	Setting,
	SingleTypeWar,
	MultipleTypeWar,
};

struct Common
{
};

using App = SceneManager<State, Common>;
