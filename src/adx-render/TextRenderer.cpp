#include "pch.h"
#include "TextRenderer.h"

#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

using namespace DirectX;

struct TextVertex
{
    XMFLOAT2 TexCoord;
};

TextRenderer::TextRenderer(Window* window)
    : mWindow(window)
{
}

TextRenderer::~TextRenderer()
{
}

bool TextRenderer::Initialize(const wchar_t* spriteFile, int cols, int rows, float glyphWidthWorld, float glyphHeightWorld, unsigned char startChar)
{
    if (mInitialized)
        return true;

    if (!mWindow)
        return false;

    if (cols <= 0 || rows <= 0)
        return false;

    mCols = cols;
    mRows = rows;
    mStartChar = startChar;
    mSpriteFile = spriteFile ? std::wstring(spriteFile) : std::wstring();

    // Lecture de la taille de l'image via WIC (optionnel, inchangé)
    UINT imageWidth = 0;
    UINT imageHeight = 0;
    if (!mSpriteFile.empty())
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        bool comInited = SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE;
        IWICImagingFactory* pFactory = nullptr;
        if (SUCCEEDED(hr))
            hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));

        if (pFactory)
        {
            IWICBitmapDecoder* pDecoder = nullptr;
            hr = pFactory->CreateDecoderFromFilename(mSpriteFile.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
            if (SUCCEEDED(hr) && pDecoder)
            {
                IWICBitmapFrameDecode* pFrame = nullptr;
                hr = pDecoder->GetFrame(0, &pFrame);
                if (SUCCEEDED(hr) && pFrame)
                {
                    pFrame->GetSize(&imageWidth, &imageHeight);
                    pFrame->Release();
                }
                pDecoder->Release();
            }
            pFactory->Release();
        }

        if (comInited)
            CoUninitialize();
    }

    // Calcul taille glyph en pixels / monde (le reste inchangé)
    int mGlyphWidthPx = 0;
    int mGlyphHeightPx = 0;
    if (imageWidth > 0 && imageHeight > 0)
    {
        mGlyphWidthPx = static_cast<int>(imageWidth / mCols);
        mGlyphHeightPx = static_cast<int>(imageHeight / mRows);
    }

    if (glyphWidthWorld <= 0.0f && mGlyphWidthPx > 0)
        mGlyphWidthWorld = static_cast<float>(mGlyphWidthPx);
    else
        mGlyphWidthWorld = glyphWidthWorld;

    if (glyphHeightWorld <= 0.0f && mGlyphHeightPx > 0)
        mGlyphHeightWorld = static_cast<float>(mGlyphHeightPx);
    else
        mGlyphHeightWorld = glyphHeightWorld;

    for (int i = 0; i < 256; ++i)
    {
        mChars[i].width = mGlyphWidthWorld;
        mChars[i].height = mGlyphHeightWorld;
        int idx = i - (int)mStartChar;
        if (idx < 0 || idx >= mCols * mRows)
        {
            mChars[i].posx = 0.f;
            mChars[i].posy = 0.f;
        }
        else
        {
            int col = idx % mCols;
            int row = idx / mCols;
            mChars[i].posx = static_cast<float>(col);
            mChars[i].posy = static_cast<float>(row);
        }
    }

    // IMPORTANT : enregistrer la texture UNE SEULE FOIS dans Window (BuildMesh) pour éviter de la recharger à chaque frame.
    // On appelle BuildMesh avec des vecteurs vides uniquement pour que Window charge la texture et crée le SRV.
    if (!mSpriteFile.empty() && mWindow)
    {
        std::vector<Vertex> dummyV;
        std::vector<std::uint32_t> dummyI;
        // use the reserved index to register texture once
        mWindow->BuildMesh(dummyV, dummyI, mTextureRegisterIndex, mSpriteFile.c_str());
        // après cet appel, Window a créé le SRV et stocké l'association index->texture interne
    }

    mInitialized = true;
    return true;
}

void TextRenderer::DrawText(const std::string& text, float x, float y, float scale)
{
    if (!mInitialized || !mWindow)
        return;

    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
    vertices.reserve(text.size() * 4);
    indices.reserve(text.size() * 6);

    float cursorX = x;
    float cursorY = y;

    float uCell = 1.0f / static_cast<float>(mCols);
    float vCell = 1.0f / static_cast<float>(mRows);

    for (size_t i = 0; i < text.size(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(text[i]);
        const FontCharacter& fc = mChars[c];

        float glyphW = fc.width * scale;
        float glyphH = fc.height * scale;

        float u0 = (fc.posx) * uCell;
        float v0 = (fc.posy) * vCell;
        float u1 = u0 + uCell;
        float v1 = v0 + vCell;

        Vertex v0v = { XMFLOAT3(cursorX, cursorY + glyphH, 0.0f), XMFLOAT4(1,1,1,1), XMFLOAT3(0,1,0), XMFLOAT2(u0, v0) };
        Vertex v1v = { XMFLOAT3(cursorX, cursorY, 0.0f),       XMFLOAT4(1,1,1,1), XMFLOAT3(0,1,0), XMFLOAT2(u0, v1) };
        Vertex v2v = { XMFLOAT3(cursorX + glyphW, cursorY, 0.0f), XMFLOAT4(1,1,1,1), XMFLOAT3(0,1,0), XMFLOAT2(u1, v1) };
        Vertex v3v = { XMFLOAT3(cursorX + glyphW, cursorY + glyphH, 0.0f), XMFLOAT4(1,1,1,1), XMFLOAT3(0,1,0), XMFLOAT2(u1, v0) };

        uint16_t baseIndex = static_cast<uint16_t>(vertices.size());
        vertices.push_back(v0v);
        vertices.push_back(v1v);
        vertices.push_back(v2v);
        vertices.push_back(v3v);

        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);

        cursorX += glyphW;
    }

    // IMPORTANT: Ne PAS passer mSpriteFile.c_str() ici — la texture est déjà enregistrée.
    int meshIndex = mNextMeshIndex++;
    MeshGeometry mesh = mWindow->BuildMesh(vertices, indices, meshIndex, nullptr);

    // Assure-toi que Window sait utiliser la texture liée au mTextureRegisterIndex.
    // Selon l'implémentation Window::Draw utilise l'index du mesh pour chercher la texture. 
    // Si nécessaire, modifie Window::Draw pour référencer la texture enregistrée via mTextureRegisterIndex.
    mWindow->Draw(mesh, meshIndex);
}