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
    std::vector<Vec2> cp;//�f�o�b�O�p
    int fx, fy;//�f�o�b�O�p
    int cx, cy;//�f�o�b�O�p
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

    /// <summary>
    /// ���D��T���Œǂ�������
    /// </summary>
    /// <param name="start">�G���_�̒T���̊J�n�ʒu</param>
    /// <param name="endX">�S�[����x���W</param>
    /// <param name="endY">�S�[����y���W</param>
    void BFS(std::pair<int, int> start, int endX, int endY);

    /// <summary>
    /// �G���_�Ōo�H�t�T��
    /// </summary>
    /// <param name="start">�T���̏����ʒu(�G�̈ʒu)</param>
    void EnemyDijkstra(std::pair<int, int> start);

    /// <summary>
    /// �G���_�Ōo�H����
    /// </summary>
    /// <param name="_x">�S�[����x���W(�v���C���[�̈ʒu)</param>
    /// <param name="_y">�S�[����y���W(�v���C���[�̈ʒu)</param>
    /// <returns>���������o�H(Vec2�z��) [0]���G�̈ʒu max���v���C���[�̈ʒu</returns>
    std::vector<Vec2> EnemyRestore(int _x, int _y);

};