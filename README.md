Tri State Switch
================

_What if CheckBox and Switch were one and the same?_

CheckBox and Switch are inherently different UI controls:
- CheckBox may be 'tristate': a third indeterminate state called PartiallyChecked represents group selection where neither all or none of the items in the group are selected.
- Switch is more interactive (except on Android), with an intermediate state of user dragging the knob around.

But what if they were combined to get the best of both worlds?

Behold: an **Өчпочмак-shaped triangle switch!**

- Fully compliant with QtQuick.Templates architecture.
- Comes with a default style implementation with animated knob icons.

Building
========

The project uses a standard CMake setup. But it links with Qt's private libraries and includes their private headers (which is the only way to have a reasonable interactive UX in Qt), therefore you may run into issues when building against any Qt version other than `6.9.2`.

License
=======

All Rights Reserved

NO ONE SHALL USE THE SOFTWARE FOR ANY PRACTICAL PURPOSE OF ANY KIND.
YOU'VE BEEN WARNED.

```
 _|  _      __   _  _|_
(_| (_)     | | (_)  |_
```
