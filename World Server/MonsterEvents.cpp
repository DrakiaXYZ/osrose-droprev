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
#include "worldmonster.h"

// called when a monster is attacked  [attack/use atk skill/use buff/run/summon]
bool CMonster::OnBeAttacked( CCharacter* Enemy )
{
    Battle->hitby = Enemy->clientid;

    if(!IsOnBattle( ))
    {

        //LMA: yeah hurt me (used for Santa Rudolph).
        if(thisnpc->helpless==1)
        {
            StartAction( Enemy, STAY_STILL_ATTACK, 0 );
            return true;
        }

        //Some monsters do not attack and stay still (mc)
        if(!stay_still)
        {
            StartAction( Enemy, NORMAL_ATTACK, 0 );
        }
        else
        {
            Log(MSG_INFO,"Stay still attack");
            StartAction( Enemy, STAY_STILL_ATTACK, 0 );
        }

    }

    return true;
}

// called when a monster die [give exp/give extra drop]
bool CMonster::OnDie( )
{
    CMap* map = GServer->MapList.Index[Position->Map];

    //LMA begin
    //CF mode 1
    //20070621-211100
    UINT special_exp=0;
    UINT special_lvl=0;


    if (map->is_cf==1)
    {
       //what monster is dead?
       if(this->montype==map->id_def_mon)
       {
           //oh my god, they killed a j&b !! (or the monster with exp)
           special_exp=(UINT) map->mon_exp;
           special_lvl= (UINT) map->mon_lvl;
           Log(MSG_INFO,"special_exp %i, special lvl %i",special_exp,special_lvl);
       }
       else
       {
           if (this->montype!=map->id_temp_mon)
           {
                UINT gs = GServer->RandNumber( 0, 100 );
                if(gs<map->percent)
                {
                  //we use the temporary monster as a decoy.
                  fPoint position_cf = GServer->RandInCircle( Position->current, 50 );
                  CMonster* monster2 = map->AddMonster( map->id_temp_mon, position_cf, 0, NULL, NULL, 0, true );
                }

           }

       }

    }

    GServer->GiveExp( this , special_lvl, special_exp );
    //LMA End


    return true;
}
// called when we create a new monster [attack players?]
bool CMonster::OnSpawn( bool Attack )
{
     lastSighCheck = clock();
    if(IsGhost( ) && Attack )
    {
        CPlayer* player = GetNearPlayer( );
        if(player==NULL) // no players or too far
            return true;
        StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
    }

    return true;
}

// [not]called when a monster is almost dead [run/use only skills/call a healer? :S xD]
bool CMonster::OnAlmostDie( )
{
    return true;
}

// called when a enemy is on sight
bool CMonster::OnEnemyOnSight( CPlayer* Enemy )
{
    clock_t etime = clock() - lastSighCheck;
    if(etime<5000) return true;
    if(!IsOnBattle( ))
    {
        if(thisnpc->aggresive>1)
        {
            UINT aggro = GServer->RandNumber(2,15);
            if(thisnpc->aggresive>=aggro && !IsGhostSeed( ))
            {
                //osprose.
                /*
                Enemy->ClearObject( this->clientid );
				SpawnMonster(Enemy, this );
				*/
                StartAction( (CCharacter*) Enemy, NORMAL_ATTACK, 0 );
            }
            else
            if(IsGhostSeed( ) || thisnpc->aggresive>5)
                MoveTo( Enemy->Position->current, true );
        }
        lastSighCheck = clock();
    }
    return true;
}

// called when enemy die
bool CMonster::OnEnemyDie( CCharacter* Enemy )
{
    Log(MSG_INFO,"An enemy died, let's stop battle");
    Position->destiny = Position->source; //ON MOB DIE
    ClearBattle( Battle );
    MoveTo( Position->source );
    return true;
}

bool CMonster::OnFar( )
{
    //if Owner too far away, we kill the bonfire.
    if(IsBonfire())
    {
       UnspawnMonster( );
       return true;
    }

    //osprose.
    if(!IsSummon())return true;

    Position->destiny = Position->source; //ON TARGET LOST
    ClearBattle( Battle );
    MoveTo( Position->source );
    return true;
}


//LMA: Arnold code for guardiantree
bool CMonster::Guardiantree(CMonster* monster,CMap* map)
{
     if(Stats->HP>9000)monster->hitcount=0;
     switch (monster->hitcount)
     {
         case 0:
              if(Stats->HP<9000)
              {
                  for(unsigned char i=0;i<5;i++){
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      map->AddMonster( 210, position, 0, NULL, NULL, 0, true );
                  }
                  monster->hitcount=1;
              }
              break;
         case 1:
              if(Stats->HP<8000)
              {
                  for(unsigned char i=0;i<5;i++){
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      map->AddMonster( 210, position, 0, NULL, NULL, 0, true );
                  }
                  monster->hitcount=2;
              }
              break;
         case 2:
              if(Stats->HP<7000)
              {
                  for(unsigned char i=0;i<5;i++){
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      map->AddMonster( 210, position, 0, NULL, NULL, 0, true );
                  }
                  monster->hitcount=3;
              }
              break;
         case 3:
              if(Stats->HP<6000)
              {
                  for(unsigned char i=0;i<5;i++){
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      map->AddMonster( 210, position, 0, NULL, NULL, 0, true );
                  }
                  monster->hitcount=4;
              }
              break;
         case 4:
              if(Stats->HP<5000)
              {
                  for(unsigned char i=0;i<5;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      map->AddMonster( 210, position, 0, NULL, NULL, 0, true );
                  }
                  monster->hitcount=5;
              }
              break;
         case 5:
              if(Stats->HP<4000)
              {
                  for(unsigned char i=0;i<5;i++){
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      map->AddMonster( 210, position, 0, NULL, NULL, 0, true );
                  }
                  monster->hitcount=6;
              }
              break;
         case 6:
              if(Stats->HP<3000)
              {
                  for(unsigned char i=0;i<5;i++){
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      map->AddMonster( 210, position, 0, NULL, NULL, 0, true );
                  }
                  monster->hitcount=7;
              }
              break;
         case 7:
              if(Stats->HP<2000)
              {
                  for(unsigned char i=0;i<5;i++){
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      map->AddMonster( 210, position, 0, NULL, NULL, 0, true );
                  }
                  monster->hitcount=8;
              }
              break;
         default:
                 monster->hitcount=9;

     }
}

//LMA: Handling Moonchild siblings
bool CMonster::MoonChild(CMonster* monster,CMap* map)
{
     if (monster->hitcount>=monster->maxhitcount)
        return true;

     if(Stats->HP>(Stats->MaxHP*0.98))
         monster->hitcount=0;

     CPlayer* player = NULL;


     switch (monster->hitcount)
     {
         case 0:
              if(Stats->HP<(Stats->MaxHP*0.98))
              {
                  Log(MSG_INFO,"case 0 for the MC");
                  for(unsigned char i=0;i<2;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      CMonster* monster2=map->AddMonster( 657, position, 0, NULL, NULL, 0, true );

                      if(i==0)
                         player = monster2->GetNearPlayer(20);    //getting near player.

                     if(player!=NULL)
                         monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );

                  }
                  for(unsigned char i=0;i<3;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      CMonster* monster2=map->AddMonster( 658, position, 0, NULL, NULL, 0, true );

                     if(player!=NULL)
                         monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );

                  }

                  monster->hitcount=1;
              }
              break;
         case 1:
              if(Stats->HP<(Stats->MaxHP*0.80))
              {
                  Log(MSG_INFO,"case 1 for the MC");
                  for(unsigned char i=0;i<3;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      CMonster* monster2=map->AddMonster( 657, position, 0, NULL, NULL, 0, true );

                      if(i==0)
                         player = monster2->GetNearPlayer(20);    //getting near player.

                     if(player!=NULL)
                         monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
                  }
                  for(unsigned char i=0;i<4;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      CMonster* monster2=map->AddMonster( 658, position, 0, NULL, NULL, 0, true );

                     if(player!=NULL)
                         monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );

                  }
                  monster->hitcount=2;
              }
              break;
         case 2:
              if(Stats->HP<(Stats->MaxHP*0.70))
              {
                  Log(MSG_INFO,"case 2 for the MC");
                  for(unsigned char i=0;i<10;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      CMonster* monster2=map->AddMonster( 657, position, 0, NULL, NULL, 0, true );

                      if(i==0)
                         player = monster2->GetNearPlayer(20);    //getting near player.

                     if(player!=NULL)
                         monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );

                  }
                  for(unsigned char i=0;i<5;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      CMonster* monster2=map->AddMonster( 658, position, 0, NULL, NULL, 0, true );

                     if(player!=NULL)
                         monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );

                  }
                  monster->hitcount=monster->maxhitcount;
              }
              break;

         default:
                 monster->hitcount=monster->maxhitcount;

     }


     return true;
}

//LMA: Handling Worm Dragon :)
bool CMonster::WormDragon(CMonster* monster,CMap* map)
{
     if (monster->hitcount>=monster->maxhitcount)
        return true;

     if(Stats->HP>(Stats->MaxHP*0.50))
         return true;

     CPlayer* player = NULL;
     int nb_worms =0;
     nb_worms=GServer->RandNumber(2,4);

      Log(MSG_INFO,"case 0 for the Worm Dragon (%i)",nb_worms);
      for(unsigned char i=0;i<nb_worms;i++)
      {
          fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
          CMonster* monster2=map->AddMonster( 202, position, 0, NULL, NULL, 0, true );

          if(i==0)
             player = monster2->GetNearPlayer(20);    //getting near player.

         if(player!=NULL)
             monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
      }

      monster->hitcount=monster->maxhitcount;


     return true;
}

//LMA: added by rl2171 Handling Cursed Ant Vagabond :)
bool CMonster::AntVagabond(CMonster* monster,CMap* map)
{
     if (monster->hitcount>=monster->maxhitcount)
        return true;

     if(Stats->HP>(Stats->MaxHP*0.50))
         return true;

     CPlayer* player = NULL;
     int nb_destroyers =0;
     nb_destroyers=GServer->RandNumber(1,2);

      Log(MSG_INFO,"case 0 for the Cursed Ant Vagabond (%i)",nb_destroyers);
      for(unsigned char i=0;i<nb_destroyers;i++)
      {
          fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
          CMonster* monster2=map->AddMonster( 1567, position, 0, NULL, NULL, 0, true );

          if(i==0)
             player = monster2->GetNearPlayer(20);    //getting near player.

         if(player!=NULL)
             monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
      }

      monster->hitcount=monster->maxhitcount;


     return true;
}

///LMA: added by rl2171 Handling Dragon Eggs :)
bool CMonster::DragonEgg(CMonster* monster,CMap* map)
{
     if (monster->hitcount>=monster->maxhitcount)
        return true;

     if(Stats->HP>(Stats->MaxHP*0.98))
         monster->hitcount=0;

     CPlayer* player = NULL;


     switch (monster->hitcount)
     {
         case 0:
              if(Stats->HP<(Stats->MaxHP*0.98))
              {
                  Log(MSG_INFO,"case 0 for the Dragon Egg");
                  for(unsigned char i=0;i<2;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      CMonster* monster2=map->AddMonster( 663, position, 0, NULL, NULL, 0, true );

                      if(i==0)
                         player = monster2->GetNearPlayer(20);    //getting near player.

                     if(player!=NULL)
                         monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );

                  }

                  monster->hitcount=1;
              }
              break;
         case 1:
              if(Stats->HP<(Stats->MaxHP*0.80))
              {
                  Log(MSG_INFO,"case 1 for the Dragon Egg");
                  for(unsigned char i=0;i<3;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      CMonster* monster2=map->AddMonster( 663, position, 0, NULL, NULL, 0, true );

                      if(i==0)
                         player = monster2->GetNearPlayer(20);    //getting near player.

                     if(player!=NULL)
                         monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
                  }
                  monster->hitcount=2;
              }
              break;
         case 2:
              if(Stats->HP<(Stats->MaxHP*0.70))
              {
                  Log(MSG_INFO,"case 2 for the Dragon Egg");
                  for(unsigned char i=0;i<2;i++)
                  {
                      fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
                      CMonster* monster2=map->AddMonster( 663, position, 0, NULL, NULL, 0, true );

                      if(i==0)
                         player = monster2->GetNearPlayer(20);    //getting near player.

                     if(player!=NULL)
                         monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );

                  }
                  monster->hitcount=monster->maxhitcount;
              }
              break;

         default:
                 monster->hitcount=monster->maxhitcount;

     }


     return true;
}

/*bool CMonster::DragonEgg(CMonster* monster,CMap* map)
{
     if (monster->hitcount>=monster->maxhitcount)
        return true;

     if(Stats->HP>(Stats->MaxHP*0.60))
         return true;

     CPlayer* player = NULL;
     int nb_egg =0;
     nb_egg=GServer->RandNumber(2,4);

      Log(MSG_INFO,"case 0 for the Dragon Eggs (%i)",nb_egg);
      for(unsigned char i=0;i<nb_egg;i++)
      {
          fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
          CMonster* monster2=map->AddMonster( 663, position, 0, NULL, NULL, 0, true );

          if(i==0)
             player = monster2->GetNearPlayer(20);    //getting near player.

         if(player!=NULL)
             monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
      }

      monster->hitcount=monster->maxhitcount;


     return true;
}
*/
//LMA: added by rl2171 Handling 1st Turak :)
bool CMonster::Turak1(CMonster* monster,CMap* map)
{
     if (monster->hitcount>=monster->maxhitcount)
        return true;

     if(Stats->HP>(Stats->MaxHP*0.00))
         return true;

     CPlayer* player = NULL;
     int nb_turak1 =0;
     nb_turak1=GServer->RandNumber(1,1);

      Log(MSG_INFO,"case 0 for the 1st Turak (%i)",nb_turak1);
      for(unsigned char i=0;i<nb_turak1;i++)
      {
          fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
          CMonster* monster2=map->AddMonster( 559, position, 0, NULL, NULL, 0, true );

          if(i==0)
             player = monster2->GetNearPlayer(20);    //getting near player.

         if(player!=NULL)
             monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
      }

      monster->hitcount=monster->maxhitcount;


     return true;
}

//LMA: added by rl2171 Handling 2nd Turak :)
bool CMonster::Turak2(CMonster* monster,CMap* map)
{
     if (monster->hitcount>=monster->maxhitcount)
        return true;

     if(Stats->HP>(Stats->MaxHP*0.00))
         return true;

     CPlayer* player = NULL;
     int nb_turak2 =0;
     nb_turak2=GServer->RandNumber(1,1);

      Log(MSG_INFO,"case 0 for the 2nd Turak (%i)",nb_turak2);
      for(unsigned char i=0;i<nb_turak2;i++)
      {
          fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
          CMonster* monster2=map->AddMonster( 560, position, 0, NULL, NULL, 0, true );

          if(i==0)
             player = monster2->GetNearPlayer(20);    //getting near player.

         if(player!=NULL)
             monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
      }

      monster->hitcount=monster->maxhitcount;


     return true;
}

/*
//LMA: added by rl2171 Handling 3rd Turak :)
bool CMonster::Turak3(CMonster* monster,CMap* map)
{
     if (monster->hitcount>=monster->maxhitcount)
        return true;

     if(Stats->HP>(Stats->MaxHP*0.50))
         return true;

     CPlayer* player = NULL;
     int nb_turak3 =0;
     nb_turak3=GServer->RandNumber(1,1);

      Log(MSG_INFO,"case 0 for the 3rd Turak (%i)",nb_turak3);
      for(unsigned char i=0;i<nb_turak3;i++)
      {
          fPoint position = GServer->RandInCircle( monster->Position->current, 5 );
          CMonster* monster2=map->AddMonster( 559, position, 0, NULL, NULL, 0, true );

          if(i==0)
             player = monster2->GetNearPlayer(20);    //getting near player.

         if(player!=NULL)
             monster2->StartAction( (CCharacter*)player, NORMAL_ATTACK, 0 );
      }

      monster->hitcount=monster->maxhitcount;


     return true;
}

*/

//osprose's summon update (old code).
/*
bool CMonster::SummonUpdate(CMonster* monster, CMap* map, UINT j)
{
     BEGINPACKET( pak, 0x799 );
 	clock_t etime = clock() - lastDegenTime;
    CPlayer* ownerclient = monster->GetOwner( );
    if( etime >= 4 * CLOCKS_PER_SEC && Stats->HP > 0 )
    {
        Stats->HP -= 2;
        lastDegenTime = clock();
        if (Stats->HP < 1||ownerclient == NULL||ownerclient->IsDead())//orphan check
        {
            //he's dead.
            Log(MSG_INFO,"Summon should be dead now");
            ADDWORD    ( pak, monster->clientid );
            ADDWORD    ( pak, monster->clientid );
            ADDDWORD   ( pak, monster->thisnpc->hp*monster->thisnpc->level );
            ADDDWORD   ( pak, 16 );
            GServer->SendToVisible( &pak, monster );
            map->DeleteMonster( monster, true, j );
            return true;
        }
        else
        if (montype > 800 && montype < 811)
        {
//			CPlayer* ownerclient = monster->GetOwner( );
//            float distance = 0x9;
//            for(UINT i=0;i<GServer->MapList.Index[Position->Map]->PlayerList.size();i++)
//            {
//                CPlayer* thisclient = GServer->MapList.Index[Position->Map]->PlayerList.at(i);
//                float tempdist = GServer->distance( Position->current, thisclient->Position->current );
//                if(tempdist<distance)
//                {
//                    thisclient->Stats->HP += ((((montype-800)*5)+((ownerclient->Stats->Level)/10))+10);
//                    thisclient->Stats->MP += (((montype-800)*5)+((ownerclient->Stats->Level)/25));
//                    BEGINPACKET( pak, 0x7b2);
//                    ADDWORD    ( pak, monster->clientid );
//                    ADDWORD    ( pak, 0x0b9d );
//                    ADDBYTE    ( pak, 0x06 );
//                    thisclient->client->SendPacket(&pak);
//
//                    RESETPACKET( pak, 0x7b5);
//                    ADDWORD    ( pak, thisclient->clientid );
//                    ADDWORD    ( pak, monster->clientid );
//                    ADDWORD    ( pak, 0x0b9d+0x3000+((ownerclient->Stats->Level%4)*0x4000) );
//                    ADDBYTE    ( pak, ownerclient->Stats->Level/4 );
//                    thisclient->client->SendPacket(&pak);
//
//                    RESETPACKET( pak, 0x7b9);
//                    ADDWORD    ( pak, monster->clientid );
//                    ADDWORD    ( pak, 0x0b9d );
//                    thisclient->client->SendPacket(&pak);
//                }
//            }
        }
    }
    else
    if (Stats->HP < 1)
    {
        Log(MSG_INFO,"Summon should be dead now 2");
        ADDWORD    ( pak, monster->clientid );
        ADDWORD    ( pak, monster->clientid );
        ADDDWORD   ( pak, monster->thisnpc->hp*monster->thisnpc->level );
        ADDDWORD   ( pak, 16 );
        GServer->SendToVisible( &pak, monster );
        map->DeleteMonster( monster, true, j );
    }

    return true;
}
*/

//osprose's summon update.
//LMA: adding the new life_time for summons :)
bool CMonster::SummonUpdate(CMonster* monster, CMap* map, UINT j)
{
    UINT die_amount=0;
 	clock_t etime = clock() - lastDegenTime;
    CPlayer* ownerclient = monster->GetOwner( );

    if( etime >= 4 * CLOCKS_PER_SEC && Stats->HP > 0 )
    {
        if(monster->life_time>0)
        {
            die_amount=(UINT)(monster->Stats->MaxHP/monster->life_time);
            //Log(MSG_INFO,"die amount for %i is %u",monster->montype,die_amount);
        }
        else
        {
            die_amount=2;
            //Log(MSG_INFO,"DEFAULT die amount for %i is %u",monster->montype,die_amount);
        }

        Stats->HP -= (die_amount*4);    //*4 because each 4 seconds ;)
        lastDegenTime = clock();
        if (Stats->HP < 1||ownerclient == NULL||ownerclient->IsDead())//orphan check
        {
            //he's dead.
            //Log(MSG_INFO,"Summon should be dead now");
            BEGINPACKET( pak, 0x799 );
            ADDWORD    ( pak, monster->clientid );
            ADDWORD    ( pak, monster->clientid );
            ADDDWORD   ( pak, monster->thisnpc->hp*monster->thisnpc->level );
            ADDDWORD   ( pak, 16 );
            GServer->SendToVisible( &pak, monster );
            map->DeleteMonster( monster, true, j );
            return true;
        }
        else
        {
            if(Stats->HP >0)
            {
                //LMA: Trying to update real HP amount.
                BEGINPACKET( pak, 0x79f );
                ADDWORD    ( pak, monster->clientid );
                ADDDWORD   ( pak, Stats->HP );
                GServer->SendToVisible( &pak, monster );
            }

        }

    }
    else if (Stats->HP < 1)
    {
        //Log(MSG_INFO,"Summon should be dead now 2");
        BEGINPACKET( pak, 0x799 );
        ADDWORD    ( pak, monster->clientid );
        ADDWORD    ( pak, monster->clientid );
        ADDDWORD   ( pak, monster->thisnpc->hp*monster->thisnpc->level );
        ADDDWORD   ( pak, 16 );
        GServer->SendToVisible( &pak, monster );
        map->DeleteMonster( monster, true, j );
    }


    return true;
}

//LMA: AIP
void CMonster::DoAi(int ainumberorg,char type)//ainumber is monster->AI type is add=0 idle=1 attacking=2 attacked=3 after killing target=4 hp<1=5
{

    //LMA: does the monster have a special AIP?
    int ainumber=ainumberorg;
    if (sp_aip!=0)
    {
        ainumber=sp_aip;
        Log(MSG_INFO,"AIP overwritten from %i to %i for monster %i (cid %i)",ainumberorg,ainumber,montype,clientid);
    }

    CAip* script = NULL;
    int AIWatch = GServer->Config.AIWatch;
    int aiindex = (ainumber*0x10000)+(type*0x100);

    bool lma_debug=false;
    int nb_turns=0;

    /*
    if(ainumber==1249||ainumber==1830)
    {
        lma_debug=true;
        LogDebugPriority(3);
    }

    if (Position->Map==2&&ainumber==1805)
    {
        lma_debug=true;
        LogDebugPriority(3);
    }*/

    //LMA: findchar and nearchar set to NULL.
    nearChar=NULL;
    findChar=NULL;

    //LMA: New way, faster?
    while(GServer->AipListMap.find(aiindex)!=GServer->AipListMap.end())
    {
        nb_turns++;
        if (lma_debug)
            LogDebug("BEGIN%i CDT Turn %i",ainumber,nb_turns);

        script = GServer->AipListMap[aiindex];

        int success = AI_SUCCESS; //needs to be AI_SUCCESS otherwise would not perform conditionless actions
        int thisaction = 0;

        for (dword i = 0; i < script->ConditionCount; i++)
        {
            int command = script->Conditions[i]->opcode;
            //if (command > 30 || command < 0) continue;
            if (command > 32 || command < 0) continue;

            success = (*GServer->aiCondFunc[command])(GServer, this, script->Conditions[i]->data);
            if(ainumber == AIWatch)LogDebug( "aiCondition %03u returned %d", command, success);

            //LMA: Special case (quite a stupid one if you're asking me)
            if(command==20&&type==1&&success==AI_FAILURE)
            {
                //they tried to do a check target level on idle... Let's say it went ok...
                success=AI_SUCCESS;
            }

            if (success == AI_FAILURE)
            {
                if (lma_debug)
                {
                    LogDebug( "DoAI%i aiCondition %03u, %i/%i Failure.",ainumber,command,i,script->ConditionCount-1);
                }

                break;
            }
            else
            {
                if (lma_debug)
                {
                    LogDebug( "DoAI%i aiCondition %03u, %i/%i Success.",ainumber,command,i,script->ConditionCount-1);
                }

            }

        }

        if (lma_debug)
        {
            LogDebug("DoAI%i END CDT Turn %i",ainumber,nb_turns);
        }


        if (success == AI_SUCCESS)
        {
            if (lma_debug)
            {
                LogDebug("DoAI%i BEGIN ACT Turn %i",ainumber,nb_turns);
            }


            //LMA: Special case, if we have a LTB and a qsd trigger in the same script, we do the LTB first.
            if(script->offset_ltb!=-1&&script->offset_qsd_trigger!=-1)
            {
                //We do the LTB.
                int command = script->Actions[script->offset_ltb]->opcode;
                if (command > 38 || command < 0) continue;

                success = (*GServer->aiActFunc[command])(GServer, this, script->Actions[script->offset_ltb]->data);
                if(ainumber == AIWatch)LogDebug( "aiAction forced: %03u returned %d", command, success);

                if(lma_debug)
                {
                    LogDebug( "DoAI%i aiAction Forced: %03u returned %d, %i/%i",ainumber,command, success,script->offset_ltb,script->ActionCount-1);
                }

            }

            for (dword i = 0; i < script->ActionCount; i++)
            {
                if(script->offset_ltb!=-1&&script->offset_qsd_trigger!=-1&&i==script->offset_ltb)
                {
                    //We cancel the LTB, already done earlier.
                    LogDebug( "DoAI%i aiAction: We cancel %i since it should be already done",ainumber,i);
                    continue;
                }

                int command = script->Actions[i]->opcode;
                if (command > 38 || command < 0) continue;

                success = (*GServer->aiActFunc[command])(GServer, this, script->Actions[i]->data);
                if(ainumber == AIWatch)LogDebug( "aiAction: %03u returned %d", command, success);

                if(lma_debug)
                {
                    LogDebug( "DoAI%i aiAction: %03u returned %d, %i/%i",ainumber,command, success,i,script->ActionCount-1);
                }

            }

            if(success == AI_SUCCESS)
            {
                if(lma_debug)
                {
                    LogDebug("DoAI%i END ACT SUCCESS Turn %i",ainumber,nb_turns);
                }

                //LMA: Santa is special, he continues (Xmas Tree Spawning), same for some special others (gifts summoning)
                if(ainumber!=1205&&ainumber!=2048&&ainumber!=2049)
                {
                    if(lma_debug)
                    {
                        GServer->pakGMObjVar(NULL, 1249,2);
                        LogDebugPriority(4);
                    }

                    thisnpc->refNPC=0;
                    nearChar=NULL;
                    findChar=NULL;

                    return; //automatically return after performing the first successful action
                }

            }
            else
            {
              if(lma_debug)
              {
                    LogDebug("DoAI%i END ACT FAILURE Turn %i",ainumber,nb_turns);
              }

            }

        }

        aiindex++;
    }

    if(lma_debug)
    {
        GServer->pakGMObjVar(NULL, 1249,2);
        LogDebugPriority(4);
    }


    thisnpc->refNPC=0;
    nearChar=NULL;
    findChar=NULL;


    return;
}
