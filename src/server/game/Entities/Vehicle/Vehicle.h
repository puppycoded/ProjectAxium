#ifndef __AXIUM_VEHICLE_H
#define __AXIUM_VEHICLE_H

#include "ObjectDefines.h"
#include "VehicleDefines.h"

struct VehicleEntry;
class Unit;

class Vehicle
{
    public:
        explicit Vehicle(Unit* unit, VehicleEntry const* vehInfo, uint32 creatureEntry);
        virtual ~Vehicle();

        void Install();
        void Uninstall();
        void Reset(bool evading = false);
        void InstallAllAccessories(bool evading);
        void ApplyAllImmunities();
        void InstallAccessory(uint32 entry, int8 seatId, bool minion, uint8 type, uint32 summonTime);   //! May be called from scripts

        Unit* GetBase() const { return _me; }
        VehicleEntry const* GetVehicleInfo() const { return _vehicleInfo; }
        uint32 GetCreatureEntry() const { return _creatureEntry; }

        bool HasEmptySeat(int8 seatId) const;
        Unit* GetPassenger(int8 seatId) const;
        int8 GetNextEmptySeat(int8 seatId, bool next) const;
        uint8 GetAvailableSeatCount() const;

        bool AddPassenger(Unit* passenger, int8 seatId = -1);
        void EjectPassenger(Unit* passenger, Unit* controller);
        void RemovePassenger(Unit* passenger);
        void RelocatePassengers(float x, float y, float z, float ang);
        void RemoveAllPassengers();
        void Dismiss();
        bool IsVehicleInUse() { return Seats.begin() != Seats.end(); }

        SeatMap Seats;

        VehicleSeatEntry const* GetSeatForPassenger(Unit* passenger);

    private:
        SeatMap::iterator GetSeatIteratorForPassenger(Unit* passenger);
        void InitMovementInfoForBase();

        Unit* _me;
        VehicleEntry const* _vehicleInfo;
        uint32 _usableSeatNum;         // Number of seats that match VehicleSeatEntry::UsableByPlayer, used for proper display flags
        uint32 _creatureEntry;         // Can be different than me->GetBase()->GetEntry() in case of players
};
#endif
