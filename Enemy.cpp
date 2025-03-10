#include "Enemy.h"
#include "./Stage.h"
#include "globals.h"
#include "Player.h"
#include <map>
#include "./Source/Input.h"
#include "ImGui/imgui.h"
#include <algorithm> 
#include<queue>

using std::queue;

namespace
{
	//四方向(↑,↓,←,→)
	Point nDir[4] = { {0,-1},{0,1},{-1,0},{1,0} };

	//ランダム移動の切り替え時間
	const int ChangeTimer = 600;

	std::vector<std::vector<floorData>> MazeCost;

	DIR dirlist[] = { UP,DOWN,LEFT,RIGHT };

	std::vector<std::vector<int>> Edist = vector(STAGE_HEIGHT, vector<int>(STAGE_WIDTH, INT_MAX));
	std::vector<std::vector<Vec2>> Eprev = vector(STAGE_HEIGHT, vector<Vec2>(STAGE_WIDTH, { -1, -1 }));
}

Enemy::Enemy()
	:pos_({ 0,0 }), isAlive_(true), nextPos_({ 0,0 }), RandTimer_(0), ChoiceMove(XYCLOSEMOVE)
{
	int rx = 0;
	int ry = 0;
	while (rx % 2 == 0 || ry % 2 == 0)
	{
		rx = GetRand(STAGE_WIDTH - 1);
		ry = GetRand(STAGE_HEIGHT - 1);
	}

	pos_ = { rx * CHA_WIDTH, ry * CHA_HEIGHT };
	forward_ = NONE;

	Stage* stage = (Stage*)FindGameObject<Stage>();
	Player* player = (Player*)FindGameObject<Player>();

	//初期位置が壁なら戻す
	Rect eRect = { pos_.x, pos_.y,CHA_WIDTH, CHA_HEIGHT };//敵の位置（Rect型）
	for (auto& obj : stage->GetStageRects())
	{
		Point op = pos_;
		if (CheckHit(eRect, obj)) {
			Rect tmpRectX = { op.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
			Rect tmpRecty = { pos_.x, op.y, CHA_WIDTH, CHA_HEIGHT };
			if (!CheckHit(tmpRectX, obj))
			{//x座標を元に戻したらぶつかっていない→x座標だけ元に戻す
				pos_.x = op.x;
			}
			else if (!CheckHit(tmpRecty, obj))
			{//y座標を元に戻したらぶつかっていない→y座標だけ元に戻す
				pos_.y = op.y;
			}
			else
			{//片方だけ戻してもやっぱりぶつかっている→両方元に戻す
				pos_ = op;
			}
		}
	}

	EnemyGridPos = { pos_.x / 32, pos_.y / 32 };
	Point tmp = player->GetGridPos();
	stage->Dijkstra({ EnemyGridPos.x, EnemyGridPos.y });
	tmpRoute = stage->restore(tmp.x, tmp.y);//ルートを保存
	stageDist = stage->GetDist();
	MazeCost = stage->GetMazeDataDijkstra();
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	Stage* stage = (Stage*)FindGameObject<Stage>();
	Player* player = (Player*)FindGameObject<Player>();
	static bool stop = false;

	EnemyGridPos = { (int)pos_.x / 32, (int)pos_.y / 32 };
	PlayerGritPos = { player->GetGridPos() };

	if (!stop) {
		Point op = pos_;
		Point move = { nDir[forward_].x, nDir[forward_].y };
		Rect eRect = { pos_.x, pos_.y,CHA_WIDTH, CHA_HEIGHT };//敵の位置（Rect型）


		if (Input::IsKeepKeyDown(KEY_INPUT_SPACE))
		{
			move = { 0,0 };
		}

		pos_ = { pos_.x + move.x, pos_.y + move.y };
		for (auto& obj : stage->GetStageRects())
		{
			if (CheckHit(eRect, obj)) {
				Rect tmpRectX = { op.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
				Rect tmpRecty = { pos_.x, op.y, CHA_WIDTH, CHA_HEIGHT };
				if (!CheckHit(tmpRectX, obj))
				{//x座標を元に戻したらぶつかっていない→x座標だけ元に戻す
					pos_.x = op.x;
				}
				else if (!CheckHit(tmpRecty, obj))
				{//y座標を元に戻したらぶつかっていない→y座標だけ元に戻す
					pos_.y = op.y;
				}
				else
				{//片方だけ戻してもやっぱりぶつかっている→両方元に戻す
					pos_ = op;
				}
				//forward_ = (DIR)(GetRand(3));
				//RightHandMove();
				break;
			}
		}
	}

	int prgssx = pos_.x % (CHA_WIDTH);
	int prgssy = pos_.y % (CHA_HEIGHT);

	//int cx = (pos_.x / (CHA_WIDTH))%2;
	//int cy = (pos_.y / (CHA_HEIGHT))%2;
	if (prgssx == 0 && prgssy == 0 /*&& cx && cy*/)
	{
		//BFS({ EnemyGridPos.x , EnemyGridPos.y }, PlayerGritPos.x, PlayerGritPos.y);
		DijkstraMove();
	}
	tmpRoute = stage->restore(PlayerGritPos.x, PlayerGritPos.y);//ゴールまでの道のりを保存
	//stageDist = stage->GetDist();//コストを保存
}

void Enemy::Draw()
{
	DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT,
		GetColor(80, 89, 10), TRUE);
	Point tp[4][3] = {
		{{pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
		{{pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
		{{pos_.x            , pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}},
		{{pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}}
	};

	DrawTriangle(tp[forward_][0].x, tp[forward_][0].y, tp[forward_][1].x, tp[forward_][1].y, tp[forward_][2].x, tp[forward_][2].y, GetColor(255, 255, 255), TRUE);

	/*for (auto itr : tmpRoute)
	{
		DrawBox(itr.x * CHA_WIDTH, itr.y * CHA_HEIGHT, itr.x * CHA_WIDTH + CHA_WIDTH, itr.y * CHA_HEIGHT + CHA_HEIGHT, GetColor(255, 255, 0), FALSE);
	}*/

	for (auto itr : cp)
	{
		DrawBox(itr.x * CHA_WIDTH, itr.y * CHA_HEIGHT, itr.x * CHA_WIDTH + CHA_WIDTH, itr.y * CHA_HEIGHT + CHA_HEIGHT, GetColor(255, 0, 0), FALSE);
	}
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (Edist[y][x] < INT_MAX)
			{
				std::string s = std::to_string(Edist[y][x]);
				const char* c = s.c_str();
				//ゴールまでのコストを表示
				DrawFormatString(x * CHA_WIDTH, y * CHA_HEIGHT, GetColor(0, 0, 255), "%s", c);
			}
		}
	}

	ImGui::Begin("config 1");
	//ImGui::Text("forward: %.1d", forward_);
	//ImGui::Text("Index_path: %.1d", index_path);
	//ImGui::Text("fx: %.1d,fy: %.1d", fx,fy);
	//ImGui::Text("cx: %.1d,cy: %.1d", cx, cy);

	ImGui::End();
}

void Enemy::YCloserMove()
{
	Player* player = (Player*)FindGameObject<Player>();
	if (pos_.y > player->GetPos().y)
	{
		forward_ = UP;
	}
	else if (pos_.y < player->GetPos().y)
	{
		forward_ = DOWN;
	}
}

void Enemy::XCloserMove()
{
	Player* player = (Player*)FindGameObject<Player>();
	if (pos_.x > player->GetPos().x)
	{
		forward_ = LEFT;
	}
	else if (pos_.x < player->GetPos().x)
	{
		forward_ = RIGHT;
	}
}

void Enemy::XYCloserMove()
{
	Player* player = (Player*)FindGameObject<Player>();
	int xdis = abs(pos_.x - player->GetPos().x);
	int ydis = abs(pos_.y - player->GetPos().y);

	if (xdis > ydis) {
		if (pos_.x > player->GetPos().x)
		{
			forward_ = LEFT;
		}
		else if (pos_.x < player->GetPos().x)
		{
			forward_ = RIGHT;
		}
	}
	else
	{
		if (pos_.y > player->GetPos().y)
		{
			forward_ = UP;
		}
		else if (pos_.y < player->GetPos().y)
		{
			forward_ = DOWN;
		}
	}
}

void Enemy::XYCloserMoveRandom()
{

	//３分の1の確率でプレイヤーに近い方に行く、残りの3分の1はランダム方向に移動、残りは何もしない
	Player* player = (Player*)FindGameObject<Player>();
	int xdis = abs(pos_.x - player->GetPos().x);
	int ydis = abs(pos_.y - player->GetPos().y);
	int rnum = GetRand(2);
	if (rnum == 0)
		XYCloserMove();
	else if (rnum == 1)
	{
		forward_ = (DIR)GetRand(3);
	}
}

void Enemy::RightHandMove()
{
	DIR myRight[4] = { RIGHT, LEFT, UP, DOWN };
	DIR myLeft[4] = { LEFT, RIGHT, DOWN, UP };
	Point nposF = { pos_.x + nDir[forward_].x, pos_.y + nDir[forward_].y };
	Point nposR = { pos_.x + nDir[myRight[forward_]].x, pos_.y + nDir[myRight[forward_]].y };
	Rect myRectF{ nposF.x, nposF.y, CHA_WIDTH, CHA_HEIGHT };
	Rect myRectR{ nposR.x, nposR.y, CHA_WIDTH, CHA_HEIGHT };
	Stage* stage = (Stage*)FindGameObject<Stage>();
	bool isRightOpen = true;
	bool isForwardOpen = true;
	for (auto& obj : stage->GetStageRects()) {
		if (CheckHit(myRectF, obj)) {
			isForwardOpen = false;
		}
		if (CheckHit(myRectR, obj)) {
			isRightOpen = false;
		}
	}
	if (isRightOpen)
	{
		forward_ = myRight[forward_];
	}
	else if (isRightOpen == false && isForwardOpen == false)
	{
		forward_ = myLeft[forward_];
	}
}

void Enemy::DijkstraMove()
{
	BFS({ EnemyGridPos.x , EnemyGridPos.y }, PlayerGritPos.x, PlayerGritPos.y);
}

void Enemy::BFS(pair<int, int> start, int endX, int endY)
{
	EnemyDijkstra(start);
	std::vector<Vec2> path = EnemyRestore(endX, endY);
	cp = path;
	if (path.empty())
	{
		forward_ = NONE;
		return;
	}
	if (index_path >= path.size() - 1)
	{
		forward_ = NONE;
		return;
	}

	// 現在の位置と次の位置を取得
	Vec2 currentPosition = path[index_path];
	Vec2 nextPosition = path[index_path + 1];
	fx = (int)currentPosition.x;
	fy = (int)currentPosition.y;
	cx = (int)nextPosition.x;
	cy = (int)nextPosition.y;

	// 次の位置に進む方向を判定
	int nextX = nextPosition.x - currentPosition.x;
	int nextY = nextPosition.y - currentPosition.y;


	// 各方向に対して、dx, dy の値に基づいて進む方向を決定
	if (nextX == 1 && nextY == 0) {
		forward_ = RIGHT;
	}
	else if (nextX == -1 && nextY == 0) {
		forward_ = LEFT;
	}
	else if (nextX == 0 && nextY == 1) {
		forward_ = DOWN;
	}
	else if (nextX == 0 && nextY == -1) {
		forward_ = UP;
	}
	else {
		forward_ = NONE;
	}
	path.erase(path.begin()); // path[0]を削除して次の座標を先頭に持ってくる
	index_path++;
}

void Enemy::EnemyDijkstra(std::pair<int, int> start)
{
	Stage* stage = (Stage*)FindGameObject<Stage>();

	//dist[1(y)][1(x)]をコストを0で初期化
	Edist[start.second][start.first] = 0;

	//Mdat型、Mdat型コンテナ、昇順
	std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>> pq;

	//コスト0,座標1,1で初期化
	pq.push(Mdat(0, { start.first, start.second }));

	while (!pq.empty())
	{
		Mdat p = pq.top();
		pq.pop();

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
			if (stage->GetStageDataXY(np.first, np.second) == STAGE_OBJ::WALL) continue;

			//探索する方向よりコストが大きいならスルー
			if (Edist[np.second][np.first] <= MazeCost[np.second][np.first].weight + cost) continue;

			//最短距離を更新
			Edist[np.second][np.first] = MazeCost[np.second][np.first].weight + cost;

			//辿った経路を保存
			Eprev[np.second][np.first] = Vec2{ (double)v.first, (double)v.second };

			//今のコストと探索方向の座標を保存
			pq.push(Mdat(Edist[np.second][np.first], np));
		}
	}

}

std::vector<Vec2> Enemy::EnemyRestore(int _x, int _y)
{
	std::vector<Vec2> path;
	int x = _x, y = _y;
	//_x,_yが-1にならない限り継続 ループ終了後_x = prev[y][x].x , _y = prev[y][x].yを代入
	for (; _x != -1 || _y != -1; _x = Eprev[y][x].x, _y = Eprev[y][x].y) {

		//pathに追跡前の位置を保管
		path.push_back(Vec2{ (double)_x, (double)_y });

		//x,yの更新
		x = (int)_x, y = (int)_y;
	}
	reverse(path.begin(), path.end());
	return path;
}

bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
	if (me.x < other.x + other.w &&
		me.x + me.w > other.x &&
		me.y < other.y + other.h &&
		me.y + me.h > other.y)
	{
		return true;
	}
	return false;
}


