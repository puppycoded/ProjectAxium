#ifndef AXIUM_GAMEOBJECTAI_H
#define AXIUM_GAMEOBJECTAI_H

#include "Define.h"
#include <list>
#include "Object.h"
#include "GameObject.h"
#include "CreatureAI.h"

class GameObjectAI
{
    protected:
        GameObject* const go;
    public:
        explicit GameObjectAI(GameObject* g) : go(g) {}
        virtual ~GameObjectAI() {}

        virtual void UpdateAI(const uint32 /*diff*/) {}

        virtual void InitializeAI() { Reset(); }

        virtual void Reset() {};

        static int Permissible(const GameObject* go);

        virtual bool GossipHello(Player* /*player*/) {return false;}
        virtual bool GossipSelect(Player* /*player*/, uint32 /*sender*/, uint32 /*action*/) {return false;}
        virtual bool GossipSelectCode(Player* /*player*/, uint32 /*sender*/, uint32 /*action*/, const char* /*code*/) {return false;}
        virtual bool QuestAccept(Player* /*player*/, Quest const* /*quest*/) {return false;}
        virtual bool QuestReward(Player* /*player*/, Quest const* /*quest*/, uint32 /*opt*/) {return false;}
        virtual uint32 GetDialogStatus(Player* /*player*/) {return 100;}
        virtual void Destroyed(Player* /*player*/, uint32 /*eventId*/) {}
        virtual void SetData(uint32 /*id*/, uint32 /*value*/) {}
        virtual void OnGameEvent(bool /*start*/, uint16 /*eventId*/) {}
        virtual void OnStateChanged(uint32 /*state*/, Unit* /*unit*/) { }
};

class NullGameObjectAI : public GameObjectAI
{
    public:
        explicit NullGameObjectAI(GameObject* g);

        void UpdateAI(const uint32 /*diff*/) {}

        static int Permissible(const GameObject* /*go*/) { return PERMIT_BASE_IDLE; }
};
#endif
