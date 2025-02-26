#pragma once
#include "./Library/GameObject.h"
#include <vector>
#include <map>
#include <string>
#include <vector>
#include "./Source/Screen.h"
#include "./globals.h"


using std::vector;
using std::pair;
/// <summary>
/// x,y���W��2�y�A
/// first :  x���W
/// second : y���W
/// </summary>
using Vec2Int = std::pair<int, int>;

/// <summary>
/// �ŒZ������x,y���W��2�y�A
/// int : �ŒZ����
/// first : x���W
/// second : y���W
/// </summary>
using Mdat = std::pair<int, Vec2Int>;

const Vec2 dirs[4]{ {0,1},{0,-1},{1,0},{-1,0} };

constexpr int STAGE_WIDTH = (Screen::WIDTH / CHA_WIDTH) % 2 ? (Screen::WIDTH / CHA_WIDTH) : (Screen::WIDTH / CHA_WIDTH) - 1;
constexpr int STAGE_HEIGHT = (Screen::HEIGHT / CHA_HEIGHT) % 2 ? (Screen::HEIGHT / CHA_HEIGHT) : (Screen::HEIGHT / CHA_HEIGHT) - 1;

//�_�C�N�X�g���@�p
	//�X�e�[�W�̈�u���b�N���Ƃ̃^�C�v�ƃE�F�C�g��ۑ�
struct floorData
{
	STAGE_OBJ type;
	int  weight;
};

class Stage :
    public GameObject
{
	vector<vector<STAGE_OBJ>> stageData;
	vector<Rect> stageRects;
public:
	Stage();
	~Stage();
	void Update() override;
	void Draw() override;
	STAGE_OBJ GetStageData(int x, int y) {return stageData[y][x];}
	vector<Rect> GetStageRects() { return stageRects; }
	vector<vector<STAGE_OBJ>>& GetStageGrid() { return stageData; }
	void setStageRects();

	void Dijkstra(pair<int, int> sp);
};

