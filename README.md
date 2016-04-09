# CKEditor addon
Freedom, fixes, and enhancements to Backdrop core CKEditor

Backdrop 1.2 has jailed CKEditor in core, with critical bugs, reduced functionality, and loss of customization possibility.

## Upgrade
Upgrade to a custom built CKEditor 4.5.5

## Fixes
* Add a link to an image
* Restore Anchor functionality (via `id` attribute)

## Additions
* The save button !
* A backdrop compliant implementation of the CKEditor uploadimage plugin, for direct upload
* Codemirror and Codesnippet plugins

## Enhancements
* Image Styles integration
* The Link plugin supports Link Attributes
  * title (for tooltips)
  * class and rel (for lightboxes)
  * and id (to define anchors))

Hopefully, the bugs will be fixed in Backdrop 1.4, but this project aims to stay as a template for customizing CKEditor integration.

There is no UI, just regular Backdrop hooks.
