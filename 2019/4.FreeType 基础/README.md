# FreeType 基础
## 介绍
FreeType是一款用于字体渲染的的软件库，其基于C语言实现。具有免费、开源、跨平台、小巧、高效、高自定义性等特点。 
### 广泛应用
在我们日常接触的各种应用与操作系统中，几乎所有的字体渲染都由FreeType来提供底层支持。全球有数以十亿计的设备正在使用FreeType。

|使用FreeType的软件与操作系统|
|---|
|[GNU/Linux](https://www.gnu.org/gnu/why-gnu-linux.html) and other free Unix operating system derivates like FreeBSD or NetBSD;|
|[iOS](https://www.apple.com/ios/ios-12/), Apple's mobile operating system for iPhones and iPads;|
|[Android](https://www.android.com/), Google's operating system for smartphones and tablet computers;|
|[ChromeOS](https://www.chromium.org/chromium-os), Google's operating system for laptop computers;|
|[ReactOS](https://reactos.org/), a free open source operating system based on the best design principles found in the Windows NT architecture;|
|[Ghostscript](https://www.ghostscript.com/), a PostScript interpreter used in many printers.|

### 丰富的文件格式支持
FreeType使用统一的接口实现对不同字体格式文件的访问，其支持的格式如下表所示。

|序号|FreeType支持字体|
|---|---|
|1|TrueType fonts (TTF) and TrueType collections (TTC)|
|2|CFF fonts  | 
|3|WOFF fonts   |
|4|OpenType fonts (OTF, both TrueType and CFF variants) and OpenType collections (OTC)   |
|5|Type 1 fonts (PFA and PFB)   |
|6|CID-keyed Type 1 fonts   |
|7|SFNT-based bitmap fonts, including color Emoji |  
|8|X11 PCF fonts   |
|9|Windows FNT fonts   |
|10|BDF fonts (including anti-aliased ones)   |
|11|PFR fonts   |
|12|Type 42 fonts (limited support)   |

## 示例开发
下面我们用一个简单的示例来一步步讲解FreeType的基本概念及API调用方式。

### 目标
本文我们的目标是把Unicode字符串"123 一曲相思" 渲染到屏幕; 首先确定输入与输出。
#### 输入
 Unicode字符串、字体文件、字体大小   
#### 输出
 对应字符的Bitmap   
![](https://raw.githubusercontent.com/timi-liuliang/writing/master/2019/4.FreeType%20%E5%9F%BA%E7%A1%80/input_output.png)

### 编译
我们可以从[www.freetype.org](https://www.freetype.org/)官网下载最新版的FreeType源码，目前的最新版本为2.10.0    

[FreeType 2.10.0](https://download.savannah.gnu.org/releases/freetype/)   

关于FreeType的编译配置可以参考下面这个CMakeLists.txt, FreeType这种C库都是比较容易编译的，CMake不需要对不同平台做不同处理。且接口稳定，从Version 2.6.0 到现在的 Version 2.10.0, CMake未改动一行，仍然可以完美编译。   

[CMakeLists.txt](https://github.com/timi-liuliang/echo/blob/master/thirdparty/freetype-2.10.0/CMakeLists.txt)   

### 包含头文件
首先要包含FreeType相关头文件，分别是 <ft2build.h> 和 FT_FREETYPE_H。 其中 FT_FREETYPE_H 等于 <freetype/freetype.h>。
```cpp
#include <ft2build.h>
#include FT_FREETYPE_H
```

### 初始化FT_Library
```cpp
FT_Library	m_library;

FT_Error result = FT_Init_FreeType(&m_library);
if(result!=FT_Err_Ok)
{
    EchoLogError("UiModule FreeType init failed.");
}
```

### 根据字体文件创建 FT_Face  
```cpp
FT_Face						m_face;
FT_Error error = FT_New_Memory_Face(library, m_memory->getData<Byte*>(), m_memory->getSize(), 0, &m_face);
if (error)
{
  EchoLogError("font file [%s] could not be opened or read, or that it is broken...", filePath);
}
```
### CharCode
```cpp
for(wchar_t c : L"ABC一曲相思")
{
    FT_ULong charCode = c;
    EchoLogError("%ld", charCode);
}
```

### 根据 charCode 加载 Glyph
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

### 拷贝Glyph 到纹理
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

## 扩展阅读   
### [装箱算法](http://www.blackpawn.com/texts/lightmaps/default.html)
### [Distance Field Fonts](https://github.com/libgdx/libgdx/wiki/Distance-field-fonts)

## 参考
[1] FreeType.[FreeType Tutorial](https://www.freetype.org/freetype2/docs/tutorial/index.html)   
[2] JimScott().[Packing Lightmaps](http://www.blackpawn.com/texts/lightmaps/default.html)   
[3] FreeType.[example1]https://www.freetype.org/freetype2/docs/tutorial/example1.c   
