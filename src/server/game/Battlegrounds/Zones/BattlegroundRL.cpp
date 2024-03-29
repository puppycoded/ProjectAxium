#include "Battleground.h"
#include "BattlegroundRL.h"
#include "Language.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "WorldPacket.h"

BattlegroundRL::BattlegroundRL()
{
    m_BgObjects.resize(BG_RL_OBJECT_MAX);

    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    //we must set messageIds
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;
}

BattlegroundRL::~BattlegroundRL()
{

}

void BattlegroundRL::StartingEventCloseDoors()
{
    for (uint8 i = BG_RL_OBJECT_DOOR_1; i <= BG_RL_OBJECT_DOOR_2; ++i)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
}

void BattlegroundRL::StartingEventOpenDoors()
{
    for (uint8 i = BG_RL_OBJECT_DOOR_1; i <= BG_RL_OBJECT_DOOR_2; ++i)
        DoorOpen(i, true);

    for (uint8 i = BG_RL_OBJECT_BUFF_1; i <= BG_RL_OBJECT_BUFF_2; ++i)
        SpawnBGObject(i, 60);
}

void BattlegroundRL::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    UpdateArenaWorldState();
}

void BattlegroundRL::AddPlayerToScoreboard(Player* player, uint32 team)
{
    BattlegroundRLScore* sc = new BattlegroundRLScore;
    sc->PlayerTeam = team;
    m_PlayerScores[player->GetGUID()] = sc;
}

void BattlegroundRL::RemovePlayer(Player* /*player*/, uint64 /*guid*/, uint32 /*team*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattlegroundRL::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        sLog->outError("Killer player not found");
        return;
    }

    Battleground::HandleKillPlayer(player, killer);

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

bool BattlegroundRL::HandlePlayerUnderMap(Player* player)
{
    player->TeleportTo(GetMapId(), 1285.810547f, 1667.896851f, 39.957642f, player->GetOrientation(), false);
    return true;
}

void BattlegroundRL::HandleAreaTrigger(Player* Source, uint32 Trigger)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    //uint32 SpellId = 0;
    //uint64 buff_guid = 0;
    switch (Trigger)
    {
        case 4696:                                          // buff trigger?
        case 4697:                                          // buff trigger?
            break;
        default:
            sLog->outError("WARNING: Unhandled AreaTrigger in BattlegroundRL: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }

    //if (buff_guid)
    //    HandleTriggerBuff(buff_guid, Source);
}

void BattlegroundRL::FillInitialWorldStates(WorldPacket &data)
{
    data << uint32(0xbba) << uint32(1);           // 9
    UpdateArenaWorldState();
}

void BattlegroundRL::Reset()
{
    //call parent's reset
    Battleground::Reset();
}

bool BattlegroundRL::SetupBattleground()
{
    // Gates
    if (!AddObject(BG_RL_OBJECT_DOOR_1, BG_RL_OBJECT_TYPE_DOOR_1, 1293.561f, 1601.938f, 31.60557f, -1.457349f, 0, 0, -0.6658813f, 0.7460576f, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RL_OBJECT_DOOR_2, BG_RL_OBJECT_TYPE_DOOR_2, 1278.648f, 1730.557f, 31.60557f, 1.684245f, 0, 0, 0.7460582f, 0.6658807f, RESPAWN_IMMEDIATELY) ||
    // Buffs
        !AddObject(BG_RL_OBJECT_BUFF_1, BG_RL_OBJECT_TYPE_BUFF_1, 1328.719971f, 1632.719971f, 34.9f, -1.448624f, 0, 0, 0.6626201f, -0.7489557f, 120) ||
        !AddObject(BG_RL_OBJECT_BUFF_2, BG_RL_OBJECT_TYPE_BUFF_2, 1243.300049f, 1699.170044f, 34.9f, -0.06981307f, 0, 0, 0.03489945f, -0.9993908f, 120))
    {
        sLog->outErrorDb("BatteGroundRL: Failed to spawn some object!");
        return false;
    }

    // Ready Markers
    if (!AddObject(BG_RL_OBJECT_READY_MARKER_A, BG_OBJECTID_READY_MARKER, 1274.0f, 1734.34f, 31.6f, 1.64f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RL_OBJECT_READY_MARKER_B, BG_OBJECTID_READY_MARKER, 1298.2f, 1598.0f, 31.61f, 4.8f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY))
    {
        sLog->outErrorDb("BatteGroundRL: Failed to spawn Ready Markers Battleground not created!");
        return false;
    }

    return true;
}

/*
Packet S->C, id 600, SMSG_INIT_WORLD_STATES (706), len 86
0000: 3C 02 00 00 80 0F 00 00 00 00 00 00 09 00 BA 0B | <...............
0010: 00 00 01 00 00 00 B9 0B 00 00 02 00 00 00 B8 0B | ................
0020: 00 00 00 00 00 00 D8 08 00 00 00 00 00 00 D7 08 | ................
0030: 00 00 00 00 00 00 D6 08 00 00 00 00 00 00 D5 08 | ................
0040: 00 00 00 00 00 00 D3 08 00 00 00 00 00 00 D4 08 | ................
0050: 00 00 00 00 00 00                               | ......
*/
