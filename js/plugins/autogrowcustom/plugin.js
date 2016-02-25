/**
 * @license Copyright (c) 2003-2016, CKSource - Frederico Knabben. All rights reserved.
 * For licensing, see LICENSE.md or http://ckeditor.com/license
 */

/**
 * @fileOverview The Auto Grow plugin.
 */

'use strict';

(function() {
  CKEDITOR.plugins.add( 'autogrowcustom', {
    init: function( editor ) {
      // This feature is available only for themed ui instance.
      if (editor.elementMode == CKEDITOR.ELEMENT_MODE_INLINE) {
        return;
      }

      editor.on('mode', function (evt) {
        if (editor.mode == 'wysiwyg') {
          // Simply set auto height with div wysiwyg.
          var editable = editor.editable();
          if (editable.isInline()) {
            var contents = editable.getParent(); // editor.ui.space('contents');
            contents.setStyle( 'height', 'auto' );
            // Provide min and max height.
            editable.setStyle('min-height', '80px');
            // ∆ IE8
            var innerHeight = (CKEDITOR.env.ie)
              ? document.documentElement.clientHeight
              : window.innerHeight;
            editable.setStyle('max-height', (innerHeight - 180)+'px');
            editable.setStyle('overflow-y', 'auto');
          }
        }
      });
    }
  });
})();

