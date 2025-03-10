#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
using std::pair;

class Player :
    public GameObject
{
    int playerImage_;
    Point pos_;
    Point GridPos_;
public:
    Player();
    ~Player();
    Point GetPos() { return pos_; }
    Point GetGridPos() { Point tmp = { -1,-1 }; tmp.x = (int)pos_.x / 32; tmp.y = (int)pos_.y / 32; return tmp; }
    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me,const Rect& other);
};

