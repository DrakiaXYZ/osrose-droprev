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
#ifndef _ROSE_PLAYER_
#define _ROSE_PLAYER_
#include "character.h"

class CPlayer: public CCharacter
{

    public:
        CPlayer( CClientSocket* );
        ~CPlayer( );

    //
    CClientSocket* client;
    bool questdebug;// Show Quest message in console?
    bool Saved;// already save this player?(used ondisconnect/returnchar)
    bool isInvisibleMode;

    //LMA: Pvp flag
    int pvp_id;

    //LMA: Warp flag
    int map_warp_zone;
    fPoint Warp_Zone;

    //LMA: HP / MP regeneration by bonfires and others...
    int sp_hp;
    int sp_mp;
    int nb_hp;
    int nb_mp;

    //Fuel
    clock_t last_fuel;

    // Fairy
    bool Fairy;
    UINT FairyListIndex;

    // variables
    bool is_born;   //LMA: just came into play?
    bool is_invisible;  //LMA: for AIP.
    UINT uw_kills;  //LMA: for Union War.
    unsigned int hits;
    USEDITEM* UsedItem;
    INFO* CharInfo;
    RIDE* Ride;
    TRADE* Trade;
    PARTY* Party;
    SHOP* Shop;
    SESSION* Session;
    CLAN* Clan;
    ATTRIBUTES* Attr;
    CRespawnPoints GMRespawnPoints;

    //Medal xp
    int wait_validation;
    time_t timerxp;
    int bonusxp;
    bool once;

    //Quest variables
    bool speaksLuna;
    bool canUseFlyingVessel;

    //LMA New Quests
    UINT QuestVariables[25];

    // Inventory/storage
    CItem storageitems[MAX_STORAGE];
    CItem itemmallitems[MAX_ITEMMALL];
    unsigned int nstorageitems;
    unsigned int nsitemmallitems;
    BSItem wishlistitems[MAX_WISHLIST];

    CItem items[MAX_INVENTORY];

    float attack_fuel;   //LMA: conso fuel.

    // skills/quickbar
    SKILLS cskills[MAX_ALL_SKILL];

    //LMA: % for dealer (when buying from NPC).
    int pc_rebate;

    int cur_max_skills[5];

    UINT quickbar[MAX_QUICKBAR];
    int p_skills;
    int dual_scratch_index; //LMA: trying to guess dual scratch skill index...

    // ExJam Quest Code - Info
    DWORD ActiveQuest;  //active quest #
    int CheckQuest; // The currently checked quest.
    strings TriggerName[10];
    SQuestData quest; // Character's quest information

    //LMA: for no qsd zone
    bool skip_qsd_zone;

    // Time
	clock_t lastRegenTime;
	clock_t lastRegenTime_hp;   //LMA: regen for HP
	clock_t lastRegenTime_mp;   //LMA: regen for MP
	clock_t lastShowTime;       //LMA HP Jumping
	clock_t firstlogin;         //LMA for fairy
	clock_t lastSaveTime;
    clock_t lastportalchecktime;  //custom events
    clock_t lastEventTime;        //custom events
    clock_t RefreshEventTimer;	  //custom events
	clock_t lastGG;

	// Visible Lists
    vector<CPlayer*>	        VisiblePlayers;	   // Visible players
    vector<CDrop*>		        VisibleDrops;	   // Visible drops

    //LMATEST
    //vector<class CMonster*>		    VisibleMonsters;   // Visible monsters
    vector<unsigned int>            VisibleMonsters;   // Visible monsters

    vector<CNPC*>			    VisibleNPCs;	   // Visible npcs

    // Functions
    bool CheckPlayerLevelUP( );
    bool CheckDoubleEquip( );  //LMA: Core fix for double weapon and shield
    bool CheckZulies( );
    bool CheckPortal( );  //custom events
    bool CheckEvents( );  //custom events
  //bool CheckCustomQuest( );
    bool PrizeExchange(CPlayer* thisclient, UINT prizeid );  //custom events
    bool CheckItems ( );
    void SetStats( );
    bool GetPlayerInfo( );
    bool IsMonster( );
    bool CleanPlayerVector( );
    bool loaddata( );
    void saveskills();  //LMA: Only saving skills there.
    void savedata( );
    int GoodSkill(int skill_id);    //LMA: which skill family?
    int FindSkillOffset(int family);    //LMA: Find a skill offset...
    void ResetSkillOffset(); //LMA: reset skill offsets.
    void AttrAllSkills();   //LMA: searchs all the skills for a player (to be used after allskill gm command for example).
    void AttrGMSkills();   //DK: ???
    void SaveSkillInfo(int family,int offset,int id,int level);    //LMA: Save some skills informations for later...
    //void UpgradeSkillInfo(int offset,int skillid,int nb_upgrade);  //LMA: Upgrade a skill level...
    void CalculateSignature( int slot );    //LMA: get item signature
    int CheckSignature( int slot );         //LMA: check signature
    UINT GetNewStorageItemSlot( CItem thisitem );
    UINT GetPlayerSkill( unsigned int id ); // Required skill check by insider
    UINT GetNewItemSlot( CItem thisitem );
    bool ClearObject( unsigned int otherclientid );
    void RestartPlayerVal( );
    bool Regeneration( );
    bool PlayerHeal( );
    bool VisiblityList( );
    bool ForceRefreshMonster(bool refresh_all, UINT monster_id);      //LMA: force refresh (20070621-211100)
    bool SpawnToPlayer( CPlayer* player, CPlayer* otherclient );
    int ReturnPvp( CPlayer* player, CPlayer* otherclient );    //LMA: for PVP.
    bool UpdateValues( );
    bool RefreshHPMP();  //LMA HP / MP Jumping
    void ReduceABC( );
    CParty* GetParty( );
    CLAN* GetClan( );
    unsigned int AddItem( CItem item );
    void TakeFuel(int add_fuel=0);   //LMA: Using fuel :)
    void GiveCP(unsigned int points);    //LMA: Give Clan Points
    void UpdateInventory( unsigned int slot1, unsigned int slot2=0xffff, bool save=true);   //LMA: this one saves into database immediatly
    void UpdateInventoryNoSave( unsigned int slot1, unsigned int slot2=0xffff );   //LMA: This one doesn't immediatly save into database.
    void SaveSlot( unsigned int slot); //LMA: Saving slot into MySQL database.
    void SaveSlot41( unsigned int slot); //LMA: Saving slot into MySQL database (Mysql 4.1+ function).
    void reduceItemsLifeSpan( bool attacked);
    bool SaveQuest( QUESTS* myquest );       //LMA: Saving quests data (Mysql 4.1+ function).
    bool PlasticSurgeon(CQuest* thisquest);      //LMA: Plastic Surgery coupons
    bool CheckItem(int itemnb,int familyid,int nb);   //Check if a peculiar item is in inventory

    // ExJam Quest Code
    void savequests( CPlayer* thisclient );
    SQuest* GetActiveQuest( );
    int GetQuestVar(short nVarType, short nVarNO);
    void SetQuestVar(short nVarType, short nVarNO, short nValue);
    //int ExecuteQuestTrigger(dword hash);
    int ExecuteQuestTrigger(dword hash, bool send_packet=false);    //LMA: Special case, we send the "good" quest name.

    // Player Stats
    unsigned int GetAttackPower( );
    unsigned int GetDefense( );
    unsigned int GetDodge( );
    unsigned int GetAccury( );
    unsigned int GetMagicDefense( );
    unsigned int GetCritical( );
    unsigned int GetAttackSpeed( );
    unsigned int GetMoveSpeed( );
    unsigned int GetCartSpeed( );

    //unsigned int GetMaxHP( );
    unsigned long long GetMaxHP( );

    unsigned int GetMaxMP( );
    float GetAttackDistance( );
    unsigned int GetHPRegenAmount( );
    unsigned int GetMPRegenAmount( );
    unsigned int GetMaxWeight( );
    unsigned int GetCurrentWeight( );
    unsigned int GetMPReduction( );
    unsigned int GetMaxSummonGauge( );
    void GetExtraStats( );
    unsigned int GetLevelEXP( );
    unsigned int GetInt( );
    unsigned getWeaponType();
    bool AddClanPoints(unsigned int count);
    void RebuildItemMall(); //LMA: Rebuild player's itemmall.
    CClientSocket* getClient();
};

#endif
