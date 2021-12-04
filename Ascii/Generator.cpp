#include "pch.h"
#include "Generator.h"
#include "FreeType.h"

#include <limits>
#include <cmath>
#include <algorithm>
#include <msclr/marshal_cppstd.h>
#include <FreeImagePlus.h>
#include <sstream>

#define generic GenericFromFreeTypeLibrary
#include <ft2build.h>
#include FT_FREETYPE_H
#undef generic

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
	scale = 0.1;
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
	fontStyle = value;
	PropertyChanged(this, gcnew PropertyChangedEventArgs("FontStyle"));
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
}

int Generator::FontHeight::get()
{
	return fontHeight;
}

void Generator::FontHeight::set(int value)
{
	fontHeight = value;
	PropertyChanged(this, gcnew PropertyChangedEventArgs("FontHeight"));
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
}

String^ Generator::FontFamily::get()
{
	return fontFamily;
}

void Generator::FontFamily::set(String^ value)
{
	if (FreeType::fonts->ContainsKey(value))
	{
		fontFamily = value;
		PropertyChanged(this, gcnew PropertyChangedEventArgs("FontFamily"));
		PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
		FontStyle = System::Linq::Enumerable::FirstOrDefault(FreeType::fonts[value]);
	}
}

String^ Generator::FileName::get()
{
	return fileName;
}

void Generator::FileName::set(String^ value)
{
	fileName = value;
	PropertyChanged(this, gcnew PropertyChangedEventArgs("FileName"));
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
}

double Generator::Scale::get()
{
	return scale;
}

void Generator::Scale::set(double value)
{
	scale = std::clamp(value, 0.01, 2.0);
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Scale"));
	PropertyChanged(this, gcnew PropertyChangedEventArgs("Data"));
}

List<String^>^ Generator::Data::get()
{
	auto result = gcnew List<String^>();
	fipImage img;

	if (img.load(marshal_as<std::string>(FileName).c_str()))
	{
		const auto maxSize = std::pair(640u, 480u);
		const auto weights = Cpp::GetCharWeights(marshal_as<std::string>(FontFamily), marshal_as<std::string>(FontStyle), FontHeight, marshal_as<std::string>(Charset));

		if (weights.size())
		{
			const auto maxBits = std::pow(2, sizeof(RGBQUAD::rgbRed) * CHAR_BIT);
			const auto range = weights.rbegin()->first - weights.begin()->first;
			auto cx = img.getWidth() * scale;
			auto cy = img.getHeight() * scale;

			std::wstringstream ss;
			ss << L"Generator::Data: " << cx << L", " << cy << std::endl;
			::OutputDebugString(ss.str().c_str()); // TODO: rem

			img.rescale((int)std::round(cx), (int)std::round(cy), FILTER_BOX);

			for (auto y = (int)img.getHeight(); y >= 0; y--)
			{
				std::string line(img.getWidth(), ' ');

				for (auto x = 0u; x < img.getWidth(); x++)
				{
					RGBQUAD rgb = { 0 };

					img.getPixelColor(x, y, &rgb);

					double weight = ((rgb.rgbRed | rgb.rgbGreen | rgb.rgbBlue) / maxBits) * range;
					auto low = weights.lower_bound(weight);
					auto high = weights.upper_bound(weight);
					auto iterator = (high == weights.cend()) ? low : (*low > *high ? low : high);

					line[x] = iterator->second;
				}

				result->Add(marshal_as<String^>(line));
			}
		}
	}

	return result;
}
