#define D3D_DEBUG_INFO
#define _CRT_SECURE_NO_WARNINGS//tfopen
#define DIRECTSOUND_VERSION 0x800	// DirectSoundのバージョン
#pragma comment(lib,"dsound.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#include <comdef.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>
#include <mmsystem.h>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include "RandMaze_C.h"
#include "Astar_C.h"
#include "DirectX_C.h"
#include "Object.h"
#include "Attack.h"

const float zoom = 1.0f;//1.0f;
const float Block_Draw_Area = 12.0f;//12.0f;
const float BLOCK_SIZE = 128.0f * zoom;
const int map_x = 41, map_y = 41, map_z = 2;//map_x,map_y共に奇数
extern D3DXVECTOR3 real_block_size(72.0f, 72.0f, 36.0f);//約数の個数で決める

int _Bx(float x) {
	return int((x + real_block_size.x / 2.0f) / real_block_size.x);
}
int _By(float y) {
	return int((y + real_block_size.y / 2.0f) / real_block_size.y);
}
int _Bz(float z) {
	return int((z + real_block_size.z) / real_block_size.z);
}

int ctoi(const char c) {
	switch (c) {
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	default: return -1;
	}
}

Direct3D direct3d;
DirectSound directsound;


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//Window
	const TCHAR* WC_BASIC = _T("BASIC_WINDOW");

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,WndProc, 0,0,hInstance,
		(HICON)LoadImage(NULL,MAKEINTRESOURCE(IDI_APPLICATION),IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_SHARED),
		(HCURSOR)LoadImage(NULL,MAKEINTRESOURCE(IDC_ARROW),IMAGE_CURSOR,0,0,LR_DEFAULTSIZE | LR_SHARED),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, WC_BASIC , NULL };//ウィンドウの設定
	if (!RegisterClassEx(&wcex)) {
		return false;
	}

	const int WINDOW_WIDTH = 1300;
	const int WINDOW_HEIGHT = 800;
	HWND hWnd = CreateWindowEx(0, WC_BASIC,
		_T("Game"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	//DirectX
	direct3d.CreateDirect3D(hWnd, WINDOW_WIDTH, WINDOW_HEIGHT);
	directsound.Create(hWnd);

	//pen
	Sprite brush;
	brush.SetPos(0, 0);
	brush.SetWidth(0, 0);

	//Objects
	Player player;
	player.SetPos(D3DXVECTOR3(real_block_size.x, real_block_size.y, 0.0f));

	//Vectors
	Chara Camera;
	std::vector<Sort_Def> Sort;
	std::vector<Enemy> enemys;
	std::vector<Effect> effects;
	std::vector<Number> numbers;

	//Textures
	Texture Block_tex[3];
	{
		Block_tex[(int)Block_Type::GRASS].Load(direct3d.pDevice3D, _T("images/Blocks/GRASS.png"));
		Block_tex[(int)Block_Type::DARK_GRASS].Load(direct3d.pDevice3D, _T("images/Blocks/DARK_GRASS.png"));
		Block_tex[(int)Block_Type::SOIL].Load(direct3d.pDevice3D, _T("images/Blocks/SOIL.png"));
	}
	Texture Effect_tex[3][2];
	{
		Effect_tex[(int)Effect_Type::WARNING][0].Load(direct3d.pDevice3D, _T("images/Effects/WARNING.png"));
		Effect_tex[(int)Effect_Type::QUESTION][0].Load(direct3d.pDevice3D, _T("images/Effects/QUESTION.png"));
		Effect_tex[(int)Effect_Type::TACKLE][0].Load(direct3d.pDevice3D, _T("images/Effects/TACKLE/1.png"));
		Effect_tex[(int)Effect_Type::TACKLE][1].Load(direct3d.pDevice3D, _T("images/Effects/TACKLE/2.png"));
	}
	Texture Num_tex[2][10];
	{
		//NORMAL
		{
			Num_tex[(int)Num_Type::NORMAL][0].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/0.png"));
			Num_tex[(int)Num_Type::NORMAL][1].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/1.png"));
			Num_tex[(int)Num_Type::NORMAL][2].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/2.png"));
			Num_tex[(int)Num_Type::NORMAL][3].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/3.png"));
			Num_tex[(int)Num_Type::NORMAL][4].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/4.png"));
			Num_tex[(int)Num_Type::NORMAL][5].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/5.png"));
			Num_tex[(int)Num_Type::NORMAL][6].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/6.png"));
			Num_tex[(int)Num_Type::NORMAL][7].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/7.png"));
			Num_tex[(int)Num_Type::NORMAL][8].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/8.png"));
			Num_tex[(int)Num_Type::NORMAL][9].Load(direct3d.pDevice3D, _T("images/Numbers/NORMAL/9.png"));
		}
		//CRITICAL
		{
			Num_tex[(int)Num_Type::CRITICAL][0].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/0.png"));
			Num_tex[(int)Num_Type::CRITICAL][1].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/1.png"));
			Num_tex[(int)Num_Type::CRITICAL][2].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/2.png"));
			Num_tex[(int)Num_Type::CRITICAL][3].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/3.png"));
			Num_tex[(int)Num_Type::CRITICAL][4].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/4.png"));
			Num_tex[(int)Num_Type::CRITICAL][5].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/5.png"));
			Num_tex[(int)Num_Type::CRITICAL][6].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/6.png"));
			Num_tex[(int)Num_Type::CRITICAL][7].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/7.png"));
			Num_tex[(int)Num_Type::CRITICAL][8].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/8.png"));
			Num_tex[(int)Num_Type::CRITICAL][9].Load(direct3d.pDevice3D, _T("images/Numbers/CRITICAL/9.png"));
		}
	}
	Texture Chara_tex[2][4][2];
	{
		//player_tex
		{
			Chara_tex[(int)Chara_Type::PLAYER][(int)DIRECTION::DOWN][0].Load(direct3d.pDevice3D, _T("images/Characters/Player/D_0.png"));
			Chara_tex[(int)Chara_Type::PLAYER][(int)DIRECTION::DOWN][1].Load(direct3d.pDevice3D, _T("images/Characters/Player/D_1.png"));
			Chara_tex[(int)Chara_Type::PLAYER][(int)DIRECTION::UP][0].Load(direct3d.pDevice3D, _T("images/Characters/Player/U_0.png"));
			Chara_tex[(int)Chara_Type::PLAYER][(int)DIRECTION::UP][1].Load(direct3d.pDevice3D, _T("images/Characters/Player/U_1.png"));
			Chara_tex[(int)Chara_Type::PLAYER][(int)DIRECTION::LEFT][0].Load(direct3d.pDevice3D, _T("images/Characters/Player/L_0.png"));
			Chara_tex[(int)Chara_Type::PLAYER][(int)DIRECTION::LEFT][1].Load(direct3d.pDevice3D, _T("images/Characters/Player/L_1.png"));
			Chara_tex[(int)Chara_Type::PLAYER][(int)DIRECTION::RIGHT][0].Load(direct3d.pDevice3D, _T("images/Characters/Player/R_0.png"));
			Chara_tex[(int)Chara_Type::PLAYER][(int)DIRECTION::RIGHT][1].Load(direct3d.pDevice3D, _T("images/Characters/Player/R_1.png"));
		}
		//cot_tex
		{
			Chara_tex[(int)Chara_Type::COT][(int)DIRECTION::DOWN][0].Load(direct3d.pDevice3D, _T("images/Characters/Cot/D_0.png"));
			Chara_tex[(int)Chara_Type::COT][(int)DIRECTION::DOWN][1].Load(direct3d.pDevice3D, _T("images/Characters/Cot/D_1.png"));
			Chara_tex[(int)Chara_Type::COT][(int)DIRECTION::UP][0].Load(direct3d.pDevice3D, _T("images/Characters/Cot/U_0.png"));
			Chara_tex[(int)Chara_Type::COT][(int)DIRECTION::UP][1].Load(direct3d.pDevice3D, _T("images/Characters/Cot/U_1.png"));
			Chara_tex[(int)Chara_Type::COT][(int)DIRECTION::LEFT][0].Load(direct3d.pDevice3D, _T("images/Characters/Cot/L_0.png"));
			Chara_tex[(int)Chara_Type::COT][(int)DIRECTION::LEFT][1].Load(direct3d.pDevice3D, _T("images/Characters/Cot/L_1.png"));
			Chara_tex[(int)Chara_Type::COT][(int)DIRECTION::RIGHT][0].Load(direct3d.pDevice3D, _T("images/Characters/Cot/R_0.png"));
			Chara_tex[(int)Chara_Type::COT][(int)DIRECTION::RIGHT][1].Load(direct3d.pDevice3D, _T("images/Characters/Cot/R_1.png"));
		}
	}

	//Sounds
	Wave wave[2];
	wave[0].Load(_T("sounds/Damage.wav"));
	wave[1].Load(_T("sounds/found.wav"));

	SoundBuffer sb[2];
	for (int i = 0; i < 2; ++i) {
		sb[i].Create(directsound.pDirectSound8, wave[i].WaveFormat, wave[i].WaveData, wave[i].DataSize);
	}

	int map[map_z][map_y][map_x];//z,y,x z:高さ y:奥行 x:横幅
	int** map_maze;

	//迷路制作
	map_maze = maze_create(map_x, map_y);

	//反映
	{
		for (int i = 0; i < map_y; i++) {
			for (int j = 0; j < map_x; j++) {
				if (i == 0 || i == map_y - 1 || j == 0 || j == map_x - 1) {
					map[0][i][j] = 2;
				}
				else {
					map[0][i][j] = 3;
				}
			}
		}
		for (int i = 0; i < map_y; i++) {
			for (int j = 0; j < map_x; j++) {
				map[1][i][j] = map_maze[i][j];
				if (map_maze[i][j] == 0 && rand() % 60 == 0) {
					enemys.push_back({ Chara_Type::COT,D3DXVECTOR3(real_block_size.x * j,real_block_size.y * i,0.0f) });
				}
			}
		}

		for (int i = 1; i < map_y - 1; i++) {
			for (int j = 1; j < map_x - 1; j++) {
				if (rand() % 5 == 0) {
					map[1][i][j] = 0;
				}
			}
		}
	}

	//Astar
	bool bool_Map[map_z][map_y][map_x];
	{
		for (int i = 0; i < map_z; i++) {
			for (int j = 0; j < map_y; j++) {
				for (int k = 0; k < map_x; k++) {
					if (map[i][j][k] != 0) {
						bool_Map[i][j][k] = 1;
					}
					else {
						bool_Map[i][j][k] = 0;
					}
				}
			}
		}
	}

	bool* pMap[map_y];//mapを代入できる形にする
	{
		for (int i = 0; i < map_y; i++) {
			pMap[i] = bool_Map[1][i];
		}
	}

	std::vector<Node> goal_Node;

	DWORD ClearColor = D3DCOLOR_XRGB(0, 0, 0);
	MSG msg = {};

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			DispatchMessage(&msg);	// アプリケーションの各ウィンドウプロシージャにメッセージを転送する
		}
		else {
			if (SUCCEEDED(direct3d.pDevice3D->BeginScene())) {
				direct3d.pDevice3D->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, ClearColor, 1.0f, 0);
				
				//Block==============================
				{
					for (int i = 0; i < map_z; i++) {
						for (int j = 0; j < map_y; j++) {
							for (int k = 0; k < map_x; k++) {
								D3DXVECTOR3 Vec = player.pos - D3DXVECTOR3(k * real_block_size.x - real_block_size.x / 2.0f, j * real_block_size.y - real_block_size.y / 2.0f, i * real_block_size.z - real_block_size.z / 2.0f);
								if (abs(Vec.x) < real_block_size.x * Block_Draw_Area && abs(Vec.y) < real_block_size.y * Block_Draw_Area && abs(Vec.z) < real_block_size.z * Block_Draw_Area) {
									if (map[i][j][k] == 1) {
										Sort.push_back({ D3DXVECTOR3(k * real_block_size.x - real_block_size.x / 2.0f ,j * real_block_size.y - real_block_size.y / 2.0f,i * real_block_size.z - real_block_size.z / 2.0f) ,D3DXVECTOR2(BLOCK_SIZE, BLOCK_SIZE * 3 / 4),D3DXVECTOR2(0.0f, BLOCK_SIZE / 4.0f * 3.0f / 4.0f), Block_tex[(int)Block_Type::GRASS].pTexture });
									}
									if (map[i][j][k] == 2) {
										Sort.push_back({ D3DXVECTOR3(k * real_block_size.x - real_block_size.x / 2.0f ,j * real_block_size.y - real_block_size.y / 2.0f,i * real_block_size.z - real_block_size.z / 2.0f),D3DXVECTOR2(BLOCK_SIZE, BLOCK_SIZE * 3 / 4),D3DXVECTOR2(0.0f, BLOCK_SIZE / 4.0f * 3.0f / 4.0),Block_tex[(int)Block_Type::SOIL].pTexture });
									}
									if (map[i][j][k] == 3) {
										Sort.push_back({ D3DXVECTOR3(k * real_block_size.x - real_block_size.x / 2.0f ,j * real_block_size.y - real_block_size.y / 2.0f,i * real_block_size.z - real_block_size.z / 2.0f),D3DXVECTOR2(BLOCK_SIZE, BLOCK_SIZE * 3 / 4),D3DXVECTOR2(0.0f, BLOCK_SIZE / 4.0f * 3.0f / 4.0),Block_tex[(int)Block_Type::DARK_GRASS].pTexture });
									}
								}
							}
						}
					}
				}

				//Player=============================
				//入力取得&work_count=0
				{
					if (GetAsyncKeyState(VK_UP) && player.walk_count >= 72 / player.walk_speed && map[_Bz(player.pos.z)][_By((player.pos - real_block_size).y)][_Bx(player.pos.x)] == 0) {
						player.walk_dir = DIRECTION::UP;
						player.walk_count = 0;
					}
					if (GetAsyncKeyState(VK_DOWN) && player.walk_count >= 72 / player.walk_speed && map[_Bz(player.pos.z)][_By((player.pos + real_block_size).y)][_Bx(player.pos.x)] == 0) {
						player.walk_dir = DIRECTION::DOWN;
						player.walk_count = 0;
					}
					if (GetAsyncKeyState(VK_LEFT) && player.walk_count >= 72 / player.walk_speed && map[_Bz(player.pos.z)][_By(player.pos.y)][_Bx((player.pos - real_block_size).x)] == 0) {
						player.walk_dir = DIRECTION::LEFT;
						player.walk_count = 0;
					}
					if (GetAsyncKeyState(VK_RIGHT) && player.walk_count >= 72 / player.walk_speed && map[_Bz(player.pos.z)][_By(player.pos.y)][_Bx((player.pos + real_block_size).x)] == 0) {
						player.walk_dir = DIRECTION::RIGHT;
						player.walk_count = 0;
					}
				}

				//count等の調整
				{
					if (player.walk_count < 72 / player.walk_speed) {
						player.walk_count++;
						player.animation_count++;
						switch (player.walk_dir) {
						case DIRECTION::UP:player.pos.y -= player.walk_speed; break;
						case DIRECTION::DOWN:player.pos.y += player.walk_speed; break;
						case DIRECTION::LEFT:player.pos.x -= player.walk_speed; break;
						case DIRECTION::RIGHT:player.pos.x += player.walk_speed; break;
						}
					}
					player.animation_count++;
					if (player.animation_count >= 32) {
						player.animation_count = 0;
					}
				}

				Camera.SetPos(player.pos);//(仮)

				//ソートに追加
				Sort.push_back({ player.pos,D3DXVECTOR2(64.0f,64.0f) * zoom,D3DXVECTOR2(0.0f,-32.0f) * zoom,Chara_tex[(int)Chara_Type::PLAYER][(int)player.walk_dir][(int)player.animation_count / 16].pTexture });

				//Enemy==============================
				for (int i = 0; (unsigned)i < enemys.size(); i++) {
					switch (enemys[i].type) {
						case Chara_Type::COT: {
							if (enemys[i].walk_count < 72 / enemys[i].walk_speed) {
								enemys[i].walk_count++;
								enemys[i].animation_count++;
								switch (enemys[i].walk_dir) {
								case DIRECTION::UP:enemys[i].pos.y -= enemys[i].walk_speed; break;
								case DIRECTION::DOWN:enemys[i].pos.y += enemys[i].walk_speed; break;
								case DIRECTION::LEFT:enemys[i].pos.x -= enemys[i].walk_speed; break;
								case DIRECTION::RIGHT:enemys[i].pos.x += enemys[i].walk_speed; break;
								}
							}
							else {
								//状態の切り替え
								if (D3DXVec3Length(&(enemys[i].pos - player.pos)) < 320) {//(仮)修正必須
									if (enemys[i].statue == Enemy_Statue::SAFE) {//瞬間
										effects.push_back({ Effect_Type::WARNING,enemys[i].pos + D3DXVECTOR3(0.0f,0.0f, real_block_size.z * 3.0f) });
										enemys[i].walk_speed = 3;
										sb[1].Play(false);
									}
									enemys[i].statue = Enemy_Statue::ACTIVE;
								}
								else {
									if (enemys[i].statue == Enemy_Statue::ACTIVE) {//瞬間
										effects.push_back({ Effect_Type::QUESTION,enemys[i].pos + D3DXVECTOR3(0.0f,0.0f, real_block_size.z * 3.0f) });
										enemys[i].walk_speed = 2;
									}
									enemys[i].statue = Enemy_Statue::SAFE;
								}

								//状態別の行動
								if (enemys[i].statue == Enemy_Statue::SAFE) {
									if (rand() % (30 * 10) == 0) {
										enemys[i].go_x = unsigned(rand() % (map_x - 2)) + 1;
										enemys[i].go_y = unsigned(rand() % (map_y - 2)) + 1;
									}
								}
								else if (enemys[i].statue == Enemy_Statue::ACTIVE) {
									enemys[i].go_x = _Bx(player.pos.x);
									enemys[i].go_y = _By(player.pos.y);
								}

								//経路探索
								if (enemys[i].go_x != -1 && enemys[i].go_y != -1) {
									enemys[i].P_Node = Astar(pMap, map_x, map_y, _Bx(enemys[i].pos.x), _By(enemys[i].pos.y), enemys[i].go_x, enemys[i].go_y);
								}
								else {
									Node null = { nullptr,enemys[i].pos.x,enemys[i].pos.y,0,0,0,NODE_TYPE::OPEN };
									enemys[i].P_Node.clear();
									enemys[i].P_Node.push_back({ null });
								}


								if (abs(_Bx(player.pos.x) - _Bx(enemys[i].pos.x)) <= 1 && abs(_By(player.pos.y) - _By(enemys[i].pos.y)) <= 1) {
									if (rand() % (30 * 2) == 0) {//変更必須
										TACKLE(&effects, &numbers, sb, player.pos,real_block_size);
									}
								}

								if ((enemys[i].P_Node[0].x) - _Bx(enemys[i].pos.x) == -1) {
									enemys[i].walk_dir = DIRECTION::LEFT;
									enemys[i].walk_count = 0;
								}
								if ((enemys[i].P_Node[0].x) - _Bx(enemys[i].pos.x) == 1) {
									enemys[i].walk_dir = DIRECTION::RIGHT;
									enemys[i].walk_count = 0;
								}
								if ((enemys[i].P_Node[0].y) - _By(enemys[i].pos.y) == 1) {
									enemys[i].walk_dir = DIRECTION::DOWN;
									enemys[i].walk_count = 0;
								}
								if ((enemys[i].P_Node[0].y) - _By(enemys[i].pos.y) == -1) {
									enemys[i].walk_dir = DIRECTION::UP;
									enemys[i].walk_count = 0;
								}
							}
							enemys[i].animation_count++;
							if (enemys[i].animation_count >= 32) {
								enemys[i].animation_count = 0;
							}
							D3DXVECTOR3 Vec = enemys[i].pos - player.pos;
							if (abs(Vec.x) < real_block_size.x * Block_Draw_Area && abs(Vec.y) < real_block_size.y * Block_Draw_Area && abs(Vec.z) < real_block_size.z * Block_Draw_Area) {
								Sort.push_back({ enemys[i].pos,D3DXVECTOR2(64.0f,64.0f) * zoom,D3DXVECTOR2(0.0f,-32.0f),Chara_tex[(int)Chara_Type::COT][(int)enemys[i].walk_dir][(int)enemys[i].animation_count / 16].pTexture });
							}
						}break;
					}
				}

				//エフェクト
				for (int i = 0; i < effects.size(); i++) {
					switch (effects[i].type) {
						case Effect_Type::WARNING:{
							effects[i].count++;
							Sort.push_back({ effects[i].s_pos,(effects[i].count * (64.0f / 30.0f) * 5.0f < 64.0f ? D3DXVECTOR2(effects[i].count * (64.0f / 30.0f) * 5.0f,effects[i].count * (64.0f / 30.0f) * 5.0f) : D3DXVECTOR2(64.0f,64.0f)) * zoom,D3DXVECTOR2(0.0f,0.0f),Effect_tex[(int)Effect_Type::WARNING][0].pTexture });
							if (effects[i].count >= 30) {
								effects.erase(effects.begin() + i);
							}
						}break;
						case Effect_Type::QUESTION: {
							effects[i].count++;
							Sort.push_back({ effects[i].s_pos,(effects[i].count * (64.0f / 30.0f) * 2.0f < 64.0f ? D3DXVECTOR2(effects[i].count * (64.0f / 30.0f) * 2.0f,effects[i].count * (64.0f / 30.0f) * 2.0f) : D3DXVECTOR2(64.0f,64.0f)) * zoom,D3DXVECTOR2(0.0f,0.0f),Effect_tex[(int)Effect_Type::QUESTION][0].pTexture });
							if (effects[i].count >= 30) {
								effects.erase(effects.begin() + i);
							}
						}break;
						case Effect_Type::TACKLE: {
							effects[i].count++;
							Sort.push_back({ effects[i].s_pos,(effects[i].count * (64.0f / 30.0f) * 5.0f < 64.0f ? D3DXVECTOR2(effects[i].count * (64.0f / 30.0f) * 5.0f,effects[i].count * (64.0f / 30.0f) * 5.0f) : D3DXVECTOR2(64.0f,64.0f)) * zoom,D3DXVECTOR2(0.0f,0.0f),effects[i].count<=15? Effect_tex[(int)Effect_Type::TACKLE][0].pTexture :Effect_tex[(int)Effect_Type::TACKLE][1].pTexture });
							if (effects[i].count >= 30) {
								effects.erase(effects.begin() + i);
							}
						}break;
					}
				}

				//数字===============================
				for (int i = 0; i < numbers.size(); i++) {
					std::string str = std::to_string(numbers[i].num);
					numbers[i].count++;
					numbers[i].s_pos.z += (-numbers[i].count + 7.0f);
					for (int j = 0; j < str.length(); j++) {
						Sort.push_back({ numbers[i].s_pos,D3DXVECTOR2(35.0f,35.0f) * zoom ,D3DXVECTOR2((float(j) + 1.0f / 2.0f - float(str.length() / 2.0f)) * 35.0f * 2.0f / 3.0f ,0.0f) ,Num_tex[(int)numbers[i].type][ctoi(str[j])].pTexture });
					}
					if (numbers[i].count >= 30.0f * 0.5f) {
						numbers.erase(numbers.begin() + i);
					}
				}

				//描画系=============================
				//ソート
				sort(Sort.begin(), Sort.end(), [](const Sort_Def& x, const Sort_Def& y) { return x.pos.x + x.pos.y + x.pos.z < y.pos.x + y.pos.y + y.pos.z; });
				//ソート順に片っ端から描画
				{
					for (int i = 0; (unsigned)i < Sort.size(); i++) {
						brush.SetPos(Sort[i].pos.x* (BLOCK_SIZE / real_block_size.x) / 2.0f - Sort[i].pos.y * (BLOCK_SIZE / real_block_size.y) / 2.0f -
							Camera.pos.x * (BLOCK_SIZE / real_block_size.x) / 2.0f + Camera.pos.y * (BLOCK_SIZE / real_block_size.y) / 2.0f
							+ Sort[i].correct.x + WINDOW_WIDTH / 2.0f,

							Sort[i].pos.x * (BLOCK_SIZE / real_block_size.x) / 4.0f + Sort[i].pos.y * (BLOCK_SIZE / real_block_size.y) / 4.0f - Sort[i].pos.z * (BLOCK_SIZE / real_block_size.z)/ 4.0f -
							Camera.pos.x * (BLOCK_SIZE / real_block_size.x) / 4.0f - Camera.pos.y * (BLOCK_SIZE / real_block_size.y) / 4.0f-Camera.pos.z * (BLOCK_SIZE / real_block_size.z) / 4.0f
							+ Sort[i].correct.y + WINDOW_HEIGHT / 2.0f
							);
						brush.SetWidth(Sort[i].size.x, Sort[i].size.y);
						brush.Draw(direct3d.pDevice3D, Sort[i].image);
					}
				}
				Sort.clear();
			}
			direct3d.pDevice3D->EndScene();
			direct3d.pDevice3D->Present(NULL, NULL, NULL, NULL);
		}
	}
	return 0;
}
