#include <iterator>
#include <utility>
#include <string>
#include <sstream>
#include <functional>
#include <iostream>

#include <mfapi.h>
#include <mfidl.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")

#include <unknwn.h>
#include <winrt/base.h>

#include <wil/resource.h>
#include <wil/com.h>

int main()
{
    try
    {
        winrt::init_apartment();
        THROW_IF_FAILED(MFStartup(MF_VERSION));

        wil::com_ptr<IMFSourceResolver> SourceResolver;
        THROW_IF_FAILED(MFCreateSourceResolver(SourceResolver.put()));
        MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;
        wil::com_ptr<IUnknown> MediaSourceUnknown;
        THROW_IF_FAILED(SourceResolver->CreateObjectFromURL(L"..\\test.mp3", MF_RESOLUTION_MEDIASOURCE, nullptr, &ObjectType, MediaSourceUnknown.put()));
        auto const MediaSource = MediaSourceUnknown.query<IMFMediaSource>();

        wil::com_ptr<IMFTopology> Topology;
        THROW_IF_FAILED(MFCreateTopology(Topology.put()));

        wil::com_ptr<IMFPresentationDescriptor> PresentationDescriptor;
        THROW_IF_FAILED(MediaSource->CreatePresentationDescriptor(PresentationDescriptor.put()));
        BOOL Select;
        wil::com_ptr<IMFStreamDescriptor> StreamDescriptor;
        THROW_IF_FAILED(PresentationDescriptor->GetStreamDescriptorByIndex(0, &Select, StreamDescriptor.put()));
        WI_ASSERT(Select && StreamDescriptor);
        wil::com_ptr<IMFTopologyNode> SourceNode;
        THROW_IF_FAILED(MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, SourceNode.put()));
        THROW_IF_FAILED(SourceNode->SetUnknown(MF_TOPONODE_SOURCE, MediaSource.get()));
        THROW_IF_FAILED(SourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, PresentationDescriptor.get()));
        THROW_IF_FAILED(SourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, StreamDescriptor.get()));
        THROW_IF_FAILED(Topology->AddNode(SourceNode.get()));

        wil::com_ptr<IMFActivate> Activate;
        THROW_IF_FAILED(MFCreateAudioRendererActivate(Activate.put()));
        wil::com_ptr<IMFTopologyNode> OutputNode;
        THROW_IF_FAILED(MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, OutputNode.put())); // https://learn.microsoft.com/en-us/windows/win32/medfound/creating-output-nodes#creating-an-output-node-from-an-activation-object
        THROW_IF_FAILED(OutputNode->SetObject(Activate.get()));
        THROW_IF_FAILED(Topology->AddNode(OutputNode.get()));

        THROW_IF_FAILED(SourceNode->ConnectOutput(0, OutputNode.get(), 0));

        wil::com_ptr<IMFMediaSession> MediaSession;
        THROW_IF_FAILED(MFCreateMediaSession(nullptr, MediaSession.put()));
        THROW_IF_FAILED(MediaSession->SetTopology(0, Topology.get()));

        wil::unique_prop_variant StartTime;
        StartTime.vt = VT_I8;
        StartTime.hVal.QuadPart = 0;
        THROW_IF_FAILED(MediaSession->Start(nullptr, &StartTime));

        auto const MediaEventGenerator = MediaSession.query<IMFMediaEventGenerator>();
        for(; ; )
        {
            wil::com_ptr<IMFMediaEvent> MediaEvent;
            THROW_IF_FAILED(MediaEventGenerator->GetEvent(0, MediaEvent.put()));
            MediaEventType Type;
            THROW_IF_FAILED(MediaEvent->GetType(&Type));
            
            #pragma region MediaEventType Names
            static std::pair<MediaEventType, char const*> constexpr const g_Items[]
            {
                #define A(Identifier) std::make_pair(Identifier, #Identifier),
                A(MEUnknown)
                A(MEError)
                A(MEExtendedType)
                A(MENonFatalError)
                A(MEGenericV1Anchor)
                A(MESessionUnknown)
                A(MESessionTopologySet)
                A(MESessionTopologiesCleared)
                A(MESessionStarted)
                A(MESessionPaused)
                A(MESessionStopped)
                A(MESessionClosed)
                A(MESessionEnded)
                A(MESessionRateChanged)
                A(MESessionScrubSampleComplete)
                A(MESessionCapabilitiesChanged)
                A(MESessionTopologyStatus)
                A(MESessionNotifyPresentationTime)
                A(MENewPresentation)
                A(MELicenseAcquisitionStart)
                A(MELicenseAcquisitionCompleted)
                A(MEIndividualizationStart)
                A(MEIndividualizationCompleted)
                A(MEEnablerProgress)
                A(MEEnablerCompleted)
                A(MEPolicyError)
                A(MEPolicyReport)
                A(MEBufferingStarted)
                A(MEBufferingStopped)
                A(MEConnectStart)
                A(MEConnectEnd)
                A(MEReconnectStart)
                A(MEReconnectEnd)
                A(MERendererEvent)
                A(MESessionStreamSinkFormatChanged)
                A(MESessionV1Anchor)
                A(MESourceUnknown)
                A(MESourceStarted)
                A(MEStreamStarted)
                A(MESourceSeeked)
                A(MEStreamSeeked)
                A(MENewStream)
                A(MEUpdatedStream)
                A(MESourceStopped)
                A(MEStreamStopped)
                A(MESourcePaused)
                A(MEStreamPaused)
                A(MEEndOfPresentation)
                A(MEEndOfStream)
                A(MEMediaSample)
                A(MEStreamTick)
                A(MEStreamThinMode)
                A(MEStreamFormatChanged)
                A(MESourceRateChanged)
                A(MEEndOfPresentationSegment)
                A(MESourceCharacteristicsChanged)
                A(MESourceRateChangeRequested)
                A(MESourceMetadataChanged)
                A(MESequencerSourceTopologyUpdated)
                A(MESourceV1Anchor)
                A(MESinkUnknown)
                A(MEStreamSinkStarted)
                A(MEStreamSinkStopped)
                A(MEStreamSinkPaused)
                A(MEStreamSinkRateChanged)
                A(MEStreamSinkRequestSample)
                A(MEStreamSinkMarker)
                A(MEStreamSinkPrerolled)
                A(MEStreamSinkScrubSampleComplete)
                A(MEStreamSinkFormatChanged)
                A(MEStreamSinkDeviceChanged)
                A(MEQualityNotify)
                A(MESinkInvalidated)
                A(MEAudioSessionNameChanged)
                A(MEAudioSessionVolumeChanged)
                A(MEAudioSessionDeviceRemoved)
                A(MEAudioSessionServerShutdown)
                A(MEAudioSessionGroupingParamChanged)
                A(MEAudioSessionIconChanged)
                A(MEAudioSessionFormatChanged)
                A(MEAudioSessionDisconnected)
                A(MEAudioSessionExclusiveModeOverride)
                A(MESinkV1Anchor)
                A(MECaptureAudioSessionVolumeChanged)
                A(MECaptureAudioSessionDeviceRemoved)
                A(MECaptureAudioSessionFormatChanged)
                A(MECaptureAudioSessionDisconnected)
                A(MECaptureAudioSessionExclusiveModeOverride)
                A(MECaptureAudioSessionServerShutdown)
                A(MESinkV2Anchor)
                A(METrustUnknown)
                A(MEPolicyChanged)
                A(MEContentProtectionMessage)
                A(MEPolicySet)
                A(METrustV1Anchor)
                A(MEWMDRMLicenseBackupCompleted)
                A(MEWMDRMLicenseBackupProgress)
                A(MEWMDRMLicenseRestoreCompleted)
                A(MEWMDRMLicenseRestoreProgress)
                A(MEWMDRMLicenseAcquisitionCompleted)
                A(MEWMDRMIndividualizationCompleted)
                A(MEWMDRMIndividualizationProgress)
                A(MEWMDRMProximityCompleted)
                A(MEWMDRMLicenseStoreCleaned)
                A(MEWMDRMRevocationDownloadCompleted)
                A(MEWMDRMV1Anchor)
                A(METransformUnknown)
                A(METransformNeedInput)
                A(METransformHaveOutput)
                A(METransformDrainComplete)
                A(METransformMarker)
                A(METransformInputStreamStateChanged)
                A(MEByteStreamCharacteristicsChanged)
                A(MEVideoCaptureDeviceRemoved)
                A(MEVideoCaptureDevicePreempted)
                A(MEStreamSinkFormatInvalidated)
                A(MEEncodingParameters)
                A(MEContentProtectionMetadata)
                A(MEDeviceThermalStateChanged)
                //A(MEReservedMax)
                #undef A
            };
            #pragma endregion
            auto const Iterator = std::find_if(std::cbegin(g_Items), std::cend(g_Items), [=] (auto&& Item) { return Item.first == Type; });
            WI_ASSERT(Iterator != std::cend(g_Items));
            switch(Type)
            {
            case MESessionTopologySet:
                {
                    std::wcout << L"Event: " << Iterator->second << std::endl;
                    wil::unique_prop_variant VariantTopology;
                    THROW_IF_FAILED(MediaEvent->GetValue(&VariantTopology));
                    WI_ASSERT(VariantTopology.vt == VT_UNKNOWN);
                    auto const Topology = wil::com_query<IMFTopology>(VariantTopology.punkVal);
                    WORD NodeCount;
                    THROW_IF_FAILED(Topology->GetNodeCount(&NodeCount));
                    std::wcout << L"  Node Count: " << NodeCount << std::endl;
                    for(WORD NodeIndex = 0; NodeIndex < NodeCount; NodeIndex++)
                    {
                        wil::com_ptr<IMFTopologyNode> TopologyNode;
                        THROW_IF_FAILED(Topology->GetNode(NodeIndex, TopologyNode.put()));
                        MF_TOPOLOGY_TYPE Type;
                        THROW_IF_FAILED(TopologyNode->GetNodeType(&Type));
                        #pragma region MF_TOPOLOGY_TYPE Names
                        static std::pair<MF_TOPOLOGY_TYPE, char const*> constexpr const g_Items[]
                        {
                            #define A(Identifier) std::make_pair(Identifier, #Identifier),
                            A(MF_TOPOLOGY_OUTPUT_NODE)
                            A(MF_TOPOLOGY_SOURCESTREAM_NODE)
                            A(MF_TOPOLOGY_TRANSFORM_NODE)
                            A(MF_TOPOLOGY_TEE_NODE)
                            //A(MF_TOPOLOGY_MAX)
                            #undef A
                        };
                        #pragma endregion
                        auto const Iterator = std::find_if(std::cbegin(g_Items), std::cend(g_Items), [=] (auto&& Item) { return Item.first == Type; });
                        WI_ASSERT(Iterator != std::cend(g_Items));
                        std::wcout << L"  Node " << NodeIndex << ": " << Iterator->second << std::endl;
                        UINT32 Count;
                        THROW_IF_FAILED(TopologyNode->GetCount(&Count));
                        for(UINT32 Index = 0; Index < Count; Index++)
                        {
                            GUID Key;
                            wil::unique_prop_variant Value;
                            THROW_IF_FAILED(TopologyNode->GetItemByIndex(Index, &Key, &Value));
                            #pragma region Topology Node Attributes https://learn.microsoft.com/en-us/windows/win32/medfound/topology-node-attributes
                            static std::pair<GUID const*, char const*> constexpr const g_Items[]
                            {
                                #define A(Identifier) std::make_pair(&Identifier, #Identifier),
                                // General Topology Node Attributes
                                A(MF_TOPONODE_CONNECT_METHOD)
                                A(MF_TOPONODE_DECODER)
                                A(MF_TOPONODE_DECRYPTOR)
                                A(MF_TOPONODE_DISCARDABLE)
                                A(MF_TOPONODE_ERROR_MAJORTYPE)
                                A(MF_TOPONODE_ERROR_SUBTYPE)
                                A(MF_TOPONODE_ERRORCODE)
                                A(MF_TOPONODE_LOCKED)
                                A(MF_TOPONODE_MARKIN_HERE)
                                A(MF_TOPONODE_MARKOUT_HERE)
                                // Source Node Attributes
                                A(MF_TOPONODE_MEDIASTART)
                                A(MF_TOPONODE_MEDIASTOP)
                                A(MF_TOPONODE_PRESENTATION_DESCRIPTOR)
                                A(MF_TOPONODE_SEQUENCE_ELEMENTID)
                                A(MF_TOPONODE_SOURCE)
                                A(MF_TOPONODE_STREAM_DESCRIPTOR)
                                A(MF_TOPONODE_WORKQUEUE_ID)
                                A(MF_TOPONODE_WORKQUEUE_MMCSS_CLASS)
                                A(MF_TOPONODE_WORKQUEUE_MMCSS_TASKID)
                                // Transform Node Attributes
                                A(MF_TOPONODE_D3DAWARE)
                                A(MF_TOPONODE_DRAIN)
                                A(MF_TOPONODE_FLUSH)
                                A(MF_TOPONODE_TRANSFORM_OBJECTID)
                                // Output Node Attributes
                                A(MF_TOPONODE_DISABLE_PREROLL)
                                A(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE)
                                A(MF_TOPONODE_RATELESS)
                                A(MF_TOPONODE_STREAMID)
                                // Tee Node Attributes
                                A(MF_TOPONODE_PRIMARYOUTPUT)
                                #undef A
                            };
                            #pragma endregion
                            auto const Iterator = std::find_if(std::cbegin(g_Items), std::cend(g_Items), [=] (auto&& Item) { return *Item.first == Key; });
                            WI_ASSERT(Iterator != std::cend(g_Items));
                            std::wostringstream Stream;
                            Stream << Iterator->second;
                            if(Key == MF_TOPONODE_TRANSFORM_OBJECTID)
                            {
                                WI_ASSERT(Value.vt == VT_CLSID);
                                OLECHAR IdentifierValue[40];
                                WI_VERIFY(StringFromGUID2(*Value.puuid, IdentifierValue, static_cast<int>(std::size(IdentifierValue))));
                                Stream << L" " << IdentifierValue;
                            }
                            std::wcout << L"    " << Stream.str() << std::endl;
                        }
                        // NOTE: Inputs, Outputs, Object
                    }
                }
                break;
            case MESessionTopologyStatus:
                {
                    UINT32 Status; // MF_TOPOSTATUS
                    THROW_IF_FAILED(MediaEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &Status));
                    #pragma region MF_TOPOSTATUS Names
                    static std::pair<MF_TOPOSTATUS, char const*> constexpr const g_Items[]
                    {
                        #define A(Identifier) std::make_pair(Identifier, #Identifier),
                        A(MF_TOPOSTATUS_INVALID)
                        A(MF_TOPOSTATUS_READY)
                        A(MF_TOPOSTATUS_STARTED_SOURCE)
                        A(MF_TOPOSTATUS_DYNAMIC_CHANGED)
                        A(MF_TOPOSTATUS_SINK_SWITCHED)
                        A(MF_TOPOSTATUS_ENDED)
                        #undef A
                    };
                    #pragma endregion
                    auto const StatusIterator = std::find_if(std::cbegin(g_Items), std::cend(g_Items), [=] (auto&& Item) { return Item.first == static_cast<MF_TOPOSTATUS>(Status); });
                    WI_ASSERT(StatusIterator != std::cend(g_Items));
                    std::wcout << L"Event: " << Iterator->second << " " << StatusIterator->second << std::endl;
                }
                break;
            case MESessionCapabilitiesChanged:
                {
                    UINT32 Capabilities, ChangeCapabilities; // MF_TOPOSTATUS
                    THROW_IF_FAILED(MediaEvent->GetUINT32(MF_EVENT_SESSIONCAPS, &Capabilities));
                    THROW_IF_FAILED(MediaEvent->GetUINT32(MF_EVENT_SESSIONCAPS_DELTA, &ChangeCapabilities));
                    #pragma region MF_TOPOSTATUS Names
                    static std::pair<UINT32, char const*> constexpr const g_Items[]
                    {
                        #define A(Identifier) std::make_pair(Identifier, #Identifier),
                        A(MFSESSIONCAP_START)
                        A(MFSESSIONCAP_SEEK)
                        A(MFSESSIONCAP_PAUSE)
                        A(MFSESSIONCAP_RATE_FORWARD)
                        A(MFSESSIONCAP_RATE_REVERSE)
                        A(MFSESSIONCAP_DOES_NOT_USE_NETWORK)
                        #undef A
                    };
                    #pragma endregion
                    auto const ToString = [] (UINT32 Value)
                    {
                        std::wostringstream Stream;
                        for(auto&& Item: g_Items)
                            if(Value & Item.first)
                            {
                                Stream << Item.second << L" ";
                                Value &= ~Item.first;
                            }
                        WI_ASSERT(!Value);
                        auto Output = Stream.str();
                        if(!Output.empty())
                            Output.erase(Output.length() - 1);
                        return Output;
                    };
                    std::wcout << L"Event: " << Iterator->second << std::endl;
                    std::wcout << L"  Capabilities: " << ToString(Capabilities) << std::endl;
                    std::wcout << L"  Change: " << ToString(ChangeCapabilities) << std::endl;
                }
                break;
            default:
                std::wcout << L"Event: " << Iterator->second << std::endl;
            }

            if(Type == MESessionStarted)
                std::wcout << L"..." << std::endl;
            if(Type == MESessionEnded)
                break;
        }

        /*

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

        */ 

        THROW_IF_FAILED(MediaSession->Shutdown());

        THROW_IF_FAILED(MFShutdown());
    }
    CATCH_LOG();
    return 0;
}
