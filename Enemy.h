#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include<vector>


class Enemy :
    public GameObject
{
private:
    enum Move
    {
        XYCLOSEMOVE = 0,
        RIGHTHANDMOVE = 1,
    };
    Move ChoiceMove;

    Point pos_;
    bool isAlive_;
    //float speed_;
    Point nextPos_;
    DIR forward_;

    int RandTimer_;
    std::vector<Vec2> tmpRoute;
    std::vector<std::vector<int>> stageDist;
    Point EnemyGridPos;
    Point PlayerGritPos;

    int index_path = 0;
    std::vector<Vec2> cp;//デバッグ用
    int fx, fy;//デバッグ用
    int cx, cy;//デバッグ用
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);

    /// <summary>
    /// Y方向だけ追いかける
    /// </summary>
    void YCloserMove();

    /// <summary>
    /// X方向だけ追いかける
    /// </summary>
    void XCloserMove();

    /// <summary>
    /// XYともに追いかける
    /// </summary>
    void XYCloserMove();

    /// <summary>
    /// ランダムで追いかける
    /// </summary>
    void XYCloserMoveRandom();

    /// <summary>
    /// 右手法で追いかける
    /// </summary>
    void RightHandMove();

    /// <summary>
    /// ダイクストラ法で追いかける
    /// </summary>
    void DijkstraMove();

    /// <summary>
    /// 幅優先探索で追いかける
    /// </summary>
    /// <param name="start">敵視点の探索の開始位置</param>
    /// <param name="endX">ゴールのx座標</param>
    /// <param name="endY">ゴールのy座標</param>
    void BFS(std::pair<int, int> start, int endX, int endY);

    /// <summary>
    /// 敵視点で経路逆探索
    /// </summary>
    /// <param name="start">探索の初期位置(敵の位置)</param>
    void EnemyDijkstra(std::pair<int, int> start);

    /// <summary>
    /// 敵視点で経路復元
    /// </summary>
    /// <param name="_x">ゴールのx座標(プレイヤーの位置)</param>
    /// <param name="_y">ゴールのy座標(プレイヤーの位置)</param>
    /// <returns>復元した経路(Vec2配列) [0]が敵の位置 maxがプレイヤーの位置</returns>
    std::vector<Vec2> EnemyRestore(int _x, int _y);

};