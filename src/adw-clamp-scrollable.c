/*
 * Copyright (C) 2020 Purism SPC
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"
#include "adw-clamp-scrollable.h"

#include <glib/gi18n-lib.h>

#include "adw-clamp-layout.h"

/**
 * SECTION:adw-clamp-scrollable
 * @short_description: A widget constraining its child to a given size.
 * @Title: AdwClampScrollable
 *
 * The #AdwClamp widget constraints the size of the widget it contains to a
 * given maximum size. It will constrain the width if it is horizontal, or the
 * height if it is vertical. The expansion of the child from its minimum to its
 * maximum size is eased out for a smooth transition.
 *
 * If the child requires more than the requested maximum size, it will be
 * allocated the minimum size it can fit in instead.
 *
 * # CSS nodes
 *
 * #AdwClamp has a single CSS node with name clamp. The node will get the style
 * classes .large when its child reached its maximum size, .small when the clamp
 * allocates its full size to its child, .medium in-between, or none if it
 * didn't compute its size yet.
 *
 * Since: 1.0
 */

enum {
  PROP_0,
  PROP_CHILD,
  PROP_MAXIMUM_SIZE,
  PROP_TIGHTENING_THRESHOLD,

  /* Overridden properties */
  PROP_ORIENTATION,
  PROP_HADJUSTMENT,
  PROP_VADJUSTMENT,
  PROP_HSCROLL_POLICY,
  PROP_VSCROLL_POLICY,

  LAST_PROP = PROP_TIGHTENING_THRESHOLD + 1,
};

struct _AdwClampScrollable
{
  GtkWidget parent_instance;

  GtkWidget *child;
  GtkOrientation orientation;

  GtkAdjustment *hadjustment;
  GtkAdjustment *vadjustment;
  GtkScrollablePolicy hscroll_policy;
  GtkScrollablePolicy vscroll_policy;

  GBinding *hadjustment_binding;
  GBinding *vadjustment_binding;
  GBinding *hscroll_policy_binding;
  GBinding *vscroll_policy_binding;
};

static GParamSpec *props[LAST_PROP];

static void adw_clamp_scrollable_buildable_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_WITH_CODE (AdwClampScrollable, adw_clamp_scrollable, GTK_TYPE_WIDGET,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_ORIENTABLE, NULL)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_SCROLLABLE, NULL)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE, adw_clamp_scrollable_buildable_init))

static GtkBuildableIface *parent_buildable_iface;

static void
set_orientation (AdwClampScrollable *self,
                 GtkOrientation      orientation)
{
  GtkLayoutManager *layout = gtk_widget_get_layout_manager (GTK_WIDGET (self));

  if (self->orientation == orientation)
    return;

  self->orientation = orientation;
  gtk_orientable_set_orientation (GTK_ORIENTABLE (layout), orientation);

  gtk_widget_queue_resize (GTK_WIDGET (self));

  g_object_notify (G_OBJECT (self), "orientation");
}

static void
set_hadjustment (AdwClampScrollable *self,
                 GtkAdjustment      *adjustment)
{
  if (self->hadjustment == adjustment)
    return;

  self->hadjustment = adjustment;

  g_object_notify (G_OBJECT (self), "hadjustment");
}

static void
set_vadjustment (AdwClampScrollable *self,
                 GtkAdjustment      *adjustment)
{
  if (self->vadjustment == adjustment)
    return;

  self->vadjustment = adjustment;

  g_object_notify (G_OBJECT (self), "vadjustment");
}

static void
set_hscroll_policy (AdwClampScrollable  *self,
                    GtkScrollablePolicy  policy)
{
  if (self->hscroll_policy == policy)
    return;

  self->hscroll_policy = policy;

  g_object_notify (G_OBJECT (self), "hscroll-policy");
}

static void
set_vscroll_policy (AdwClampScrollable  *self,
                    GtkScrollablePolicy  policy)
{
  if (self->vscroll_policy == policy)
    return;

  self->vscroll_policy = policy;

  g_object_notify (G_OBJECT (self), "vscroll-policy");
}

static void
adw_clamp_scrollable_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  AdwClampScrollable *self = ADW_CLAMP_SCROLLABLE (object);

  switch (prop_id) {
  case PROP_CHILD:
    g_value_set_object (value, adw_clamp_scrollable_get_child (self));
    break;
  case PROP_MAXIMUM_SIZE:
    g_value_set_int (value, adw_clamp_scrollable_get_maximum_size (self));
    break;
  case PROP_TIGHTENING_THRESHOLD:
    g_value_set_int (value, adw_clamp_scrollable_get_tightening_threshold (self));
    break;
  case PROP_ORIENTATION:
    g_value_set_enum (value, self->orientation);
    break;
  case PROP_HADJUSTMENT:
    g_value_set_object (value, self->hadjustment);
    break;
  case PROP_VADJUSTMENT:
    g_value_set_object (value, self->vadjustment);
    break;
  case PROP_HSCROLL_POLICY:
    g_value_set_enum (value, self->hscroll_policy);
    break;
  case PROP_VSCROLL_POLICY:
    g_value_set_enum (value, self->vscroll_policy);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
adw_clamp_scrollable_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  AdwClampScrollable *self = ADW_CLAMP_SCROLLABLE (object);

  switch (prop_id) {
  case PROP_CHILD:
    adw_clamp_scrollable_set_child (self, g_value_get_object (value));
    break;
  case PROP_MAXIMUM_SIZE:
    adw_clamp_scrollable_set_maximum_size (self, g_value_get_int (value));
    break;
  case PROP_TIGHTENING_THRESHOLD:
    adw_clamp_scrollable_set_tightening_threshold (self, g_value_get_int (value));
    break;
  case PROP_ORIENTATION:
    set_orientation (self, g_value_get_enum (value));
    break;
  case PROP_HADJUSTMENT:
    set_hadjustment (self, g_value_get_object (value));
    break;
  case PROP_VADJUSTMENT:
    set_vadjustment (self, g_value_get_object (value));
    break;
  case PROP_HSCROLL_POLICY:
    set_hscroll_policy (self, g_value_get_enum (value));
    break;
  case PROP_VSCROLL_POLICY:
    set_vscroll_policy (self, g_value_get_enum (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
adw_clamp_scrollable_dispose (GObject *object)
{
  AdwClampScrollable *self = ADW_CLAMP_SCROLLABLE (object);

  adw_clamp_scrollable_set_child (self, NULL);

  G_OBJECT_CLASS (adw_clamp_scrollable_parent_class)->dispose (object);
}

static void
adw_clamp_scrollable_class_init (AdwClampScrollableClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = adw_clamp_scrollable_get_property;
  object_class->set_property = adw_clamp_scrollable_set_property;
  object_class->dispose = adw_clamp_scrollable_dispose;

  g_object_class_override_property (object_class,
                                    PROP_ORIENTATION,
                                    "orientation");

  g_object_class_override_property (object_class,
                                    PROP_HADJUSTMENT,
                                    "hadjustment");

  g_object_class_override_property (object_class,
                                    PROP_VADJUSTMENT,
                                    "vadjustment");

  g_object_class_override_property (object_class,
                                    PROP_HSCROLL_POLICY,
                                    "hscroll-policy");

  g_object_class_override_property (object_class,
                                    PROP_VSCROLL_POLICY,
                                    "vscroll-policy");

  props[PROP_CHILD] =
      g_param_spec_object ("child",
                           _("Child"),
                           _("The child widget"),
                           GTK_TYPE_WIDGET,
                           G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * AdwClampScrollable:maximum-size:
   *
   * The maximum size to allocate to the child. It is the width if the clamp is
   * horizontal, or the height if it is vertical.
   *
   * Since: 1.0
   */
  props[PROP_MAXIMUM_SIZE] =
      g_param_spec_int ("maximum-size",
                        _("Maximum size"),
                        _("The maximum size allocated to the child"),
                        0, G_MAXINT, 600,
                        G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * AdwClampScrollable:tightening-threshold:
   *
   * The size starting from which the clamp will tighten its grip on the child,
   * slowly allocating less and less of the available size up to the maximum
   * allocated size. Below that threshold and below the maximum width, the child
   * will be allocated all the available size.
   *
   * If the threshold is greater than the maximum size to allocate to the child,
   * the child will be allocated all the width up to the maximum.
   * If the threshold is lower than the minimum size to allocate to the child,
   * that size will be used as the tightening threshold.
   *
   * Effectively, tightening the grip on the child before it reaches its maximum
   * size makes transitions to and from the maximum size smoother when resizing.
   *
   * Since: 1.0
   */
  props[PROP_TIGHTENING_THRESHOLD] =
      g_param_spec_int ("tightening-threshold",
                        _("Tightening threshold"),
                        _("The size from which the clamp will tighten its grip on the child"),
                        0, G_MAXINT, 400,
                        G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, LAST_PROP, props);

  gtk_widget_class_set_layout_manager_type (widget_class, ADW_TYPE_CLAMP_LAYOUT);
  gtk_widget_class_set_css_name (widget_class, "clamp");
}

static void
adw_clamp_scrollable_init (AdwClampScrollable *self)
{
}

static void
adw_clamp_scrollable_buildable_add_child (GtkBuildable *buildable,
                                          GtkBuilder   *builder,
                                          GObject      *child,
                                          const char   *type)
{
  if (GTK_IS_WIDGET (child))
    adw_clamp_scrollable_set_child (ADW_CLAMP_SCROLLABLE (buildable), GTK_WIDGET (child));
  else
    parent_buildable_iface->add_child (buildable, builder, child, type);
}

static void
adw_clamp_scrollable_buildable_init (GtkBuildableIface *iface)
{
  parent_buildable_iface = g_type_interface_peek_parent (iface);

  iface->add_child = adw_clamp_scrollable_buildable_add_child;
}

/**
 * adw_clamp_scrollable_new:
 *
 * Creates a new #AdwClampScrollable.
 *
 * Returns: a new #AdwClampScrollable
 *
 * Since: 1.0
 */
GtkWidget *
adw_clamp_scrollable_new (void)
{
  return g_object_new (ADW_TYPE_CLAMP_SCROLLABLE, NULL);
}

/**
 * adw_clamp_scrollable_get_child:
 * @self: a #AdwClampScrollable
 *
 * Gets the child widget of @self.
 *
 * Returns: (nullable) (transfer none): the child widget of @self
 *
 * Since: 1.0
 */
GtkWidget *
adw_clamp_scrollable_get_child (AdwClampScrollable *self)
{
  g_return_val_if_fail (ADW_IS_CLAMP_SCROLLABLE (self), NULL);

  return self->child;
}

/**
 * adw_clamp_scrollable_set_child:
 * @self: a #AdwClampScrollable
 * @child: (nullable): the child widget
 *
 * Sets the child widget of @self.
 *
 * Since: 1.0
 */
void
adw_clamp_scrollable_set_child (AdwClampScrollable *self,
                                GtkWidget          *child)
{
  g_return_if_fail (ADW_IS_CLAMP_SCROLLABLE (self));
  g_return_if_fail (child == NULL || GTK_IS_WIDGET (child));

  if (self->child == child)
    return;

  if (self->child) {
    g_clear_pointer (&self->hadjustment_binding, g_binding_unbind);
    g_clear_pointer (&self->vadjustment_binding, g_binding_unbind);
    g_clear_pointer (&self->hscroll_policy_binding, g_binding_unbind);
    g_clear_pointer (&self->vscroll_policy_binding, g_binding_unbind);

    gtk_widget_unparent (self->child);
  }

  self->child = child;

  if (child) {
    gtk_widget_set_parent (child, GTK_WIDGET (self));

    self->hadjustment_binding =
      g_object_bind_property (self, "hadjustment",
                              child, "hadjustment",
                              G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
    self->vadjustment_binding =
      g_object_bind_property (self, "vadjustment",
                              child, "vadjustment",
                              G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
    self->hscroll_policy_binding =
      g_object_bind_property (self, "hscroll-policy",
                              child, "hscroll-policy",
                              G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
    self->vscroll_policy_binding =
      g_object_bind_property (self, "vscroll-policy",
                              child, "vscroll-policy",
                              G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  }

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_CHILD]);
}

/**
 * adw_clamp_scrollable_get_maximum_size:
 * @self: a #AdwClampScrollable
 *
 * Gets the maximum size to allocate to the contained child. It is the width if
 * @self is horizontal, or the height if it is vertical.
 *
 * Returns: the maximum width to allocate to the contained child.
 *
 * Since: 1.0
 */
gint
adw_clamp_scrollable_get_maximum_size (AdwClampScrollable *self)
{
  AdwClampLayout *layout;

  g_return_val_if_fail (ADW_IS_CLAMP_SCROLLABLE (self), 0);

  layout = ADW_CLAMP_LAYOUT (gtk_widget_get_layout_manager (GTK_WIDGET (self)));

  return adw_clamp_layout_get_maximum_size (layout);
}

/**
 * adw_clamp_scrollable_set_maximum_size:
 * @self: a #AdwClampScrollable
 * @maximum_size: the maximum size
 *
 * Sets the maximum size to allocate to the contained child. It is the width if
 * @self is horizontal, or the height if it is vertical.
 *
 * Since: 1.0
 */
void
adw_clamp_scrollable_set_maximum_size (AdwClampScrollable *self,
                                       gint                maximum_size)
{
  AdwClampLayout *layout;

  g_return_if_fail (ADW_IS_CLAMP_SCROLLABLE (self));

  layout = ADW_CLAMP_LAYOUT (gtk_widget_get_layout_manager (GTK_WIDGET (self)));

  if (adw_clamp_layout_get_maximum_size (layout) == maximum_size)
    return;

  adw_clamp_layout_set_maximum_size (layout, maximum_size);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_MAXIMUM_SIZE]);
}

/**
 * adw_clamp_scrollable_get_tightening_threshold:
 * @self: a #AdwClampScrollable
 *
 * Gets the size starting from which the clamp will tighten its grip on the
 * child.
 *
 * Returns: the size starting from which the clamp will tighten its grip on the
 * child.
 *
 * Since: 1.0
 */
gint
adw_clamp_scrollable_get_tightening_threshold (AdwClampScrollable *self)
{
  AdwClampLayout *layout;

  g_return_val_if_fail (ADW_IS_CLAMP_SCROLLABLE (self), 0);

  layout = ADW_CLAMP_LAYOUT (gtk_widget_get_layout_manager (GTK_WIDGET (self)));

  return adw_clamp_layout_get_tightening_threshold (layout);
}

/**
 * adw_clamp_scrollable_set_tightening_threshold:
 * @self: a #AdwClampScrollable
 * @tightening_threshold: the tightening threshold
 *
 * Sets the size starting from which the clamp will tighten its grip on the
 * child.
 *
 * Since: 1.0
 */
void
adw_clamp_scrollable_set_tightening_threshold (AdwClampScrollable *self,
                                               gint                tightening_threshold)
{
  AdwClampLayout *layout;

  g_return_if_fail (ADW_IS_CLAMP_SCROLLABLE (self));

  layout = ADW_CLAMP_LAYOUT (gtk_widget_get_layout_manager (GTK_WIDGET (self)));

  if (adw_clamp_layout_get_tightening_threshold (layout) == tightening_threshold)
    return;

  adw_clamp_layout_set_tightening_threshold (layout, tightening_threshold);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_TIGHTENING_THRESHOLD]);
}