# 基于FreeType,实现字体图片生成

## 1.初始化FT_Library
```cpp
FT_Library	m_library;

FT_Error result = FT_Init_FreeType(&m_library);
if(result!=FT_Err_Ok)
{
    EchoLogError("UiModule FreeType init failed.");
}
```
## 根据字体文件创建 FT_Face  
```cpp
FT_Face						m_face;
FT_Error error = FT_New_Memory_Face(library, m_memory->getData<Byte*>(), m_memory->getSize(), 0, &m_face);
if (error == FT_Err_Unknown_File_Format)
{
		EchoLogError("the font file [%s] could be opened and read, but it appears that its font format is unsupported", filePath);
}
else if (error)
{
	 EchoLogError("font file [%s] could not be opened or read, or that it is broken...", filePath);
}
```
## 加载 Glyph
```cpp
FontGlyph* FontFace::loadGlyph(i32 charCode, i32 fontSize)
 {
     // get glyph index
     i32 glyphIndex = FT_Get_Char_Index( m_face, charCode);

     // set pixel size
     FT_Error error = FT_Set_Pixel_Sizes(m_face, fontSize * 2, fontSize * 2);
     if (error)
        return nullptr;

     // load glyph
     i32 loadFlags = FT_LOAD_DEFAULT;
     error = FT_Load_Glyph( m_face, glyphIndex, loadFlags);
     if(error)
         return nullptr;

     // convert to an anti-aliased bitmap
     error = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);
     if(error)
         return nullptr;

     return copyGlyphToTexture(charCode, m_face->glyph);
 }
```

## 拷贝Glyph 到纹理
```cpp
i32 glyphWidth = 128;
i32 glyphHeight = 128;
Color glyphBitmap[128*128];
if(!copyGlyphToBitmap( glyphBitmap, glyphWidth, glyphHeight, charCode, glyphSlot))
    return nullptr;

bool FontFace::copyGlyphToBitmap(Color* oColor, i32& ioWidth, i32& ioHeight, i32 charCode, FT_GlyphSlot glyphSlot)
{
    FT_Bitmap* bitmap = &glyphSlot->bitmap;
    if(ioWidth>=bitmap->width && ioHeight>=bitmap->rows)
    {
        for(i32 w=0; w<bitmap->width; w++)
        {
            for(i32 h=0; h<bitmap->rows; h++)
            {
                i32 index = h * bitmap->width + w;
                oColor[index].r = bitmap->buffer[index];
                oColor[index].g = bitmap->buffer[index];
                oColor[index].b = bitmap->buffer[index];
                oColor[index].a = bitmap->buffer[index];
            }
        }

        ioWidth = bitmap->width;
        ioHeight = bitmap->rows;

        return true;
    }

    return false;
}
```

### 参考
[1] FreeType.[FreeType Tutorial](https://www.freetype.org/freetype2/docs/tutorial/index.html)   
[2] JimScott.[Packing Lightmaps](http://www.blackpawn.com/texts/lightmaps/default.html)
[3] https://www.freetype.org/freetype2/docs/tutorial/example1.c
