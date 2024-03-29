/*
 * Scripts for spells with SPELLFAMILY_SHAMAN and SPELLFAMILY_GENERIC spells used by shaman players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_sha_".
 */

#include "ScriptPCH.h"
#include "SpellAuraEffects.h"

enum ShamanSpells
{
    SHAMAN_SPELL_GLYPH_OF_MANA_TIDE        = 55441,
    SHAMAN_SPELL_MANA_TIDE_TOTEM           = 39609,
    SHAMAN_SPELL_FIRE_NOVA_R1              = 1535,
    SHAMAN_SPELL_FIRE_NOVA_TRIGGERED_R1    = 8349,
    SHAMAN_SPELL_SATED                     = 57724,
    SHAMAN_SPELL_EXHAUSTION                = 57723,

    // For Earthen Power
    SHAMAN_TOTEM_SPELL_EARTHBIND_TOTEM     = 6474,
    SHAMAN_TOTEM_SPELL_EARTHEN_POWER       = 59566,
};

// 51474 - Astral shift
class spell_sha_astral_shift : public SpellScriptLoader
{
    public:
        spell_sha_astral_shift() : SpellScriptLoader("spell_sha_astral_shift") { }

        class spell_sha_astral_shift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_astral_shift_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                // reduces all damage taken while stun, fear or silence
                if (GetTarget()->GetUInt32Value(UNIT_FIELD_FLAGS) & (UNIT_FLAG_FLEEING | UNIT_FLAG_SILENCED) || (GetTarget()->GetUInt32Value(UNIT_FIELD_FLAGS) & (UNIT_FLAG_STUNNED) && GetTarget()->HasAuraWithMechanic(1<<MECHANIC_STUN)))
                    absorbAmount = CalculatePctN(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_astral_shift_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_astral_shift_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_astral_shift_AuraScript();
        }
};

// 1535 Fire Nova
class spell_sha_fire_nova : public SpellScriptLoader
{
    public:
        spell_sha_fire_nova() : SpellScriptLoader("spell_sha_fire_nova") { }

        class spell_sha_fire_nova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_fire_nova_SpellScript);

            bool Validate(SpellInfo const* spellEntry)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_FIRE_NOVA_R1))
                    return false;
                if (sSpellMgr->GetFirstSpellInChain(SHAMAN_SPELL_FIRE_NOVA_R1) != sSpellMgr->GetFirstSpellInChain(spellEntry->Id))
                    return false;

                uint8 rank = sSpellMgr->GetSpellRank(spellEntry->Id);
                if (!sSpellMgr->GetSpellWithRank(SHAMAN_SPELL_FIRE_NOVA_TRIGGERED_R1, rank, true))
                    return false;
                return true;
            }

            SpellCastResult CheckFireTotem()
            {
                // fire totem
                if (!GetCaster()->m_SummonSlot[1])
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_FIRE_TOTEM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint8 rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);
                if (uint32 spellId = sSpellMgr->GetSpellWithRank(SHAMAN_SPELL_FIRE_NOVA_TRIGGERED_R1, rank))
                {
                    Creature* totem = caster->GetMap()->GetCreature(caster->m_SummonSlot[1]);
                    if (totem && totem->isTotem())
                        caster->CastSpell(totem, spellId, true);
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_sha_fire_nova_SpellScript::CheckFireTotem);
                OnEffectHitTarget += SpellEffectFn(spell_sha_fire_nova_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_fire_nova_SpellScript();
        }
};

// 39610 Mana Tide Totem
class spell_sha_mana_tide_totem : public SpellScriptLoader
{
    public:
        spell_sha_mana_tide_totem() : SpellScriptLoader("spell_sha_mana_tide_totem") { }

        class spell_sha_mana_tide_totem_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_mana_tide_totem_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_GLYPH_OF_MANA_TIDE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_MANA_TIDE_TOTEM))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    if (unitTarget->getPowerType() == POWER_MANA)
                    {
                        int32 effValue = GetEffectValue();
                        // Glyph of Mana Tide
                        if (Unit* owner = caster->GetOwner())
                            if (AuraEffect* dummy = owner->GetAuraEffect(SHAMAN_SPELL_GLYPH_OF_MANA_TIDE, 0))
                                effValue += dummy->GetAmount();
                        // Regenerate 6% of Total Mana Every 3 secs
                        int32 effBasePoints0 = int32(CalculatePctN(unitTarget->GetMaxPower(POWER_MANA), effValue));
                        caster->CastCustomSpell(unitTarget, SHAMAN_SPELL_MANA_TIDE_TOTEM, &effBasePoints0, NULL, NULL, true, NULL, NULL, GetOriginalCaster()->GetGUID());
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_mana_tide_totem_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_mana_tide_totem_SpellScript();
        }
};

// 6474 - Earthbind Totem
class spell_sha_earthbind_totem : public SpellScriptLoader
{
    public:
        spell_sha_earthbind_totem() : SpellScriptLoader("spell_sha_earthbind_totem") { }

        class spell_sha_earthbind_totem_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthbind_totem_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_TOTEM_SPELL_EARTHBIND_TOTEM))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SHAMAN_TOTEM_SPELL_EARTHEN_POWER))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* aurEff)
            {
                if (!GetCaster())
                    return;
                if (Player* owner = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself())
                    if (AuraEffect* aur = owner->GetDummyAuraEffect(SPELLFAMILY_SHAMAN, 2289, 0))
                        if (roll_chance_i(aur->GetBaseAmount()))
                            GetTarget()->CastSpell((Unit*)NULL, SHAMAN_TOTEM_SPELL_EARTHEN_POWER, true);
            }

            void Register()
            {
                 OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_earthbind_totem_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_earthbind_totem_AuraScript();
        }
};

class EarthenPowerTargetSelector
{
    public:
        EarthenPowerTargetSelector() { }
 
        bool operator() (Unit* target)
        {
            if (!target->HasAuraWithMechanic(1 << MECHANIC_SNARE))
                return true;

            return false;
        }
};

class spell_sha_earthen_power : public SpellScriptLoader
{
    public:
        spell_sha_earthen_power() : SpellScriptLoader("spell_sha_earthen_power") { }

        class spell_sha_earthen_power_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earthen_power_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(EarthenPowerTargetSelector());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_earthen_power_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_earthen_power_SpellScript();
        }
};

class spell_sha_bloodlust : public SpellScriptLoader
{
    public:
        spell_sha_bloodlust() : SpellScriptLoader("spell_sha_bloodlust") { }

        class spell_sha_bloodlust_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_bloodlust_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_SATED))
                    return false;
                return true;
            }

            void RemoveInvalidTargets(std::list<Unit*>& targets)
            {
                targets.remove_if(Axium::UnitAuraCheck(true, SHAMAN_SPELL_SATED));
            }

            void ApplyDebuff()
            {
                GetHitUnit()->CastSpell(GetHitUnit(), SHAMAN_SPELL_SATED, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_2, TARGET_UNIT_CASTER_AREA_RAID);
                AfterHit += SpellHitFn(spell_sha_bloodlust_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_bloodlust_SpellScript();
        }
};

class spell_sha_heroism : public SpellScriptLoader
{
    public:
        spell_sha_heroism() : SpellScriptLoader("spell_sha_heroism") { }

        class spell_sha_heroism_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_heroism_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_EXHAUSTION))
                    return false;
                return true;
            }

            void RemoveInvalidTargets(std::list<Unit*>& targets)
            {
                targets.remove_if(Axium::UnitAuraCheck(true, SHAMAN_SPELL_EXHAUSTION));
            }

            void ApplyDebuff()
            {
                GetHitUnit()->CastSpell(GetHitUnit(), SHAMAN_SPELL_EXHAUSTION, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_heroism_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_heroism_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_heroism_SpellScript::RemoveInvalidTargets, EFFECT_2, TARGET_UNIT_CASTER_AREA_RAID);
                AfterHit += SpellHitFn(spell_sha_heroism_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_heroism_SpellScript();
        }
};

void AddSC_shaman_spell_scripts()
{
    new spell_sha_astral_shift();
    new spell_sha_fire_nova();
    new spell_sha_mana_tide_totem();
    new spell_sha_earthbind_totem();
    new spell_sha_earthen_power();
    new spell_sha_bloodlust();
    new spell_sha_heroism();
}
