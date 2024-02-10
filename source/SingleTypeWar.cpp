# include "SingleTypeWar.hpp"

void updateBlockCountHistory(const Type type, const double time, const int32 countChange, Array<Array<std::pair<double, size_t>>>& blockCountsHistory)
{
	// 60秒経過後、過去60秒より古いデータを削除
	while (!blockCountsHistory[static_cast<size_t>(type)].isEmpty() && blockCountsHistory[static_cast<size_t>(type)].front().first < time - 60) {
		blockCountsHistory[static_cast<size_t>(type)].pop_front();
	}

	int32 newCount = (blockCountsHistory[static_cast<size_t>(type)].isEmpty() ? 0 : blockCountsHistory[static_cast<size_t>(type)].back().second) + countChange;
	blockCountsHistory[static_cast<size_t>(type)].push_back({ time, newCount });
}

// ブロックのタイプを更新
void updateBlock(Block& block, const Ball& ball, const double time, const int32 change, HashTable<Type, size_t>& blockCounts, Array<Array<std::pair<double, size_t>>>& blockCountsHistory)
{
	blockCounts.at(block.getType())--;
	updateBlockCountHistory(block.getType(), time, -change, blockCountsHistory);
	block.setType(ball.getType());
	blockCounts.at(block.getType())++;
	updateBlockCountHistory(block.getType(), time, change, blockCountsHistory);
}

// ボールとブロックの衝突判定と反射
void checkCollisionAndBounce(Ball& ball, Grid<Block>& blocks, HashTable<Type, size_t>& blockCounts, Array<Array<std::pair<double, size_t>>>& blockCountsHistory)
{
	for (auto& block : blocks) {
		if (!ball.getCircle().intersects(block.getRect())) {
			continue;
		}

		if (ball.getType() == block.getType()) {
			continue;
		}

		double affinity = TypeAffinityTable.at({ ball.getType(), block.getType() });

		if (affinity == 2.0) {
			updateBlock(block, ball, Scene::Time(), 1, blockCounts, blockCountsHistory);
		}
		else if (affinity == 1.0 && RandomBool(0.25)) {
			updateBlock(block, ball, Scene::Time(), 1, blockCounts, blockCountsHistory);
		}
		else if (affinity == 0.5 && RandomBool(0.125)) {
			updateBlock(block, ball, Scene::Time(), 1, blockCounts, blockCountsHistory);
		}
		else if (affinity == 0.0) {
			updateBlockCountHistory(block.getType(), Scene::Time(), 0, blockCountsHistory);
		}

		ball.bounce(block.getRect());
	}
}

void adjustBallCounts(Array<Ball>& balls, const Grid<Block>& blocks, HashTable<Type, size_t>& ballCounts, const HashTable<Type, size_t>& blockCounts)
{
	// 各タイプのボールの数を調整
	for (const auto& pair : blockCounts)
	{
		Type type = pair.first;
		int32 blockCount = pair.second;
		// 調整後のボールの数
		int32 desiredBallCount = Max(blockCount / 100, 1);

		// ボールが足りない場合は同じタイプのブロックからボールを追加
		while (ballCounts[type] < desiredBallCount)
		{
			bool blockExists = false;
			for (const auto& block : blocks)
			{
				if (block.getType() == type)
				{
					balls.push_back(Ball(type, block.getRect().center()));
					ballCounts[type]++;
					blockExists = true;
					break;
				}
			}
			if (not blockExists)
			{
				break;
			}
		}

		// ボールが多すぎる場合は削除
		while (ballCounts[type] > desiredBallCount)
		{
			// このタイプのボールを見つけて削除
			for (auto it = balls.begin(); it != balls.end(); ++it)
			{
				if (it->getType() == type)
				{
					balls.erase(it);
					ballCounts[type]--;
					break;
				}
			}
		}
	}
}

void drawGraph(const Array<Array<std::pair<double, size_t>>>& blockCountsHistory)
{
	size_t maxBlockCount = 0;
	for (const auto& history : blockCountsHistory) {
		for (const auto& pair : history) {
			if (pair.second > maxBlockCount) {
				maxBlockCount = pair.second;
			}
		}
	}

	// 現在の時間を取得
	double currentTime = Scene::Time();

	// 過去60秒以降の最小の時間を計算
	double minTime = std::max(0.0, currentTime - 60);

	// 時間の最大値
	double maxTime = 60;

	// グラフのサイズとスケールを計算
	const double graphHeight = 1000.0;
	const double graphWidth = 1700.0;
	const double timeScale = graphWidth / maxTime;
	const double yScale = graphHeight / maxBlockCount; // maxBlockCountはブロックの最大数

	// グリッドの縦線を描画
	for (int32 i = 0; i <= maxTime; i += 10) {
		const double x = i * timeScale;
		Line(x, 0, x, graphHeight).draw(1, Palette::Gray);
		const String label = Format(i);
		FontAsset(U"Label")(label).draw(Arg::topCenter = Vec2(x, graphHeight + 10), Palette::White);
	}

	// x軸を描画
	Line(0, graphHeight, graphWidth, graphHeight).draw(1, Palette::White);

	// x軸のラベルを描画
	FontAsset(U"Label")(U"Time (s)").draw(Arg::topCenter = Vec2(graphWidth / 2, graphHeight + 40), Palette::White);

	// y軸を描画
	Line(0, 0, 0, graphHeight).draw(1, Palette::White);

	// y軸のラベルとグリッドの横線を描画
	for (int32 i = 0; i <= maxBlockCount; i += 100) { // maxBlockCountはブロックの最大数
		const double y = graphHeight - i * yScale;
		const String label = Format(i);
		FontAsset(U"Label")(label).draw(Arg::rightCenter = Vec2(-40, y), Palette::White);
		Line(-10, y, 0, y).draw(1, Palette::White);
		Line(0, y, graphWidth, y).draw(0.5, Palette::Gray);
	}

	// グラフを描画
	for (size_t i = 0; i < blockCountsHistory.size(); ++i) {
		const Color color = typeColors.at(static_cast<Type>(i));
		for (size_t j = 1; j < blockCountsHistory[i].size(); ++j) {
			if (blockCountsHistory[i][j - 1].first >= minTime && blockCountsHistory[i][j].first >= minTime) {
				const double x1 = (blockCountsHistory[i][j - 1].first - minTime) * timeScale;
				const double y1 = graphHeight - blockCountsHistory[i][j - 1].second * yScale;
				const double x2 = (blockCountsHistory[i][j].first - minTime) * timeScale;
				const double y2 = graphHeight - blockCountsHistory[i][j].second * yScale;
				Line(x1, y1, x2, y2).draw(5, color);
			}
		}
	}
}

SingleTypeWar::SingleTypeWar(const InitData& init)
	: IScene(init)
{

	for (size_t type = 0; type < typeCount; ++type)
	{
		ballCounts[static_cast<Type>(type)] = 0;
		blockCounts[static_cast<Type>(type)] = 0;
	}

	for (size_t y = 0; y < GridSize.y; ++y)
	{
		for (size_t x = 0; x < GridSize.x; ++x)
		{
			const Point SectionIndex = Point{ x, y } / gridSectionSize;
			Type type = static_cast<Type>((SectionIndex.y * 3 + SectionIndex.x) % typeCount);
			blocks[y][x] = Block{ Vec2(x, y) * BlockSize, type, BlockSize };
			blockCounts[type]++;
		}
	}

	for (size_t i = 0; i < typeCount; ++i)
	{
		blockCountsHistory[i].push_back({ 0, blockCounts[static_cast<Type>(i)] });
	}

	const Vec2 blockSectionSize = { (BlockSize * GridSize / 3).x, (BlockSize * GridSize / 6).y };

	for (size_t i = 0; i < typeCount; ++i)
	{
		// セクションのインデックスを計算
		int32 sectionIndexX = i % 3;
		int32 sectionIndexY = i / 3;

		// セクションの中心を計算
		Vec2 sectionCenter = Vec2(sectionIndexX, sectionIndexY) * blockSectionSize + blockSectionSize / 2;

		// ボールを作成し、初期位置をセクションの中心に設定
		balls.emplace_back(static_cast<Type>(i), sectionCenter);
		ballCounts[static_cast<Type>(i)]++;
	}
}

void SingleTypeWar::update()
{
	for (auto& ball : balls)
	{
		ball.update(FieldSize);
		checkCollisionAndBounce(ball, blocks, blockCounts, blockCountsHistory);
	}

	adjustBallCounts(balls, blocks, ballCounts, blockCounts);
}

void SingleTypeWar::draw() const
{
	{
		const Transformer2D transformer{ matTitle, TransformCursor::Yes };

		FontAsset(U"Title")(U"Pong Wars Pokemon").draw(0, 0, Palette::White);
		FontAsset(U"Title")(U"@ito_eight").draw(0, 60, Palette::White);
	}

	{
		const Transformer2D transformer{ matField, TransformCursor::Yes };

		for (auto& block : blocks)
		{
			block.draw();
		}

		for (auto& ball : balls)
		{
			ball.draw();
		}

		Wall.draw(Palette::White);
	}

	{
		const Transformer2D transformerForRanking{ matForRanking, TransformCursor::Yes };

		Array<std::pair<Type, int32>> blockCountsRanking(blockCounts.begin(), blockCounts.end());
	
		// ブロック数が大きい順に並べ替え
		std::sort(blockCountsRanking.begin(), blockCountsRanking.end(),
			[](const std::pair<Type, int32>& a, const std::pair<Type, int32>& b) {
					return a.second > b.second;
			});
		// ランキングを描画
		for (size_t i = 0; i < blockCountsRanking.size(); ++i)
		{
			const Vec2 textPos = { 0, 25 * i };
			const Type type = blockCountsRanking[i].first;
			const String text = typeName.at(type) + U": ";
			FontAsset(U"Ranking")(text).draw(textPos, typeColors.at(type));
			FontAsset(U"Ranking")(blockCountsRanking[i].second).draw(Arg::topRight = textPos.movedBy(150, 0), typeColors.at(type));
		}
	}

	{
		const Transformer2D transformerForPlot{ matForPlot, TransformCursor::Yes };
		drawGraph(blockCountsHistory);
	}
}
