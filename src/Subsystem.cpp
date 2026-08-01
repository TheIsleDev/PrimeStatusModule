
#include <TheIsle/ATIDinosaurBase.hpp>
#include <Subsystem.hpp>


const ConditionField StatusSubsystem::ConditionFields[] = {
	{&FEligiblePrimeElder::bPrimeCondition1, STR("Visit a Sanctuary")},
	{&FEligiblePrimeElder::bPrimeCondition2, STR("Get Nested In")},
	{&FEligiblePrimeElder::bPrimeCondition3, STR("Get Perfect Diet")},
	{&FEligiblePrimeElder::bPrimeCondition4, STR("Visit Mass Migration Zone")},
	{&FEligiblePrimeElder::bPrimeCondition5, STR("Visit 2 Migration Zones")},
	{&FEligiblePrimeElder::bPrimeCondition6, STR("Visit 4 Patrol Zones")},
	{&FEligiblePrimeElder::bPrimeCondition7, STR("Never be infertile")},
	{&FEligiblePrimeElder::bPrimeCondition8, STR("Never get muscle spasms")},
	{&FEligiblePrimeElder::bPrimeCondition9, STR("Raise children to subadult")},
	{&FEligiblePrimeElder::bPrimeCondition10, STR("Be a Hypsi, Troodon, Beipi, Dryo or Deino.")},
	{&FEligiblePrimeElder::bIsEligiblePrime, STR("Prime Eligibility")},
};

StatusSubsystem::StatusSubsystem() {
	// Мы хотим что бы эта хуйня тикала только когда игра тикает, и не на оборот...
	FireCallBackID = Hook::RegisterEngineTickPreCallback(
		[this](Hook::TCallbackIterationData<void>& info, UEngine* Context, float DeltaSeconds, bool bIdleMode) {
			Tick(DeltaSeconds, bIdleMode);
		}
		, {false, true, STR("StatusSystem"), STR("TickingStatus")}
	);
}

StatusSubsystem::~StatusSubsystem() {
	Hook::UnregisterCallback(FireCallBackID);
}


void StatusSubsystem::NotifyPrimeConditionDiff(ATIPlayerController* Player, const FEligiblePrimeElder& Old, const FEligiblePrimeElder& New) {
	int CompletedTasks{0};
	RC::StringType ChangeAnnounce;
	for (const ConditionField& Field : ConditionFields) {
		bool OldVal = Old.*Field.Member;
		bool NewVal = New.*Field.Member;
		if (NewVal) CompletedTasks++;
		if (OldVal == NewVal) continue;
		ChangeAnnounce += fmt::format(STR("[{}: {}]"), Field.Name, NewVal);
	}

	// Прикольная натификация слева у игрока
	RC::StringType MessageStr = fmt::format(STR("Prime Tasks [{}/5] {}"), CompletedTasks, ChangeAnnounce);
	Player->ClientShowNotification(FText(MessageStr));
}

void StatusSubsystem::Tick(float DeltaSeconds, bool bIdleMode) {
	// Нахуя тикать если ничего не изменилось?
	if (bIdleMode) return;

	static ATIGameModeBase* GameMode = static_cast<ATIGameModeBase*>(UObjectGlobals::FindFirstOf(STR("BP_SurvivalGameMode_C")));;

    CachedPrimeData NewCached;
	// Проходим только по активным игрокам, мз минусов если чел проебет таск пока вылетевший то не регнет
    TSet<ATIPlayerController*> ActivePlayers = GameMode->AllPlayerControllers();
    for (ATIPlayerController* Player : ActivePlayers) {
        APawn* Pawn = Player->Pawn();
        if (!Pawn || !Pawn->IsA(ATIDinosaurBase::StaticClass())) continue;

        FString SteamID = Player->SteamId();
        ATIDinosaurBase* Dino = static_cast<ATIDinosaurBase*>(Pawn);
        int32 DinoID = Dino->ID();
        FEligiblePrimeElder& NewData = Dino->EligiblePrimeElderData();
        FEligiblePrimeElderHolder* OldData = Cached.Find(SteamID);
        if (!OldData || OldData->DinoID != DinoID) {
            NewCached.Add(SteamID, {DinoID, NewData});
            continue;
        }

        NewCached.Add(SteamID, *OldData);
		// Сверяем байтики, так быстрее
        if (!std::memcmp(&OldData->Holder, &NewData, sizeof(FEligiblePrimeElder))) continue;

		// Байтики не совпали, оповещаем тогда
        NotifyPrimeConditionDiff(Player, OldData->Holder, NewData);
        NewCached.Add(SteamID, {DinoID, NewData});
    }
    Cached = NewCached;
}
