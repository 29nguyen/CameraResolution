// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using System;
using System.Collections.Generic;
using System.Numerics;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.DirectX.Direct3D11;
using Windows.Media.Effects;
using Windows.Media.MediaProperties;
using Windows.UI;

namespace ExampleGallery.Effects
{
    public sealed class RotatedTilesEffect : IBasicVideoEffect
    {
        CanvasDevice canvasDevice;
        int numColumns, numRows;

        // Scales a number from SinCos range [-1, 1] to range [outputMin, outputMax].
        private float Rescale(float input, float outputMin, float outputMax)
        {
            return outputMin + (input + 1) / 2 * (outputMax - outputMin);
        }

        public IReadOnlyList<VideoEncodingProperties> SupportedEncodingProperties { get { return new List<VideoEncodingProperties>(); } }

        public bool IsReadOnly { get { return false; } }

        public MediaMemoryTypes SupportedMemoryTypes { get { return MediaMemoryTypes.Gpu; } }

        public void SetProperties(IPropertySet configuration) { }

        public bool TimeIndependent { get { return false; } }

        public void Close(MediaEffectClosedReason reason) { if (canvasDevice != null) { canvasDevice.Dispose(); } }

        public void DiscardQueuedFrames() { }

        public void ProcessFrame(ProcessVideoFrameContext context)
        {
            using (CanvasBitmap input = CanvasBitmap.CreateFromDirect3D11Surface(canvasDevice, context.InputFrame.Direct3DSurface))
            using (CanvasRenderTarget output = CanvasRenderTarget.CreateFromDirect3D11Surface(canvasDevice, context.OutputFrame.Direct3DSurface))
            using (CanvasDrawingSession ds = output.CreateDrawingSession())
            {
                ds.Clear(Colors.Black);
                byte[] buffer = input.GetPixelBytes();

                for (uint i = 0; i < numColumns; i++)
                {
                    for (uint j = 0; j < numRows; j++)
                    {
                        buffer[3*(j * numColumns + i)+0] = 255;
                    }
                }
                var bitmap = CanvasBitmap.CreateFromBytes(canvasDevice,buffer,(numColumns),(numRows), Windows.Graphics.DirectX.DirectXPixelFormat.B8G8R8A8UIntNormalized);
                ds.DrawImage(bitmap);
            }
        }

        public void SetEncodingProperties(VideoEncodingProperties encodingProperties, IDirect3DDevice device)
        {
            canvasDevice = CanvasDevice.CreateFromDirect3D11Device(device);
            numColumns = (int)(encodingProperties.Width);
            numRows = (int)(encodingProperties.Height );
        }
    }
}
