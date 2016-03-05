#pragma once
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Media::Effects;
using namespace Windows::Media::MediaProperties;
using namespace Windows::UI;


namespace BufferAccess_Effects
{
	public ref class AccessBuffer sealed: public IBasicVideoEffect
	{
	private:
		CanvasDevice ^canvasDevice;
		int numColumns, numRows;

	public:
		virtual property IVectorView<VideoEncodingProperties^> ^SupportedEncodingProperties
		{
			IVectorView<VideoEncodingProperties^> ^get();
		}
		
		virtual property bool IsReadOnly
		{
			bool get();
		}

		virtual property MediaMemoryTypes SupportedMemoryTypes
		{
			MediaMemoryTypes get();
		}

		virtual void SetProperties(IPropertySet ^configuration);

		virtual property bool TimeIndependent
		{
			bool get();
		}

		virtual void Close(MediaEffectClosedReason reason);

		virtual void DiscardQueuedFrames();

		virtual void ProcessFrame(ProcessVideoFrameContext ^context);

		virtual void SetEncodingProperties(VideoEncodingProperties ^encodingProperties, IDirect3DDevice ^device);
	};
}
