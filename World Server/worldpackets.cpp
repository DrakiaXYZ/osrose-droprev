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
#include "worldserver.h"

// Send Characters information
void CWorldServer::pakPlayer( CPlayer *thisclient )
{
    bool has_cart=false;
    bool has_cg=false;
    int nb_30=30;
    int nb_skills=0;
    bool is_dual_scratch=false;


	CRespawnPoint* thisrespawn = GetRespawnByID( thisclient->Position->respawn );
	if(thisrespawn!=NULL)
	{
        thisclient->Position->destiny = thisrespawn->dest;
    	thisclient->Position->current = thisclient->Position->destiny;
    	thisclient->Position->Map = thisrespawn->destMap;
    }
    else
    {
        thisclient->Position->destiny.x = 5300;
        thisclient->Position->destiny.y = 5200;
        thisclient->Position->current = thisclient->Position->destiny;
        thisclient->Position->Map = 2;
    }

    CMap* map = MapList.Index[thisclient->Position->Map];
    map->AddPlayer( thisclient );

    //LMA: we execute the map "welcome quest trigger", used to set pvp most of the time.
    if(!thisclient->is_born)
    {
        thisclient->pvp_id=-1;
        dword hash_zone=map->QSDzone;

        if(hash_zone>0)
        {
            //int success=thisclient->ExecuteQuestTrigger(hash_zone,false);
            int success=thisclient->ExecuteQuestTrigger(hash_zone,true);
            if (success==QUEST_SUCCESS)
            {
                //Log(MSG_WARNING,"successful first map %i trigger for %s",map->id,thisclient->CharInfo->charname);
            }
            else
            {
                Log(MSG_WARNING,"UNsuccessful first map %i trigger for %s",map->id,thisclient->CharInfo->charname);
            }

        }

        thisclient->is_born=true;
    }

    //LMA: Little fix if players comes with too much HP / MP In Game (exit when fairied?)
    if (thisclient->Stats->HP>thisclient->GetMaxHP())
       thisclient->Stats->HP=thisclient->GetMaxHP();
    if (thisclient->Stats->MP>thisclient->GetMaxMP())
       thisclient->Stats->MP=thisclient->GetMaxMP();

	//driving skills :)
	for (int i=60;i<60+MAX_DRIVING_SKILL;i++)
	{
	    if (thisclient->cskills[i].id==5000)
            has_cart=true;
	    if (thisclient->cskills[i].id==5001)
            has_cg=true;
        if (thisclient->cskills[i].id==0)
            break;
	}

    BEGINPACKET( pak, 0x715 );
    ADDBYTE    ( pak, thisclient->CharInfo->Sex );               // Sex
    ADDWORD    ( pak, thisclient->Position->Map );		         // Map
    ADDFLOAT   ( pak, thisclient->Position->current.x*100 );	 // Pos X
    ADDFLOAT   ( pak, thisclient->Position->current.y*100 );	 // Pos Y
    ADDWORD    ( pak, 0x0000 );
    ADDDWORD   ( pak, thisclient->CharInfo->Face );			     // Face
    ADDDWORD   ( pak, thisclient->CharInfo->Hair );	             // Hair
    ADDWORD    ( pak, thisclient->items[2].itemnum );	         // Cap
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[2] ) ); // Cap Refine
    ADDWORD    ( pak, thisclient->items[3].itemnum );	         // Body
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[3] ) ); // Body Refine
    ADDWORD    ( pak, thisclient->items[5].itemnum );	         // Gloves
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[5] ) ); // Gloves Refine
    ADDWORD    ( pak, thisclient->items[6].itemnum );	         // Boots
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[6] ));	 // Boots Refine
    ADDWORD    ( pak, thisclient->items[1].itemnum );	         // Face
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[1] ) ); // Face Refine
    ADDWORD    ( pak, thisclient->items[4].itemnum );	         // Back
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[4] ) ); // Back Refine
    ADDWORD    ( pak, thisclient->items[7].itemnum );	         // Weapon
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[7] ) ); // Weapon Refine
    ADDWORD    ( pak, thisclient->items[8].itemnum );	         // SubWeapon
    ADDWORD    ( pak, BuildItemRefine( thisclient->items[8] ) ); // SubWeapon Refine
    ADDBYTE    ( pak, 0 );
    ADDWORD    ( pak, 0x140f );
	ADDWORD    ( pak, thisclient->CharInfo->Job );			     // Job
    ADDBYTE    ( pak, thisclient->CharInfo->unionid );           //LMA: Union ID

    ADDBYTE( pak, 0 );                                           //LMA: Union Fame
    ADDBYTE( pak, thisclient->CharInfo->unionfame );             //LMA: Union Fame

    ADDWORD    ( pak, thisclient->Attr->Str );			         // Str
    ADDWORD    ( pak, thisclient->Attr->Dex );				     // Dex
    ADDWORD    ( pak, thisclient->Attr->Int );				     // Int
    ADDWORD    ( pak, thisclient->Attr->Con );				     // Con
    ADDWORD    ( pak, thisclient->Attr->Cha );				     // Cha
    ADDWORD    ( pak, thisclient->Attr->Sen );				     // Sen
	ADDWORD    ( pak, thisclient->Stats->HP );                   // Current HP
	ADDWORD    ( pak, thisclient->Stats->MP );                   // Current MP
	ADDWORD    ( pak, thisclient->CharInfo->Exp );               // Exp
    ADDWORD    ( pak, 0 );
	ADDWORD    ( pak, thisclient->Stats->Level );			      // Level
	ADDWORD    ( pak, thisclient->CharInfo->StatPoints );		  // Stat Points
	ADDWORD    ( pak, thisclient->CharInfo->SkillPoints );        // Skill Points
	ADDWORD    ( pak, 0x6464 );
    for(int i=0; i<4; i++)
       ADDWORD( pak,0);       //null

    //LMA: Union points
    ADDWORD( pak, thisclient->CharInfo->union01 );
    ADDWORD( pak, thisclient->CharInfo->union02 );
    ADDWORD( pak, thisclient->CharInfo->union03 );
    ADDWORD( pak, thisclient->CharInfo->union04 );
    ADDWORD( pak, thisclient->CharInfo->union05 );

    //rest is 0?
	for(int i=0; i<19; i++)
        ADDBYTE( pak, 0 );    //null

    ADDWORD( pak, thisclient->CharInfo->stamina );					// Stamina
    for(int i=0; i<320; i++) ADDBYTE( pak, 0 );

    //LMA: Value for driving skill so it doesn't say missing conditions ?
    if (has_cart||has_cg)
    {
        ADDWORD( pak, 3500 );
    }
    else
    {
        ADDWORD( pak, 0 );
    }

    ADDWORD( pak, 0 );
    ADDWORD( pak, 0 );

    //LMA: test 2008/11/07 (reorganisation)
 	for(int i=0; i<MAX_CLASS_SKILL; i++) // Class Skills
 	{
 	    if(thisclient->cskills[i].id==0)
 	    {
 	        ADDWORD( pak,0);
 	        continue;
 	    }

        ADDWORD( pak, thisclient->cskills[i].id+thisclient->cskills[i].level-1 );
 	}

    //Driving skill (and some stuff)
	for (int i=60;i<60+MAX_DRIVING_SKILL;i++)
	{
        ADDWORD( pak, thisclient->cskills[i].id );
	}

    //Unique
    for(int i=90; i<90+MAX_UNIQUE_SKILL; i++)
    {
 	    if(thisclient->cskills[i].id==0)
 	    {
 	        ADDWORD( pak,0);
 	        continue;
 	    }

        ADDWORD( pak, thisclient->cskills[i].id+thisclient->cskills[i].level-1 );
    }

    //Mileage
    for(int i=120; i<120+MAX_MILEAGE_SKILL; i++)
    {
 	    if(thisclient->cskills[i].id==0)
 	    {
 	        ADDWORD( pak,0);
 	        continue;
 	    }

        ADDWORD( pak, thisclient->cskills[i].id+thisclient->cskills[i].level-1 );
    }

    //Basic Skills
	for(int i=320; i<320+MAX_BASIC_SKILL; i++)
	{
        ADDWORD( pak, thisclient->cskills[i].id);
	}

	for(int i=0; i<48; i++)       // QuickBar
        ADDWORD( pak, thisclient->quickbar[i] );
	ADDDWORD   ( pak, thisclient->CharInfo->charid );	            // CharID
	for(int i=0; i<80;i++) ADDBYTE( pak, 0 );
    ADDSTRING  ( pak, thisclient->CharInfo->charname );             // Char Name
	ADDBYTE    ( pak, 0 );
    thisclient->client->SendPacket( &pak );
}

// Send Inventory Information
void CWorldServer::pakInventory( CPlayer *thisclient )
{
	BEGINPACKET( pak, 0x716 );
	ADDQWORD( pak, thisclient->CharInfo->Zulies );
	//ADDWORD( pak, 0 );
	for(unsigned j=0; j<140; j++)
    {
        ADDDWORD( pak, BuildItemHead( thisclient->items[j] ) );
       	ADDDWORD( pak, BuildItemData( thisclient->items[j] ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
	}

    thisclient->client->SendPacket( &pak );


    return;
}

// Get this user set up with the encryption and stuff
bool CWorldServer::pakDoIdentify( CPlayer *thisclient, CPacket *P )
{
	MYSQL_ROW row;
	thisclient->Session->userid = GETDWORD((*P), 0);
	memcpy( thisclient->Session->password, &P->Buffer[4], 32 );
	MYSQL_RES *result = DB->QStore("SELECT username,lastchar,accesslevel,zulystorage FROM accounts WHERE id=%i AND password='%s'", thisclient->Session->userid, thisclient->Session->password);
    if(result==NULL) return false;
	if (mysql_num_rows( result ) != 1)
    {
		Log( MSG_HACK, "Someone tried to connect to world server with an invalid account" );
		DB->QFree( );
		return false;
	}
	row = mysql_fetch_row(result);
	strncpy( thisclient->Session->username, row[0],16 );
	strncpy( thisclient->CharInfo->charname, row[1],16 );
	thisclient->Session->accesslevel = atoi(row[2]);
	//thisclient->CharInfo->Storage_Zulies = atoi( row[3] );
	thisclient->CharInfo->Storage_Zulies = atoll( row[3] );
	DB->QFree( );
	if(!thisclient->loaddata( )) return false;
	Log( MSG_INFO, "User '%s'(#%i) logged in with character '%s'", thisclient->Session->username, thisclient->Session->userid, thisclient->CharInfo->charname);
	BEGINPACKET( pak, 0x070c );
	ADDBYTE    ( pak, 0 );
	ADDDWORD   ( pak, 0x87654321 );
	ADDDWORD   ( pak, 0x00000000 );
	thisclient->client->SendPacket( &pak );
    pakPlayer(thisclient);
    pakInventory(thisclient);
    pakQuestData(thisclient);

    //LMA: Jrose unlimited (zrose)
    RESETPACKET( pak, 0x7de );
    ADDWORD ( pak, 0x1001 ); // 0x1001 to 0x1013 (game plan?)
    ADDDWORD ( pak, 2 ); // options (plan time?) [2 = unlimited]
    thisclient->client->SendPacket( &pak );

	RESETPACKET( pak, 0x702 );
	ADDSTRING  ( pak, Config.WELCOME_MSG );
	ADDBYTE    ( pak, 0 );
	thisclient->client->SendPacket( &pak );

	//SendSysMsg( thisclient, "Open Source Rose Online Private Server" );

	thisclient->SetStats( );
	return true;
}

// Give the user an ID
bool CWorldServer::pakDoID( CPlayer* thisclient, CPacket* P )
{
	// Assign a new id to this person
   	thisclient->clientid = GetNewClientID();
	if (thisclient->clientid <=1)
    {
		Log( MSG_WARNING, "User '%s'(#%i) denied access. Server is full.", thisclient->Session->username, thisclient->Session->userid );
		return false;
	}

	if(thisclient->Stats->HP<=0)
        thisclient->Stats->HP=10*thisclient->GetMaxHP( )/100;

	Log( MSG_INFO, "User '%s'(#%i) assigned id #%i", thisclient->Session->username, thisclient->Session->userid, thisclient->clientid );
    if( thisclient->Party->party )
    {
        BEGINPACKET( pak, 0x7d5 );
        ADDDWORD   ( pak, thisclient->CharInfo->charid );
        ADDWORD    ( pak, thisclient->clientid );
        ADDWORD    ( pak, thisclient->GetMaxHP( ) );
        ADDWORD    ( pak, thisclient->Stats->HP );
        ADDDWORD   ( pak, 0x01000000 );
        ADDDWORD   ( pak, 0x0000000f );
        ADDWORD    ( pak, 0x1388 );
        thisclient->Party->party->SendToMembers( &pak, thisclient );
    }

    //LMA: We do the warp zone here.
    CMap* map = MapList.Index[thisclient->Position->Map];
    //LMA: We do a quest trigger if needed...
    //If we are already in the map, no need to redo all the stuff again.
    //We don't do the qsdzone again if in the UW map...
    dword hash_zone=map->QSDzone;
    //if(hash_zone>0&&((id!=thisclient->Position->Map)||(id!=9)))

    if(thisclient->skip_qsd_zone)
    {
        hash_zone=0;
        thisclient->skip_qsd_zone=false;
    }

    if(hash_zone>0)
    {
        //LMA: we set the pvp vars to nothing again.
        thisclient->pvp_id=-1;
        thisclient->pvp_status=-1;

        //int success=thisclient->ExecuteQuestTrigger(hash_zone,false);
        int success=thisclient->ExecuteQuestTrigger(hash_zone,true);
        if (success==QUEST_SUCCESS)
        {
            //Log(MSG_WARNING,"teleporting player %s, quest %u returned ok",thisclient->CharInfo->charname,hash_zone);

            //Special case for map 9 (UW).
            if(map->id==9)
            {
                if(thisclient->UWPosition->Map>0&&thisclient->UWPosition->source.x>0&&thisclient->UWPosition->source.y>0)
                {
                    Log(MSG_WARNING,"Player %s is UW teleporting to map %i (%.2f,%.2f)",thisclient->CharInfo->charname,thisclient->UWPosition->Map,thisclient->UWPosition->source.x,thisclient->UWPosition->source.y);
                }

            }

        }
        else
        {
            Log(MSG_WARNING,"teleporting player %s, quest %u returned false",thisclient->CharInfo->charname,hash_zone);
        }

    }
    //LMA: end of zone

    BEGINPACKET( pak, 0x753 );
    ADDWORD    ( pak, thisclient->clientid );			// USER ID
    ADDWORD    ( pak, thisclient->Stats->HP );		// CURRENT HP
    ADDWORD    ( pak, thisclient->Stats->MP );		// CURRENT MP
    ADDDWORD   ( pak, thisclient->CharInfo->Exp );				// CURRENT EXP
    ADDDWORD   ( pak, 0x00000000 );						// LVL EXP (UNSUSED)
    // thanks to StrikeX to post this source
        //[economy]
    ADDWORD    ( pak, 0x0063 );  // World Rate
    ADDBYTE    ( pak, 0x70 );
    ADDBYTE    ( pak, 0x69 );
    ADDBYTE    ( pak, 0x68 );
    ADDBYTE    ( pak, 0x67 );
    ADDWORD    ( pak, 0x0062 );  // Town rate
    ADDBYTE    ( pak, 0x61 );  // misc rate
    ADDBYTE    ( pak, 0x32 );  //1
    ADDBYTE    ( pak, 0x32 );  //2
    ADDBYTE    ( pak, 0x32 );  //3
    ADDBYTE    ( pak, 0x32 );  //4
    ADDBYTE    ( pak, 0x32 );  //5
    ADDBYTE    ( pak, 0x32 );  //6
    ADDBYTE    ( pak, 0x32 );  //7
    ADDBYTE    ( pak, 0x32 );  //8
    ADDBYTE    ( pak, 0x32 );  //9
    ADDBYTE    ( pak, 0x32 );  //10
    ADDBYTE    ( pak, 0x32 );  //11

    //CMap* map = MapList.Index[thisclient->Position->Map];

    //LMA: new pvp code.
    if(map->allowpvp!=1)
    {
        //non player vs player map
        ADDWORD(pak, 0x0000);
    }
    else
    {
        //player vs player map
        ADDWORD(pak, 0x0001);
    }

    ADDWORD    (pak, 0x0000 );

    // Map Time
    ADDDWORD( pak, map->MapTime );

    thisclient->pvp_id=thisclient->ReturnPvp(NULL,thisclient);
    ADDWORD(pak,thisclient->pvp_id);

    //LMA: Update.
    ADDWORD(pak, 0x0000 );
    thisclient->client->SendPacket( &pak );
    // set weight
    RESETPACKET( pak, 0x762 );
    ADDWORD    ( pak, thisclient->clientid );       	// USER ID
    ADDBYTE    ( pak, 1 );								// SOMETHING TO DO WITH WEIGHT
    thisclient->client->SendPacket( &pak );
    // set speed
	RESETPACKET(pak, 0x782 );
	ADDWORD    ( pak, thisclient->clientid );
	ADDBYTE    ( pak, thisclient->Status->Stance );

	//LMA: Base Speed.
	//ADDWORD    ( pak, thisclient->Stats->Move_Speed );
	ADDWORD    ( pak, thisclient->Stats->Base_Speed);

	SendToVisible( &pak, thisclient );

	//Log(MSG_INFO,"Pak Player %s, stance %i base move_speed %i",thisclient->CharInfo->charname,thisclient->Status->Stance,thisclient->Stats->Base_Speed);


    thisclient->CleanPlayerVector( );
	thisclient->Session->inGame = true;
	thisclient->firstlogin=clock();    //LMA for fairy

	//LMA: In some special cases, we have to warp the guy again...
	if(thisclient->map_warp_zone!=0&&thisclient->Warp_Zone.x!=0&&thisclient->Warp_Zone.y!=0)
	{
	    CMap* mapWarp = MapList.Index[thisclient->map_warp_zone];
	    mapWarp->TeleportPlayer(thisclient,thisclient->Warp_Zone);
	    thisclient->map_warp_zone=0;
	    thisclient->Warp_Zone.x=0;
	    thisclient->Warp_Zone.y=0;
	}


	return true;
}

// Move Characters in map
bool CWorldServer::pakMoveChar( CPlayer* thisclient, CPacket* P )
{
    if( thisclient->Shop->open || (!thisclient->Ride->Drive && thisclient->Ride->Ride) || !thisclient->Status->CanMove || thisclient->Status->Stun != 0xff || thisclient->Status->Sleep != 0xff)
        return true;
    if( thisclient->Status->Stance==1 )
        thisclient->Status->Stance=3;
    ClearBattle( thisclient->Battle );
	thisclient->Battle->target = GETWORD((*P), 0x00 );
	thisclient->Position->destiny.x = GETFLOAT((*P), 0x02 )/100;
    thisclient->Position->destiny.y = GETFLOAT((*P), 0x06 )/100;
    thisclient->Position->lastMoveTime = clock();

    //LMA: Checking if we got fuel.
    if(thisclient->Status->Stance==DRIVING)
    {
        if(thisclient->items[136].itemnum==0)
        {
            Log(MSG_WARNING,"%s should be riding but doesn't have a motor?",thisclient->CharInfo->charname);
            return true;
        }

        if(thisclient->items[136].lifespan==0)
        {
            //No fuel anymore, can't move.
            return true;
        }

    }

    //LMA: Overweight player.
    if(thisclient->Status->Stance==RUNNING)
    {
        if(!thisclient->Status->CanRun)
        {
            thisclient->Status->Stance=WALKING;
            thisclient->Stats->Base_Speed=thisclient->GetMoveSpeed();
            thisclient->Stats->Move_Speed=thisclient->Stats->Base_Speed;
        }

    }

	BEGINPACKET( pak, 0x79a );
	ADDWORD    ( pak, thisclient->clientid );		// USER ID
	ADDWORD    ( pak, thisclient->Battle->target );		// TARGET

	//LMA: Base Speed
	//ADDWORD    ( pak, thisclient->Stats->Move_Speed );	// MSPEED
    ADDWORD    ( pak, thisclient->Stats->Base_Speed );	// MSPEED

	ADDFLOAT   ( pak, GETFLOAT((*P), 0x02 ) );	// POSITION X
	ADDFLOAT   ( pak, GETFLOAT((*P), 0x06 ) );	// POSITION Y
	ADDWORD    ( pak, GETWORD((*P), 0x0a ) );		// POSITION Z (NOT USED)
	SendToVisible( &pak, thisclient );

    if(thisclient->Position->Map==8)
    {
        Log(MSG_INFO,"pakMoveChar %s, (%.2f;%.2f) to (%.2f;%.2f) speed %u/%u",thisclient->CharInfo->charname,thisclient->Position->current.x,thisclient->Position->current.y,thisclient->Position->destiny.x,thisclient->Position->destiny.y,thisclient->Stats->Base_Speed,thisclient->Stats->Move_Speed);
    }

	return true;
}

// Stop Character movement
bool CWorldServer::pakStopChar( CPlayer* thisclient, CPacket* P )
{
    ClearBattle( thisclient->Battle );
	thisclient->Position->destiny.x = GETFLOAT((*P), 0x00 )/100;
    thisclient->Position->destiny.y = GETFLOAT((*P), 0x04 )/100;
	BEGINPACKET( pak, 0x770 );
	ADDWORD    ( pak, thisclient->clientid );		// USER ID
	ADDFLOAT   ( pak, thisclient->Position->destiny.x*100 );	// POSITION X
	ADDFLOAT   ( pak, thisclient->Position->destiny.y*100 );	// POSITION Y
	ADDWORD    ( pak, GETWORD((*P), 0x08 ) );		// POSITION Z (NOT USED)
	SendToVisible( &pak, thisclient );
	return true;
}

//LMA: Spawn Ifo Objects
bool CWorldServer::pakSpawnIfoObject( CPlayer* thisclient, UINT npctype,bool forcerefresh)
{
    if(npctype!=GServer->WarpGate.virtualNpctypeID)
        return true;

    //Credits to Maxxon for this one ;)
    BEGINPACKET(pak, 0x7d6);
    ADDWORD(pak, GServer->WarpGate.clientID);
    //ADDWORD(pak, 0x2022); // unused
    ADDBYTE(pak,GServer->WarpGate.IfoX);
    ADDBYTE(pak,GServer->WarpGate.IfoY);
    ADDWORD(pak, GServer->WarpGate.id); // object number
    ADDWORD(pak, GServer->WarpGate.IfoObjVar[0]); // event id. 0x0000 closed, 0x01 open

    //This can happen (see qsd Actions.
    if(thisclient!=NULL)
    {
        thisclient->client->SendPacket( &pak );
    }

    if (forcerefresh)
    {
        //we send to all people in map, just to be sure ^_^
        SendToAllInMap(&pak,WarpGate.mapid);
    }

    //Log(MSG_INFO,"Spawning WarpGate, does it appear? %i",GServer->WarpGate.IfoObjVar[0]);


	return true;
}


// Spawn NPC
bool CWorldServer::pakSpawnNPC( CPlayer* thisclient, CNPC* thisnpc )
{
	BEGINPACKET( pak, 0x791 );
    ADDWORD( pak, thisnpc->clientid );
	ADDFLOAT( pak, thisnpc->pos.x*100 );
    ADDFLOAT( pak, thisnpc->pos.y*100 );
    ADDFLOAT( pak, thisnpc->pos.x*100 );
    ADDFLOAT( pak, thisnpc->pos.y*100 );
	ADDBYTE( pak, 0x00 );
    ADDWORD( pak, 0x0000 );
	ADDWORD( pak, 0x0000 );
	ADDWORD( pak, 0x03e8 );
	ADDWORD( pak, 0x0000 );
	ADDWORD( pak, 0x0001 );
	ADDWORD( pak, 0x0000 );//Buffs
	ADDWORD( pak, 0x0000 );//Buffs
	ADDWORD( pak, 0x0000 );//buffs
	ADDWORD( pak, thisnpc->npctype );

	//LMA: Dialog time, or we send the dialogID (default one), or the tempdialogID (event for example).
	if (thisnpc->dialog!=0)
	{
        ADDWORD( pak, thisnpc->dialog );
        //Log(MSG_INFO,"Special dialog %i for NPC %i",thisnpc->dialog, thisnpc->npctype);
    }
    else if(thisnpc->thisnpc->dialogid!=0)
    {
         //Log(MSG_INFO,"Default dialog %i for NPC %i",thisnpc->thisnpc->dialogid, thisnpc->npctype);
         ADDWORD( pak, thisnpc->thisnpc->dialogid );
    }
    else
    {
        // NPC dialog should be type - 900, if not specified in DB
        ADDWORD( pak, thisnpc->npctype - 900 );
        //Log(MSG_INFO,"NO default or special dialog %i for NPC %i",thisnpc->npctype - 900, thisnpc->npctype);
    }

	ADDFLOAT( pak, thisnpc->dir );

    if (thisnpc->npctype == 1115)
    {
       ADDBYTE( pak, GServer->Config.Cfmode ) // Burtland Clan Field open/close
       ADDBYTE( pak, 0 );
    }

    //2do: error I guess, extra DWORD in the case of CF NPC.

    //Event:
    if (thisnpc->event!=0&&thisnpc->npctype!=1115)
    {
        //Log(MSG_INFO,"Event number %i for NPC %i",thisnpc->event, thisnpc->npctype);
        ADDWORD ( pak, thisnpc->event);
    }
    else if(thisnpc->npctype!=1115)
    {
        ADDWORD ( pak, 0 );
     }

    //ADDBYTE( pak, 0 );
    thisclient->client->SendPacket( &pak );
	return true;
}

// Changes stance
bool CWorldServer::pakChangeStance( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
	BYTE stancenum = GETBYTE((*P),0x00);
	BYTE previous_stance=thisclient->Status->Stance;
	//Log(MSG_INFO,"Changing stance from %i to %i",previous_stance,stancenum);

	if (stancenum == 0)
	{
        if(thisclient->Status->Stance == RUNNING) //Walking
        {
            stancenum = WALKING;
            thisclient->Status->Stance = WALKING;
        }
        else
        if(thisclient->Status->Stance != DRIVING) // Running
        {
            stancenum = RUNNING;
            thisclient->Status->Stance = RUNNING;
        }
    }
    else
	if (stancenum == 1)
    {
        if(thisclient->Status->Stance!=DRIVING && thisclient->Status->Stance>0x01) //
        {   // Walking
			thisclient->Status->Stance = 0x01;
        }
        else
        if(thisclient->Status->Stance<0x04)
        {   //Runing
			thisclient->Status->Stance = 0x03;
        }
    }
    else
	if (stancenum == 2)
    {
		if (thisclient->Status->Stance==RUNNING || thisclient->Status->Stance==0x02)
		{// // Driving
            // Clean Buffs
            stancenum = DRIVING;
			thisclient->Status->Stance = DRIVING;
            thisclient->Ride->Ride = false;
            thisclient->Ride->Drive = true;
            thisclient->Ride->charid = 0;
        }
		else
		{ // Running
            stancenum = RUNNING;
			thisclient->Status->Stance = RUNNING;
        }
	}
    else
		thisclient->Status->Stance = stancenum;
	if(!thisclient->Status->CanMove)
	   thisclient->Status->Stance = RUNNING;
	thisclient->Stats->Move_Speed = thisclient->GetMoveSpeed( );

	BEGINPACKET( pak, 0x782 );
	ADDWORD( pak, thisclient->clientid );
	ADDBYTE( pak, thisclient->Status->Stance );

    //LMA: putting the speed again...
    //ADDWORD( pak, thisclient->Stats->Move_Speed );
    ADDWORD( pak, thisclient->Stats->Base_Speed );

    SendToVisible( &pak, thisclient );

    //Fuel.
	if (thisclient->Status->Stance==DRIVING)
	{
       thisclient->last_fuel=clock();
       //forcing refresh for good value :)
       thisclient->TakeFuel();
    }

    if (previous_stance==DRIVING)
    {
       thisclient->TakeFuel();
       thisclient->last_fuel=0;
    }

   //LMA: eh eh ^_^
   //We force the refresh of the engine so the mspeed client side is done again.
   if(thisclient->Fairy&&(previous_stance==DRIVING||thisclient->Status->Stance==DRIVING))
   {
       int cartslot=1;
       int srcslot=136;
       int slot1=srcslot;
       int slot2=slot1;

       if(thisclient->items[slot1].itemnum==0)
       {
           return true;
       }

        RESETPACKET( pak, 0x718 );
        //if(slot2!=0xffff && slot2!=0xffff) {ADDBYTE( pak, 2 );}
        if(slot1!=0xffff && slot2!=0xffff) {ADDBYTE( pak, 2 );}
        else {ADDBYTE( pak, 1 );}
        if(slot1!=0xffff)
        {
            ADDBYTE    ( pak, slot1);
            ADDDWORD   ( pak, GServer->BuildItemHead( thisclient->items[slot1] ) );
            ADDDWORD   ( pak, GServer->BuildItemData( thisclient->items[slot1] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
        }
        if(slot2!=0xffff)
        {
            ADDBYTE    ( pak, slot2 );
            ADDDWORD   ( pak, GServer->BuildItemHead( thisclient->items[slot2] ) );
            ADDDWORD   ( pak, GServer->BuildItemData( thisclient->items[slot2] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
        }
        thisclient->client->SendPacket( &pak );

    /*
        RESETPACKET( pak, 0x7ca );
        ADDWORD    ( pak, thisclient->clientid );
        ADDWORD    ( pak, cartslot);
        ADDWORD    ( pak, thisclient->items[srcslot].itemnum);
        //LMA TEST:
        //ADDWORD    ( pak, lma_speed );
        ADDWORD    ( pak,0);

        //LMA: change?
        if(thisclient->Status->Stance==DRIVING)
        {
            //ADDWORD    ( pak, thisclient->Stats->Move_Speed );
            //ADDWORD    ( pak, thisclient->Stats->Base_Speed );
        }

        SendToVisible( &pak, thisclient );
        */
   }


	return true;
}

// Spawn Drop
bool CWorldServer::pakSpawnDrop( CPlayer* thisclient, CDrop* thisdrop )
{
	BEGINPACKET( pak, 0x7a6 );
	ADDFLOAT( pak, thisdrop->pos.x*100 );
	ADDFLOAT( pak, thisdrop->pos.y*100 );
	if (thisdrop->type==1)
    {
		// -- ZULY --
		ADDDWORD( pak, 0xccccccdf );
		ADDDWORD( pak, thisdrop->amount );
        ADDDWORD( pak, 0xcccccccc );
            ADDWORD ( pak, 0xcccc );
		ADDWORD( pak, thisdrop->clientid );

		//LMA: it's the owner.
		//ADDWORD( pak, 0x0000 );
		ADDWORD( pak, thisdrop->owner );

		ADDWORD( pak, 0x5f90 );
	}
    else
    {
		// -- ITEM --
		/*//LMA: bad way?
		ADDDWORD( pak, BuildItemHead( thisdrop->item ) );
		ADDDWORD( pak, thisdrop->amount );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
		ADDWORD( pak, thisdrop->clientid );
		ADDDWORD( pak, BuildItemData( thisdrop->item ) );*/

		//LMA: new way.
		ADDDWORD( pak, BuildItemHead( thisdrop->item ) );
		ADDDWORD( pak, BuildItemData( thisdrop->item ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
		ADDWORD( pak, thisdrop->clientid );
		ADDWORD( pak, thisdrop->owner );
		ADDWORD( pak, 0x5f90 );
		//LMA: End
	}
	thisclient->client->SendPacket( &pak );
	return true;
}

// Player Left
void CWorldServer::pakClearUser( CPlayer* thisclient )
{
	BEGINPACKET( pak, 0x794 );
	ADDWORD( pak, thisclient->clientid );
	SendToVisible( &pak, thisclient, false );
}

// Drop items on map
bool CWorldServer::pakDoDrop( CPlayer* thisclient, CPacket* P )
{
	BYTE itemid = GETBYTE((*P), 0x0);
	if(!CheckInventorySlot(thisclient, itemid ))
	   return false;
	DWORD amount = GETDWORD((*P), 0x1);
	if (itemid == 0)
    {
		if ( amount<1 ) return true;
		if ( thisclient->CharInfo->Zulies < amount ) return true;
		CDrop* thisdrop = new CDrop;
		assert(thisdrop);
		thisdrop->clientid = GetNewClientID();
		thisdrop->type = 1; // ZULY
		thisdrop->pos = RandInCircle( thisclient->Position->current, 3 );
		thisdrop->posMap = thisclient->Position->Map;
		thisdrop->droptime = time(NULL);
		thisdrop->amount = amount;
		thisdrop->owner = 0;
		CMap* map = MapList.Index[thisdrop->posMap];
		map->AddDrop( thisdrop );
		thisclient->CharInfo->Zulies -= amount;
		BEGINPACKET( pak, 0x71d );
		ADDQWORD( pak, thisclient->CharInfo->Zulies );
		thisclient->client->SendPacket( &pak );
	}
    else
    {
        // fixed by tomciaaa [item count drop correctly and dissaper from inventory]
        bool flag = false;
        if(thisclient->items[itemid].itemtype >=10 && thisclient->items[itemid].itemtype <= 13)
        {
            if(thisclient->items[itemid].count<amount) return true;
           thisclient->items[itemid].count -= amount;
           flag= (thisclient->items[itemid].count <= 0);
        }
        else
        {
           flag= true;
        }
		CDrop* thisdrop = new CDrop;
		assert(thisdrop);
		thisdrop->clientid = GetNewClientID();
		thisdrop->type = 2; // ITEM
		thisdrop->pos = RandInCircle( thisclient->Position->current, 3 );
		thisdrop->posMap = thisclient->Position->Map;
		thisdrop->droptime = time(NULL);
		thisdrop->amount = amount;
		thisdrop->item = thisclient->items[itemid];
		thisdrop->item.count = amount;
		thisdrop->owner = 0;
		CMap* map = MapList.Index[thisdrop->posMap];
		map->AddDrop( thisdrop );
		if (flag)
		   ClearItem(thisclient->items[itemid]);
        thisclient->UpdateInventory( itemid );
	}
	return true;
}

// Picks up item
bool CWorldServer::pakPickDrop( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    thisclient->RestartPlayerVal( );
	WORD dropid = GETWORD((*P), 0x00);
	CDrop* thisdrop = GetDropByID( dropid, thisclient->Position->Map );
	if (thisdrop==NULL) return true;
	bool flag = false;
    BEGINPACKET( pak, 0x7a7 );
	ADDWORD    ( pak, thisdrop->clientid );
    CPlayer* dropowner = 0;
    if( (thisdrop->owner==0 || thisdrop->owner==thisclient->CharInfo->charid || time(NULL)-thisdrop->droptime>=30 ) || ( thisclient->Party->party!=NULL && thisclient->Party->party == thisdrop->thisparty ))
    {
        if( thisclient->Party->party!=NULL )
        {
            unsigned int dropparty = thisclient->Party->party->Option/0x10;
            if( dropparty == 8 ) // Orderly
            {
                unsigned int num = 0;
                bool dpflag = false;
                if( thisdrop->type==1 )
                {
                    num = thisclient->Party->party->LastZulies;
                    thisclient->Party->party->LastZulies++;
                    if( thisclient->Party->party->LastZulies>=thisclient->Party->party->Members.size() )
                        thisclient->Party->party->LastZulies = 0;
                }
                else
                if( thisdrop->type==2 )
                {
                    num = thisclient->Party->party->LastItem;
                    thisclient->Party->party->LastItem++;
                    if( thisclient->Party->party->LastItem>=thisclient->Party->party->Members.size() )
                        thisclient->Party->party->LastItem = 0;
                }
                unsigned int n = 0;
                while( !dpflag )
                {
                    n++;
                    if( num>=thisclient->Party->party->Members.size() )
                        num = 0;
                    dropowner = thisclient->Party->party->Members.at( num );
                    if( dropowner == NULL )
                        num++;
                    else
                    {
                        dpflag = true;
                        num++;
                    }
                    if(n>20) // not founded yet? >.>
                    {
                        dropowner = thisclient;
                        dpflag = true;
                    }
                }
            }
            else // Equal Loot
            {
                dropowner = thisclient;
            }
        }
        else
        {
            dropowner = thisclient;
        }
        if( thisdrop->type == 1 ) //Zulie
        {
		    dropowner->CharInfo->Zulies += thisdrop->amount;
		    ADDWORD( pak, 0 );
		    ADDBYTE( pak, 0 );
		    ADDDWORD( pak, 0xccccccdf );
		    ADDDWORD( pak, thisdrop->amount );
                ADDDWORD( pak, 0xcccccccc );
                ADDWORD ( pak, 0xcccc );
		    dropowner->client->SendPacket( &pak );
		    flag = true;
        }
        else
        if( thisdrop->type == 2 ) // Item
        {
            //LMA: drop can be something else than a mere item...
            unsigned int type=0;
            if(thisdrop->item.itemtype==10)
            {
                type = UseList.Index[thisdrop->item.itemnum]->type;
            }

            if (type == 320 && thisdrop->item.itemtype == 10)
            {
                RESETPACKET( pak,0x7a3 );
                ADDWORD    ( pak, dropowner->clientid );
                ADDWORD    ( pak, thisdrop->item.itemnum );
                SendToVisible( &pak, dropowner );
                flag = true;
            }
            else
            {
                unsigned int tempslot = dropowner->AddItem( thisdrop->item );
                if(tempslot!=0xffff)// we have slot
                {
                    unsigned int slot1 = tempslot;
                    unsigned int slot2 = 0xffff;
                    if(tempslot>MAX_INVENTORY)
                    {
                        slot1 = tempslot/1000;
                        slot2 = tempslot%1000;
                    }
                    ADDBYTE    ( pak, 0x00 );
                    ADDBYTE    ( pak, slot1 );
                    ADDBYTE    ( pak, 0x00 );
                    ADDDWORD   ( pak, BuildItemHead( dropowner->items[slot1] ) );
                    ADDDWORD   ( pak, BuildItemData( dropowner->items[slot1] ) );
                    ADDDWORD( pak, 0x00000000 );
                    ADDWORD ( pak, 0x0000 );

                    dropowner->client->SendPacket( &pak );
                    dropowner->UpdateInventory( slot1, slot2 );
                    flag = true;
                }
                else
                {
                    ADDBYTE    (pak, 0x03);
                    dropowner->client->SendPacket(&pak);
                }

            }

        }
    }
	else
	{
		ADDBYTE    (pak, 0x02);
		thisclient->client->SendPacket(&pak);
    }

	if( flag )
	{
        if( thisclient->Party->party!=NULL )
        {
            unsigned int dropparty = thisclient->Party->party->Option/0x10;
            if( dropparty == 8 )
            {
                BEGINPACKET( pak, 0x7d3 );
                ADDWORD    ( pak, dropowner->clientid );
                if( thisdrop->type == 1 )
                {
                    ADDDWORD( pak, 0xccccccdf );
        		    ADDDWORD( pak, thisdrop->amount );
                    ADDDWORD( pak, 0xcccccccc );
                    ADDWORD ( pak, 0xcccc );
                }
                else
                if( thisdrop->type == 2 )
                {
                    ADDDWORD   ( pak, BuildItemHead( thisdrop->item ) );
                    ADDDWORD   ( pak, BuildItemData( thisdrop->item ) );
                    ADDDWORD( pak, 0x00000000 );
                    ADDWORD ( pak, 0x0000 );

                }
                thisclient->Party->party->SendToMembers( &pak, dropowner );
            }
        }
		CMap* map = MapList.Index[thisdrop->posMap];
		pthread_mutex_lock( &map->DropMutex );
		map->DeleteDrop( thisdrop );
		pthread_mutex_unlock( &map->DropMutex );
    }
	return true;
}

// Changes equipment
bool CWorldServer::pakChangeEquip( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
	WORD srcslot = GETWORD((*P), 0);
	WORD destslot = GETWORD((*P), 2);

	//Log(MSG_INFO,"Trying to move item %i::%i (slot %i) to slot %i (where is %i::%i)",thisclient->items[srcslot].itemtype,thisclient->items[srcslot].itemnum,srcslot,destslot,thisclient->items[destslot].itemtype,thisclient->items[destslot].itemnum);
	if(!CheckInventorySlot(thisclient, srcslot))
	   return false;
	if(!CheckInventorySlot(thisclient, destslot))
	   return false;

    //LMA: it seems naRose sends the swap weirdly sometimes...
    if(thisclient->items[srcslot].itemtype==0||thisclient->items[srcslot].itemnum==0)
    {
        //Log(MSG_WARNING,"Problem when trying to change slot %i to %i (item not there anymore?)",srcslot,destslot);
        int tempslot=0;
        tempslot=destslot;
        destslot=srcslot;
        srcslot=tempslot;
        if(thisclient->items[srcslot].itemtype==0||thisclient->items[srcslot].itemnum==0)
        {
            Log(MSG_WARNING,"Problem (2) when trying to change slot %i to %i (item not there anymore?)",srcslot,destslot);
            return true;
        }

    }

    if (destslot < 13 && GServer->EquipList[thisclient->items[srcslot].itemtype].Index[thisclient->items[srcslot].itemnum]->level > thisclient->Stats->Level)
       return true;
	if( destslot==0 ) destslot = thisclient->GetNewItemSlot( thisclient->items[srcslot] );
	if( destslot==0xffff ) return true;
    if( srcslot==7 || destslot==7 )
    {
        // clear the buffs if we change the weapon
        for(UINT i=0;i<15;i++)
        {
            switch(thisclient->MagicStatus[i].Buff)
            {
                case A_ATTACK:
                case A_ACCUR:
                case A_HASTE:
                case A_CRITICAL:
                case A_STEALTH:
                case A_CLOAKING:
                    thisclient->MagicStatus[i].Duration = 0;
                break;
            }
        }
    }

	CItem tmpitm = thisclient->items[srcslot];
	thisclient->items[srcslot] = thisclient->items[destslot];
	thisclient->items[destslot] = tmpitm;
	thisclient->UpdateInventory( srcslot, destslot );

    //LMA: test.
    //Log(MSG_INFO,"Item Srcslot (after UV) %i, %i::%i",srcslot,thisclient->items[srcslot].itemtype,thisclient->items[srcslot].itemnum);

	BEGINPACKET( pak, 0x7a5 );
	ADDWORD    ( pak, thisclient->clientid );
	ADDWORD    ( pak, srcslot );
	ADDWORD    ( pak, thisclient->items[srcslot].itemnum );
	ADDWORD    ( pak, BuildItemRefine( thisclient->items[srcslot] ) );
	ADDWORD    ( pak, thisclient->Stats->Move_Speed );
	SendToVisible( &pak, thisclient );

	//LMA: narose sends change a new way sometimes...
    if (destslot<10)
    {
        BEGINPACKET( pak, 0x7a5 );
        ADDWORD    ( pak, thisclient->clientid );
        ADDWORD    ( pak, destslot );
        ADDWORD    ( pak, thisclient->items[destslot].itemnum );
        ADDWORD    ( pak, BuildItemRefine( thisclient->items[destslot] ) );
        ADDWORD    ( pak, thisclient->Stats->Move_Speed );
        SendToVisible( &pak, thisclient );
    }

    if( srcslot==7 || destslot==7 )
    {
        // if is two hand weapon, we have to check if have shield and unequip it
        UINT weapontype = EquipList[WEAPON].Index[thisclient->items[7].itemnum]->type;
        switch(weapontype)
        {
            case TWO_HAND_SWORD:
            case SPEAR:
            case TWO_HAND_AXE:
            case BOW:
            case GUN:
            case LAUNCHER:
            case STAFF:
            case KATAR:
            case DOUBLE_SWORD:
            case DUAL_GUN:
            {
                if(thisclient->items[8].itemnum!=0)
                { // we should unequip the shield
                    UINT newslot = thisclient->GetNewItemSlot( thisclient->items[8] );
                    if(newslot==0xffff)
                    {
                        // we have no slot for the shield, we have to change back the items
                        tmpitm = thisclient->items[destslot];
                        thisclient->items[destslot] = thisclient->items[srcslot];
                        thisclient->items[srcslot] = tmpitm;
                        SendSysMsg( thisclient, "You don't have free slot" );
                        return true;
                    }
                    thisclient->items[newslot] = thisclient->items[8];
                    ClearItem( thisclient->items[8] );
                    thisclient->UpdateInventory( newslot, 8 );
                    RESETPACKET( pak, 0x7a5 );
                    ADDWORD    ( pak, thisclient->clientid );
                    ADDWORD    ( pak, 8 );
                    ADDWORD    ( pak, thisclient->items[8].itemnum );
                    ADDWORD    ( pak, BuildItemRefine( thisclient->items[8] ) );
                    ADDWORD    ( pak, thisclient->Stats->Move_Speed );
                    SendToVisible( &pak, thisclient );
                }
            }
            break;
        }
    }
    thisclient->SetStats( );
	return true;
}

// Construct a cart/change parts
bool CWorldServer::pakChangeCart( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;

	WORD cartslot = GETWORD((*P), 0);
	WORD srcslot = GETWORD((*P), 0) + 135;
	WORD destslot = GETWORD((*P), 2);

    //LMA: no part change if already riding, except for weapons?
    //2do: check if parts are compatible? To avoid cart frame with CG part?
    if(thisclient->Status->Stance==DRIVING&&destslot!=139&&srcslot!=139)
        return true;

	if(!CheckInventorySlot( thisclient, srcslot ))
	   return false;
	if(!CheckInventorySlot( thisclient, destslot ))
	   return false;
	if( destslot == 0 ) destslot=thisclient->GetNewItemSlot( thisclient->items[srcslot] );
	if( destslot == 0xffff ) return true;

	//LMA: we need to check if all items have the same PatType...
    //LMA: test only if equip.
	if (destslot>=135&&destslot<140&&thisclient->items[srcslot].itemnum>0)
	{
        int good_type=0;
        for (int k=135;k<140;k++)
        {
            if(thisclient->items[k].itemnum==0)
                continue;
            if(good_type==0)
            {
                good_type=PatList.Index[thisclient->items[k].itemnum]->parttype;
            }
            else
            {
                if(good_type!=PatList.Index[thisclient->items[k].itemnum]->parttype)
                {
                    Log(MSG_WARNING,"%s has mixed type pieces on his cart...",thisclient->CharInfo->charname);
                    return true;
                }

            }

        }

        if(good_type>0&&(good_type!=PatList.Index[thisclient->items[srcslot].itemnum]->parttype))
        {
            GServer->SendPM(thisclient,"You can't use mixed part");
            Log(MSG_WARNING,"%s tries to use mix pieces for his cart...",thisclient->CharInfo->charname);
            return true;
        }

	}

	CItem tmpitm = thisclient->items[srcslot];
	thisclient->items[srcslot] = thisclient->items[destslot];
	thisclient->items[destslot] = tmpitm;

    //LMA: Getting good mspeed value for packet.
    thisclient->UpdateInventory( srcslot, destslot );
    unsigned int lma_speed=thisclient->GetCartSpeed();
    thisclient->Stats->Move_Speed = thisclient->GetMoveSpeed( );

	BEGINPACKET( pak, 0x7ca );
	ADDWORD    ( pak, thisclient->clientid );
	ADDWORD    ( pak, cartslot);
	ADDWORD    ( pak, thisclient->items[srcslot].itemnum);
	//ADDWORD    ( pak, BuildItemRefine( thisclient->items[srcslot] ) );
	//ADDWORD    ( pak, thisclient->Stats->Move_Speed );

	//ADDWORD    ( pak, tmpitm.itemnum);
	//ADDWORD    ( pak, BuildItemRefine( tmpitm ) );

	//LMA TEST:
	//ADDWORD    ( pak, lma_speed );
	ADDWORD    ( pak,0);

	//LMA: change?
	if(thisclient->Status->Stance==DRIVING)
	{
	    ADDWORD    ( pak, thisclient->Stats->Move_Speed );
	    //ADDWORD    ( pak, thisclient->Stats->Base_Speed );

        //LMA: Used to refresh the fuel and attack_fuel
        thisclient->TakeFuel();
	}

	SendToVisible( &pak, thisclient );

    //thisclient->UpdateInventory( srcslot, destslot );
	//thisclient->Stats->Move_Speed = thisclient->GetMoveSpeed( );
	return true;
}

// Start attacking a monster or player
bool CWorldServer::pakStartAttack( CPlayer* thisclient, CPacket* P )
{
    if(!thisclient->CanAttack( )) return true;

    //LMA: Do we have fuel?
    if(thisclient->Status->Stance==DRIVING)
    {
        if(thisclient->items[136].itemnum==0||thisclient->items[139].itemnum==0)
        {
            Log(MSG_WARNING,"%s wants to attack but without motor %i or weapon %i ?",thisclient->CharInfo->charname,thisclient->items[136].itemnum,thisclient->items[139].itemnum);
            return true;
        }

        if(thisclient->items[136].lifespan==0)
        {
            //No fuel anymore, can't attack.
            return true;
        }

    }

    WORD clientid = GETWORD((*P),0x00);
    if ( thisclient->Battle->target == clientid ) return true;

    int weapontype = EquipList[WEAPON].Index[thisclient->items[7].itemnum]->type;
    if( weapontype == 231 && thisclient->items[132].count<1 )
        return true;
    else
    if( weapontype == 232 && thisclient->items[133].count<1 )
        return true;
    else
    if( weapontype == 233 && thisclient->items[134].count<1 )
        return true;
    else
    if( weapontype == 271 && thisclient->items[132].count<1 )
        return true;

    CMap* map = MapList.Index[thisclient->Position->Map];
    CCharacter* character = map->GetCharInMap( clientid );
    if(character==NULL) return true;
    if(character->IsMonster( ))
    {
        if(map->allowpvp==0 && character->IsSummon( )) return true;
        if(!character->IsSummon())
            character->AddDamage( (CCharacter*)thisclient, 0 );
    }
	if(character->IsMonster())
	{
	   // SET MONSTER HEALTH
        //LMA: Little check, for now we "only" have a DWORD for monster's HP so there is a limit
        //broken by some monsters (Turak boss)
        if(character->Stats->HP>MAXHPMOB)
        {
            character->Stats->HP=(long long) MAXHPMOB;
        }

	   BEGINPACKET( pak, 0x79f );
	   ADDWORD    ( pak, character->clientid );
	   ADDDWORD   ( pak, character->Stats->HP );
	   thisclient->client->SendPacket( &pak );
    }
    thisclient->StartAction( character, NORMAL_ATTACK, 0 );
    thisclient->Battle->contatk = true;


	return true;
}

//LMA-Shin: PVP Packet? In fact sent to tell to stop...
bool CWorldServer::pakPvp796( CPlayer* thisclient, CPacket* P )
{
   BEGINPACKET( pak, 0x796 );
   ADDWORD    ( pak, thisclient->clientid );
   ADDWORD    ( pak, thisclient->Battle->target);
   ADDWORD    ( pak, thisclient->GetMoveSpeed() );
   ADDFLOAT   ( pak, thisclient->Position->current.x*100 );
   ADDFLOAT   ( pak, thisclient->Position->current.y*100 );
   SendToVisible( &pak, thisclient );

	return true;
}

//Drakia's telegate hacker fix - needs testing.  Comment above and uncomment below for test
// Goto next map through gate
bool CWorldServer::pakGate( CPlayer* thisclient, CPacket* P )
{
    thisclient->Session->inGame = false;
    word GateID = GETWORD((*P), 0x00);

    /*CTeleGate* thisgate = GetTeleGateByID( GateID );
    fPoint position;
    UINT map = 0;

    // I'm setting this at 50 distance from the point of teleport. Increase if you run into
    // broken telegates. We might need to dump the scale and work from that - Drakia
    if( thisgate == NULL ||thisclient->Position->Map != thisgate->srcMap ||distance(thisclient->Position->current, thisgate->src) > 100 )
    {
        if(thisgate == NULL)
        {
            Log( MSG_HACK, "Player %s[Map: %i X: %f Y: %f] - Gate Hacking[ID: %i], Gates doesn't exist!",
                     thisclient->CharInfo->charname, thisclient->Position->Map,
                     thisclient->Position->current.x, thisclient->Position->current.y, GateID);
            return true;
        }

      Log( MSG_HACK, "Player %s[Map: %i X: %f Y: %f] - Gate Hacking[ID: %i], distance %.2f",
                     thisclient->CharInfo->charname, thisclient->Position->Map,
                     thisclient->Position->current.x, thisclient->Position->current.y, GateID,distance(thisclient->Position->current, thisgate->src) );
        return true;
	}*/


	//LMA: new way.
    bool is_hack=true;
    fPoint position;
    UINT map = 0;
    CTeleGate* thisgate=NULL;
    CTeleGate* lastgate=NULL;

	for (int k=0;k<6;k++)
	{
	    thisgate = GetTeleGateByID( GateID,k);

	    if (thisgate==NULL)
	    {
	        if(k==0)
	        {
                Log( MSG_HACK, "Player %s[Map: %i X: %f Y: %f] - Gate Hacking[ID: %i], Gates doesn't exist!",
                         thisclient->CharInfo->charname, thisclient->Position->Map,
                         thisclient->Position->current.x, thisclient->Position->current.y, GateID);
	        }
	        else
	        {
	            //Hack.
                Log( MSG_HACK, "Player %s[Map: %i X: %f Y: %f] - Gate Hacking[ID: %i, offset %i], distance %.2f",
                             thisclient->CharInfo->charname, thisclient->Position->Map,
                             thisclient->Position->current.x, thisclient->Position->current.y, GateID,lastgate->offset,distance(thisclient->Position->current, lastgate->src) );
	        }

            return true;
	    }

	    if(thisclient->Position->Map != thisgate->srcMap ||distance(thisclient->Position->current, thisgate->src) > 100)
	    {
            //hack or wrong gate?
            lastgate=thisgate;
            continue;
	    }

	    //good gate :)
	    is_hack=false;
	    Log(MSG_INFO,"Gate %i found, offset %i",thisgate->id,k);
	    break;
	}
	//LMA: end new way.

	if(is_hack)
	{
        Log( MSG_HACK, "Player %s[Map: %i X: %f Y: %f] - Gate Hacking[ID: %i, offset %i], distance %.2f",
                     thisclient->CharInfo->charname, thisclient->Position->Map,
                     thisclient->Position->current.x, thisclient->Position->current.y, GateID,lastgate->offset,distance(thisclient->Position->current, lastgate->src) );
	    return true;
	}

	map = thisgate->destMap;
	position = thisgate->dest;
    MapList.Index[map]->TeleportPlayer( thisclient, position );


	return true;
}


// Emotions
bool CWorldServer::pakDoEmote( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    thisclient->RestartPlayerVal( );
	BEGINPACKET( pak, 0x781 );
	ADDWORD( pak, GETWORD((*P), 0) );
	ADDWORD( pak, 0x8000 );
	ADDWORD( pak, thisclient->clientid );
	SendToVisible( &pak, thisclient );
	return true;
}

// This function is for increasing your stats
bool CWorldServer::pakAddStats( CPlayer* thisclient, CPacket* P )
{
	BYTE statid = GETBYTE((*P), 0);
	int statval = -1;
	int nstatval = 0;
	switch(statid)
    {
		case 0: nstatval=(int)floor((float)thisclient->Attr->Str/5); break;
		case 1: nstatval=(int)floor((float)thisclient->Attr->Dex/5); break;
		case 2: nstatval=(int)floor((float)thisclient->Attr->Int/5); break;
		case 3: nstatval=(int)floor((float)thisclient->Attr->Con/5); break;
		case 4: nstatval=(int)floor((float)thisclient->Attr->Cha/5); break;
		case 5: nstatval=(int)floor((float)thisclient->Attr->Sen/5); break;
	}
	if (thisclient->CharInfo->StatPoints>=nstatval && nstatval < ((Config.MaxStat/5)+1))
    {
		switch(statid)
        {
			case 0: statval=++thisclient->Attr->Str; break;
			case 1: statval=++thisclient->Attr->Dex; break;
			case 2: statval=++thisclient->Attr->Int; break;
			case 3: statval=++thisclient->Attr->Con; break;
			case 4: statval=++thisclient->Attr->Cha; break;
			case 5: statval=++thisclient->Attr->Sen; break;
		}
	}
	if (statval>0)
    {
		thisclient->CharInfo->StatPoints-=nstatval;
		BEGINPACKET( pak, 0x7a9 );
		ADDBYTE( pak, statid );
		ADDWORD( pak, (unsigned short)statval );
		thisclient->client->SendPacket( &pak );
	}
	thisclient->SetStats( );
	return true;
}

// Normal Chat
bool CWorldServer::pakNormalChat( CPlayer* thisclient, CPacket* P )
{
	if (P->Buffer[0]=='/')
    {
		return pakGMCommand( thisclient, P );
        //std::cout << thisclient->CharInfo->charname << ": " << P->Buffer << "\n";

    }
    else
    {
		BEGINPACKET( pak, 0x783 );
		ADDWORD( pak, thisclient->clientid );
		ADDSTRING( pak, P->Buffer );
		ADDBYTE( pak, 0 );
		SendToVisible( &pak, thisclient );
	}
	return true;
};

// Exit rose
bool CWorldServer::pakExit( CPlayer* thisclient, CPacket* P )
{
	BEGINPACKET( pak, 0x707 );
	ADDWORD( pak, 0 );
	thisclient->client->SendPacket( &pak );
	thisclient->client->isActive = false;
	return true;
}

// move skill to function keys
bool CWorldServer::pakMoveSkill ( CPlayer* thisclient, CPacket* P )
{
	BYTE slotid = GETBYTE((*P), 0x0 );
	WORD itemid = GETWORD((*P), 0x1 );
	if(slotid>47) return true;
	thisclient->quickbar[slotid] = itemid;
	BEGINPACKET( pak, 0x7aa );
	ADDBYTE( pak, slotid );
	ADDWORD( pak, itemid );
	thisclient->client->SendPacket( &pak );
	return true;
}

// User is checking up on the server
bool CWorldServer::pakPing ( CPlayer* thisclient, CPacket* P )
{
	BEGINPACKET( pak, 0x0700 );
	ADDWORD( pak, 0 );
	thisclient->client->SendPacket( &pak );
	return true;
}

// GameGuard
bool CWorldServer::pakGameGuard ( CPlayer* thisclient, CPacket* P )
{
	thisclient->lastGG = clock();
	return true;
}

// LMA: Bonus Exp Time coupon (Using Drose fix)
bool CWorldServer::pakExpTC ( CPlayer* thisclient, CPacket* P )
{
    unsigned int action = GETBYTE((*P),0);
    //TEST
    time_t rawtime;
    struct tm * timeinfo;

    if (thisclient->timerxp==0||thisclient->wait_validation==0)
    {
       Log(MSG_HACK,"Bonus XP packet received from %s (shouldn't be).",thisclient->CharInfo->charname);
       return true;
    }

    //validating Xp bonus :)
    thisclient->bonusxp=thisclient->wait_validation;
    rawtime=thisclient->timerxp;
    //time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    switch(action)
    {
        case 0x04: //Medal
        {
             BEGINPACKET( pak, 0x822 );
             ADDBYTE( pak, 0x04 );
             ADDBYTE( pak, 0x00 );
             ADDWORD( pak, 0x0000 );
             ADDWORD(pak,timeinfo->tm_year+1900);   //Year
             ADDBYTE( pak, timeinfo->tm_mon+1);  //Month
             ADDBYTE( pak, timeinfo->tm_mday);  //Day
             ADDBYTE( pak, timeinfo->tm_hour+1);  //hour
             //ADDBYTE( pak, timeinfo->tm_min+1); //minutes?
             ADDBYTE( pak, 0x00);  //minutes?

			 thisclient->client->SendPacket( &pak );
             return true;
        }
        break;
        default:
                Log(MSG_WARNING,"TimeCoupon - unknown action: %i", action );
    }


	return true;
}

// Changing Respawn for a client
bool CWorldServer::pakChangeRespawn( CPlayer* thisclient, CPacket* P )
{
        //player position (map)
       unsigned int spot = 0;
        switch(thisclient->Position->Map)
        {
            case 22:  //AP
            {
                spot = 1;
                break;
            }
            case 1:  //Zant
            {
                spot = 2;
                break;
            }
            case 2:  //Junon
            {
                spot = 4;
                break;
            }
            case 51:  //Eucar
            {
                spot = 5;
                break;
            }
            case 61:  //Xita
            {
                spot = 68;
                break;
            }
        }

                thisclient->Position->saved = spot;
    return true;
}

// User Died
bool CWorldServer::pakUserDied ( CPlayer* thisclient, CPacket* P )
{
    thisclient->Session->inGame = false;
    BYTE respawn = GETBYTE((*P),0);
    //1 - Current / 2 - save point
    CMap* map = MapList.Index[thisclient->Position->Map];
    CRespawnPoint* thisrespawn = NULL;
    CRespawnPoint dungeonSpawn;

    if(respawn==1)
    {
        thisrespawn = map->GetNearRespawn( thisclient );
    }
    else
    {
        //LMA: special case for dungeons... STB[4]==1
        thisrespawn = &dungeonSpawn;
        thisrespawn->destMap=map->id;
        thisrespawn->id=1;
        thisrespawn->dest.z=0;
        switch(map->id)
        {
            case 31: //Goblin Cave B1
            {
                thisrespawn->dest.x=5516;
                thisrespawn->dest.y=5437;
            }
            break;
            case 32: //Goblin Cave B2
            {
                thisrespawn->dest.x=5435;
                thisrespawn->dest.y=5259;
            }
            break;
            case 33: //Goblin Cave B3
            {
                thisrespawn->dest.x=5605;
                thisrespawn->dest.y=5488;
            }
            break;
            case 40: //Grand Ballroom
            {
                thisrespawn->dest.x=5184;
                thisrespawn->dest.y=5211;
            }
            break;
            case 56: //Forgotten temple B1
            {
                thisrespawn->dest.x=5035;
                thisrespawn->dest.y=5200;
            }
            break;
            case 57: //Forgotten temple B1
            {
                thisrespawn->dest.x=5540;
                thisrespawn->dest.y=5145;
            }
            break;
            case 65: //Prison
            {
                thisrespawn->dest.x=5485;
                thisrespawn->dest.y=5304;
            }
            break;

            default:
            {
                thisrespawn = GetRespawnByID( thisclient->Position->saved );
            }
            break;
        }

    }

    //LMA: Special Union War case ;)
    /*
    if(thisclient->Position->Map==9)
    {
        int success=QUEST_FAILURE;
        char* tempName = "PvP10-331";
        dword hash = MakeStrHash(tempName);
        success=thisclient->ExecuteQuestTrigger(hash,true);
        if (success==QUEST_SUCCESS)
        {
            Log(MSG_WARNING,"Player %s has been killed in UW, 100%HP 100%MP %s",thisclient->CharInfo->charname);
            Log(MSG_INFO,"Player %s died in UW, we give him %I64i Hp",thisclient->CharInfo->charname,thisclient->Stats->HP);

            if(thisclient->UWPosition->Map>0&&thisclient->UWPosition->source.x>0&&thisclient->UWPosition->source.y>0)
            {
                Log(MSG_WARNING,"Player %s is UW teleporting after death to map %i (%.2f,%.2f)",thisclient->CharInfo->charname,thisclient->UWPosition->Map,thisclient->UWPosition->source.x,thisclient->UWPosition->source.y);
                fPoint coord;
                coord.x = thisclient->UWPosition->source.x;
                coord.y = thisclient->UWPosition->source.y;
                MapList.Index[thisclient->UWPosition->Map]->TeleportPlayer( thisclient, coord, false );
            }

        }
        else
        {
            Log(MSG_WARNING,"Player %s has been killed in UW, quest returned failure",thisclient->CharInfo->charname);
        }

    }
    else
    {
        thisclient->Stats->HP = thisclient->Stats->MaxHP * 10 / 100;
        Log(MSG_INFO,"Player %s died, we give him %I64i Hp",thisclient->CharInfo->charname,thisclient->Stats->HP);
        if(thisrespawn!=NULL)
        {
            // geo edit for saved town warp // 29 sep 07
            map = MapList.Index[thisrespawn->destMap];
            map->TeleportPlayer( thisclient, thisrespawn->dest, false );
        }
        else
        {
            Log(MSG_WARNING,"Player died, respawn not found for map %i, respawn %i",map->id,respawn);
            fPoint coord;
            coord.x = 5200;
            coord.y = 5200;
            MapList.Index[2]->TeleportPlayer( thisclient, coord, false );
        }

    }
    */

    if(thisclient->Stats->HP<=0||(thisclient->Stats->HP < (thisclient->Stats->MaxHP * 10 / 100)))
    {
        thisclient->Stats->HP = thisclient->Stats->MaxHP * 10 / 100;
        Log(MSG_INFO,"Player %s died, we give him %I64i Hp",thisclient->CharInfo->charname,thisclient->Stats->HP);
    }
    else
    {
        Log(MSG_INFO,"Player %s died, he has %I64i Hp",thisclient->CharInfo->charname,thisclient->Stats->HP);
    }

    //LMA: cleaning buffs.
    //and resetting stats.
    for(int j = 0; j < 30; j++)
    {
        thisclient->MagicStatus[j].Duration = 0;
        thisclient->MagicStatus[j].BuffTime = 0;
    }

    thisclient->RefreshBuff();
    thisclient->SetStats( );

    if(thisrespawn!=NULL)
    {
        // geo edit for saved town warp // 29 sep 07
        map = MapList.Index[thisrespawn->destMap];
        map->TeleportPlayer( thisclient, thisrespawn->dest, false );
    }
    else
    {
        Log(MSG_WARNING,"Player died, respawn not found for map %i, respawn %i",map->id,respawn);
        fPoint coord;
        coord.x = 5200;
        coord.y = 5200;
        MapList.Index[2]->TeleportPlayer( thisclient, coord, false );
    }

    /*
	for(unsigned int i=0;i<30;i++)
	{	// Clean Buffs
        thisclient->MagicStatus[i].Duration = 0;
    }
    */


	return true;
}

// Shouting
bool CWorldServer::pakShout ( CPlayer* thisclient, CPacket* P )
{
	BEGINPACKET(pak, 0x0785);
	ADDSTRING  ( pak, thisclient->CharInfo->charname );
	ADDBYTE    ( pak, 0 );
	ADDSTRING  ( pak, &P->Buffer[0] );
	ADDBYTE    ( pak, 0 );
	SendToMap  ( &pak, thisclient->Position->Map );

	return true;
}

// Whispering
bool CWorldServer::pakWhisper ( CPlayer* thisclient, CPacket* P )
{
	char msgto[17];
	memset( &msgto, '\0', 17 );
	strncpy( msgto, (char*)&(*P).Buffer[0],16 );
	CPlayer* otherclient = GetClientByCharName( msgto );
	if(otherclient!=NULL)
	{
	   BEGINPACKET( pak, 0x0784 );
	   ADDSTRING( pak, thisclient->CharInfo->charname );
	   ADDBYTE( pak, 0 );
	   ADDSTRING( pak, &P->Buffer[strlen(msgto)+1] );
	   ADDBYTE( pak, 0 );
	   otherclient->client->SendPacket( &pak );
	   return true;
	}
	BEGINPACKET( pak, 0x0784 );
	ADDSTRING( pak, msgto );
	ADDBYTE( pak, 0 );
	ADDBYTE( pak, 0 );
	thisclient->client->SendPacket( &pak );
	return true;
}

// Return to Char Select
bool CWorldServer::pakCharSelect ( CPlayer* thisclient, CPacket* P )
{
   	thisclient->savedata();
  	if(!thisclient->Session->inGame) return true;
	//Maxxon: Party crash when exit fix.
	OnClientDisconnect(thisclient->client);
    thisclient->Session->isLoggedIn = false;
    if(thisclient->client!=NULL) thisclient->client->isActive = false;
    //send packet to change messenger status (offline)
   	BEGINPACKET( pak, 0x7e1 );
   	ADDBYTE    ( pak, 0xfa );
   	ADDWORD    ( pak, thisclient->CharInfo->charid );
   	ADDBYTE    ( pak, 0x00 );
   	cryptPacket( (char*)&pak, cct );
   	send( csock, (char*)&pak, pak.Size, 0 );
   	pakClearUser( thisclient );
   	ClearClientID( thisclient->clientid );
    thisclient->Saved = true;
	RESETPACKET( pak, 0x505 );
	ADDDWORD( pak, thisclient->Session->userid );
	cryptPacket( (char*)&pak, cct );
	send( csock, (char*)&pak, pak.Size, 0 );
	return true;
}

// Buy from NPC (usual and Clan Shop).
//2do: Count and check Z and reward points BEFORE giving the items...
bool CWorldServer::pakNPCBuy ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    WORD NPC = GETWORD((*P), 0);
	CNPC* thisnpc = GetNPCByID( NPC, thisclient->Position->Map );
	if( thisnpc==NULL )
	   return true;

	BYTE buycount = GETBYTE((*P), 2);
	BYTE sellcount = GETBYTE((*P), 3);

     //Clan Shop case...
     //Log(MSG_INFO,"Buying /selling from NPC %i",thisnpc->npctype);
     bool is_clanshop=false;
     bool is_union=false;
     UINT nb_union_points=0;
     if (thisnpc->npctype==1752)
     {
        is_clanshop=true;

        //impossible to buy from clan shop if you don't have a clan
        if (thisclient->Clan->clanid==0)
        {
            return true;
        }

     }

     //LMA: Union Shops.
     if(thisnpc->npctype>=1109&&thisnpc->npctype<=1112)
     {
         is_union=true;

         switch (thisclient->CharInfo->unionid)
         {
             case 1:
             {
                 nb_union_points=thisclient->CharInfo->union01;
             }
             break;
            case 2:
             {
                 nb_union_points=thisclient->CharInfo->union02;
             }
             break;
             case 3:
             {
                 nb_union_points=thisclient->CharInfo->union03;
             }
             break;
             case 4:
             {
                 nb_union_points=thisclient->CharInfo->union04;
             }
             break;
             case 5:
             {
                 nb_union_points=thisclient->CharInfo->union05;
             }
             break;
             default:
             {
                 //Can't be.
                 Log(MSG_WARNING,"%s tried to shop in Union shop but isn't in an union (%i)",thisclient->CharInfo->charname,thisclient->CharInfo->unionid);
                 return true;
             }
             break;
         }

     }

	BYTE ncount = 0;
	BEGINPACKET( pak, 0x717 );
	ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
	ADDBYTE    ( pak, 0x00 );
	for (int i=0; i<buycount; i++)
    {
		BYTE tabid = GETBYTE((*P), 8+(i*4));
		BYTE itemid = GETBYTE((*P), 9+(i*4));
		WORD count = GETWORD((*P), 10+(i*4));
		unsigned sellid = 0;
		switch(tabid)
		{
            case 0:
                sellid = thisnpc->thisnpc->tab1;
            break;
            case 1:
                sellid = thisnpc->thisnpc->tab2;
            break;
            case 2:
                sellid = thisnpc->thisnpc->tab3;
            break;
            case 3:
                sellid = thisnpc->thisnpc->specialtab;
            break;
            default:
                sellid = 0;
        }
        if(SellList.Index[sellid]==NULL) // invalid tab
            continue;
		int tmpcount = count;
        while (tmpcount>0)
        {
            CItem thisitem;
            thisitem.itemnum = SellList.Index[sellid]->item[itemid] % 1000;
            thisitem.itemtype = SellList.Index[sellid]->item[itemid] / 1000;
            if (thisitem.itemtype==0||thisitem.itemnum==0)
            {
                Log(MSG_WARNING,"Problem with selling list, NPC %i, tabid %i, sellid %i, itemid %i",thisnpc->npctype,tabid,sellid,itemid);
                GServer->SendPM(thisclient,"Sorry, there is a problem with this NPC, warn admin");
                return true;
            }


            //LMA: to avoid mem bug.
            thisitem.count=tmpcount;
            unsigned newslot = thisclient->GetNewItemSlot( thisitem );
			if ( newslot == 0xffff ) { ncount-=1; break; }
			int thisslotcount = 999 - thisclient->items[newslot].count;
			if ( thisslotcount > tmpcount ) thisslotcount=tmpcount;
			tmpcount -= thisslotcount;
			thisitem.count = thisslotcount + thisclient->items[newslot].count;
			thisitem.refine = 0;
			thisitem.durability = GServer->STB_ITEM[thisitem.itemtype-1].rows[thisitem.itemnum][29];
			thisitem.lifespan = 100;
			thisitem.appraised = true;
			thisitem.socketed = false;
			thisitem.stats = 0;
			thisitem.gem = 0;
			thisitem.sp_value=0;

			if(thisitem.itemtype==14)
			{
			    thisitem.sp_value=thisitem.lifespan*10;
			}

			thisitem.last_sp_value=0;

			//checking money / reward points now...
    		switch(thisitem.itemtype)
    		{
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 8:
                case 9:
                case 14:
                {
                    float price = 0;
                    UINT quality = 0;
                    UINT bprice = 0;
                    if(thisitem.itemtype<10 )
                    {
                        quality = EquipList[thisitem.itemtype].Index[thisitem.itemnum]->quality;
                        bprice = EquipList[thisitem.itemtype].Index[thisitem.itemnum]->price;
                    }
                    else
                    {
                        quality = PatList.Index[thisitem.itemnum]->quality;
                        bprice = PatList.Index[thisitem.itemnum]->price;
                    }
                    price = quality;
                    price += 0x32; // town rate
                    price *= bprice;
                    price += 0.5;
                    price /= 100;

                    //LMA: price tweak.
                    if(!is_clanshop&&!is_union)
                    {
                        price = (float)round(price/1.25);
                    }
                    else
                    {
                        price = (float)round(price);
                    }

                    if (is_clanshop)
                    {
                        if(thisitem.itemtype<10 )
                        {
                            price=EquipList[thisitem.itemtype].Index[thisitem.itemnum]->craft_difficult;
                        }
                        else
                        {
                            price=PatList.Index[thisitem.itemnum]->craft_difficult;
                        }

                        if (thisclient->CharInfo->rewardpoints<(long int) price)
                        {
                          Log(MSG_HACK, "Not enough reward points player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price);
                          return true;
                        }
                        thisclient->CharInfo->rewardpoints -= (long int) price;
                    }
                    else if (is_union)
                    {
                        if(thisitem.itemtype<10 )
                        {
                            price=EquipList[thisitem.itemtype].Index[thisitem.itemnum]->craft_difficult;
                        }
                        else
                        {
                            price=PatList.Index[thisitem.itemnum]->craft_difficult;
                        }

                        if (nb_union_points<(long int) price)
                        {
                          Log(MSG_HACK, "Not enough Union points player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price);
                          return true;
                        }
                        nb_union_points -= (long int) price;
                    }
                    else
                    {
                        if (thisclient->CharInfo->Zulies<(long int)price)
                        {
                          Log(MSG_HACK, "Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int) price);
                          return true;
                        }
                        thisclient->CharInfo->Zulies -= (long int)price;
                    }

                    Log( MSG_INFO, "%s:: Item bought: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                }
                break;
                case 10:
                case 12:
                {
                    // this values should be the same than packet 753
                    BYTE values[11] = {0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32};
                    UINT type = 0;
                    UINT bprice = 0;
                    UINT pricerate = 0;
                    UINT pvalue = 0;
                    if(thisitem.itemtype==10)
                    {
                        type = UseList.Index[thisitem.itemnum]->type;
                        bprice = UseList.Index[thisitem.itemnum]->price;
                        pricerate = UseList.Index[thisitem.itemnum]->pricerate;
                        pvalue = UseList.Index[thisitem.itemnum]->pricevalue;
                    }
                    else
                    {
                        type = NaturalList.Index[thisitem.itemnum]->type;
                        bprice = NaturalList.Index[thisitem.itemnum]->price;
                        pricerate = NaturalList.Index[thisitem.itemnum]->pricerate;
                        pvalue = NaturalList.Index[thisitem.itemnum]->pricevalue;
                    }
                    unsigned int value = 0;
                    float price = 0;
                    bool flag;
                    if(type<421)
                        if(type<311)
                            flag = false;
                        else
                            if(type>312)
                                flag = false;
                            else
                                flag = true;
                    else
                        if(type<=428)
                            flag = true;
                        else
                            if(type<311)
                                flag = false;
                            else
                                flag = true;
                    if(flag)
                    {
                        value = pvalue;
                        if(value>=11)
                            value ^= 0xffffffff;
                        else
                            value = values[value];
                        value -= 0x32;
                        value *= pricerate;
                        price = value * 0.001;
                        price += 1;
                        price *= bprice;
                        price += 0.5;

                        //LMA: price tweak.
                        if(!is_clanshop&&!is_union)
                        {
                            price = (float)floor(price/1.25);
                        }
                        else
                        {
                            price = (float)floor(price);
                        }

                        if (is_clanshop)
                        {
                            if(thisitem.itemtype==10)
                            {
                                price = UseList.Index[thisitem.itemnum]->craft_difficult;
                            }
                            else
                            {
                                price = 0;
                            }

                            if (thisclient->CharInfo->rewardpoints<(long int) price*count)
                            {
                              Log(MSG_HACK, "Not enough reward points player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price*count);
                              return true;
                            }
                            thisclient->CharInfo->rewardpoints -= (long int) price*count;
                        }
                        else if (is_union)
                        {
                            if(thisitem.itemtype==10)
                            {
                                price = UseList.Index[thisitem.itemnum]->craft_difficult;
                            }
                            else
                            {
                                price = 0;
                            }

                            if (nb_union_points<(long int) price*count)
                            {
                              Log(MSG_HACK, "Not enough Union points player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price*count);
                              return true;
                            }
                            nb_union_points -= (long int) price*count;
                        }
                        else
                        {
                            if (thisclient->CharInfo->Zulies<(long int)price*count)
                            {
                              Log(MSG_HACK, "Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int)price*count);
                              return true;
                            }
                            thisclient->CharInfo->Zulies -= (long int)price*count;
                        }

                        Log( MSG_INFO, "%s:: Item bought: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                    }
                    else
                    {
                        float price = pricerate;
                        unsigned int value = 0x61 - 0x32; // misc rate - 0x32
                        price *= value;
                        price *= 0.001;
                        price += 1;
                        price *= bprice;
                        price += 0.5;
                        //LMA: price tweak.

                        if(!is_clanshop&&!is_union)
                        {
                            price = (float)floor(price/1.25);
                        }
                        else
                        {
                            price = (float)floor(price);
                        }

                        if (is_clanshop)
                        {
                            if(thisitem.itemtype==10)
                            {
                                price = UseList.Index[thisitem.itemnum]->craft_difficult;
                            }
                            else
                            {
                                price = 0;
                            }

                            if (thisclient->CharInfo->rewardpoints<(long int) price*count)
                            {
                              Log(MSG_HACK, "Not enough reward points player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price*count);
                              return true;
                            }
                            thisclient->CharInfo->rewardpoints -= (long int) price*count;
                        }
                        else if (is_union)
                        {
                            if(thisitem.itemtype==10)
                            {
                                price = UseList.Index[thisitem.itemnum]->craft_difficult;
                            }
                            else
                            {
                                price = 0;
                            }

                            if (nb_union_points<(long int) price*count)
                            {
                              Log(MSG_HACK, "Not enough Union points player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price*count);
                              return true;
                            }
                            nb_union_points -= (long int) price*count;
                        }
                        else
                        {
                            if (thisclient->CharInfo->Zulies<(long int)price*count)
                            {
                              Log(MSG_HACK, "Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int)price*count);
                              return true;
                            }
                            thisclient->CharInfo->Zulies -= (long int)price*count;
                        }

                        Log( MSG_INFO, "%s:: Item bought: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                    }
                }
                break;
                case 7:
                case 11:
                case 13:
                {
                    float price = 0;
                    UINT bprice = 0;
                    UINT pricerate;
                    if(thisitem.itemtype==7)
                    {
                        pricerate = EquipList[7].Index[thisitem.itemnum]->pricerate;
                        bprice = EquipList[7].Index[thisitem.itemnum]->price;
                    }
                    else
                    {
                        switch(thisitem.itemtype)
                        {
                            case 11:
                                pricerate = JemList.Index[thisitem.itemnum]->pricerate;
                                bprice = JemList.Index[thisitem.itemnum]->price;
                            break;
                            case 13:continue;
                        }
                    }
                    price = pricerate;
                    unsigned int value = 0x61 - 0x32; // misc rate - 0x32
                    price *= value;
                    price *= 0.001;
                    price += 1;
                    price *= bprice;
                    price += 0.5;

                    //LMA: price tweak.
                    if(!is_clanshop&&!is_union)
                    {
                        price = (float)round(price/1.25);
                    }
                    else
                    {
                        price = (float)round(price);
                    }

                    if (is_clanshop)
                    {
                        if(thisitem.itemtype==7)
                        {
                            price = EquipList[7].Index[thisitem.itemnum]->craft_difficult;
                        }
                        else if(thisitem.itemtype==11)
                        {
                            price = JemList.Index[thisitem.itemnum]->craft_difficult;
                        }

                        if (thisclient->CharInfo->rewardpoints<(long int) price*count)
                        {
                          Log(MSG_HACK, "Not enough reward points player %s, have %u, need %u",thisclient->CharInfo->charname,thisclient->CharInfo->rewardpoints,(long int) price*count);
                          return true;
                        }
                        thisclient->CharInfo->rewardpoints -= (long int) price*count;
                    }
                    else if (is_union)
                    {
                        if(thisitem.itemtype==7)
                        {
                            price = EquipList[7].Index[thisitem.itemnum]->craft_difficult;
                        }
                        else if(thisitem.itemtype==11)
                        {
                            price = JemList.Index[thisitem.itemnum]->craft_difficult;
                        }

                        if (nb_union_points<(long int) price*count)
                        {
                          Log(MSG_HACK, "Not enough Union points player %s, have %u, need %u",thisclient->CharInfo->charname,nb_union_points,(long int) price*count);
                          return true;
                        }
                        nb_union_points -= (long int) price*count;
                    }
                    else
                    {
                        if (thisclient->CharInfo->Zulies<(long int)price*count)
                        {
                          Log(MSG_HACK, "Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,(long int)price*count);
                          return true;
                        }

                        thisclient->CharInfo->Zulies -= (long int)price*count;
                    }

                    Log( MSG_INFO, "%s:: Item bought: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                }
                break;
                default:
                    Log( MSG_WARNING, "Invalid Item Type: %i", thisitem.itemtype );
            }

            //We add item at the end when checks done.
			thisclient->items[newslot] = thisitem;

			//Saving item in database now.
			thisclient->SaveSlot41(newslot);

			ADDBYTE  ( pak, newslot );
			ADDDWORD ( pak, BuildItemHead( thisclient->items[newslot] ) );
			ADDDWORD ( pak, BuildItemData( thisclient->items[newslot] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );

			ncount++;
		}
	}

	//refresh Reward points from player if needed
	if (is_clanshop&&buycount>0)
	{
       GServer->pakGMClanRewardPoints(thisclient,thisclient->CharInfo->charname,0);
    }

    //LMA: refresh Union Points
    if(is_union&&buycount>0)
    {
         switch (thisclient->CharInfo->unionid)
         {
             case 1:
             {
                 thisclient->CharInfo->union01=nb_union_points;
             }
             break;
            case 2:
             {
                 thisclient->CharInfo->union02=nb_union_points;
             }
             break;
             case 3:
             {
                 thisclient->CharInfo->union03=nb_union_points;
             }
             break;
             case 4:
             {
                 thisclient->CharInfo->union04=nb_union_points;
             }
             break;
             case 5:
             {
                 thisclient->CharInfo->union05=nb_union_points;
             }
             break;
             default:
             {
                 //Can't be.
                 Log(MSG_WARNING,"%s tried to shop in Union shop but isn't in an union (%i)",thisclient->CharInfo->charname,thisclient->CharInfo->unionid);
                 return true;
             }
             break;
         }

        int new_offset=80+thisclient->CharInfo->unionid;
        BEGINPACKET( pak, 0x721 );
        ADDWORD( pak, new_offset );
        ADDWORD( pak, nb_union_points );
        ADDWORD( pak, 0x0000 );
        thisclient->client->SendPacket( &pak );
        RESETPACKET( pak, 0x730 );
        ADDWORD    ( pak, 0x0005 );
        ADDDWORD   ( pak, 0x40b3a24d );
        thisclient->client->SendPacket( &pak );
    }

	for (int i=0; i<sellcount; i++)
    {
		BYTE slotid = GETBYTE((*P), 8+(buycount*4)+(i*3));
		WORD count = GETWORD((*P), 9+(buycount*4)+(i*3));
        if (thisclient->items[slotid].count < count)
           return true;
		CItem thisitem = thisclient->items[slotid];
		thisitem.count = count;

		if (thisclient->items[slotid].count<thisitem.count)
		{
          Log(MSG_HACK, "[NPC-SELL] Player %s tryes to sell %i [%i:%i], but has only %i",thisclient->CharInfo->charname,thisitem.count,thisitem.itemtype,thisitem.itemnum,thisclient->items[slotid].count);
          return true;
        }

		switch(thisitem.itemtype)
		{
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 8:
            case 9:
            case 14:
            {
                float price = 0;
                price = 7.142857E-05F * 5000;
                if(thisitem.itemtype<10)
                {
                    price *= EquipList[thisitem.itemtype].Index[thisitem.itemnum]->price;
                }
                else
                {
                    price *= PatList.Index[thisitem.itemnum]->price;
                }
                price *= thisitem.durability + 0xc8;
                price *= 40;
                price *= 0xc8 - 0x62; //town rate
                price *= 1.000000E-06;
                price = (float)floor(price);
    			Log( MSG_INFO, "%s:: Item Sold: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
        		thisclient->CharInfo->Zulies += (long int)price*count;
            }
            break;
            case 10:
            case 12:
            {
                // this values are the same from packet 753
                BYTE values[11] = {0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32};
                UINT type = 0;
                UINT bprice = 0;
                UINT pricerate = 0;
                UINT pvalue = 0;
                if(thisitem.itemtype==10)
                {
                   type = UseList.Index[thisitem.itemnum]->type;
                   bprice = UseList.Index[thisitem.itemnum]->price;
                   pricerate = UseList.Index[thisitem.itemnum]->pricerate;
                   pvalue = UseList.Index[thisitem.itemnum]->pricevalue;
                }
                else
                {
                   type = NaturalList.Index[thisitem.itemnum]->type;
                   bprice = NaturalList.Index[thisitem.itemnum]->price;
                   pricerate = NaturalList.Index[thisitem.itemnum]->pricerate;
                   pvalue = NaturalList.Index[thisitem.itemnum]->pricevalue;
                }
                unsigned int value = 0;
                float price = 0;
                bool flag;
                if(type<421)
                {
                    if(type<311)
                        flag = false;
                    else
                        if(type>312)
                            flag = false;
                        else
                            flag = true;
                }
                else
                {
                    if(type<=428)
                        flag = true;
                    else
                        if(type<311)
                            flag = false;
                        else
                            flag = true;
                }
                if(flag)
                {
                    value = pvalue;
                    if(value>=11)
                        value ^= 0xffffffff;
                    else
                        value = values[value];
                    value -= 0x32;
                    value *= pricerate;
                    value += 1000;
                    value *= bprice;
                    value *= (200 - 0x62); //town rate ( 100)
                    price = value * 5.555555555555556E-06;
                    price = (float)floor(price);
                	Log( MSG_INFO, "%s:: Item Sold: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                    thisclient->CharInfo->Zulies += (long int)price*count;
                }
                else
                {
                    float price = pricerate;
                    unsigned int value = 0x61 - 0x32; // misc rate -0x32
                    price *= value;
                    price += 1000;
                    price *= bprice;
                    price *= (200 - 0x62); //town rate ( 100)
                    price *= 5.555555555555556E-06;
                    price = (float)floor(price);
                	Log( MSG_INFO, "%s:: Item Sold: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
                    thisclient->CharInfo->Zulies += (long int)price*count;
                }
            }
            break;
            case 7:
            case 11:
            case 13:
            {
                float price = 0;
                UINT bprice = 0;
                UINT pricerate = 0;
                switch(thisitem.itemtype)
                {
                    case 7:
                        pricerate = EquipList[thisitem.itemtype].Index[thisitem.itemnum]->pricerate;
                        bprice = EquipList[thisitem.itemtype].Index[thisitem.itemnum]->price;
                    break;
                    case 11:
                        pricerate = JemList.Index[thisitem.itemnum]->pricerate;
                        bprice = JemList.Index[thisitem.itemnum]->price;
                    break;
                    case 13:continue;//can we sell quest items? :S

                }
                price = pricerate;
                price *= 0x61 - 0x32;   // misc rate -0x32
                price += 1000;
                price *= bprice;
                price *= (200 - 0x62); //town rate ( 100)
                price *= 5.555555555555556E-06;
                price = (float)floor(price);
        		Log( MSG_INFO, "%s:: Item Sold: itemnum %i, itemtype %i, itemcount %i, price %0.0f",thisclient->CharInfo->charname,thisitem.itemnum, thisitem.itemtype, thisitem.count, price);
            	thisclient->CharInfo->Zulies += (long int)price*count;
            }
            break;
            default:
                Log( MSG_WARNING, "Invalid Item Type: %i", thisitem.itemtype );
        }
        thisclient->items[slotid].count	-= count;
        if( thisclient->items[slotid].count <=0 )
    		ClearItem( thisclient->items[slotid] );
		ADDBYTE( pak, slotid );
		ADDDWORD( pak, BuildItemHead( thisclient->items[slotid] ) );
		ADDDWORD( pak, BuildItemData( thisclient->items[slotid] ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );

		ncount++;
	   //Saving item in database now.
	   thisclient->SaveSlot41(slotid);
	}

	SETQWORD( pak, 0, thisclient->CharInfo->Zulies );
	SETBYTE( pak, 8, ncount );
	thisclient->client->SendPacket( &pak );


	return true;
}

// Attack skill
bool CWorldServer::pakStartSkill ( CPlayer* thisclient, CPacket* P )
{
    if( thisclient->Shop->open || thisclient->Status->Stance==DRIVING || thisclient->Status->Mute !=0xff )
        return true;
    fPoint thispoint;
    UINT targetid = GETWORD( (*P), 0 );
    UINT skillnum = GETWORD( (*P), 2 );

    if(skillnum>=MAX_ALL_SKILL)
    {
        Log( MSG_HACK, "Invalid Skill id %i for %s ", skillnum, thisclient->CharInfo->charname );
        return true;
    }

    Log( MSG_INFO, "pakStartSkill for %s (%i)", thisclient->CharInfo->charname,skillnum);

    //LMA: Some Basic skills (double scratch) has no level...
    unsigned int skillid = thisclient->cskills[skillnum].id+thisclient->cskills[skillnum].level-1;
    if(thisclient->cskills[skillnum].id!=0&&thisclient->cskills[skillnum].level==0)
    {
        if (thisclient->GoodSkill(thisclient->cskills[skillnum].id)!=2)
        {
            Log( MSG_HACK, "Invalid Skill id %i has no level and is not basic for %s ", skillnum, thisclient->CharInfo->charname );
            return true;
        }

        skillid = thisclient->cskills[skillnum].id;
    }

    CMap* map = MapList.Index[thisclient->Position->Map];
    CCharacter* character = map->GetCharInMap( targetid );
    if(character==NULL) return true;

	if(character->IsMonster())
	{
        if(character->Stats->HP>MAXHPMOB)
        {
            character->Stats->HP=(long long) MAXHPMOB;
        }

	   BEGINPACKET( pak, 0x79f );
	   ADDWORD( pak, character->clientid );
	   /*ADDWORD( pak, character->Stats->HP );
	   ADDWORD( pak, 0 );*/
	   ADDDWORD   ( pak, character->Stats->HP );    //LMA: DDWORD :)
	   thisclient->client->SendPacket( &pak );
    }
	CSkills* thisskill = GetSkillByID( skillid );
	if(thisskill==NULL)
	   return true;
	if(thisskill->target==9 && !character->IsDead())
	{
        ClearBattle( thisclient->Battle );
        return true;
    }
    if( isSkillTargetFriendly( thisskill ) )
    {
        Log(MSG_INFO,"Buff because friendly");
        thisclient->StartAction( character, SKILL_BUFF, skillid );
    }
    else
    {
        Log(MSG_INFO,"Attack because foe");
        thisclient->StartAction( character, SKILL_ATTACK, skillid );
    }
	return true;
}

// Trade action
bool CWorldServer::pakTradeAction ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
	CPacket pak;
	BYTE action = GETBYTE( (*P), 0 );
	thisclient->Trade->trade_target = GETWORD( (*P), 1 );
	CPlayer* otherclient = GetClientByID( thisclient->Trade->trade_target, thisclient->Position->Map );
	if (otherclient==NULL) return true;
	switch(action)
    {
		case 0:
			// REQUEST TRADE
			RESETPACKET( pak, 0x7c0 );
			ADDBYTE( pak, 0 );
			ADDWORD( pak, thisclient->clientid );
			ADDBYTE( pak, 0 );
			otherclient->client->SendPacket( &pak );
			thisclient->Trade->trade_status=2;
			otherclient->Trade->trade_status=1;
			break;
		case 1:
			// ACCEPT TRADE
			RESETPACKET( pak, 0x7c0 );
			ADDBYTE( pak, 1 );
			ADDWORD( pak, thisclient->clientid );
			ADDBYTE( pak, 0 );
			otherclient->client->SendPacket( &pak );
			thisclient->Trade->trade_status=3;
			otherclient->Trade->trade_status=3;
			for(int i=0; i<11; i++) thisclient->Trade->trade_count[i]=0;
			for(int i=0; i<10; i++) thisclient->Trade->trade_itemid[i]=0;
			for(int i=0; i<11; i++) otherclient->Trade->trade_count[i]=0;
			for(int i=0; i<10; i++) otherclient->Trade->trade_itemid[i]=0;
			break;
		case 3:
			RESETPACKET( pak, 0x7c0 );
			ADDBYTE( pak, 3 );
			ADDWORD( pak, thisclient->clientid );
			ADDBYTE( pak, 0 );
			otherclient->client->SendPacket( &pak );
			thisclient->Trade->trade_target=0;
			otherclient->Trade->trade_target=0;
			thisclient->Trade->trade_status=0;
			thisclient->Trade->trade_status=0;
			break;
		case 4:
			RESETPACKET( pak, 0x7c0 );
			ADDBYTE( pak, 4 );
			ADDWORD( pak, thisclient->clientid );
			ADDBYTE( pak, 0 );
			otherclient->client->SendPacket( &pak );
			thisclient->Trade->trade_status=4;
			break;
		case 6:
			if (thisclient->Trade->trade_status==6)
				thisclient->Trade->trade_status=4;
			else
				thisclient->Trade->trade_status=6;
			if (otherclient->Trade->trade_status==6)
            {
				RESETPACKET( pak, 0x7c0 );
				// Complete the trade
				ADDBYTE( pak, 6 );
				ADDWORD( pak, otherclient->clientid );
				ADDBYTE( pak, 0 );
				thisclient->client->SendPacket( &pak );
				RESETPACKET( pak, 0x7c0 );
				ADDBYTE( pak, 6 );
				ADDWORD( pak, thisclient->clientid );
				ADDBYTE( pak, 0 );
				otherclient->client->SendPacket( &pak );

				// Check if user has enough zuly
                if ( thisclient->CharInfo->Zulies < thisclient->Trade->trade_count[0x0a])
                {
                  Log(MSG_HACK, "[TRADE] Player %s has to trade %li, but has only %li",thisclient->CharInfo->charname,thisclient->Trade->trade_count[0x0a],thisclient->CharInfo->Zulies);
                  return true;
                }
                //LMA: bug... surely...
                //if ( thisclient->CharInfo->Zulies < otherclient->Trade->trade_count[0x0a]) return true;
                if ( otherclient->CharInfo->Zulies < otherclient->Trade->trade_count[0x0a])
                {
                  Log(MSG_HACK, "[TRADE] Player %s has to trade %li, but has only %li",otherclient->CharInfo->charname,otherclient->Trade->trade_count[0x0a],otherclient->CharInfo->Zulies);
                  return true;
                }

				// Update the zuly
				//LMA: anti hack...
				long long zulythis=0;
				long long zulyother=0;
				zulythis=thisclient->CharInfo->Zulies;
				zulyother=otherclient->CharInfo->Zulies;

				thisclient->CharInfo->Zulies -= thisclient->Trade->trade_count[0x0a];
				otherclient->CharInfo->Zulies -= otherclient->Trade->trade_count[0x0a];
				thisclient->CharInfo->Zulies += otherclient->Trade->trade_count[0x0a];
				otherclient->CharInfo->Zulies += thisclient->Trade->trade_count[0x0a];

				unsigned tucount = 0;
				unsigned oucount = 0;

				// Begin this clients inventory update
				BEGINPACKET( pakt, 0x717 );
				ADDQWORD( pakt, thisclient->CharInfo->Zulies );
				ADDBYTE( pakt, 0 );

				// Begin the other clients inventory update
				BEGINPACKET( pako, 0x717 );
				ADDQWORD( pako, otherclient->CharInfo->Zulies );
				ADDBYTE( pako, 0 );

                //LMA: check before actually giving the items to players...
  				for (unsigned i=0; i<10; i++)
                {
                    //LMA: Dupe test.
                    for (int ii=i+1;ii<10;ii++)
                    {
                        if(thisclient->Trade->trade_itemid[i]!=0&&(thisclient->Trade->trade_itemid[i]==thisclient->Trade->trade_itemid[ii]))
                        {
                          Log(MSG_HACK, "[TRADE] Player %s tried to dupe item in slot %i (%u::%u)",thisclient->CharInfo->charname,thisclient->Trade->trade_itemid[i],thisclient->items[thisclient->Trade->trade_itemid[i]].itemtype,thisclient->items[thisclient->Trade->trade_itemid[i]].itemnum);
                          thisclient->CharInfo->Zulies=zulythis;
                          otherclient->CharInfo->Zulies=zulyother;
                          return true;
                        }

                        if(otherclient->Trade->trade_itemid[i]!=0&&(otherclient->Trade->trade_itemid[i]==otherclient->Trade->trade_itemid[ii]))
                        {
                          Log(MSG_HACK, "[TRADE] Player %s tried to dupe item in slot %i (%u::%u)",otherclient->CharInfo->charname,otherclient->Trade->trade_itemid[i],otherclient->items[otherclient->Trade->trade_itemid[i]].itemtype,otherclient->items[otherclient->Trade->trade_itemid[i]].itemnum);
                          thisclient->CharInfo->Zulies=zulythis;
                          otherclient->CharInfo->Zulies=zulyother;
                          return true;
                        }

                    }

					if(thisclient->Trade->trade_count[i] > 0)
                    {
						//LMA: anti hack check.
						if(thisclient->items[thisclient->Trade->trade_itemid[i]].count < thisclient->Trade->trade_count[i])
						{
                          Log(MSG_HACK, "[TRADE] Player %s has to trade %i [%i:%i], but has only %i",thisclient->CharInfo->charname,thisclient->Trade->trade_count[i],thisclient->items[thisclient->Trade->trade_itemid[i]].itemtype,thisclient->items[thisclient->Trade->trade_itemid[i]].itemnum,thisclient->items[thisclient->Trade->trade_itemid[i]].count);
          				  thisclient->CharInfo->Zulies=zulythis;
        				  otherclient->CharInfo->Zulies=zulyother;
                          return true;
                        }

					}
					if(otherclient->Trade->trade_count[i] > 0)
                    {
						//LMA: anti hack check.
						if(otherclient->items[otherclient->Trade->trade_itemid[i]].count<otherclient->Trade->trade_count[i])
						{
                          Log(MSG_HACK, "[TRADE] Player %s has to trade %i [%i:%i], but has only %i",otherclient->CharInfo->charname,otherclient->Trade->trade_count[i],otherclient->items[otherclient->Trade->trade_itemid[i]].itemtype,otherclient->items[otherclient->Trade->trade_itemid[i]].itemnum,otherclient->items[otherclient->Trade->trade_itemid[i]].count);
          				  thisclient->CharInfo->Zulies=zulythis;
        				  otherclient->CharInfo->Zulies=zulyother;
                          return true;
                        }

					}
				}

                //Ok, go, it should be ok now...
				for (unsigned i=0; i<10; i++)
                {
					if(thisclient->Trade->trade_count[i] > 0)
                    {
						CItem thisitem = thisclient->items[thisclient->Trade->trade_itemid[i]];
						thisitem.sig_data=0;
						thisitem.sig_head=0;
						thisitem.sig_gem=0;
						thisitem.sp_value=0;
						thisitem.last_sp_value=0;
						unsigned newslot = otherclient->GetNewItemSlot( thisitem );
						if(newslot==0xffff) continue;
						thisclient->items[thisclient->Trade->trade_itemid[i]].count -= thisclient->Trade->trade_count[i];
						if( thisclient->items[thisclient->Trade->trade_itemid[i]].count<=0)
      						ClearItem(thisclient->items[thisclient->Trade->trade_itemid[i]]);
                        if (otherclient->items[newslot].count > 0)
                            thisitem.count = otherclient->items[newslot].count + thisclient->Trade->trade_count[i];
                        else
                            thisitem.count = thisclient->Trade->trade_count[i];
                        otherclient->items[newslot]=thisitem;
						ADDBYTE( pakt, (unsigned char)thisclient->Trade->trade_itemid[i] );
						ADDDWORD( pakt, BuildItemHead( thisclient->items[thisclient->Trade->trade_itemid[i]] ) );
						ADDDWORD( pakt, BuildItemData( thisclient->items[thisclient->Trade->trade_itemid[i]] ) );
                        ADDDWORD( pakt, 0x00000000 );
                        ADDWORD ( pakt, 0x0000 );
						ADDBYTE( pako, newslot );
						ADDDWORD( pako, BuildItemHead( otherclient->items[newslot] ) );
						ADDDWORD( pako, BuildItemData( otherclient->items[newslot] ) );
                        ADDDWORD( pako, 0x00000000 );
                        ADDWORD ( pako, 0x0000 );
						tucount++;
						oucount++;
						//Saving slots in database.
						otherclient->SaveSlot41(newslot);
						thisclient->SaveSlot41(thisclient->Trade->trade_itemid[i]);
					}
					if(otherclient->Trade->trade_count[i] > 0)
                    {
						CItem thisitem = otherclient->items[otherclient->Trade->trade_itemid[i]];
						thisitem.sig_data=0;
						thisitem.sig_head=0;
						thisitem.sig_gem=0;
                        thisitem.sp_value=0;
                        thisitem.last_sp_value=0;
						unsigned newslot = thisclient->GetNewItemSlot( thisitem );
						if(newslot==0xffff) continue;
						otherclient->items[otherclient->Trade->trade_itemid[i]].count -= otherclient->Trade->trade_count[i];
						if( otherclient->items[otherclient->Trade->trade_itemid[i]].count<=0 )
      						ClearItem( otherclient->items[otherclient->Trade->trade_itemid[i]] );
                        if (thisclient->items[newslot].count > 0)
                            thisitem.count = thisclient->items[newslot].count + otherclient->Trade->trade_count[i];
                        else
                            thisitem.count = otherclient->Trade->trade_count[i];
                        thisclient->items[newslot]=thisitem;
						ADDBYTE( pako, (unsigned char)otherclient->Trade->trade_itemid[i] );
						ADDDWORD( pako, BuildItemHead( otherclient->items[otherclient->Trade->trade_itemid[i]] ) );
						ADDDWORD( pako, BuildItemData( otherclient->items[otherclient->Trade->trade_itemid[i]] ) );
                        ADDDWORD( pako, 0x00000000 );
                        ADDWORD ( pako, 0x0000 );
						ADDBYTE( pakt, newslot );
						ADDDWORD( pakt, BuildItemHead( thisclient->items[newslot] ) );
						ADDDWORD( pakt, BuildItemData( thisclient->items[newslot] ) );
                        ADDDWORD( pakt, 0x00000000 );
                        ADDWORD ( pakt, 0x0000 );
						tucount++;
						oucount++;
						//Saving slots in database.
						thisclient->SaveSlot41(newslot);
                        otherclient->SaveSlot41(otherclient->Trade->trade_itemid[i]);
					}
				}

				// Update itemcount and send
				SETBYTE( pakt, 8, tucount );
				SETBYTE( pako, 8, oucount );
				thisclient->client->SendPacket( &pakt );
				otherclient->client->SendPacket( &pako );
				thisclient->Trade->trade_status=0;
				otherclient->Trade->trade_status=0;
			}
			break;
	}

	return true;
}

// Trade action
bool CWorldServer::pakTradeAdd ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
	BYTE islot = GETBYTE((*P),0);
	WORD slotid = GETWORD((*P),1);
	DWORD count = GETDWORD((*P),3);
	CPlayer* otherclient = GetClientByID( thisclient->Trade->trade_target, thisclient->Position->Map );
	if (otherclient==NULL) return true;
	BEGINPACKET( pak, 0x7c1 );
	ADDBYTE( pak, islot );
	if( islot == 0x0a )
    {
        if (count > thisclient->CharInfo->Zulies)
           return true;
		ADDDWORD( pak, 0xccccccdf );
		ADDDWORD( pak, count );
        ADDDWORD( pak, 0xcccccccc );
        ADDWORD ( pak, 0xcccc );
		thisclient->Trade->trade_count[islot] = count;
	}
    else
    {
        if(islot>0x0a)
            return false;
		if( count != 0 )
        {
            if(count>thisclient->items[slotid].count)
                return false;
			thisclient->Trade->trade_count[islot] = count;
			thisclient->Trade->trade_itemid[islot] = slotid;
			CItem tmpitem = thisclient->items[slotid]; tmpitem.count = count;
			ADDDWORD( pak, BuildItemHead( tmpitem ) );
			ADDDWORD( pak, BuildItemData( tmpitem ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
		}
        else
        {
			thisclient->Trade->trade_count[islot] = 0;
			thisclient->Trade->trade_itemid[islot] = 0;
			ADDDWORD( pak, 0 );
			ADDDWORD( pak, 0 );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
		}
	}
	otherclient->client->SendPacket( &pak );
	return true;
}

// Send Quest information (QSD version)
void CWorldServer::pakQuestData( CPlayer *thisclient )
{
    BEGINPACKET (pak, 0x71b);
    for(dword i = 0; i < 5; i++) ADDWORD( pak, thisclient->quest.EpisodeVar[i]);
    for(dword i = 0; i < 3; i++) ADDWORD( pak, thisclient->quest.JobVar[i]);
    for(dword i = 0; i < 7; i++) ADDWORD( pak, thisclient->quest.PlanetVar[i]);
    for(dword i = 0; i < 10; i++) ADDWORD( pak, thisclient->quest.UnionVar[i]);

    for( unsigned i = 0; i < 10; i++ )
    {
        ADDWORD( pak,  thisclient->quest.quests[i].QuestID );
        long int Time = 0;
        if (thisclient->quest.quests[i].QuestID > 0 && STB_QUEST.rows[thisclient->quest.quests[i].QuestID][1] > 0) {
            Time += thisclient->quest.quests[i].StartTime; // Start time
            Time += STB_QUEST.rows[thisclient->quest.quests[i].QuestID][1] * 10; // Time to finish
            Time -= time(NULL); // Current time
            if (Time < 0) Time = 0; // Time is up
            Time /= 10; // Divide to get 10's of seconds
        }
        ADDDWORD( pak, Time ); // Time Left
        for(dword j = 0; j < 10; j++) ADDWORD( pak, thisclient->quest.quests[i].Variables[j]);
        for(dword j = 0; j < 4; j++) ADDBYTE( pak, thisclient->quest.quests[i].Switches[j]);
        for(dword j = 0; j < 6; j++) {
            if (thisclient->quest.quests[i].Items[j].itemnum != 0) {
                //ADDWORD( pak,  thisclient->quest.quests[i].Items[j].GetPakHeader() );
                //ADDDWORD( pak,  thisclient->quest.quests[i].Items[j].GetPakData() );
                ADDDWORD( pak, BuildItemHead( thisclient->quest.quests[i].Items[j] ) );
                ADDDWORD( pak, BuildItemData( thisclient->quest.quests[i].Items[j] ) );
                ADDDWORD( pak, 0x00000000 );
                ADDWORD ( pak, 0x0000 );
            } else {
                //ADDWORD( pak,  0 );
                //ADDDWORD( pak, 0 );
                ADDDWORD( pak, 0 );
                ADDDWORD( pak, 0 );
                ADDDWORD( pak, 0x00000000 );
                ADDWORD ( pak, 0x0000 );
            }
        }
    }
    for( unsigned i = 0; i < 0x40; i++ )
        ADDBYTE( pak, thisclient->quest.flags[i] );

    //LMA: new fields:
    //beginning of clan vars?
    for( unsigned i = 0; i < 12; i++ )
        ADDBYTE( pak, 0x00 );

    //clan vars?
    for( unsigned i = 0; i < 8; i++ )
        ADDBYTE( pak, 0x00 );

    // Wishlist.
    /*for (unsigned i = 0; i < 180; i++)
        ADDBYTE( pak, 0 );*/

    //LMA: Getting wishlist.
    GetWishlist(thisclient);

    for (unsigned i = 0; i < MAX_WISHLIST; i++)
    {
        ADDDWORD( pak, thisclient->wishlistitems[i].head);
        ADDDWORD( pak, thisclient->wishlistitems[i].data );
        ADDDWORD( pak, 0x00 );
        ADDWORD( pak, 0x00 );
    }


    thisclient->client->SendPacket( &pak );
}

// Handle quest triggers
bool CWorldServer::pakGiveQuest( CPlayer* thisclient, CPacket* P )
{
  byte action = GETBYTE((*P),0);
  byte slot = GETBYTE((*P),1);
  dword hash = GETDWORD((*P),2);


  if( thisclient->questdebug )
  {
      SendPM( thisclient, "Event Trigger [%08x] Action %i", hash, action);
  }

  LogDebugPriority(3);
  LogDebug("PakGiveQuest %u ([%08x]), action %i slot %i",hash,hash,action,slot);
  Log(MSG_INFO,"PakGiveQuest %u ([%08x]), action %i slot %i",hash,hash,action,slot);
  LogDebugPriority(4);

  if (action == 2)
  {
    if( thisclient->questdebug ) SendPM( thisclient, "Delete quest - Slot %i", slot);
    for (dword i = slot; i < 9; i++) thisclient->quest.quests[i] = thisclient->quest.quests[i+1];
    thisclient->quest.quests[9].Clear();
    BEGINPACKET ( pak, 0x730);
    ADDBYTE ( pak, 0x03);
    ADDBYTE ( pak, slot);
    ADDDWORD( pak, hash);
    thisclient->client->SendPacket(&pak);

      LogDebugPriority(3);
      LogDebug("PakGiveQuest %u ([%08x]), action %i slot %i delete quest",hash,hash,action,slot);
      LogDebugPriority(4);
    return true;
  }

  if (action != 3)
  {
      return false;
  }

  int success = thisclient->ExecuteQuestTrigger(hash);

  BEGINPACKET ( pak, 0x730);
  ADDBYTE ( pak, success);
  ADDBYTE ( pak, 0);
  ADDDWORD( pak, hash);
  thisclient->client->SendPacket(&pak);

  Log(MSG_INFO,"PakGiveQuest %u end",hash);

  LogDebugPriority(3);
  LogDebug("PakGiveQuest %u ([%08x]), action %i slot %i result %i",hash,hash,action,slot,success);
  LogDebugPriority(4);


  return true;
}

// Self skills
bool CWorldServer::pakSkillSelf( CPlayer* thisclient, CPacket* P )
{
    if( thisclient->Shop->open || thisclient->Status->Stance==DRIVING ||
        thisclient->Status->Mute!=0xff || !thisclient->Status->CanCastSkill)
        return true;
    WORD num = GETWORD((*P),0);
    if(num>=MAX_ALL_SKILL)
    {
        Log( MSG_HACK, "Invalid Skill id %i for %s ", num, thisclient->CharInfo->charname );
        return true;
    }

	unsigned int skillid = thisclient->cskills[num].id+thisclient->cskills[num].level-1;
	CSkills* thisskill = GetSkillByID( skillid );
	Log( MSG_INFO, "pakSkillSelf for %s (slot %i, skill %i)", thisclient->CharInfo->charname,num,skillid);

	if(thisskill == NULL)
	   return true;
	unsigned int skilltarget = thisskill->target;
    unsigned int skillrange = thisskill->aoeradius;

	if( thisskill->aoe == 0 )
	{
        thisclient->StartAction( NULL, BUFF_SELF, skillid );
    }
    else
    {
        if(isSkillTargetFriendly( thisskill ))
        {
            thisclient->StartAction( NULL, BUFF_AOE, skillid );
        }
        else
        {
            thisclient->StartAction( NULL, SKILL_AOE, skillid );
        }
    }
    return true;
}

// Consumible Items
bool CWorldServer::pakUseItem ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    BYTE slot = GETBYTE((*P),0);
    if(!CheckInventorySlot( thisclient, slot))
        return false;
    if( thisclient->items[slot].count<=0 )
        return true;
    CUseInfo* thisuse = GetUseItemInfo( thisclient, slot);
    if(thisuse == NULL)
    {
        Log(MSG_WARNING,"[%i]Invalid Item, Item: %i. - Type: %i",
        thisclient->clientid, thisclient->items[slot].itemnum,
        thisclient->items[slot].itemtype);
        return true;
    }
    bool flag = false;
    switch(thisuse->usescript)
    {
       case 0: //  cherry berrys
            thisclient->StartAction( NULL, BUFF_SELF, thisuse->usevalue );
            //thisclient->items[slot].count -= 1;
            //if( thisclient->items[slot].count == 0 )
            //    ClearItem( thisclient->items[slot] );
            flag = true;
        break;
        case 1: // Food
        {
            thisclient->UsedItem->usevalue = thisuse->usevalue;
            thisclient->UsedItem->usetype = thisuse->usetype;
            thisclient->UsedItem->userate = 15;
            thisclient->UsedItem->used = 0;
            BEGINPACKET( pak,0x7a3 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, thisuse->itemnum );
            SendToVisible( &pak, thisclient );
            flag = true;
        }
        break;
        case 2: // Return Scroll
        {
            BEGINPACKET( pak,0x7a3 );
            ADDWORD    ( pak,thisclient->clientid );
            ADDWORD    ( pak, thisuse->itemnum );
            ADDBYTE    ( pak,slot );
            thisclient->client->SendPacket( &pak );
            //thisclient->items[slot].count -= 1;
            //if( thisclient->items[slot].count == 0 )
            //    ClearItem( thisclient->items[slot] );
            fPoint thispoint;
            thispoint.x = floor( thisuse->usevalue/10000 );
            thispoint.y = floor( thisuse->usevalue%10000 );
            TeleportTo ( thisclient, thisuse->usetype, thispoint );
            flag = true;
        }
        break;
        case 3: // Charm Scroll
        {
	        CSkills* thisskill = GetSkillByID( thisuse->usetype );
            if(thisskill == NULL)
            {
                Log(MSG_WARNING, "Char %s tried to use invalid scroll type: %i", thisclient->CharInfo->charname, thisuse->usetype );
                delete thisuse;
          	    return true;
            }
        	int skilltarget = thisskill->target;
            int skillrange = thisskill->aoeradius;
        	if( thisskill->aoe == 0 )
        	{
                thisclient->StartAction( NULL,BUFF_SELF,thisuse->usetype );
            }
            else
            {
                if(isSkillTargetFriendly( thisskill ))
                {
                    thisclient->StartAction( NULL,BUFF_AOE,thisuse->usetype );
                }
                else
                {
                    thisclient->StartAction( NULL,SKILL_AOE,thisuse->usetype );
                }
            }
            flag = true;
        }
        break;
        case 4: // Dance Scroll | FireCrackers | Emotion
        {
            //Start Animation
            BEGINPACKET( pak, 0x7b2 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, thisuse->usevalue );
            SendToVisible( &pak, thisclient );
            //Finish Animation
            RESETPACKET( pak, 0x7bb );
            ADDWORD    ( pak, thisclient->clientid );
            SendToVisible( &pak, thisclient );
            //????
            RESETPACKET( pak, 0x7b9);
            ADDWORD    ( pak, thisclient->clientid);
            ADDWORD    ( pak, thisuse->usevalue );
	        SendToVisible( &pak, thisclient );
            flag = true;
        }
        break;
        case 5: // Summons
        {
            CNPCData* thisnpc = GetNPCDataByID( thisuse->usevalue );
            if(thisnpc==NULL)
            {
                Log( MSG_WARNING,"[%i]NPCDATA NOT Founded: %i.",
                                    thisclient->clientid,thisuse->usevalue );
                delete thisuse;
                return true;
            }
            //Start Animation
            BEGINPACKET( pak, 0x7b2 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, thisuse->usetype );
            //ADDBYTE    ( pak, 6 );
            SendToVisible( &pak, thisclient );

            /*
            //Finish Animation
            RESETPACKET( pak, 0x7bb );
            ADDWORD    ( pak, thisclient->clientid );
            SendToVisible( &pak, thisclient );
            */

            //????
            RESETPACKET( pak, 0x7b9);
            ADDWORD    ( pak, thisclient->clientid);
            ADDWORD    ( pak, thisuse->usetype );
	        SendToVisible( &pak, thisclient );

	        // Add our Mob to the mobs list
           	fPoint position = RandInCircle( thisclient->Position->current, 5 );
           	CMap* map = MapList.Index[thisclient->Position->Map];
           	CMonster * this_summon=map->AddMonster( thisuse->usevalue, position, thisclient->clientid );

            if (this_summon!=NULL)
            {
           	   this_summon->skillid=thisuse->usetype;
           	   this_summon->buffid=thisuse->use_buff;

           	   //LMA: new HP amount.
                this_summon->Stats->MaxHP=SummonFormula(thisclient,this_summon);
                this_summon->Stats->HP=this_summon->Stats->MaxHP;
            }

            flag = true;
        }
        break;
        case 6: // Snowball
        {
            WORD clientid = GETWORD((*P),2);
            fPoint thispoint;
            CMap* map = MapList.Index[thisclient->Position->Map];
            CCharacter* character = map->GetCharInMap( clientid );
            if(character==NULL) return true;
            thisclient->StartAction( character, SKILL_BUFF, thisuse->usevalue );
            flag = true;
        }
        break;
        case 7: // scrolls [damage,maxhp,...]
        {
	        CSkills* thisskill = GetSkillByID( thisuse->usetype );
            if(thisskill == NULL)
            {
                Log(MSG_WARNING, "Char %s tried to use invalid scroll type: %i", thisclient->CharInfo->charname, thisuse->usetype );
                delete thisuse;
          	    return true;
            }
        	int skilltarget = thisskill->target;
            int skillrange = thisskill->aoeradius;
        	if( thisskill->aoe == 0 )
        	{
                thisclient->StartAction( NULL, BUFF_SELF, thisuse->usetype );
            }
            else
            {
                if(isSkillTargetFriendly( thisskill ))
                {
                    thisclient->StartAction( NULL, BUFF_AOE, thisuse->usetype );
                }
                else
                {
                    thisclient->StartAction( NULL, SKILL_AOE, thisuse->usetype );
                }
            }
            flag = true;
        }
        break;
        case 10: // Skill Book
        {
            flag = LearnSkill( thisclient, thisuse->usevalue );
        }
        break;
        case 11:
        {
             //LMA: some special skill cases where it's handled in quest itself :)
             flag=false;
        }
        break;
        case 12:
        {
             //LMA: Refuel :)
            thisclient->TakeFuel(thisuse->usevalue);
            flag=true;
        }
        break;
        case 13:
        {
             //LMA: Clan Points :)
            thisclient->GiveCP(thisuse->usevalue);
            flag=true;
        }
        break;
        case 14:
        {
            //LMA: PY's code, different script for dance scrolls
            CSkills* thisskill = GetSkillByID( thisuse->usetype );
            if(thisskill == NULL)
            {
                Log(MSG_WARNING, "Char %s tried to use invalid Dance scroll type: %i", thisclient->CharInfo->charname, thisuse->usetype );
                delete thisuse;
                return true;
            }
            Log(MSG_INFO, "Char %s used a Dance scroll type: %i", thisclient->CharInfo->charname, thisuse->usetype );
            int skilltarget = thisskill->target;
            int skillrange = thisskill->aoeradius;
            flag = true;
            Log(MSG_INFO, "Sending skill request. AOE range = %i", skillrange);
            if(skillrange > 0 ) //AOE effect
            {
                thisclient->StartAction( NULL,BUFF_AOE,thisuse->usetype );
            }
            else
            {
                thisclient->StartAction( NULL,BUFF_SELF,thisuse->usetype );
            }
            flag=true;
        }
        break;
        case 15: //LMA: Rocks
        {
            WORD clientid = GETWORD((*P),2);
            fPoint thispoint;
            CMap* map = MapList.Index[thisclient->Position->Map];
            CCharacter* character = map->GetCharInMap( clientid );
            if(character==NULL) return true;
            thisclient->StartAction( character, SKILL_ATTACK, thisuse->usevalue );
            flag = true;
        }
        break;
        case 16: //LMA: bombs
        {
            WORD clientid = GETWORD((*P),2);

            //LMA: for now we use this...
            fPoint thispoint;
            CMap* map = MapList.Index[thisclient->Position->Map];
            CCharacter* character = map->GetCharInMap( clientid );
            if(character==NULL)
            {
                return true;
            }

            thisclient->StartAction( character, SKILL_ATTACK, thisuse->usevalue );
            flag = true;
        }
        break;
        case 17:
        {
            //LMA: Skill resets...
            int l_b=0;
            int l_e=0;
            switch(thisuse->itemnum)
            {
                case 95:
                case 898:
                case 900:
                {
                    //Job skill.
                    l_b=0;
                    l_e=60;
                }
                break;
                case 96:
                case 901:
                {
                    //unique
                    l_b=90;
                    l_e=120;
                }
                break;
                case 97:
                case 902:
                {
                    //mileage
                    l_b=120;
                    l_e=320;
                }
                break;
                case 98:
                case 903:
                {
                    //all skills
                    l_b=0;
                    l_e=320;
                }
                break;
                default:
                {
                    Log(MSG_WARNING,"Unknown 323 case for %i",thisuse->itemnum);
                }
                break;

            }

            if(l_b==0&&l_e==0)
                return true;

            //let's delete some skills...
            for (int k=l_b;k<l_e;k++)
            {
                thisclient->cskills[k].id=0;
                thisclient->cskills[k].level=1;
                thisclient->cskills[k].thisskill=NULL;
            }

            //LMA: for tests.
            thisclient->saveskills();
            BEGINPACKET( pak,0x7a3 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, thisuse->itemnum );
            SendToVisible( &pak, thisclient );

            thisclient->items[slot].count -= 1;
            if( thisclient->items[slot].count <= 0 )
                ClearItem( thisclient->items[slot] );
            thisclient->UpdateInventory(slot,0xffff);
            flag=false;
        }
        break;
    }
    if(flag == true)
    {
        thisclient->items[slot].count -= 1;
        if( thisclient->items[slot].count <= 0 )
            ClearItem( thisclient->items[slot] );
        BEGINPACKET( pak,0x7a3 );
        ADDWORD    ( pak, thisclient->clientid );
        ADDWORD    ( pak, thisuse->itemnum );
        ADDBYTE    ( pak, slot );
        thisclient->client->SendPacket( &pak );
    }
    delete thisuse;
    return true;
}

//LMA: Level UP Skill (new way)
bool  CWorldServer::pakLevelUpSkill( CPlayer *thisclient, CPacket* P )
{
    WORD pos = GETWORD ((*P),0);   // number of skill. appears to be an array index
    WORD skill = GETWORD ((*P),2); // skill id
    if(pos >= MAX_ALL_SKILL)
    {
        Log( MSG_HACK, "Invalid Skill id %i for %s ", pos, thisclient->CharInfo->charname );
        return false;
    }
    CSkills* thisskill = GetSkillByID( skill );
    if(thisskill==NULL)
        return true;
    if(thisclient->cskills[pos].id != skill - thisclient->cskills[pos].level)
    {
        Log(MSG_WARNING,"Skill Upgrade, wrong org skill %i != %i",thisclient->cskills[pos].id,skill - thisclient->cskills[pos].level);
        return true;
    }

    // checks made for prerequisite skills here.
    UINT hasPreskill = 0;
    for(int i=0;i<3;i++)
    {
        int preskill = thisskill->rskill[i];
        if(thisskill->lskill[i] > 0)
            preskill += thisskill->lskill[i] - 1;
        if(preskill == 0)
        {
            hasPreskill ++; // no preskill defined in this element so give a credit then skip to the next preskill.
            continue;
        }
        //Log( MSG_INFO, "[DEBUG] Checking %i / 3, preskill %i",i,preskill);

        for(int skillid=0;skillid<MAX_ALL_SKILL;skillid++)
        {
            //Log( MSG_INFO, "[DEBUG] Skillid %i < %u",skillid,MAX_SKILL);
            if(thisclient->cskills[skillid].id == thisskill->rskill[i] && thisclient->cskills[skillid].level >= thisskill->lskill[i])
            //int checkskill = thisclient->cskills[skillid].id + thisclient->cskills[skillid].level -1;
            //if(preskill == checkskill)
            {
                hasPreskill ++;
                //Log( MSG_INFO, "[DEBUG] Skill matched to requirements");
                break; // no need to carry on. Skill found
            }
        }
   }

    //Log( MSG_INFO, "[DEBUG] hasPreskill state = %i",hasPreskill);
    if(hasPreskill != 3)
    {
        Log( MSG_INFO, "Prerequisite skills not all found" );
        return true; //doesn't have the necessary prerequisite skill
    }

    // is character a high enough level?
    if(thisskill->clevel > thisclient->Stats->Level)
    {
        Log( MSG_INFO, "Character level too low" );
        return true; //not high enough level
    }

    //check that it is the next skill in the series
    if(thisclient->cskills[pos].id != skill - thisclient->cskills[pos].level) //check that it is the next skill in the series
    {
        Log( MSG_INFO, "Skill Id doesn't match next in the CSkills series" );
        return true;
    }

    if(thisclient->CharInfo->SkillPoints >= thisskill->sp)
    {
       thisclient->CharInfo->SkillPoints -= 1;

       BEGINPACKET( pak, 0x7b1 );
       ADDBYTE    ( pak, 0x00);
       ADDWORD    ( pak, pos);
       ADDWORD    ( pak, skill);
       ADDWORD    ( pak, thisclient->CharInfo->SkillPoints);
       thisclient->client->SendPacket( &pak );
       thisclient->SetStats( );

       thisclient->cskills[pos].level+=1;
       thisclient->cskills[pos].thisskill = thisskill;

       //thisclient->UpgradeSkillInfo(pos,thisclient->cskills[pos].id,1);
       thisclient->saveskills();
    }


    return true;
}


// Equip bullets arrows and cannons
bool CWorldServer::pakEquipABC ( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    BYTE itemslot = GETBYTE((*P),0);
    int slot = ((itemslot-32)/4)+72;
    int dest = 0;
    int type = (itemslot-32)%4;
    if(!CheckInventorySlot( thisclient, slot))
        return false;
    if(itemslot>3)
    {
        dest = type+132;
    }
    else
    {
        slot = itemslot + 132;
        dest = thisclient->GetNewItemSlot( thisclient->items[slot] );
        if(dest==0xffff)
            return true;
        type=300;
    }
	CItem tmpitm = thisclient->items[slot];
	thisclient->items[slot] =thisclient->items[dest];
	thisclient->items[dest] = tmpitm;

	thisclient->UpdateInventory( dest, slot );
    BEGINPACKET( pak, 0x7ab );
    ADDWORD    ( pak, thisclient->clientid );
    unsigned int effect = ( thisclient->items[dest].itemnum * 32 ) + type;
    ADDWORD    ( pak, effect );
    SendToVisible( &pak, thisclient );
    thisclient->Stats->Attack_Power = thisclient->GetAttackPower( );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////
// Craft item  (From RageZone)  (Crafting modifications by: Killerfly)  	//
/////////////////////////////////////////////////////////////////////////////////

bool CWorldServer::pakCraft( CPlayer* thisclient, CPacket* P )
{
    //0x00=success
    //0x01=failure
    //0x02=not enough MP
    //0x03=not enough materials
    //0x04=no materials
    //0x05=craft skill level is insufficient.
    bool painting=false;
	if(thisclient->Shop->open==true)
        return true;
	CItem item;
	item.count = 1;
	// item durability randomizer
	int lowest = thisclient->Attr->Con/ 10 + 17;
	int highest = thisclient->Attr->Con / 10 + 70;
	int range=(highest-lowest)+1;
	item.durability = lowest+int(range*rand()/(RAND_MAX + 1.0));

	// durability set
	item.itemnum = (GETWORD((*P), 3));
	item.itemtype = (GETBYTE((*P), 0x2));
	item.lifespan = 100; //Its new so 100%
	item.refine = 0;

	// stats randomizer
	int changeofstatslow = thisclient->Attr->Sen / 13 + 10;
	int changeofstatshigh = thisclient->Attr->Sen / 13 + 50;
	int changeofstatsrange = (changeofstatshigh-changeofstatslow)+1;
	if (changeofstatslow+int(changeofstatsrange*rand()/(RAND_MAX + 1.0)) > 50)
	{
        int statslowget = 1;
        int statshighget = 256;
        int setstatrange=(statshighget-statslowget)+1;
        item.stats = statslowget+int(setstatrange*rand()/(RAND_MAX + 1.0));
        Log(MSG_INFO,"Craft item has a stat %i",item.stats);
        item.appraised = 1;
	}
	else
	{
        item.stats = 0;
        item.appraised = 0;
        Log(MSG_INFO,"Craft item has NO stat");
	}


    //LMA: Craft success or failure?
    int failure=0;
    UINT craftID=0; //LMA: Searching skill
    UINT craft_level=0; //LMA: Searching skill
    item.sp_value=0;
    item.last_sp_value=-1;
    switch(item.itemtype)
    {
        case 10:
        {
            //Use
            failure=UseList.Index[item.itemnum]->craft_difficult;
            craftID=UseList.Index[item.itemnum]->craft;
            craft_level=UseList.Index[item.itemnum]->craftlevel;
        }
        break;
        case 11:
        {
            //JEM
            failure=JemList.Index[item.itemnum]->craft_difficult;
            craftID=JemList.Index[item.itemnum]->craft;
            craft_level=JemList.Index[item.itemnum]->craft_level;
        }
        break;
        case 14:
        {
            //PAT
            failure=PatList.Index[item.itemnum]->craft_difficult;
            item.sp_value=item.lifespan*10;
            craftID=PatList.Index[item.itemnum]->craft;
            craft_level=PatList.Index[item.itemnum]->craft_level;

            //LMA: Let's check if it's a painting job.
            for(char used=5; used != 13; used +=2)
            {
                WORD material= GETWORD((*P), used);
                if(CheckInventorySlot(thisclient,material))
                {
                    //PAT painting: 12::442
                    if (thisclient->items[material].itemnum==442&&thisclient->items[material].itemtype==12)
                    {
                        painting=true;
                        break;
                    }

                }

            }

        }
        break;
        default:
        {
            //Equiplist.
            if(item.itemtype>=1&&item.itemtype<=9)
            {
                failure=EquipList[item.itemtype].Index[item.itemnum]->craft_difficult;
                craftID=EquipList[item.itemtype].Index[item.itemnum]->craft;
                craft_level=EquipList[item.itemtype].Index[item.itemnum]->craft_level;
            }
            else
            {
                Log(MSG_HACK,"Incorrect Itemtype in craft %i, player %s",item.itemtype,thisclient->CharInfo->charname);
                return true;
            }

        }
        break;
    }

    //LMA: Searching for the good skill in player's skill list.
    if(craftID==0||craft_level==0)
    {
        Log(MSG_HACK,"Player %s tried to craft item %i::%i with incorrect craft values %i,%i",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level);
        return true;
    }

    bool skill_found=true;

    //LMA: no MP taken and no failure when painting.
    if(!painting)
    {
        skill_found=false;

        for (int k=0;k<MAX_ALL_SKILL;k++)
        {
            if(thisclient->cskills[k].thisskill==NULL||thisclient->cskills[k].thisskill->skilltype!=2)
            {
                continue;
            }

            if(thisclient->cskills[k].thisskill->atkpower==craftID&&thisclient->cskills[k].level>=craft_level)
            {
                skill_found=true;
                //LMA: checking MP amount.
                for (int j=0;j<2;j++)
                {
                    if(thisclient->cskills[k].thisskill->costtype[j]==0||thisclient->cskills[k].thisskill->costamount[j]==0)
                    {
                        continue;
                    }

                    if(thisclient->cskills[k].thisskill->costtype[j]==A_MP)
                    {
                        if(thisclient->Stats->MP<thisclient->cskills[k].thisskill->costamount[j])
                        {
                            Log(MSG_WARNING,"Player %s tried to craft item %i::%i, craft values %i,%i, not enough MP",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level);
                            //not enough MP
                            BEGINPACKET( pak, 0x07d8);
                            ADDWORD( pak, thisclient->clientid );
                            ADDBYTE( pak, 0x02);
                            ADDBYTE( pak, 0x00);
                            thisclient->client->SendPacket( &pak );
                            return true;
                        }

                        thisclient->Stats->MP-=thisclient->cskills[k].thisskill->costamount[j];
                    }
                    else if(thisclient->cskills[k].thisskill->costtype[j]==A_HP)
                    {
                        if(thisclient->Stats->HP<thisclient->cskills[k].thisskill->costamount[j])
                        {
                            Log(MSG_WARNING,"Player %s tried to craft item %i::%i, craft values %i,%i, not enough HP",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level);
                            //not enough HP
                            BEGINPACKET( pak, 0x07d8);
                            ADDWORD( pak, thisclient->clientid );
                            ADDBYTE( pak, 0x02);
                            ADDBYTE( pak, 0x00);
                            thisclient->client->SendPacket( &pak );
                            return true;
                        }

                        thisclient->Stats->HP-=thisclient->cskills[k].thisskill->costamount[j];
                    }
                    else
                    {
                        Log(MSG_WARNING,"Player %s tried to craft item %i::%i, craft values %i,%i, unknown costype %i",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level,thisclient->cskills[k].thisskill->costtype[j]);
                    }

                }

                break;
            }

        }

        if (!skill_found)
        {
            Log(MSG_HACK,"Player %s tried to craft item %i::%i, craft values %i,%i but without the skill...",thisclient->CharInfo->charname,item.itemtype,item.itemnum,craftID,craft_level);
            BEGINPACKET( pak, 0x07d8);
            ADDWORD( pak, thisclient->clientid );
            ADDBYTE( pak, 0x05);
            ADDBYTE( pak, 0x00);
            thisclient->client->SendPacket( &pak );
            return true;
        }

        //end of search.


        failure-=(int) (thisclient->Attr->Con/40);
        if (failure<=0)
            failure=1;
        if (failure>=100)
            failure=99;

        //LMA: Failure.
        bool test_fail=false;
        //test_fail=true;
        if((GServer->RandNumber(0,100)<=failure)||test_fail)
        {
            Log(MSG_INFO,"Craft has failed");

            BEGINPACKET( pak, 0x07d8);
            ADDWORD( pak, thisclient->clientid );
            ADDBYTE( pak, 0x00);
            ADDBYTE( pak, item.itemtype);
            ADDWORD( pak, item.itemnum);// item id not shifted
            thisclient->client->SendPacket(&pak);

            RESETPACKET( pak, 0x07af);
            ADDBYTE( pak, 0x01);

            int nb_items=0;
            int nb_fails=0;
            for(char used=5; used != 13; used +=2)
            {
                if(GETWORD((*P), used)!=0)
                {
                    nb_items++;
                }

            }

            nb_fails=GServer->RandNumber(0,nb_items);

            if(nb_fails>(nb_items-1))
            {
                nb_fails=nb_items-1;
            }

            if(nb_fails<0)
            {
                nb_fails=0;
            }

            ADDWORD( pak, nb_fails);

            int bar[4];
            bar[0] = item.durability * 9;
            bar[1] = changeofstatsrange * 9;
            bar[2] = item.durability + changeofstatsrange * 6;
            bar[3] = item.durability + changeofstatsrange + bar[2] / 3;

            nb_items=0;
            for(char used=5; used != 13; used +=2)
            {
                if(GETWORD((*P), used)!=0&&nb_items<=nb_fails)
                {
                    ADDWORD( pak, bar[nb_items]);
                }
                else
                {
                    ADDWORD( pak, 0x99a0);
                }

                nb_items++;
            }

            ADDDWORD(pak, 0xCD48FB40);
            ADDDWORD(pak, 0x00CD01CD);
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x03E8 );
            thisclient->client->SendPacket( &pak );

            //deleting first item in player's inventory.
            int	materialnumber = 0;
            if(item.itemtype<10)
            {
                materialnumber = EquipList[item.itemtype].Index[item.itemnum]->material;
            }
            else
            {
                switch(item.itemtype)
                {
                    //UseList by core
                    case 10:materialnumber = UseList.Index[item.itemnum]->material;break;
                    case 11:materialnumber = JemList.Index[item.itemnum]->material;break;
                    case 14:materialnumber = PatList.Index[item.itemnum]->material;break;
                }
            }

            nb_items=0;
            int	m = 0;
            for(char used=5; used != 13; used +=2)
            {
                if(nb_items>nb_fails)
                {
                    break;
                }

                WORD material= GETWORD((*P), used);
                if (material != 0)
                {
                    if(!CheckInventorySlot( thisclient, material))
                    {
                        Log(MSG_HACK, "Player: %s, can't delete items in craft failure0 slot %i", thisclient->CharInfo->charname,material);
                        return true;
                    }

                    if (thisclient->items[material].count < ProductList.Index[materialnumber]->amount[m])
                    {
                        Log(MSG_HACK, "Player: %s, can't delete items in craft failure1", thisclient->CharInfo->charname);
                        return true;
                    }

                    UINT clientMat = (thisclient->items[material].itemtype * 1000) + thisclient->items[material].itemnum;

                    if (ProductList.Index[materialnumber]->item[m]>0&&clientMat != ProductList.Index[materialnumber]->item[m])
                    {
                        Log(MSG_HACK, "Player: %s, can't delete items in craft failure2", thisclient->CharInfo->charname);
                        return true;
                    }

                    //LMA: The first material can be generic.
                    if(m==0&&ProductList.Index[materialnumber]->item[m]==0&&ProductList.Index[materialnumber]->item_0_family!=0)
                    {
                        if(ReturnItemType(thisclient,material)!=ProductList.Index[materialnumber]->item_0_family)
                        {
                            Log(MSG_HACK, "Player: %s, can't delete items in craft failure3 (incorrect type)", thisclient->CharInfo->charname);
                            return true;
                        }

                    }

                    thisclient->items[material].count -= ProductList.Index[materialnumber]->amount[m];
                    m++;

                    if (thisclient->items[material].count <=0)
                    {
                        ClearItem(thisclient->items[material]);
                    }

                }

                nb_items++;
            }

            //LMA: saving slots.
            for(char used=5; used != 13; used +=2)
            {
                WORD slot= GETWORD((*P), used);
                if (slot == 0)
                {
                    continue;
                }

                thisclient->SaveSlot41(slot);
            }


            return true;
        }

    }

	// stats set
	item.socketed = 0;
	item.gem = 0;

	unsigned newslot= thisclient->GetNewItemSlot( item );
	if (newslot !=0xffff)
    {
		//get material used lookup number
		int	materialnumber = 0;
		if(item.itemtype<10)
		{
            materialnumber = EquipList[item.itemtype].Index[item.itemnum]->material;
        }
        else
        {
            switch(item.itemtype)
            {
                //UseList by core
                case 10:materialnumber = UseList.Index[item.itemnum]->material;break;
                case 11:materialnumber = JemList.Index[item.itemnum]->material;break;
                case 14:materialnumber = PatList.Index[item.itemnum]->material;break;
            }
        }

		int	m = 0;

		//LMA: TEST fix 4th tab by Shakar
		//for(char used=5; used != 11; used +=2)
		for(char used=5; used != 13; used +=2)
        {
            WORD material= GETWORD((*P), used);//gets inventory location
            if (material != 0)
            {
                if(!CheckInventorySlot( thisclient, material))
                {
                    BEGINPACKET( pak, 0x07d8);
                    ADDWORD( pak, thisclient->clientid );
                    ADDBYTE( pak, 0x03);
                    ADDBYTE( pak, 0x00);
                    thisclient->client->SendPacket( &pak );
                    return true;
                }

                if (thisclient->items[material].count < ProductList.Index[materialnumber]->amount[m])
                {
                    Log(MSG_HACK, "Nb Client craft mats don't equal server. Player: %s", thisclient->CharInfo->charname);
                    BEGINPACKET( pak, 0x07d8);
                    ADDWORD( pak, thisclient->clientid );
                    ADDBYTE( pak, 0x03);
                    ADDBYTE( pak, 0x00);
                    thisclient->client->SendPacket( &pak );
                    return true;
                }

                UINT clientMat = (thisclient->items[material].itemtype * 1000) + thisclient->items[material].itemnum;
                //Log(MSG_HACK, "Clientmat: %i | Productlist: %i | materialnumber: %i | material: %i", clientMat, ProductList.Index[materialnumber]->item[m], materialnumber, material);

                //LMA: sometimes there is only a check on the amount.
                if (ProductList.Index[materialnumber]->item[m]>0&&clientMat != ProductList.Index[materialnumber]->item[m])
                {
                    Log(MSG_HACK, "Client craft mats don't equal server. Player: %s", thisclient->CharInfo->charname);
                    BEGINPACKET( pak, 0x07d8);
                    ADDWORD( pak, thisclient->clientid );
                    ADDBYTE( pak, 0x03);
                    ADDBYTE( pak, 0x00);
                    thisclient->client->SendPacket( &pak );
                    return true;
                }

                //LMA: The first material can be generic.
                if(m==0&&ProductList.Index[materialnumber]->item[m]==0&&ProductList.Index[materialnumber]->item_0_family!=0)
                {
                    if(ReturnItemType(thisclient,material)!=ProductList.Index[materialnumber]->item_0_family)
                    {
                        Log(MSG_HACK, "Player: %s, can't delete items in craft failure3 (incorrect type)", thisclient->CharInfo->charname);
                        BEGINPACKET( pak, 0x07d8);
                        ADDWORD( pak, thisclient->clientid );
                        ADDBYTE( pak, 0x03);
                        ADDBYTE( pak, 0x00);
                        thisclient->client->SendPacket( &pak );
                        return true;
                    }

                }

                thisclient->items[material].count -= ProductList.Index[materialnumber]->amount[m];
                m++;

                if (thisclient->items[material].count <=0)
                {
                    ClearItem(thisclient->items[material]);
                }

            }

        }

		thisclient->items[newslot] = item;

        BEGINPACKET( pak, 0x07d8);
        ADDWORD( pak, thisclient->clientid );

        //LMA: Itemtype?
        //ADDWORD( pak, 0x0100);
        ADDBYTE( pak, 0x00);
        ADDBYTE( pak, item.itemtype);

        ADDWORD( pak, item.itemnum);// item id not shifted
        thisclient->client->SendPacket(&pak);

        RESETPACKET( pak, 0x07af);
        ADDBYTE( pak, 0x00);//00
        ADDWORD( pak, newslot);

        // Make craft bars
        int bar1 = item.durability * 9;
        int bar2 = changeofstatsrange * 9;
        int bar3 = item.durability + changeofstatsrange * 6;
        int bar4 = item.durability + changeofstatsrange + bar3 / 3;

        /*ADDWORD( pak, bar1);//progress bar1 0 is empty 0x0400 is full bar
        ADDWORD( pak, bar2);//progress bar2 0 is empty 0x0400 is full bar*/
        if ((GETWORD((*P),  5))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar1);}
        if ((GETWORD((*P),  7))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar2);}
        if ((GETWORD((*P),  9))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar3);}//progress bar3 0 is empty 0x0400 is full bar
        if ((GETWORD((*P), 11))==0){ ADDWORD( pak, 0x99a0);}else{ ADDWORD( pak, bar4);}//progress bar4 0 is empty 0x0400 is full bar
        ADDDWORD(pak, BuildItemHead(item));

         if (item.itemtype == 11)
         {
             ADDWORD( pak, 0x0001);// amount
             ADDWORD( pak, 0x0000);
         }
         else
         {
             ADDDWORD(pak, BuildItemData(item));
         }

        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
        thisclient->client->SendPacket(&pak);

        //LMA: No Exp for painting.
        if(!painting)
        {
            int crafting_exp = item.durability + changeofstatsrange * (thisclient->Stats->Level/ 15);
            thisclient->CharInfo->Exp += crafting_exp;//  add exp
            RESETPACKET( pak, 0x79b );
            ADDDWORD   ( pak, thisclient->CharInfo->Exp );
            ADDWORD    ( pak, thisclient->CharInfo->stamina );
            ADDWORD    ( pak, 0 );
            thisclient->client->SendPacket( &pak );
        }

     }
     else
     {
        BEGINPACKET( pak, 0x07d8);
        ADDWORD( pak, thisclient->clientid );
        ADDBYTE( pak, 0x03);
        ADDBYTE( pak, 0x00);
        thisclient->client->SendPacket( &pak );

         RESETPACKET (pak, 0x702);
         ADDSTRING(pak, "No free slot !");
         ADDBYTE(pak, 0);
         thisclient->client->SendPacket(&pak);
     }

    //LMA: saving slots.
    for(char used=5; used != 13; used +=2)
    {
        WORD slot= GETWORD((*P), used);
        if (slot == 0)
        {
            continue;
        }

        thisclient->SaveSlot41(slot);
    }

    //saving "result".
    if(newslot!=0xffff)
    {
        thisclient->SaveSlot41(newslot);
    }


     return true;
}


///////////////////////////////////////////////////////////////////////////////////
// After Craft  (From RageZone)  (if you coded this tell me to add your name)    //
///////////////////////////////////////////////////////////////////////////////////
bool CWorldServer::pakModifiedItemDone( CPlayer* thisclient, CPacket* P )
{
     DWORD result = GETDWORD((*P), 0x00 );
     BEGINPACKET( pak, 0x07d8);
     ADDWORD( pak, thisclient->clientid );
     ADDDWORD( pak, result);
     SendToVisible( &pak , thisclient);
     return true;

}

// Aoe Skill
bool CWorldServer::pakSkillAOE( CPlayer* thisclient, CPacket* P)
{
    Log(MSG_INFO,"[pakSkillAOE] AOE_TARGET");

    if( thisclient->Shop->open || thisclient->Status->Stance==DRIVING ||
        thisclient->Status->Mute!=0xff || !thisclient->Status->CanCastSkill) return true;
    UINT num = GETWORD( (*P), 0 );

    //LMA: position of monster targeted (in fact it's a zone, monster is just a mean to an end).
    thisclient->Position->aoedestiny.x=GETFLOAT((*P), 0x02 )/100;
    thisclient->Position->aoedestiny.y=GETFLOAT((*P), 0x06 )/100;
    thisclient->Position->aoedestiny.z=0;

    //Log(MSG_INFO,"AOE Current %.2f,%.2f",thisclient->Position->current.x,thisclient->Position->current.y);
    //Log(MSG_INFO,"[pakSkillAOE] num=%i,x=%.2f, y=%.2f",num,thisclient->Position->aoedestiny.x,thisclient->Position->aoedestiny.y);
    if(num>=MAX_ALL_SKILL)
    {
        Log( MSG_HACK, "Invalid Skill id %i for %s ", num, thisclient->CharInfo->charname );
        return false;
    }

    Log( MSG_INFO, "pakSkillAOE for %s (%i)", thisclient->CharInfo->charname,num);

    unsigned int skillid = thisclient->cskills[num].id+thisclient->cskills[num].level-1;
    //Log(MSG_INFO,"[pakSkillAOE] skillid=%i",skillid);
    CSkills* thisskill = GetSkillByID( skillid );
    if(thisskill==NULL) return true;
    if(thisskill->aoe==1)
    {
        thisclient->StartAction( NULL , AOE_TARGET, skillid );
    }


    return true;
}

// Identify Item
bool CWorldServer::pakidentify( CPlayer* thisclient, CPacket* P)
{
    WORD itemslot = GETWORD ((*P),0);
    if(!CheckInventorySlot( thisclient, itemslot))
        return false;
    thisclient->items[itemslot].appraised = true;
    BEGINPACKET( pak, 0x7ba );
    ADDWORD    ( pak, itemslot );
    ADDBYTE    ( pak, 0x00 );
    thisclient->client->SendPacket( &pak );
    return true;
}

// Show Storage Items
bool CWorldServer::pakStorage( CPlayer* thisclient, CPacket* P)
{
    BYTE action = GETBYTE((*P),0);
    //LMA: handling Japanese version
    BYTE page = GETBYTE((*P),1);

    switch(action)
    {
        case 0x00:
        {
             //LMA: get storage from database to be sure.
             //GetAllStorage(thisclient);
            BEGINPACKET( pak, 0x7ad );
            ADDBYTE    ( pak, 0x00 );
            ADDBYTE    ( pak, thisclient->nstorageitems ); //numero de items
            for(int i=0;i<160;i++)
            {
           		if( thisclient->storageitems[i].itemtype !=0 )
           		{
                    ADDBYTE    ( pak, i );
                  	ADDDWORD   ( pak, BuildItemHead( thisclient->storageitems[i] ) );
               		ADDDWORD   ( pak, BuildItemData( thisclient->storageitems[i] ) );
                    ADDDWORD( pak, 0x00000000 );
                    ADDWORD ( pak, 0x0000 );
                }
            }
            ADDQWORD( pak, thisclient->CharInfo->Storage_Zulies );
            thisclient->client->SendPacket( &pak );
        }
        break;
        default:
            Log( MSG_INFO, "Storage unknown action: %i ", action);
    }
    return true;
}

// Change Storage (Deposit/Withdraw items)
//2do: take Zulyes from player's money when getting / putting items from / into storage.
bool CWorldServer::pakChangeStorage( CPlayer* thisclient, CPacket* P)
{
    BYTE action = GETBYTE((*P),0);
    switch(action)
    {
        case 0x00: //Deposit
        {
            BYTE itemslot = GETBYTE((*P),1);
            if(!CheckInventorySlot( thisclient, itemslot ))
                return false;

            CItem newitem = thisclient->items[itemslot];
 			//Maxxon: Deposit Fee
            int storageprice=0;
            switch (newitem.itemtype) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                {
                    // EQ
                    CEquip* neweq = EquipList[newitem.itemtype].Index[newitem.itemnum];
                    if (neweq == NULL) {
                        storageprice = -1;
                    } else {
                        storageprice = (neweq->price / 200 + 1) * (neweq->pricerate + 1);
                    }
                }
                    break;
                case 10:
                {
                    // consumes
                    CUseData* newuse = UseList.Index[newitem.itemnum];
                    if (newuse == NULL) {
                        storageprice = -1;
                    } else {
                        storageprice = GETWORD((*P),6) * ((newuse->price / 200 + 1) * (newuse->pricerate + 1));
                    }
                }
                    break;
                case 11:
                {
                    // gems
                    CJemData* thisjem = JemList.Index[newitem.itemnum];
                    if (thisjem == NULL) {
                        storageprice = -1;
                    } else {
                        storageprice = GETWORD((*P),6) * ((thisjem->price / 200 + 1) * (thisjem->pricerate + 1));
                    }
                }
                    break;
                case 12:
                {
                    // mats
                    CNaturalData* newnatural = NaturalList.Index[newitem.itemnum];
                    if (newnatural == NULL) {
                        storageprice = -1;
                    } else {
                        storageprice = GETWORD((*P),6) * ((newnatural->price / 200 + 1) * (newnatural->pricerate + 1));
                    }
                }
                    break;
                case 14:
                {
                    // PAT
                    CPatData* newpat = PatList.Index[newitem.itemnum];
                    if (newpat == NULL) {
                        storageprice = -1;
                    } else {
                        storageprice = (newpat->price / 200 + 1) * (newpat->pricerate + 1);
                    }
                }
                    break;
                default:
                    Log (MSG_ERROR, "pakChangeStorage: item type %i unknown!", newitem.itemtype);
                    storageprice = -1;
                    break;
            }

            if (storageprice < 0) {
                Log(MSG_HACK, "pakChangeStorage: %s tried to hack your server slot %i, price %i",thisclient->CharInfo->charname,itemslot,storageprice);
                return false;
            }

            if (storageprice <= thisclient->CharInfo->Zulies) {
                thisclient->CharInfo->Zulies -= storageprice;
            } else {
                // not enough zulies
                return true;
            }
			//End of deposit Fee.

            if(newitem.itemtype>9 && newitem.itemtype<14)
            {
                WORD count = GETWORD((*P),6);
                if(count>thisclient->items[itemslot].count)
                    count = thisclient->items[itemslot].count;
                newitem.count = count;
                thisclient->items[itemslot].count -= count;
                if(thisclient->items[itemslot].count<=0)
                    ClearItem(thisclient->items[itemslot]);
            }
            else
            {
                ClearItem(thisclient->items[itemslot]);
            }
            int newslot = thisclient->GetNewStorageItemSlot ( newitem );
            if(newslot==0xffff)
                return true;
            //Log(MSG_INFO,"New (?) slot for deposit: %i",newslot);

            //LMA: New code (stackables?)
            if (thisclient->storageitems[newslot].itemnum!=0)
            {
               //Log(MSG_INFO,"it should be a stackable in slot: %i, from %i (+%i)",newslot,thisclient->storageitems[newslot].count,newitem.count);
               newitem.count+=thisclient->storageitems[newslot].count;
               //Log(MSG_INFO,"so new=%i",newitem.count);
            }
            else
            {
                //Log(MSG_INFO,"it should be a new slot: %i",newslot);
                thisclient->nstorageitems++;
            }

            BEGINPACKET( pak, 0x7ae );
            ADDWORD    ( pak, itemslot );
            ADDWORD    ( pak, newslot );
            if (Config.jrose==1)
                ADDWORD ( pak, 0x00 );            //LMA:Jrose (thanks z)
	       	ADDDWORD   ( pak, BuildItemHead( thisclient->items[itemslot] ) );
    		ADDDWORD   ( pak, BuildItemData( thisclient->items[itemslot] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
            ADDDWORD   ( pak, BuildItemHead( newitem ) );
            ADDDWORD   ( pak, BuildItemData( newitem ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
    		ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            thisclient->client->SendPacket( &pak );

            thisclient->storageitems[newslot] = newitem;

            //LMA: need to save the storage item...
            SaveSlotStorage(thisclient,newslot);
            //We save the inventory slot as well
            thisclient->SaveSlot41(itemslot);
        }
        break;//thanks to anon for post that this break was missing
        case 0x01: //Withdraw
        {
            BYTE storageslot = GETBYTE((*P),1);
            if(storageslot>=160)
            {
                Log( MSG_HACK, "%s, Invalid storage slot %i from %s",thisclient->CharInfo->charname, storageslot, thisclient->Session->username );
                return false;
            }

            //LMA: get the slot concerned to refresh it from MySQL storage
            if(!GetSlotStorage(thisclient,storageslot))
            {
                Log( MSG_HACK, "%s, Invalid storage slot %i from %s (from Mysql)",thisclient->CharInfo->charname, storageslot, thisclient->Session->username );
                return false;
            }

            //CItem newitem =  newitem = thisclient->storageitems[storageslot];
            CItem newitem = thisclient->storageitems[storageslot];
            if(newitem.itemtype>9 && newitem.itemtype<14)
            {
                WORD count = GETWORD((*P),6);
                if( count>thisclient->storageitems[storageslot].count )
                    count = thisclient->storageitems[storageslot].count;
                newitem.count = count;
                thisclient->storageitems[storageslot].count -= count;
                if(thisclient->storageitems[storageslot].count<=0)
                    ClearItem(thisclient->storageitems[storageslot]);
            }
            else
            {
                ClearItem(thisclient->storageitems[storageslot]);
            }

            int newslot= thisclient->GetNewItemSlot ( newitem );
            //no place in player's inventory, so back to storage.
            if(newslot==0xffff)
            {
                thisclient->storageitems[storageslot] = newitem;
                return true;
            }

            int amount = 0;
            if(thisclient->items[newslot].count>0)
            {
              int amount = thisclient->items[newslot].count;
			  newitem.count+=amount;
            }
            if( newitem.count > 999 )
            {
                amount = 999 - newitem.count;
                newitem.count = 999;
            }
            thisclient->items[newslot] = newitem;
            if( amount > 0 )
            {
                newitem.count = amount;
                unsigned int newslot2 = thisclient->GetNewItemSlot( newitem );
                if( newslot2 == 0xffff )
                {
                    thisclient->storageitems[storageslot] = thisclient->items[newslot];
                    thisclient->items[newslot].count = amount;
                    return true;
                }
                thisclient->items[newslot2] = newitem;
                thisclient->UpdateInventory( newslot2 );
            }
            BEGINPACKET( pak, 0x7ae );
            ADDWORD    ( pak, newslot );
            ADDWORD    ( pak, storageslot );
            if (Config.jrose==1)
                ADDWORD ( pak, 0x00 );            //LMA:Jrose (thanks z)
	       	ADDDWORD   ( pak, BuildItemHead( thisclient->items[newslot] ) );
    		ADDDWORD   ( pak, BuildItemData( thisclient->items[newslot] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
            ADDDWORD   ( pak, BuildItemHead( thisclient->storageitems[storageslot] ) );
            ADDDWORD   ( pak, BuildItemData( thisclient->storageitems[storageslot] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );


    		ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x00 );

            thisclient->client->SendPacket( &pak );

            if(thisclient->storageitems[storageslot].itemnum==0)
                thisclient->nstorageitems--;

            //LMA: need to save the storage item...
            SaveSlotStorage(thisclient,storageslot);
            //We save the inventory slot as well
            thisclient->SaveSlot41(newslot);
        }
        break;
        default:
            Log( MSG_INFO, "Storage unknown action: %i ", action);
    }
    return true;
}

// Deposit/Widthdraw zulies
bool CWorldServer::pakStoreZuly( CPlayer* thisclient, CPacket* P)
{
    BYTE action = GETBYTE ((*P), 0 );
    QWORD zuly = GETQWORD ((*P), 1 );
    switch(action)
    {
        case 0x10://deposit'
        {
            if(zuly > thisclient->CharInfo->Zulies)
                return true;
            thisclient->CharInfo->Zulies -= zuly;
            thisclient->CharInfo->Storage_Zulies += zuly;
            BEGINPACKET( pak, 0x7da );
            ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDQWORD   ( pak, thisclient->CharInfo->Storage_Zulies );
            ADDBYTE    ( pak, 0x00 );
            thisclient->client->SendPacket( &pak );

            //LMA: Saving Zuly Storage
            SaveZulyStorage(thisclient);
        }
        break;
        case 0x11://withdraw
        {
             //LMA: refreshing Zuly from MySQL...
             if(!GetZulyStorage(thisclient))
                   return true;
            if(zuly > thisclient->CharInfo->Storage_Zulies )
                return true;
            thisclient->CharInfo->Zulies += zuly;
            thisclient->CharInfo->Storage_Zulies -= zuly;
            BEGINPACKET( pak, 0x7da );
            ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDQWORD   ( pak, thisclient->CharInfo->Storage_Zulies );
            ADDBYTE    ( pak, 0x00 );
            thisclient->client->SendPacket( &pak );

            //LMA: Saving Zuly Storage
            SaveZulyStorage(thisclient);
        }
        break;
        default:
            Log( MSG_INFO, "Storage unknown action: %i ", action);
    }
    return true;
}


// Open Shop
bool CWorldServer::pakOpenShop( CPlayer* thisclient, CPacket* P )
{
     //LMA 139+:
     //They added six extra 0x00 between items and prices + shop name...
     UINT lma_offset=6;


    if( thisclient->Shop->open )
        return true;
    BYTE nselling = GETBYTE((*P),0);
    BYTE nbuying = GETBYTE((*P),1);
    if(nselling>30 || nbuying>30)
        return true;
    int nchar = ((nselling + nbuying) * (12+lma_offset) ) + nselling + nbuying + 2;
    strncpy(thisclient->Shop->name ,(char*)&(*P).Buffer[nchar] , P->Size-nchar );

    Log(MSG_INFO,"[Create] Begin Shop %s:",thisclient->Shop->name);
    thisclient->Shop->Selling = nselling;
    thisclient->Shop->Buying = nbuying;
    for(int i=0;i<nselling;i++)
    {
        int n=(i*(13+lma_offset))+2;
        BYTE slot =  GETBYTE((*P),n);
        if(!CheckInventorySlot( thisclient, slot))
            return false;
        thisclient->Shop->SellingList[i].slot = slot;
        if( thisclient->items[slot].itemtype>9 && thisclient->items[slot].itemtype<14 )
        {
            int itemCount = GETWORD((*P),n+5);
            if (thisclient->items[slot].count < itemCount)
               return true;
            thisclient->Shop->SellingList[i].count = itemCount; // geo edit for invisible stackables // 30 sep 07
        }
        else
            thisclient->Shop->SellingList[i].count = 1;
        thisclient->Shop->SellingList[i].price = GETDWORD((*P),n+9+lma_offset);

        //LMA Log:
        Log(MSG_INFO,"[S-%i/%i], Item (%i:%i), slot %i, Nb %i, Price %i",i+1,nselling,thisclient->items[slot].itemtype,thisclient->items[slot].itemnum,slot,thisclient->Shop->SellingList[i].count,thisclient->Shop->SellingList[i].price);
    }
    for(int i=0;i<nbuying;i++)
    {
        unsigned int n=(nselling*(13+lma_offset))+2+(i*(12+lma_offset))+i;
        BYTE slot = GETBYTE((*P),n);
        thisclient->Shop->BuyingList[slot].slot = slot;
        DWORD head = GETDWORD((*P),n+1);
        DWORD data = GETDWORD((*P),n+5);
        CItem thisitem = GetItemByHeadAndData( head, data );
        thisclient->Shop->BuyingList[slot].item = thisitem;
        if( thisitem.itemtype>9 && thisitem.itemtype<14 )
            thisclient->Shop->BuyingList[slot].count = GETWORD((*P),n+5);
        else
            thisclient->Shop->BuyingList[slot].count = 1;
        thisclient->Shop->BuyingList[slot].price = GETDWORD((*P),n+9+lma_offset);

        //LMA Log:
        Log(MSG_INFO,"[B-%i/%i], Item (%i:%i), slot %i, Nb %i, Price %i",i+1,nbuying,thisitem.itemtype,thisitem.itemnum,slot,thisclient->Shop->BuyingList[slot].count,thisclient->Shop->BuyingList[slot].price);
    }

    Log(MSG_INFO,"[Create] End Shop %s.",thisclient->Shop->name);

    BEGINPACKET( pak, 0x796 );
    ADDWORD    ( pak, thisclient->clientid );
    ADDFLOAT   ( pak, thisclient->Position->current.x );
    ADDFLOAT   ( pak, thisclient->Position->current.y );
    ADDWORD    ( pak, 0x9057 );//>???
    SendToVisible( &pak, thisclient );

    RESETPACKET( pak, 0x7c2 );
    ADDWORD    ( pak, thisclient->clientid );
    ADDWORD    ( pak, thisclient->Shop->ShopType );
    ADDSTRING  ( pak, thisclient->Shop->name );
    ADDBYTE    ( pak, 0x00 );
    SendToVisible( &pak, thisclient );
    thisclient->Shop->open = true;
    return true;
}


// Show Shop to other players
bool CWorldServer::pakShowShop( CPlayer* thisclient, CPacket* P )
{
    WORD otherclientid = GETWORD((*P),0);
    CPlayer* otherclient = GetClientByID ( otherclientid, thisclient->Position->Map );
    if(otherclient==NULL)
        return true;
    BEGINPACKET( pak, 0x7c4 );
    ADDBYTE    ( pak, otherclient->Shop->Selling );
    ADDBYTE    ( pak, otherclient->Shop->Buying );
    for(unsigned int i = 0; i<otherclient->Shop->Selling;i++)
    {
        CItem thisitem =  otherclient->items[otherclient->Shop->SellingList[i].slot];
        thisitem.count = otherclient->Shop->SellingList[i].count;
        ADDBYTE     ( pak, i );
        ADDDWORD    ( pak, BuildItemHead( thisitem ) );
        ADDDWORD    ( pak, BuildItemData( thisitem ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
       ADDDWORD    ( pak, otherclient->Shop->SellingList[i].price );
    }
    for(unsigned int i = 0; i<30;i++)
    {
        if(otherclient->Shop->BuyingList[i].count>0)
        {
            CItem thisitem = otherclient->Shop->BuyingList[i].item;
            ADDBYTE     ( pak, i );
            ADDDWORD    ( pak, BuildItemHead( thisitem ) );
            ADDDWORD    ( pak, BuildItemData( thisitem ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
            ADDDWORD    ( pak, otherclient->Shop->BuyingList[i].price );
        }
    }
    ADDBYTE    ( pak, 0x00 );
    thisclient->client->SendPacket( &pak );
    return true;
}

// Buy From Shop
//LMA: checking for Zuly hacks
bool CWorldServer::pakBuyShop( CPlayer* thisclient, CPacket* P )
{
    WORD otherclientid = GETWORD((*P),0);
    BYTE action = GETBYTE((*P),2);
    switch(action)
    {
        case 0x01://check this
        {
            CPlayer* otherclient = GetClientByID( otherclientid, thisclient->Position->Map );
            if( otherclient==NULL )
                return true;
            BYTE slot = GETBYTE((*P),3);
            unsigned int count = 0;
            unsigned int invslot = otherclient->Shop->SellingList[slot].slot;
            CItem newitem =  otherclient->items[invslot];
            if(otherclient->items[invslot].itemtype>9 &&
                    otherclient->items[invslot].itemtype<14)
                count = GETWORD((*P),8);
            else
                count = 1;
            if( count > otherclient->Shop->SellingList[slot].count )
                return true;
            newitem.count = count;
            if( thisclient->CharInfo->Zulies<(otherclient->Shop->SellingList[slot].price*count) )
                return true;
            unsigned int newslot = thisclient->GetNewItemSlot ( newitem );
            if(newslot==0xffff)
                return true;

            //LMA: check for hacks...
            if (thisclient->CharInfo->Zulies < (otherclient->Shop->SellingList[slot].price*count))
            {
              Log(MSG_HACK, "[Buy in Shop] Not enough Zuly player %s, have %li, need %li",thisclient->CharInfo->charname,thisclient->CharInfo->Zulies,otherclient->Shop->SellingList[slot].price*count);
              return true;
            }

            thisclient->CharInfo->Zulies -= (otherclient->Shop->SellingList[slot].price*count);
            otherclient->CharInfo->Zulies += (otherclient->Shop->SellingList[slot].price*count);
            if(otherclient->items[invslot].itemtype>9 &&
                    otherclient->items[invslot].itemtype<14)
            {
                if(otherclient->items[invslot].count<=count)
                {
                     ClearItem(otherclient->items[invslot])
                     otherclient->Shop->SellingList[slot].slot = 0;
                     otherclient->Shop->SellingList[slot].count = 0;
                     otherclient->Shop->SellingList[slot].price = 0;
                }
                else
                {
                    if(otherclient->Shop->SellingList[slot].count<=count)
                    {
                        otherclient->Shop->SellingList[slot].slot = 0;
                        otherclient->Shop->SellingList[slot].count = 0;
                        otherclient->Shop->SellingList[slot].price = 0;
                    }
                    else
                    {
                        otherclient->Shop->SellingList[slot].count -= count;
                    }
                    otherclient->items[invslot].count-=count;
                }
            }
            else
            {
                ClearItem( otherclient->items[invslot] )
                otherclient->Shop->SellingList[slot].slot = 0;
                otherclient->Shop->SellingList[slot].count = 0;
                otherclient->Shop->SellingList[slot].price = 0;
            }
            if(thisclient->items[newslot].count!=0)
                thisclient->items[newslot].count += newitem.count;
            else
                thisclient->items[newslot] = newitem;
            //update inventory (buyer)
            BEGINPACKET( pak, 0x7c7 );
            ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x01 ); //buy action
            ADDBYTE    ( pak, newslot );
            ADDDWORD   ( pak, BuildItemHead( thisclient->items[newslot] ) );
            ADDDWORD   ( pak, BuildItemData( thisclient->items[newslot] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
            thisclient->client->SendPacket( &pak );

            //update inventory (seller)
            RESETPACKET( pak, 0x7c7 );
            ADDQWORD   ( pak, otherclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x01 ); //buy action
            ADDBYTE    ( pak, invslot );
            ADDDWORD   ( pak, BuildItemHead( otherclient->items[invslot] ) );
            ADDDWORD   ( pak, BuildItemData( otherclient->items[invslot] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
            otherclient->client->SendPacket( &pak );

            //LMA: Saving slots
            thisclient->SaveSlot41(newslot);
            otherclient->SaveSlot41(invslot);

            //update shop
            RESETPACKET( pak, 0x7c6 );
            ADDWORD    ( pak, otherclient->clientid );
            ADDBYTE    ( pak, 0x05 );
            ADDBYTE    ( pak, 0x01 );
            ADDBYTE    ( pak, slot );
            if(otherclient->Shop->SellingList[slot].count<=0)
            {
               ADDDWORD ( pak, 0x00000000 );
               ADDDWORD ( pak, 0x00000000 );
               ADDDWORD( pak, 0x00000000 );
               ADDWORD ( pak, 0x0000 );
            }
            else
            {
               ADDDWORD ( pak, BuildItemHead( otherclient->items[invslot] ));
               ADDDWORD ( pak, BuildItemData( otherclient->items[invslot] ));
               ADDDWORD( pak, 0x00000000 );
               ADDWORD ( pak, 0x0000 );
            }
            SendToVisible( &pak, otherclient );
        }
        break;
        default:
            Log( MSG_WARNING, "Buy unknown action: %i", action );
    }
    return true;
}

// Sell in Shop
bool CWorldServer::pakSellShop( CPlayer* thisclient, CPacket* P )
{
    WORD otherclientid = GETWORD((*P),0);
    BYTE action = GETBYTE((*P),2);
    switch(action)
    {
        case 0x01:
        {
            CPlayer* otherclient = GetClientByID( otherclientid, thisclient->Position->Map );
            if( otherclient==NULL )
                return true;
            unsigned int count = 0;
            BYTE invslot = GETBYTE((*P),3);
            BYTE slot = GETBYTE((*P),4);
            if(!CheckInventorySlot( thisclient, slot))
                return false;
            CItem newitem =  thisclient->items[invslot];
            if(thisclient->items[invslot].itemtype>9 && thisclient->items[invslot].itemtype<14)
                count = GETWORD((*P),9);
            else
                count = 1;
            newitem.count = count;
            if( otherclient->CharInfo->Zulies<(otherclient->Shop->BuyingList[slot].price*count) )
                return true;
            unsigned int newslot = otherclient->GetNewItemSlot ( newitem );
            if( newslot==0xffff )
                return true;
            if( thisclient->items[invslot].count<count )
                return true;
            thisclient->CharInfo->Zulies += (otherclient->Shop->BuyingList[slot].price*count);
            otherclient->CharInfo->Zulies -= (otherclient->Shop->BuyingList[slot].price*count);
            if(thisclient->items[invslot].itemtype>9 &&
                    thisclient->items[invslot].itemtype<14)
            {
                if(thisclient->items[invslot].count<=count)
                {
                    ClearItem( thisclient->items[invslot] );
                    if(count>=otherclient->Shop->BuyingList[slot].count)
                    {
                         ClearItem( otherclient->Shop->BuyingList[slot].item );
                         otherclient->Shop->BuyingList[slot].slot = 0;
                         otherclient->Shop->BuyingList[slot].count = 0;
                         otherclient->Shop->BuyingList[slot].price = 0;
                    }
                    else
                    {
                         otherclient->Shop->BuyingList[slot].count -= count;
                    }
                }
                else
                {
                    if(otherclient->Shop->BuyingList[slot].count<=count)
                    {
                         ClearItem( otherclient->Shop->BuyingList[slot].item );
                         otherclient->Shop->BuyingList[slot].slot = 0;
                         otherclient->Shop->BuyingList[slot].count = 0;
                         otherclient->Shop->BuyingList[slot].price = 0;
                    }
                    else
                    {
                        otherclient->Shop->BuyingList[slot].count -= count;
                    }
                    thisclient->items[invslot].count-=count;
                }
            }
            else
            {
                ClearItem( thisclient->items[invslot] )
                ClearItem( otherclient->Shop->BuyingList[slot].item );
                otherclient->Shop->BuyingList[slot].slot = 0;
                otherclient->Shop->BuyingList[slot].count = 0;
                otherclient->Shop->BuyingList[slot].price = 0;
            }
            if(otherclient->items[newslot].count!=0)
                otherclient->items[newslot].count += newitem.count;
            else
                otherclient->items[newslot] = newitem;

            //update inventory (seller)
            BEGINPACKET( pak, 0x7c7 );
            ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x01 );
            ADDBYTE    ( pak, invslot );
            ADDDWORD   ( pak, BuildItemHead( thisclient->items[invslot] ) );
            ADDDWORD   ( pak, BuildItemData( thisclient->items[invslot] ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
            thisclient->client->SendPacket( &pak );

            //update inventory (buyer)
            RESETPACKET( pak, 0x7c7 );
            ADDQWORD   ( pak, otherclient->CharInfo->Zulies );
            ADDBYTE    ( pak, 0x01 );
            ADDBYTE    ( pak, newslot );
            ADDDWORD   ( pak, BuildItemHead( otherclient->items[newslot] ) );
            ADDDWORD   ( pak, BuildItemData( otherclient->items[newslot] ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
            otherclient->client->SendPacket( &pak );

            //LMA: Saving slots
            thisclient->SaveSlot41(invslot);
            otherclient->SaveSlot41(newslot);

            //update shop
            CItem thisitem = otherclient->Shop->BuyingList[slot].item;
            RESETPACKET( pak, 0x7c6 );
            ADDWORD    ( pak, otherclient->clientid );
            ADDBYTE    ( pak, 0x07 );
            ADDBYTE    ( pak, 0x01 );
            ADDBYTE    ( pak, slot );
            ADDDWORD   ( pak, BuildItemHead( thisitem ) );
            ADDDWORD   ( pak, BuildItemData( thisitem ) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
            SendToVisible( &pak, otherclient );
        }
        break;
        default:
            Log( MSG_WARNING, "Sell unknown action: %i", action );
    }
    return true;
}

// Close Shop
bool CWorldServer::pakCloseShop( CPlayer* thisclient, CPacket* P )
{
    BEGINPACKET( pak, 0x7c3 );
    ADDWORD    ( pak, thisclient->clientid );
    ADDBYTE    ( pak, 0x00 );
    SendToVisible( &pak, thisclient );
    thisclient->Shop->open = false;
    for(unsigned int j=0;j<30;j++)
    {
        thisclient->Shop->SellingList[j].slot = 0;
        thisclient->Shop->SellingList[j].count = 0;
        thisclient->Shop->SellingList[j].price = 0;
        thisclient->Shop->BuyingList[j].slot = 0;
        thisclient->Shop->BuyingList[j].count = 0;
        thisclient->Shop->BuyingList[j].price = 0;
        ClearItem( thisclient->Shop->BuyingList[j].item );

    }
    return true;
}

// Modified item (put gem, refine, Drill )
bool CWorldServer::pakModifiedItem( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open==true)
        return true;
    BYTE action = GETBYTE((*P),0);
    switch(action)
    {
        case 0x01://Gemming
        {
            BYTE destslot = GETBYTE((*P),1);
            BYTE srcslot = GETBYTE((*P),2);
            if(!CheckInventorySlot( thisclient, destslot))
                return false;
            if(!CheckInventorySlot( thisclient, srcslot))
                return false;
            if( thisclient->items[srcslot].count<=0 )
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 ); // dont have requerid material
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }
            if( thisclient->items[destslot].gem!=0 )
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x02 ); //no socket available
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }
            thisclient->items[destslot].gem = thisclient->items[srcslot].itemnum;
            thisclient->items[srcslot].count--;
            if(thisclient->items[srcslot].count<1)
                ClearItem( thisclient->items[srcslot] );
            BEGINPACKET( pak, 0x7bc );
            ADDBYTE    ( pak, 0x01 );//gemming success
            ADDBYTE    ( pak, 0x02 );
            ADDBYTE    ( pak, destslot );
            ADDDWORD   ( pak, BuildItemHead( thisclient->items[destslot] ) );
            ADDDWORD   ( pak, BuildItemData( thisclient->items[destslot] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
            ADDBYTE    ( pak, srcslot );
            ADDDWORD   ( pak, BuildItemHead( thisclient->items[srcslot] ) );
            ADDDWORD   ( pak, BuildItemData( thisclient->items[srcslot] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
            thisclient->client->SendPacket( &pak );
            thisclient->SetStats( );
        }
        break; //case 0x02 ( Surprise gift box )

       case 0x02: // Treasure Chests, Gift Box - by Drakia, // Disassemble - by Geobot
        {
            Log(MSG_INFO,"DSM: B0 %i, B1 %i, B2 %i, B3 %i, B4 %i",GETBYTE((*P), 0),GETBYTE((*P), 1),GETBYTE((*P), 2),GETBYTE((*P), 3),GETBYTE((*P), 4));

            CItem item;
            CItem itemextra;
            CChest* thischest;
            unsigned int chestSlot = GETBYTE((*P), 3);
            unsigned int rewardCount = (RandNumber(0, 100) < 20)?2:1;
            bool ischest=false;

            //Chests are only itemtype 10
            if(thisclient->items[chestSlot].itemtype==10)
            {
                ischest=true;
                thischest = GetChestByID(thisclient->items[chestSlot].itemnum);
                if(thischest==NULL)
                {
                    ischest=false;
                }

            }

            if (!ischest)
            {
                //start disassemble
                BYTE src = GETBYTE((*P),3);
                if(!CheckInventorySlot( thisclient, src))
                    return false;
                if(thisclient->items[src].count < 1)
                    return false;

               int k = BreakList.size();
               for(int i=0;i<BreakList.size();i++)
               {
                    if(thisclient->items[src].itemnum == BreakList.at(i)->itemnum && thisclient->items[src].itemtype == BreakList.at(i)->itemtype)
                    {
                       k = i;
                       break;
                    }

               }

               bool is_failed=false;
               bool not_found=false;
               if(k==BreakList.size())
               {
                   not_found=true;
                   is_failed=true;
               }

               UINT totalprob = 0;
                if(!is_failed)
                {
                   for(int i=0;i<BreakList.at(k)->total;i++)
                   {
                       totalprob += BreakList.at(k)->prob[i];
                   }

                }

               if(totalprob==0)
               {
                   is_failed=true;
                   //return false;
               }

               UINT rand = RandNumber(0,99999);
               UINT m = 99;
                if(!is_failed)
                {
                   for(int i=0;i<BreakList.at(k)->total;i++)
                   {
                       if(rand < BreakList.at(k)->prob[i])
                           m = i;
                       else
                           rand -= BreakList.at(k)->prob[i];
                   }

                   //if(m>14)
                   if(m>=20)
                   {
                       is_failed=true;
                       //return false;
                   }

                }

                //LMA: we have to return something, else it'll crash the client !
                //If me return no packet, it freezes the client anyway ^_^
                if(is_failed)
                {
                   Log(MSG_WARNING,"Player %s tried to disassemble item (%i:%i), it's not in break list or in error! (not found %i, failed %i)",thisclient->CharInfo->charname,thisclient->items[src].itemtype,thisclient->items[src].itemnum,is_failed,not_found);

                    //let's give him a banana for his trouble ;)
                   CItem newitem;

                    //LMA: we give him other items if item mall, event or unique gear.
                   if(not_found)
                   {
                        int grade=0;
                        if(thisclient->items[src].itemtype>9)
                        {
                            Log(MSG_WARNING,"Weird itemtype for disassemble %i::%i for %s",thisclient->items[src].itemtype,thisclient->items[src].itemnum,thisclient->CharInfo->charname);
                        }
                        else
                        {
                            if(thisclient->items[src].itemnum>=EquipList[thisclient->items[src].itemtype].max)
                            {
                                Log(MSG_WARNING,"Weird itemnum for disassemble %i::%i for %s (>= %u)",thisclient->items[src].itemtype,thisclient->items[src].itemnum,thisclient->CharInfo->charname,EquipList[thisclient->items[src].itemtype].max);
                            }
                            else
                            {
                                grade=EquipList[thisclient->items[src].itemtype].Index[thisclient->items[src].itemnum]->itemgrade;
                            }

                        }

                        if(grade==13)
                        {
                            //item mall
                            newitem.itemnum = 449;
                            newitem.itemtype = 12;
                            newitem.count = RandNumber(1,4);
                        }
                        else if(grade==14)
                        {
                            //event
                            newitem.itemnum = 448;
                            newitem.itemtype = 12;
                            newitem.count = RandNumber(1,4);
                        }
                        else if(grade==11)
                        {
                            //unique
                            newitem.itemnum = RandNumber(392,394);
                            newitem.itemtype = 12;
                            newitem.count = RandNumber(1,4);
                        }
                        else
                        {
                            //banana
                            newitem.itemnum = 102;
                            newitem.itemtype = 10;
                            newitem.count = 1;
                        }

                   }
                   else
                   {
                        newitem.itemnum = 102;
                        newitem.itemtype = 10;
                        newitem.count = 1;
                   }

                   newitem.refine = 0;
                   newitem.lifespan = 100;
                   newitem.durability = 40;
                   newitem.socketed=0;
                   newitem.appraised=0;
                   newitem.stats=0;
                   newitem.gem=0;
                   newitem.sp_value=0;
                   newitem.last_sp_value=0;

                   unsigned newslot = thisclient->GetNewItemSlot(newitem);
                   if(newslot == 0xffff)
                   {
                       //This should never happen since client is handling that.
                       //We let the client crashes in this case ^_^
                       return false;
                   }

                   if(thisclient->items[newslot].count > 0)
                   {
                       thisclient->items[newslot].count += newitem.count;
                       if(thisclient->items[newslot].count > 999)
                           thisclient->items[newslot].count = 999;
                   }
                   else
                       thisclient->items[newslot] = newitem;

                  thisclient->items[src].count -= 1;
                  if( thisclient->items[src].count < 1)
                      ClearItem( thisclient->items[src] );
                  thisclient->UpdateInventory(src);

                  BEGINPACKET( pak, 0x7bc );
                  ADDBYTE    ( pak, 0x07 );//disassemble success
                  ADDBYTE    ( pak, 0x02 );//number of items to follow
                  ADDBYTE    ( pak, newslot );
                  ADDDWORD   ( pak, BuildItemHead( thisclient->items[newslot] ) );
                  ADDDWORD   ( pak, BuildItemData( thisclient->items[newslot] ) );
                  ADDWORD    ( pak, 0x0000);
                  ADDWORD    ( pak, 0x0000);
                  ADDWORD    ( pak, 0x0000);
                  ADDBYTE    ( pak, src );
                  ADDDWORD   ( pak, BuildItemHead( thisclient->items[src] ) );
                  ADDDWORD   ( pak, BuildItemData( thisclient->items[src] ) );
                  ADDWORD    ( pak, 0x0000);
                  ADDWORD    ( pak, 0x0000);
                  ADDWORD    ( pak, 0x0000);
                  thisclient->client->SendPacket( &pak );
                   return true;
                }

               UINT num = BreakList.at(k)->product[m] % 1000;
               UINT type = int(BreakList.at(k)->product[m] / 1000);

               CItem newitem;
               newitem.itemnum = num;
               newitem.itemtype = type;

               //LMA: naRose new STB.
               //newitem.count = BreakList.at(k)->amount[m];
                if(BreakList.at(k)->amount_min[m]!=BreakList.at(k)->amount_max[m])
                {
                    newitem.count = RandNumber(BreakList.at(k)->amount_min[m],BreakList.at(k)->amount_max[m]);
                }
                else
                {
                    newitem.count =BreakList.at(k)->amount_max[m];
                }

               newitem.refine = 0;
               newitem.lifespan = 100;
               newitem.durability = RandNumber(40,50);
               newitem.socketed=0;
               newitem.appraised=0;
               newitem.stats=0;
               newitem.gem=0;
               newitem.sp_value=0;
               newitem.last_sp_value=0;

               unsigned newslot = thisclient->GetNewItemSlot(newitem);
               if(newslot == 0xffff)
               {
                   //This should never happen since client is handling that.
                   //We let the client crashes in this case ^_^
                   return false;
               }

                //LMA: PAT:
                if (newslot>=135&&newslot<=136)
                {
                    newitem.sp_value=newitem.lifespan*10;
                }

               if(thisclient->items[newslot].count > 0)
               {
                   thisclient->items[newslot].count += newitem.count;
                   if(thisclient->items[newslot].count > 999)
                       thisclient->items[newslot].count = 999;
               }
               else
                   thisclient->items[newslot] = newitem;

              thisclient->items[src].count -= 1;
              if( thisclient->items[src].count < 1)
                  ClearItem( thisclient->items[src] );
              thisclient->UpdateInventory(src);

              BEGINPACKET( pak, 0x7bc );
              ADDBYTE    ( pak, 0x07 );//disassemble success
              ADDBYTE    ( pak, 0x02 );//number of items to follow
              ADDBYTE    ( pak, newslot );
              ADDDWORD   ( pak, BuildItemHead( thisclient->items[newslot] ) );
              ADDDWORD   ( pak, BuildItemData( thisclient->items[newslot] ) );
              ADDWORD    ( pak, 0x0000);
              ADDWORD    ( pak, 0x0000);
              ADDWORD    ( pak, 0x0000);
              ADDBYTE    ( pak, src );
              ADDDWORD   ( pak, BuildItemHead( thisclient->items[src] ) );
              ADDDWORD   ( pak, BuildItemData( thisclient->items[src] ) );
              ADDWORD    ( pak, 0x0000);
              ADDWORD    ( pak, 0x0000);
              ADDWORD    ( pak, 0x0000);
              thisclient->client->SendPacket( &pak );
              //   end disassemble

              return true;
            }

            //LMA: Mileage box?
            int is_mileage=GServer->UseList.Index[thisclient->items[chestSlot].itemnum]->is_mileage;
            int bonus=0;

            unsigned int randv = RandNumber( 1, thischest->probmax );

            DWORD prob = 1;
            for(UINT i=0;i<thischest->Rewards.size();i++)
            {
                CReward* reward = thischest->Rewards.at( i );
                prob += reward->prob;

                //LMA: extra bonus for mileage boxes.
                if(is_mileage==1)
                    bonus=GServer->RandNumber(1, 300);
                if(reward->type>=10)
                    bonus=0;

                if(randv<=prob)
                {
                    item.itemtype = reward->type;
                    item.itemnum = reward->id;
                    //item.count = reward->rewardamount;

                    //LMA: naRose changed their way.
                    //item.count = RandNumber( 1,reward->rewardamount);
                    if(reward->rewardamount_max!=reward->rewardamount_min)
                    {
                        item.count = RandNumber( reward->rewardamount_min,reward->rewardamount_max);
                    }
                    else
                    {
                        item.count =reward->rewardamount_max;
                    }

                    item.socketed = false;
                    item.appraised = true;
                    item.lifespan = 100;
                    item.durability = 100;
                    item.refine = 0;
                    item.stats = bonus;
                    item.gem = 0;
                    item.sp_value=0;
                    item.last_sp_value=0;
                    prob = reward->prob;
                    break;
                }
            }

            unsigned int rewardmax = RandNumber( 1, thischest->rewardposs ); //test
            bonus=0;

            //if (rewardCount > 1)
            if (rewardmax >1)
            {
                DWORD probextra = 1;
                randv = RandNumber( 1, thischest->probmax - prob );
                itemextra.itemnum = 0;
                for(UINT i=0;i<thischest->Rewards.size() - 1;i++)
                {
                    CReward* reward = thischest->Rewards.at( i );
                    if (reward->id != item.itemnum)
                    {
                        prob += reward->prob;

                        //LMA: extra bonus for mileage boxes.
                        if(is_mileage==1)
                            bonus=GServer->RandNumber(1, 300);
                        if(reward->type>=10)
                            bonus=0;

                        if(randv<=prob)
                        {
                            itemextra.itemtype = reward->type;
                            itemextra.itemnum = reward->id;
                            //itemextra.count = reward->rewardamount;

                            //LMA: naRose changed their way.
                            //itemextra.count = RandNumber( 1,reward->rewardamount);
                            if(reward->rewardamount_max!=reward->rewardamount_min)
                            {
                                itemextra.count = RandNumber( reward->rewardamount_min,reward->rewardamount_max);
                            }
                            else
                            {
                                itemextra.count =reward->rewardamount_max;
                            }

                            itemextra.socketed = false;
                            itemextra.appraised = true;
                            itemextra.lifespan = 100;
                            itemextra.durability = 100;
                            itemextra.refine = 0;
                            itemextra.stats = bonus;
                            itemextra.gem = 0;
                            itemextra.sp_value=0;
                            itemextra.last_sp_value=0;
                            break;
                        }

                    }

                }

                if (itemextra.itemnum == 0)
                {
                    Log(MSG_INFO, "Could not obtain secondary reward. Make sure all chests have atleast 2 rewards.");
                    //rewardCount = 1;
                    rewardmax=1;
                }

            }

            thisclient->items[chestSlot].count--;
            if (thisclient->items[chestSlot].count < 1)
                ClearItem(thisclient->items[chestSlot]);

            unsigned int tempslot = thisclient->AddItem(item);
            if (tempslot != 0xffff)
            {
                //LMA: temp fix.
                //TODO: really add "rewardmax" items...
                if(rewardmax>1)
                {
                    //For now, only 1 item + 1 bonus item + chest.
                    rewardmax=2;
                }

                BEGINPACKET( pak, 0x7bc );
                ADDBYTE (pak, 0x13);  // Status code. Congrats?
                //ADDBYTE (pak, (rewardCount + 1));  // Number of items
                ADDBYTE (pak, (rewardmax + 1));  // Number of items

                ADDBYTE (pak, tempslot);
                ADDDWORD(pak, BuildItemHead(thisclient->items[tempslot]));
                ADDDWORD(pak, BuildItemData(thisclient->items[tempslot]));
                ADDDWORD( pak, 0x00000000 );
                ADDWORD ( pak, 0x0000 );
                //if (rewardCount > 1)
                if (rewardmax > 1)
                {
                    tempslot = thisclient->AddItem(itemextra);
                    if (tempslot != 0xffff)
                    {
                        ADDBYTE(pak, tempslot);
                        ADDDWORD(pak, BuildItemHead(thisclient->items[tempslot]));
                        ADDDWORD(pak, BuildItemData(thisclient->items[tempslot]));
                        ADDDWORD(pak, 0x00000000);
                        ADDWORD(pak, 0x0000);
                    } else {
                        Log(MSG_WARNING, "Error adding second item");
                        return true;
                    }
                }

                ADDBYTE (pak, chestSlot);
                ADDDWORD(pak, BuildItemHead(thisclient->items[chestSlot]));
                ADDDWORD(pak, BuildItemData(thisclient->items[chestSlot]));
                ADDDWORD( pak, 0x00000000 );
                ADDWORD ( pak, 0x0000 );

                thisclient->client->SendPacket( &pak );
            }
            return true;
        }
        break;

        case 0x05://Refine
        {
            BYTE item = GETBYTE((*P),3);
            BYTE material = GETBYTE((*P),4);

            if(!CheckInventorySlot( thisclient, item))
                return false;
            if(!CheckInventorySlot( thisclient, material))
                return false;

            //LMA: We need to get the "real" item needed for upgrade.
            UINT gradeIndex=EquipList[thisclient->items[item].itemtype].Index[thisclient->items[item].itemnum]->itemgradeID;

            if(gradeIndex==0||gradeIndex>=ProductList.max)
            {
                Log(MSG_HACK,"Player %s can't refine %u::%u, index %u",thisclient->CharInfo->charname,thisclient->items[item].itemtype,thisclient->items[item].itemnum,gradeIndex);
                return false;
            }

            UINT needed_itemtype=0;
            UINT needed_itemnum=0;
            UINT needed_amount=0;
            UINT zulyamount=0;
            int quality=0;

            needed_amount=ProductList.Index[gradeIndex]->amount[0];
            needed_itemtype=gi(ProductList.Index[gradeIndex]->item[0],0);
            needed_itemnum=gi(ProductList.Index[gradeIndex]->item[0],1);

            //LMA: special case venurune & nepturune
            int extra_offset=0;
            bool venurune=false;
            bool nepturune=false;

            if(thisclient->items[material].itemtype==12&&thisclient->items[material].itemnum==445)
            {
                venurune=true;
                extra_offset=1;//it's an offset, NOT a %!
                needed_amount=1;
                needed_itemnum=445;
                needed_itemtype=12;
            }
            if(thisclient->items[material].itemtype==12&&thisclient->items[material].itemnum==456)
            {
                nepturune=true;
                extra_offset=1;//it's an offset, NOT a %!
                needed_amount=1;
                needed_itemnum=456;
                needed_itemtype=12;
            }
            if(needed_itemtype!=thisclient->items[material].itemtype||needed_itemnum!=thisclient->items[material].itemnum)
            {
                Log(MSG_HACK,"Player %s uses wrong item (%u::%u instead of %u::%u) to refine %u::%u",thisclient->CharInfo->charname,thisclient->items[material].itemtype,thisclient->items[material].itemnum,needed_itemtype,needed_itemnum,thisclient->items[item].itemtype,thisclient->items[item].itemnum);
                return false;
            }

            //LMA: item amount
            //if(thisclient->items[item].count<1 || thisclient->items[material].count<1)
            if(thisclient->items[item].count<1 || thisclient->items[material].count<needed_amount)
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                Log(MSG_HACK,"Player %s hasn't enough item %i*(%u::%u) for refine",thisclient->CharInfo->charname,needed_amount,needed_itemtype,needed_itemnum);
                return true;
            }

            //LMA: taking the money, we need the quality.
            if(thisclient->items[item].itemtype<=0||thisclient->items[item].itemtype>=11)
            {
                Log(MSG_WARNING,"Error, player %s tryes to refine item %u::%u",thisclient->CharInfo->charname,thisclient->items[item].itemtype,thisclient->items[item].itemnum);
            }
            else
            {
                if(thisclient->items[item].itemnum>=EquipList[thisclient->items[item].itemtype].max)
                {
                    Log(MSG_WARNING,"Error, player %s tryes to refine itemn %u::%u but >= %u ",thisclient->CharInfo->charname,thisclient->items[item].itemtype,thisclient->items[item].itemnum,EquipList[thisclient->items[item].itemtype].max);
                }
                else
                {
                    quality=EquipList[thisclient->items[item].itemtype].Index[thisclient->items[item].itemnum]->quality;
                }

            }

            //end of zuly check.

            unsigned int nextlevel = ( thisclient->items[item].refine / 16 ) + 1;
            if( nextlevel > 9 )
                return true;

            //LMA: How much zuly? :)
            if(quality>0&&nextlevel>1)
            {
                float basis=(pow(quality,3)*(float)((float)127/(float)7685496)+pow(quality,2)*(float)((float)435665/(float)1097928)+quality*(float)((float)31437683/(float)3842748)-(float)((float)77200/(float)24633));
                float quant=((float)(nextlevel-1)*nextlevel)/2;
                zulyamount=(UINT)(basis*quant);
                Log(MSG_INFO,"Trying to get %u Z for quality %i (basis %f, quant %f)",zulyamount,quality,basis,quant);
            }

            if(zulyamount>0)
            {
                if (thisclient->CharInfo->Zulies<zulyamount)
                {
                    SendPM(thisclient,"You don't have enough money to refine, %u needed");
                    Log(MSG_WARNING,"Error, player %s tryes to refine item %u::%u but hasn't enough money, %u needed, he has %li",thisclient->CharInfo->charname,thisclient->items[item].itemtype,thisclient->items[item].itemnum,zulyamount,thisclient->CharInfo->Zulies);
                    return true;
                }

                //taking the money (packet not needed).
                thisclient->CharInfo->Zulies-=zulyamount;
                /*
                BEGINPACKET( pak, 0x71d );
                ADDQWORD( pak, thisclient->CharInfo->Zulies );
                thisclient->client->SendPacket( &pak );*/
            }

            unsigned int prefine = rand()%100;
            bool success = false;
            if( prefine <= (upgrade[nextlevel][extra_offset]) )
            {
                success = true;
            }

            BEGINPACKET( pak, 0x7bc );
            if( success )
            {
                thisclient->items[item].refine = nextlevel*16;
                ADDBYTE    ( pak, 0x10 );// 0x10 successful
            }
            else
            {
                //LMA: Old code.
                /*
                if (nextlevel > 5) { // take item away
                    ClearItem( thisclient->items[item] );
                } else { // decreade grade
                    thisclient->items[item].refine = RandNumber(0,4) * 16;
                }
                ADDBYTE    ( pak, 0x11 );// 0x11 Fail
                */

                //LMA: new way, Getting item grade.
                int grade=0;
                if(thisclient->items[item].itemtype>9)
                {
                    Log(MSG_WARNING,"Weird itemtype for refine %i::%i for %s",thisclient->items[item].itemtype,thisclient->items[item].itemnum,thisclient->CharInfo->charname);
                }
                else
                {
                    if(thisclient->items[item].itemnum>=EquipList[thisclient->items[item].itemtype].max)
                    {
                        Log(MSG_WARNING,"Weird itemnum for refine %i::%i for %s (>= %u)",thisclient->items[item].itemtype,thisclient->items[item].itemnum,thisclient->CharInfo->charname,EquipList[thisclient->items[item].itemtype].max);
                    }
                    else
                    {
                        grade=EquipList[thisclient->items[item].itemtype].Index[thisclient->items[item].itemnum]->itemgrade;
                    }

                }

                if(grade>14)
                {
                    Log(MSG_WARNING,"Weird grade for refine %i::%i for %s, %u",thisclient->items[item].itemtype,thisclient->items[item].itemnum,thisclient->CharInfo->charname,grade);
                    grade=0;
                }

                if(nextlevel<=refine_grade[grade][0])
                {
                    //Success, always success :)
                    success=true;
                    thisclient->items[item].refine = nextlevel*16;
                    ADDBYTE    ( pak, 0x10 );// 0x10 successful
                }
                else if (nextlevel<=refine_grade[grade][1])
                {
                    //Only degrade.
                    if(nepturune)
                    {
                        thisclient->items[item].refine = (nextlevel-1) * 16;
                    }
                    else if(nextlevel>1)
                    {
                        thisclient->items[item].refine = RandNumber(0,nextlevel-1) * 16;
                    }

                    else
                    {
                        thisclient->items[item].refine = 0;
                    }

                    ADDBYTE    ( pak, 0x11 );// 0x11 Fail
                }
                else
                {
                    if(!nepturune) //Ouch, break time.
                    {
                        ClearItem( thisclient->items[item] );
                    }
                    ADDBYTE    ( pak, 0x11 );// 0x11 Fail
                }

            }

            //thisclient->items[material].count--; // geo edit, moved this up two lines
            thisclient->items[material].count-=needed_amount;   //LMA: item amount
            if(thisclient->items[material].count<1)
                ClearItem( thisclient->items[material] );

            ADDBYTE    ( pak, 0x03 );//items a actualizar
            ADDBYTE    ( pak, material );
            ADDDWORD   ( pak, BuildItemHead( thisclient->items[material] ) );
            ADDDWORD   ( pak, BuildItemData( thisclient->items[material] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
            ADDBYTE    ( pak, item );
            ADDDWORD   ( pak, BuildItemHead( thisclient->items[item] ) );
            ADDDWORD   ( pak, BuildItemData( thisclient->items[item] ) );
            ADDDWORD( pak, 0x00000000 );
            ADDWORD ( pak, 0x0000 );
            ADDBYTE    ( pak, 0x00 );
            ADDDWORD   ( pak, 0x002f0000 );
            ADDDWORD   ( pak, 0x00000017 );
            thisclient->client->SendPacket( &pak );
        }
        break;
        case 0x06: // Drill
        {
            BYTE material = GETBYTE((*P), 1);
            BYTE item = GETBYTE((*P), 3);
            if(!CheckInventorySlot( thisclient, item))
                return false;
            if(!CheckInventorySlot( thisclient, material))
                return false;
            if( thisclient->items[material].count<=0 )
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x12 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
                return true;
            }
            //LMA: already drilled
            if(thisclient->items[item].socketed)
            {
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x23 );
                ADDBYTE    ( pak, 0x00 );
                thisclient->client->SendPacket( &pak );
               return true;
            }

            thisclient->items[material].count -= 1;

            if( thisclient->items[material].count<=0 )
                ClearItem(thisclient->items[material]);

            //LMA: Adding some probability there :)
            int luck_time=RandNumber(0,100);
            Log(MSG_INFO,"Drill succes? %i/%i",luck_time,UseList.Index[thisclient->items[material].itemnum]->useeffect[1]);
            if(luck_time>UseList.Index[thisclient->items[material].itemnum]->useeffect[1])
            {
                //fail
                thisclient->items[item].socketed = false;
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x21 );
                ADDBYTE    ( pak, 0x01 );
                ADDBYTE    ( pak, material);
                ADDDWORD   ( pak, BuildItemHead( thisclient->items[material] ) );
                ADDDWORD   ( pak, BuildItemData( thisclient->items[material] ) );
                ADDDWORD( pak, 0x00000000 );
                ADDWORD ( pak, 0x0000 );
                thisclient->client->SendPacket(&pak);
            }
            else
            {
                thisclient->items[item].socketed = true;
                BEGINPACKET( pak, 0x7bc );
                ADDBYTE    ( pak, 0x20 );
                ADDBYTE    ( pak, 0x02 );
                ADDBYTE    ( pak, material);
                ADDDWORD   ( pak, BuildItemHead( thisclient->items[material] ) );
                ADDDWORD   ( pak, BuildItemData( thisclient->items[material] ) );
                ADDDWORD( pak, 0x00000000 );
                ADDWORD ( pak, 0x0000 );
                ADDBYTE    ( pak, item );
                ADDDWORD   ( pak, BuildItemHead( thisclient->items[item] ) );
                ADDDWORD   ( pak, BuildItemData( thisclient->items[item] ) );
                ADDDWORD( pak, 0x00000000 );
                ADDWORD ( pak, 0x0000 );
                thisclient->client->SendPacket(&pak);
            }

            //Saving in database.
            //thisclient->SaveSlot41(material);
            //thisclient->SaveSlot41(item);
        }
        break;
        default:
            Log( MSG_WARNING,"Modified Item unknown action: %i", action);
    }
    return true;
}

// Repair (NPC and tool)
bool CWorldServer::pakRepairItem( CPlayer* thisclient, CPacket* P,int packet_type)
{
    BYTE slot_tool=0;
    BYTE slot = GETBYTE((*P),2);
    if(!CheckInventorySlot( thisclient, slot)) return false;
    if(thisclient->items[slot].count<1) return true;

    //LMA: We have NPC or tool repair, comes from different packet.
    switch( packet_type)
    {
        case 1:
        {
            //NPC repair (NPC client ID).
            BYTE npc_id=GETWORD((*P),0);
            //let's check the NPC is amongst the good ones...
            CNPC* testnpc=GServer->GetNPCByID(npc_id, thisclient->Position->Map);
            if (testnpc==NULL)
            {
                Log(MSG_HACK,"%s tried to repair with an unknown NPC!",thisclient->CharInfo->charname);
                return true;
            }

            //checking the NPC to a list of "correct" NPC.
            int liste_npc[6];
            liste_npc[0]=1008;  //Raffle
            liste_npc[1]=1093;   //Crune
            liste_npc[2]=1034;   //Ronk
            liste_npc[3]=1062; //Punwell
            liste_npc[4]=1181;   //pavrick
            liste_npc[5]=1223;   //nel eldora

            bool is_found=false;
            int k=0;
            for (k=0;k<6;k++)
            {
                if(testnpc->npctype==liste_npc[k])
                {
                    is_found=true;
                    break;
                }

            }

            if (!is_found)
            {
                Log(MSG_HACK,"%s tried to repair with an wrong NPC (%i)!",thisclient->CharInfo->charname,testnpc->npctype);
                return true;
            }

            //checking distance now.
            if(GServer->distance(thisclient->Position->current,testnpc->pos)>20)
            {
                Log(MSG_HACK,"%s tried to repair but too far from NPC %i!",thisclient->CharInfo->charname,testnpc->npctype);
                return true;
            }

            Log(MSG_INFO,"Npc index %i (%i) is repairing for %s",npc_id,liste_npc[k],thisclient->CharInfo->charname);
        }
        break;
        case 2:
        {
            //Tool repair.
            slot_tool=GETBYTE((*P),0);
            //let's check if we have a tool in stock :)
            if(!CheckInventorySlot( thisclient, slot_tool)) return false;
            if(thisclient->items[slot_tool].count<1)
            {
                Log(MSG_HACK,"Player %s tryed to repair without tool",thisclient->CharInfo->charname);
                return true;
            }

            //Is it a repair tool?
            if(thisclient->items[slot_tool].itemnum>=UseList.max||UseList.Index[thisclient->items[slot_tool].itemnum]->type!=315)
            {
                Log(MSG_HACK,"Player %s tryed to repair without a good tool",thisclient->CharInfo->charname);
                return true;
            }

            //it's a good tool, let's take one of them.
            thisclient->items[slot_tool].count--;
            if (thisclient->items[slot_tool].count==0)
                ClearItem( thisclient->items[slot_tool] );

            //Ok now the quality of the tool :)
            if (UseList.Index[thisclient->items[slot_tool].itemnum]->quality<=50)
            {
                //standard quality, so durability goes away...
                thisclient->items[slot].durability-=1;
                if(thisclient->items[slot].durability<=0)
                    thisclient->items[slot].durability=1;
            }

        }
        break;

    }

    thisclient->items[slot].lifespan = 100;

    //Still TODO: find where prices of storage and repair are and add it to the code.
    if(packet_type==1)
    {
        BEGINPACKET( pak, 0x7cd );
        ADDQWORD   ( pak, thisclient->CharInfo->Zulies );
        ADDBYTE    ( pak, 0x01 );
        ADDBYTE    ( pak, slot );
        ADDDWORD   ( pak, BuildItemHead( thisclient->items[slot] ));
        ADDDWORD   ( pak, BuildItemData( thisclient->items[slot] ));
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );
        thisclient->client->SendPacket( &pak );
        thisclient->SetStats( );
        thisclient->SaveSlot41(slot);
        return true;
    }

    //repair tool.
    BEGINPACKET( pak, 0x7cb );
    ADDBYTE    ( pak, 0x02 );
    ADDBYTE    ( pak, slot_tool );
    ADDDWORD   ( pak, BuildItemHead( thisclient->items[slot_tool] ));
    ADDDWORD   ( pak, BuildItemData( thisclient->items[slot_tool] ));
    ADDDWORD( pak, 0x00000000 );
    ADDWORD ( pak, 0x0000 );
    ADDBYTE    ( pak, slot );
    ADDDWORD   ( pak, BuildItemHead( thisclient->items[slot] ));
    ADDDWORD   ( pak, BuildItemData( thisclient->items[slot] ));
    ADDDWORD( pak, 0x00000000 );
    ADDWORD ( pak, 0x0000 );
    thisclient->client->SendPacket( &pak );
    thisclient->SetStats( );
    thisclient->SaveSlot41(slot_tool);
    thisclient->SaveSlot41(slot);


    return true;
}

// Ride request
bool CWorldServer::pakRideRequest( CPlayer* thisclient, CPacket* P )
{
    if(thisclient->Shop->open)
        return true;
    BYTE action = GETBYTE((*P),0);
    switch(action)
    {
        case 0x01://Ride Request
        {
            WORD tclientid = GETWORD((*P),1);
            WORD oclientid = GETWORD((*P),3);
            if( tclientid != thisclient->clientid )
                return true;
            CPlayer* otherclient = GetClientByID( oclientid, thisclient->Position->Map );
            if(otherclient==NULL)
                return true;
            BEGINPACKET( pak, 0x7dd );
            ADDBYTE    ( pak, 0x01 );
            ADDWORD    ( pak, thisclient->clientid );
            ADDWORD    ( pak, oclientid );
            ADDFLOAT   ( pak, thisclient->Position->current.x*100 );
            ADDFLOAT   ( pak, thisclient->Position->current.y*100 );
            otherclient->client->SendPacket( &pak );
        }
        break;
        case 0x02://Accept
        {
            WORD oclientid = GETWORD((*P),1);
            WORD tclientid = GETWORD((*P),3);
            if( tclientid != thisclient->clientid )
                return true;
            CPlayer* otherclient = GetClientByID( oclientid, thisclient->Position->Map );
            if(otherclient==NULL)
                return true;
            BEGINPACKET( pak, 0x796 );
            ADDWORD    ( pak, tclientid );
            ADDFLOAT   ( pak, thisclient->Position->current.x*100 );
            ADDFLOAT   ( pak, thisclient->Position->current.y*100 );
            ADDWORD    ( pak, 0x0000 );
            SendToVisible( &pak, otherclient );

            RESETPACKET( pak, 0x7dd );
            ADDBYTE    ( pak, 0x02 );
            ADDWORD    ( pak, oclientid );
            ADDWORD    ( pak, tclientid );
            SendToVisible( &pak, otherclient );
            thisclient->Ride->Ride = true;
            thisclient->Ride->Drive = false;
            thisclient->Ride->charid = otherclient->CharInfo->charid;
            otherclient->Ride->Ride = true;
            otherclient->Ride->Drive = true;
            otherclient->Ride->charid = thisclient->CharInfo->charid;
        }
        break;
        case 0x03://Denied
        {
            WORD oclientid = GETWORD((*P),1);
            WORD tclientid = GETWORD((*P),3);
            if( tclientid != thisclient->clientid )
                return true;
            CPlayer* otherclient = GetClientByID( oclientid, thisclient->Position->Map );
            if(otherclient==NULL)
                return true;
            BEGINPACKET( pak, 0x7dd );
            ADDBYTE    ( pak, 0x03 );
            ADDWORD    ( pak, oclientid );
            ADDWORD    ( pak, tclientid );
            otherclient->client->SendPacket( &pak );
        }
        break;
    }
	return true;
}

bool CWorldServer::pakWeight( CPlayer* thisclient, CPacket* P )
{
    BYTE weight = GETBYTE((*P),0) & 0xff;
    if(weight>110)
        thisclient->Status->CanAttack = false;
    else
        thisclient->Status->CanAttack = true;
    if(weight>100)
        thisclient->Status->CanRun = false;
    else
        thisclient->Status->CanRun = true;
    thisclient->Stats->Move_Speed = thisclient->GetMoveSpeed( );
    BEGINPACKET( pak, 0x762 );
    ADDWORD    ( pak, thisclient->clientid );
    ADDBYTE    ( pak, weight );
    thisclient->client->SendPacket( &pak );
    return true;
}

// Show Monster HP or Player Regeneration
bool CWorldServer::pakShowHeal( CPlayer* thisclient, CPacket* P )
{
    WORD clientid = GETWORD((*P),0);
    CCharacter* character = MapList.Index[thisclient->Position->Map]->GetCharInMap( clientid );
    if(character==NULL) return true;
    if(character->IsMonster())
    {
    	// SET MONSTER HEALTH
        if(character->Stats->HP>MAXHPMOB)
        {
            character->Stats->HP=(long long) MAXHPMOB;
        }

    	BEGINPACKET( pak, 0x79f );
    	ADDWORD( pak, character->clientid );
    	/*ADDWORD( pak, character->Stats->HP );
    	ADDWORD( pak, 0 );*/
    	ADDDWORD   ( pak, character->Stats->HP );    //LMA: DDWORD :)
        thisclient->client->SendPacket( &pak );
        thisclient->Battle->contatk = false;
    }
    thisclient->Battle->target = clientid;
    return true;
}

// Repair Tool
bool CWorldServer::pakRepairTool( CPlayer* thisclient, CPacket* P )
{
    BEGINPACKET( pak, 0x7cb );

    thisclient->items[(GETBYTE((*P), 0x0))].count -= 1;
    thisclient->items[(GETBYTE((*P), 0x2))].lifespan = 100;
    if(thisclient->items[(GETBYTE((*P), 0x0))].count <= 0){
        ClearItem(thisclient->items[(GETBYTE((*P), 0x0))]);
        RESETPACKET (pak, 0x7cb);
        ADDBYTE(pak, 1);
        ADDBYTE(pak,GETBYTE((*P), 0x2));
        ADDDWORD( pak, BuildItemHead( thisclient->items[GETBYTE((*P), 0x2)]) );
        ADDDWORD( pak, BuildItemData( thisclient->items[GETBYTE((*P), 0x2)]) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );

        thisclient->client->SendPacket(&pak);
    }
    else{
         RESETPACKET (pak, 0x7cb);
         ADDBYTE(pak, 2);
         ADDBYTE(pak,GETBYTE((*P), 0x0));
         ADDDWORD(pak, BuildItemHead(thisclient->items[GETBYTE((*P), 0x0)]));
         ADDWORD(pak, (thisclient->items[(GETBYTE((*P), 0x0))].count));//amount
         ADDWORD(pak, 0x0000);
         ADDBYTE(pak,GETBYTE((*P), 0x2));
         ADDDWORD( pak, BuildItemHead( thisclient->items[GETBYTE((*P), 0x2)]) );
         ADDDWORD( pak, BuildItemData( thisclient->items[GETBYTE((*P), 0x2)]) );
        ADDDWORD( pak, 0x00000000 );
        ADDWORD ( pak, 0x0000 );

    thisclient->client->SendPacket(&pak);
    }
    return true;
}

// Disconnect char
bool CWorldServer::pakCharDSClient( CPlayer* thisclient, CPacket* P )
{
    unsigned int userid = GETDWORD((*P), 1 );
    CPlayer* otherclient = GetClientByUserID( userid );
    if(otherclient==NULL) return true;

    BYTE action = GETBYTE((*P),0);
    switch(action)
    {
        case 1:
        {
            if(otherclient==NULL)
            {
                Log(MSG_WARNING, "userid '%s' not found online", userid );
                return true;
            }

            BEGINPACKET( pak, 0x707 );
            ADDWORD( pak, 0 );
            otherclient->client->SendPacket( &pak );

            otherclient->client->isActive = false;
        }
        break;
    }
    return true;
}


// Pack Printscreen
bool CWorldServer::pakPrintscreen( CPlayer* thisclient, CPacket* P )
{
    BEGINPACKET( pak, 0x7eb );
    ADDWORD    ( pak, thisclient->Session->userid );
    ADDWORD    ( pak, 0x0302 );
    ADDWORD    ( pak, 0x2d17 );
    thisclient->client->SendPacket( &pak );
    return true;
}

// CharServer Connected
bool CWorldServer::pakCSReady ( CPlayer* thisclient, CPacket* P )
{
	if(thisclient->Session->isLoggedIn) return false;
	if(GETDWORD((*P),0) != Config.WorldPass )
    {
        Log( MSG_HACK, "CharServer Tried to connect WorldServer with wrong password ");
        return true;
    }
	thisclient->Session->isLoggedIn=true;
	return true;
}

// CharServer - Disconnect User
bool CWorldServer::pakCSCharSelect ( CPlayer* thisclient, CPacket* P )
{
    if(!thisclient->Session->isLoggedIn) return false;
	Log( MSG_INFO, "Char server requested client kill" );
	DWORD userid = GETDWORD( (*P), 0 );
	CPlayer* otherclient = GetClientByUserID( userid );
	if(otherclient!=NULL)
	{
        if(otherclient->client==NULL) return true;
        otherclient->client->isActive = false;
    }
	Log( MSG_INFO, "Client killed" );
	return true;
}

//LMA: ItemMall handling
bool CWorldServer::pakItemMall( CPlayer* thisclient, CPacket* P )
{

     switch (GETBYTE((*P), 0x0))
     {
            case 0x01:
            {
                 //list of player's itemmall
                 if (thisclient->nsitemmallitems==0)
                    return true;
                 ReturnItemMallList(thisclient);
            }
            break;
            case 0x02:
            {
                 //transfer to another player?
                 return true;
            }
            break;
            case 0x03:
            {
                 //takes one item to itemmall to player's inventory
                 WORD mal_qty=GETWORD( (*P), 0x01);
                 BYTE mall_slot=GETBYTE((*P), 0x03);
                 Log(MSG_INFO,"%s Trying to get %i from slot %i in mileage",thisclient->CharInfo->charname,mal_qty,mall_slot);
                 //Handle this...
                 TakeItemMallList(thisclient,mal_qty,mall_slot);
                 return true;
            }
            break;
            default:
            {
                    Log(MSG_INFO,"Unknown command %i for ItemMall",GETBYTE((*P), 0x0));
                    return true;
            }
            break;
     }


    return true;
}

/**
	* add item to the wishlist
	* @param thisclient CPlayer class
	* @param P CPacket class
	* @return bool true if the item was added else false
*/
bool CWorldServer::pakAddWishList( CPlayer* thisclient , CPacket* P )
{
	if(thisclient==NULL) return false;
	UINT slot = GETBYTE( (*P), 0 );
	if(slot>=MAX_WISHLIST) return true;
	UINT head = GETDWORD((*P),1);
	UINT data = GETDWORD((*P),5);

    //LMA: We delete a slot.
	if (head==0&&data==0)
	{
	    DB->QExecute( "DELETE FROM wishlist WHERE itemowner=%u AND slot=%i",thisclient->CharInfo->charid, slot );
	    return true;
	}

    CItem testitem = GetItemByHeadAndData( head , data);
	// check if is a valid item
	if(testitem.itemtype>14 || testitem.itemtype<1) return false;
	// save to the database
	/*
	DB->QExecute( "DELETE FROM wishlist WHERE itemowner=%u AND slot=%i",
		thisclient->CharInfo->charid, slot );
	DB->QExecute( "INSERT INTO wishlist (itemowner,slot,itemhead,itemdata) VALUES (%u,%i,%u,%u)",
		thisclient->CharInfo->charid, slot, head, data );
    */

    //LMA: new way:
    //%I64i
    DB->QExecute("INSERT INTO wishlist (itemowner,slot,itemhead,itemdata) VALUES(%u,%i,%u,%u) ON DUPLICATE KEY UPDATE itemowner=VALUES(itemowner),slot=VALUES(slot),itemhead=VALUES(itemhead),itemdata=VALUES(itemdata)",
        thisclient->CharInfo->charid, slot, head, data );


	return true;
}
