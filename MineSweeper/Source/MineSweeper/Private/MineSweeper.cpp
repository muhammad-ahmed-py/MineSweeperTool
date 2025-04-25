// Copyright Epic Games, Inc. All Rights Reserved.

#include "MineSweeper.h"
#include "MineSweeperStyle.h"
#include "MineSweeperCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "SMinesweeperWidget.h"

static const FName MineSweeperTabName("MineSweeper");

#define LOCTEXT_NAMESPACE "FMineSweeperModule"

void FMineSweeperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FMineSweeperStyle::Initialize();
	FMineSweeperStyle::ReloadTextures();

	FMineSweeperCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMineSweeperCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FMineSweeperModule::PluginButtonClicked),
		FCanExecuteAction());

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MineSweeperTabName,
		FOnSpawnTab::CreateRaw(this, &FMineSweeperModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FMinesweeperTabTitle", "Minesweeper"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMineSweeperModule::RegisterMenus));
}

void FMineSweeperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MineSweeperTabName);

	FMineSweeperStyle::Shutdown();

	FMineSweeperCommands::Unregister();
}

void FMineSweeperModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MineSweeperTabName);
}

TSharedRef<class SDockTab> FMineSweeperModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SMinesweeperWidget)
		];
}

void FMineSweeperModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMineSweeperCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMineSweeperCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMineSweeperModule, MineSweeper)
