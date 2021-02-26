/*
 * Copyright (C) 2019 Alexander Mikhaylenko <exalm7659@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#pragma once

#if !defined(_ADWAITA_INSIDE) && !defined(ADWAITA_COMPILATION)
#error "Only <adwaita.h> can be included directly."
#endif

#include "adw-version.h"

#include <gtk/gtk.h>
#include "adw-navigation-direction.h"

G_BEGIN_DECLS

#define ADW_TYPE_SWIPEABLE (adw_swipeable_get_type ())

ADW_AVAILABLE_IN_ALL
G_DECLARE_INTERFACE (AdwSwipeable, adw_swipeable, ADW, SWIPEABLE, GtkWidget)

/**
 * AdwSwipeableInterface:
 * @parent: The parent interface.
 * @get_distance: Gets the swipe distance.
 * @get_snap_points: Gets the snap points
 * @get_progress: Gets the current progress.
 * @get_cancel_progress: Gets the cancel progress.
 * @get_swipe_area: Gets the swipeable rectangle.
 *
 * An interface for swipeable widgets.
 *
 * Since: 1.0
 **/
struct _AdwSwipeableInterface
{
  GTypeInterface parent;

  double            (*get_distance)        (AdwSwipeable *self);
  double *          (*get_snap_points)     (AdwSwipeable *self,
                                            int          *n_snap_points);
  double            (*get_progress)        (AdwSwipeable *self);
  double            (*get_cancel_progress) (AdwSwipeable *self);
  void              (*get_swipe_area)      (AdwSwipeable           *self,
                                            AdwNavigationDirection  navigation_direction,
                                            gboolean                is_drag,
                                            GdkRectangle           *rect);

  /*< private >*/
  gpointer padding[4];
};

ADW_AVAILABLE_IN_ALL
double           adw_swipeable_get_distance        (AdwSwipeable *self);
ADW_AVAILABLE_IN_ALL
double          *adw_swipeable_get_snap_points     (AdwSwipeable *self,
                                                    int          *n_snap_points);
ADW_AVAILABLE_IN_ALL
double           adw_swipeable_get_progress        (AdwSwipeable *self);
ADW_AVAILABLE_IN_ALL
double           adw_swipeable_get_cancel_progress (AdwSwipeable *self);
ADW_AVAILABLE_IN_ALL
void             adw_swipeable_get_swipe_area      (AdwSwipeable           *self,
                                                    AdwNavigationDirection  navigation_direction,
                                                    gboolean                is_drag,
                                                    GdkRectangle           *rect);

G_END_DECLS
