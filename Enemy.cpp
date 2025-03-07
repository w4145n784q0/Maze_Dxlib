#include "Enemy.h"
#include "./Stage.h"
#include "globals.h"
#include "Player.h"
#include <map>
#include "./Source/Input.h"
#include "ImGui/imgui.h"
#include <algorithm> 

namespace
{
	//四方向(↑,↓,←,→)
	Point nDir[4] = { {0,-1},{0,1},{-1,0},{1,0} };

	//ランダム移動の切り替え時間
	const int ChangeTimer = 600;

	int temp = 1;

	DIR dirlist[] = { UP,DOWN,LEFT,RIGHT };
}

Enemy::Enemy()
	:pos_({ 0,0 }), isAlive_(true), nextPos_({ 0,0 }),RandTimer_(0),ChoiceMove(XYCLOSEMOVE)
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
	stage->Dijkstra({EnemyGridPos.x, EnemyGridPos.y });
	tmpRoute = stage->restore(tmp.x, tmp.y);//ルートを保存
	stageDist = stage->GetDist();
	
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

	if (!stop) {
		Point op = pos_;
		Point move = { nDir[forward_].x, nDir[forward_].y };
		Rect eRect = { pos_.x, pos_.y,CHA_WIDTH, CHA_HEIGHT };//敵の位置（Rect型）


		//if (Input::IsKeepKeyDown(KEY_INPUT_SPACE))
		//{
		//	move = { 0,0 };
		//}
		
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
	int cx = (pos_.x / (CHA_WIDTH))%2;
	int cy = (pos_.y / (CHA_HEIGHT))%2;
	if (prgssx == 0 && prgssy == 0 && cx && cy)
	{
		DijkstraMove();
	}

	//Point tmp = player->GetGridPos();
	//tmpRoute = stage->restore(tmp.x, tmp.y);//ゴールまでの道のりを保存
	//stageDist = stage->GetDist();//コストを保存
}

void Enemy::Draw()
{
	DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, 
		GetColor(80, 89, 10), TRUE);
	Point tp[4][3] = {
		{{pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
		{{pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
		{{pos_.x            , pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH/2, pos_.y  + CHA_HEIGHT}},
		{{pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH/2, pos_.y + CHA_HEIGHT}}
					};

	DrawTriangle(tp[forward_][0].x, tp[forward_][0].y, tp[forward_][1].x, tp[forward_][1].y, tp[forward_][2].x, tp[forward_][2].y, GetColor(255,255,255), TRUE);

	for (auto itr : tmpRoute)
	{
		DrawBox(itr.x * CHA_WIDTH, itr.y * CHA_HEIGHT, itr.x * CHA_WIDTH + CHA_WIDTH, itr.y * CHA_HEIGHT + CHA_HEIGHT, GetColor(255, 255, 0), FALSE);
	}

	ImGui::Begin("config 1");
	ImGui::Text("forward: %.1d", forward_);
	//ImGui::Text("x: %.1d", EnemyGridPos.x);
	//ImGui::Text("y: %.1d", EnemyGridPos.y);
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
	Stage* stage = (Stage*)FindGameObject<Stage>();
	Player* player = (Player*)FindGameObject<Player>();
	//forward_ = stage->DijkstraRoute({ EnemyGridPos.x , EnemyGridPos.y }, player->GetGridPos().x, player->GetGridPos().y);
	forward_ = stage->DijkstraQueue({ EnemyGridPos.x , EnemyGridPos.y }, player->GetGridPos().x, player->GetGridPos().y);
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


