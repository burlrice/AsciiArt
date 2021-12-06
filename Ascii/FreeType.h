#pragma once

#include <string>
#include <array>

#define generic _generic
#include <ft2build.h>
#include FT_FREETYPE_H
#undef generic

namespace Ascii
{
	using namespace System::Collections::Generic;

	using System::String;

	public ref class FreeType
	{
	public:
		static property Dictionary<String^, List<String^>^>^ FixedWidthFonts { Dictionary<String^, List<String^>^>^ get(); };

	private:
		static void Init();

	internal:
		static Dictionary<String^, List<String^>^>^ fonts;
		static Dictionary<System::Tuple<String^, String^>^, System::Tuple<String^, double>^>^ files;
	};

	namespace Cpp
	{
		size_t CountBits(const FT_Bitmap& bitmap);
		std::array<char, 256> GetCharWeights(const std::string& family, const std::string& style, int height, const std::string& charset);
	};
}
