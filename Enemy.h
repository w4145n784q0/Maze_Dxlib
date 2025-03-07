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
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);

    /// <summary>
    /// Y���������ǂ�������
    /// </summary>
    void YCloserMove();

    /// <summary>
    /// X���������ǂ�������
    /// </summary>
    void XCloserMove();

    /// <summary>
    /// XY�Ƃ��ɒǂ�������
    /// </summary>
    void XYCloserMove();

    /// <summary>
    /// �����_���Œǂ�������
    /// </summary>
    void XYCloserMoveRandom();

    /// <summary>
    /// �E��@�Œǂ�������
    /// </summary>
    void RightHandMove();

    /// <summary>
    /// �_�C�N�X�g���@�Œǂ�������
    /// </summary>
    void DijkstraMove();

    //void DijkstraMove2();

};