# FFprobe 命令汇总

参考地址：https://ffmpeg.org/ffprobe-all.html

## 一、show_entries 

### 1. 概述

#### 1.1 容器和格式相关

- `format` - 显示容器格式信息
- `chapters` - 显示章节信息
- `chapter` - 单个章节信息
- `programs` - 显示程序信息
- `program` - 单个程序信息

#### 1.2 流相关

- `streams` - 显示所有流信息
- `stream` - 单个流信息
- `stream_groups` - 显示流组信息
- `stream_group` - 单个流组信息

#### 1.3 帧和包相关

- `frames` - 显示帧信息
- `frame` - 单个帧信息
- `packets` - 显示包信息
- `packet` - 单个包信息
- `packets_and_frames` - 显示包和帧信息

#### 1.4 其他

- `library_versions` - 显示库版本信息
- `pixel_formats` - 显示像素格式信息
- `error` - 显示错误信息

#### 1.5 使用语法

```bash
show_entries 的语法格式为： section_name=entry1,entry2:other_section=entry3,entry4
```

- 使用 `:` 分隔不同的 section
- 使用 `=` 指定 section 内的特定条目
- 使用 `,` 分隔同一 section 内的多个条目
- 如果只指定 section 名称而不加 `=`，则显示该 section 的所有条目

#### 1.6 示例

```bash
# 显示流的索引和编解码器类型，包的 PTS 时间和流索引  
ffprobe -show_entries packet=pts_time,duration_time,stream_index:stream=index,codec_type  
  
# 显示 format section 的所有信息，stream section 只显示编解码器类型  
ffprobe -show_entries format:stream=codec_type  
  
# 显示流和格式的所有标签  
ffprobe -show_entries stream_tags:format_tags
```



### 2. 详细表格

#### 2.1 主要 SECTION 类型

名称|	描述	|主要 ENTRY 参数
--|--|--
format|	容器格式信息	|format_name, format_long_name, start_time, duration, size, bit_rate, probe_score, nb_streams, nb_programs
streams	|流信息|	index, codec_name, codec_long_name, codec_type, codec_tag_string, codec_tag, width, height, coded_width, coded_height, sample_aspect_ratio, display_aspect_ratio, pix_fmt, level, color_range, color_space, color_trc, color_primaries, chroma_location, field_order, refs, sample_fmt, sample_rate, channels, channel_layout, bits_per_sample, initial_padding, id, r_frame_rate, avg_frame_rate, time_base, start_pts, start_time, duration_ts, duration, bit_rate, max_bit_rate, bits_per_raw_sample, nb_frames, nb_read_frames, nb_read_packets
packets	|包信息|	codec_type, stream_index, pts, pts_time, dts, dts_time, duration, duration_time, size, pos, flags, data, data_hash
frames	|帧信息|	media_type, stream_index, key_frame, pts, pts_time, pkt_dts, pkt_dts_time, best_effort_timestamp, best_effort_timestamp_time, pkt_duration, pkt_duration_time, pkt_pos, pkt_size, width, height, pix_fmt, sample_aspect_ratio, pict_type, coded_picture_number, display_picture_number, interlaced_frame, top_field_first, repeat_pict, chroma_location, color_range, color_space, color_primaries, color_trc
chapters	|章节信息|	id, time_base, start, start_time, end, end_time
programs	|程序信息|	program_id, program_num, nb_streams, pmt_pid, pcr_pid
stream_groups	|流组信息|	index, id, nb_streams, type

#### 2.2  标签相关 

名称	|描述|	ENTRY 参数
--|--|--
format_tags	|格式标签|	任意标签键名（如 title, artist, album 等）
stream_tags|	流标签|	任意标签键名（如 title, language, handler_name 等）
chapter_tags|	章节标签|	任意标签键名
program_tags|	程序标签|	任意标签键名
stream_group_tags|	流组标签|	任意标签键名
packet_tags|	包标签|	任意标签键名
frame_tags|	帧标签|	任意标签键名

#### 2.3 附加数据相关

名称|	描述|	ENTRY 参数
--|--|--
stream_side_data_list|	流附加数据列表|	side_data_type, side_data_size
frame_side_data_list|	帧附加数据列表|	side_data_type, side_data_size
packet_side_data_list|	包附加数据列表|	side_data_type, side_data_size

#### 2.4 配置相关

名称|	描述|	ENTRY 参数
--|--|--
stream_disposition|	流配置标志|	default, dub, original, comment, lyrics, karaoke, forced, hearing_impaired, visual_impaired, clean_effects, attached_pic, timed_thumbnails, captions, descriptions, metadata, dependent, still_image
stream_group_disposition|	流组配置标志|	同 stream_disposition
program_stream_disposition|	程序流配置标志|	同 stream_disposition

#### 2.5 版本和系统信息

名称|	描述|	ENTRY 参数
--|--|--
library_versions|	库版本信息|	name, major, minor, micro, version, ident
program_version|	程序版本信息|	version, copyright, compiler_ident, configuration
pixel_formats|	像素格式信息|	name, nb_components, log2_chroma_w, log2_chroma_h, bit_depths, flags
error|	错误信息|	code, string

这些 SECTION 和 ENTRY 定义来自 ffprobe 源码中的 SectionID 枚举和相应的显示函数。 ffprobe.c:157-224 具体的 ENTRY 参数在各个 show_* 函数中实现，例如 show_packet 函数定义了 packet section 的所有可用条目。 ffprobe.c:1212-1227 流信息的 ENTRY 参数在 show_stream 函数中定义，包括视频、音频和字幕的不同参数。 ffprobe.c:1750-1839



## 二、show_frames

### 1.  概述

使用语法

```bash
# 以JSON格式输出帧信息  
ffprobe -show_frames -output_format json input.mp4  
  
# 显示帧信息并包含数据哈希  
ffprobe -show_frames -show_data_hash md5 input.mp4  
  
# 显示特定条目的帧信息  
ffprobe -show_entries frame=width,height,pict_type input.mp4
```


### 2. 示例

#### 2.1 音频用例
```bash
# 命令
ffprobe -show_entries frame -of xml bensound-sunny.mp3

# 音频输出
<frame media_type="audio" stream_index="0" key_frame="1" pkt_pts="1960427520" pkt_pts_time="138.919184" pkt_dts="1960427520" pkt_dts_time="138.919184" best_effort_timestamp="1960427520" best_effort_timestamp_time="138.919184" pkt_duration="368640" pkt_duration_time="0.026122" pkt_pos="1944869" pkt_size="366" sample_fmt="fltp" nb_samples="1152" channels="2" channel_layout="stereo"/>

<frame media_type="audio" stream_index="0" key_frame="1" pkt_pts="1975541760" pkt_pts_time="139.990204" pkt_dts="1975541760" pkt_dts_time="139.990204" best_effort_timestamp="1975541760" best_effort_timestamp_time="139.990204" pkt_duration="368640" pkt_duration_time="0.026122" pkt_pos="1959863" pkt_size="366" sample_fmt="fltp" nb_samples="1152" channels="2" channel_layout="stereo"/>
```

#### 2.1 视频用例
```bash
# 命令
ffprobe -show_entries frame -of xml 20250627_152516_3.mp4

# 视频输出
<frame media_type="video" stream_index="0" key_frame="0" pkt_pts="7310" pkt_pts_time="7.310000" pkt_dts="7607" pkt_dts_time="7.607000" best_effort_timestamp="7607" best_effort_timestamp_time="7.607000" pkt_duration="111" pkt_duration_time="0.111000" pkt_pos="3099497" pkt_size="10722" width="8192" height="2048" pix_fmt="yuv420p" pict_type="B" coded_picture_number="74" display_picture_number="0" interlaced_frame="0" top_field_first="0" repeat_pict="0" chroma_location="left"/>

<frame media_type="video" stream_index="0" key_frame="0" pkt_pts="7607" pkt_pts_time="7.607000" pkt_dts="7729" pkt_dts_time="7.729000" best_effort_timestamp="7729" best_effort_timestamp_time="7.729000" pkt_duration="122" pkt_duration_time="0.122000" pkt_pos="3125536" pkt_size="5513" width="8192" height="2048" pix_fmt="yuv420p" pict_type="B" coded_picture_number="77" display_picture_number="0" interlaced_frame="0" top_field_first="0" repeat_pict="0" chroma_location="left"/>
```

### 2. 详细表格

#### 2.1 通用帧参数

| 参数名                             | 中文名称     | 描述                             |
| ---------------------------------- | ------------ | -------------------------------- |
| media_type                         | 媒体类型     | 媒体流的类型（视频/音频/字幕等） |
| stream_index                       | 流索引       | 帧所属的流索引号                 |
| key_frame                          | 关键帧       | 是否为关键帧（1或0）             |
| pts（pkt_pts）                     | 显示时间戳   | 帧的显示时间戳                   |
| pts_time（pkt_pts_time）           | 显示时间     | 显示时间戳转换为秒               |
| pkt_dts                            | 包解码时间戳 | 包的解码时间戳                   |
| pkt_dts_time                       | 包解码时间   | 包解码时间戳转换为秒             |
| best_effort_timestamp              | 最佳时间戳   | 最佳估计的时间戳                 |
| best_effort_timestamp_time         | 最佳时间     | 最佳时间戳转换为秒               |
| duration（pkt_duration）           | 持续时间戳   | 帧持续时间                       |
| duration_time（pkt_duration_time） | 持续时间     | 帧持续时间转换为秒               |
| pkt_pos                            | 包位置       | 包在文件中的字节位置             |
| pkt_size                           | 包大小       | 包的字节大小                     |

#### 2.2 视频专用参数

| 参数名              | 中文名称     | 描述                |
| ------------------- | ------------ | ------------------- |
| width               | 宽度         | 视频帧宽度（像素）  |
| height              | 高度         | 视频帧高度（像素）  |
| crop_top            | 裁剪顶部     | 顶部裁剪像素数      |
| crop_bottom         | 裁剪底部     | 底部裁剪像素数      |
| crop_left           | 裁剪左侧     | 左侧裁剪像素数      |
| crop_right          | 裁剪右侧     | 右侧裁剪像素数      |
| pix_fmt             | 像素格式     | 像素格式名称        |
| sample_aspect_ratio | 样本宽高比   | 像素宽高比          |
| pict_type           | 图像类型     | 帧类型（I、P、B等） |
| coded_picture_number| 编码图像编号 | 帧在编码流中的绝对顺序位置|
| display_picture_number | 显示图像编号 | 帧在播放时的显示顺序位置|
| interlaced_frame    | 隔行扫描帧   | 是否为隔行扫描帧    |
| top_field_first     | 顶场优先     | 顶场是否优先显示    |
| lossless            | 无损         | 是否为无损编码      |
| repeat_pict         | 重复图像     | 图像重复次数        |
| color_range         | 色彩范围     | 色彩值范围          |
| color_space         | 色彩空间     | 色彩空间标准        |
| color_primaries     | 色彩基色     | 色彩基色标准        |
| color_trc           | 色彩传递特性 | 色彩传递函数        |
| chroma_location     | 色度位置     | 色度采样位置        |

#### 2.3 音频专用参数

| 参数名         | 中文名称 | 描述             |
| -------------- | -------- | ---------------- |
| sample_fmt     | 采样格式 | 音频采样格式     |
| nb_samples     | 采样数量 | 帧中的采样点数量 |
| channels       | 声道数   | 音频声道数量     |
| channel_layout | 声道布局 | 声道布局描述     |

#### 2.4 可选附加部分

| 部分名         | 中文名称     | 描述                                |
| -------------- | ------------ | ----------------------------------- |
| tags           | 标签         | 帧元数据标签                        |
| logs           | 日志         | 相关日志信息                        |
| side_data_list | 附加数据列表 | 帧附加数据（如HDR元数据、时间码等） |