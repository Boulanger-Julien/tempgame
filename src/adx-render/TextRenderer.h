#pragma once
#include "pch.h"
#include "../adx-render/Window.h"
#include <array>
#include <string>

struct FontCharacter
{
	float width, height; // Dimensions in world units
    float posx, posy; // pos on grid (col,row)
};

class TextRenderer
{
public:
    // Rendering text using a sprite-shee
    // Each glyph is a cell in a grid (cols x rows) inside a single texture
    // This class builds quads (4 vertices, 6 indices) per glyph
    // And Window renders as a mesh
    TextRenderer() { Initialize(); }
    ~TextRenderer();

    // - spriteFile : path to texture (wstring)
    // - cols / rows : number of colomns/lines dans in  sprite sheet
    // - glyphWidthWorld / glyphHeightWorld : size of a glyph in world units
    // - startChar : code of first character in grid (default space 32)
    bool Initialize();

    // Draws texte at pos(x,y) in world units; scale multiplies dimensions definined by glyphWidthWorld / glyphHeightWorld
    // text: the ASCII/UTF-8 string
    // x, y: starting position in world units
    // scale: scale factor applied to glyph world size
    void DrawTxt(const std::string& text, float x, float y, float scale = 1.0f);

private:
    bool mInitialized = false;
    std::wstring mSpriteFile;
    int mCols = 0;
    int mRows = 0;
    float mGlyphWidthWorld = 1.0f;
    float mGlyphHeightWorld = 1.0f;
    unsigned char mStartChar = 32;
    std::array<FontCharacter, 256> mChars;
    int mNextMeshIndex = 0;

    int mTextureRegisterIndex = 1000;
    static constexpr int MAX_CHARS = 256;

    int mMeshIndex = 0;
    MeshGeometry mMesh;

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;
};
