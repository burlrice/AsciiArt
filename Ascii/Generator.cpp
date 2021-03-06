#include "pch.h"
#include "Generator.h"
#include "FreeType.h"

#include <limits>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <msclr/marshal_cppstd.h>
#include <FreeImagePlus.h>

using namespace Ascii;
using namespace msclr::interop;
using namespace System;

static const std::string charset = []()
{
	std::string result;

	for (char c = ' '; c <= '~'; c++)
	{
		result += c;
	}

	return result;
}();

Generator::Generator()
{
	fontFamily = gcnew String("Courier New");
	fontStyle = gcnew String("Regular");
	fontHeight = 22;
	fileName = gcnew String("");
	scale = 0.08;
	fontAspectRatio = 1.0;
	r = false;
	g = true; 
	b = false;
}

Generator::~Generator()
{
	this->!Generator();
}

Generator::!Generator()
{
}

String^ Generator::Charset::get()
{
	return marshal_as<String^>(charset);
}

String^ Generator::FontStyle::get()
{
	return fontStyle;
}

void Generator::FontStyle::set(String^ value)
{
	if (fontStyle != value)
	{
		fontStyle = value;
		PropertyChanged(this, gcnew PropertyChangedEventArgs("FontStyle"));
		PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
	}
}

int Generator::FontHeight::get()
{
	return fontHeight;
}

void Generator::FontHeight::set(int value)
{
	if (fontHeight != value)
	{
		fontHeight = value;
		PropertyChanged(this, gcnew PropertyChangedEventArgs("FontHeight"));
		PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
	}
}

String^ Generator::FontFamily::get()
{
	return fontFamily;
}

void Generator::FontFamily::set(String^ value)
{
	if (FreeType::fonts->ContainsKey(value))
	{
		if (fontFamily != value)
		{
			fontFamily = value;
			PropertyChanged(this, gcnew PropertyChangedEventArgs("FontFamily"));
			PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
			FontStyle = System::Linq::Enumerable::FirstOrDefault(FreeType::fonts[value]);
		}
	}
}

String^ Generator::FileName::get()
{
	return fileName;
}

void Generator::FileName::set(String^ value)
{
	if (fileName != value)
	{
		fileName = value;
		PropertyChanged(this, gcnew PropertyChangedEventArgs("FileName"));
		PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
	}
}

double Generator::Scale::get()
{
	return scale;
}

void Generator::Scale::set(double value)
{
	value = std::clamp(value, 0.01, 2.0);

	if (scale != value)
	{
		scale = value;
		PropertyChanged(this, gcnew PropertyChangedEventArgs("Scale"));
		PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
	}
}

double Generator::FontAspectRatio::get()
{
	return fontAspectRatio;
}

void Generator::FontAspectRatio::set(double value)
{
	fontAspectRatio = value;
}

List<String^>^ Generator::Data::get()
{
	auto result = gcnew List<String^>();
	fipImage img;

	if (img.load(marshal_as<std::string>(FileName).c_str()))
	{
		auto family = marshal_as<std::string>(FontFamily);
		auto style = marshal_as<std::string>(FontStyle);
		auto charset = marshal_as<std::string>(Charset);

		const auto maxSize = std::pair(640u, 480u);
		const auto weights = Cpp::GetCharWeights(family, style, FontHeight, charset);

		if (weights.size())
		{
			auto cx = img.getWidth() * scale / fontAspectRatio;
			auto cy = img.getHeight() * scale;

			img.rescale((int)std::round(cx), (int)std::round(cy), FILTER_BOX);
			img.convertTo32Bits();
			
			if (!Invert)
			{
				img.invert();
			}

			for (auto y = (int)img.getHeight(); y >= 0; y--)
			{
				std::string line(img.getWidth(), ' ');

				for (auto x = 0u; x < img.getWidth(); x++)
				{
					RGBQUAD rgb = { 0 };

					img.getPixelColor(x, y, &rgb);

					auto index = (r ? rgb.rgbRed : 0) | (g ? rgb.rgbGreen : 0) | (b ? rgb.rgbBlue : 0);

					line[x] = weights[index];
				}

				result->Add(marshal_as<String^>(line));
			}
		}
	}

	return result;
}

bool Generator::R::get()
{
	return r;
}

void Generator::R::set(bool value)
{
	r = value;
	PropertyChanged(this, gcnew PropertyChangedEventArgs("R"));
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
}

bool Generator::G::get()
{
	return g;
}

void Generator::G::set(bool value)
{
	g = value;
	PropertyChanged(this, gcnew PropertyChangedEventArgs("G"));
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
}

bool Generator::B::get()
{
	return b;
}

void Generator::B::set(bool value)
{
	b = value;
	PropertyChanged(this, gcnew PropertyChangedEventArgs("B"));
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
}

bool Generator::Invert::get()
{
	return invert;
}

void Generator::Invert::set(bool value)
{
	invert = value;
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Invert"));
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
}