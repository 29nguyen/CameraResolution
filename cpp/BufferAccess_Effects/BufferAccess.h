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
	public ref class AccessBuffer sealed: IBasicVideoEffect
	{
	private:
		CanvasDevice ^canvasDevice;
		int numColumns, numRows;

	public:
		property IVector<VideoEncodingProperties^> ^SupportedEncodingProperties
		{
			IVector<VideoEncodingProperties^> ^get();
		}

		property bool IsReadOnly
		{
			bool get();
		}

		property MediaMemoryTypes SupportedMemoryTypes
		{
			MediaMemoryTypes get();
		}

		void SetProperties(IPropertySet ^configuration);

		property bool TimeIndependent
		{
			bool get();
		}

		void Close(MediaEffectClosedReason reason);

		void DiscardQueuedFrames();

		void ProcessFrame(ProcessVideoFrameContext ^context);

		void SetEncodingProperties(VideoEncodingProperties ^encodingProperties, IDirect3DDevice ^device);
	};
}
