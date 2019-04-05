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
FT_Library 代表 FreeType库的一个实例，通过调用FT_Init_FreeType函数来实例化它。应用在运行期间只需创建一个FT_Library实例，类似于Lua中的luaState*。   
```cpp
FT_Library	m_library;

FT_Error result = FT_Init_FreeType(&m_library);
if(result!=FT_Err_Ok)
{
    EchoLogError("UiModule FreeType init failed.");
}
```

### 根据字体文件创建 FT_Face  
每一个FT_Face对应一个字体文件，可以通过FT_New_Memory_Face 或 FT_New_Face 来创建。纠结内存占用的同学，可以使用内存映射的方式，避免字体文件占用内存。
```cpp
FT_Face m_face;
FT_Error error = FT_New_Memory_Face(library, m_memory->getData<Byte*>(), m_memory->getSize(), 0, &m_face);
if (error)
{
  EchoLogError("font file [%s] could not be opened or read, or that it is broken...", filePath);
}
```
### 获取字符编码 CharCode
前面我们已经初始了化了FreeType库，加载了字体文件 Dragons.ttf。接下来要通过字符的CharCode从 FT_Face中获取字符信息，首先我们需要通过字符串遍历，获取字符串中每个字符的CharCode。    
```cpp
for(wchar_t c : L"ABC一曲相思")
{
    FT_ULong charCode = c;
    EchoLogError("%ld", charCode);
}
```

如果字符串并非utf16格式，一般需要进行字符串格式转换，以utf8 转 utf16 为例：

```cpp
// utf16 to utf8
String StringUtil::WCS2MBS(const WString &str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.to_bytes(str);
}

// utf8 to utf16
WString StringUtil::MBS2WCS(const String& str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(str);
}
```

### 根据 CharCode 加载 Glyph
获取了CharCode 以及 FT_Face 实例，现在就可以加载字符了。加载的字符位于FT_Face的 glyph变量中，其类型为 FT_Glyph_Slot;
```cpp
FontGlyph* FontFace::loadGlyph(i32 charCode, i32 fontSize)
 {
     // get glyph index
     i32 glyphIndex = FT_Get_Char_Index( m_face, charCode);

     // set pixel size
     FT_Error error = FT_Set_Pixel_Sizes(m_face, fontSize, fontSize);
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

     return copyGlyphToTexture(m_face->glyph);
 }
```

### 拷贝Glyph 到纹理
通过FT_GlyphSlot 及其Format就可获取其对应的Bitmap数据了。此处假定bitmap->pixel_mode 格式为 FT_PIXEL_MODE_GRAY    
```cpp
    void FontFace::copyGlyphToBitmap(FT_GlyphSlot glyphSlot)
    {
        FT_Bitmap* bitmap = &glyphSlot->bitmap;
        
        // convert glyph to bitmap(color array)
        i32 glyphWidth = bitmap->width;
        i32 glyphHeight = bitmap->rows;
        vector<Color>::type Colors(glyphWidth*glyphHeight);
        
        for(i32 w=0; w<bitmap->width; w++)
        {
            for(i32 h=0; h<bitmap->rows; h++)
            {
                i32 index = h * bitmap->width + w;
                Colors[index].r = bitmap->buffer[index];
                Colors[index].g = bitmap->buffer[index];
                Colors[index].b = bitmap->buffer[index];
                Colors[index].a = bitmap->buffer[index];
            }
        }
    }
```

## 扩展阅读   
### [装箱算法](http://www.blackpawn.com/texts/lightmaps/default.html)
在应用执行文本的渲染时，为了减少渲染批次。需要把每个字符对应的bitmap合并成一张较大的bitmap, 一般叫做AtlasTexture。该算法在特效纹理合并以及光照图纹理合并中也较常用。
### [Distance Field Fonts](https://github.com/libgdx/libgdx/wiki/Distance-field-fonts)
基于Bitmap的字符纹理在处理较大字体时对纹理大小要求较高，或者容易产生失真。且对轮廓及字体阴影的支持不优雅，Distance Field Fonts很好的解决了这些问题。

## 参考
[1] FreeType.[FreeType Tutorial](https://www.freetype.org/freetype2/docs/tutorial/index.html)   
[2] JimScott.[Packing Lightmaps](http://www.blackpawn.com/texts/lightmaps/default.html)   
[3] FreeType.[example1](https://www.freetype.org/freetype2/docs/tutorial/example1.c)   
