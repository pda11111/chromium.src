// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/bookmarks/bookmark_drag_drop.h"

#import <Cocoa/Cocoa.h>

#include <cmath>

#include "base/logging.h"
#include "base/mac/scoped_nsobject.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/string16.h"
#include "base/strings/sys_string_conversions.h"
#include "chrome/browser/bookmarks/bookmark_model_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/bookmarks/bookmark_drag_drop.h"
#import "chrome/browser/ui/cocoa/bookmarks/bookmark_bar_controller.h"
#import "chrome/browser/ui/cocoa/drag_util.h"
#include "components/bookmarks/browser/bookmark_model.h"
#include "components/bookmarks/browser/bookmark_node_data.h"
#include "ui/base/dragdrop/drag_drop_types.h"

namespace {

// Make a drag image from the drop data.
NSImage* MakeDragImage(BookmarkModel* model,
                       const std::vector<const BookmarkNode*>& nodes) {
  if (nodes.size() == 1) {
    const BookmarkNode* node = nodes[0];
    const gfx::Image& favicon = model->GetFavicon(node);
    return drag_util::DragImageForBookmark(
        favicon.IsEmpty() ? nil : favicon.ToNSImage(),
        node->GetTitle(),
        bookmarks::kDefaultBookmarkWidth);
  } else {
    // TODO(feldstein): Do something better than this. Should have badging
    // and a single drag image.
    // http://crbug.com/37264
    return [NSImage imageNamed:NSImageNameMultipleDocuments];
  }
}

}  // namespace

namespace chrome {

void DragBookmarks(Profile* profile,
                   const std::vector<const BookmarkNode*>& nodes,
                   gfx::NativeView view,
                   ui::DragDropTypes::DragEventSource source) {
  DCHECK(!nodes.empty());

  // Allow nested message loop so we get DnD events as we drag this around.
  bool was_nested = base::MessageLoop::current()->IsNested();
  base::MessageLoop::current()->SetNestableTasksAllowed(true);

  bookmarks::BookmarkNodeData drag_data(nodes);
  drag_data.SetOriginatingProfilePath(profile->GetPath());
  drag_data.WriteToClipboard(ui::CLIPBOARD_TYPE_DRAG);

  // Synthesize an event for dragging, since we can't be sure that
  // [NSApp currentEvent] will return a valid dragging event.
  NSWindow* window = [view window];
  NSPoint position = [window mouseLocationOutsideOfEventStream];
  NSTimeInterval event_time = [[NSApp currentEvent] timestamp];
  NSEvent* drag_event = [NSEvent mouseEventWithType:NSLeftMouseDragged
                                           location:position
                                      modifierFlags:NSLeftMouseDraggedMask
                                          timestamp:event_time
                                       windowNumber:[window windowNumber]
                                            context:nil
                                        eventNumber:0
                                         clickCount:1
                                           pressure:1.0];

  // TODO(avi): Do better than this offset.
  NSImage* drag_image = MakeDragImage(
      BookmarkModelFactory::GetForProfile(profile), nodes);
  NSSize image_size = [drag_image size];
  position.x -= std::floor(image_size.width / 2);
  position.y -= std::floor(image_size.height / 5);
  [window dragImage:drag_image
                 at:position
             offset:NSZeroSize
              event:drag_event
         pasteboard:[NSPasteboard pasteboardWithName:NSDragPboard]
             source:nil
          slideBack:YES];

  base::MessageLoop::current()->SetNestableTasksAllowed(was_nested);
}

}  // namespace chrome
