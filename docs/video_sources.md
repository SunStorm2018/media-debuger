# 免费视频下载测试网站汇总



|网站名称|	网址 |	特点|	许可证|	推荐测试链接|
|--|--|--|--|--|
Internet Archive|	https://archive.org/details/movies|	海量开源视频资源，历史影片、教育内容	|多种开源许可证|	wget https://archive.org/download/ExampleVideo/example_video.mp4
Pexels Videos|	https://www.pexels.com/videos/	|高质量免费stock videos，支持商业用途	|Pexels License	|网站直接下载
Coverr|	https://coverr.co/	|精美免费背景视频，高质量	|MIT License|	直接MP4下载
Videvo|	https://www.videvo.net/	|大量免费视频素材	|多种许可证	|注意查看具体条款
Big Buck Bunny|	https://download.blender.org/peach/bigbuckbunny_movies/	|经典测试视频，多分辨率	|Creative Commons|	wget http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4
Elephants Dream|	https://orange.blender.org/download/ |	Blender开源电影|	Creative Commons	|wget http://ftp.nluug.nl/pub/graphics/blender/demo/movies/ED_HD.avi
Test Videos Project|	https://test-videos.co.uk/	|专门为测试设计	|免费测试使用	|wget https://test-videos.co.uk/bigbuckbunny/mp4-h264
Sample Videos|	https://www.sample-videos.com/	|各种格式和大小样本|	测试用途	|wget https://www.sample-videos.com/video123/mp4/720/big_buck_bunny_720p_1mb.mp4
File Examples|	https://file-examples.com/	|文件格式示例|	测试用途	|wget https://file-examples.com/wp-content/uploads/2017/04/file_example_MP4_480_1_5MG.mp4
Xiph.org Test Media	|https://media.xiph.org/	|各种编解码器测试文件	|开源	|wget https://media.xiph.org/video/derf/

### 小文件测试（快速下载）

```bash
# 1MB 测试视频
wget https://www.sample-videos.com/video123/mp4/720/big_buck_bunny_720p_1mb.mp4

# 5MB 测试视频
wget https://www.sample-videos.com/video123/mp4/720/big_buck_bunny_720p_5mb.mp4

# 10MB 测试视频
wget https://www.sample-videos.com/video123/mp4/720/big_buck_bunny_720p_10mb.mp4
```

### 多分辨率测试

```bash
# 480p
wget http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4

# 720p
wget http://distribution.bbb3d.renderfarming.net/video/mp4/bbb_sunflower_720p_30fps_normal.mp4

# 1080p
wget http://distribution.bbb3d.renderfarming.net/video/mp4/bbb_sunflower_1080p_60fps_normal.mp4
```

