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


	//�񎟌��z��@MazeData[MazeSize.y][MazeSize.x]{FLOOR�ŏ������p
	std::vector<std::vector<floorData>> MazeDataDijkstra(STAGE_HEIGHT, std::vector<floorData>(STAGE_WIDTH, { STAGE_OBJ::EMPTY, 0 })); //���H���̂���

	//�_�C�N�X�g���@�̃R�X�g(�����܂ł̈ړ���)�ۑ��p
	//dist�̗v�f����[0]~[20](21��),���ꂼ�ꂪ39��INT_MAX������
	//�_�C�N�X�g���@�͎n�߂Ɋe�l�𖳌���(INT_MAX)�ŏ�����
	std::vector<std::vector<int>> dist(STAGE_HEIGHT, std::vector<int>(STAGE_WIDTH, INT_MAX));

	//�o�H�ۑ��p
	//prev�̗v�f��21��,���ꂼ�ꂪ39��{-1,-1}������
	std::vector<std::vector<Vec2>> prev(STAGE_HEIGHT, std::vector<Vec2>(STAGE_WIDTH, { -1, -1 }));

	void DigDug(int x, int y, vector<vector<STAGE_OBJ>>& _stage)
	{
		_stage[y][x] = STAGE_OBJ::EMPTY;

		Point Dir[]{ {0,-1},{1, 0},{0, 1},{-1,0} };

		//�ǁi���@��\��j�̃��X�g
		std::vector<int> dList;
		for (int i = 0; i < 4; i++)
		{
			//next��[0]~[3]�܂ŉ�
			Point next = Point{ x + Dir[i].x, y + Dir[i].y };
			//next�̂���ɐ�([0]~[3])���擾
			Point nextNext = { next.x + Dir[i].x, next.y + Dir[i].y };

			//��O�Ȃ��蒼��
			if (nextNext.x < 0 || nextNext.y < 0 || nextNext.x > STAGE_WIDTH - 1 || nextNext.y > STAGE_HEIGHT - 1)
				continue;

			//�ǂɂȂ��Ă�����������X�g�ɓ����
			if (_stage[nextNext.y][nextNext.x] == STAGE_OBJ::WALL)
			{
				dList.push_back(i);
			}
		}
		if (dList.empty())
		{
			return;
		}

		//���X�g�̒����烉���_���ɒl����擾�A���X�g�̗v�f�ԍ��Ƃ���
		int nrand = rand() % dList.size();

		//���X�g��nrand�ڂ���Ɨ̈�ɕۊǁA
		int tmp = dList[nrand];

		//�����_���ȕ����Ƃ���ɐ���擾
		Point next = { x + Dir[tmp].x, y + Dir[tmp].y };
		Point nextNext = { next.x + Dir[tmp].x, next.y + Dir[tmp].y };

		//��������ɂ���
		_stage[next.y][next.x] = STAGE_OBJ::EMPTY;
		_stage[nextNext.y][nextNext.x] = STAGE_OBJ::EMPTY;

		//�X�^�b�N�ɕۊ�
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

	//���@��@
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

		//�O�ǂ𖄂߂�+�d�ݕt��
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

	//�_�|��
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
						//�_�𗧂Ă��炷�������|��
						int rnd = rand() % 4;//0:���@1:���@2:���@3:��
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

		//�O�ǂ𖄂߂�
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

	//�O���b�h
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

	//route = restore(GOAL_POINT.x, GOAL_POINT.y );//�o�H���t�ǐ�

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
			//case 1://�f�t�H���g
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
				//�S�[���܂ł̃R�X�g��\��
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
	//dist[1(y)][1(x)]���R�X�g��0�ŏ�����
	dist[sp.second][sp.first] = 0;

	//Mdat�^�AMdat�^�R���e�i�A����
	std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>> pq;

	//�R�X�g0,���W1,1�ŏ�����
	pq.push(Mdat(0, { sp.first, sp.second }));

	while (!pq.empty())
	{
		Mdat p = pq.top();
		pq.pop();

		//Rect{ (int)p.second.first * CHA_WIDTH, (int)p.second.second * CHA_HEIGHT, BLOCK_SIZE }.draw(Palette::Red);

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
			if (stageData[np.second][np.first] == STAGE_OBJ::WALL) continue;

			//�T������������R�X�g���傫���Ȃ�X���[
			if (dist[np.second][np.first] <= MazeDataDijkstra[np.second][np.first].weight + cost) continue;

			//�ŒZ�������X�V
			dist[np.second][np.first] = MazeDataDijkstra[np.second][np.first].weight + cost;

			//�H�����o�H��ۑ�
			prev[np.second][np.first] = Vec2{ (double)v.first, (double)v.second };

			//���̃R�X�g�ƒT�������̍��W��ۑ�
			pq.push(Mdat(dist[np.second][np.first], np));
		}
	}

}

std::vector<Vec2> Stage::restore(int _x, int _y)
{
	std::vector<Vec2> path;
	int x = _x, y = _y;
	//_x,_y��-1�ɂȂ�Ȃ�����p�� ���[�v�I����_x = prev[y][x].x , _y = prev[y][x].y����
	for (; _x != -1 || _y != -1; _x = prev[y][x].x, _y = prev[y][x].y) {

		//path�ɒǐՑO�̈ʒu��ۊ�
		path.push_back(Vec2{ (double)_x, (double)_y });

		//x,y�̍X�V
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
