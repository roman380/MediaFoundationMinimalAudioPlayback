#include <iostream>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wmcodecdsp.h>
#include <combaseapi.h>

#include <unknwn.h>
#include <winrt/base.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")

int main()
{
    try
    {
        winrt::init_apartment();
        winrt::check_hresult(MFStartup(MF_VERSION));

        winrt::com_ptr<IMFSourceResolver> SourceResolver;
        winrt::check_hresult(MFCreateSourceResolver(SourceResolver.put()));
        MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;
        winrt::com_ptr<IUnknown> MediaSourceUnknown;
        winrt::check_hresult(SourceResolver->CreateObjectFromURL(L"..\\test.mp3", MF_RESOLUTION_MEDIASOURCE, nullptr, &ObjectType, MediaSourceUnknown.put()));
        auto const MediaSource = MediaSourceUnknown.as<IMFMediaSource>();

        winrt::com_ptr<IMFTopology> Topology;
        winrt::check_hresult(MFCreateTopology(Topology.put()));

        winrt::com_ptr<IMFPresentationDescriptor> PresentationDescriptor;
        winrt::check_hresult(MediaSource->CreatePresentationDescriptor(PresentationDescriptor.put()));
        BOOL Select;
        winrt::com_ptr<IMFStreamDescriptor> StreamDescriptor;
        winrt::check_hresult(PresentationDescriptor->GetStreamDescriptorByIndex(0, &Select, StreamDescriptor.put()));
        WINRT_ASSERT(Select && StreamDescriptor);
        winrt::com_ptr<IMFTopologyNode> SourceNode;
        winrt::check_hresult(MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, SourceNode.put()));
        winrt::check_hresult(SourceNode->SetUnknown(MF_TOPONODE_SOURCE, MediaSource.get()));
        winrt::check_hresult(SourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, PresentationDescriptor.get()));
        winrt::check_hresult(SourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, StreamDescriptor.get()));
        winrt::check_hresult(Topology->AddNode(SourceNode.get()));

        winrt::com_ptr<IMFActivate> Activate;
        winrt::check_hresult(MFCreateAudioRendererActivate(Activate.put()));
        winrt::com_ptr<IMFTopologyNode> OutputNode;
        winrt::check_hresult(MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, OutputNode.put())); // https://learn.microsoft.com/en-us/windows/win32/medfound/creating-output-nodes#creating-an-output-node-from-an-activation-object
        winrt::check_hresult(OutputNode->SetObject(Activate.get()));
        winrt::check_hresult(Topology->AddNode(OutputNode.get()));

        winrt::check_hresult(SourceNode->ConnectOutput(0, OutputNode.get(), 0));

        winrt::com_ptr<IMFMediaSession> MediaSession;
        winrt::check_hresult(MFCreateMediaSession(nullptr, MediaSession.put()));
        winrt::check_hresult(MediaSession->SetTopology(0, Topology.get()));

        PROPVARIANT StartTime;
        PropVariantInit(&StartTime);
        StartTime.vt = VT_I8;
        StartTime.hVal.QuadPart = 0;
        winrt::check_hresult(MediaSession->Start(nullptr, &StartTime));

        auto const MediaEventGenerator = MediaSession.as<IMFMediaEventGenerator>();
        for(; ; )
        {
            winrt::com_ptr<IMFMediaEvent> MediaEvent;
            winrt::check_hresult(MediaEventGenerator->GetEvent(0, MediaEvent.put()));
            MediaEventType Type;
            winrt::check_hresult(MediaEvent->GetType(&Type));
            std::wcout << L"Event: " << static_cast<int>(Type) << std::endl;
        }

        winrt::check_hresult(MFShutdown());
    }
    catch(winrt::hresult_error const& Exception)
    {
        std::wcerr << L"Exception: " << Exception.message().c_str() << std::endl;
    }
    return 0;
}
