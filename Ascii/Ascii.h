#pragma once

namespace Ascii 
{
	using namespace System::Collections::Generic;

	public ref class FreeType
	{
	public:
		static property Dictionary<System::String^, List<System::String^>^>^ FixedWidthFonts { Dictionary<System::String^, List<System::String^>^>^ get(); };

	private:
		static void Init();

		static Dictionary<System::String^, List<System::String^>^>^ fonts;
		static Dictionary<System::Tuple<System::String^, System::String^>^, System::String^>^ files;
	};
}
