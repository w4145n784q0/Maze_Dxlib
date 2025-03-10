#include "Stage.h"
#include "./globals.h"
#include <stack>
#include<queue>
#include<concurrent_priority_queue.h>
#include<string>
#include "ImGui/imgui.h"

using std::vector;

namespace {
	std::stack<Point> prStack;


	//二次元配列　MazeData[MazeSize.y][MazeSize.x]{FLOORで初期化｝
	std::vector<std::vector<floorData>> MazeDataDijkstra(STAGE_HEIGHT, std::vector<floorData>(STAGE_WIDTH, { STAGE_OBJ::EMPTY, 0 })); //迷路そのもの

	//ダイクストラ法のコスト(到着までの移動量)保存用
	//distの要素数は[0]~[20](21個),それぞれが39個のINT_MAXを持つ
	//ダイクストラ法は始めに各値を無限大(INT_MAX)で初期化
	std::vector<std::vector<int>> dist(STAGE_HEIGHT, std::vector<int>(STAGE_WIDTH, INT_MAX));

	//経路保存用
	//prevの要素数21個,それぞれが39個の{-1,-1}を持つ
	std::vector<std::vector<Vec2>> prev(STAGE_HEIGHT, std::vector<Vec2>(STAGE_WIDTH, { -1, -1 }));

	void DigDug(int x, int y, vector<vector<STAGE_OBJ>>& _stage)
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

		//外壁を埋める+重み付け
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
				{
					_stage[j][i] = STAGE_OBJ::WALL;
				}
				else
				{
					//MazeDataDijkstra[j][i].weight = (rand() % 5) + 1;
					MazeDataDijkstra[j][i].weight = 1;
				}

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
	//for (int y = 0; y < STAGE_HEIGHT; y++)
	//{
	//	for (int x = 0; x < STAGE_WIDTH; x++)
	//	{
	//		if (y == 0 || y == STAGE_HEIGHT - 1 || x == 0 || x == STAGE_WIDTH - 1)
	//		{
	//			stageData[y][x] = STAGE_OBJ::WALL;
	//		}
	//		else
	//		{
	//			if (x % 2 == 0 && y % 2 == 0)
	//			{
	//				stageData[y][x] = STAGE_OBJ::WALL;
	//			}
	//			else
	//			{
	//				stageData[y][x] = STAGE_OBJ::EMPTY;
	//				MazeDataDijkstra[y][x].weight = (rand() % 5) + 1;
	//				//MazeDataDijkstra[y][x].weight = 1;
	//			}
	//		}

	//	}
	//}
	setStageRects();

	//Dijkstra({ 1,1 });

	//route = restore(GOAL_POINT.x, GOAL_POINT.y );//経路を逆追跡

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
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(255, 255, 255), TRUE);
				break;
			default:
				break;
			}

			//switch (MazeDataDijkstra[y][x].weight)
			//{
			//case 1://デフォルト
			//	DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(102, 205, 170), TRUE);
			//	break;
			//case 2://lightgreen
			//	DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(144, 238, 144), TRUE);
			//	break;
			//case 3://lime
			//	DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(0, 255, 0), TRUE);
			//	break;
			//case 4://darkolivegreen
			//	DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(85, 107, 47), TRUE);
			//	break;
			//case 5://purple
			//	DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(128, 0, 128), TRUE);
			//	break;
			//default:

			//	break;
			//}


			//if (dist[y][x] < INT_MAX)
			//{
				//std::string s = std::to_string(dist[y][x]);
				//const char* c = s.c_str();
				//ゴールまでのコストを表示
				//DrawFormatString(x * CHA_WIDTH, y * CHA_HEIGHT,GetColor(0,0,255), "%s", c);
			//}
		}
	}

	/*for (auto itr : route)
	{
		DrawBox(itr.x * CHA_WIDTH, itr.y * CHA_HEIGHT, itr.x * CHA_WIDTH + CHA_WIDTH, itr.y * CHA_HEIGHT + CHA_HEIGHT, GetColor(255, 255, 0), FALSE);
	}*/

	//ImGui::Begin("config 1");
	//ImGui::Text("x: %.1d", sx);
	//ImGui::Text("y: %.1d", sy);

	//ImGui::Text("nextx: %.1d", sx2);
	//ImGui::Text("nexty: %.1d", sy2);
	//ImGui::End();
}

void Stage::setStageRects()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (stageData[y][x] == STAGE_OBJ::WALL)
			{
				stageRects.push_back(Rect(x * CHA_WIDTH, y * CHA_HEIGHT, CHA_WIDTH, CHA_HEIGHT));
			}
		}
	}
}


void Stage::Dijkstra(pair<int, int> sp)
{
	//dist[1(y)][1(x)]をコストを0で初期化
	dist[sp.second][sp.first] = 0;

	//Mdat型、Mdat型コンテナ、昇順
	std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>> pq;

	//コスト0,座標1,1で初期化
	pq.push(Mdat(0, { sp.first, sp.second }));

	while (!pq.empty())
	{
		Mdat p = pq.top();
		pq.pop();

		//Rect{ (int)p.second.first * CHA_WIDTH, (int)p.second.second * CHA_HEIGHT, BLOCK_SIZE }.draw(Palette::Red);

		//最短距離を保管
		int cost = p.first;

		//現在値を保管
		Vec2Int v = p.second;

		for (int i = 0; i < 4; i++)
		{
			//探索方向　dirs[0]~[3]から一個ずつ確認
			//first : 探索先のx方向
			//second : 探索先のy方向
			Vec2Int np = { v.first + (int)dirs[i].x, v.second + (int)dirs[i].y };

			//0かグリッドを超える探索はしない
			if (np.first < 0 || np.second < 0 || np.first >= STAGE_WIDTH || np.second >= STAGE_HEIGHT) continue;

			//壁なら探索しない
			if (stageData[np.second][np.first] == STAGE_OBJ::WALL) continue;

			//探索する方向よりコストが大きいならスルー
			if (dist[np.second][np.first] <= MazeDataDijkstra[np.second][np.first].weight + cost) continue;

			//最短距離を更新
			dist[np.second][np.first] = MazeDataDijkstra[np.second][np.first].weight + cost;

			//辿った経路を保存
			prev[np.second][np.first] = Vec2{ (double)v.first, (double)v.second };

			//今のコストと探索方向の座標を保存
			pq.push(Mdat(dist[np.second][np.first], np));
		}
	}

}

std::vector<Vec2> Stage::restore(int _x, int _y)
{
	std::vector<Vec2> path;
	int x = _x, y = _y;
	//_x,_yが-1にならない限り継続 ループ終了後_x = prev[y][x].x , _y = prev[y][x].yを代入
	for (; _x != -1 || _y != -1; _x = prev[y][x].x, _y = prev[y][x].y) {

		//pathに追跡前の位置を保管
		path.push_back(Vec2{ (double)_x, (double)_y });

		//x,yの更新
		x = (int)_x, y = (int)_y;
	}
	reverse(path.begin(), path.end());
	return path;
}

DIR Stage::DijkstraRoute(pair<int, int> sp, int endX, int endY)
{
	Dijkstra(sp);
	std::vector path = restore(endX, endY);

	if (path.empty())
		return DIR::NONE;

	int index = -1;
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i].x == sp.first && path[i].y == sp.second)
		{
			index = i;
			break;
		}
	}
	if (index >= path.size() - 1)
		return DIR::NONE;

	int findX = path[index + 1].x - path[index].x;
	int findY = path[index + 1].y - path[index].y;
	Vec2 FindPos = { findX,findY };

	sx = FindPos.x;
	sy = FindPos.y;
	//sx2 = path[index + 1].x;
	//sy2 = path[index + 1].y;

	DIR dirs[] = { UP, DOWN, LEFT, RIGHT };
	Point Xdir[] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
	for (int i = 0; i < 4; i++)
	{
		if ((int)FindPos.x == Xdir[i].x && (int)FindPos.y == Xdir[i].y)
		{
			return dirs[i];
		}
	}

	return DIR::NONE;
}

DIR Stage::DijkstraQueue(pair<int, int> sp, int endX, int endY)
{
	Dijkstra(sp);
	std::vector path = restore(endX, endY);

	Vec2 start = { sp.first,  sp.second };
	Vec2 current = { endX,endY };
	while (prev[current.y][current.x] != start) {
		current = prev[current.y][current.x];
	}

	if (current.x > sp.first) {
		return RIGHT;
	}
	else if (current.x < sp.first) {
		return LEFT;
	}
	else if (current.y > sp.second) {
		return DOWN;
	}
	else if (current.y < sp.second) {
		return UP;
	}
	return NONE;
}

std::vector<std::vector<int>> Stage::GetDist()
{
	return dist;
}

std::vector<std::vector<floorData>> Stage::GetMazeDataDijkstra()
{
	return MazeDataDijkstra;
}
