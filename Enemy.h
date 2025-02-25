#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"


class Enemy :
    public GameObject
{
private:
    enum Move
    {
        XYCLOSEMOVE = 0,
        RIGHTMOVE = 1,
    };
    Move ChoiceMove;

    Point pos_;
    bool isAlive_;
    float speed_;
    Point nextPos_;
    DIR forward_;

    int RandTimer_;
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
    /// 
    /// </summary>
    void SetRandomMove();

    void SetMove(Move _move){if(_move == RIGHTMOVE)  }
};