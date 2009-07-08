/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://www.dev-osrose.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    depeloped with Main erose/hrose source server + some change from the original eich source
*/
#include "character.h"
#include "worldserver.h"

void CCharacter::DoAttack( )
{
     /*
     if(IsSummon())
       Log(MSG_INFO,"Someone attacks type=%i,skillid=%i",Battle->atktype,Battle->skillid);
     */

    //osptest begin
    if(IsSummon())
    {
        CCharacter* Enemy = GetCharTarget( );
        if(Enemy == NULL && Battle->atktype != SKILL_AOE && Battle->atktype != BUFF_AOE)
        {
            //LogDebug("No Enemy found");
            ClearBattle( Battle );
            return;
        }
        //LogDebug("Client id: %i Enemy client id: %i",clientid, Enemy->clientid);
        if(this == Enemy) //summoned monster is attacking itself. It has been observed to happen
        {
            //Log(MSG_INFO,"Clearing Battle for this summon");
            ClearBattle( Battle );
            return;
        }
    }
    //osptest end

    //LMA: If we're dead, why are we still fighting?
    if(IsDead())
    {
        ClearBattle(Battle);
        return;
    }

    CMap* map = GServer->MapList.Index[Position->Map];
    switch(Battle->atktype)
    {
        case NORMAL_ATTACK://normal attack
        {
            CCharacter* Enemy = GetCharTarget( );
            if(Enemy==NULL)
            {
                ClearBattle( Battle );
                return;
            }

            //osptest
            if(Enemy == this)
            {
                //Log(MSG_INFO,"WTF?? I AM trying to attack myself");
                ClearBattle( Battle );
            }
            //osptest end

            //LMA: Some logs.
            /*
            if(map->id==8)
            {
                bool is_ok=false;
                bool is_reached=false;
                bool can_attack=false;

                float distance = GServer->distance( Position->current, Enemy->Position->current );
                is_reached=IsTargetReached( Enemy );
                can_attack=CanAttack( );

                if(is_reached&&can_attack)
                    is_ok=true;

                if(IsMonster())
                {
                    Log(MSG_INFO,"monster has reached? %i (%.2f/%.2f), canattack? %i, will attack? %i",is_reached,distance,Stats->Attack_Distance,can_attack,is_ok);

                    if (is_ok)
                        Log(MSG_INFO,"NORMAL_ATTACK, monster %i has reached %i (%.2f<=%.2f)",clientid,Enemy->clientid,distance,Stats->Attack_Distance);
                    else
                        Log(MSG_INFO,"NORMAL_ATTACK, monster %i hasn't reached? %i (%.2f>%.2f)",clientid,Enemy->clientid,distance,Stats->Attack_Distance);
                }
                else
                {
                    Log(MSG_INFO,"player has reached? %i (%.2f/%.2f), canattack? %i, will attack? %i",is_reached,distance,Stats->Attack_Distance,can_attack,is_ok);

                    if (is_ok)
                        Log(MSG_INFO,"NORMAL_ATTACK, player %i has reached %i (%.2f<=%.2f)",clientid,Enemy->clientid,distance,Stats->Attack_Distance);
                    else
                        Log(MSG_INFO,"NORMAL_ATTACK, player %i hasn't reached? %i (%.2f>%.2f)",clientid,Enemy->clientid,distance,Stats->Attack_Distance);
                }

            }
            */
            //End of logs.


            if(IsTargetReached( Enemy ) && CanAttack( ))
            {
                NormalAttack( Enemy );

                if (Enemy->IsMonster()) // do monster AI script when monster is attacked.
                {
                    CMonster* monster = GServer->GetMonsterByID(Enemy->clientid, Enemy->Position->Map);

                    if(monster!=NULL)
                        monster->DoAi(monster->thisnpc->AI, 3);
                }

            }

        }
        break;
        case SKILL_ATTACK://skill attack
        {
            CCharacter* Enemy = GetCharTarget( );
            if(Enemy==NULL)
            {
                ClearBattle( Battle );
                return;
            }
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill==NULL)
            {
                ClearBattle( Battle );
                return;
            }
            if(IsTargetReached( Enemy, skill ) && CanAttack( ))
            {
                SkillAttack( Enemy, skill );

                if (Enemy->IsMonster())
                {
                    CMonster* monster = GServer->GetMonsterByID(Enemy->clientid, Enemy->Position->Map);

                    if(monster!=NULL)
                        monster->DoAi(monster->thisnpc->AI, 3);
                }

            }

        }
        break;
        case MONSTER_SKILL_ATTACK://LMA: monster skill attack
        {
            CCharacter* Enemy = GetCharTarget( );
            if(Enemy==NULL)
            {
                ClearBattle( Battle );
                return;
            }
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill==NULL)
            {
                ClearBattle( Battle );
                return;
            }
            if(IsTargetReached( Enemy, skill ) && CanAttack( ))
                SkillAttack( Enemy, skill );
        }
        break;
        case SUMMON_BUFF://Summon buffs (support)
        {
            //LMA: For summon that buff player :)
            if (Battle->bufftarget==0)
               break;
            CCharacter* master = GetCharBuffTarget( );
            if(master==NULL)
            {
                ClearBattle( Battle );
                return;
            }
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill==NULL)
            {
                ClearBattle( Battle );
                return;
            }
            if(IsTargetReached( master, skill ) && CanAttack( ))
                SummonBuffSkill( master, skill );
        }
        break;
        case MONSTER_SKILL_BUFF://LMA: monster buffs
        {
            CCharacter* Enemy= GetCharTarget( );
            if(Enemy==NULL)
            {
                ClearBattle( Battle );
                return;
            }
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill==NULL)
            {
                ClearBattle( Battle );
                return;
            }

            if(IsTargetReached( Enemy, skill ) && CanAttack( ))
            {
                BuffSkill( Enemy, skill );
            }

        }
        break;
       case SKILL_BUFF://buffs
        {
            /*
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill==NULL)
            {
                ClearBattle( Battle );
                return;
            } // debuff or buff? [netwolf]
            switch (skill->target)
             {
                    case 5:
                    {
                     CCharacter* Enemy = GetCharTarget( );
                     if(Enemy==NULL)
                     {
                                     ClearBattle( Battle );
                                     return;
                     }
                     if(IsTargetReached( Enemy, skill ) && CanAttack( ))
                     {
                        //Log(MSG_INFO,"%i:: DeBuff time for %i",clientid,Enemy->clientid);
                        DebuffSkill( Enemy, skill );
                     }
                    }break;
                    default:
                    {
                            CCharacter* Target = GetCharTarget( );
                            if(Target==NULL)
                            {
                                     ClearBattle( Battle );
                                     return;
                            }
                            if(IsTargetReached( Target, skill ) && CanAttack( ))
                            {
                               //Log(MSG_INFO,"%i:: Buff time for %i",clientid,Target->clientid);
                               BuffSkill( Target, skill );
                            }
                    }break;
            }
            */
            //osptest
            CCharacter* Enemy = GetCharTarget( );
            if(Enemy == NULL)
            {
                //LogDebug("this target is NULL");
                ClearBattle( Battle );
                return;
            }

            CSkills* skill = GServer->GetSkillByID( Battle->skillid );

            if(skill == NULL)
            {
                //LogDebug("this skill is NULL");
                //ClearBattle( Battle );
                return;
            }

            //LogDebug("Called skill buff code for skill %i",skill->id);
            if(IsTargetReached( Enemy, skill ) && CanAttack( ))
            {
                //LogDebug("Skill successfully cast");
                //BuffSkill( Enemy, skill );

                if(IsPlayer())
                {
                    CPlayer* plkiller=(CPlayer*) this;
                    Log(MSG_WARNING,"Before Buff skill Player %i (%s) targets cid %i",clientid,plkiller->CharInfo->charname,plkiller->Battle->target);
                }

                BuffSkill( Enemy, skill );

                if(IsPlayer())
                {
                    CPlayer* plkiller=(CPlayer*) this;
                    Log(MSG_WARNING,"After Buff skill Player %i (%s) targets cid %i",clientid,plkiller->CharInfo->charname,plkiller->Battle->target);
                }

            }
            //osptest end
        }
        break;  //LMA: break was missing?
        case SKILL_AOE:
        case SKILL_SELF:
        case AOE_TARGET:
        {
            CCharacter* Enemy = NULL;
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill==NULL)
            {
                ClearBattle( Battle );
                return;
            }
            if(Battle->atktype==AOE_TARGET)
            {
                //LMA 2008/09/02: new version, the target is a zone, not a monster... so we stick with aoedestiny ;)
                float distance=GServer->distance( Position->current, Position->aoedestiny );
                //Log(MSG_INFO,"distance %f / range: %i, current: %.2f,%.2f, aoe: %.2f,%.2f",distance,skill->range,Position->current.x,Position->current.y,Position->aoedestiny.x,Position->aoedestiny.y);
                //osprose: canattacl
                if(distance<=skill->range&&CanAttack( ))
                {
                    Log(MSG_INFO,"[DoAttack] In AOE_TARGET time for AoeSkill");
                    AoeSkill( skill, NULL );    //LMA: no specific Enemy in Zone.
                }
                else
                {
                    //LMA: Too verbose...
                    //Log(MSG_INFO,"[DoAttack] In AOE_TARGET not reached or can't attack");
                }

            }
            else
            {
                if(Enemy==NULL)
                {
                   Log(MSG_INFO,"player %i: default AOE attack (%i/%i) %i for enemy NULL ", clientid,Battle->atktype,AOE_TARGET,skill->id);
                }
                else
                {
                   Log(MSG_INFO,"player %i: default AOE attack (%i/%i) %i for enemy %i ", clientid,Battle->atktype,AOE_TARGET,skill->id,Enemy->clientid);
                }

                AoeSkill( skill, Enemy );
            }
        }
        break;
        case BUFF_SELF:
        {
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill==NULL)
            {
                //osprose
                //ClearBattle( Battle );
                return;
            }
            BuffSkill( this, skill );
        }
        break;
        case MONSTER_BUFF_SELF:    //LMA: monster selfs buffs.
        {
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill==NULL)
            {
                ClearBattle( Battle );
                return;
            }
            BuffSkill( this, skill );
        }
        break;
       case BUFF_AOE:
        {
            CSkills* skill = GServer->GetSkillByID( Battle->skillid );
            if(skill==NULL)
            {
                ClearBattle( Battle );
                return;
            }

            /*
            //[netwolf]
            switch (skill->target)
             {
                    case 5:
                    {
                         CCharacter* Enemy = NULL;
                         AoeDebuff( skill, Enemy );
                    } break;
                    default:
                    {
                            AoeBuff( skill );
                    }   break;
             }
             */
             //osprose
             AoeBuff( skill );
             //osprose end
        }
        break;
        default:
        {
            return;
        }
        break;
    }

}

// do normal attack
void CCharacter::NormalAttack( CCharacter* Enemy )
{
    //LMA: Sometimes it's fired several times, no need to kill several times ;)
    bool is_already_dead=Enemy->IsDead();

    Position->destiny = Position->current;
    reduceItemsLifeSpan( false );
    Enemy->OnBeAttacked( this );
    float attack = (float)Stats->Attack_Power - ((Enemy->Stats->Magic_Defense+Enemy->Stats->Defense )/2);
    if(attack<0) attack = 5;
    attack *= 0.65;
    float d_attack = attack / 100;
    if(attack<0) attack = 5;
    attack += (float)GServer->RandNumber( 0, 7 ) - 2;
    attack += ((Stats->Level - Enemy->Stats->Level) * d_attack);
    if(attack<7) attack = 5;
    long int hitpower = (long int)floor(attack + GServer->RandNumber(0, 4));
    /*if(IsPlayer( ))
        hitpower = (long int)floor(attack * 1.2 );*/
    if(IsPlayer( )) //temp fix to find balance btw monster and player
        hitpower = (long int)floor(attack * (GServer->Config.PlayerDmg/100.00));
    if(IsMonster( )) //temp fix to find balance btw monster and player
        hitpower = (long int)floor(attack * (GServer->Config.MonsterDmg/100.00));

    //TEST
    long int hitsave=hitpower;
    hitpower+=Stats->ExtraDamage_add;
    hitpower+=((hitpower*(Stats->ExtraDamage))/100);  //LMA: ED, Devilking / Arnold
    //Log(MSG_INFO,"ED: before %i, after %i (extra *%i + %i)",hitsave,hitpower,Stats->ExtraDamage,Stats->ExtraDamage_add);

    bool critical = false;
    if(hitpower<=0)
    {
        hitpower = 0;
    }
    else
    {
        if(GServer->RandNumber(0,300)<Stats->Critical)
        {
            hitpower = (long int)floor(hitpower*1.5);
            critical = true;
        }
    }
    // dodge
    unsigned int hitvalue = (unsigned int)floor(Stats->Accury * 50 / Enemy->Stats->Dodge);
    if(hitvalue>100) hitvalue = 100;
    if(GServer->RandNumber( 0, 100 )>hitvalue)
        hitpower = 0; // dodged
    if(!Enemy->IsSummon( ) && Enemy->IsMonster( ))
    {
        Enemy->AddDamage( this, (long long) hitpower );
        Enemy->damagecounter += (long long) hitpower;// is for AI
    }

    Enemy->Stats->HP -=  (long long) hitpower;

    //LMA: logs.
    if(Position->Map==8||Position->Map==40||Position->Map==9)
    {
        if(Enemy->IsPlayer())
        {
            //Log(MSG_INFO,"Player is hit %li, HP goes to %I64i",hitpower,Enemy->Stats->HP);
        }

    }
    //End of logs.

    /*
    if (Enemy->IsMonster())
        Log(MSG_INFO,"Normal Attack, monster HP %I64i, hitpower %li",Enemy->Stats->HP,hitpower);
    else
        Log(MSG_INFO,"Normal Attack, Player HP %I64i, hitpower %li",Enemy->Stats->HP,hitpower);
    */

    // actually the target was hit, if it was sleeping, set duration of
    // sleep to 0. map process will remove sleep then at next player-update
    if (Enemy->Status->Sleep != 0xff) {
        Enemy->MagicStatus[Enemy->Status->Sleep].Duration = 0;
    }

    //if (IsPlayer())
    //printf("Target suffered %i physical damage, %i HP still remain \n", hitpower, Enemy->Stats->HP);
    //else printf("Mob did %i physical damage, %i HP still remain \n", hitpower, Enemy->Stats->HP);
    Enemy->reduceItemsLifeSpan(true);
    BEGINPACKET( pak, 0x799 );
    ADDWORD    ( pak, clientid );
    ADDWORD    ( pak, Battle->atktarget );

    //LMA: little test (see TESTDEATH)
    //ADDDWORD   ( pak, hitpower );

    if(Enemy->IsDead())
    {
        //LMA: UW handling, we add the handling for other quests as well...
        //if (!is_already_dead&&(GServer->MapList.Index[Position->Map]->QSDkilling>0||GServer->MapList.Index[Position->Map]->QSDDeath>0)&&IsPlayer()&&Enemy->IsPlayer())
        if (!is_already_dead&&((GServer->MapList.Index[Position->Map]->QSDkilling>0&&IsPlayer()&&Enemy->IsPlayer())||(GServer->MapList.Index[Position->Map]->QSDDeath&&Enemy->IsPlayer())))
        {
            //Log(MSG_INFO,"UWKILL begin normal atk");
            UWKill(Enemy);
        }

        //LMA: TESTDEATH :: We try to force the DEATH
        ADDDWORD   ( pak, Enemy->Stats->MaxHP );

        //Logs.
        /*if(IsPlayer()&&Enemy->IsPlayer())
        {
            CPlayer* plkilled=(CPlayer*) Enemy;
            CPlayer* plkiller=(CPlayer*) this;
            Log(MSG_INFO,"NA CID %i (%s) killed %i (%s) (NORMAL_ATTACK).",clientid,plkiller->CharInfo->charname,Enemy->clientid,plkilled->CharInfo->charname);
        }
        else
        {
            Log(MSG_INFO,"NA CID %i killed %i (NORMAL_ATTACK).",clientid,Enemy->clientid);
        }*/

        CDrop* thisdrop = NULL;
        //Log(MSG_WARNING,"Dead, critical %i",critical);

        //LMA: Seems that 24 is for critical and skill? We disable critical final hit for now.
        //ADDDWORD   ( pak, critical?28:16 );
        ADDDWORD   ( pak, 16 );
        if(!Enemy->IsSummon( ) && !Enemy->IsPlayer( ))
        {
            thisdrop = Enemy->GetDrop( );
            if(thisdrop!=NULL)
            {
                //LMA: unused anymore?
                /*ADDFLOAT   ( pak, thisdrop->pos.x*100 );
                ADDFLOAT   ( pak, thisdrop->pos.y*100 );
                if(thisdrop->type==1)
                {
                    ADDDWORD( pak, 0xccccccdf );
                    ADDDWORD( pak, thisdrop->amount );
                    ADDDWORD( pak, 0xcccccccc );
                    ADDWORD ( pak, 0xcccc );
                }
                else
                {
                    ADDDWORD   ( pak, GServer->BuildItemHead( thisdrop->item ) );
                    ADDDWORD   ( pak, GServer->BuildItemData( thisdrop->item ) );
                    ADDDWORD( pak, 0x00000000 );
                    ADDWORD ( pak, 0x0000 );
                }
                ADDWORD    ( pak, thisdrop->clientid );
                ADDWORD    ( pak, thisdrop->owner );*/

                CMap* map = GServer->MapList.Index[thisdrop->posMap];
                map->AddDrop( thisdrop );
            }
        }

        //GServer->SendToVisible( &pak, Enemy, thisdrop );
        GServer->SendToVisible( &pak, Enemy );

        //LMA: test
        //OnEnemyDie( Enemy );
        ClearBattle(Battle);
        if(!is_already_dead)
            TakeExp(Enemy);
        //end of test.
        //Log(MSG_WARNING,"End of dead packet",);


        //LMA: test packet...
        //0x820 - 01 00 00 00
        if(Enemy->IsPlayer())
        {
            CPlayer* plkilled=(CPlayer*) Enemy;
            if(plkilled!=NULL)
            {
                BEGINPACKET( pak, 0x820 );
                ADDWORD    ( pak, 1 );
                ADDWORD    ( pak, 0 );
                plkilled->client->SendPacket(&pak);
                //Log(MSG_WARNING,"NA Packet 820 sent to %s",plkilled->CharInfo->charname);
            }

        }
        else if (Enemy->IsMonster())
        {
            //LMA let's send a suicide packet so monster should be killed twice.
            CMonster* thisMonster = GServer->GetMonsterByID(Enemy->clientid, Enemy->Position->Map);
            BEGINPACKET( pak, 0x799 );
            ADDWORD    ( pak, Enemy->clientid );
            ADDWORD    ( pak, Enemy->clientid );
            ADDDWORD   ( pak, thisMonster->thisnpc->hp*thisMonster->thisnpc->level );
            ADDDWORD   ( pak, 16 );
            GServer->SendToVisible( &pak, Enemy );
        }

        //LMA: We send two other packets, just to be sure...
        /*
        if(Enemy->IsPlayer())
        {
            for (int k=0;k<1;k++)
            {
                BEGINPACKET( pak, 0x799 );
                ADDWORD    ( pak, clientid );
                ADDWORD    ( pak, Battle->atktarget );
                ADDDWORD   ( pak, Enemy->Stats->MaxHP*2);
                ADDDWORD   ( pak, 16 );
                GServer->SendToVisible( &pak, Enemy);
                Log(MSG_WARNING,"Special death Packet, sent");
            }

        }
        */

    }
    else
    {
        //LMA: TESTDEATH
        ADDDWORD   ( pak, hitpower );

        //Logs.
        /*if(IsPlayer()&&Enemy->IsPlayer())
        {
            CPlayer* plkilled=(CPlayer*) Enemy;
            CPlayer* plkiller=(CPlayer*) this;
            Log(MSG_WARNING,"NA hitpower %i, critical %i, %i (%s) hitting %i (%s)",hitpower,critical,clientid,plkiller->CharInfo->charname,Enemy->clientid,plkilled->CharInfo->charname);
            Log(MSG_WARNING,"NA Player %i (%s) targets cid %i",clientid,plkiller->CharInfo->charname,plkiller->Battle->target);
        }
        else
        {
            Log(MSG_WARNING,"NA, Not dead, hitpower %i, critical %i (%i hitting %i)",hitpower,critical,clientid,Enemy->clientid);
        }*/

        ADDDWORD   ( pak, (hitpower>0?(critical?12:0):0) );
        GServer->SendToVisible( &pak, Enemy );
    }

    Battle->lastAtkTime = clock( );
    ReduceABC( );

    //LMA: We take fuel.
    if(IsPlayer()&&Status->Stance==DRIVING)
    {
        CPlayer* plkiller=(CPlayer*) this;

        if(plkiller->items[136].itemnum==0||plkiller->items[139].itemnum==0)
        {
            Log(MSG_WARNING,"%s should be riding but doesn't have a motor %i or weapon %i (normal_attack)?",plkiller->CharInfo->charname,plkiller->items[136].itemnum,plkiller->items[139].itemnum);
            return;
        }

        //We take the fuel amount.
        //Log(MSG_INFO,"lifespan before %i, sp_value %i",plkiller->items[136].lifespan,plkiller->items[136].sp_value);
        plkiller->items[136].sp_value-=(int)plkiller->attack_fuel;
        if(plkiller->items[136].sp_value<0)
        {
            plkiller->items[136].sp_value=0;
        }

        plkiller->items[136].lifespan=(int)(plkiller->items[136].sp_value/10);

        if(plkiller->items[136].lifespan<0)
        {
            plkiller->items[136].sp_value=0;
        }

        //Log(MSG_INFO,"lifespan after %i, sp_value %i, attack fuel %.2f",plkiller->items[136].lifespan,plkiller->items[136].sp_value,plkiller->attack_fuel);

        BEGINPACKET( pak,0x7ce );
        ADDWORD    ( pak, 0x88 ); //Slot
        ADDWORD    ( pak, plkiller->items[136].sp_value );
        plkiller->client->SendPacket( &pak );

        //We stop attack if needed
        if(plkiller->items[136].lifespan==0)
        {
            BEGINPACKET( pak, 0x796 );
            ADDWORD    ( pak, plkiller->clientid );
            ADDFLOAT   ( pak, plkiller->Position->current.x*100 );
            ADDFLOAT   ( pak, plkiller->Position->current.y*100 );
            ADDWORD    ( pak, 0x0000 );
            GServer->SendToVisible( &pak, plkiller );

            //clear battle.
            ClearBattle(Battle);
            //Log(MSG_INFO,"No more fuel to attack");
        }

    }


    return;
}

// do skill attack
bool CCharacter::SkillAttack( CCharacter* Enemy, CSkills* skill )
{
    Position->destiny = Position->current;
    if(Battle->castTime==0)
    {
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, this );
        Battle->castTime = clock();
        return true;
    }
    else
    {
        clock_t etime = clock() - Battle->castTime;
        if(etime<SKILL_DELAY)
            return true;
    }
    Battle->castTime = 0;
    UseAtkSkill( Enemy, skill );
    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP<0) Stats->MP=0;
    if(Battle->contatk)
    {
        Battle->atktype = NORMAL_ATTACK;
        Battle->skilltarget = 0;
        Battle->atktarget = Battle->target;
        Battle->skillid = 0;
    }
    else
    {
        //osprose
        //ClearBattle( Battle );
    }
    GServer->DoSkillScript( this, skill );       //So far only used for summons
    Battle->lastAtkTime = clock( );
    return true;
}

// do buff skill
bool CCharacter::BuffSkill( CCharacter* Target, CSkills* skill )
{
    Position->destiny = Position->current;
    if(Battle->castTime==0)
    {
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, (CCharacter*)this );
        Battle->castTime = clock();
        //Log(MSG_INFO,"7bb battle casttime==0");
        return true;
    }
    else
    {
        clock_t etime = clock() - Battle->castTime;
        if(etime<SKILL_DELAY)
        {
            //Log(MSG_INFO,"etime<SKILL_DELAY: %i<%i",etime,SKILL_DELAY);
            return true;
        }

    }

    //Log(MSG_INFO,"new stat MP %i",Stats->MP);

    //Log(MSG_INFO,"%i Really doing skill %i to %i",clientid,skill->id,Target->clientid);

    Battle->castTime = 0;
    UseBuffSkill( Target, skill );
    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP<0)
    {
        Stats->MP=0;
    }

    /*
    ClearBattle( Battle );
    GServer->DoSkillScript( this, skill );
    */

    //osprose
    GServer->DoSkillScript( this, skill );
    if(!IsMonster())
    {
        //ClearBattle( Battle ); // clear battle for players when they use buff skills
        Battle->bufftarget = 0;
        Battle->skilltarget = 0;
        Battle->skillid = 0;
        //Battle->atktype = NORMAL_ATTACK;
        Battle->atktype = 0;
    }
    else //Monsters need to be reset to normal attack and clear skill attacks.
    {
        Battle->bufftarget = 0;
        Battle->skilltarget = 0;
        Battle->skillid = 0;
        Battle->atktype = NORMAL_ATTACK;
    }
    //osprose end

    Battle->lastAtkTime = clock( );
    return true;
}

// do Debuff skill [netwolf]
bool CCharacter::DebuffSkill( CCharacter* Enemy, CSkills* skill )
{
    Position->destiny = Position->current;
    if(Battle->castTime==0)
    {
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, (CCharacter*)this );
        Battle->castTime = clock();
        return true;
    }
    else
    {
        clock_t etime = clock() - Battle->castTime;
        if(etime<SKILL_DELAY)
            return true;
    }
    Battle->castTime = 0;
    UseDebuffSkill( Enemy, skill );
    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP<0) Stats->MP=0;
    ClearBattle( Battle );
    GServer->DoSkillScript( this, skill );
    Battle->lastAtkTime = clock( );
    return true;
}

//LMA: A summon does a buff skill
//2do: check if master is debuffed.
bool CCharacter::SummonBuffSkill( CCharacter* Target, CSkills* skill )
{
    Position->destiny = Position->current;
    if(Battle->castTime==0)
    {
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, (CCharacter*)this );
        Battle->castTime = clock();
        //Log(MSG_INFO,"0x7bb");
        return true;
    }
    else
    {
        clock_t etime = clock() - Battle->castTime;
        if(etime<SKILL_DELAY)
            return true;
    }

    //LMA: For now, it seems the summon buff only one time :(
    //Log(MSG_INFO,"Real buff skill");
    Battle->castTime = 0;
    UseBuffSkill( Target, skill );
    ClearBattle( Battle );
    GServer->DoSkillScript( this, skill );
    Battle->lastAtkTime = clock( );
    return true;
}

// do AoE skill
bool CCharacter::AoeSkill( CSkills* skill, CCharacter* Enemy )
{

    //LMA: not here since AOE can have NULL Enemy, will be catched later by the call to the skill.
    //bool is_already_dead=Enemy->IsDead();
    Log(MSG_INFO,"In AOE Skill");
    Position->destiny = Position->current;
    //Log(MSG_INFO,"Position in AOE: %.2f,%.2f",Position->current.x,Position->current.y);

    //LMA: handling case of AOE_SKILLS and AOE_TARGET (the target point is not the same).
    fPoint goodtarget;
    goodtarget=Position->current;
    if (Battle->atktype==AOE_TARGET)
    {
       goodtarget=Position->aoedestiny;
    }

    if(Battle->castTime==0)
    {
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, (CCharacter*)this );
        Battle->castTime = clock();
        return true;
    }
    else
    {
        //LMA: No delay for AOE_TARGET skill (zone)
        if (Battle->atktype!=AOE_TARGET)
        {
            clock_t etime = clock() - Battle->castTime;
            if(etime<SKILL_DELAY)
                return true;
        }

    }

    Battle->castTime = 0;
    CMap* map = GServer->MapList.Index[Position->Map];
    /*
    for(UINT i=0;i<map->MonsterList.size();i++)
    {
        CMonster* monster = map->MonsterList.at(i);
        if(monster->clientid==clientid) continue;
        if(IsSummon( ) || IsPlayer( ))
        {
            if(monster->IsSummon( ) && (map->allowpvp==0 || monster->owner==clientid)) continue;
        }
        else
        {
            if(!monster->IsSummon( )) continue;
        }
        if(GServer->IsMonInCircle( goodtarget,monster->Position->current,(float)skill->aoeradius+1))
        {
            Log(MSG_INFO,"AOE Attack (1) monster %i radius %.2f",monster->montype,(float)skill->aoeradius+1);
            UseAtkSkill( (CCharacter*) monster, skill );
        }

    }

    if(map->allowpvp!=0 || (IsMonster( ) && !IsSummon( )))
    {
        for(UINT i=0;i<map->PlayerList.size();i++)
        {
            CPlayer* player = map->PlayerList.at(i);
            if(player->clientid==clientid) continue;
            if(GServer->IsMonInCircle( goodtarget,player->Position->current,(float)skill->aoeradius+1))
            {
                Log(MSG_INFO,"AOE Attack (2) player %s",player->CharInfo->charname);
                UseAtkSkill( (CCharacter*) player, skill );
            }

        }
    }

    */

    //osprose
    //TODO: pvp ?
    UINT save_atktype=Battle->atktype;

    if(IsPlayer() || IsSummon())
    {
        //monsters.
        for(UINT i=0;i<map->MonsterList.size();i++)
        {
            CMonster* monster = map->MonsterList.at(i);
            if(monster->clientid == clientid) continue;
            if(monster->IsSummon( ) && (map->allowpvp == 0 || monster->owner == clientid)) continue;
            if(GServer->IsMonInCircle( goodtarget,monster->Position->current,(float)skill->aoeradius+1))
            {
                Log(MSG_INFO,"AOE Attack (1) player attacks monster %i (clientID %u) radius %.2f",monster->montype,monster->clientid,(float)skill->aoeradius+1);

                //LMA: we have to since AOE doesn't have a specific target and atkskil resets some values.
                Battle->skilltarget=monster->clientid;
                Battle->skillid=skill->id;
                Battle->atktype = save_atktype;
                UseAtkSkill( (CCharacter*) monster, skill );

                //LMA: TEST is there a "buff/debuff" skill with it too?
                /*
                if (skill->status[0]!=0&&skill->buff[0]!=0&&monster->Stats->HP>0)
                {
                    UseBuffSkill( (CCharacter*)monster, skill );
                    Log(MSG_INFO,"AOE Attack (1) player buffs monster %i",monster->montype);
                }
                */

                //LMA: AIP time.
                monster->DoAi(monster->thisnpc->AI, 3);
                //END OF TEST.

            }

        }

        //Pvp, group Vs group, here all.
        if(map->allowpvp!=0&&IsPlayer())
        {
            CPlayer* plattacker=(CPlayer*) this;
            for (UINT i=0;i<map->PlayerList.size();i++)
            {
                CPlayer* thisplayer=map->PlayerList.at(i);
                if(thisplayer==NULL)
                {
                    continue;
                }

                //not attacking allies.
                if(thisplayer->pvp_id==plattacker->pvp_id)
                {
                    continue;
                }

                if(GServer->IsMonInCircle( goodtarget,thisplayer->Position->current,(float)skill->aoeradius+1))
                {
                    //LMA: we have to since AOE doesn't have a specific target and atkskil resets some values.
                    Battle->skilltarget=thisplayer->clientid;
                    Battle->skillid=skill->id;
                    Battle->atktype = save_atktype;
                    UseAtkSkill( (CCharacter*) thisplayer, skill );
                    Log(MSG_WARNING,"AOE Skill pvp player %s",thisplayer->CharInfo->charname);
                }

            }

        }

    }

    if(IsMonster() && !IsSummon())
    {
        for(UINT i=0;i<map->PlayerList.size();i++)
        {
            CPlayer* player = map->PlayerList.at(i);
            if(player->clientid == clientid) continue;
            if(GServer->IsMonInCircle( goodtarget,player->Position->current,(float)skill->aoeradius+1))
            {
                Log(MSG_INFO,"AOE Attack (2) monster attacks player %s radius %.2f",player->CharInfo->charname,(float)skill->aoeradius+1);
                UseAtkSkill( (CCharacter*) player, skill );
            }

        }
    }
    //osprose end

    if(Enemy!=NULL)
    {
        if(!Enemy->IsDead( ))
        {
            //LMA: UW handling.
            //LMA: done in AtkSkill
            /*
            if ((Position->Map==9)&&IsPlayer()&&Enemy->IsPlayer())
            {
                Log(MSG_INFO,"UWKILL begin AOE skill");
                UWKill(Enemy);
            }
            */

            Battle->atktarget = Battle->target;
            Battle->atktype = NORMAL_ATTACK;
            Battle->skilltarget = 0;
            Battle->skillid = 0;

            //LMA: done in AtkSkill
            /*
            if(!is_already_dead)
                TakeExp(Enemy);
            */
        }
        else
        {
            ClearBattle( Battle );
        }

    }
    else
    {
        ClearBattle( Battle );
    }

    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP<0) Stats->MP=0;
    Battle->lastAtkTime = clock( );
    return true;
}

bool CCharacter::AoeBuff( CSkills* skill )
{
    Position->destiny = Position->current;

    if(Battle->castTime==0)
    {
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, (CCharacter*)this );
        Battle->castTime = clock();
        return true;
    }
    else
    {
        clock_t etime = clock() - Battle->castTime;
        if(etime<SKILL_DELAY)
            return true;
    }

    Battle->castTime = 0;
    CMap* map = GServer->MapList.Index[Position->Map];

    //checking if buffing party and no party so himself :)
    if(skill->target==1 && GetParty( )==NULL)
    {
        Log(MSG_INFO,"AOEBuffs, buffing only myself");
        UseBuffSkill( this, skill );
        ClearBattle( Battle );
        Battle->lastAtkTime = clock( );
        return true;
    }

    //LMA: Summon don't go there...
    //if(IsMonster())
    if(IsMonster()&&!IsSummon())
    {
        Log(MSG_INFO,"Monster in AOE Buff");
        for(UINT i=0;i<map->MonsterList.size();i++)
        {
            CMonster* monster = map->MonsterList.at(i);
            switch(skill->target)
            {
                case tPartyMember: // party
                break;
                case 2: //tClanMember
                break;
                case 3: //tAlly
                case 7: //tAllCharacters
                case 8: //tAllMembers. all characters
                {
                     //Log(MSG_INFO,"Applying AOE buff as long as I can find a close enough monster");
                     if(GServer->IsMonInCircle( Position->current,monster->Position->current,(float)skill->aoeradius + 1))
                            UseBuffSkill( (CCharacter*)monster, skill );
                }
                break;
                case 5: //hostile.
                break;
            }
        }

    }
    else
    {
        //players / summons
        for(UINT i=0;i<map->PlayerList.size();i++)
        {
            CPlayer* player = map->PlayerList.at(i);
            switch(skill->target)
            {
                case 1: // party
                {
                    if(player->Party->party==GetParty( ))
                    {
                        if(GServer->IsMonInCircle( Position->current,player->Position->current,(float)skill->aoeradius+1))
                            UseBuffSkill( (CCharacter*)player, skill );
                    }
                }
                break;
                case 2: //clan member
                {
                    if (player->Clan == GetClan( ))
                    {
                        //Log(MSG_INFO,"Applying AOE buff as long as I can find a close enough clan member");
                        if(GServer->IsMonInCircle( Position->current, player->Position->current, (float)skill->aoeradius + 1 ) )
                            UseBuffSkill( (CCharacter*)player, skill );
                    }
                }
                break;
                case 3:  //ally
                case 7: //everyone
                case 8: //all characters
                {
                    if(GServer->IsMonInCircle( Position->current,player->Position->current,(float)skill->aoeradius + 1))
                         UseBuffSkill( (CCharacter*)player, skill );
                }
                break;
                case 5: //hostile
                break;
            }
        }

    }


    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP<0) Stats->MP=0;

    //ClearBattle( Battle );
    //osprose
    Battle->bufftarget = 0;
    Battle->skilltarget = 0;
    Battle->skillid = 0;

    //LMA: no because in weird cases (party+first target buff+aoe buff)
    //a party buffer could attack a buffed one.
    //Battle->atktype = NORMAL_ATTACK;
    Battle->atktype = 0;
    //ClearBattle( Battle );
    //osprose end

    Battle->lastAtkTime = clock( );
    return true;
}

// Aoe Debuff [netwolf]
bool CCharacter::AoeDebuff( CSkills* skill, CCharacter* Enemy )
{
    Position->destiny = Position->current;

    if(Battle->castTime==0)
    {
        BEGINPACKET( pak, 0x7bb );
        ADDWORD    ( pak, clientid );
        GServer->SendToVisible( &pak, (CCharacter*)this );
        Battle->castTime = clock();
        return true;
    }
    else
    {
        clock_t etime = clock() - Battle->castTime;
        if(etime<SKILL_DELAY)
            return true;
    }
    Battle->castTime = 0;

    CMap* map = GServer->MapList.Index[Position->Map];
    for(UINT i=0;i<map->MonsterList.size();i++)
    {
        CMonster* monster = map->MonsterList.at(i);
        if(monster->clientid==clientid) continue;
        if(IsSummon( ) || IsPlayer( ))
        {
            if(monster->IsSummon( ) && (map->allowpvp==0 || monster->owner==clientid)) continue;
        }
        else
        {
            if(!monster->IsSummon( )) continue;
        }
        if(GServer->IsMonInCircle( Position->current,monster->Position->current,(float)skill->aoeradius+1))
        {
            Log(MSG_INFO,"AOE Debuff (1) monster %i",monster->montype);
            UseDebuffSkill( (CCharacter*) monster, skill );
        }

    }

    if(map->allowpvp!=0 || (IsMonster( ) && !IsSummon( )))
    {
        for(UINT i=0;i<map->PlayerList.size();i++)
        {
            CPlayer* player = map->PlayerList.at(i);
            if(player->clientid==clientid) continue;
            if(GServer->IsMonInCircle( Position->current,player->Position->current,(float)skill->aoeradius+1))
            {
                Log(MSG_INFO,"AOE Debuff (2) player %s",player->CharInfo->charname);
                UseDebuffSkill( (CCharacter*) player, skill );
            }

        }
    }

    Stats->MP -= (skill->mp - (skill->mp * Stats->MPReduction / 100));
    if(Stats->MP<0) Stats->MP=0;
    ClearBattle( Battle );
    Battle->lastAtkTime = clock( );
    return true;
}

// use skill attack
void CCharacter::UseAtkSkill( CCharacter* Enemy, CSkills* skill, bool deBuff )
{
    //LMA: Sometimes it's fired several times, no need to kill several times ;)
    bool is_already_dead=Enemy->IsDead();

    reduceItemsLifeSpan( false );
    Enemy->reduceItemsLifeSpan(true);
    //Skill power calculations
    long int skillpower = skill->atkpower + (long int)floor(GetInt( )/2);
    long int level_diff = Stats->Level - Enemy->Stats->Level;
    skillpower -= Enemy->Stats->Magic_Defense;
    if(Enemy->IsMonster()){
        if(level_diff >= 1){
            skillpower += Stats->Attack_Power * (level_diff / 5) + (level_diff*2);
        }else{
            skillpower += Stats->Attack_Power - (level_diff / 2);
        }
    }
    //Tell enemy he's attacked & add damage & send the dmg packet

    Log(MSG_INFO,"Atk Skill damage %li, monster HP before %li",skillpower,Enemy->Stats->HP);

    bool bflag = false;
    Enemy->OnBeAttacked( this );
    if(skillpower<=0) skillpower = 0;
    if(IsPlayer())
    {
        //LMA: ED, Devilking / Arnold
        skillpower+=Stats->ExtraDamage_add;
        skillpower+=((skillpower*(Stats->ExtraDamage))/100);
    }
    if(!Enemy->IsSummon( ) && Enemy->IsMonster( ))
    {
        Enemy->AddDamage( this, (long long) skillpower );
        Enemy->damagecounter+= (long long) skillpower;// is for AI
    }
    Enemy->Stats->HP -=  (long long) skillpower;
    Log(MSG_INFO,"Atk Skill damage %li, monster HP after %li",skillpower,Enemy->Stats->HP);

    // actually the target was hit, if it was sleeping, set duration of
    // sleep to 0. map process will remove sleep then at next player-update
    if (Enemy->Status->Sleep != 0xff) {
        Enemy->MagicStatus[Enemy->Status->Sleep].Duration = 0;
    }

    //LMA: patch by sickb0y
    //Problems with some mage skills.
    /*
    BEGINPACKET( pak, 0x7b6 );
    ADDWORD    ( pak, Enemy->clientid );
    ADDWORD    ( pak, clientid );
    ADDDWORD   ( pak, 0x000007f8 );
    ADDBYTE    ( pak, 0x00 );
    ADDDWORD   ( pak, skillpower );
    */

    unsigned short command = (skill->skilltype == 6 || skill->skilltype == 9) ? 0x799 : 0x7b6;
    BEGINPACKET( pak, command );

    if(command == 0x799)
    {
        ADDWORD    ( pak, clientid );
        ADDWORD    ( pak, Enemy->clientid );
        //LMA: Skillpower elsewhere.
        //ADDDWORD   ( pak, skillpower );
    }
    else
    {
        ADDWORD    ( pak, Enemy->clientid );
        ADDWORD    ( pak, clientid );
        ADDDWORD   ( pak, 0x000007f8 );
        ADDBYTE    ( pak, 0x00 );
        //LMA: Skillpower elsewhere.
        //ADDDWORD   ( pak, skillpower );
    }

    //end of patch

    //If Enemy is killed
    if(Enemy->IsDead())
    {
        //LMA: Skillpower elsewhere.
        ADDDWORD   ( pak, Enemy->Stats->MaxHP);

        //LMA: UW handling, adding support for other quests.
        //if (!is_already_dead&&(GServer->MapList.Index[Position->Map]->QSDkilling>0||GServer->MapList.Index[Position->Map]->QSDDeath>0)&&IsPlayer()&&Enemy->IsPlayer())
        if (!is_already_dead&&((GServer->MapList.Index[Position->Map]->QSDkilling>0&&IsPlayer()&&Enemy->IsPlayer())||(GServer->MapList.Index[Position->Map]->QSDDeath&&Enemy->IsPlayer())))
        {
            Log(MSG_INFO,"UWKILL begin skill atk");
            UWKill(Enemy);
        }

        Log(MSG_INFO,"Enemy is dead");
        CDrop* thisdrop = NULL;
        ADDDWORD   ( pak, 16 );

        /*LMA: disabling drop for now.
        if(!Enemy->IsSummon( ) && !Enemy->IsPlayer( ))
        {
            thisdrop = Enemy->GetDrop( );
            if(thisdrop!=NULL)
            {
                ADDFLOAT   ( pak, thisdrop->pos.x*100 );
                ADDFLOAT   ( pak, thisdrop->pos.y*100 );
                if(thisdrop->type==1)
                {
                    ADDDWORD( pak, 0xccccccdf );
                    ADDDWORD( pak, thisdrop->amount );
                    ADDDWORD( pak, 0xcccccccc );
                    ADDWORD ( pak, 0xcccc );
                }
                else
                {
                    ADDDWORD   ( pak, GServer->BuildItemHead( thisdrop->item ) );
                    ADDDWORD   ( pak, GServer->BuildItemData( thisdrop->item ) );
                    ADDDWORD( pak, 0x00000000 );
                    ADDWORD ( pak, 0x0000 );
                }
                ADDWORD    ( pak, thisdrop->clientid );
                ADDWORD    ( pak, thisdrop->owner );
                CMap* map = GServer->MapList.Index[thisdrop->posMap];
                map->AddDrop( thisdrop );
            }
        }

        GServer->SendToVisible( &pak, Enemy, thisdrop );
        */

        if(!Enemy->IsSummon( ) && !Enemy->IsPlayer( ))
        {
            thisdrop = Enemy->GetDrop( );
            if(thisdrop!=NULL)
            {
                /*ADDFLOAT   ( pak, thisdrop->pos.x*100 );
                ADDFLOAT   ( pak, thisdrop->pos.y*100 );
                if(thisdrop->type==1)
                {
                    ADDDWORD( pak, 0xccccccdf );
                    ADDDWORD( pak, thisdrop->amount );
                    ADDDWORD( pak, 0xcccccccc );
                    ADDWORD ( pak, 0xcccc );
                }
                else
                {
                    ADDDWORD   ( pak, GServer->BuildItemHead( thisdrop->item ) );
                    ADDDWORD   ( pak, GServer->BuildItemData( thisdrop->item ) );
                    ADDDWORD( pak, 0x00000000 );
                    ADDWORD ( pak, 0x0000 );
                }
                ADDWORD    ( pak, thisdrop->clientid );
                ADDWORD    ( pak, thisdrop->owner );*/
                CMap* map = GServer->MapList.Index[thisdrop->posMap];
                map->AddDrop( thisdrop );
            }

        }

        GServer->SendToVisible( &pak, Enemy );
        //LMA: end of test.

        //LMA: test
        //OnEnemyDie( Enemy );
        ClearBattle(Battle);
        if(!is_already_dead)
        {
            TakeExp(Enemy);

            //Some monsters trigger QSD on death...
            /*
            if(IsPlayer()&&Enemy->IsMonster()&&Enemy->char_montype==3003)
            {
                CPlayer* thisplayer= (CPlayer*) this;
                //StPatrick_01;
                thisplayer->ExecuteQuestTrigger(71055222,true);
            }
            */

        }
        //end of test.

        //LMA: Special Packet to tell if the player is dead?
        if(Enemy->IsPlayer())
        {
            CPlayer* plkilled=(CPlayer*) Enemy;
            if(plkilled!=NULL)
            {
                BEGINPACKET( pak, 0x820 );
                ADDWORD    ( pak, 1 );
                ADDWORD    ( pak, 0 );
                plkilled->client->SendPacket(&pak);
                Log(MSG_WARNING,"Packet 820 sent to %s",plkilled->CharInfo->charname);
            }

        }
        else if (Enemy->IsMonster())
        {
            //LMA let's send a suicide packet so monster should be killed twice.
            CMonster* thisMonster = GServer->GetMonsterByID(Enemy->clientid, Enemy->Position->Map);
            BEGINPACKET( pak, 0x799 );
            ADDWORD    ( pak, Enemy->clientid );
            ADDWORD    ( pak, Enemy->clientid );
            ADDDWORD   ( pak, thisMonster->thisnpc->hp*thisMonster->thisnpc->level );
            ADDDWORD   ( pak, 16 );
            GServer->SendToVisible( &pak, Enemy );
        }

    }
    else
    {
        //LMA: Skillpower elsewhere.
        ADDDWORD   ( pak, skillpower );

        //If enemy is still alive
        Log(MSG_INFO,"The enemy is still alive");
        ADDDWORD   ( pak, 4 );
        GServer->SendToVisible( &pak, Enemy );

        //osprose
        //if (deBuff) return;

        //GOTO debuffing section
        //bflag = GServer->AddDeBuffs( skill, Enemy, GetInt( ) );
        bflag = GServer->AddBuffs( skill, Enemy, GetInt( ) ); // send to AddBuffs instead.

        //Send (de)buff information to the whole world
        if(skill->nbuffs>0 && bflag)
        {
            Log(MSG_INFO,"The enemy cliendID %u is being buffed with %u",Battle->skilltarget,Battle->skillid);
            BEGINPACKET( pak, 0x7b5 );
            ADDWORD    ( pak, Battle->skilltarget );
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, Battle->skillid );
            ADDWORD    ( pak, GetInt( ) );
            ADDBYTE    ( pak, skill->nbuffs );
            GServer->SendToVisible( &pak, Enemy );
        }
    }
    if (deBuff) return;
    //Send skill animation to the world
    RESETPACKET( pak, 0x7b9);
    ADDWORD    ( pak, clientid);
    ADDWORD    ( pak, Battle->skillid);
    ADDWORD    ( pak, 1);
    GServer->SendToVisible( &pak, this );

    //osprose
    Battle->bufftarget = 0;
    Battle->skilltarget = 0;
    Battle->skillid = 0;
    Battle->atktype = NORMAL_ATTACK;
    //osprose end

    return;
}

// use buff skill
void CCharacter::UseBuffSkill( CCharacter* Target, CSkills* skill )
{
    bool bflag = false;
    bflag = GServer->AddBuffs( skill, Target, GetInt( ) );
    Log(MSG_INFO,"In UseBuffSkills, skill %i, nbuffs %i, bflag %i",skill->id,skill->nbuffs,bflag);
    if(skill->nbuffs>0 && bflag == true)
    {
        BEGINPACKET( pak, 0x7b5 );
        ADDWORD    ( pak, Target->clientid );
        ADDWORD    ( pak, clientid );
        ADDWORD    ( pak, Battle->skillid );
        ADDWORD    ( pak, GetInt( ) );
        ADDBYTE    ( pak, skill->nbuffs );
        GServer->SendToVisible( &pak, Target );
    }
    BEGINPACKET( pak, 0x7b9);
    ADDWORD    ( pak, clientid);
    ADDWORD    ( pak, Battle->skillid);
    ADDWORD    ( pak, 1);
	GServer->SendToVisible( &pak, (CCharacter*)this );

	//LMA: Patch for Revive stuff...
	if(skill->skilltype==20&&Target->IsPlayer())
	{
	    CPlayer* thisclient = GServer->GetClientByID(Target->clientid,Position->Map);
	    if (thisclient==NULL)
	    {
	        CPlayer* thisclient = GServer->GetClientByID(Target->clientid,Target->Position->Map);
	    }

	    if (thisclient==NULL)
	    {
	        Log(MSG_WARNING,"Can't find clientID %i for revive...",Target->clientid);
	        return;
	    }

        //not exact but should be fair enough...
        Log(MSG_INFO,"before Revive, HP %I64i, Xp %I64i, skill %i",thisclient->Stats->HP,thisclient->CharInfo->Exp,skill->atkpower);
        unsigned long long new_exp=(unsigned long long) (thisclient->CharInfo->Exp*3*skill->atkpower/(100*100));
        thisclient->CharInfo->Exp+=new_exp;
        thisclient->Stats->HP=thisclient->Stats->MaxHP*30/100;
	    Log(MSG_INFO,"Revive, new HP %I64i, %I64i, Xp added %I64i, new %I64i",thisclient->Stats->HP,Target->Stats->HP,new_exp,thisclient->CharInfo->Exp);
        BEGINPACKET( pak, 0x79b );
        ADDDWORD   ( pak, thisclient->CharInfo->Exp );
        ADDWORD    ( pak, thisclient->CharInfo->stamina );
        ADDWORD    ( pak, 0 );
        thisclient->client->SendPacket( &pak );
	}
	//End of Patch.


	return;
}

// use Debuff skill
void CCharacter::UseDebuffSkill( CCharacter* Enemy, CSkills* skill )
{
    bool bflag = false;
    bflag = GServer->AddBuffs( skill, Enemy, GetInt( ) );
    if(skill->nbuffs>0 && bflag == true)
    {
        BEGINPACKET( pak, 0x7b5 );
        ADDWORD    ( pak, Enemy->clientid );
        ADDWORD    ( pak, clientid );
        ADDWORD    ( pak, Battle->skillid );
        ADDWORD    ( pak, GetInt( ) );
        ADDBYTE    ( pak, skill->nbuffs );
        GServer->SendToVisible( &pak, Enemy );
    }
    BEGINPACKET( pak, 0x7b9);
    ADDWORD    ( pak, clientid);
    ADDWORD    ( pak, Battle->skillid);
    ADDWORD    ( pak, 1);
    GServer->SendToVisible( &pak, (CCharacter*)this );
}

// Use a skill (gm command)
bool CCharacter::UseSkill( CSkills* skill, CCharacter *Target )
{
  if (skill->atkpower > 0)
  {
    Log(MSG_INFO, "Need to do %i%s %s in range %i to target %i",
        ( skill->atkpower ),
        ( (skill->range > 0) ? " AOE" : "" ),
        ( (GServer->isSkillTargetFriendly(skill)) ? "healing" : "damage" ),
        ( skill->range ),
        ( skill->target ) );
  }
  for (int i = 0; i < 2; i++) {
    Log(MSG_INFO, "Status ID: %i", skill->status[i]);
    if (skill->status[i] == 0) continue;
    CStatus* status = GServer->GetStatusByID(skill->status[i]);
    if (status == NULL) continue;
    /************************
    We'll probably need to use status->decrease (Figure out whether buf is up/down
    status->repeat will tell us whether it's a one-time (Stat Boost [2]), repeat
    (Recovery, continueing damage [1]) or Special (Status Effect [3]).
    ************************/
    if (status->repeat == 1) // Continuous
    {
      Log(MSG_INFO, "Need to take stat %i and %s it by %i%s over %i seconds", skill->buff[i],
          ((status->decrease) ? "decrease" : "increase"),
          ((skill->value1[i] != 0) ? skill->value1[i] : skill->value2[i]),
          ((skill->value1[i] != 0) ? "" : "%"), skill->duration);

    } else if (status->repeat == 2) // Once (Stat Boost)
    {
      Log(MSG_INFO, "Need to take stat %i and %s it by %i%s for %i seconds", skill->buff[i],
          ((status->decrease) ? "decrease" : "increase"),
          ((skill->value1[i] != 0) ? skill->value1[i] : skill->value2[i]),
          ((skill->value1[i] != 0) ? "" : "%"), skill->duration);

    } else if (status->repeat == 3) // Status Effect (Poison,etc)
    {
      Log(MSG_INFO, "Need to give user status effect %i for %i seconds", skill->buff[i],
          ((skill->value1[i] != 0) ? skill->value1[i] : skill->value2[i]),
          ( skill->duration ) );

    }
  }
  return true;
}

//LMA: We take exp from a dead player.
bool CCharacter::TakeExp( CCharacter *Target )
{
    //LMA: We don't take exp from a player if the killer is a Player.
    if(!Target->IsPlayer()||((!Target->IsPlayer())&&(!IsPlayer())))
    {
        return true;
    }

    //no need to take exp from UW.
    if(Position->Map==9)
    {
        return true;
    }

    //We take 3% of his xp.
    CPlayer* thisclient = GServer->GetClientByID(Target->clientid,Target->Position->Map);
    if (thisclient==NULL)
    {
        CPlayer* thisclient = GServer->GetClientByID(Target->clientid,Position->Map);
    }

    if (thisclient==NULL)
    {
        Log(MSG_WARNING,"Can't find clientID %i to take his Exp...",Target->clientid);
        return true;
    }

    Log(MSG_INFO,"Player %i died, Xp before %I64i",Target->clientid,thisclient->CharInfo->Exp);
    unsigned long long new_exp=(unsigned long long) (thisclient->CharInfo->Exp*3/100);
    thisclient->CharInfo->Exp-=new_exp;
    BEGINPACKET( pak, 0x79b );
    ADDDWORD   ( pak, thisclient->CharInfo->Exp );
    ADDWORD    ( pak, thisclient->CharInfo->stamina );
    ADDWORD    ( pak, 0 );
    thisclient->client->SendPacket( &pak );
    Log(MSG_INFO,"Player %i died, Xp taken %I64i, new %I64i",Target->clientid,new_exp,thisclient->CharInfo->Exp);


    return true;
}

//LMA: Union war kill (and more generally qsd killing / death trigger).
void CCharacter::UWKill(CCharacter* Enemy)
{
    if(!IsPlayer()&&!Enemy->IsPlayer())
    {
        Log(MSG_WARNING,"No player for UWKill");
        return;
    }

    CPlayer* plkiller=NULL;
    CPlayer* plkilled=NULL;

    if(IsPlayer())
    {
        plkiller=(CPlayer*) this;
    }

    if(Enemy->IsPlayer())
    {
        plkilled=(CPlayer*) Enemy;
    }

    int killer_level=0;
    int killed_level=0;

    dword hashkilling = GServer->MapList.Index[Position->Map]->QSDkilling;
    dword hashdeath = GServer->MapList.Index[Position->Map]->QSDDeath;

    //The killer.
    if(hashkilling>0&&plkiller!=NULL)
    {
        plkiller->ExecuteQuestTrigger(hashkilling,true);
        //Log(MSG_WARNING,"quest hash %u to the killer %s",hashkilling,plkiller->CharInfo->charname);
        killer_level=plkiller->Stats->Level;
    }

    //The killed one.
    if(hashkilling>0&&plkilled!=NULL)
    {
        plkilled->ExecuteQuestTrigger(hashdeath,true);
        //Log(MSG_WARNING,"quest hash %u to the killed %s",hashdeath,plkilled->CharInfo->charname);
        killed_level=plkilled->Stats->Level;
    }

    //Let's give some exp points to the killer...
    if(!IsPlayer()||!Enemy->IsPlayer())
    {
        //only if both are players.
        return;
    }

    if(plkiller==NULL)
    {
        return;
    }

    if(plkilled==NULL)
    {
        killed_level=killer_level;
    }

    UINT bonus_exp=GServer->GetColorExp(killer_level,killed_level,7000);
    plkiller->CharInfo->Exp += bonus_exp;
    BEGINPACKET( pak, 0x79b );
    ADDDWORD   ( pak, plkiller->CharInfo->Exp );
    ADDWORD    ( pak, plkiller->CharInfo->stamina );

    if(plkilled!=NULL)
    {
        ADDWORD    ( pak, plkilled->clientid );
    }
    else
    {
        ADDWORD    ( pak, 0 );
    }

    plkiller->client->SendPacket( &pak );
    //Log(MSG_WARNING,"Giving %u exp to %s",bonus_exp,plkiller->CharInfo->charname);


  return;
}

