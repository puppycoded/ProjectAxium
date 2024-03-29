#ifndef AXIUM_TOTEMAI_H
#define AXIUM_TOTEMAI_H

#include "CreatureAI.h"
#include "Timer.h"

class Creature;
class Totem;

class TotemAI : public CreatureAI
{
    public:

        explicit TotemAI(Creature* c);

        void MoveInLineOfSight(Unit*);
        void AttackStart(Unit*);
        void EnterEvadeMode();

        void UpdateAI(const uint32);
        static int Permissible(const Creature*);

    private:
        uint64 i_victimGuid;
};
#endif

