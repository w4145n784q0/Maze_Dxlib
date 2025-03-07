#pragma once


	struct Point
	{
		int x, y;
	};

	struct Pointf
	{
		float x, y;
	};

	struct Rect
	{
		int x, y, w, h;
		Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
		Point GetCenter() { return Point{ x + w / 2, y + h / 2 }; }
	};


	enum class STAGE_OBJ {
		EMPTY,
		WALL,
		GOAL,
		MAX_OBJECT
	};


	enum DIR
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		NONE,
		MAXDIR
	};

	struct Vec2
	{
		double x;
		double y;
	};
	
	const int CHA_WIDTH = 32;
	const int CHA_HEIGHT = 32;

	inline Point operator +(const Point& p1, const Point& p2) {
		return Point{ p1.x + p2.x, p1.y + p2.y };
	}

	inline Point operator -(const Point& p1, const Point& p2) {
		return Point{ p1.x - p2.x, p1.y - p2.y };
	}

	inline bool operator == (const Point& p1, const Point& p2) {
		if (p1.x == p2.x && p1.y == p2.y)
			return true;
		else
			return false;
	}

	inline bool operator != (const Point& p1, const Point& p2) {
		if (p1.x != p2.x || p1.y != p2.y)
			return true;
		else
			return false;
	}

	inline bool operator == (const Vec2& p1, const Vec2& p2) {
		if (p1.x == p2.x && p1.y == p2.y)
			return true;
		else
			return false;
	}

	inline bool operator != (const Vec2& p1, const Vec2& p2) {
		if (p1.x != p2.x || p1.y != p2.y)
			return true;
		else
			return false;
	}