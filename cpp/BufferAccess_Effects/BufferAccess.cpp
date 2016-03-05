
#include "pch.h"
#include "BufferAccess.h"


using namespace Windows::Foundation;
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
using namespace Platform::Collections;

namespace BufferAccess_Effects
{
	IVectorView<VideoEncodingProperties^> ^AccessBuffer::SupportedEncodingProperties::get()
	{
		return ref new VectorView<VideoEncodingProperties^>();
	}

	bool AccessBuffer::IsReadOnly::get()
	{
		return false;
	}

	MediaMemoryTypes AccessBuffer::SupportedMemoryTypes::get()
	{
		return MediaMemoryTypes::Gpu;
	}

	void AccessBuffer::SetProperties(IPropertySet ^configuration)
	{
	}

	bool AccessBuffer::TimeIndependent::get()
	{
		return false;
	}

	void AccessBuffer::Close(MediaEffectClosedReason reason)
	{
		if (canvasDevice != nullptr)
		{
			delete canvasDevice;
		}
	}

	void AccessBuffer::DiscardQueuedFrames()
	{
	}

	void AccessBuffer::ProcessFrame(ProcessVideoFrameContext ^context)
	{
		{
			CanvasBitmap ^input = CanvasBitmap::CreateFromDirect3D11Surface(canvasDevice, context->InputFrame->Direct3DSurface);
			{
				CanvasRenderTarget ^output = CanvasRenderTarget::CreateFromDirect3D11Surface(canvasDevice, context->OutputFrame->Direct3DSurface);
				{
					CanvasDrawingSession ^ds = output->CreateDrawingSession();
					ds->Clear(Colors::Black);
					Platform::Array<uint8> ^buffer = input->GetPixelBytes();

					for (int i = 0; i < numColumns; i++)
					{
						for (int j = 0; j < numRows; j++)
						{
							buffer[3 * (j * numColumns + i) + 0] = 255;
						}
					}
					auto bitmap = CanvasBitmap::CreateFromBytes(canvasDevice, buffer, (numColumns), (numRows), Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized);
					ds->DrawImage(bitmap);
				}
			}
		}
	}

	void AccessBuffer::SetEncodingProperties(VideoEncodingProperties ^encodingProperties, IDirect3DDevice ^device)
	{
		canvasDevice = CanvasDevice::CreateFromDirect3D11Device(device);
		numColumns = safe_cast<int32>(encodingProperties->Width);
		numRows = safe_cast<int32>(encodingProperties->Height);
	}

}