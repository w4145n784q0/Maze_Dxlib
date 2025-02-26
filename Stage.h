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
/// x,y座標の2ペア
/// first :  x座標
/// second : y座標
/// </summary>
using Vec2Int = std::pair<int, int>;

/// <summary>
/// 最短距離とx,y座標の2ペア
/// int : 最短距離
/// first : x座標
/// second : y座標
/// </summary>
using Mdat = std::pair<int, Vec2Int>;

const Vec2 dirs[4]{ {0,1},{0,-1},{1,0},{-1,0} };

constexpr int STAGE_WIDTH = (Screen::WIDTH / CHA_WIDTH) % 2 ? (Screen::WIDTH / CHA_WIDTH) : (Screen::WIDTH / CHA_WIDTH) - 1;
constexpr int STAGE_HEIGHT = (Screen::HEIGHT / CHA_HEIGHT) % 2 ? (Screen::HEIGHT / CHA_HEIGHT) : (Screen::HEIGHT / CHA_HEIGHT) - 1;

//ダイクストラ法用
	//ステージの一ブロックごとのタイプとウェイトを保存
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

