#pragma once

#include <map>
#include <string>

namespace Ascii
{
	using namespace System::Collections::Generic;
	using namespace System::ComponentModel;

	using System::String;

	public ref class Generator : public INotifyPropertyChanged
	{
	public:
		Generator();

		property String^ Charset { String^ get(); }
		property String^ FontFamily { String^ get(); void set(String^); }
		property String^ FontStyle { String^ get(); void set(String^); }
		property int FontHeight { int get(); void set(int); }
		property String^ FileName { String^ get(); void set(String^); }
		property double Scale { double get(); void set(double); }
		property bool R { bool get(); void set(bool); }
		property bool G { bool get(); void set(bool); }
		property bool B { bool get(); void set(bool); }
		property double FontAspectRatio { double get(); void set(double); }
		property List<String^>^ Data { List<String^>^ get(); }

		virtual event PropertyChangedEventHandler^ PropertyChanged;

	protected:

	private:
		!Generator();
		virtual ~Generator();

		String^ fontFamily;
		String^ fontStyle;
		int fontHeight;
		String^ fileName;
		double scale, fontAspectRatio;
		bool r, g, b;
	};
}
