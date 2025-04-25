#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Http.h"

class SUniformGridPanel;

class SMinesweeperWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMinesweeperWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget> BuildConfigPanel();
	TSharedRef<SWidget> BuildGrid();

	void OnStartNewGame();
	FReply OnTileClicked(int32 Row, int32 Col);

	void ProcessNaturalLanguagePrompt(const FString& Prompt);
	void ParseAndApplyMCPResponse(const FString& Response);
	void SendPromptToLLM(const FString& Prompt);

	int32 GridWidth;
	int32 GridHeight;
	int32 NumBombs;

	TArray<TArray<int32>> GridData;
	TArray<TArray<bool>> Revealed;
	TSharedPtr<SUniformGridPanel> GridPanel;

	void GenerateGrid();
	void RevealTile(int32 Row, int32 Col);
	int32 CountAdjacentBombs(int32 Row, int32 Col);
	bool IsInsideGrid(int32 Row, int32 Col) const;
};