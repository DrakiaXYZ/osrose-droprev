/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007,2008,2009 OSRose Team http://www.dev-osrose.com

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

    developed with Main erose/hrose source server + some change from the original eich source
*/
#include "character.h"
#include "worldserver.h"

// return true if this character is monster
bool CCharacter::IsMonster( )
{
    if(CharType==2 || CharType==3)
        return true;
    return false;
}

// return true if this character is player
bool CCharacter::IsPlayer( )
{
    if(CharType==1)
        return true;
    return false;
}

// return true if HP is lower to 1
bool CCharacter::IsDead( )
{
    if(Stats->HP<=0)
        return true;
    return false;
}

// return true if this character is summon
bool CCharacter::IsSummon( )
{
    if(CharType==3)
        return true;
    return false;
}

// Virtual function [return item drop]
CDrop* CCharacter::GetDrop( )
{
    return NULL;
}

// start action [attack]
void CCharacter::StartAction( CCharacter* Target, BYTE action, UINT skillid, bool restart, CCharacter* receiver )
{
    //Log(MSG_INFO,"Someone does an action %i, skill %i",action,skillid);
    BEGINPACKET( pak, 0 );
    if (restart)
    {
       Target=GetCharTarget( );
       action=Battle->atktype;
       skillid=Battle->skillid;
    }

    //Drakia: If the target is NULL, we should only do something that doesn't require a target.
    if (Target == NULL && (action != SKILL_AOE &&action != BUFF_SELF &&action != BUFF_AOE &&action != MONSTER_BUFF_SELF &&action != AOE_TARGET))
    {
        Log(MSG_WARNING,"We tried to start attack %i without a target",action);
        return;
    }

    //LMA: don't attack a dead or an offline player... It's stupid...
    if(Target!=NULL)
    {
        if(Target->IsPlayer())
        {
            CPlayer* thisplayer=reinterpret_cast<CPlayer*>(Target);
            if(!thisplayer->Session->inGame)
            {
                //Log(MSG_INFO,"We don't attack a player not in game yet...");
                return;
            }

        }

        if(Target->IsDead())
        {
            //but we can if it's a friendly (restore...).
            if(action!=SKILL_BUFF)
            {
                ClearBattle(Battle);
                return;
            }

        }

    }


    switch(action)
    {
        case NORMAL_ATTACK:
        {
            RESETPACKET( pak, 0x798 );
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, Target->clientid );

            //ADDWORD    ( pak, Stats->Move_Speed );
            ADDWORD    ( pak, Stats->Base_Speed );
            //Log(MSG_INFO,"move speed %u, base speed %u",Stats->Move_Speed,Stats->Base_Speed);

            ADDFLOAT   ( pak, Target->Position->current.x*100 );
            ADDFLOAT   ( pak, Target->Position->current.y*100 );
            Battle->target = Target->clientid;
            Battle->atktarget = Target->clientid;
            Battle->atktype = action;
            Position->destiny  = Target->Position->current;
            Position->lastMoveTime = clock( );
        }
        break;
        case SKILL_ATTACK:
        case SKILL_BUFF:
        {
        	RESETPACKET( pak, 0x7b3 );
        	ADDWORD    ( pak, clientid );
        	ADDWORD    ( pak, Target->clientid );
        	ADDWORD    ( pak, skillid );
        	ADDWORD    ( pak, 50000 );
            ADDFLOAT   ( pak, Target->Position->current.x*100 );
            ADDFLOAT   ( pak, Target->Position->current.y*100 );
            Battle->target = Target->clientid;

            if(action==SKILL_ATTACK) Battle->skilltarget = Target->clientid;
            else Battle->bufftarget = Target->clientid;

            Battle->atktype = action;
            Position->destiny  = Target->Position->current;
            Battle->skillid = skillid;
            Position->lastMoveTime = clock( );
        }
        break;
        case MONSTER_SKILL_ATTACK:
        case MONSTER_SKILL_BUFF:
        {
        	RESETPACKET( pak, 0x7b3 );
        	ADDWORD    ( pak, clientid );
        	ADDWORD    ( pak, Target->clientid );
        	ADDWORD    ( pak, skillid );
            ADDBYTE (pak,0x2b);
            ADDBYTE (pak,0x01);
            ADDFLOAT   ( pak, Target->Position->current.x*100 );
            ADDFLOAT   ( pak, Target->Position->current.y*100 );
            ADDBYTE    ( pak, 0x06);
            Battle->target = Target->clientid;

            if(action==MONSTER_SKILL_ATTACK) Battle->skilltarget = Target->clientid;
            else Battle->bufftarget = Target->clientid;

            Battle->atktype = action;
            Position->destiny  = Target->Position->current;
            Battle->skillid = skillid;
            Position->lastMoveTime = clock( );
        }
        break;
        case SUMMON_BUFF:
        {
            //LMA: Special case for Support summons
        	RESETPACKET( pak, 0x7b3 );
        	ADDWORD    ( pak, clientid );
        	ADDWORD    ( pak, Target->clientid );
        	ADDWORD    ( pak, skillid );
            ADDBYTE (pak,0x2b);
            ADDBYTE (pak,0x01);
            ADDFLOAT   ( pak, Target->Position->current.x*100 );
            ADDFLOAT   ( pak, Target->Position->current.y*100 );
            /*
            ADDFLOAT   ( pak, Position->current.x*100 );
            ADDFLOAT   ( pak, Position->current.y*100 );
            */
            ADDBYTE    ( pak, 0x06);
            Battle->target = 0;
            //Battle->skilltarget = Target->clientid;
            Battle->bufftarget = Target->clientid;
            Battle->atktype = action;
            Position->destiny  = Target->Position->current;
            Battle->skillid = skillid;
            Position->lastMoveTime = clock( );
            GServer->SendToVisible( &pak, Target );
            return;

        }
        break;
        case SKILL_AOE:
        case BUFF_SELF:
        case BUFF_AOE:
        {
            RESETPACKET( pak, 0x7b2);
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, skillid );
            Battle->atktype = action;
            Battle->skillid = skillid;
        }
        break;
        case MONSTER_BUFF_SELF:
        {
            RESETPACKET( pak, 0x7b2);
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, skillid );
            Battle->atktype = action;
            Battle->skillid = skillid;
        }
        break;
        case AOE_TARGET:
        {
            //LMA 2008/09/02: new version, the target is a zone, not a monster... so we stick with aoedestiny ;)
            RESETPACKET( pak, 0x7b4 );
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, skillid );
            ADDFLOAT   ( pak, Position->aoedestiny.x*100 );
            ADDFLOAT   ( pak, Position->aoedestiny.y*100 );
            Battle->atktype = action;
            Battle->skillid = skillid;
            Battle->skilltarget = 0;
            Battle->target = 0;
            Position->destiny  = Position->aoedestiny;
            Position->lastMoveTime = clock( );
            Log(MSG_INFO,"StartAction, AOE_TARGET, target (%.2f,%.2f)",Position->aoedestiny.x,Position->aoedestiny.y);
        }
        break;
        case STAY_STILL_ATTACK:
        {
             //LMA: Very special case where the monster don't really attack (mc)
             Battle->atktype = action;
             Battle->skillid = skillid;
             Battle->skilltarget = Target->clientid;
             return;
         }
        default: return;
    }
    //if (getClient()==NULL)
       GServer->SendToVisible( &pak, this );
    /*else
        getClient();*/

    if (!IsPlayer()) Battle->contatk = true;
}

bool CCharacter::IsOnBattle( )
{
    //Bonfire don't attack...
    if(Battle->atktype==0) return false;
    switch(Battle->atktype)
    {
        case NORMAL_ATTACK:
        {
            if(Battle->atktarget!=0) return true;
        }
        break;
        case SKILL_ATTACK:
        case AOE_TARGET:
        {
            if(Battle->skilltarget!=0 && Battle->skillid!=0) return true;
        }
        break;
        case SKILL_BUFF:
        {
            if(Battle->bufftarget!=0 && Battle->skillid!=0) return true;
        }
        break;
        case SUMMON_BUFF:
        {
             if(Battle->bufftarget!=0&&Battle->skillid!=0) return true;
        }
        break;
        case MONSTER_SKILL_BUFF:
        {
             if(Battle->bufftarget!=0 && Battle->skillid!=0) return true;
        }
        break;
        case MONSTER_SKILL_ATTACK:
        {
             if(Battle->skilltarget!=0 && Battle->skillid!=0) return true;
        }
        break;
        case MONSTER_BUFF_SELF:
        case SKILL_AOE:
        case SKILL_SELF:
        case BUFF_SELF:
        case BUFF_AOE:
        {
            if(Battle->skillid!=0) return true;
        }
        break;
        case STAY_STILL_ATTACK:
        {
              //LMA: Very special case where the monster don't really attack (mc)
              return true;
         }
        default: return false;
    }
}

bool CCharacter::CanAttack( ) // updated by Core
{

    // we cannot attack while sleeping or stunned
    if (Status->Sleep != 0xff || Status->Stun != 0xff)
    {
        return false;
    }

     clock_t etime = clock() - Battle->lastAtkTime;
    //if( etime < CLOCKS_PER_SEC * 100 / Stats->Attack_Speed ) return false;   for the time being let's just let it be unsynced to avoid disbalanced
    int weapontype = IsPlayer()?getWeaponType():0;
    //the fix: not much but it works ;)
    if (weapontype == 212)
        return true;

    //MZ
    //it's my fix, to protect from exception divide by zero
    //During fight, at change of the weapon it happens
    if (0 == Stats->Attack_Speed) return false;
    //MZ

    if ( weapontype == 231 || weapontype == 232 || weapontype == 233 )
    {
       if( (etime < CLOCKS_PER_SEC * (GServer->ATTK_SPEED_MODIF*4/3) / Stats->Attack_Speed) || Status->Stun != 0xff )
       {
            /*
            //LMA: logs
           if(Position->Map==8)
           {
               Log(MSG_INFO,"CanAttack false weapontype %i stun %i etime %u ATTK_SPEED_MODIF %u aspeed %u",weapontype,Status->Stun,etime,GServer->ATTK_SPEED_MODIF,Stats->Attack_Speed);
           }
           //end of logs.
           */

           return false;
       }

    }
    else
    {
       if( (etime < CLOCKS_PER_SEC * GServer->ATTK_SPEED_MODIF / Stats->Attack_Speed) || Status->Stun != 0xff )
       {
           /*
           //LMA: logs
           if(Position->Map==8)
           {
               Log(MSG_INFO,"CanAttack false weapontype %i stun %i etime %u ATTK_SPEED_MODIF %u aspeed %u",weapontype,Status->Stun,etime,GServer->ATTK_SPEED_MODIF,Stats->Attack_Speed);
           }
           //end of logs.
           */

           return false;
       }

    }

    return true;
}

// return true if target is reached
bool CCharacter::IsTargetReached( CCharacter* Enemy, CSkills* skill )
{
    CMap* map = GServer->MapList.Index[Position->Map];
    float distance = GServer->distance( Position->current, Enemy->Position->current );
    if(skill==NULL)
    {
        if(distance<=Stats->Attack_Distance)
            return true;
    }
    else
    {
        if(distance<=skill->range)
            return true;
    }
    return false;
}

// return true if the character is moving
bool CCharacter::IsMoving( )
{
    if(Position->current.x == Position->destiny.x && Position->current.y == Position->destiny.y )
        return false;
    return true;
}

bool CCharacter::stopMoving()
{
     if( Position->current.x == Position->destiny.x && Position->current.y == Position->destiny.y )
        return false;
     Position->destiny =Position->current; //STOP MOVING
     printf(IsPlayer()?"Stopping char movement\n":"Stopping mob movement\n");
     printf("Stopped moving at: %i, %i\n", (int)Position->current.x, (int)Position->current.y);
     return true;
}

// VIRTUAL [add player damage]
void CCharacter::AddDamage( CCharacter* enemy, long long hitpower)
{
    Log(MSG_INFO,"In CCharacter::AddDamage");
}

// return character target
CCharacter* CCharacter::GetCharTarget( )
{
    return GServer->MapList.Index[Position->Map]->GetCharInMap( Battle->target );
}

// return character Buff target
CCharacter* CCharacter::GetCharBuffTarget( )
{
    return GServer->MapList.Index[Position->Map]->GetCharInMap( Battle->bufftarget );
}

// VIRTUAL [reduce ammon/bullets/cannons]
void CCharacter::ReduceABC( )
{

}

// VIRTUAL [get extra buff value]
int CCharacter::GetEVBuff( )
{
    return 0;
}

// VIRTUAL [return intelligence]
unsigned int CCharacter::GetInt( )
{
    return 15;
}


void CCharacter::RefreshBuff( )
{
    bool bflag = false;
    for( UINT i=0;i<30;i++)
    {
        if(MagicStatus[i].Buff == 0) continue;
        clock_t etime = clock() - MagicStatus[i].BuffTime;
        if( etime >= MagicStatus[i].Duration * CLOCKS_PER_SEC )
        {
            switch(MagicStatus[i].Status)
            {
                case 18: // attack power up
                case 19: // attack power down
                case 48: // attack power up
                    if(i<15)
                        Status->Attack_up = 0xff;
                    else
                        Status->Attack_down = 0xff;
                    Stats->Attack_Power = GetAttackPower( );
                break;
                case 20: // def up
                case 21: // def down
                case 49: // def up
                    if(i<15)
                        Status->Defense_up= 0xff;
                    else
                        Status->Defense_down = 0xff;
                    Stats->Defense = GetDefense( );
                break;
                case 24: //accuracy up
                case 25: //accuracy down
                case 51: //attack accuracy up.
                    if(i<15)
                        Status->Accury_up= 0xff;
                    else
                        Status->Accury_down = 0xff;
                    Stats->Accury = GetAccury( );
                break;
                case 22: // macic resistance up
                case 23: // magic resistance down
                case 50: // magic resistance up
                    if(i<15)
                        Status->Magic_Defense_up = 0xff;
                    else
                        Status->Magic_Defense_down = 0xff;
                    Stats->Magic_Defense = GetMagicDefense( );
                break;
                case 28: //dodge up
                case 29: //dodge down
                case 53: //dodge rate up
                    if(i<15)
                        Status->Dodge_up = 0xff;
                    else
                        Status->Dodge_down = 0xff;
                    Stats->Dodge = GetDodge( );
                break;
                case 14: //dash
                case 15: //slow
                case 46: //movement speed increased
                    if(i<15)
                        Status->Dash_up = 0xff;
                    else
                        Status->Dash_down = 0xff;
                    Stats->Move_Speed = GetMoveSpeed( );
                break;
                case 16: // haste attack
                case 17: // slow attack
                case 47: // attack speed up
                    if(i<15)
                        Status->Haste_up = 0xff;
                    else
                        Status->Haste_down = 0xff;
                    Stats->Attack_Speed = GetAttackSpeed( );
                break;
                case 26: // crit up
                case 27: // crit down
                case 52: // crit up
                    if(i<15)
                        Status->Critical_up = 0xff;
                    else
                        Status->Critical_down = 0xff;
                    Stats->Critical = GetCritical( );
                break;
                case 12: // max HP up
                case 44: // max HP up
                    if(i<15)
                        Status->HP_up = 0xff;
                    else
                        Status->HP_down = 0xff;
                    Stats->MaxHP = GetMaxHP( );
                break;
                case 13: // max MP up
                case 45: // max MP up
                    if(i<15)
                        Status->MP_up = 0xff;
                    else
                        Status->MP_down = 0xff;
                    Stats->MaxMP = GetMaxMP( );
                break;
                case 32: // faint
                     Status->Stuned = 0xff;
                     Status->CanAttack = true;
                     //printf("removing stun\n");
                break;
                case 7: case 8: case 9: case 10: case 11: //poisoned
                     Status->Poisoned = 0xff;
                     //printf("removing poison\n");
                break;
                case 30: // muted
                     Status->Muted = 0xff;
                     Status->CanCastSkill = true;
                break;
                case 31: // sleep May need to be fixed later to accomodate multiple status effects.
                     Status->Sleep = 0xff;
                     Status->CanMove = true;
                     Status->CanCastSkill = true;
                     Status->CanAttack = true;
                break;
                case 36: //A_Extra_Damage:
                case 54: //A_GMExtra_Damage:
                case 83: //Valkyrie Charm:
                     if(i<15)
                     {
                        Status->ExtraDamage_up = 0xff;
                        Stats->ExtraDamage = 0xff;
                     }
                     else
                     {
                         Status->ExtraDamage_down = 0xff;
                         Stats->ExtraDamage = 0xff;
                     }

                break;
                case 56: // Taunt
                     Status->Taunt = 0xff;
                break;
                case 58: case 61: case 71: case 77:  case 78: case 79: case 80://flame
                     Status->Flamed = 0xff;
                break;
                case 33://Stealth,Camoflauge
                     if(i<15)
                        Status->Stealth = 0xff;
                     if(IsAttacking( ))
                        MagicStatus[i].Duration = 0;
                     else
                        0xff;
                break;
                case 34://Cloaking
                     Status->Cloaking = 0xff;
                break;
                case 35: //ShieldDamage:
                     if(i<15)
//                     {
                        Status->ShieldDamage_up = 0xff;
//                        Stats->ShieldDamage = 0xff;
//                     }
                     else
//                     {
                         Status->ShieldDamage_down = 0xff;
//                         Stats->ShieldDamage = 0xff;
//                     }

                break;
            }

            MagicStatus[i].Status = 0;
            MagicStatus[i].Buff = 0;
            MagicStatus[i].BuffTime = 0;
            MagicStatus[i].Duration = 0;
            MagicStatus[i].Value = 0;
            bflag = true;
        }
        else if (MagicStatus[i].Status >= 7 && MagicStatus[i].Status <= 11 && etime > 1*CLOCKS_PER_SEC) //Do poison dmg every 1.5 seconds
        {
             Stats->HP -= MagicStatus[i].Status; //Actually take 7, 8, 9, 10 or 11 from the health. Based on the Status itself
             MagicStatus[i].BuffTime += 1*CLOCKS_PER_SEC;
             MagicStatus[i].Duration -= 1;
             //printf("did %i poison dmg to the player, still %i seconds and %i HP remain \n", MagicStatus[i].Status, MagicStatus[i].Duration, Stats->HP);

             //A bunch of messy code to send dmg packet
             BEGINPACKET( pak, 0x7b6 );
             ADDWORD    ( pak, clientid );
             ADDWORD    ( pak, 0 );
             ADDDWORD   ( pak, 0x000007f8 );
             ADDBYTE    ( pak, 0x00 );
             ADDDWORD   ( pak, MagicStatus[i].Status );

             //If Enemy is killed
             if( IsDead())
             {
                 //printf("char died\n");
                 CDrop* thisdrop = NULL;
                 ADDDWORD   ( pak, 16 );
                 if( !IsSummon( ) && !IsPlayer( ))
                 {
                     thisdrop = GetDrop( );
                     if( thisdrop!=NULL)
                     {
                         ADDFLOAT   ( pak, thisdrop->pos.x*100 );
                         ADDFLOAT   ( pak, thisdrop->pos.y*100 );
                         if( thisdrop->type==1)
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
                 GServer->SendToVisible( &pak, this, thisdrop );
             }

             //If enemy is still alive
             else
             {
                 ADDDWORD   ( pak, 4 );
                 GServer->SendToVisible( &pak, this );
             }
         }
        else if ( MagicStatus[i].Status == 58 || MagicStatus[i].Status == 61 || MagicStatus[i].Status == 71 || MagicStatus[i].Status >= 77 && MagicStatus[i].Status <= 80 && etime > 1*CLOCKS_PER_SEC) //Do flame dmg every 1.5 seconds
        {
             Stats->HP -= MagicStatus[i].Status;
             MagicStatus[i].BuffTime += 1*CLOCKS_PER_SEC;
             MagicStatus[i].Duration -= 1;
             printf("did %i flame dmg to the player, still %i seconds and %i HP remain \n", MagicStatus[i].Status, MagicStatus[i].Duration, Stats->HP);

             //A bunch of messy code to send dmg packet
             BEGINPACKET( pak, 0x7b6 );
             ADDWORD    ( pak, clientid );
             ADDWORD    ( pak, 0 );
             ADDDWORD   ( pak, 0x000007f8 );
             ADDBYTE    ( pak, 0x00 );
             ADDDWORD   ( pak, MagicStatus[i].Status );

             //If Enemy is killed
             if( IsDead())
             {
                 //printf("char died\n");
                 CDrop* thisdrop = NULL;
                 ADDDWORD   ( pak, 16 );
                 if( !IsSummon( ) && !IsPlayer( ))
                 {
                     thisdrop = GetDrop( );
                     if( thisdrop!=NULL)
                     {
                         ADDFLOAT   ( pak, thisdrop->pos.x*100 );
                         ADDFLOAT   ( pak, thisdrop->pos.y*100 );
                         if( thisdrop->type==1)
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
                 GServer->SendToVisible( &pak, this, thisdrop );
             }

             //If enemy is still alive
             else
             {
                 ADDDWORD   ( pak, 4 );
                 GServer->SendToVisible( &pak, this );
             }
         }

    }
    if(bflag)
    {
        BEGINPACKET( pak,0x7b7 );
        ADDWORD    ( pak, clientid );
        ADDDWORD   ( pak, GServer->BuildBuffs( this ) );
        GServer->SendToVisible( &pak, this );
    }
}


// VIRTUAL [return party pointer]
CParty* CCharacter::GetParty( )
{
    return NULL;
}

// VIRTUAL [return clan pointer]
CLAN* CCharacter::GetClan( )
{
    return NULL;
}

bool CCharacter::IsAttacking( )
{
    if(IsOnBattle( ))
    {
        switch(Battle->atktype)
        {
            case SKILL_BUFF:
            case BUFF_SELF:
            case BUFF_AOE:
                return false;
        }
    }
    else return false;
    return true;
}

// [VIRTUAL]
void CCharacter::reduceItemsLifeSpan(bool atacked)
{
}

// [VIRTUAL]
unsigned CCharacter::getWeaponType()
{
}

// [VIRTUAL]
bool CCharacter::AddClanPoints(unsigned int count)
{
}

// [VIRTUAL]
CClientSocket* CCharacter::getClient()
{
   return NULL;
}
