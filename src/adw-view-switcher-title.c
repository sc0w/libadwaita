/*
 * Copyright (C) 2019 Zander Brown <zbrown@gnome.org>
 * Copyright (C) 2019 Purism SPC
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"

#include "adw-view-switcher-title.h"
#include "adw-squeezer.h"
#include "adw-window-title.h"

/**
 * AdwViewSwitcherTitle:
 *
 * A view switcher title.
 *
 * A widget letting you switch between multiple views contained by a
 * [class@Gtk.Stack] via an [class@Adw.ViewSwitcher].
 *
 * It is designed to be used as the title widget of a [class@Adw.HeaderBar], and
 * will display the window's title when the window is too narrow to fit the view
 * switcher e.g. on mobile phones, or if there are less than two views.
 *
 * You can conveniently bind the [property@Adw.ViewSwitcherBar:reveal] property
 * to [property@Adw.ViewSwitcherTitle:title-visible] to automatically reveal the
 * view switcher bar when the title label is displayed in place of the view
 * switcher.
 *
 * An example of the UI definition for a common use case:
 *
 * ```xml
 * <object class="GtkWindow"/>
 *   <child type="titlebar">
 *     <object class="AdwHeaderBar">
 *       <property name="centering-policy">strict</property>
 *       <child type="title">
 *         <object class="AdwViewSwitcherTitle" id="title">
 *           <property name="stack">stack</property>
 *         </object>
 *       </child>
 *     </object>
 *   </child>
 *   <child>
 *     <object class="GtkBox">
 *       <child>
 *         <object class="GtkStack" id="stack"/>
 *       </child>
 *       <child>
 *         <object class="AdwViewSwitcherBar">
 *           <property name="stack">stack</property>
 *           <binding name="reveal">
 *             <lookup name="title-visible">title</lookup>
 *           </binding>
 *         </object>
 *       </child>
 *     </object>
 *   </child>
 * </object>
 * ```
 *
 * ## CSS nodes
 *
 * `AdwViewSwitcherTitle` has a single CSS node with name `viewswitchertitle`.
 *
 * Since: 1.0
 */

enum {
  PROP_0,
  PROP_POLICY,
  PROP_STACK,
  PROP_TITLE,
  PROP_SUBTITLE,
  PROP_VIEW_SWITCHER_ENABLED,
  PROP_TITLE_VISIBLE,
  LAST_PROP,
};

struct _AdwViewSwitcherTitle
{
  GtkWidget parent_instance;

  AdwSqueezer *squeezer;
  AdwWindowTitle *title_widget;
  AdwViewSwitcher *view_switcher;

  gboolean view_switcher_enabled;
  GtkSelectionModel *pages;
};

static GParamSpec *props[LAST_PROP];

G_DEFINE_TYPE (AdwViewSwitcherTitle, adw_view_switcher_title, GTK_TYPE_WIDGET)

static void
update_view_switcher_visible (AdwViewSwitcherTitle *self)
{
  AdwSqueezerPage *switcher_page;
  int count = 0;

  if (!self->squeezer)
    return;

  if (self->view_switcher_enabled && self->pages) {
    guint i, n;

    n = g_list_model_get_n_items (G_LIST_MODEL (self->pages));
    for (i = 0; i < n; i++) {
      GtkStackPage *page = g_list_model_get_item (G_LIST_MODEL (self->pages), i);

      if (gtk_stack_page_get_visible (page))
        count++;
    }
  }

  switcher_page = adw_squeezer_get_page (self->squeezer, GTK_WIDGET (self->view_switcher));
  adw_squeezer_page_set_enabled (switcher_page, count > 1);
}

static void
notify_squeezer_visible_child_cb (GObject *self)
{
  g_object_notify_by_pspec (self, props[PROP_TITLE_VISIBLE]);
}

static void
adw_view_switcher_title_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  AdwViewSwitcherTitle *self = ADW_VIEW_SWITCHER_TITLE (object);

  switch (prop_id) {
  case PROP_POLICY:
    g_value_set_enum (value, adw_view_switcher_title_get_policy (self));
    break;
  case PROP_STACK:
    g_value_set_object (value, adw_view_switcher_title_get_stack (self));
    break;
  case PROP_TITLE:
    g_value_set_string (value, adw_view_switcher_title_get_title (self));
    break;
  case PROP_SUBTITLE:
    g_value_set_string (value, adw_view_switcher_title_get_subtitle (self));
    break;
  case PROP_VIEW_SWITCHER_ENABLED:
    g_value_set_boolean (value, adw_view_switcher_title_get_view_switcher_enabled (self));
    break;
  case PROP_TITLE_VISIBLE:
    g_value_set_boolean (value, adw_view_switcher_title_get_title_visible (self));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
adw_view_switcher_title_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  AdwViewSwitcherTitle *self = ADW_VIEW_SWITCHER_TITLE (object);

  switch (prop_id) {
  case PROP_POLICY:
    adw_view_switcher_title_set_policy (self, g_value_get_enum (value));
    break;
  case PROP_STACK:
    adw_view_switcher_title_set_stack (self, g_value_get_object (value));
    break;
  case PROP_TITLE:
    adw_view_switcher_title_set_title (self, g_value_get_string (value));
    break;
  case PROP_SUBTITLE:
    adw_view_switcher_title_set_subtitle (self, g_value_get_string (value));
    break;
  case PROP_VIEW_SWITCHER_ENABLED:
    adw_view_switcher_title_set_view_switcher_enabled (self, g_value_get_boolean (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
adw_view_switcher_title_dispose (GObject *object) {
  AdwViewSwitcherTitle *self = (AdwViewSwitcherTitle *)object;

  if (self->pages)
    g_signal_handlers_disconnect_by_func (self->pages, G_CALLBACK (update_view_switcher_visible), self);

  if (self->squeezer)
    gtk_widget_unparent (GTK_WIDGET (self->squeezer));

  G_OBJECT_CLASS (adw_view_switcher_title_parent_class)->dispose (object);
}

static void
adw_view_switcher_title_class_init (AdwViewSwitcherTitleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = adw_view_switcher_title_dispose;
  object_class->get_property = adw_view_switcher_title_get_property;
  object_class->set_property = adw_view_switcher_title_set_property;

  /**
   * AdwViewSwitcherTitle:policy: (attributes org.gtk.Property.get=adw_view_switcher_title_get_policy org.gtk.Property.set=adw_view_switcher_title_set_policy)
   *
   * The policy to determine which mode to use.
   *
   * Since: 1.0
   */
  props[PROP_POLICY] =
    g_param_spec_enum ("policy",
                       "Policy",
                       "The policy to determine the mode to use",
                       ADW_TYPE_VIEW_SWITCHER_POLICY,
                       ADW_VIEW_SWITCHER_POLICY_AUTO,
                       G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * AdwViewSwitcherTitle:stack: (attributes org.gtk.Property.get=adw_view_switcher_title_get_stack org.gtk.Property.set=adw_view_switcher_title_set_stack)
   *
   * The stack the view switcher controls.
   *
   * Since: 1.0
   */
  props[PROP_STACK] =
    g_param_spec_object ("stack",
                         "Stack",
                         "The stack the view switcher controls",
                         GTK_TYPE_STACK,
                         G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * AdwViewSwitcherTitle:title: (attributes org.gtk.Property.get=adw_view_switcher_title_get_title org.gtk.Property.set=adw_view_switcher_title_set_title)
   *
   * The title to display.
   *
   * The title should give a user additional details. A good title should not
   * include the application name.
   *
   * Since: 1.0
   */
  props[PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "The title to display",
                         NULL,
                         G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * AdwViewSwitcherTitle:subtitle: (attributes org.gtk.Property.get=adw_view_switcher_title_get_subtitle org.gtk.Property.set=adw_view_switcher_title_set_subtitle)
   *
   * The subtitle to display.
   *
   * The subtitle should give a user additional details.
   *
   * Since: 1.0
   */
  props[PROP_SUBTITLE] =
    g_param_spec_string ("subtitle",
                         "Subtitle",
                         "The subtitle to display",
                         NULL,
                         G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * AdwViewSwitcherTitle:view-switcher-enabled: (attributes org.gtk.Property.get=adw_view_switcher_title_get_view_switcher_enabled org.gtk.Property.set=adw_view_switcher_title_set_view_switcher_enabled)
   *
   * Whether the view switcher is enabled.
   *
   * If it is disabled, the title will be displayed instead. This allows to
   * programmatically hide the view switcher even if it fits in the available
   * space.
   *
   * This can be used e.g. to ensure the view switcher is hidden below a certain
   * window width, or any other constraint you find suitable.
   *
   * Since: 1.0
   */
  props[PROP_VIEW_SWITCHER_ENABLED] =
    g_param_spec_boolean ("view-switcher-enabled",
                         "View switcher enabled",
                         "Whether the view switcher is enabled",
                         TRUE,
                         G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * AdwViewSwitcherTitle:title-visible: (attributes org.gtk.Property.get=adw_view_switcher_title_get_title_visible)
   *
   * Whether the title is currently visible.
   *
   * If the title is visible, it means the view switcher is hidden an it may be
   * wanted to show an alternative switcher, e.g. a [class@Adw.ViewSwitcherBar].
   *
   * Since: 1.0
   */
  props[PROP_TITLE_VISIBLE] =
    g_param_spec_boolean ("title-visible",
                         "Title visible",
                         "Whether the title is currently visible",
                         TRUE,
                         G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, LAST_PROP, props);

  gtk_widget_class_set_css_name (widget_class, "viewswitchertitle");
  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BIN_LAYOUT);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/Adwaita/ui/adw-view-switcher-title.ui");
  gtk_widget_class_bind_template_child (widget_class, AdwViewSwitcherTitle, squeezer);
  gtk_widget_class_bind_template_child (widget_class, AdwViewSwitcherTitle, title_widget);
  gtk_widget_class_bind_template_child (widget_class, AdwViewSwitcherTitle, view_switcher);
  gtk_widget_class_bind_template_callback (widget_class, notify_squeezer_visible_child_cb);
}

static void
adw_view_switcher_title_init (AdwViewSwitcherTitle *self)
{
  /* This must be initialized before the template so the embedded view switcher
   * can pick up the correct default value.
   */
  self->view_switcher_enabled = TRUE;

  gtk_widget_init_template (GTK_WIDGET (self));

  update_view_switcher_visible (self);
}

/**
 * adw_view_switcher_title_new:
 *
 * Creates a new `AdwViewSwitcherTitle`.
 *
 * Returns: the newly created `AdwViewSwitcherTitle`
 *
 * Since: 1.0
 */
GtkWidget *
adw_view_switcher_title_new (void)
{
  return g_object_new (ADW_TYPE_VIEW_SWITCHER_TITLE, NULL);
}

/**
 * adw_view_switcher_title_get_policy: (attributes org.gtk.Method.get_property=policy)
 * @self: a `AdwViewSwitcherTitle`
 *
 * Gets the policy of @self.
 *
 * Returns: the policy of @self
 *
 * Since: 1.0
 */
AdwViewSwitcherPolicy
adw_view_switcher_title_get_policy (AdwViewSwitcherTitle *self)
{
  g_return_val_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self), ADW_VIEW_SWITCHER_POLICY_NARROW);

  return adw_view_switcher_get_policy (self->view_switcher);
}

/**
 * adw_view_switcher_title_set_policy: (attributes org.gtk.Method.set_property=policy)
 * @self: a `AdwViewSwitcherTitle`
 * @policy: the new policy
 *
 * Sets the policy of @self.
 *
 * Since: 1.0
 */
void
adw_view_switcher_title_set_policy (AdwViewSwitcherTitle  *self,
                                    AdwViewSwitcherPolicy  policy)
{
  g_return_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self));

  if (adw_view_switcher_get_policy (self->view_switcher) == policy)
    return;

  adw_view_switcher_set_policy (self->view_switcher, policy);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_POLICY]);

  gtk_widget_queue_resize (GTK_WIDGET (self));
}

/**
 * adw_view_switcher_title_get_stack: (attributes org.gtk.Method.get_property=stack)
 * @self: a `AdwViewSwitcherTitle`
 *
 * Gets the stack controlled by @self.
 *
 * Returns: (nullable) (transfer none): the stack
 *
 * Since: 1.0
 */
GtkStack *
adw_view_switcher_title_get_stack (AdwViewSwitcherTitle *self)
{
  g_return_val_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self), NULL);

  return adw_view_switcher_get_stack (self->view_switcher);
}

/**
 * adw_view_switcher_title_set_stack: (attributes org.gtk.Method.set_property=stack)
 * @self: a `AdwViewSwitcherTitle`
 * @stack: (nullable): a stack
 *
 * Sets the stack controlled by @self.
 *
 * Since: 1.0
 */
void
adw_view_switcher_title_set_stack (AdwViewSwitcherTitle *self,
                                   GtkStack             *stack)
{
  GtkStack *previous_stack;

  g_return_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self));
  g_return_if_fail (stack == NULL || GTK_IS_STACK (stack));

  previous_stack = adw_view_switcher_get_stack (self->view_switcher);

  if (previous_stack == stack)
    return;

  if (previous_stack) {
    g_signal_handlers_disconnect_by_func (self->pages, G_CALLBACK (update_view_switcher_visible), self);
    g_clear_object (&self->pages);
  }

  adw_view_switcher_set_stack (self->view_switcher, stack);

  if (stack) {
    self->pages = gtk_stack_get_pages (stack);

    g_signal_connect_swapped (self->pages, "items-changed", G_CALLBACK (update_view_switcher_visible), self);
  }

  update_view_switcher_visible (self);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_STACK]);
}

/**
 * adw_view_switcher_title_get_title: (attributes org.gtk.Method.get_property=title)
 * @self: a `AdwViewSwitcherTitle`
 *
 * Gets the title of @self.
 *
 * Returns: (nullable): the title
 *
 * Since: 1.0
 */
const char *
adw_view_switcher_title_get_title (AdwViewSwitcherTitle *self)
{
  g_return_val_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self), NULL);

  return adw_window_title_get_title (self->title_widget);
}

/**
 * adw_view_switcher_title_set_title: (attributes org.gtk.Method.set_property=title)
 * @self: a `AdwViewSwitcherTitle`
 * @title: (nullable): a title
 *
 * Sets the title of @self.
 *
 * Since: 1.0
 */
void
adw_view_switcher_title_set_title (AdwViewSwitcherTitle *self,
                                   const char           *title)
{
  g_return_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self));

  if (g_strcmp0 (adw_window_title_get_title (self->title_widget), title) == 0)
    return;

  adw_window_title_set_title (self->title_widget, title);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_TITLE]);
}

/**
 * adw_view_switcher_title_get_subtitle: (attributes org.gtk.Method.get_property=subtitle)
 * @self: a `AdwViewSwitcherTitle`
 *
 * Gets the subtitle of @self.
 *
 * Returns: (nullable): the subtitle
 *
 * Since: 1.0
 */
const char *
adw_view_switcher_title_get_subtitle (AdwViewSwitcherTitle *self)
{
  g_return_val_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self), NULL);

  return adw_window_title_get_subtitle (self->title_widget);
}

/**
 * adw_view_switcher_title_set_subtitle: (attributes org.gtk.Method.get_property=subtitle)
 * @self: a `AdwViewSwitcherTitle`
 * @subtitle: (nullable): a subtitle
 *
 * Sets the subtitle of @self.
 *
 * Since: 1.0
 */
void
adw_view_switcher_title_set_subtitle (AdwViewSwitcherTitle *self,
                                      const char           *subtitle)
{
  g_return_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self));

  if (g_strcmp0 (adw_window_title_get_subtitle (self->title_widget), subtitle) == 0)
    return;

  adw_window_title_set_subtitle (self->title_widget, subtitle);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_SUBTITLE]);
}

/**
 * adw_view_switcher_title_get_view_switcher_enabled: (attributes org.gtk.Method.get_property=view-switcher-enabled)
 * @self: a `AdwViewSwitcherTitle`
 *
 * Gets whether @self's view switcher is enabled.
 *
 * Returns: whether the view switcher is enabled
 *
 * Since: 1.0
 */
gboolean
adw_view_switcher_title_get_view_switcher_enabled (AdwViewSwitcherTitle *self)
{
  g_return_val_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self), FALSE);

  return self->view_switcher_enabled;
}

/**
 * adw_view_switcher_title_set_view_switcher_enabled: (attributes org.gtk.Method.set_property=view-switcher-enabled)
 * @self: a `AdwViewSwitcherTitle`
 * @enabled: whether the view switcher is enabled
 *
 * Sets whether @self's view switcher is enabled.
 *
 * Since: 1.0
 */
void
adw_view_switcher_title_set_view_switcher_enabled (AdwViewSwitcherTitle *self,
                                                   gboolean              enabled)
{
  g_return_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self));

  enabled = !!enabled;

  if (self->view_switcher_enabled == enabled)
    return;

  self->view_switcher_enabled = enabled;
  update_view_switcher_visible (self);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_VIEW_SWITCHER_ENABLED]);
}

/**
 * adw_view_switcher_title_get_title_visible: (attributes org.gtk.Method.get_property=title-visible)
 * @self: a `AdwViewSwitcherTitle`
 *
 * Gets whether the title of @self is currently visible.
 *
 * Returns: whether the title of @self is currently visible
 *
 * Since: 1.0
 */
gboolean
adw_view_switcher_title_get_title_visible (AdwViewSwitcherTitle *self)
{
  g_return_val_if_fail (ADW_IS_VIEW_SWITCHER_TITLE (self), FALSE);

  return adw_squeezer_get_visible_child (self->squeezer) == GTK_WIDGET (self->title_widget);
}
