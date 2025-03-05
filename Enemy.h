#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"


class Enemy :
    public GameObject
{
private:
    bool IsRandom;
    enum Move
    {
        XYCLOSEMOVE = 0,
        RIGHTHANDMOVE = 1,
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
    /// 
    /// </summary>
    void SetMove();
};