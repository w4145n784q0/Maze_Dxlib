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
    /// Y•ûŒü‚¾‚¯’Ç‚¢‚©‚¯‚é
    /// </summary>
    void YCloserMove();

    /// <summary>
    /// X•ûŒü‚¾‚¯’Ç‚¢‚©‚¯‚é
    /// </summary>
    void XCloserMove();

    /// <summary>
    /// XY‚Æ‚à‚É’Ç‚¢‚©‚¯‚é
    /// </summary>
    void XYCloserMove();

    /// <summary>
    /// ƒ‰ƒ“ƒ_ƒ€‚Å’Ç‚¢‚©‚¯‚é
    /// </summary>
    void XYCloserMoveRandom();

    /// <summary>
    /// ‰EŽè–@‚Å’Ç‚¢‚©‚¯‚é
    /// </summary>
    void RightHandMove();

    /// <summary>
    /// 
    /// </summary>
    void SetMove();
};