# include "Common.hpp"

void DrawGradientText(const Font& font, const String& text, const Vec2& pos, const ColorF& topColor, const ColorF& bottomColor)
{
	const Vec2 basePos = pos;

	Vec2 penPos{ basePos };

	// 文字単位で描画を制御するためのループ
	for (const auto& glyph : font.getGlyphs(text))
	{
		// 改行文字なら
		if (glyph.codePoint == U'\n')
		{
			// ペンの X 座標をリセット
			penPos.x = basePos.x;

			// ペンの Y 座標をフォントの高さ分進める
			penPos.y += font.height();

			continue;
		}

		const Vec2 offset = glyph.getOffset();
		const double topPos = offset.y;
		const double bottomPos = (offset.y + glyph.texture.size.y);

		const double topT = (topPos / font.height());
		const double bottomT = (bottomPos / font.height());

		// グラデーションの色
		const ColorF c1 = topColor.lerp(bottomColor, topT);
		const ColorF c2 = topColor.lerp(bottomColor, bottomT);

		// 文字のテクスチャを描画
		glyph.texture
			.draw(penPos + offset, Arg::top = c1, Arg::bottom = c2);

		penPos.x += glyph.xAdvance;
	}
}

const Polygon getRectFrame(const RectF& rect, const double thickness)
{
	return Polygon{
		{ rect.tl(), rect.tr(), rect.br(), rect.bl() },
		{ {
			rect.bl().movedBy(thickness, -thickness),
			rect.br().movedBy(-thickness, -thickness),
			rect.tr().movedBy(-thickness, thickness),
			rect.tl().movedBy(thickness, thickness)
		 } }
	};
}

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
