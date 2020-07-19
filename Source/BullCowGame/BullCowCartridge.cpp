// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();

    //Fill list of words from file
    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordList/HiddenWordList.txt");
    FFileHelper::LoadFileToStringArrayWithPredicate(Isograms, *WordListPath, [](const FString& Word)
        {
            return Word.Len() > 4 && Word.Len() < 8 && IsIsogram(Word);
        });

    SetupGame();
}

void UBullCowCartridge::OnInput(const FString& Input) // When the player hits enter
{

    if (bGameOver)
    {
        ClearScreen();
        SetupGame();
    }
    else //Check players guess
    {
        ProcessGuess(Input);
    }
}

void UBullCowCartridge::SetupGame() 
{
    int32 RandomNumber = FMath::RandRange(0, Isograms.Num() - 1);
    HiddenWord = Isograms[RandomNumber];
    PlayerLives = HiddenWord.Len();
    bGameOver = false;
    
    //Welcome Player
    PrintLine(TEXT("Hi there! And welcome to Bulls and Cows!"));
    PrintLine(TEXT("Guess the %i letter word!"), HiddenWord.Len());
    PrintLine(TEXT("You have %i lives."), PlayerLives);
    PrintLine(TEXT("Type in your guess. \nPress enter to continue..."));

    PrintLine(TEXT("DEBUG ----> Hidden Word: %s"), *HiddenWord); //TODO: Remove
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("\nPress enter to play again."));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    //Transform Guess into Lowercase
    FString LowerGuess = Guess.ToLower();

    if (LowerGuess == HiddenWord)
    {
        PrintLine(TEXT("Correct! You Win!"));
        EndGame();
        return;
    }

    if (LowerGuess.Len() != HiddenWord.Len()) 
    {
        PrintLine(TEXT("The guess doesn't have the correct number of characters. Try again."));
        PrintLine(TEXT("The word is %i letters long."), HiddenWord.Len());
        return;
    }

    if (!IsIsogram(LowerGuess))
    {
        PrintLine(TEXT("The guess is not an isogram. Try again."));
        return;
    }

	--PlayerLives;
	PrintLine(TEXT("You lost a life!"));

    FBullCowCount Count = GetBullCows(LowerGuess);
    PrintLine(TEXT("Bulls: %i, Cows: %i"), Count.Bulls, Count.Cows);

	if (PlayerLives <= 0)
	{
		ClearScreen();
		PrintLine(TEXT("You have no lives left!"));
		PrintLine(TEXT("The Hidden Word was: &s"), *HiddenWord);
		EndGame();
		return;
	}

	PrintLine(TEXT("Guess Again. You have %i remaining lives."), PlayerLives);
}

bool UBullCowCartridge::IsIsogram(const FString& Guess)
{
    //Sort Characters and compare adjacent Characters
    TArray<TCHAR> GuessChars = Guess.GetCharArray();
    GuessChars.Sort();
    for (int32 Index = 0; Index < GuessChars.Num() - 1; Index++)
    {
        if (GuessChars[Index] == GuessChars[Index +1])
        {
            return false;
        }
    }
    return true;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const
{
    FBullCowCount Count;

    for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
    {
        if (Guess[GuessIndex] == HiddenWord[GuessIndex])
        {
            Count.Bulls++;
            continue;
        }
        for (int32 HiddenWordIndex = 0; HiddenWordIndex < HiddenWord.Len(); HiddenWordIndex++)
        {
            if (Guess[GuessIndex] == HiddenWord[HiddenWordIndex])
            {
                Count.Cows++;
                break;
            }
        }
    }
    return Count;
}