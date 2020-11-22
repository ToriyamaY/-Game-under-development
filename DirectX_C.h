#pragma once

class Direct3D {
public:
	IDirect3D9* pD3D9;
	IDirect3DDevice9* pDevice3D;
	Direct3D();
	~Direct3D();
	bool CreateDirect3D(HWND hWnd, int Width, int Height);
};

Direct3D::Direct3D() {
	pD3D9 = NULL;
	pDevice3D = NULL;
}

Direct3D::~Direct3D() {
	if (pDevice3D != NULL) {
		pDevice3D->Release();
	}
	if (pD3D9 != NULL) {
		pD3D9->Release();
	}
}

bool Direct3D::CreateDirect3D(HWND hWnd, int Width, int Height) {
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	D3DDISPLAYMODE Display;
	pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Display);
	D3DPRESENT_PARAMETERS D3DParam = { Width,Height,Display.Format,1,D3DMULTISAMPLE_NONE,0,D3DSWAPEFFECT_DISCARD,hWnd,TRUE,TRUE,D3DFMT_D24S8,0,0,D3DPRESENT_INTERVAL_DEFAULT };
	if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DParam.hDeviceWindow,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
		if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DParam.hDeviceWindow,
			D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
			if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, D3DParam.hDeviceWindow,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
				if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, D3DParam.hDeviceWindow,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D))) {
					pD3D9->Release();
					return false;
				}

	pDevice3D->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice3D->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice3D->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice3D->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	pDevice3D->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDevice3D->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	pDevice3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice3D->SetRenderState(D3DRS_BLENDFACTOR, (DWORD)0x888888888);
	return true;
}

class Texture {
public:
	IDirect3DTexture9* pTexture;
	Texture();
	~Texture();
	bool Load(IDirect3DDevice9* pDevice3D, const wchar_t* Filename);
};

Texture::Texture() {
	pTexture = NULL;
}

Texture::~Texture() {
	if (pTexture != NULL) {
		pTexture->Release();
	}
}

bool Texture::Load(IDirect3DDevice9* pDevice3D, const wchar_t* Filename) {
	HRESULT             hRet;
	D3DXIMAGE_INFO      iinfo;
	hRet = D3DXGetImageInfoFromFile(Filename, &iinfo);
	if (hRet == D3D_OK){
		if (FAILED(hRet = D3DXCreateTextureFromFileEx(
			pDevice3D,
			Filename,
			iinfo.Width, iinfo.Height, //★取得した画像ファイルのサイズを指定する
			1, 0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			D3DX_FILTER_NONE,
			D3DX_FILTER_NONE,
			0x00000000,
			NULL, NULL,
			&pTexture))) {
			return false;
		}
	}
	return true;
}

struct Vertex {
	float x, y, z;
	float  rhw;
	float u, v;
};

class Sprite {
public:
	static const DWORD SPRITE_FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
	D3DXVECTOR2 pos;
	int width;
	int height;
	Sprite();
	~Sprite();
	void SetPos(float x, float y);
	void SetWidth(int Width, int Height);
	void Draw(IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture, bool);
};

Sprite::Sprite() {
	pos.x = 0.0f;
	pos.y = 0.0f;
	width = 0;
	height = 0;
}

Sprite::~Sprite() {

}

void Sprite::SetPos(float x, float y) {
	pos.x = x;
	pos.y = y;
}

void Sprite::SetWidth(int Width, int Height) {
	width = Width;
	height = Height;
}

void Sprite::Draw(IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture, bool isTurn = false) {
	Vertex vtx[4] = {
		{ pos.x + width / 2 , pos.y - height / 2, 0.0f, 1.0f,(isTurn ? 0.0f : 1.0f), 0.0f},
		{ pos.x + width / 2, pos.y + height / 2, 0.0f, 1.0f,(isTurn ? 0.0f : 1.0f), 1.0f},
		{ pos.x - width / 2, pos.y - height / 2, 0.0f, 1.0f,(isTurn ? 1.0f : 0.0f), 0.0f},
		{ pos.x - width / 2, pos.y + height / 2, 0.0f, 1.0f,(isTurn ? 1.0f : 0.0f), 1.0f},
	};
	D3DXMATRIXA16 a;
	pDevice3D->SetTexture(0, pTexture);
	D3DXMatrixScaling(&a, 5.0f, 0.0f, 0.0f);
	pDevice3D->SetTransform(D3DTS_WORLD, &a);
	pDevice3D->SetFVF(SPRITE_FVF);
	pDevice3D->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtx, sizeof(Vertex));
}

class DirectSound{
public:
	IDirectSound8* pDirectSound8; 
	DirectSound();
	~DirectSound();

	bool Create(HWND hWnd);
};

DirectSound::DirectSound(){
	pDirectSound8 = NULL;	// DirectSoundデバイス
}

DirectSound::~DirectSound(){
	if (pDirectSound8 != NULL)
		pDirectSound8->Release();
}

bool DirectSound::Create(HWND hWnd){
	DirectSoundCreate8(NULL, &pDirectSound8, NULL);
	if (FAILED(pDirectSound8->SetCooperativeLevel(hWnd, DSSCL_PRIORITY))){
		return false;
	}

	return true;
}

class Wave {
public:
	WAVEFORMATEX WaveFormat;
	byte* WaveData;
	int DataSize;

	Wave();
	~Wave();

	bool Load(const wchar_t* FileName);
};

Wave::Wave(){
	memset(&WaveFormat, 0, sizeof(WAVEFORMATEX));
	WaveData = NULL;
	DataSize = 0;
}

Wave::~Wave(){
	if (WaveData != NULL)
		delete[] WaveData;
}

bool Wave::Load(const wchar_t* FileName){
	FILE* fp;
	if (!(fp = ::_tfopen(FileName, _T("rb"))))
		return false;

	char chunkId[5] = {};
	char tmp[5] = {};
	unsigned int chunkSize = 0;

	// RIFFチャンク読み込み
	fread(chunkId, sizeof(char) * 4, 1, fp);
	fread(&chunkSize, sizeof(unsigned int), 1, fp);
	fread(tmp, sizeof(char) * 4, 1, fp);
	if (strcmp(chunkId, "RIFF") || strcmp(tmp, "WAVE")) {
		return false;	// Waveファイルじゃない
	}

	// 子チャンク読み込み
	bool fmtchunk = false;
	bool datachunk = false;
	while (true)
	{
		fread(chunkId, sizeof(char) * 4, 1, fp);
		fread(&chunkSize, sizeof(unsigned int), 1, fp);
		if (!strcmp(chunkId, "fmt "))
		{
			if (chunkSize >= sizeof(WAVEFORMATEX))
			{
				fread(&WaveFormat, sizeof(WAVEFORMATEX), 1, fp);
				int diff = chunkSize - sizeof(WAVEFORMATEX);
				fseek(fp, diff, SEEK_CUR);
			}
			else
			{
				memset(&WaveFormat, 0, sizeof(WAVEFORMATEX));
				fread(&WaveFormat, chunkSize, 1, fp);
			}
			fmtchunk = true;
		}else if (!strcmp(chunkId, "data")){
			DataSize = chunkSize;
			WaveData = new byte[chunkSize];
			if (fread(WaveData, sizeof(byte), chunkSize, fp) != chunkSize){
				fclose(fp);
				return false;
			}
			datachunk = true;
		}
		else {
			fseek(fp, chunkSize, SEEK_CUR);
		}

		if (fmtchunk && datachunk)
			break;
	}

	fclose(fp);
	return true;
}

class SoundBuffer{
public:
	IDirectSoundBuffer8* pSecondaryBuffer;

	SoundBuffer();
	~SoundBuffer();

	bool Create(IDirectSound8* pDirectSound8, WAVEFORMATEX& WaveFormat, byte* WaveData, int DataSize);

	void Play(bool isLoop);
	void Stop();
};

SoundBuffer::SoundBuffer(){
	pSecondaryBuffer = NULL;
}

SoundBuffer::~SoundBuffer(){
	Stop();

	if (pSecondaryBuffer != NULL)
		pSecondaryBuffer->Release();
}

bool SoundBuffer::Create(IDirectSound8* pDirectSound8, WAVEFORMATEX& WaveFormat, byte* WaveData, int DataSize){

	DSBUFFERDESC desc = {};
	if (WaveFormat.nChannels == 1) {
		desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
			DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_STATIC;
		desc.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
	}
	else {
		desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
			DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLPAN | DSBCAPS_STATIC;
		desc.guid3DAlgorithm = GUID_NULL;
	}
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwBufferBytes = DataSize;
	desc.lpwfxFormat = &WaveFormat;

	IDirectSoundBuffer* pPrimaryBuffer = NULL;
	pDirectSound8->CreateSoundBuffer(&desc, &pPrimaryBuffer, NULL);
	pPrimaryBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&pSecondaryBuffer);
	pPrimaryBuffer->Release();

	unsigned char* block1 = NULL;
	unsigned char* block2 = NULL;
	unsigned long blockSize1 = 0;
	unsigned long blockSize2 = 0;
	pSecondaryBuffer->Lock(0, DataSize,(void**)&block1, &blockSize1, (void**)&block2, &blockSize2, DSBLOCK_ENTIREBUFFER);
	memcpy(block1, WaveData, DataSize);
	pSecondaryBuffer->Unlock(block1, blockSize1, block2, 0);

	return true;
}

void SoundBuffer::Play(bool isLoop){
	if (pSecondaryBuffer != NULL) {
		DWORD LoopFlag = (isLoop ? DSBPLAY_LOOPING : 0);
		pSecondaryBuffer->Play(0, 0, LoopFlag);
	}
}
void SoundBuffer::Stop(){
	if (pSecondaryBuffer != NULL)
		pSecondaryBuffer->Stop();
}