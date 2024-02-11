# pragma once
# include "Common.hpp"
# include "BallWithMultipleType.hpp"
# include "BlockWithMultipleType.hpp"

class MultipleTypeWar : public App::Scene
{
public:
	MultipleTypeWar(const InitData& init);

	void update() override;
	void draw() const override;
private:
	void addBlockCounts(const double time);
	void deleteBlockCounts(const double time);
	void updateBlockCountsHistory();
	void updateBlockCountsRanking();
	void updateBlock(BlockWithMultipleType& block, const BallWithMultipleType& ball, double time);
	void checkCollisionAndBounce(BallWithMultipleType& ball);
	void adjustBallCounts();
	void drawBlocks() const;
	void drawBalls() const;
	void drawRankingBest() const;
	void drawRankingWorst() const;
	void drawGraph() const;

	const Vec2 BlockSize{ 10, 10 };
	// const Size GridSize{ 72, 72 };
	const Size GridSize{ 144, 144 };
	const Size FieldSize = GridSize * BlockSize.asPoint();
	const size_t typeCount{ static_cast<size_t>(Type::Count) };
	const size_t typeCombinationCount = typeCount * (typeCount - 1) / 2;

	const Size gridSectionSize{ GridSize.x / typeCount, GridSize.y / typeCount };

	const double WallThickness = 20;
	const Polygon Wall = getRectFrame(RectF{ FieldSize }.stretched(WallThickness), WallThickness);

	HashTable<Types, size_t> ballCounts;
	HashTable<Types, size_t> blockCounts;
	Array<std::pair<double, HashTable<Types, size_t>>> blockCountsHistory;
	Array<std::pair<Types, int32>> blockCountsRanking;

	HashTable<Types, Array<BallWithMultipleType>> ballsByType;
	Grid<BlockWithMultipleType> blocks{ GridSize };

	const double scaleField = 0.6;
	const double scaleRankingBest = 0.9;
	const double scaleRankingWorst = 0.9;

	// 描画位置と大きさを設定
	Mat3x2 matTitle = Mat3x2::Translate(950, 0);
	Mat3x2 matField = Mat3x2::Scale(scaleField, scaleField).translated(WallThickness, WallThickness);
	Mat3x2 matForPlot = Mat3x2::Scale(0.35, 0.35).translated(950, 200);
	Mat3x2 matRankingBest = Mat3x2::Scale(scaleRankingBest, scaleRankingBest).translated(1600, 0);
	Mat3x2 matRankingWorst = Mat3x2::Scale(scaleRankingWorst, scaleRankingWorst).translated(1600, 520);
};




