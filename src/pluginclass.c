/* SLV2
 * Copyright (C) 2007-2011 David Robillard <http://drobilla.net>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "slv2/types.h"
#include "slv2/pluginclass.h"
#include "slv2/value.h"
#include "slv2_internal.h"

/* private */
SLV2PluginClass
slv2_plugin_class_new(SLV2World   world,
                      SLV2Node    parent_node,
                      SLV2Node    uri,
                      const char* label)
{
	if (parent_node && sord_node_get_type(parent_node) != SORD_URI) {
		return NULL;  // Not an LV2 plugin superclass (FIXME: discover properly)
	}
	SLV2PluginClass pc = (SLV2PluginClass)malloc(sizeof(struct _SLV2PluginClass));
	pc->world      = world;
	pc->uri        = slv2_value_new_from_node(world, uri);
	pc->label      = slv2_value_new(world, SLV2_VALUE_STRING, label);
	pc->parent_uri = (parent_node)
		? slv2_value_new_from_node(world, parent_node)
		: NULL;
	return pc;
}

void
slv2_plugin_class_free(SLV2PluginClass plugin_class)
{
	assert(plugin_class->uri);
	slv2_value_free(plugin_class->uri);
	slv2_value_free(plugin_class->parent_uri);
	slv2_value_free(plugin_class->label);
	free(plugin_class);
}

SLV2Value
slv2_plugin_class_get_parent_uri(SLV2PluginClass plugin_class)
{
	if (plugin_class->parent_uri)
		return plugin_class->parent_uri;
	else
		return NULL;
}

SLV2Value
slv2_plugin_class_get_uri(SLV2PluginClass plugin_class)
{
	assert(plugin_class->uri);
	return plugin_class->uri;
}

SLV2Value
slv2_plugin_class_get_label(SLV2PluginClass plugin_class)
{
	return plugin_class->label;
}

SLV2PluginClasses
slv2_plugin_class_get_children(SLV2PluginClass plugin_class)
{
	// Returned list doesn't own categories
	SLV2PluginClasses result = g_ptr_array_new();

	for (unsigned i = 0; i < ((GPtrArray*)plugin_class->world->plugin_classes)->len; ++i) {
		SLV2PluginClass c = g_ptr_array_index(
			(GPtrArray*)plugin_class->world->plugin_classes, i);
		SLV2Value parent = slv2_plugin_class_get_parent_uri(c);
		if (parent && slv2_value_equals(slv2_plugin_class_get_uri(plugin_class), parent))
			g_ptr_array_add(result, c);
	}

	return result;
}
