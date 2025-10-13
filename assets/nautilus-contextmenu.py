# ~/.local/share/nautilus-python/extensions/
# /usr/share/nautilus-python/extensions/

#!/usr/bin/env python3
from gi.repository import Nautilus, GObject
import os
import subprocess
import json
import locale

class MediaDebuggerExtension(GObject.GObject, Nautilus.MenuProvider):
    
    def __init__(self):
        super().__init__()
        # 假设 media-debuger 可执行文件在系统路径中
        self.media_debuger_cmd = "media-debuger"
        # 本地化设置
        self.lang = self._get_system_language()
    
    def _get_system_language(self):
        """获取系统语言设置"""
        try:
            lang, _ = locale.getdefaultlocale()
            if lang and 'zh' in lang.lower():
                return 'zh'
        except:
            pass
        return 'en'
    
    def _t(self, en_text, zh_text):
        """简单的本地化函数"""
        return zh_text if self.lang == 'zh' else en_text
    
    def get_file_items(self, window, files):
        """为选中的媒体文件添加菜单项"""
        if len(files) != 1:
            return []
        
        file = files[0]
        if not file.is_directory() and self._is_media_file(file):
            return self._create_media_menu_items(file)
        return []
    
    def get_background_items(self, window, file):
        """为空白处右键添加菜单项 - 显示基础信息"""
        return self._create_basic_info_menu_items()
    
    def _is_media_file(self, file):
        """检查文件是否为媒体文件"""
        if file.is_directory():
            return False
            
        filename = file.get_name().lower()
        media_extensions = [
            '.mp4', '.avi', '.mkv', '.mov', '.wmv', '.flv', '.webm',
            '.mp3', '.wav', '.flac', '.aac', '.ogg', '.m4a',
            '.jpg', '.jpeg', '.png', '.bmp', '.gif', '.tiff'
        ]
        return any(filename.endswith(ext) for ext in media_extensions)
    
    def _create_media_menu_items(self, file):
        """为媒体文件创建菜单项"""
        menu_items = []
        
        # meida Info 子菜单
        meida_info_menu = Nautilus.Menu()
        meida_info_menu_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::MediaInfo",
            label=self._t("Meida Info", "媒体信息"),
            tip=self._t("Show media information", "显示媒体信息")
        )
        
        # streams Info
        streams_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::Streams",
            label=self._t("Streams", "流信息"),
            tip=self._t("Show video streams information", "显示流信息")
        )
        streams_item.connect('activate', self._show_streams_info, file)
        meida_info_menu.append_item(streams_item)
        
        # Frame Info - Audio
        format_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::FrameAudio",
            label=self._t("Formats", "格式"),
            tip=self._t("Show audio frame information", "显示格式信息")
        )
        format_item.connect('activate', self._show_format_info, file)
        meida_info_menu.append_item(format_item)
        
        meida_info_menu_item.set_submenu(meida_info_menu)
        menu_items.append(meida_info_menu_item)
        
        # Frame Info 子菜单
        frame_info_menu = Nautilus.Menu()
        frame_info_menu_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::FrameInfo",
            label=self._t("Frame Info", "帧信息"),
            tip=self._t("Show frame information", "显示帧信息")
        )
        
        # Frame Info - Video
        frame_video_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::FrameVideo",
            label=self._t("Video", "视频"),
            tip=self._t("Show video frame information", "显示视频帧信息")
        )
        frame_video_item.connect('activate', self._show_frame_video_info, file)
        frame_info_menu.append_item(frame_video_item)
        
        # Frame Info - Audio
        frame_audio_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::FrameAudio",
            label=self._t("Audio", "音频"),
            tip=self._t("Show audio frame information", "显示音频帧信息")
        )
        frame_audio_item.connect('activate', self._show_frame_audio_info, file)
        frame_info_menu.append_item(frame_audio_item)
        
        frame_info_menu_item.set_submenu(frame_info_menu)
        menu_items.append(frame_info_menu_item)
        
        # Packet Info 子菜单
        packet_info_menu = Nautilus.Menu()
        packet_info_menu_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::PacketInfo",
            label=self._t("Packet Info", "包信息"),
            tip=self._t("Show packet information", "显示包信息")
        )
        
        # Packet Info - Video
        packet_video_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::PacketVideo",
            label=self._t("Video", "视频"),
            tip=self._t("Show video packet information", "显示视频包信息")
        )
        packet_video_item.connect('activate', self._show_packet_video_info, file)
        packet_info_menu.append_item(packet_video_item)
        
        # Packet Info - Audio
        packet_audio_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::PacketAudio",
            label=self._t("Audio", "音频"),
            tip=self._t("Show audio packet information", "显示音频包信息")
        )
        packet_audio_item.connect('activate', self._show_packet_audio_info, file)
        packet_info_menu.append_item(packet_audio_item)
        
        packet_info_menu_item.set_submenu(packet_info_menu)
        menu_items.append(packet_info_menu_item)
        
        return menu_items
    
    def _create_basic_info_menu_items(self):
        """创建基础信息菜单项"""
        menu_items = []
        
        # Basic Info 子菜单
        basic_info_menu = Nautilus.Menu()
        basic_info_menu_item = Nautilus.MenuItem(
            name="MediaDebuggerExtension::BasicInfo",
            label=self._t("Basic Info", "基础信息"),
            tip=self._t("Show basic information", "显示基础信息")
        )
        
        # 完整的 basic_items 列表（中英文对照）
        basic_items = [
            (self._t("Version", "版本信息"), "version"),
            (self._t("Build Configuration", "构建配置"), "buildconf"),
            (self._t("Formats", "格式信息"), "formats"),
            (self._t("Muxers", "复用器"), "muxers"),
            (self._t("Demuxers", "解复用器"), "demuxers"),
            (self._t("Devices", "设备信息"), "devices"),
            (self._t("Codecs", "编解码器"), "codecs"),
            (self._t("Decoders", "解码器"), "decoders"),
            (self._t("Encoders", "编码器"), "encoders"),
            (self._t("Bitstream Filters", "比特流过滤器"), "bsfs"),
            (self._t("Protocols", "协议"), "protocols"),
            (self._t("Filters", "过滤器"), "filters"),
            (self._t("Pixel Formats", "像素格式"), "pixfmts"),
            (self._t("Channel Layouts", "声道布局"), "layouts"),
            (self._t("Sample Formats", "采样格式"), "samplefmts"),
            (self._t("Colors", "颜色信息"), "colors"),
            (self._t("License", "许可证信息"), "L")
        ]
        
        for label, cmd_type in basic_items:
            item = Nautilus.MenuItem(
                name=f"MediaDebuggerExtension::Basic{cmd_type}",
                label=label,
                tip=self._t(f"Show {cmd_type}", f"显示{label}")
            )
            item.connect('activate', self._show_basic_info, cmd_type)
            basic_info_menu.append_item(item)
        
        basic_info_menu_item.set_submenu(basic_info_menu)
        menu_items.append(basic_info_menu_item)
        
        return menu_items
    
    def _execute_media_debuger(self, args):
        """执行 media-debuger 命令（使用 GUI 界面）"""
        try:
            cmd = [self.media_debuger_cmd] + args
            # 使用 subprocess.Popen 启动 GUI 应用，不等待完成
            subprocess.Popen(cmd)
            return True
        except FileNotFoundError:
            print(f"Error: {self.media_debuger_cmd} not found in PATH")
            return False
        except Exception as e:
            print(f"Error executing media-debuger: {e}")
            return False
    
    def _show_basic_info(self, menu, basic_type):
        """显示基础信息（使用 GUI）"""
        args = ["-b", basic_type]
        self._execute_media_debuger(args)

    def _show_streams_info(self, menu, file):
        """显示视频流信息（使用 GUI）"""
        file_path = file.get_location().get_path()
        args = ["-m", file_path, "--streams"]
        self._execute_media_debuger(args)
    
    def _show_format_info(self, menu, file):
        """显示视频格式信息（使用 GUI）"""
        file_path = file.get_location().get_path()
        args = ["-m", file_path, "--format"]
        self._execute_media_debuger(args)
    
    def _show_frame_video_info(self, menu, file):
        """显示视频帧信息（使用 GUI）"""
        file_path = file.get_location().get_path()
        args = ["-m", file_path, "-s", "v", "-f", "f"]
        self._execute_media_debuger(args)
    
    def _show_frame_audio_info(self, menu, file):
        """显示音频帧信息（使用 GUI）"""
        file_path = file.get_location().get_path()
        args = ["-m", file_path, "-s", "a", "-f", "f"]
        self._execute_media_debuger(args)
    
    def _show_packet_video_info(self, menu, file):
        """显示视频包信息（使用 GUI）"""
        file_path = file.get_location().get_path()
        args = ["-m", file_path, "-s", "v", "-f", "p"]
        self._execute_media_debuger(args)
    
    def _show_packet_audio_info(self, menu, file):
        """显示音频包信息（使用 GUI）"""
        file_path = file.get_location().get_path()
        args = ["-m", file_path, "-s", "a", "-f", "p"]
        self._execute_media_debuger(args)
