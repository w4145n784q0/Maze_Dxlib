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

const Point START_POINT{ 1,1 };
const Point GOAL_POINT{ STAGE_WIDTH - 2,STAGE_HEIGHT - 2 };

//ダイクストラ法用
//ステージの一ブロックごとのタイプとウェイトを保存
struct floorData
{
	STAGE_OBJ type; //そのブロックの種類 floor,wall...
	int  weight; //移動コスト
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
	/// ダイクストラ法の全探索
	/// </summary>
	/// <param name="sp">全探索の始点</param>
	void Dijkstra(pair<int, int> sp);

	/// <summary>
	/// 経路の逆追跡
	/// </summary>
	/// <param name="_x">逆追跡を始める位置のx座標</param>
	/// <param name="_y">逆追跡を始める位置のy座標</param>
	/// <returns>探索した経路の配列</returns>
	std::vector<Vec2> restore(int _x, int _y);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="sp">全探索の始点</param>
	/// <param name="endX">逆追跡を始める位置のx座標</param>
	/// <param name="endY">逆追跡を始める位置のy座標</param>
	/// <returns>次に進む方向</returns>
	DIR DijkstraRoute(pair<int, int> sp, int endX, int endY);

	std::vector<std::vector<int>> GetDist();

	
};