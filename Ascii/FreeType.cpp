#include "pch.h"

#include "FreeType.h"
#include "Generator.h"

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

using namespace Ascii;
using namespace msclr::interop;
using namespace System;

void FreeType::Init()
{
	auto charset = marshal_as<std::string> ((gcnew Generator())->Charset);
	std::unique_ptr<FT_Library, std::function<void(FT_Library*)>> library(new FT_Library(), [](auto* p) { FT_Done_FreeType(*p); });

	FT_Init_FreeType(library.get());
	fonts = gcnew Dictionary<String^, List<String^>^>();
	files = gcnew Dictionary<Tuple<String^, String^>^, String^>();

	for (const auto& file : std::filesystem::directory_iterator{ "C:\\Windows\\Fonts" })
	{
		std::unique_ptr<FT_Face, std::function<void(FT_Face*)>> face(new FT_Face(), [](auto* p) { FT_Done_Face(*p); });

		if (FT_New_Face(*library.get(), file.path().string().c_str(), 0, face.get()) == FT_Err_Ok)
		{
			std::set<unsigned int> advance;

			FT_Set_Pixel_Sizes(*face.get(), 32, 32);

			for (const auto& c : charset)
			{
				if (auto glyphIndex = FT_Get_Char_Index(*face.get(), (FT_ULong)c))
				{
					FT_Load_Glyph(*face.get(), glyphIndex, FT_LOAD_DEFAULT);
					FT_GlyphSlot slot = (*face)->glyph;
					FT_Render_Glyph((*face)->glyph, FT_RENDER_MODE_MONO);
					
					advance.insert(slot->advance.x);
				}
			}

			if (advance.size() == 1)
			{
				auto family = marshal_as<String^>((*face)->family_name);
				auto style = marshal_as<String^>((*face)->style_name);

				if (!fonts->ContainsKey(family))
				{
					fonts->Add(family, gcnew List<String^>());
				}

				if (!fonts[family]->Contains(style))
				{
					fonts[family]->Add(style);
					files->Add(gcnew Tuple<String^, String^>(family, style), marshal_as<String^>(file.path().string()));
				}
			}
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

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t Ascii::Cpp::CountBits(const FT_Bitmap& bitmap)
{
	size_t result = 0;

	for (unsigned int y = 0; y < bitmap.rows; y++)
	{
		const auto* src = &bitmap.buffer[y * bitmap.pitch];

		for (int x = 0; x < bitmap.pitch; x++)
		{
			result += std::bitset<8>(src[x]).count();
		}
	}

	return result;
}

std::map<double, char> Ascii::Cpp::GetCharWeights(const std::string& family, const std::string& style, int height, const std::string& charset)
{
	std::map<double, char> result;

	try
	{
		Tuple<String^, String^> key(marshal_as<String^>(family), marshal_as<String^>(style));
		auto file = marshal_as<std::string>(FreeType::files[% key]);
		std::unique_ptr<FT_Library, std::function<void(FT_Library*)>> library(new FT_Library(), [](auto* p) { FT_Done_FreeType(*p); });

		FT_Init_FreeType(library.get());

		std::unique_ptr<FT_Face, std::function<void(FT_Face*)>> face(new FT_Face(), [](auto* p) { FT_Done_Face(*p); });

		if (FT_New_Face(*library.get(), file.c_str(), 0, face.get()) == FT_Err_Ok)
		{
			FT_Set_Pixel_Sizes(*face.get(), height, height);

			for (const auto& c : charset)
			{
				if (auto glyphIndex = FT_Get_Char_Index(*face.get(), (FT_ULong)c))
				{
					FT_Load_Glyph(*face.get(), glyphIndex, FT_LOAD_DEFAULT);
					FT_GlyphSlot slot = (*face)->glyph;
					FT_Render_Glyph((*face)->glyph, FT_RENDER_MODE_MONO);

					double weight = CountBits(slot->bitmap) / (double)(height * height);

					result.emplace(weight, c);
				}
			}
		}
	}
	catch (KeyNotFoundException^ e)
	{

	}
	
	return result;
}