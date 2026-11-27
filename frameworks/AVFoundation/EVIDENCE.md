# AVFoundation playback: evidence

## What Reef's AVFoundation does (the original binary of the package)
- Every media path ends in an error made on the spot: AVURLAsset loading and AVPlayerItem fail with "Media playback is not
  available." (AVFoundationErrorDomain -11833), AVAssetReader / AVAssetWriter / AVAssetExportSession / AVMutableCompositionTrack
  with "Media reading / writing / export / editing is not available." (-11800 for editing).
- `-[AVSampleBufferDisplayLayer enqueueSampleBuffer:]` and `-flush` are empty; AVPlayerLayer only keeps its player.
- The only use of the host is the camera (`reef_appkit_host_interface`, reef_ui_camera_*).
- Reef's VideoToolbox does decode (VTDecompressionSession over the host's VA-API decoder, avcC/hvcC/vpcC) and AudioToolbox
  plays (AudioQueue over the host's audio output), but nothing in AVFoundation uses them.

## What this wrapper adds
- `ReefMediaFFmpeg.c`: the host's FFmpeg (libavformat 60 = FFmpeg 6.1, 61 = 7.1), looked up with reef_host_symbol. The few
  structure fields read were measured with offsetof against each release's headers (Ubuntu 24.04 and Debian 13 packages).
- Sound goes to the audio output of Reef's AppKit host (interface version 24, functions 216..220: PulseAudio, or a paced
  null sink when there is no server).
- `AVFoundationAsset.m` (assets, tracks, compositions), `AVFoundationPlayback.m` (engine, AVPlayerItem, AVPlayer,
  AVPlayerLayer), `AVFoundationTime.m` (NSValue/NSCoder CMTime extensions, export compatibility).

## What QuickTime Player (macOS 26.7.2) needs, read from its binary
- `-[MGPlayerDocument _updateMainViewControllerAVFoundation]`: the movie view replaces the loading view when
  `[asset statusOfValueForKey:@"tracks"] == AVKeyValueStatusLoaded`, `player.currentItem.status == ReadyToPlay`, and asks
  the private `-[AVPlayerItem hasVideo]`; it adds a periodic time observer with an interval of INT64_MAX/1 (fires on jumps).
- `-[MGAssetLoader _validateLoadedValues]`: every key it loaded must be Loaded (2) or Cancelled (4) without an error.
- `-[MGMediaCompositionPlayerItemController updatePlayerItemIfNeeded]`: a plain clip plays as
  `[[AVPlayerItem alloc] initWithAsset:]` with forward/reverse end times; slow motion goes through PFVideoAdjustments.
- `-[MGPlayerDocument setPlayer:]` makes `[[AVPlayerController alloc] initWithPlayer:]` (AVKit, private) and observes
  `playing`, `player.currentItem.tracks`, `player.currentItem.hasVideo`; `-[MGVideoPlaybackViewController loadView]` gives
  the AVPlayerView subclass `setPlayerController:` and observes `playerController.contentDimensions`,
  `playerController.hasEnabledVideo`, `playerLayerFrame`.
- The signatures of AVPlayerController and AVPlayerView (times and rates are doubles, `floatingPlaybackControlsMargin` is
  an NSEdgeInsets) were read with the Objective-C runtime from AVKit on the Mac VM (class_copyMethodList).

## Behaviour checked
- test /t/avt2 (asset keys, tracks, composition, item status, 30 pictures a second into an AVPlayerLayer, seek, end
  notification) on Reef in the test containers (original package and open-reef), 360p to 1080p H.264 with AAC.
- The FFmpeg 6.1 binding natively against the laptop's libavformat.so.60 (probe, 180 pictures, 6.01 s of sound).
