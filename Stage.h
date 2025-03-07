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

const Point START_POINT{ 1,1 };
const Point GOAL_POINT{ STAGE_WIDTH - 2,STAGE_HEIGHT - 2 };

//�_�C�N�X�g���@�p
//�X�e�[�W�̈�u���b�N���Ƃ̃^�C�v�ƃE�F�C�g��ۑ�
struct floorData
{
	STAGE_OBJ type; //���̃u���b�N�̎�� floor,wall...
	int  weight; //�ړ��R�X�g
};

class Stage :
    public GameObject
{
	vector<vector<STAGE_OBJ>> stageData;
	vector<Rect> stageRects;
	std::vector<Vec2> route;

	std::vector<Vec2> route_;
public:
	Stage();
	~Stage();
	void Update() override;
	void Draw() override;
	STAGE_OBJ GetStageData(int x, int y) {return stageData[y][x];}
	vector<Rect> GetStageRects() { return stageRects; }
	vector<vector<STAGE_OBJ>>& GetStageGrid() { return stageData; }
	void setStageRects();

	/// <summary>
	/// �_�C�N�X�g���@�̑S�T��
	/// </summary>
	/// <param name="sp">�S�T���̎n�_</param>
	void Dijkstra(pair<int, int> sp);

	/// <summary>
	/// �o�H�̋t�ǐ�
	/// </summary>
	/// <param name="_x">�t�ǐՂ��n�߂�ʒu��x���W</param>
	/// <param name="_y">�t�ǐՂ��n�߂�ʒu��y���W</param>
	/// <returns>�T�������o�H�̔z��</returns>
	std::vector<Vec2> restore(int _x, int _y);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="sp">�S�T���̎n�_</param>
	/// <param name="endX">�t�ǐՂ��n�߂�ʒu��x���W</param>
	/// <param name="endY">�t�ǐՂ��n�߂�ʒu��y���W</param>
	/// <returns>���ɐi�ޕ���</returns>
	DIR DijkstraRoute(pair<int, int> sp, int endX, int endY);

	std::vector<std::vector<int>> GetDist();

	
};