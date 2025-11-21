# SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
# SPDX-License-Identifier: MIT
dh_clean
rm *.o ui_* moc_* qrc_* Makefile MediaDebuger media-debuger *.qrc .qmake.stash media-debuger.debug media-debuger-qt*
rm -rf app_logs/ .qm/ 
rm -rf debian/.debhelper/ debian/media-debuger/ debian/files debian/debhelper-build-stamp debian/media-debuger.debhelper.log debian/media-debuger.substvars
rm -rf debian/*.log debian/*.substvars debian/media-debuger-dde-filemanager-contextmenu/ debian/media-debuger-nautilus-contextmenu/ debian/media-debuger-dbus
rm -rf build-qt5/ build-qt6/
clear
ls -al
