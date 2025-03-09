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
    std::vector<Vec2> CurrentRoute;

    int index_path = 0;
    std::vector<Vec2> cp;
    int fx, fy;
    int cx, cy;
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

    void BFS(std:: pair<int, int> start, int endX, int endY);
    void EnemyDijkstra(std::pair<int, int> start);
    std::vector<Vec2> EnemyRestore(int _x, int _y);
    //void DijkstraMove2();

};