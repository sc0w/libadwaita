/*
 * Copyright (C) 2019 Alexander Mikhaylenko <exalm7659@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#include "hdy-gizmo-private.h"
#include "hdy-shadow-helper-private.h"
\
/**
 * PRIVATE:hdy-shadow-helper
 * @short_description: Shadow helper used in #HdyLeaflet
 * @title: HdyShadowHelper
 * @See_also: #HdyLeaflet
 * @stability: Private
 *
 * A helper class for drawing #HdyLeaflet transition shadow.
 *
 * Since: 0.0.12
 */

struct _HdyShadowHelper
{
  GObject parent_instance;

  GtkWidget *widget;

  GtkWidget *dimming;
  GtkWidget *shadow;
  GtkWidget *border;
  GtkWidget *outline;

  gdouble progress;
  gdouble shadow_opacity;
};

G_DEFINE_TYPE (HdyShadowHelper, hdy_shadow_helper, G_TYPE_OBJECT);

enum {
  PROP_0,
  PROP_WIDGET,
  LAST_PROP,
};

static GParamSpec *props[LAST_PROP];

static void
hdy_shadow_helper_constructed (GObject *object)
{
  HdyShadowHelper *self = HDY_SHADOW_HELPER (object);

  self->dimming = hdy_gizmo_new ("dimming", NULL, NULL, NULL, NULL, NULL, NULL);
  self->shadow = hdy_gizmo_new ("shadow", NULL, NULL, NULL, NULL, NULL, NULL);
  self->border = hdy_gizmo_new ("border", NULL, NULL, NULL, NULL, NULL, NULL);
  self->outline = hdy_gizmo_new ("outline", NULL, NULL, NULL, NULL, NULL, NULL);

  gtk_widget_set_can_target (self->dimming, FALSE);
  gtk_widget_set_can_target (self->shadow, FALSE);
  gtk_widget_set_can_target (self->border, FALSE);
  gtk_widget_set_can_target (self->outline, FALSE);

  gtk_widget_set_parent (self->dimming, self->widget);
  gtk_widget_set_parent (self->shadow, self->widget);
  gtk_widget_set_parent (self->border, self->widget);
  gtk_widget_set_parent (self->outline, self->widget);

  hdy_shadow_helper_clear (self);

  G_OBJECT_CLASS (hdy_shadow_helper_parent_class)->constructed (object);
}

static void
hdy_shadow_helper_dispose (GObject *object)
{
  HdyShadowHelper *self = HDY_SHADOW_HELPER (object);

  g_clear_pointer (&self->dimming, gtk_widget_unparent);
  g_clear_pointer (&self->shadow, gtk_widget_unparent);
  g_clear_pointer (&self->border, gtk_widget_unparent);
  g_clear_pointer (&self->outline, gtk_widget_unparent);
  g_clear_object (&self->widget);

  G_OBJECT_CLASS (hdy_shadow_helper_parent_class)->dispose (object);
}

static void
hdy_shadow_helper_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  HdyShadowHelper *self = HDY_SHADOW_HELPER (object);

  switch (prop_id) {
  case PROP_WIDGET:
    g_value_set_object (value, self->widget);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
hdy_shadow_helper_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  HdyShadowHelper *self = HDY_SHADOW_HELPER (object);

  switch (prop_id) {
  case PROP_WIDGET:
    self->widget = GTK_WIDGET (g_object_ref (g_value_get_object (value)));
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
hdy_shadow_helper_class_init (HdyShadowHelperClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = hdy_shadow_helper_constructed;
  object_class->dispose = hdy_shadow_helper_dispose;
  object_class->get_property = hdy_shadow_helper_get_property;
  object_class->set_property = hdy_shadow_helper_set_property;

  /**
   * HdyShadowHelper:widget:
   *
   * The widget the shadow will be drawn for. Must not be %NULL
   *
   * Since: 0.0.11
   */
  props[PROP_WIDGET] =
    g_param_spec_object ("widget",
                         _("Widget"),
                         _("The widget the shadow will be drawn for"),
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, LAST_PROP, props);
}

static void
hdy_shadow_helper_init (HdyShadowHelper *self)
{
}

/**
 * hdy_shadow_helper_new:
 *
 * Creates a new #HdyShadowHelper object.
 *
 * Returns: The newly created #HdyShadowHelper object
 *
 * Since: 0.0.12
 */
HdyShadowHelper *
hdy_shadow_helper_new (GtkWidget *widget)
{
  return g_object_new (HDY_TYPE_SHADOW_HELPER,
                       "widget", widget,
                       NULL);
}

void
hdy_shadow_helper_clear (HdyShadowHelper *self)
{
  gtk_widget_set_child_visible (self->dimming, FALSE);
  gtk_widget_set_child_visible (self->shadow, FALSE);
  gtk_widget_set_child_visible (self->border, FALSE);
  gtk_widget_set_child_visible (self->outline, FALSE);

  self->progress = 0;
}

static void
set_style_classes (HdyShadowHelper *self,
                   GtkPanDirection  direction)
{
  const gchar *classes[2];

  switch (direction) {
  case GTK_PAN_DIRECTION_LEFT:
    classes[0] = "left";
    break;
  case GTK_PAN_DIRECTION_RIGHT:
    classes[0] = "right";
    break;
  case GTK_PAN_DIRECTION_UP:
    classes[0] = "up";
    break;
  case GTK_PAN_DIRECTION_DOWN:
    classes[0] = "down";
    break;
  default:
    g_assert_not_reached ();
  }
  classes[1] = NULL;

  gtk_widget_set_css_classes (self->dimming, classes);
  gtk_widget_set_css_classes (self->shadow, classes);
  gtk_widget_set_css_classes (self->border, classes);
  gtk_widget_set_css_classes (self->outline, classes);
}

void
hdy_shadow_helper_size_allocate (HdyShadowHelper *self,
                                 gint             width,
                                 gint             height,
                                 gint             baseline,
                                 gint             x,
                                 gint             y,
                                 gdouble          progress,
                                 GtkPanDirection  direction)
{
  gdouble distance, remaining_distance;
  gint shadow_size, border_size, outline_size;
  GtkOrientation orientation;

  gtk_widget_set_child_visible (self->dimming, TRUE);
  gtk_widget_set_child_visible (self->shadow, TRUE);
  gtk_widget_set_child_visible (self->border, TRUE);
  gtk_widget_set_child_visible (self->outline, TRUE);

  set_style_classes (self, direction);

  gtk_widget_allocate (self->dimming, width, height, baseline,
                       gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x, y)));

  switch (direction) {
  case GTK_PAN_DIRECTION_LEFT:
  case GTK_PAN_DIRECTION_RIGHT:
    distance = width;
    orientation = GTK_ORIENTATION_HORIZONTAL;
    break;
  case GTK_PAN_DIRECTION_UP:
  case GTK_PAN_DIRECTION_DOWN:
    distance = height;
    orientation = GTK_ORIENTATION_VERTICAL;
    break;
  default:
    g_assert_not_reached ();
  }

  gtk_widget_measure (self->shadow, orientation, -1, &shadow_size, NULL, NULL, NULL);
  gtk_widget_measure (self->border, orientation, -1, &border_size, NULL, NULL, NULL);
  gtk_widget_measure (self->outline, orientation, -1, &outline_size, NULL, NULL, NULL);

  switch (direction) {
  case GTK_PAN_DIRECTION_LEFT:
    gtk_widget_allocate (self->shadow, shadow_size, MAX (height, shadow_size), baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x, y)));
    gtk_widget_allocate (self->border, border_size, MAX (height, border_size), baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x, y)));
    gtk_widget_allocate (self->outline, outline_size, MAX (height, outline_size), baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x - outline_size, y)));
    break;
  case GTK_PAN_DIRECTION_RIGHT:
    gtk_widget_allocate (self->shadow, shadow_size, MAX (height, shadow_size), baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x + width - shadow_size, y)));
    gtk_widget_allocate (self->border, border_size, MAX (height, border_size), baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x + width - border_size, y)));
    gtk_widget_allocate (self->outline, outline_size, MAX (height, outline_size), baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x + width, y)));
    break;
  case GTK_PAN_DIRECTION_UP:
    gtk_widget_allocate (self->shadow, MAX (width, shadow_size), shadow_size, baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x, y)));
    gtk_widget_allocate (self->border, MAX (width, border_size), border_size, baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x, y)));
    gtk_widget_allocate (self->outline, MAX (width, outline_size), outline_size, baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x, y - outline_size)));
    break;
  case GTK_PAN_DIRECTION_DOWN:
    gtk_widget_allocate (self->shadow, MAX (width, shadow_size), shadow_size, baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x, y + height - shadow_size)));
    gtk_widget_allocate (self->border, MAX (width, border_size), border_size, baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x, y + height - border_size)));
    gtk_widget_allocate (self->border, MAX (width, outline_size), outline_size, baseline,
                         gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (x, y + height)));
    break;
  default:
    g_assert_not_reached ();
  }

  remaining_distance = (1 - progress) * (gdouble) distance;
  if (remaining_distance < shadow_size)
    self->shadow_opacity = (remaining_distance / shadow_size);
  else
    self->shadow_opacity = 1;

  self->progress = progress;
}

void
hdy_shadow_helper_snapshot (HdyShadowHelper *self,
                            GtkSnapshot     *snapshot)
{
  if (self->progress >= 1)
    return;

  gtk_snapshot_push_opacity (snapshot, 1 - self->progress);
  gtk_widget_snapshot_child (self->widget, self->dimming, snapshot);
  gtk_snapshot_pop (snapshot);

  gtk_snapshot_push_opacity (snapshot, self->shadow_opacity);
  gtk_widget_snapshot_child (self->widget, self->shadow, snapshot);
  gtk_snapshot_pop (snapshot);

  gtk_widget_snapshot_child (self->widget, self->border, snapshot);
  gtk_widget_snapshot_child (self->widget, self->outline, snapshot);
}
