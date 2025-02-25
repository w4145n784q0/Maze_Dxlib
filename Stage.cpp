#include "Stage.h"
#include "./globals.h"
#include <stack>
#include<queue>
#include<concurrent_priority_queue.h>

namespace {
	std::stack<Point> prStack;

	//ダイクストラ法のコスト保存用
	//distの要素数は[0]~[20](21個),それぞれが39個のINT_MAXを持つ
	//ダイクストラ法は始めに各値を無限大(INT_MAX)で初期化
	std::vector<std::vector<int>> dist(STAGE_HEIGHT, std::vector<int>(STAGE_WIDTH, INT_MAX));
	
	//経路保存用
	//prevの要素数21個,それぞれが39個の{-1,-1}を持つ
	std::vector<std::vector<Vec2>> prev(STAGE_HEIGHT, std::vector<Vec2>(STAGE_WIDTH, { -1, -1 })); 

	void DigDug(int x, int y, vector<vector<STAGE_OBJ>> &_stage)
	{
		_stage[y][x] = STAGE_OBJ::EMPTY;
		
		Point Dir[]{ {0,-1},{1, 0},{0, 1},{-1,0} };

		//壁（穴掘り予定）のリスト
		std::vector<int> dList;
		for (int i = 0; i < 4; i++)
		{
			//nextを[0]~[3]まで回す
			Point next = Point{ x + Dir[i].x, y + Dir[i].y };
			//nextのさらに先([0]~[3])を取得
			Point nextNext = { next.x + Dir[i].x, next.y + Dir[i].y };

			//場外ならやり直し
			if (nextNext.x < 0 || nextNext.y < 0 || nextNext.x > STAGE_WIDTH - 1 || nextNext.y > STAGE_HEIGHT - 1)
				continue;

			//壁になっている方向をリストに入れる
			if (_stage[nextNext.y][nextNext.x] == STAGE_OBJ::WALL)
			{
				dList.push_back(i);
			}
		}
		if (dList.empty())
		{
			return;
		}

		//リストの中からランダムに値を一つ取得、リストの要素番号とする
		int nrand = rand() % dList.size();

		//リストのnrand目を作業領域に保管、
		int tmp = dList[nrand];

		//ランダムな方向とさらに先を取得
		Point next = { x + Dir[tmp].x, y + Dir[tmp].y };
		Point nextNext = { next.x + Dir[tmp].x, next.y + Dir[tmp].y };

		//両方を空にする
		_stage[next.y][next.x] = STAGE_OBJ::EMPTY;
		_stage[nextNext.y][nextNext.x] = STAGE_OBJ::EMPTY;

		//スタックに保管
		prStack.push(nextNext);
		DigDug(nextNext.x, nextNext.y, _stage);
	}


	void AllWall(int w, int h, vector<vector<STAGE_OBJ>>& _stage)
	{
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++) {
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
					_stage[j][i] = STAGE_OBJ::EMPTY;
				else
					_stage[j][i] = STAGE_OBJ::WALL;
			}
		}
	}

	//穴掘り法
	void MakeMazeDigDug(int w, int h, vector<vector<STAGE_OBJ>>& _stage)
	{
		AllWall(w, h, _stage);
		Point sp{ 1, 1 };
		prStack.push(sp);
		while (!prStack.empty())
		{
			sp = prStack.top();
			prStack.pop();
			DigDug(sp.x, sp.y, _stage);
		}

		//外壁を埋める
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
					_stage[j][i] = STAGE_OBJ::WALL;
				continue;
			}
		}
	}

	//棒倒し
	void MakeMazePushDownBar(int w, int h, vector<vector<STAGE_OBJ>>& _stage)
	{
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				if (!(i == 0 || j == 0 || i == w - 1 || j == h - 1))
				{
					if (i % 2 == 0 && j % 2 == 0)
					{
						_stage[j][i] = STAGE_OBJ::WALL;
						//棒を立てたらすかさず倒す
						int rnd = rand() % 4;//0:↑　1:→　2:↓　3:←
						switch (rnd)
						{
						case 0:
							_stage[j - 1][i] = STAGE_OBJ::WALL;
							break;
						case 1:
							_stage[j][i + 1] = STAGE_OBJ::WALL;
							break;
						case 2:
							_stage[j + 1][i] = STAGE_OBJ::WALL;
							break;
						case 3:
							_stage[j][i - 1] = STAGE_OBJ::WALL;
							break;
						default:
							break;
						}
					}
				}
			}
		}

		//外壁を埋める
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
					_stage[j][i] = STAGE_OBJ::WALL;
				continue;
			}
		}
	}

}




Stage::Stage()
{
	stageData = vector(STAGE_HEIGHT, vector<STAGE_OBJ>(STAGE_WIDTH, STAGE_OBJ::EMPTY));

	MakeMazeDigDug(STAGE_WIDTH, STAGE_HEIGHT, stageData);
	//MakeMazePushDownBar(STAGE_WIDTH, STAGE_HEIGHT, stageData);

	//グリッド
	/*for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (y == 0 || y == STAGE_HEIGHT - 1 || x == 0 || x == STAGE_WIDTH - 1)
			{
				stageData[y][x] = STAGE_OBJ::WALL;
			}
			else
			{
				if (x % 2 == 0 && y % 2 == 0)
					stageData[y][x] = STAGE_OBJ::WALL;
				else
					stageData[y][x] = STAGE_OBJ::EMPTY;
			}

		}
	}*/
	setStageRects();
}

Stage::~Stage()
{
}

void Stage::Update()
{
}

void Stage::Draw()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			switch (stageData[y][x])
			{
			case STAGE_OBJ::EMPTY:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(102, 205, 170), TRUE);
				break;
			case STAGE_OBJ::WALL:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(119, 136, 153), TRUE);
				break;
			case STAGE_OBJ::GOAL:
			
				break;
			default:
				break;
			}
		}
	}
}

void Stage::setStageRects()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (stageData[y][x] == STAGE_OBJ::WALL)
			{
				stageRects.push_back(Rect(x * CHA_WIDTH, y * CHA_HEIGHT,  CHA_WIDTH, CHA_HEIGHT));
			}
		}
	}

}

void Stage::Dijkstra(pair<int, int> sp)
{
	dist[sp.second][sp.first] = 0;//スタートのy,x
	std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>> pq;
	pq.push(Mdat(0, { sp.first, sp.second }));

	while (!pq.empty())
	{
		Mdat p = pq.top();
		pq.pop();

		//Rect{ (int)p.second.first * CHA_WIDTH, (int)p.second.second * CHA_HEIGHT, BLOCK_SIZE }.draw(Palette::Red);
		int c = p.first;
		Vec2Int v = p.second;

		for (int i = 0; i < 4; i++)
		{
			//方向を一個ずつ確認
			Vec2Int np = { v.first + (int)dirs[i].x, v.second + (int)dirs[i].y };

			//0かグリッドを超える探索はしない
			if (np.first < 0 || np.second < 0 || np.first >= MazeSize.x || np.second >= MazeSize.y) continue;

			//壁なら探索しない
			if (MazeData[np.second][np.first].type == WALL) continue;

			//
			if (dist[np.second][np.first] <= MazeData[np.second][np.first].weight + c) continue;

			dist[np.second][np.first] = MazeData[np.second][np.first].weight + c;
			pre[np.second][np.first] = Vec2(v.first, v.second);
			pq.push(Mdat(dist[np.second][np.first], np));
		}
	}

}
