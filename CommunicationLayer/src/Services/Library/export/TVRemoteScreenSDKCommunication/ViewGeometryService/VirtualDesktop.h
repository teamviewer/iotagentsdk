//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//********************************************************************************//
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace TVRemoteScreenSDKCommunication
{

namespace ViewGeometryService
{

struct Rect
{
	Rect() = default;

	Rect(int32_t _x, int32_t _y, int32_t _width, int32_t _height)
	: x{_x}
	, y{_y}
	, width{_width}
	, height{_height}
	{}

	int32_t x = 0;
	int32_t y = 0;
	int32_t width = 0;
	int32_t height = 0;
};

struct Screen
{
	Screen() = default;

	Screen(const std::string& _name, const Rect& _geometry)
	: name{_name}
	, geometry{_geometry}
	{}

	std::string name;
	Rect geometry;
};

struct VirtualDesktop
{
	VirtualDesktop() = default;

	VirtualDesktop(int32_t _width, int32_t _height, std::vector<Screen> _screens)
	: width{_width}
	, height{_height}
	, screens{std::move(_screens)}
	{}

	int32_t width = 0;
	int32_t height = 0;
	std::vector<Screen> screens;
};

enum class UpdateGeometryResult
{
	Ok = 0,
	Failed = 1,
};

} // namespace ViewGeometryService

} // namespace TVRemoteScreenSDKCommunication
