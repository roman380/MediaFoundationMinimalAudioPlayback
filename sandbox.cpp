#include <iterator>
#include <utility>
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
        WINRT_ASSERT(Select && StreamDescriptor);
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

        PROPVARIANT StartTime;
        PropVariantInit(&StartTime);
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
            if(Iterator != std::cend(g_Items))
                std::wcout << L"Event: " << Iterator->second << std::endl;
            else
                std::wcout << L"Event: " << static_cast<int>(Type) << std::endl;
        }

        /*
            Event: MESessionTopologySet
            Event: MESessionNotifyPresentationTime
            Event: MESessionCapabilitiesChanged
            Event: MESessionTopologyStatus
            Event: MESessionTopologyStatus
            Event: MEExtendedType
            Event: MESessionCapabilitiesChanged
            Event: MESessionStarted
            Event: MEEndOfPresentation
            ...
            Event: MESessionTopologyStatus
            Event: MESessionCapabilitiesChanged
            Event: MESessionEnded
        */ 

        THROW_IF_FAILED(MFShutdown());
    }
    CATCH_LOG();
    return 0;
}
