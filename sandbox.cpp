#include <iostream>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <Wmcodecdsp.h>
#include <combaseapi.h>

int main()
{
    HRESULT hr=CoInitialize(NULL);

    if(SUCCEEDED(hr)){
   
        // Initialize the Media Foundation platform.
       MFStartup(MF_VERSION);
      
            MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;
            IMFSourceResolver* pSourceResolver;
            IUnknown* pSource = nullptr;
            IMFMediaSource* mSource;
            IMFPresentationDescriptor* ppPresentationDescriptor;
            IMFTransform* mtransform = NULL;
            IMFTopology* topology = NULL;
            IMFStreamDescriptor* sdesip = NULL;
            IMFTopologyNode* source = NULL;
            IMFTopologyNode* transform;
            IMFTopologyNode* output = NULL;
            IMFMediaSink* mediasink = NULL;
            IMFMediaTypeHandler* mediatypehandler;
            IMFMediaType* mediatype;
            IMFMediaType* decodermediatype;
            IMFMediaSession* mediasession;

         MFCreateSourceResolver(&pSourceResolver);
           

          pSourceResolver->CreateObjectFromURL(
                L"song.mp3",                       // URL of the source.
                MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
                NULL,                       // Optional property store.
                &ObjectType,     // Receives the created object type. 
                &pSource          // Receives a pointer to the media source.
            );
         

           pSource->QueryInterface(IID_PPV_ARGS(&mSource));





            mSource->CreatePresentationDescriptor(&ppPresentationDescriptor);

            ppPresentationDescriptor->SelectStream(0);
         hr = CoCreateInstance(__uuidof(CMP3DecMediaObject),
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_IMFTransform,
                (void**)&mtransform);

         if (SUCCEEDED(hr)) {

         }
            MFCreateTopology(
                &topology
            );

            

            MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &source);

            source->SetUnknown(MF_TOPONODE_SOURCE, mSource);
            source->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, ppPresentationDescriptor);
            BOOL value;
            ppPresentationDescriptor->GetStreamDescriptorByIndex(0, &value, &sdesip);

          

            MFCreateMediaType(&decodermediatype);
            decodermediatype->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);

            decodermediatype->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

            decodermediatype->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 1);

            sdesip->GetMediaTypeHandler(&mediatypehandler);

            mediatypehandler->GetCurrentMediaType(&mediatype);

            mtransform->SetInputType(0, mediatype, NULL);
            mtransform->SetOutputType(0, decodermediatype, NULL);

            source->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, sdesip);

            topology->AddNode(source);

         

            MFCreateTopologyNode(MF_TOPOLOGY_TRANSFORM_NODE, &transform);


            transform->SetObject(mtransform);

            topology->AddNode(transform);

            IMFActivate* active;


           hr =  MFCreateAudioRendererActivate(&active);

          if (SUCCEEDED(hr)) {
              
             
          }

         
        
          

        hr =     active->ActivateObject(__uuidof(IMFMediaSink), (void**)&mediasink);


        if (SUCCEEDED(hr)) {
           
           
        }
           
           
            MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &output);

           std::cout<<

      output->SetObject(mediasink);

            output->SetUINT32(MF_TOPONODE_STREAMID, 0);

            output->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);

            topology->AddNode(output);


        source->ConnectOutput(0, transform, 0);

          transform->ConnectOutput(0, output, 0);

           

            MFCreateMediaSession(
                NULL,
                &mediasession
            );

            PROPVARIANT var;
            PropVariantInit(&var);
            var.vt = VT_I8;
            var.hVal.QuadPart = 0; // 10^7 = 1 second.
            mediasession->SetTopology(MFSESSION_SETTOPOLOGY_NORESOLUTION, topology);
           
            mediasession->Start(NULL, &var);

          pSourceResolver->Release();
            pSource->Release();
            mSource->Release();
           ppPresentationDescriptor->Release();
           mtransform->Release();
           topology->Release();
           sdesip->Release();
           source->Release();
           transform->Release();
          output->Release();
           mediatypehandler->Release();
            mediatype->Release();
          decodermediatype->Release();
          mediasession->Shutdown();
            mediasession->Release();
            mediasink->Release();

            MFShutdown();

        }
        CoUninitialize();
        }
    