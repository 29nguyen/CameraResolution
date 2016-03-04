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

#include "pch.h"
#include "Scenario1_PreviewSettings.xaml.h"
#include "StreamResolutionHelper.h"
#include <sstream>

using namespace BufferAccess_Effects;
using namespace ExampleGallery::Effects;
using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Effects;
using namespace Windows::Media;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes a new instance of the <see cref="Scenario1_PreviewSettings"/> class.
/// </summary>
Scenario1_PreviewSettings::Scenario1_PreviewSettings() 
    : _rootPage(MainPage::Current)
    , _mediaCapture1(nullptr)
	, _mediaCapture2(nullptr)
    , _isPreviewing(false)
{
    InitializeComponent();
	ScenarioInit();
}

Scenario1_PreviewSettings::~Scenario1_PreviewSettings()
{
    CleanupCameraAsync(1);
	CleanupCameraAsync(2);
}

void Scenario1_PreviewSettings::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    CleanupCameraAsync(1);
	CleanupCameraAsync(2);
}

void Scenario1_PreviewSettings::ScenarioInit()
{
	EnumerateWebcamsAsync();
}

//// Hieu
//// Emumerate Camera devices
void Scenario1_PreviewSettings::EnumerateWebcamsAsync()
{
	ShowStatusMessage("Enumerating Webcams...");
	m_devInfoCollection1 = nullptr;
	m_devInfoCollection2 = nullptr;
	

	EnumedDeviceList1->Items->Clear();
	EnumedDeviceList2->Items->Clear();
	create_task(DeviceInformation::FindAllAsync(DeviceClass::VideoCapture)).then([this](task<DeviceInformationCollection^> findTask)
	{
		try
		{
			m_devInfoCollection1 = findTask.get();
			m_devInfoCollection2 = findTask.get();
			if (m_devInfoCollection1 == nullptr || m_devInfoCollection1->Size == 0)
			{
				ShowStatusMessage("No WebCams 1 found.");
			}

			else if (m_devInfoCollection2 == nullptr || m_devInfoCollection2->Size == 0)
			{
				ShowStatusMessage("No WebCams 2 found.");
			}
			else
			{
				// Enumerate for Camera 1
				for (unsigned int i = 0; i < m_devInfoCollection1->Size; i++)
				{
					auto devInfo = m_devInfoCollection1->GetAt(i);
					auto location = devInfo->EnclosureLocation;

					if (location != nullptr)
					{

						if (location->Panel == Windows::Devices::Enumeration::Panel::Front)
						{
							EnumedDeviceList1->Items->Append(devInfo->Name + "-Front");
						}
						else if (location->Panel == Windows::Devices::Enumeration::Panel::Back)
						{
							EnumedDeviceList1->Items->Append(devInfo->Name + "-Back");
						}
						else {
							EnumedDeviceList1->Items->Append(devInfo->Name);
						}
					}
					else {
						EnumedDeviceList1->Items->Append(devInfo->Name);
					}
				}

				// Enumerate for Camera 2
				for (unsigned int i = 0; i < m_devInfoCollection2->Size; i++)
				{
					auto devInfo = m_devInfoCollection2->GetAt(i);
					auto location = devInfo->EnclosureLocation;

					if (location != nullptr)
					{

						if (location->Panel == Windows::Devices::Enumeration::Panel::Front)
						{
							EnumedDeviceList2->Items->Append(devInfo->Name + "-Front");
						}
						else if (location->Panel == Windows::Devices::Enumeration::Panel::Back)
						{
							EnumedDeviceList2->Items->Append(devInfo->Name + "-Back");
						}
						else {
							EnumedDeviceList2->Items->Append(devInfo->Name);
						}
					}
					else {
						EnumedDeviceList2->Items->Append(devInfo->Name);
					}
				}

				EnumedDeviceList1->SelectedIndex = 0;
				EnumedDeviceList2->SelectedIndex = 0;
				ShowStatusMessage("Enumerating Webcams completed successfully.");
			}

		}
		catch (Platform::Exception^ e)
		{
			ShowExceptionMessage(e);
		}
	});
}

/// <summary>
/// Initializes the camera
/// </summary>
task<void> Scenario1_PreviewSettings::InitializeCameraAsync(int i, Windows::Media::Capture::MediaCaptureInitializationSettings^ settings)
{
    _mediaCapture1 = ref new MediaCapture();
	_mediaCapture2 = ref new MediaCapture();
    _mediaCaptureFailedEventToken1 =
        _mediaCapture1->Failed += ref new MediaCaptureFailedEventHandler(this, &Scenario1_PreviewSettings::MediaCapture_Failed);
	_mediaCaptureFailedEventToken2 =
		_mediaCapture2->Failed += ref new MediaCaptureFailedEventHandler(this, &Scenario1_PreviewSettings::MediaCapture_Failed);

	if (i == 1)
	{
		return create_task(_mediaCapture1->InitializeAsync(settings))
			.then([this]()
		{
			PreviewControl1->Source = _mediaCapture1.Get();
			return create_task(_mediaCapture1->StartPreviewAsync());
		}).then([this](task<void> previousTask)
		{
			try
			{
				_isPreviewing = true;
				previousTask.get();
				return previousTask;
			}
			catch (AccessDeniedException^)
			{
				_rootPage->Current->NotifyUser("The app was denied access to the camera", NotifyType::ErrorMessage);
				return CleanupCameraAsync(1);
			}
		});
	}

	else if (i == 2)
	{
		return create_task(_mediaCapture2->InitializeAsync(settings))
			.then([this]()
		{
			PreviewControl2->Source = _mediaCapture2.Get();
			return create_task(_mediaCapture2->StartPreviewAsync());
		}).then([this](task<void> previousTask)
		{
			try
			{
				_isPreviewing = true;
				previousTask.get();
				return previousTask;
			}
			catch (AccessDeniedException^)
			{
				_rootPage->Current->NotifyUser("The app was denied access to the camera", NotifyType::ErrorMessage);
				return CleanupCameraAsync(2);
			}
		});
	}
	
}

/// <summary>
/// Cleans up the camera and state variables
/// </summary>
task<void> Scenario1_PreviewSettings::CleanupCameraAsync(int i)
{
    task<void> cleanupTask;

	if (i == 1)
	{
		if (_isPreviewing)
		{
			cleanupTask = create_task(_mediaCapture1->StopPreviewAsync());
			_isPreviewing = false;
		}
		else
		{
			cleanupTask = create_task([]() {});
		}

		return cleanupTask.then([this]()
		{
			if (_mediaCapture1 != nullptr)
			{
				_mediaCapture1->Failed -= _mediaCaptureFailedEventToken1;
				_mediaCapture1 = nullptr;
			}
		});
	}
	else if (i == 2)
	{
		if (_isPreviewing)
		{
			cleanupTask = create_task(_mediaCapture2->StopPreviewAsync());
			_isPreviewing = false;
		}
		else
		{
			cleanupTask = create_task([]() {});
		}

		return cleanupTask.then([this]()
		{
			if (_mediaCapture2 != nullptr)
			{
				_mediaCapture2->Failed -= _mediaCaptureFailedEventToken2;
				_mediaCapture2 = nullptr;
			}
		});
	}
}

/// <summary>
/// Initializes the camera and populates the UI
/// </summary>
/// <param name="sender"></param>
void Scenario1_PreviewSettings::InitializeCameraButton_Tapped1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
	auto settings = ref new Windows::Media::Capture::MediaCaptureInitializationSettings();
	auto chosenDevInfo = m_devInfoCollection1->GetAt(EnumedDeviceList1->SelectedIndex);
	settings->VideoDeviceId = chosenDevInfo->Id;

    Button^ button = static_cast<Button^>(sender);
    button->IsEnabled = false;

    create_task(InitializeCameraAsync(1, settings))
        .then([this, button]()
    {
        if (_isPreviewing)
        {
            button->Visibility = Windows::UI::Xaml::Visibility::Visible;
            PreviewControl1->Visibility = Windows::UI::Xaml::Visibility::Visible;
            PopulateSettingsComboBox(1);
        }
    });    
}

void Scenario1_PreviewSettings::InitializeCameraButton_Tapped2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
	auto settings = ref new Windows::Media::Capture::MediaCaptureInitializationSettings();
	auto chosenDevInfo = m_devInfoCollection2->GetAt(EnumedDeviceList2->SelectedIndex);
	settings->VideoDeviceId = chosenDevInfo->Id;

	Button^ button = static_cast<Button^>(sender);
	button->IsEnabled = false;

	create_task(InitializeCameraAsync(2, settings))
		.then([this, button]()
	{
		if (_isPreviewing)
		{
			button->Visibility = Windows::UI::Xaml::Visibility::Visible;
			PreviewControl2->Visibility = Windows::UI::Xaml::Visibility::Visible;
			PopulateSettingsComboBox(2);
		}
	});
}


/// <summary>
///  Event handler for Preview settings combo box. Updates stream resolution based on the selection.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_PreviewSettings::ComboBoxSettings_Changed1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
	if (_isPreviewing)
	{
		auto selectedItem = static_cast<ComboBoxItem^>(static_cast<ComboBox^>(sender)->SelectedItem);
		auto encodingProperties = static_cast<IMediaEncodingProperties^>(selectedItem->Tag);
		create_task(_mediaCapture1->VideoDeviceController->SetMediaStreamPropertiesAsync(MediaStreamType::VideoPreview, encodingProperties));
		CreatEffectAsync();
		ShowStatusMessage("Add effect successfully");
	}
}

void Scenario1_PreviewSettings::ComboBoxSettings_Changed2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^)
{
	if (_isPreviewing)
	{
		auto selectedItem = static_cast<ComboBoxItem^>(static_cast<ComboBox^>(sender)->SelectedItem);
		auto encodingProperties = static_cast<IMediaEncodingProperties^>(selectedItem->Tag);
		create_task(_mediaCapture2->VideoDeviceController->SetMediaStreamPropertiesAsync(MediaStreamType::VideoPreview, encodingProperties));
	}
}

/// <summary>
/// Populates the combo box with all possible combinations of settings returned by the camera driver
/// </summary>
void Scenario1_PreviewSettings::PopulateSettingsComboBox(int i)
{
	if (i == 1)
	{
		// Query all properties of the device
		auto allProperties = _mediaCapture1->VideoDeviceController->GetAvailableMediaStreamProperties(MediaStreamType::VideoPreview);
		// Populate the combo box with the entries
		for (auto encodingProperties : allProperties)
		{
			ComboBoxItem^ comboBoxItem = ref new ComboBoxItem();
			auto streamHelper = ref new StreamResolutionHelper(encodingProperties);
			comboBoxItem->Content = streamHelper->GetFriendlyName();
			comboBoxItem->Tag = encodingProperties;
			CameraSettings1->Items->Append(comboBoxItem);
		}
	}
	else if (i == 2)
	{
		// Query all properties of the device
		auto allProperties = _mediaCapture2->VideoDeviceController->GetAvailableMediaStreamProperties(MediaStreamType::VideoPreview);

		// Populate the combo box with the entries
		for (auto encodingProperties : allProperties)
		{
			ComboBoxItem^ comboBoxItem = ref new ComboBoxItem();
			auto streamHelper = ref new StreamResolutionHelper(encodingProperties);
			comboBoxItem->Content = streamHelper->GetFriendlyName();
			comboBoxItem->Tag = encodingProperties;
			CameraSettings2->Items->Append(comboBoxItem);
		}
	}
}

void Scenario1_PreviewSettings::MediaCapture_Failed(MediaCapture^, MediaCaptureFailedEventArgs^ e)
{
    String^ message = e->Message;
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message]()
    {
        _rootPage->Current->NotifyUser("Preview stopped: " + message, NotifyType::ErrorMessage);
        _isPreviewing = false;
        CleanupCameraAsync(1);
		CleanupCameraAsync(2);
    }));
}

void Scenario1_PreviewSettings::ShowStatusMessage(Platform::String^ text)
{
	_rootPage->NotifyUser(text, NotifyType::StatusMessage);
}

void Scenario1_PreviewSettings::ShowExceptionMessage(Platform::Exception^ ex)
{
	_rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
}

void Scenario1_PreviewSettings::lstEnumedDevices_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	//Have to modify here to enable the Initialize camera button
}

void Scenario1_PreviewSettings::EnumedDevices_SelectionChanged2(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	//Have to modify here to enable the Initialize camera button
}

task<void> Scenario1_PreviewSettings::CreatEffectAsync()
{
	Platform::String ^typeName = AccessBuffer::typeid->FullName;
	ShowStatusMessage(typeName);
	effectPropertySet = ref new PropertySet();
	// Create the definition, which will contain some initialization settings
	auto definition = ref new VideoEffectDefinition(typeName);
	
	// Add the effect to the preview stream
	return create_task(_mediaCapture1->AddVideoEffectAsync(definition, MediaStreamType::VideoPreview))
		.then([this](IMediaExtension^ extension)
	{
	});
}