/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "icecrown_citadel.h"
#include "Spell.h"

#define GOSSIP_SENDER_ICC_PORT 631

class icecrown_citadel_teleport : public GameObjectScript
{
    public:
        icecrown_citadel_teleport() : GameObjectScript("icecrown_citadel_teleport") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Light's Hammer.", GOSSIP_SENDER_ICC_PORT, LIGHT_S_HAMMER_TELEPORT);
            if (InstanceScript* instance = go->GetInstanceScript())
            {
                if (instance->GetBossState(DATA_LORD_MARROWGAR) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to the Oratory of the Damned.", GOSSIP_SENDER_ICC_PORT, ORATORY_OF_THE_DAMNED_TELEPORT);
                if (instance->GetBossState(DATA_LADY_DEATHWHISPER) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to the Rampart of Skulls.", GOSSIP_SENDER_ICC_PORT, RAMPART_OF_SKULLS_TELEPORT);
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to the Deathbringer's Rise.", GOSSIP_SENDER_ICC_PORT, DEATHBRINGER_S_RISE_TELEPORT);
                if (instance->GetData(DATA_COLDFLAME_JETS) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to the Upper Spire.", GOSSIP_SENDER_ICC_PORT, UPPER_SPIRE_TELEPORT);
                if (instance->GetBossState(DATA_VALITHRIA_DREAMWALKER) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Sindragosa's Lair", GOSSIP_SENDER_ICC_PORT, SINDRAGOSA_S_LAIR_TELEPORT);
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go->GetGOInfo()->GetGossipMenuId()), go->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 sender, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();
            SpellEntry const* spell = sSpellStore.LookupEntry(action);
            if (!spell)
                return false;

            if (player->isInCombat())
            {
                Spell::SendCastResult(player, spell, 0, SPELL_FAILED_AFFECTING_COMBAT);
                return true;
            }

            if (sender == GOSSIP_SENDER_ICC_PORT)
            {
                //Preload the Lich King's platform before teleporting player to there
                if (action == FROZEN_THRONE_TELEPORT)
                    player->GetMap()->LoadGrid(530.3f, -2122.67f);

                player->CastSpell(player, spell, true);

                //Give him 2 tries after teleport, just in case if player will fall through the ground
                if (action == FROZEN_THRONE_TELEPORT)
                    TeleportPlayerToFrozenThrone(player);
            }

            return true;
        }
};

class at_frozen_throne_teleport : public AreaTriggerScript
{
    public:
        at_frozen_throne_teleport() : AreaTriggerScript("at_frozen_throne_teleport") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (instance->GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE && instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && instance->GetBossState(DATA_SINDRAGOSA) == DONE)
                    player->CastSpell(player, FROZEN_THRONE_TELEPORT, true);

            return true;
        }
};

void AddSC_icecrown_citadel_teleport()
{
    new icecrown_citadel_teleport();
    new at_frozen_throne_teleport();
}
