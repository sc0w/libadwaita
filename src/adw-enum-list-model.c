/*
 * Copyright (C) 2020 Purism SPC
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#include "adw-enum-list-model.h"
#include "adw-enum-value-object-private.h"

#include <gio/gio.h>

/**
 * SECTION:adw-enum-list-model
 * @short_description: TODO
 * @Title: AdwEnumListModel
 *
 * TODO
 */

struct _AdwEnumListModel
{
  GObject parent_instance;

  GType enum_type;
  GEnumClass *enum_class;

  AdwEnumValueObject **objects;
};

enum {
  PROP_0,
  PROP_ENUM_TYPE,
  LAST_PROP,
};

static GParamSpec *props[LAST_PROP];

static void adw_enum_list_model_list_model_init (GListModelInterface *iface);

G_DEFINE_TYPE_WITH_CODE (AdwEnumListModel, adw_enum_list_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, adw_enum_list_model_list_model_init))

static void
adw_enum_list_model_constructed (GObject *object)
{
  AdwEnumListModel *self = ADW_ENUM_LIST_MODEL (object);
  guint i;

  self->enum_class = g_type_class_ref (self->enum_type);

  self->objects = g_new0 (AdwEnumValueObject *, self->enum_class->n_values);

  for (i = 0; i < self->enum_class->n_values; i++)
    self->objects[i] = adw_enum_value_object_new (&self->enum_class->values[i]);

  G_OBJECT_CLASS (adw_enum_list_model_parent_class)->constructed (object);
}

static void
adw_enum_list_model_finalize (GObject *object)
{
  AdwEnumListModel *self = ADW_ENUM_LIST_MODEL (object);

  g_clear_pointer (&self->enum_class, g_type_class_unref);
  g_clear_pointer (&self->objects, g_free);

  G_OBJECT_CLASS (adw_enum_list_model_parent_class)->finalize (object);
}

static void
adw_enum_list_model_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  AdwEnumListModel *self = ADW_ENUM_LIST_MODEL (object);

  switch (prop_id) {
  case PROP_ENUM_TYPE:
    g_value_set_gtype (value, adw_enum_list_model_get_enum_type (self));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
adw_enum_list_model_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  AdwEnumListModel *self = ADW_ENUM_LIST_MODEL (object);

  switch (prop_id) {
  case PROP_ENUM_TYPE:
    self->enum_type = g_value_get_gtype (value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
adw_enum_list_model_class_init (AdwEnumListModelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = adw_enum_list_model_constructed;
  object_class->finalize = adw_enum_list_model_finalize;
  object_class->get_property = adw_enum_list_model_get_property;
  object_class->set_property = adw_enum_list_model_set_property;

  props[PROP_ENUM_TYPE] =
    g_param_spec_gtype ("enum-type",
                        _("Enum type"),
                        _("The type of the enum to construct the model from"),
                        G_TYPE_ENUM,
                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, LAST_PROP, props);
}

static void
adw_enum_list_model_init (AdwEnumListModel *self)
{
}

static GType
adw_enum_list_model_get_item_type (GListModel *list)
{
  return ADW_TYPE_ENUM_VALUE_OBJECT;
}

static guint
adw_enum_list_model_get_n_items (GListModel *list)
{
  AdwEnumListModel *self = ADW_ENUM_LIST_MODEL (list);

  return self->enum_class->n_values;
}

static gpointer
adw_enum_list_model_get_item (GListModel *list,
                              guint       position)
{
  AdwEnumListModel *self = ADW_ENUM_LIST_MODEL (list);

  if (position >= self->enum_class->n_values)
    return NULL;

  return g_object_ref (self->objects[position]);
}

static void
adw_enum_list_model_list_model_init (GListModelInterface *iface)
{
  iface->get_item_type = adw_enum_list_model_get_item_type;
  iface->get_n_items = adw_enum_list_model_get_n_items;
  iface->get_item = adw_enum_list_model_get_item;
}

AdwEnumListModel *
adw_enum_list_model_new (GType enum_type)
{
  return g_object_new (ADW_TYPE_ENUM_LIST_MODEL,
                       "enum-type", enum_type,
                       NULL);
}

GType
adw_enum_list_model_get_enum_type (AdwEnumListModel *self)
{
  g_return_val_if_fail (ADW_IS_ENUM_LIST_MODEL (self), G_TYPE_INVALID);

  return self->enum_type;
}

guint
adw_enum_list_model_find_position (AdwEnumListModel *self,
                                   gint              value)
{
  guint i;

  g_return_val_if_fail (ADW_IS_ENUM_LIST_MODEL (self), 0);

  for (i = 0; i < self->enum_class->n_values; i++)
    if (self->enum_class->values[i].value == value)
      return i;

  g_critical ("%s does not contain value %d",
              G_ENUM_CLASS_TYPE_NAME (self->enum_class), value);

  return 0;
}