# pragma once
# include "Common.hpp"

class SingleTypeWar : public App::Scene
{
public:
	SingleTypeWar(const InitData& init);

	void update() override;
	void draw() const override;
private:
	void addBlockCounts(const double time);
	void deleteBlockCounts(const double time);
	void updateBlockCountsHistory();
	void updateBlockCountsRanking();
	void updateBlock(Block& block, const Ball& ball, const double time, const int32 change);
	void checkCollisionAndBounce(Ball& ball);
	void adjustBallCounts();
	void drawBlocks() const;
	void drawBalls() const;
	void drawRankingBest() const;
	void drawGraph() const;

	const Vec2 BlockSize{ 15, 15 };
	const Size GridSize{ 60, 60 };
	const Size FieldSize = GridSize * BlockSize.asPoint();
	const Size gridSectionSize{ GridSize.x / 3, GridSize.y / 6 };

	const double WallThickness = 20;
	const Polygon Wall = getRectFrame(RectF{ FieldSize }.stretched(WallThickness), WallThickness);

	HashTable<Type, size_t> ballCounts;
	HashTable<Type, size_t> blockCounts;
	Array<std::pair<double, HashTable<Type, size_t>>> blockCountsHistory;
	Array<std::pair<Type, int32>> blockCountsRanking;

	const size_t typeCount{ static_cast<size_t>(Type::Count) };
	HashTable<Type, Array<Ball>> ballsByType;
	Grid<Block> blocks{ GridSize };

	// 描画位置と大きさを設定
	Mat3x2 matTitle = Mat3x2::Translate(1000, 0);
	Mat3x2 matField = Mat3x2::Translate(WallThickness, WallThickness);
	Mat3x2 matPlot = Mat3x2::Scale(0.4, 0.4).translated(1000, 200);
	Mat3x2 matRanking = Mat3x2::Scale(0.8, 0.8).translated(1700, 200);
};




