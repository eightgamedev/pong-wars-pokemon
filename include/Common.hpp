# pragma once

# include <Siv3D.hpp>
# include "Type.hpp"
# include "Ball.hpp"
# include "Block.hpp"

void DrawGradientText(const Font& font, const String& text, const Vec2& pos, const ColorF& topColor, const ColorF& bottomColor);
const Polygon getRectFrame(const RectF& rect, const double thickness);
void updateBlockCountHistory(const Type type, const double time, const int32 countChange, Array<Array<std::pair<double, size_t>>>& blockCountsHistory);
void updateBlock(Block& block, const Ball& ball, const double time, const int32 change, HashTable<Type, size_t>& blockCounts, Array<Array<std::pair<double, size_t>>>& blockCountsHistory);
void checkCollisionAndBounce(Ball& ball, Grid<Block>& blocks, HashTable<Type, size_t>& blockCounts, Array<Array<std::pair<double, size_t>>>& blockCountsHistory);
void adjustBallCounts(Array<Ball>& balls, const Grid<Block>& blocks, HashTable<Type, size_t>& ballCounts, const HashTable<Type, size_t>& blockCounts);
void drawGraph(const Array<Array<std::pair<double, size_t>>>& blockCountsHistory);

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
