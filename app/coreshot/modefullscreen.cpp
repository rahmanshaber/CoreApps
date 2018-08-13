/*
CoreBox give's a file's detail information.

CoreBox is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see {http://www.gnu.org/licenses/}. */

#include "modefullscreen.h"


ModeFullscreen::ModeFullscreen(QWidget *parent) : ScreenWidget(parent)
{
    setObjectName("Fullscreen Mode");
}


QPixmap ModeFullscreen::subscreen() const
{
    return screen();
}
