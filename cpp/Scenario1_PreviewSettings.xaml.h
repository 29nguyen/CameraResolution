//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "Scenario1_PreviewSettings.g.h"
#include "MainPage.xaml.h"

using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation::Collections;

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_PreviewSettings sealed
    {
    public:
        Scenario1_PreviewSettings();
        virtual ~Scenario1_PreviewSettings();

    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ _rootPage;
		void ScenarioInit();
        Platform::Agile<Windows::Media::Capture::MediaCapture^> _mediaCapture1;
		Platform::Agile<Windows::Media::Capture::MediaCapture^> _mediaCapture2;
        Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventToken1;
		Windows::Foundation::EventRegistrationToken _mediaCaptureFailedEventToken2;
		IPropertySet ^effectPropertySet = nullptr;
		//IBasicVideoEffect ^_accessbuffer;
        bool _isPreviewing;

        Concurrency::task<void> InitializeCameraAsync(int i, Windows::Media::Capture::MediaCaptureInitializationSettings^ settings);
        Concurrency::task<void> CleanupCameraAsync(int i);
		Concurrency::task<void> GetPreviewFrameAsSoftwareBitmapAsync();
		Concurrency::task<void> CreatEffectAsync();

        void InitializeCameraButton_Tapped1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void InitializeCameraButton_Tapped2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void ComboBoxSettings_Changed1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ComboBoxSettings_Changed2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void PopulateSettingsComboBox(int i);

        void MediaCapture_Failed(Windows::Media::Capture::MediaCapture^ sender, Windows::Media::Capture::MediaCaptureFailedEventArgs^ e);

		void EnumerateWebcamsAsync();

		void ShowStatusMessage(Platform::String^ text);
		void ShowExceptionMessage(Platform::Exception^ ex);
		void lstEnumedDevices_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void EnumedDevices_SelectionChanged2(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

		DeviceInformationCollection^ m_devInfoCollection1;
		DeviceInformationCollection^ m_devInfoCollection2;
    };
}
