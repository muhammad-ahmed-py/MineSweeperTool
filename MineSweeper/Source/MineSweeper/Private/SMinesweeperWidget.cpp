#include "SMinesweeperWidget.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/MessageDialog.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"

void SMinesweeperWidget::Construct(const FArguments& InArgs)
{
	GridWidth = 20;
	GridHeight = 20;
	NumBombs = 5;

	ChildSlot
		[
		SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(5)
			[
				BuildConfigPanel()
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(5)
			[
				SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().FillWidth(1).Padding(5)
					[
						SNew(SEditableTextBox)
							.HintText(FText::FromString("Enter game config like: 'Start a 10x10 game with 15 bombs'"))
							.OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type CommitType)
							{
								if (CommitType == ETextCommit::OnEnter)
								{
									ProcessNaturalLanguagePrompt(Text.ToString());
								}
							})
					]
			]
			+ SVerticalBox::Slot().FillHeight(1).Padding(5)
			[
				SAssignNew(GridPanel, SUniformGridPanel)
			]
		];

	OnStartNewGame();
}

TSharedRef<SWidget> SMinesweeperWidget::BuildConfigPanel()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(5)
		[
			SNew(STextBlock).Text(FText::FromString("Width"))
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(5)
		[
			SNew(SEditableTextBox).Text(FText::AsNumber(GridWidth))
				.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType)
				{
					GridWidth = FCString::Atoi(*NewText.ToString());
				})
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(5)
		[
			SNew(STextBlock).Text(FText::FromString("Height"))
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(5)
		[
			SNew(SEditableTextBox).Text(FText::AsNumber(GridHeight))
				.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType)
				{
					GridHeight = FCString::Atoi(*NewText.ToString());
				})
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(5)
		[
			SNew(STextBlock).Text(FText::FromString("Bombs"))
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(5)
		[
			SNew(SEditableTextBox).Text(FText::AsNumber(NumBombs))
				.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType)
				{
					NumBombs = FCString::Atoi(*NewText.ToString());
				})
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(5)
		[
			SNew(SButton)
				.Text(FText::FromString("Start New Game"))
				.OnClicked_Lambda([this]() -> FReply
				{
					OnStartNewGame();
					return FReply::Handled();
				})
		];
}

TSharedRef<SWidget> SMinesweeperWidget::BuildGrid()
{
	return SNew(STextBlock).Text(FText::FromString("Grid will render here"));
}

void SMinesweeperWidget::OnStartNewGame()
{
	GenerateGrid();
	if (GridPanel.IsValid())
	{
		GridPanel->ClearChildren();
		for (int32 Y = 0; Y < GridHeight; ++Y)
		{
			for (int32 X = 0; X < GridWidth; ++X)
			{
				GridPanel->AddSlot(X, Y)
					[
						SNew(SButton)
							.Text_Lambda([this, Y, X]() -> FText
							{
								if (!Revealed[Y][X]) return FText::FromString(" ");
								if (GridData[Y][X] == -1) return FText::FromString("*");
								int32 Count = CountAdjacentBombs(Y, X);
								return Count > 0 ? FText::AsNumber(Count) : FText::FromString(" ");
							})
							.OnClicked_Lambda([this, Y, X]()->FReply
							{
								RevealTile(Y, X);
								
								// Force UI to refresh after revealing a tile
								if (GridPanel.IsValid())
								{
									GridPanel->ClearChildren();
									for (int32 y = 0; y < GridHeight; ++y)
									{
										for (int32 x = 0; x < GridWidth; ++x)
										{
											GridPanel->AddSlot(x, y)
												[
													SNew(SButton)
														.Text_Lambda([this, y, x]() -> FText
														{
															if (!Revealed[y][x]) return FText::FromString(" ");
															if (GridData[y][x] == -1) return FText::FromString("*");
															int32 Count = CountAdjacentBombs(y, x);
															return Count > 0 ? FText::AsNumber(Count) : FText::FromString(" ");
														})
														.OnClicked_Lambda([this, y, x]() -> FReply
														{
															RevealTile(y, x);
															return FReply::Handled();
														})
												];
										}
									}
								}
								
								return FReply::Handled();
							})
					];
			}
		}
	}
}

void SMinesweeperWidget::GenerateGrid()
{
	GridData.SetNum(GridHeight);
	Revealed.SetNum(GridHeight);
	for (int32 Y = 0; Y < GridHeight; ++Y)
	{
		GridData[Y].SetNum(GridWidth);
		Revealed[Y].Init(false, GridWidth);
	}

	int32 BombsPlaced = 0;
	while (BombsPlaced < NumBombs)
	{
		int32 X = FMath::RandRange(0, GridWidth - 1);
		int32 Y = FMath::RandRange(0, GridHeight - 1);
		if (GridData[Y][X] != -1)
		{
			GridData[Y][X] = -1;
			++BombsPlaced;
		}
	}
}

bool SMinesweeperWidget::IsInsideGrid(int32 Row, int32 Col) const
{
	return Row >= 0 && Row < GridHeight && Col >= 0 && Col < GridWidth;
}

void SMinesweeperWidget::RevealTile(int32 Row, int32 Col)
{
	if (!IsInsideGrid(Row, Col) || Revealed[Row][Col]) return;

	Revealed[Row][Col] = true;
	if (GridData[Row][Col] == -1)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Game Over!"));
		return;
	}

	int32 Adjacent = CountAdjacentBombs(Row, Col);
	if (Adjacent == 0)
	{
		for (int32 Y = -1; Y <= 1; ++Y)
		{
			for (int32 X = -1; X <= 1; ++X)
			{
				if (X != 0 || Y != 0)
				{
					RevealTile(Row + Y, Col + X);
				}
			}
		}
	}
}

int32 SMinesweeperWidget::CountAdjacentBombs(int32 Row, int32 Col)
{
	int32 Count = 0;
	for (int32 Y = -1; Y <= 1; ++Y)
	{
		for (int32 X = -1; X <= 1; ++X)
		{
			int32 R = Row + Y;
			int32 C = Col + X;
			if (IsInsideGrid(R, C) && GridData[R][C] == -1)
			{
				++Count;
			}
		}
	}
	return Count;
}

void SMinesweeperWidget::ProcessNaturalLanguagePrompt(const FString& Prompt)
{
	SendPromptToLLM(Prompt);
}

void SMinesweeperWidget::SendPromptToLLM(const FString& Prompt)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL("https://api.deepseek.com/chat/completions");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", "Bearer sk-c933846fced045e4a968cfff240e46a8");

	TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject);
	JsonBody->SetStringField("model", "deepseek-chat");

	TArray<TSharedPtr<FJsonValue>> Messages;
	TSharedPtr<FJsonObject> UserMsg = MakeShareable(new FJsonObject);
	UserMsg->SetStringField("role", "user");
	UserMsg->SetStringField("content", Prompt);
	Messages.Add(MakeShareable(new FJsonValueObject(UserMsg)));
	JsonBody->SetArrayField("messages", Messages);

	FString BodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
	FJsonSerializer::Serialize(JsonBody.ToSharedRef(), Writer);
	Request->SetContentAsString(BodyString);

	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr RequestPtr, FHttpResponsePtr Response, bool bSuccess)
		{
			UE_LOG(LogTemp, Display, TEXT("Response Code: %d"), Response->GetResponseCode());

			if (bSuccess && Response->GetResponseCode() == 200)
			{
				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
				if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
				{
					const TArray<TSharedPtr<FJsonValue>>* Choices;
					if (JsonObject->TryGetArrayField("choices", Choices) && Choices->Num() > 0)
					{
						auto MsgObj = (*Choices)[0]->AsObject()->GetObjectField("message");
						FString Content = MsgObj->GetStringField("content");
						ParseAndApplyMCPResponse(Content);
					}
				}
			}
		});

	Request->ProcessRequest();
}

void SMinesweeperWidget::ParseAndApplyMCPResponse(const FString& Response)
{
	FString WidthStr, HeightStr, MinesStr;
	FParse::Value(*Response, TEXT("width="), WidthStr);
	FParse::Value(*Response, TEXT("height="), HeightStr);
	FParse::Value(*Response, TEXT("mines="), MinesStr);

	GridWidth = FCString::Atoi(*WidthStr);
	GridHeight = FCString::Atoi(*HeightStr);
	NumBombs = FCString::Atoi(*MinesStr);

	OnStartNewGame();
}