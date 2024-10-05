class AIntroductionActor : AActor
{
	UPROPERTY(DefaultComponent, RootComponent)
	USceneComponent SceneRoot;

	UPROPERTY(DefaultComponent, Attach = SceneRoot)
	UStaticMeshComponent Mesh;
	
	UPROPERTY(DefaultComponent, Attach = SceneRoot)
	UBillboardComponent Billboard;



	UPROPERTY()
	float CountdownDuration = 5.0;

	float CurrentTimer = 0.0;
	bool bIsCountdownActive = false;
	
	UFUNCTION(BlueprintOverride)
	void BeginPlay()
	{
		// CurrentTimer = CountdownDuration;
		// bIsCountdownActive = true;
	}

	UFUNCTION()
	void StartCountdown()
	{
		CurrentTimer = CountdownDuration;
		bIsCountdownActive = true;
	}

	UFUNCTION(BlueprintEvent)
	void FinishCountdown()
	{
		
	}


	UFUNCTION(BlueprintOverride)
	void Tick(float DeltaSeconds)
	{
		if (bIsCountdownActive)
		{
			CurrentTimer -= DeltaSeconds;

			if (CurrentTimer <= 0.0)
			{

				Print("Cooldown  was completed!");
				FinishCountdown();
				bIsCountdownActive = false;
			}
		}
	}
}