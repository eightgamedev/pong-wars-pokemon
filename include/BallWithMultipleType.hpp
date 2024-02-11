# pragma once
# include "Type.hpp"

class BallWithMultipleType
{
public:
	BallWithMultipleType(Types types, const Vec2& pos)
		: m_types(types)
		, m_pos(pos)
		, m_circle(400, 400, 2)
	{

		m_circleColors.push_back(typeColors.at(m_types.type1));
		m_circleColors.push_back(typeColors.at(m_types.type2));

		// 右上、右下、左上、左下の4方向からランダムに決定
		const Array<Vec2> directions = { Vec2(1, -1), Vec2(1, 1), Vec2(-1, -1), Vec2(-1, 1) };
		m_ballVelocity = directions[Random(3)] * 100;
	}

	void update(const Size& FieldSize)
	{
		m_pos.moveBy(m_ballVelocity * Scene::DeltaTime());
		m_circle.setPos(m_pos);
		updatePosition(FieldSize);
		//bounceWall(Wall);
	}

	void draw() const
	{
		Color innerColor = m_circleColors[0];
		Color outerColor = m_circleColors[1];
		m_circle.draw(outerColor).drawFrame(1, Palette::Black).stretched(- m_circle.r / 2).draw(innerColor);
	}

	Circle getCircle() const
	{
		return m_circle;
	}

	Types getTypes() const
	{
		return m_types;
	}

	Vec2 getPos() const
	{
		return m_pos;
	}

	bool hasSingleType() const
	{
		return m_types.type1 == m_types.type2;
	}

	void bounce(RectF rect)
	{
		if (rect.right().intersects(m_circle)
		 || rect.left().intersects(m_circle))
		{
			bounceX();
		}

		if (rect.bottom().intersects(m_circle)
		 || rect.top().intersects(m_circle))
		{
			bounceY();
		}
	}

private:
	void updatePosition(const Vec2& FieldSize)
	{
		m_pos += m_ballVelocity * Scene::DeltaTime();
		m_circle.setPos(m_pos);

		if (m_pos.y < 0)
		{
			m_pos.y += FieldSize.y;
		}
		m_pos.y = Math::Fmod(m_pos.y, FieldSize.y);

		if (m_pos.x < 0)
		{
			m_pos.x += FieldSize.x;
		}
		m_pos.x = Math::Fmod(m_pos.x, FieldSize.x);
	}

	void bounceWall(Polygon wall)
	{
		if (wall.intersects(m_circle))
		{
			bounceX();
			bounceY();
		}
	}

	void bounceX()
	{
		m_ballVelocity.x *= -1;
	}

	void bounceY()
	{
		m_ballVelocity.y *= -1;
	}

	Array<Color> m_circleColors;
	Types m_types;
	Vec2 m_pos;
	Circle m_circle;
	Vec2 m_ballVelocity;
};
