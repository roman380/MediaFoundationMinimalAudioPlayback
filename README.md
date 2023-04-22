# MediaFoundationMinimalAudioPlayback

See [original StackOverflow question](https://stackoverflow.com/q/76056442/868014).

The application plays an MP3 file (you should supply yours) in less than 100 lines of C++ code with Microsoft Media Foundation.

See [`wil`](tree/wil) branch with a bit more of code, and the code that produces printout below.

Note that we are supplying MP3 file media source (node 0) and default audio output renderer (AKA sink, output; node 1). Microsoft Media Foundation resolves topology and supplies two transforms in order to connect source to sink: 

1. [`MF_TOPONODE_DECODER`](https://learn.microsoft.com/en-us/windows/win32/medfound/mf-toponode-decoder-attribute) [`CLSID_MP3DecMediaObject`](https://www.magnumdb.com/search?q=%7BBBEEA841-0A63-4F52-A7AB-A9B3A84ED38A%7D) to decode MP3 audio
2. [`CLSID_AudioResamplerMediaObject`](https://www.magnumdb.com/search?q=%7BF447B69E-1884-4A7E-8055-346F74D6EDB3%7D) to fit audio format to what device is capable to play

## Details

```
Event: MESessionTopologySet
  Node Count: 4
  Node 0: MF_TOPOLOGY_SOURCESTREAM_NODE
    MF_TOPONODE_MEDIASTART
    MF_TOPONODE_SOURCE
    MF_TOPONODE_PRESENTATION_DESCRIPTOR
    MF_TOPONODE_STREAM_DESCRIPTOR
  Node 1: MF_TOPOLOGY_OUTPUT_NODE
    MF_TOPONODE_STREAMID
    MF_TOPONODE_TRANSFORM_OBJECTID {D23E6476-B104-4707-81CB-E1CA19A07016}
  Node 2: MF_TOPOLOGY_TRANSFORM_NODE
    MF_TOPONODE_MARKIN_HERE
    MF_TOPONODE_MARKOUT_HERE
    MF_TOPONODE_DECODER
    MF_TOPONODE_TRANSFORM_OBJECTID {BBEEA841-0A63-4F52-A7AB-A9B3A84ED38A}
  Node 3: MF_TOPOLOGY_TRANSFORM_NODE
    MF_TOPONODE_TRANSFORM_OBJECTID {F447B69E-1884-4A7E-8055-346F74D6EDB3}
Event: MESessionNotifyPresentationTime
Event: MESessionCapabilitiesChanged
  Capabilities: MFSESSIONCAP_START MFSESSIONCAP_SEEK MFSESSIONCAP_RATE_FORWARD MFSESSIONCAP_DOES_NOT_USE_NETWORK
  Change: MFSESSIONCAP_START MFSESSIONCAP_SEEK MFSESSIONCAP_RATE_FORWARD MFSESSIONCAP_DOES_NOT_USE_NETWORK
Event: MESessionTopologyStatus MF_TOPOSTATUS_READY
Event: MESessionTopologyStatus MF_TOPOSTATUS_STARTED_SOURCE
Event: MEExtendedType
Event: MESessionCapabilitiesChanged
  Capabilities: MFSESSIONCAP_START MFSESSIONCAP_SEEK MFSESSIONCAP_PAUSE MFSESSIONCAP_RATE_FORWARD MFSESSIONCAP_DOES_NOT_USE_NETWORK
  Change: MFSESSIONCAP_PAUSE
Event: MESessionStarted
...
Event: MEEndOfPresentation
Event: MESessionTopologyStatus MF_TOPOSTATUS_ENDED
Event: MESessionCapabilitiesChanged
  Capabilities: MFSESSIONCAP_START MFSESSIONCAP_SEEK MFSESSIONCAP_RATE_FORWARD MFSESSIONCAP_DOES_NOT_USE_NETWORK
  Change: MFSESSIONCAP_PAUSE
Event: MESessionEnded
```
