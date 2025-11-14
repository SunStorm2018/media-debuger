#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
# SPDX-License-Identifier: MIT

from gi.repository import Nautilus, GObject
import os
import subprocess

class OpenFolderExtension(GObject.GObject, Nautilus.MenuProvider):
    
    def __init__(self):
        super().__init__()
    
    def get_file_items(self, window, files):
        """为选中的文件添加菜单项"""
        if len(files) != 1:
            return []
        
        file = files[0]
        if file.is_directory():
            return [self._create_open_folder_item(file)]
        else:
            return [self._create_open_parent_item(file)]
    
    def get_background_items(self, window, file):
        """为空白处右键添加菜单项"""
        return [self._create_open_folder_item(file)]
    
    def _create_open_folder_item(self, file):
        """创建打开文件夹菜单项"""
        item = Nautilus.MenuItem(
            name="OpenFolderExtension::OpenInNewWindow",
            label="my---在新窗口中打开",
            tip="在新文件管理器窗口中打开此位置"
        )
        item.connect('activate', self._open_in_new_window, file)
        return item
    
    def _create_open_parent_item(self, file):
        """为文件创建打开父目录菜单项"""
        item = Nautilus.MenuItem(
            name="OpenFolderExtension::OpenParent",
            label="my---打开所在文件夹",
            tip="在文件管理器中打开此文件所在的文件夹"
        )
        item.connect('activate', self._open_parent_directory, file)
        return item
    
    def _open_in_new_window(self, menu, file):
        """打开新窗口的处理函数"""
        if file.is_directory():
            target_path = file.get_location().get_path()
        else:
            target_path = file.get_parent_location().get_path()
        
        subprocess.Popen(['nautilus', target_path])
    
    def _open_parent_directory(self, menu, file):
        """打开父目录的处理函数"""
        parent = file.get_parent_location()
        if parent:
            target_path = parent.get_path()
            subprocess.Popen(['nautilus', target_path])
