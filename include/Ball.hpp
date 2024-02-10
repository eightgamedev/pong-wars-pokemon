# pragma once
# include "Type.hpp"

class Ball
{
public:
	Ball(Type type, const Vec2& pos)
		: m_type(type)
		, m_pos(pos)
		, m_circle(400, 400, 2)
		, m_circleColor(typeColors.at(type))
	{
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
		m_circle.draw(m_circleColor).drawFrame(2, Palette::Black);
	}

	Circle getCircle() const
	{
		return m_circle;
	}

	Type getType() const
	{
		return m_type;
	}

	Vec2 getPos() const
	{
		return m_pos;
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

	Color m_circleColor;
	Type m_type;
	Vec2 m_pos;
	Circle m_circle;
	Vec2 m_ballVelocity;
};
