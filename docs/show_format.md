# 基础格式信息

| 键                 | 描述                            |
| :----------------- | :------------------------------ |
| `filename`         | 文件名                          |
| `nb_streams`       | 流数量（通常是视频流+音频流）   |
| `nb_programs`      | 节目数量（用于广播流，通常为0） |
| `format_name`      | 容器格式类型                    |
| `format_long_name` | 格式完整名称                    |
| `start_time`       | 开始时间（秒）                  |
| `duration`         | 视频总时长（秒）                |
| `size`             | 文件大小（字节）                |
| `bit_rate`         | 总比特率（bps）                 |
| `probe_score`      | 格式识别置信度（0-100）         |

## 元数据标签 (tags)

| 键                  | 描述              |
| :------------------ | :---------------- |
| `major_brand`       | MP4主要品牌标识符 |
| `minor_version`     | MP4次要版本号     |
| `compatible_brands` | 兼容的品牌列表    |
| `creation_time`     | 文件创建时间      |
| `title`             | 视频标题          |
| `artist`            | 艺术家/创作者     |
| `encoder`           | 编码器名称和版本  |
| `copyright`         | 版权信息          |
| `description`       | 视频描述          |

## 补充说明

- **品牌标识相关**：`major_brand`、`minor_version`、`compatible_brands` 是MP4容器的标准头部信息，用于播放器识别文件格式兼容性
- **时间信息**：`creation_time` 为UTC时间格式，如果显示1970年通常表示时间戳未正确设置
- **编码信息**：`encoder` 字段显示该文件是使用FFmpeg的Libavformat库编码的
- **内容信息**：`title`、`artist`、`copyright`、`description` 都是用户可编辑的元数据字段

这些字段中，tags部分的所有内容都可以使用 `-metadata` 参数进行修改。



# 原始数据

```bash
ffprobe -v quiet -show_format -print_format json input.mp4
```

```json
{
    "format": {
        "filename": "input.mp4",
        "nb_streams": 2,
        "nb_programs": 0,
        "format_name": "mov,mp4,m4a,3gp,3g2,mj2",
        "format_long_name": "QuickTime / MOV",
        "start_time": "0.000000",
        "duration": "52.209000",
        "size": "4372373",
        "bit_rate": "669979",
        "probe_score": 100,
        "tags": {
            "major_brand": "isom",
            "minor_version": "512",
            "compatible_brands": "isomiso2avc1mp41",
            "creation_time": "1970-01-01T00:00:00.000000Z",
            "title": "Sintel Trailer",
            "artist": "Durian Open Movie Team",
            "encoder": "Lavf52.62.0",
            "copyright": "(c) copyright Blender Foundation | durian.blender.org",
            "description": "Trailer for the Sintel open movie project"
        }
    }
}

```

