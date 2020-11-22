#pragma once

enum class NODE_TYPE {
    NONE, OPEN, CLOSED
};

struct Node {
    Node* P;
    int x;
    int y;
    int num;
    int SUITEI;
    int score;
    NODE_TYPE opened;
};

void Node_Open(Node** nodes, const int Gx, const int Gy, int x, int y, int n, Node* P) {
    if (nodes[y][x].opened != NODE_TYPE::CLOSED) {
        nodes[y][x].num = n;
        nodes[y][x].SUITEI = abs(x - Gx) + abs(y - Gy);
        nodes[y][x].score = nodes[y][x].num + nodes[y][x].SUITEI;//4way移動
        nodes[y][x].opened = NODE_TYPE::OPEN;
        nodes[y][x].P = P;
    }
}

void four_Node_Open(Node** nodes, bool** map, int map_x, int map_y, const int Gx, const int Gy, int x, int y, int n, Node* P) {
    if (x >= 1 && map[y][x - 1] != 1) {
        Node_Open(nodes, Gx, Gy, x - 1, y, n + 1, P);
    }
    if (x <= (map_x - 1) - 1 && map[y][x + 1] != 1) {
        Node_Open(nodes, Gx, Gy, x + 1, y, n + 1, P);
    }
    if (y >= 1 && map[y - 1][x] != 1) {
        Node_Open(nodes, Gx, Gy, x, y - 1, n + 1, P);
    }
    if (y <= (map_y - 1) - 1 && map[y + 1][x] != 1) {
        Node_Open(nodes, Gx, Gy, x, y + 1, n + 1, P);
    }
    nodes[y][x].opened = NODE_TYPE::CLOSED;
}

std::vector<Node> Astar(bool** map, int map_x, int map_y, const int Sx, const int Sy, const int Gx, const int Gy) {//隣同士でありながらサーチ->未定義

    std::vector<Node> Goal_Node;
    if (abs(Sx - Gx) <= 1 && abs(Sy - Gy) <= 1) {
        Goal_Node.push_back({ nullptr,Sx,Sy,0,0,0, NODE_TYPE::NONE });
        return Goal_Node;
    }

    if (map[Gy][Gx] == 1 || map[Sy][Sx] == 1) {
        Goal_Node.push_back({ nullptr,Sx,Sy,0,0,0, NODE_TYPE::NONE });
        return Goal_Node;
    }

    Node** nodes = new Node * [map_y];
    for (int i = 0; i < map_y; i++) {
        nodes[i] = new Node[map_x];
    }
    for (int i = 0; i < map_y; i++) {
        for (int j = 0; j < map_x; j++) {
            nodes[i][j] = { nullptr,j,i,0,0,0, NODE_TYPE::NONE };
        }
    }

    Node_Open(nodes, Gx, Gy, Sx, Sy, 0, nullptr);

    std::vector<Node> Node_list;
    while (1) {
        Node_list.clear();
        //Nord状態の描画
        //Open状態のNodeをNode_listにpush_backします。

        for (int i = 0; i < map_y; i++) {
            for (int j = 0; j < map_x; j++) {
                if (nodes[i][j].opened == NODE_TYPE::OPEN) {
                    Node_list.push_back(nodes[i][j]);
                }
            }
        }

        //基準Ｎｏｄｅ算出
        int min = Node_list[0].score;
        for (int i = 0; unsigned(i) < Node_list.size(); i++) {
            if (min > Node_list[i].score) {
                min = Node_list[i].score;
            }
        }

        //基準Ｎｏｄｅの4方向開封
        for (int i = 0; unsigned(i) < Node_list.size(); i++) {
            if ((Node_list[i].x - 1 == Gx && Node_list[i].y == Gy) ||
                (Node_list[i].x + 1 == Gx && Node_list[i].y == Gy) ||
                (Node_list[i].x == Gx && Node_list[i].y + 1 == Gy) ||
                (Node_list[i].x == Gx && Node_list[i].y - 1 == Gy)) {
                Node* Pointer = &nodes[Node_list[i].y][Node_list[i].x];
                while (1) {
                    Goal_Node.insert(Goal_Node.begin(), *Pointer);
                    Pointer = Pointer->P;
                    if (Pointer->P == nullptr) {
                        for (int i = 0; i < map_y; i++) {
                            delete[] nodes[i];
                        }
                        delete[] nodes;
                        return Goal_Node;
                    }
                }
            }

            if (Node_list[i].score == min) {
                four_Node_Open(nodes, map, map_x, map_y, Gx, Gy, Node_list[i].x, Node_list[i].y, Node_list[i].num, &nodes[Node_list[i].y][Node_list[i].x]);
            }
        }
    }
}