/*************************************************************************/
/*  box_container.cpp                                                    */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "flow_container.h"
#include <typeinfo>
#include <iostream>
#include "core/typedefs.h"

void FlowContainer::_resort() {
	const int sep = get_constant("separation");
	int length = vertical ? get_size().height : get_size().width;
	int line_offset = 0, item_offset = 0;
	int min_length = 0;
	bool first_line, first_item;
	
	int line_start = 0;
	first_line = true;
	while (line_start < get_child_count()) {
		int line_end = line_start;
		int line_depth = 0;
		item_offset = 0;
		first_item = true;
		for (const Control *child; (child = get_child(line_end)->cast_to<Control>()); line_end++) {
			const int item_length = vertical ? child->get_minimum_size().height : child->get_minimum_size().width;
			const int item_depth = vertical ? child->get_minimum_size().width : child->get_minimum_size().height;
			item_offset += (first_item ? 0 : sep);
			if ((item_offset += item_length) >= length && !first_item)
				break;
			first_item = false;
			line_depth = MAX(line_depth, item_depth);
			min_length = MAX(min_length, item_length);
		}
			
		if (!first_line) line_offset += sep;
		first_line = false;
		item_offset = 0;
		first_item = true;
		for (int i = line_start; i < line_end; i++) {
			Control *const child = get_child(i)->cast_to<Control>();
			const int item_length = vertical ? child->get_minimum_size().height : child->get_minimum_size().width;
			item_offset += (first_item ? 0 : sep);
			first_item = false;
			const Point2 offset = vertical ? Point2(line_offset, item_offset) : Point2(item_offset, line_offset);
			//const Size2 size = Size2(vertical ? ((child->get_h_size_flags() & SIZE_EXPAND) ? line_depth : child->get_minimum_size().width) : child->get_minimum_size().width, vertical ? child->get_minimum_size().height : ((child->get_v_size_flags() & SIZE_EXPAND) ? line_depth : child->get_minimum_size().height));
			const Size2 size = Size2(vertical ? line_depth : child->get_minimum_size().width, vertical ? child->get_minimum_size().height : line_depth);
			fit_child_in_rect(child, Rect2(offset, size));
			item_offset += item_length;
		}
		line_start = line_end;
		line_offset += line_depth;
	}
	minimum_size = vertical ? Point2(line_offset, min_length) : Point2(min_length, line_offset);
}

Size2 FlowContainer::get_minimum_size() const {
	return minimum_size;
}

void FlowContainer::_notification(int p_what) {
	switch(p_what) {
		case NOTIFICATION_SORT_CHILDREN: {
			_resort();
		} break;
	}
}

FlowContainer::FlowContainer(bool p_vertical)
: vertical(p_vertical), minimum_size(0, 0) {
	set_stop_mouse(false);
}
