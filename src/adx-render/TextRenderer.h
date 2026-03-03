#pragma once
#include "framework.h"
#include "../adx-render/Window.h"
#include <array>
#include <string>

struct FontCharacter
{
    float width, height; // Dimensions en "unités monde"
    float posx, posy; // position dans la grille (col,row) — stocké en tant que indices flottants
};

class TextRenderer
{
public:
    // Construire avec la fenêtre (utilisée pour BuildMesh/Draw)
    TextRenderer(Window* window);
    ~TextRenderer();

    // Initialise la sprite sheet.
    // - spriteFile : chemin vers la texture (wstring)
    // - cols / rows : nombre de colonnes/lines dans la sprite sheet
    // - glyphWidthWorld / glyphHeightWorld : taille d'un glyph en unités monde (pour positionnement)
    // - startChar : code du premier caractère dans la grille (par défaut espace 32)
    bool Initialize(const wchar_t* spriteFile, int cols, int rows, float glyphWidthWorld = 1.0f, float glyphHeightWorld = 1.0f, unsigned char startChar = 32);

    // Dessine le texte à la position (x,y) en unités monde ; scale multiplie les dimensions définies par glyphWidthWorld / glyphHeightWorld.
    void DrawText(const std::string& text, float x, float y, float scale = 1.0f);

private:
    bool mInitialized = false;
    Window* mWindow = nullptr;
    std::wstring mSpriteFile;
    int mCols = 0;
    int mRows = 0;
    float mGlyphWidthWorld = 1.0f;
    float mGlyphHeightWorld = 1.0f;
    unsigned char mStartChar = 32;
    std::array<FontCharacter, 256> mChars;
    int mNextMeshIndex = 0;

    // index réservé pour enregistrer la texture UNE FOIS dans Window::BuildMesh
    int mTextureRegisterIndex = 100000; // valeur arbitraire improbable d'ID entité
};
