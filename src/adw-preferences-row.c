/*
 * Copyright (C) 2019 Purism SPC
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#include "adw-preferences-row.h"

/**
 * SECTION:adw-preferences-row
 * @short_description: A #GtkListBox row used to present preferences.
 * @Title: AdwPreferencesRow
 *
 * The #AdwPreferencesRow widget has a title that #AdwPreferencesWindow will use
 * to let the user look for a preference. It doesn't present the title in any
 * way and it lets you present the preference as you please.
 *
 * #AdwActionRow and its derivatives are convenient to use as preference rows as
 * they take care of presenting the preference's title while letting you compose
 * the inputs of the preference around it.
 *
 * Since: 1.0
 */

typedef struct
{
  gchar *title;

  gboolean use_underline;
} AdwPreferencesRowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AdwPreferencesRow, adw_preferences_row, GTK_TYPE_LIST_BOX_ROW)

enum {
  PROP_0,
  PROP_TITLE,
  PROP_USE_UNDERLINE,
  LAST_PROP,
};

static GParamSpec *props[LAST_PROP];

static void
adw_preferences_row_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  AdwPreferencesRow *self = ADW_PREFERENCES_ROW (object);

  switch (prop_id) {
  case PROP_TITLE:
    g_value_set_string (value, adw_preferences_row_get_title (self));
    break;
  case PROP_USE_UNDERLINE:
    g_value_set_boolean (value, adw_preferences_row_get_use_underline (self));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
adw_preferences_row_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  AdwPreferencesRow *self = ADW_PREFERENCES_ROW (object);

  switch (prop_id) {
  case PROP_TITLE:
    adw_preferences_row_set_title (self, g_value_get_string (value));
    break;
  case PROP_USE_UNDERLINE:
    adw_preferences_row_set_use_underline (self, g_value_get_boolean (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
adw_preferences_row_finalize (GObject *object)
{
  AdwPreferencesRow *self = ADW_PREFERENCES_ROW (object);
  AdwPreferencesRowPrivate *priv = adw_preferences_row_get_instance_private (self);

  g_free (priv->title);

  G_OBJECT_CLASS (adw_preferences_row_parent_class)->finalize (object);
}

static void
adw_preferences_row_class_init (AdwPreferencesRowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = adw_preferences_row_get_property;
  object_class->set_property = adw_preferences_row_set_property;
  object_class->finalize = adw_preferences_row_finalize;

  /**
   * AdwPreferencesRow:title:
   *
   * The title of the preference represented by this row.
   *
   * Since: 1.0
   */
  props[PROP_TITLE] =
    g_param_spec_string ("title",
                         _("Title"),
                         _("The title of the preference"),
                         "",
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * AdwPreferencesRow:use-underline:
   *
   * Whether an embedded underline in the text of the title indicates a
   * mnemonic.
   *
   * Since: 1.0
   */
  props[PROP_USE_UNDERLINE] =
    g_param_spec_boolean ("use-underline",
                          _("Use underline"),
                          _("If set, an underline in the text indicates the next character should be used for the mnemonic accelerator key"),
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, LAST_PROP, props);
}

static void
adw_preferences_row_init (AdwPreferencesRow *self)
{
}

/**
 * adw_preferences_row_new:
 *
 * Creates a new #AdwPreferencesRow.
 *
 * Returns: a new #AdwPreferencesRow
 *
 * Since: 1.0
 */
GtkWidget *
adw_preferences_row_new (void)
{
  return g_object_new (ADW_TYPE_PREFERENCES_ROW, NULL);
}

/**
 * adw_preferences_row_get_title:
 * @self: a #AdwPreferencesRow
 *
 * Gets the title of the preference represented by @self.
 *
 * Returns: (transfer none) (nullable): the title of the preference represented
 *          by @self, or %NULL.
 *
 * Since: 1.0
 */
const gchar *
adw_preferences_row_get_title (AdwPreferencesRow *self)
{
  AdwPreferencesRowPrivate *priv;

  g_return_val_if_fail (ADW_IS_PREFERENCES_ROW (self), NULL);

  priv = adw_preferences_row_get_instance_private (self);

  return priv->title;
}

/**
 * adw_preferences_row_set_title:
 * @self: a #AdwPreferencesRow
 * @title: (nullable): the title, or %NULL.
 *
 * Sets the title of the preference represented by @self.
 *
 * Since: 1.0
 */
void
adw_preferences_row_set_title (AdwPreferencesRow *self,
                               const gchar       *title)
{
  AdwPreferencesRowPrivate *priv;

  g_return_if_fail (ADW_IS_PREFERENCES_ROW (self));

  priv = adw_preferences_row_get_instance_private (self);

  if (g_strcmp0 (priv->title, title) == 0)
    return;

  g_free (priv->title);
  priv->title = g_strdup (title);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_TITLE]);
}

/**
 * adw_preferences_row_get_use_underline:
 * @self: a #AdwPreferencesRow
 *
 * Gets whether an embedded underline in the text of the title indicates a
 * mnemonic. See adw_preferences_row_set_use_underline().
 *
 * Returns: %TRUE if an embedded underline in the title indicates the mnemonic
 *          accelerator keys.
 *
 * Since: 1.0
 */
gboolean
adw_preferences_row_get_use_underline (AdwPreferencesRow *self)
{
  AdwPreferencesRowPrivate *priv;

  g_return_val_if_fail (ADW_IS_PREFERENCES_ROW (self), FALSE);

  priv = adw_preferences_row_get_instance_private (self);

  return priv->use_underline;
}

/**
 * adw_preferences_row_set_use_underline:
 * @self: a #AdwPreferencesRow
 * @use_underline: %TRUE if underlines in the text indicate mnemonics
 *
 * If true, an underline in the text of the title indicates the next character
 * should be used for the mnemonic accelerator key.
 *
 * Since: 1.0
 */
void
adw_preferences_row_set_use_underline (AdwPreferencesRow *self,
                                       gboolean           use_underline)
{
  AdwPreferencesRowPrivate *priv;

  g_return_if_fail (ADW_IS_PREFERENCES_ROW (self));

  priv = adw_preferences_row_get_instance_private (self);

  use_underline = !!use_underline;

  if (priv->use_underline == use_underline)
    return;

  priv->use_underline = use_underline;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_USE_UNDERLINE]);
}