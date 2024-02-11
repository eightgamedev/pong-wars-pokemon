# include "Common.hpp"

void DrawGradientText(const Font& font, const String& text, const Vec2& pos, const ColorF & topColor, const ColorF& bottomColor)
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


