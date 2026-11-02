// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "ListViewBaseHeaderItemIntegrationTests.h"

#include <generic\DependencyObjectTests.h>
#include <generic\FrameworkElementTests.h>

#include <XamlTailored.h>
#include <TestEvent.h>
#include <TestCleanupWrapper.h>
#include <ControlHelper.h>
#include <RuntimeEnabledFeaturesEnum.h>
#include <WUCRenderingScopeGuard.h>
#include <AutomationClient\AutomationGenericTests.h>
#include <AutomationClient\AutomationEventHandler.h>
#include <AutomationClient\AutomationClientManager.h>
#include <AutomationClient\AutomationGenericTests.h>
#include <UIAutomationHelper.h>

#include <ChangeDPI.h>

using namespace Microsoft::UI::Xaml::Tests::Common;
using namespace test_infra;
using namespace Microsoft::UI::Xaml::Tests::Automation;

namespace Microsoft { namespace UI { namespace Xaml { namespace Tests { namespace Controls { namespace ListViewBaseHeaderItem {

    template<typename T>
    static void SetupMoCoWithVariousVisualStates(Platform::String^ type)
    {
        T^ normal = nullptr;
        T^ pointerOver = nullptr;
        T^ pressed = nullptr;
        T^ disabled = nullptr;
        T^ focused = nullptr;

        RunOnUIThread([&]()
        {
            auto rootPanel = dynamic_cast<xaml_controls::Grid^> (xaml_markup::XamlReader::Load(
                L"<Grid xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml' x:Name='root'>"
                L"    <StackPanel>"
                L"        <" + type + L" x:Name='normal' Content='Normal' AutomationProperties.Name='item1'/>"
                L"        <" + type + L" x:Name='pointerOver' Content='Pointer Over'/>"
                L"        <" + type + L" x:Name='pressed' Content='Pressed'/>"
                L"        <" + type + L" x:Name='disabled' Content='Disabled'/>"
                L"        <" + type + L" x:Name='focused' Content='Focused'/>"
                L"    </StackPanel>"
                L"</Grid>"));
            VERIFY_IS_NOT_NULL(rootPanel);

            normal = safe_cast<T^>(rootPanel->FindName(L"normal"));
            pointerOver = safe_cast<T^>(rootPanel->FindName(L"pointerOver"));
            pressed = safe_cast<T^>(rootPanel->FindName(L"pressed"));
            disabled = safe_cast<T^>(rootPanel->FindName(L"disabled"));
            focused = safe_cast<T^>(rootPanel->FindName(L"focused"));

            TestServices::WindowHelper->WindowContent = rootPanel;
        });
        TestServices::WindowHelper->WaitForIdle();

        RunOnUIThread([&]()
        {
            VisualStateManager::GoToState(normal, "Normal", false);

            VisualStateManager::GoToState(pointerOver, "PointerOver", false);

            VisualStateManager::GoToState(pressed, "Pressed", false);

            VisualStateManager::GoToState(disabled, "Disabled", false);

            VisualStateManager::GoToState(focused, "Focused", false);
        });
        TestServices::WindowHelper->WaitForIdle();
    }

    //
    // Class & Test Setup
    //
    bool IntegrationTests::ClassSetup()
    {
        CommonTestSetupHelper::CommonTestClassSetup();
        return true;
    }

    bool IntegrationTests::ClassCleanup()
    {
        return true;
    }

    bool IntegrationTests::TestSetup()
    {
        test_infra::TestServices::WindowHelper->InitializeXaml();
        return true;
    }

    bool IntegrationTests::TestCleanup()
    {
        test_infra::TestServices::WindowHelper->ShutdownXaml();
        TestServices::WindowHelper->VerifyTestCleanup();
        return true;
    }

    //
    // Test Cases
    //
    void IntegrationTests::CanInstantiate()
    {
        Generic::DependencyObjectTests<xaml_controls::ListViewHeaderItem>::CanInstantiate();
        Generic::DependencyObjectTests<xaml_controls::GridViewHeaderItem>::CanInstantiate();
    }

    void IntegrationTests::CanEnterAndLeaveLiveTree()
    {
        Generic::FrameworkElementTests<xaml_controls::ListViewHeaderItem>::CanEnterAndLeaveLiveTree();
        Generic::FrameworkElementTests<xaml_controls::GridViewHeaderItem>::CanEnterAndLeaveLiveTree();
    }

    void IntegrationTests::VisualStyleListViewHeaderItem()
    {
        // MockDComp should be injected and detached per test, since it keeps information like the surfaces
        // that get created.
        WUCRenderingScopeGuard guard(DCompRendering::WUCCompleteSynchronousCompTree, false /*resizeWindow*/);

        // Set a consistent window size and zoom scale, so that the DComp tree is predictable.
        TestServices::WindowHelper->SetWindowSizeOverride(wf::Size(400.0f, 600.0f));

        SetupMoCoWithVariousVisualStates<xaml_controls::ListViewHeaderItem>(L"ListViewHeaderItem");

        TestServices::Utilities->VerifyMockDCompOutput(MockDComp::SurfaceComparison::NoComparison);
    }

    void IntegrationTests::VisualStyleGridViewHeaderItem()
    {
        // MockDComp should be injected and detached per test, since it keeps information like the surfaces
        // that get created.
        WUCRenderingScopeGuard guard(DCompRendering::WUCCompleteSynchronousCompTree, false /*resizeWindow*/);

        // Set a consistent window size and zoom scale, so that the DComp tree is predictable.
        TestServices::WindowHelper->SetWindowSizeOverride(wf::Size(400.0f, 600.0f));

        SetupMoCoWithVariousVisualStates<xaml_controls::GridViewHeaderItem>(L"GridViewHeaderItem");

        TestServices::Utilities->VerifyMockDCompOutput(MockDComp::SurfaceComparison::NoComparison);
    }

    void IntegrationTests::CanGotoPointerOverVisualState()
    {
        TestCleanupWrapper cleanup;

        xaml_controls::Grid^ rootPanel = nullptr;
        xaml_controls::ListViewHeaderItem^ headerItem = nullptr;
        xaml_animation::Storyboard^ pointerOverStoryboard = nullptr;
        RunOnUIThread([&]()
        {
            rootPanel = safe_cast<xaml_controls::Grid^> (xaml_markup::XamlReader::Load(
                L"<Grid xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml' "
                L"      Width='400' Height='400' VerticalAlignment='Top' HorizontalAlignment='Left' Background='Black'/> "));

            test_infra::TestServices::WindowHelper->WindowContent = rootPanel;
            headerItem = ref new xaml_controls::ListViewHeaderItem();

            headerItem->Content = L"ListViewHeaderItem with all normal visual states";

            headerItem->Style = dynamic_cast<xaml::Style^>(xaml_markup::XamlReader::Load(
                L"<Style xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml' TargetType='ListViewHeaderItem'>"
                L"  <Setter Property='Template'>"
                L"    <Setter.Value>"
                L"      <ControlTemplate TargetType='ListViewHeaderItem'>"
                L"        <Border x:Name='Border'>"
                L"          <VisualStateManager.VisualStateGroups>"
                L"            <VisualStateGroup x:Name='CommonStates'>"
                L"              <VisualState x:Name='Normal'>"
                L"                <Storyboard>"
                L"                  <ObjectAnimationUsingKeyFrames Storyboard.TargetName='Border' Storyboard.TargetProperty='Background'>"
                L"                    <DiscreteObjectKeyFrame KeyTime='0' Value='Transparent' />"
                L"                  </ObjectAnimationUsingKeyFrames>"
                L"                  <ObjectAnimationUsingKeyFrames Storyboard.TargetName='ContentPresenter' Storyboard.TargetProperty='Foreground'>"
                L"                    <DiscreteObjectKeyFrame KeyTime='0' Value='Red' />"
                L"                  </ObjectAnimationUsingKeyFrames>"
                L"                </Storyboard>"
                L"              </VisualState>"
                L"              <VisualState x:Name='PointerOver'>"
                L"                <Storyboard>"
                L"                  <ObjectAnimationUsingKeyFrames Storyboard.TargetName='Border' Storyboard.TargetProperty='Background'>"
                L"                    <DiscreteObjectKeyFrame KeyTime='0' Value='Black' />"
                L"                  </ObjectAnimationUsingKeyFrames>"
                L"                  <ObjectAnimationUsingKeyFrames Storyboard.TargetName='ContentPresenter' Storyboard.TargetProperty='Foreground'>"
                L"                    <DiscreteObjectKeyFrame KeyTime='0' Value='White' />"
                L"                  </ObjectAnimationUsingKeyFrames>"
                L"                </Storyboard>"
                L"              </VisualState>"
                L"              <VisualState x:Name='Pressed'>"
                L"                <Storyboard>"
                L"                  <ObjectAnimationUsingKeyFrames Storyboard.TargetName='Border' Storyboard.TargetProperty='Background'>"
                L"                    <DiscreteObjectKeyFrame KeyTime='0' Value='Green' />"
                L"                  </ObjectAnimationUsingKeyFrames>"
                L"                  <ObjectAnimationUsingKeyFrames Storyboard.TargetName='ContentPresenter' Storyboard.TargetProperty='Foreground'>"
                L"                    <DiscreteObjectKeyFrame KeyTime='0' Value='Blue' />"
                L"                  </ObjectAnimationUsingKeyFrames>"
                L"                </Storyboard>"
                L"              </VisualState>"
                L"              <VisualState x:Name='Disabled'>"
                L"                <Storyboard>"
                L"                  <ObjectAnimationUsingKeyFrames Storyboard.TargetName='Border' Storyboard.TargetProperty='Background'>"
                L"                    <DiscreteObjectKeyFrame KeyTime='0' Value='Orange' />"
                L"                  </ObjectAnimationUsingKeyFrames>"
                L"                  <ObjectAnimationUsingKeyFrames Storyboard.TargetName='ContentPresenter' Storyboard.TargetProperty='Foreground'>"
                L"                    <DiscreteObjectKeyFrame KeyTime='0' Value='Red' />"
                L"                  </ObjectAnimationUsingKeyFrames>"
                L"                </Storyboard>"
                L"              </VisualState>"
                L"            </VisualStateGroup>"
                L"          </VisualStateManager.VisualStateGroups>"
                L"          <ContentPresenter x:Name='ContentPresenter' Content='{TemplateBinding Content}'/>"
                L"        </Border>"
                L"      </ControlTemplate>"
                L"    </Setter.Value>"
                L"  </Setter>"
                L"</Style>"
                ));
            rootPanel->Children->Append(headerItem);
        });

        TestServices::WindowHelper->WaitForIdle();

        RunOnUIThread([&]()
        {
            // find the root border and find the Month to Year transition.
            auto rootBorder = safe_cast<xaml_controls::Border^>(xaml_media::VisualTreeHelper::GetChild(headerItem, 0));
            VERIFY_IS_NOT_NULL(rootBorder);
            auto vsgs = VisualStateManager::GetVisualStateGroups(rootBorder);

            VisualState^ pointerOverState = nullptr;

            for (auto vsg : vsgs)
            {
                if (vsg->Name == L"CommonStates")
                {
                    for (auto state : vsg->States)
                    {
                        if (state->Name == L"PointerOver")
                        {
                            pointerOverState = state;
                            break;
                        }
                    }
                    if (pointerOverState != nullptr)
                    {
                        break;
                    }
                }
            }

            VERIFY_IS_NOT_NULL(pointerOverState);
            pointerOverStoryboard = pointerOverState->Storyboard;
            VERIFY_IS_NOT_NULL(pointerOverStoryboard);
        });

        auto storyboardCompletedEvent = std::make_shared<Event>();
        auto storyboardCompletedRegistration = CreateSafeEventRegistration(xaml_animation::Storyboard, Completed);

        storyboardCompletedRegistration.Attach(pointerOverStoryboard,
            ref new ::Windows::Foundation::EventHandler<Platform::Object ^>(
            [storyboardCompletedEvent](Platform::Object ^sender, Platform::Object ^e)
        {
            storyboardCompletedEvent->Set();
        }));

        TestServices::InputHelper->MoveMouse(headerItem);

        TestServices::WindowHelper->WaitForIdle();

        storyboardCompletedEvent->WaitForDefault();
        VERIFY_IS_TRUE(storyboardCompletedEvent->HasFired());

        TestServices::InputHelper->MoveMouse(::Windows::Foundation::Point(0, 0));
    }

    // Verifies the GroupPositionStates of the header of the group containing the item.
    static void VerifyHeaderGroupPositionState(xaml_controls::ListViewBase^ list, int itemIndex, const wchar_t* expectedState)
    {
        LOG_OUTPUT(L"Header of item %d: expecting %s.", itemIndex, expectedState);

        auto item = safe_cast<xaml::DependencyObject^>(list->ContainerFromIndex(itemIndex));
        auto header = safe_cast<xaml::FrameworkElement^>(list->GroupHeaderContainerFromItemContainer(item));
        auto root = safe_cast<xaml::FrameworkElement^>(xaml_media::VisualTreeHelper::GetChild(header, 0));
        auto groups = VisualStateManager::GetVisualStateGroups(root);

        VERIFY_ARE_EQUAL(Platform::StringReference(expectedState), groups->GetAt(0)->CurrentState->Name);
    }

    void IntegrationTests::VerifyGroupPositionStates()
    {
        TestCleanupWrapper cleanup;
        xaml_controls::ListView^ list = nullptr;
        Platform::Collections::Vector<Platform::Object^>^ groups = nullptr;

        RunOnUIThread([&]()
        {
            list = safe_cast<xaml_controls::ListView^>(xaml_markup::XamlReader::Load(
                LR"(<ListView xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'
                        xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'
                        Width='200' Height='400' VerticalAlignment='Top'>
                    <ListView.GroupStyle>
                        <GroupStyle HidesIfEmpty='True'>
                            <GroupStyle.HeaderContainerStyle>
                                <Style TargetType='ListViewHeaderItem'>
                                    <Setter Property='Template'>
                                        <Setter.Value>
                                            <ControlTemplate TargetType='ListViewHeaderItem'>
                                                <Border Height='20'>
                                                    <VisualStateManager.VisualStateGroups>
                                                        <VisualStateGroup x:Name='GroupPositionStates'>
                                                            <VisualState x:Name='FirstGroup'/>
                                                            <VisualState x:Name='SubsequentGroup'/>
                                                        </VisualStateGroup>
                                                    </VisualStateManager.VisualStateGroups>
                                                </Border>
                                            </ControlTemplate>
                                        </Setter.Value>
                                    </Setter>
                                </Style>
                            </GroupStyle.HeaderContainerStyle>
                        </GroupStyle>
                    </ListView.GroupStyle>
                </ListView>)"));

            // Groups [], [a], [b]; the empty group's header is hidden.
            groups = ref new Platform::Collections::Vector<Platform::Object^>();
            groups->Append(ref new Platform::Collections::Vector<Platform::String^>());
            groups->Append(ref new Platform::Collections::Vector<Platform::String^>({ L"a" }));
            groups->Append(ref new Platform::Collections::Vector<Platform::String^>({ L"b" }));

            auto cvs = ref new xaml_data::CollectionViewSource();
            cvs->IsSourceGrouped = true;
            cvs->Source = groups;
            list->ItemsSource = cvs->View;
            TestServices::WindowHelper->WindowContent = list;
        });
        TestServices::WindowHelper->WaitForIdle();

        LOG_OUTPUT(L"The first non-empty group is first.");
        RunOnUIThread([&]()
        {
            VerifyHeaderGroupPositionState(list, 0, L"FirstGroup");
            VerifyHeaderGroupPositionState(list, 1, L"SubsequentGroup");

            safe_cast<Platform::Collections::Vector<Platform::String^>^>(groups->GetAt(0))->Append(L"x");
        });
        TestServices::WindowHelper->WaitForIdle();

        LOG_OUTPUT(L"Filling the empty group makes it first.");
        RunOnUIThread([&]()
        {
            VerifyHeaderGroupPositionState(list, 0, L"FirstGroup");
            VerifyHeaderGroupPositionState(list, 1, L"SubsequentGroup");

            groups->RemoveAt(0);
        });
        TestServices::WindowHelper->WaitForIdle();

        LOG_OUTPUT(L"Removing the first group makes the next one first.");
        RunOnUIThread([&]()
        {
            VerifyHeaderGroupPositionState(list, 0, L"FirstGroup");
            VerifyHeaderGroupPositionState(list, 1, L"SubsequentGroup");

            groups->InsertAt(0, ref new Platform::Collections::Vector<Platform::String^>({ L"y" }));
        });
        TestServices::WindowHelper->WaitForIdle();

        LOG_OUTPUT(L"Inserting a group before the first one makes it subsequent.");
        RunOnUIThread([&]()
        {
            VerifyHeaderGroupPositionState(list, 0, L"FirstGroup");
            VerifyHeaderGroupPositionState(list, 1, L"SubsequentGroup");
        });
    }

    //
    // Private Methods
    //

} } } } } } // Microsoft::UI::Xaml::Tests::Controls::ListViewBaseHeaderItem
