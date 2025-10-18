# 视频流字段说明

以（Stream 0 - H.264视频）为例：

| 键                    | 描述                                |
| :-------------------- | :---------------------------------- |
| `index`               | 流索引号（0表示第一个流）           |
| `codec_name`          | 编解码器简称（H.264）               |
| `codec_long_name`     | 编解码器完整名称                    |
| `profile`             | 编码配置文件（High档次）            |
| `codec_type`          | 流类型（video）                     |
| `codec_tag_string`    | 编解码器标签字符串                  |
| `codec_tag`           | 编解码器十六进制标签                |
| `width`               | 视频宽度（854像素）                 |
| `height`              | 视频高度（480像素）                 |
| `coded_width`         | 编码时宽度                          |
| `coded_height`        | 编码时高度                          |
| `closed_captions`     | 是否包含隐藏式字幕                  |
| `has_b_frames`        | B帧数量（2个）                      |
| `pix_fmt`             | 像素格式（yuv420p）                 |
| `level`               | H.264级别（30）                     |
| `chroma_location`     | 色度采样位置                        |
| `refs`                | 参考帧数量（1个）                   |
| `is_avc`              | 是否为AVC格式                       |
| `nal_length_size`     | NAL单元长度大小                     |
| `r_frame_rate`        | 真实帧率（24/1 = 24fps）            |
| `avg_frame_rate`      | 平均帧率（24/1 = 24fps）            |
| `time_base`           | 时间基准（1/24秒）                  |
| `start_pts`           | 起始显示时间戳                      |
| `start_time`          | 起始时间（0秒）                     |
| `duration_ts`         | 持续时间戳单位                      |
| `duration`            | 流持续时间（52.208333秒）           |
| `bit_rate`            | 视频比特率（537875 bps ≈ 538 kbps） |
| `bits_per_raw_sample` | 每个原始样本的位数                  |
| `nb_frames`           | 总帧数（1253帧）                    |
| `disposition`         | 流的行为标志                        |
| `tags`                | 流元数据标签                        |

# 音频流字段说明

以（Stream 1 -AAC ）音频为例：

| 键                 | 描述                                |
| :----------------- | :---------------------------------- |
| `index`            | 流索引号（1表示第二个流）           |
| `codec_name`       | 编解码器简称（AAC）                 |
| `codec_long_name`  | 编解码器完整名称                    |
| `profile`          | 编码配置文件（LC低复杂度）          |
| `codec_type`       | 流类型（audio）                     |
| `codec_tag_string` | 编解码器标签字符串                  |
| `codec_tag`        | 编解码器十六进制标签                |
| `sample_fmt`       | 采样格式（fltp浮点型）              |
| `sample_rate`      | 采样率（48000 Hz）                  |
| `channels`         | 声道数（2声道）                     |
| `channel_layout`   | 声道布局（stereo立体声）            |
| `bits_per_sample`  | 每个样本的位数（0表示可变）         |
| `r_frame_rate`     | 真实帧率（音频通常为0/0）           |
| `avg_frame_rate`   | 平均帧率（音频通常为0/0）           |
| `time_base`        | 时间基准（1/48000秒）               |
| `start_pts`        | 起始显示时间戳                      |
| `start_time`       | 起始时间（0秒）                     |
| `duration_ts`      | 持续时间戳单位                      |
| `duration`         | 流持续时间（51.946667秒）           |
| `bit_rate`         | 音频比特率（126694 bps ≈ 127 kbps） |
| `nb_frames`        | 总帧数（2435帧）                    |
| `disposition`      | 流的行为标志                        |
| `tags`             | 流元数据标签                        |

# 关键信息总结

## **视频流特征：**

- 分辨率：854×480
- 帧率：24 fps
- 编码：H.264 High Profile
- 时长：52.21秒
- 码率：538 kbps

## **音频流特征：**

- 格式：AAC LC
- 采样率：48 kHz
- 声道：立体声
- 时长：51.95秒
- 码率：127 kbps

## metadata信息：

**注意：** 两个流的tags部分含义相同：

- `creation_time`：创建时间
- `language`：语言（und表示未定义）
- `handler_name`：处理器名称
- `vendor_id`：供应商ID



# 原始输出：

`ffprobe -v quiet -show_streams -print_format json input.mp4`



```json
{
    "streams": [
        {
            "index": 0,
            "codec_name": "h264",
            "codec_long_name": "H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10",
            "profile": "High",
            "codec_type": "video",
            "codec_tag_string": "avc1",
            "codec_tag": "0x31637661",
            "width": 854,
            "height": 480,
            "coded_width": 854,
            "coded_height": 480,
            "closed_captions": 0,
            "has_b_frames": 2,
            "pix_fmt": "yuv420p",
            "level": 30,
            "chroma_location": "left",
            "refs": 1,
            "is_avc": "true",
            "nal_length_size": "4",
            "r_frame_rate": "24/1",
            "avg_frame_rate": "24/1",
            "time_base": "1/24",
            "start_pts": 0,
            "start_time": "0.000000",
            "duration_ts": 1253,
            "duration": "52.208333",
            "bit_rate": "537875",
            "bits_per_raw_sample": "8",
            "nb_frames": "1253",
            "disposition": {
                "default": 1,
                "dub": 0,
                "original": 0,
                "comment": 0,
                "lyrics": 0,
                "karaoke": 0,
                "forced": 0,
                "hearing_impaired": 0,
                "visual_impaired": 0,
                "clean_effects": 0,
                "attached_pic": 0,
                "timed_thumbnails": 0
            },
            "tags": {
                "creation_time": "1970-01-01T00:00:00.000000Z",
                "language": "und",
                "handler_name": "VideoHandler",
                "vendor_id": "[0][0][0][0]"
            }
        },
        {
            "index": 1,
            "codec_name": "aac",
            "codec_long_name": "AAC (Advanced Audio Coding)",
            "profile": "LC",
            "codec_type": "audio",
            "codec_tag_string": "mp4a",
            "codec_tag": "0x6134706d",
            "sample_fmt": "fltp",
            "sample_rate": "48000",
            "channels": 2,
            "channel_layout": "stereo",
            "bits_per_sample": 0,
            "r_frame_rate": "0/0",
            "avg_frame_rate": "0/0",
            "time_base": "1/48000",
            "start_pts": 0,
            "start_time": "0.000000",
            "duration_ts": 2493440,
            "duration": "51.946667",
            "bit_rate": "126694",
            "nb_frames": "2435",
            "disposition": {
                "default": 1,
                "dub": 0,
                "original": 0,
                "comment": 0,
                "lyrics": 0,
                "karaoke": 0,
                "forced": 0,
                "hearing_impaired": 0,
                "visual_impaired": 0,
                "clean_effects": 0,
                "attached_pic": 0,
                "timed_thumbnails": 0
            },
            "tags": {
                "creation_time": "1970-01-01T00:00:00.000000Z",
                "language": "und",
                "handler_name": "SoundHandler",
                "vendor_id": "[0][0][0][0]"
            }
        }
    ]
}
```

