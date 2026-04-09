#include "pch.h"
#include "TextRenderer.h"
#include "adx-shooter/GameManager.h"

TextRenderer::~TextRenderer() {}

bool TextRenderer::Initialize() {
    if (mInitialized) return true;
    mCols = 15;
    mRows = 8;
    mGlyphHeightWorld = 1;
    mStartChar = 32;
    mSpriteFile = L"sheet.dds";
	mMeshIndex = 1;

    for (int i = 0; i < 256; ++i) {
        mChars[i].width = 1;
        mChars[i].height = 1;

        int idx = i - static_cast<int>(mStartChar);
        if (idx < 0 || idx >= mCols * mRows) {
            mChars[i].posx = 0.f;
            mChars[i].posy = 0.f;
        }
        else {
            mChars[i].posx = static_cast<float>(idx % mCols);
            mChars[i].posy = static_cast<float>(idx / mCols);
        }
    }
    mMesh = GameManager::GetInstance().GetWindow()->CreateDynamicMesh(mMeshIndex, MAX_CHARS * 4, MAX_CHARS * 6);

    std::vector<Vertex> dummyV;
    std::vector<uint32_t> dummyI;
    std::wstring fullPath = L"..\\..\\res\\Textures\\" + mSpriteFile;
    GameManager::GetInstance().GetWindow()->BuildMesh(dummyV, dummyI, fullPath.c_str());
    GameManager::GetInstance().GetWindow()->SetTextureToIndex({ mMeshIndex }, fullPath.c_str());
    mInitialized = true;
    return true;
}

void TextRenderer::DrawTxt(const std::string& text, float x, float y, float scale) {
    if (!mInitialized || text.empty()) return;

    std::vector<Vertex> vertices;
    vertices.reserve(text.size() * 4);

    float cursorX = x;
    float cursorY = y;

    const float uCell = 1.0f / static_cast<float>(mCols);
    const float vCell = 1.0f / static_cast<float>(mRows);

    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);

        if (c == '\n') {
            cursorX = x;
            cursorY += mGlyphHeightWorld * scale; // Y descend en UI
            continue;
        }

        const FontCharacter& fc = mChars[c];
        float gw = fc.width * scale;
        float gh = fc.height * scale;

        float u0 = fc.posx * uCell;
        float v0 = fc.posy * vCell;
        float u1 = u0 + uCell;
        float v1 = v0 + vCell;

        vertices.push_back({ XMFLOAT3(cursorX, cursorY + gh, 0.f),      XMFLOAT4(1,1,1,1), XMFLOAT3(0,0,-1), XMFLOAT2(u0, v1) });
        vertices.push_back({ XMFLOAT3(cursorX, cursorY, 0.f),           XMFLOAT4(1,1,1,1), XMFLOAT3(0,0,-1), XMFLOAT2(u0, v0) });
        vertices.push_back({ XMFLOAT3(cursorX + gw, cursorY, 0.f),      XMFLOAT4(1,1,1,1), XMFLOAT3(0,0,-1), XMFLOAT2(u1, v0) });
        vertices.push_back({ XMFLOAT3(cursorX + gw, cursorY + gh, 0.f), XMFLOAT4(1,1,1,1), XMFLOAT3(0,0,-1), XMFLOAT2(u1, v1) });

        cursorX += gw;

        if (vertices.size() / 4 >= (size_t)MAX_CHARS) break;
    }

    void* mappedData = nullptr;
    HRESULT hr = mMesh.VertexBufferGPU->Map(0, nullptr, &mappedData);
    if (SUCCEEDED(hr)) {
        memcpy(mappedData, vertices.data(), vertices.size() * sizeof(Vertex));
        mMesh.VertexBufferGPU->Unmap(0, nullptr);
    }

    mMesh.DrawArgs["box"].IndexCount = static_cast<UINT>(vertices.size() / 4 * 6);

    GameManager::GetInstance().GetWindow()->UpdateUI(mMeshIndex, XMMatrixIdentity());
    GameManager::GetInstance().GetWindow()->DrawUI(mMesh, mMeshIndex);
}