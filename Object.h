#pragma once

extern enum class DIRECTION {
	UP, DOWN, LEFT, RIGHT
};

extern enum class Block_Type {
	GRASS, SOIL, DARK_GRASS
};

extern enum class Chara_Type {
	PLAYER, COT
};

extern enum class Enemy_Statue {
	SAFE, ACTIVE
};

extern enum class Effect_Type {
	WARNING, QUESTION,TACKLE
};

extern enum class Num_Type{
	NORMAL,CRITICAL
};

class Sort_Def {
public:
	D3DXVECTOR3 pos;
	D3DXVECTOR2 size;
	D3DXVECTOR2 correct;
	IDirect3DTexture9* image;
public:
	Sort_Def(D3DXVECTOR3 pos, D3DXVECTOR2 size,D3DXVECTOR2 correct,IDirect3DTexture9* image) {
		this->pos = pos;
		this->size = size;
		this->correct = correct;
		this->image = image;
	}
};

class Chara {
public:
	D3DXVECTOR3 pos;
public:
	void SetPos(D3DXVECTOR3);
};

void Chara::SetPos(D3DXVECTOR3 pos){
	this->pos = pos;
}

class Player:public Chara {
public:
	DIRECTION walk_dir = DIRECTION::RIGHT;//UDLRで4方向を示します。
	int walk_count = 72;//マス目移動の時に使われます。0~72の間で変化し、座標系に影響を与えます。
	int walk_speed = 4;//仮想の定数です。
	int animation_count = 0;//animationのタイマー
	Player();
};

Player::Player() {

}

union Enemy_Select{
	Enemy_Select() {

	};
	~Enemy_Select() {

	};
};

class Enemy :public Chara {
public:
	DIRECTION walk_dir = DIRECTION::RIGHT;
	Chara_Type type = Chara_Type::COT;
	Enemy_Select sel;
	Enemy_Statue statue = Enemy_Statue::SAFE;
	std::vector<Node> P_Node;
	int walk_count = 72;
	int walk_speed = 3;
	int animation_count = 0;
	int go_x = -1, go_y = -1;
	Enemy(Chara_Type, D3DXVECTOR3);
};

Enemy::Enemy(Chara_Type type, D3DXVECTOR3 pos) {
	this->type = type;
	this->pos = pos;
	switch (this->type) {
	case Chara_Type::COT:walk_speed = 2; break;
	}
}

class Effect {
public:
	Effect_Type type;
	D3DXVECTOR3 s_pos;//screen_pos
	int count = 0;
public:
	Effect(Effect_Type type, D3DXVECTOR3 s_pos) {
		this->type = type;
		this->s_pos = s_pos;
	}
};

class Number {
public:
	int num;
	D3DXVECTOR3 s_pos;
	Num_Type type;
	int count = 0;
public:
	Number(int num,D3DXVECTOR3 s_pos, Num_Type type) {
		this->num = num;
		this->s_pos = s_pos;
		this->type = type;
	}
};