#pragma once

#include <string>
#include <map>

#define generic GenericFromFreeTypeLibrary
#include <ft2build.h>
#include FT_FREETYPE_H
#undef generic

namespace Ascii
{
	using namespace System::Collections::Generic;

	public ref class FreeType
	{
	public:
		static property Dictionary<System::String^, List<System::String^>^>^ FixedWidthFonts { Dictionary<System::String^, List<System::String^>^>^ get(); };

	private:
		static void Init();

	internal:
		static Dictionary<System::String^, List<System::String^>^>^ fonts;
		static Dictionary<System::Tuple<System::String^, System::String^>^, System::String^>^ files;
	};

	namespace Cpp
	{
		size_t CountBits(const FT_Bitmap& bitmap);
		std::map<double, char> GetCharWeights(const std::string& family, const std::string& style, int height, const std::string& charset);
	};
}
