#include "Stage.h"
#include "./globals.h"
#include <stack>
#include<queue>
#include<concurrent_priority_queue.h>

namespace {
	std::stack<Point> prStack;

	//�_�C�N�X�g���@�̃R�X�g�ۑ��p
	//dist�̗v�f����[0]~[20](21��),���ꂼ�ꂪ39��INT_MAX������
	//�_�C�N�X�g���@�͎n�߂Ɋe�l�𖳌���(INT_MAX)�ŏ�����
	std::vector<std::vector<int>> dist(STAGE_HEIGHT, std::vector<int>(STAGE_WIDTH, INT_MAX));
	
	//�o�H�ۑ��p
	//prev�̗v�f��21��,���ꂼ�ꂪ39��{-1,-1}������
	std::vector<std::vector<Vec2>> prev(STAGE_HEIGHT, std::vector<Vec2>(STAGE_WIDTH, { -1, -1 })); 

	void DigDug(int x, int y, vector<vector<STAGE_OBJ>> &_stage)
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
	dist[sp.second][sp.first] = 0;//�X�^�[�g��y,x
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
			//����������m�F
			Vec2Int np = { v.first + (int)dirs[i].x, v.second + (int)dirs[i].y };

			//0���O���b�h�𒴂���T���͂��Ȃ�
			if (np.first < 0 || np.second < 0 || np.first >= MazeSize.x || np.second >= MazeSize.y) continue;

			//�ǂȂ�T�����Ȃ�
			if (MazeData[np.second][np.first].type == WALL) continue;

			//
			if (dist[np.second][np.first] <= MazeData[np.second][np.first].weight + c) continue;

			dist[np.second][np.first] = MazeData[np.second][np.first].weight + c;
			pre[np.second][np.first] = Vec2(v.first, v.second);
			pq.push(Mdat(dist[np.second][np.first], np));
		}
	}

}
