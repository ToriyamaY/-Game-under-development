#pragma once

void TACKLE(std::vector<Effect>* effects, std::vector<Number>* numbers, SoundBuffer* sb, D3DXVECTOR3 pos, D3DXVECTOR3 real_block_size) {
	effects->push_back({ Effect_Type::TACKLE,pos + D3DXVECTOR3(0.0f ,0.0f , real_block_size.z / 2.0f) });
	if (rand() % 10 == 0) {
		numbers->push_back({ rand() % 3 + 3 ,pos + D3DXVECTOR3(0.0f,0.0f,real_block_size.z * 2.0f),Num_Type::CRITICAL });
	}
	else {
		numbers->push_back({ rand() % 3 + 1 ,pos + D3DXVECTOR3(0.0f,0.0f,real_block_size.z * 2.0f),Num_Type::NORMAL });
	}

	sb[0].Play(false);
}