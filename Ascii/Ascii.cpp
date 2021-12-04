#include "pch.h"

#include "Ascii.h"

#include <string>
#include <set>
#include <map>
#include <memory>
#include <functional>
#include <filesystem>
#include <bitset>
#include <numeric>
#include <msclr/marshal_cppstd.h>
#include <FreeImagePlus.h>

#define generic GenericFromFreeTypeLibrary
#include <ft2build.h>
#include FT_FREETYPE_H
#undef generic

using namespace Ascii;
using namespace msclr::interop;
using namespace System;

void FreeType::Init()
{
	fipImage img; // TODO: rem
	/*
	"D:\Dev\Home\AsciiArt\Ascii\freeimage\Dist\x64\FreeImaged.dll"
	D:\Dev\Home\AsciiArt\WpfApp1\bin\Debug\net6.0-windows
	*/

	std::unique_ptr<FT_Library, std::function<void(FT_Library*)>> library(new FT_Library(), [](auto* p) { FT_Done_FreeType(*p); });
	std::multimap<unsigned int, std::tuple<std::string, std::string, std::string>> sorted;
	constexpr unsigned int widthThreshold = 16;
	const std::string chars = []()
	{
		std::string result;

		for (char c = ' '; c <= '~'; c++)
		{
			result += c;
		}

		return result;
	}();

	FT_Init_FreeType(library.get());
	fonts = gcnew Dictionary<String^, List<String^>^>();
	files = gcnew Dictionary<Tuple<String^, String^>^, String^>();

	for (const auto& file : std::filesystem::directory_iterator{ "C:\\Windows\\Fonts" })
	{
		std::unique_ptr<FT_Face, std::function<void(FT_Face*)>> face(new FT_Face(), [](auto* p) { FT_Done_Face(*p); });

		if (FT_New_Face(*library.get(), file.path().string().c_str(), 0, face.get()) == FT_Err_Ok)
		{
			auto key = std::make_tuple(std::string((*face)->family_name), std::string((*face)->style_name), file.path().string());
			std::set<unsigned int> widths;

			FT_Set_Pixel_Sizes(*face.get(), 32, 32);

			for (const auto& c : chars)
			{
				if (auto glyphIndex = FT_Get_Char_Index(*face.get(), (FT_ULong)c))
				{
					FT_Load_Glyph(*face.get(), glyphIndex, FT_LOAD_DEFAULT);
					FT_GlyphSlot slot = (*face)->glyph;
					FT_Render_Glyph((*face)->glyph, FT_RENDER_MODE_MONO);
					widths.insert(slot->bitmap.width);
				}
			}

			sorted.emplace(widths.size(), key);
		}
	}

	for (auto& i : sorted)
	{
		if (i.first > widthThreshold)
		{
			break;
		}

		auto family = marshal_as<String^>(std::get<0>(i.second));
		auto style = marshal_as<String^>(std::get<1>(i.second));

		if (!fonts->ContainsKey(family))
		{
			fonts->Add(family, gcnew List<String^>());
		}

		if (!fonts[family]->Contains(style))
		{
			fonts[family]->Add(style);
			files->Add(gcnew Tuple<String^, String^>(family, style), marshal_as<String^>(std::get<2>(i.second)));
		}
	}
}

Dictionary<String^, List<String^>^>^ FreeType::FixedWidthFonts::get()
{
	if (!fonts)
	{
		Init();
	}

	return fonts;
}