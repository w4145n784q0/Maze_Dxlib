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
	//�l����(��,��,��,��)
	Point nDir[4] = { {0,-1},{0,1},{-1,0},{1,0} };

	//�����_���ړ��̐؂�ւ�����
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

	//�����ʒu���ǂȂ�߂�
	Rect eRect = { pos_.x, pos_.y,CHA_WIDTH, CHA_HEIGHT };//�G�̈ʒu�iRect�^�j
	for (auto& obj : stage->GetStageRects())
	{
		Point op = pos_;
		if (CheckHit(eRect, obj)) {
			Rect tmpRectX = { op.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
			Rect tmpRecty = { pos_.x, op.y, CHA_WIDTH, CHA_HEIGHT };
			if (!CheckHit(tmpRectX, obj))
			{//x���W�����ɖ߂�����Ԃ����Ă��Ȃ���x���W�������ɖ߂�
				pos_.x = op.x;
			}
			else if (!CheckHit(tmpRecty, obj))
			{//y���W�����ɖ߂�����Ԃ����Ă��Ȃ���y���W�������ɖ߂�
				pos_.y = op.y;
			}
			else
			{//�Е������߂��Ă�����ς�Ԃ����Ă��遨�������ɖ߂�
				pos_ = op;
			}
		}
	}

	EnemyGridPos = { pos_.x / 32, pos_.y / 32 };
	Point tmp = player->GetGridPos();
	stage->Dijkstra({ EnemyGridPos.x, EnemyGridPos.y });
	tmpRoute = stage->restore(tmp.x, tmp.y);//���[�g��ۑ�
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
		Rect eRect = { pos_.x, pos_.y,CHA_WIDTH, CHA_HEIGHT };//�G�̈ʒu�iRect�^�j


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
				{//x���W�����ɖ߂�����Ԃ����Ă��Ȃ���x���W�������ɖ߂�
					pos_.x = op.x;
				}
				else if (!CheckHit(tmpRecty, obj))
				{//y���W�����ɖ߂�����Ԃ����Ă��Ȃ���y���W�������ɖ߂�
					pos_.y = op.y;
				}
				else
				{//�Е������߂��Ă�����ς�Ԃ����Ă��遨�������ɖ߂�
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
	tmpRoute = stage->restore(PlayerGritPos.x, PlayerGritPos.y);//�S�[���܂ł̓��̂��ۑ�
	//stageDist = stage->GetDist();//�R�X�g��ۑ�
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
				//�S�[���܂ł̃R�X�g��\��
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

	//�R����1�̊m���Ńv���C���[�ɋ߂����ɍs���A�c���3����1�̓����_�������Ɉړ��A�c��͉������Ȃ�
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

	// ���݂̈ʒu�Ǝ��̈ʒu���擾
	Vec2 currentPosition = path[index_path];
	Vec2 nextPosition = path[index_path + 1];
	fx = (int)currentPosition.x;
	fy = (int)currentPosition.y;
	cx = (int)nextPosition.x;
	cy = (int)nextPosition.y;

	// ���̈ʒu�ɐi�ޕ����𔻒�
	int nextX = nextPosition.x - currentPosition.x;
	int nextY = nextPosition.y - currentPosition.y;


	// �e�����ɑ΂��āAdx, dy �̒l�Ɋ�Â��Đi�ޕ���������
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
	path.erase(path.begin()); // path[0]���폜���Ď��̍��W��擪�Ɏ����Ă���
	index_path++;
}

void Enemy::EnemyDijkstra(std::pair<int, int> start)
{
	Stage* stage = (Stage*)FindGameObject<Stage>();

	//dist[1(y)][1(x)]���R�X�g��0�ŏ�����
	Edist[start.second][start.first] = 0;

	//Mdat�^�AMdat�^�R���e�i�A����
	std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>> pq;

	//�R�X�g0,���W1,1�ŏ�����
	pq.push(Mdat(0, { start.first, start.second }));

	while (!pq.empty())
	{
		Mdat p = pq.top();
		pq.pop();

		//�ŒZ������ۊ�
		int cost = p.first;

		//���ݒl��ۊ�
		Vec2Int v = p.second;

		for (int i = 0; i < 4; i++)
		{
			//�T�������@dirs[0]~[3]�������m�F
			//first : �T�����x����
			//second : �T�����y����
			Vec2Int np = { v.first + (int)dirs[i].x, v.second + (int)dirs[i].y };

			//0���O���b�h�𒴂���T���͂��Ȃ�
			if (np.first < 0 || np.second < 0 || np.first >= STAGE_WIDTH || np.second >= STAGE_HEIGHT) continue;

			//�ǂȂ�T�����Ȃ�
			if (stage->GetStageDataXY(np.first, np.second) == STAGE_OBJ::WALL) continue;

			//�T������������R�X�g���傫���Ȃ�X���[
			if (Edist[np.second][np.first] <= MazeCost[np.second][np.first].weight + cost) continue;

			//�ŒZ�������X�V
			Edist[np.second][np.first] = MazeCost[np.second][np.first].weight + cost;

			//�H�����o�H��ۑ�
			Eprev[np.second][np.first] = Vec2{ (double)v.first, (double)v.second };

			//���̃R�X�g�ƒT�������̍��W��ۑ�
			pq.push(Mdat(Edist[np.second][np.first], np));
		}
	}

}

std::vector<Vec2> Enemy::EnemyRestore(int _x, int _y)
{
	std::vector<Vec2> path;
	int x = _x, y = _y;
	//_x,_y��-1�ɂȂ�Ȃ�����p�� ���[�v�I����_x = prev[y][x].x , _y = prev[y][x].y����
	for (; _x != -1 || _y != -1; _x = Eprev[y][x].x, _y = Eprev[y][x].y) {

		//path�ɒǐՑO�̈ʒu��ۊ�
		path.push_back(Vec2{ (double)_x, (double)_y });

		//x,y�̍X�V
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


