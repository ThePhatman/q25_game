/*
==============================================================================

GLADIATOR

==============================================================================
*/

#include "g_local.h"
#include "m_gladiator.h"

static int	sound_pain1;
static int	sound_pain2;
static int	sound_die;
static int	sound_gun;
static int	sound_cleaver_swing;
static int	sound_cleaver_hit;
static int	sound_cleaver_miss;
static int	sound_idle;
static int	sound_search;
static int	sound_sight;

/*
* 
* NOTE(gnemeth): Em3raldTig3r's BFGladiator sounds
*
*/
static int bfglad_sound_pain1;
static int bfglad_sound_pain2;
static int bfglad_sound_gun;
static int bfglad_die;
static int bfglad_idle;
static int bfglad_sight;


void gladiator_idle (edict_t *self)
{
	if (!(self->spawnflags & SF_MONSTER_AMBUSH)) {

		if (strcmp(self->classname, "monster_bfgladiator") == 0) {
			gi.sound(self, CHAN_VOICE, bfglad_idle, 1, ATTN_IDLE, 0);
		}
		else {
			gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
		}

	}
}

void gladiator_sight (edict_t *self, edict_t *other)
{
	if (strcmp(self->classname, "monster_bfgladiator") == 0) {
		gi.sound(self, CHAN_VOICE, bfglad_sight, 1, ATTN_NORM, 0);
	}
	else {
		gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
	}
}

void gladiator_search (edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_search, 1, ATTN_NORM, 0);
}

void gladiator_cleaver_swing (edict_t *self)
{
	gi.sound (self, CHAN_WEAPON, sound_cleaver_swing, 1, ATTN_NORM, 0);
}

mframe_t gladiator_frames_stand [] =
{
	ai_stand, 0, NULL,
	ai_stand, 0, NULL,
	ai_stand, 0, NULL,
	ai_stand, 0, NULL,
	ai_stand, 0, NULL,
	ai_stand, 0, NULL,
	ai_stand, 0, NULL
};
mmove_t gladiator_move_stand = {FRAME_stand1, FRAME_stand7, gladiator_frames_stand, NULL};

void gladiator_stand (edict_t *self)
{
	self->monsterinfo.currentmove = &gladiator_move_stand;
}


mframe_t gladiator_frames_walk [] =
{
	ai_walk, 15, NULL,
	ai_walk, 7,  NULL,
	ai_walk, 6,  NULL,
	ai_walk, 5,  NULL,
	ai_walk, 2,  NULL,
	ai_walk, 0,  NULL,
	ai_walk, 2,  NULL,
	ai_walk, 8,  NULL,
	ai_walk, 12, NULL,
	ai_walk, 8,  NULL,
	ai_walk, 5,  NULL,
	ai_walk, 5,  NULL,
	ai_walk, 2,  NULL,
	ai_walk, 2,  NULL,
	ai_walk, 1,  NULL,
	ai_walk, 8,  NULL
};
mmove_t gladiator_move_walk = {FRAME_walk1, FRAME_walk16, gladiator_frames_walk, NULL};

void gladiator_walk (edict_t *self)
{
	self->monsterinfo.currentmove = &gladiator_move_walk;
}


mframe_t gladiator_frames_run [] =
{
	ai_run, 23,	NULL,
	ai_run, 14,	NULL,
	ai_run, 14,	NULL,
	ai_run, 21,	NULL,
	ai_run, 12,	NULL,
	ai_run, 13,	NULL
};
mmove_t gladiator_move_run = {FRAME_run1, FRAME_run6, gladiator_frames_run, NULL};

void gladiator_run (edict_t *self)
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		self->monsterinfo.currentmove = &gladiator_move_stand;
	else
		self->monsterinfo.currentmove = &gladiator_move_run;
}


void GaldiatorMelee (edict_t *self)
{
	vec3_t	aim;

	VectorSet (aim, MELEE_DISTANCE, self->mins[0], -4);
	if (fire_hit (self, aim, (20 + (rand() %5)), 300))
		gi.sound (self, CHAN_AUTO, sound_cleaver_hit, 1, ATTN_NORM, 0);
	else
		gi.sound (self, CHAN_AUTO, sound_cleaver_miss, 1, ATTN_NORM, 0);
}

mframe_t gladiator_frames_attack_melee [] =
{
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, gladiator_cleaver_swing,
	ai_charge, 0, NULL,
	ai_charge, 0, GaldiatorMelee,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, gladiator_cleaver_swing,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, GaldiatorMelee,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL
};
mmove_t gladiator_move_attack_melee = {FRAME_melee1, FRAME_melee17, gladiator_frames_attack_melee, gladiator_run};

void gladiator_melee (edict_t *self)
{
	self->monsterinfo.currentmove = &gladiator_move_attack_melee;
}


void GladiatorGun(edict_t* self)
{
	vec3_t    start;
	vec3_t    dir;
	vec3_t    forward, right;

	AngleVectors(self->s.angles, forward, right, NULL);
	G_ProjectSource(self->s.origin, monster_flash_offset[MZ2_GLADIATOR_RAILGUN_1], forward, right, start);

	// calc direction to where we targted

	// Lazarus fog reduction of accuracy
	if (self->monsterinfo.visibility < FOG_CANSEEGOOD)
	{
		self->pos1[0] += crandom() * 640 * (FOG_CANSEEGOOD - self->monsterinfo.visibility);
		self->pos1[1] += crandom() * 640 * (FOG_CANSEEGOOD - self->monsterinfo.visibility);
		self->pos1[2] += crandom() * 320 * (FOG_CANSEEGOOD - self->monsterinfo.visibility);
	}

	VectorSubtract(self->pos1, start, dir);
	VectorNormalize(dir);

	if (strcmp(self->classname, "monster_bfgladiator") == 0) {
		monster_fire_bfg(self, start, dir, 25, 300, 100, 300, MZ2_MAKRON_BFG, true);
	}
	else {
		monster_fire_railgun(self, start, dir, 50, 100, MZ2_GLADIATOR_RAILGUN_1);
	}
}

mframe_t gladiator_frames_attack_gun [] =
{
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, GladiatorGun,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL,
	ai_charge, 0, NULL
};
mmove_t gladiator_move_attack_gun = {FRAME_attack1, FRAME_attack9, gladiator_frames_attack_gun, gladiator_run};

void gladiator_attack(edict_t* self)
{
	float    range;
	vec3_t    v;

	// a small safe zone
	VectorSubtract(self->s.origin, self->enemy->s.origin, v);
	range = VectorLength(v);
	if (range <= (MELEE_DISTANCE + 32))
		return;

	// charge up the railgun
	if (strcmp(self->classname, "monster_bfgladiator") == 0) {
		gi.sound(self, CHAN_WEAPON, bfglad_sound_gun, 1, ATTN_NORM, 0);
	}
	else {
		gi.sound(self, CHAN_WEAPON, sound_gun, 1, ATTN_NORM, 0);
	}
	VectorCopy(self->enemy->s.origin, self->pos1);    //save for aiming the shot
	self->pos1[2] += self->enemy->viewheight;
	self->monsterinfo.currentmove = &gladiator_move_attack_gun;
}


mframe_t gladiator_frames_pain [] =
{
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL
};
mmove_t gladiator_move_pain = {FRAME_pain1, FRAME_pain6, gladiator_frames_pain, gladiator_run};

mframe_t gladiator_frames_pain_air [] =
{
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL
};
mmove_t gladiator_move_pain_air = {FRAME_painup1, FRAME_painup7, gladiator_frames_pain_air, gladiator_run};

void gladiator_pain(edict_t* self, edict_t* other, float kick, int damage)
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;

	if (level.time < self->pain_debounce_time)
	{
		if ((self->velocity[2] > 100) && (self->monsterinfo.currentmove == &gladiator_move_pain))
			self->monsterinfo.currentmove = &gladiator_move_pain_air;
		return;
	}

	self->pain_debounce_time = level.time + 3;

	if (strcmp(self->classname, "monster_bfgladiator") == 0) {
		if (random() < 0.5)
			gi.sound(self, CHAN_VOICE, bfglad_sound_pain1, 1, ATTN_NORM, 0);
		else
			gi.sound(self, CHAN_VOICE, bfglad_sound_pain2, 1, ATTN_NORM, 0);
	}
	else {
		if (random() < 0.5)
			gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
		else
			gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
	}

	if (skill->value == 3)
		return;        // no pain anims in nightmare

	if (self->velocity[2] > 100)
		self->monsterinfo.currentmove = &gladiator_move_pain_air;
	else
		self->monsterinfo.currentmove = &gladiator_move_pain;
}


void gladiator_dead (edict_t *self)
{
	VectorSet (self->mins, -16, -16, -24);
	VectorSet (self->maxs, 16, 16, -8);
	self->movetype = MOVETYPE_TOSS;
	self->svflags |= SVF_DEADMONSTER;
	self->nextthink = 0;
	gi.linkentity (self);
	M_FlyCheck (self);

	// Lazarus monster fade
	if (world->effects & FX_WORLDSPAWN_CORPSEFADE)
	{
		self->think=FadeDieSink;
		self->nextthink=level.time+corpse_fadetime->value;
	}
}

mframe_t gladiator_frames_death [] =
{
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL,
	ai_move, 0, NULL
};
mmove_t gladiator_move_death = {FRAME_death1, FRAME_death22, gladiator_frames_death, gladiator_dead};

void gladiator_die(edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, vec3_t point)
{
	// Put first to see if this solveth the LOOPIN' SOUNDS
	if (self->deadflag == DEAD_DEAD)
	{
		return;
	}
	
	int n;

	self->monsterinfo.power_armor_type = POWER_ARMOR_NONE;

	// check for gib
	if (self->health <= self->gib_health && !(self->spawnflags & SF_MONSTER_NOGIB))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);
		for (n = 0; n < 2; n++)
			ThrowGib(self, "models/objects/gibs/bone/tris.md2", 0, 0, damage, GIB_ORGANIC);
		for (n = 0; n < 4; n++)
			ThrowGib(self, "models/objects/gibs/sm_meat/tris.md2", 0, 0, damage, GIB_ORGANIC);
		ThrowHead(self, "models/objects/gibs/head2/tris.md2", 0, 0, damage, GIB_ORGANIC);
		self->deadflag = DEAD_DEAD;
		return;
	}

	// regular death
	if (strcmp(self->classname, "monster_bfgladiator") == 0) {
		gi.sound(self, CHAN_VOICE, bfglad_die, 1, ATTN_NORM, 0);
	}
	else {
		gi.sound(self, CHAN_VOICE, sound_die, 1, ATTN_NORM, 0);
	}
	self->s.skinnum |= 1;
	self->deadflag = DEAD_DEAD;
	self->takedamage = DAMAGE_YES;
	self->monsterinfo.currentmove = &gladiator_move_death;
}

//===========
//PGM
qboolean gladiator_blocked (edict_t *self, float dist)
{
	if (blocked_checkshot (self, 0.25 + (0.05 * skill->value) ))
		return true;

	if (blocked_checkplat (self, dist))
		return true;

	return false;
}
//PGM
//===========

/*QUAKED monster_gladiator (1 .5 0) (-32 -32 -24) (32 32 40) Ambush Trigger_Spawn Sight GoodGuy NoGib
*/
void SP_monster_gladiator (edict_t *self)
{
	if (deathmatch->value)
	{
		G_FreeEdict (self);
		return;
	}


	sound_pain1 = gi.soundindex ("gladiator/pain.wav");	
	sound_pain2 = gi.soundindex ("gladiator/gldpain2.wav");	
	sound_die = gi.soundindex ("gladiator/glddeth2.wav");	
	sound_gun = gi.soundindex ("gladiator/railgun.wav");
	sound_cleaver_swing = gi.soundindex ("gladiator/melee1.wav");
	sound_cleaver_hit = gi.soundindex ("gladiator/melee2.wav");
	sound_cleaver_miss = gi.soundindex ("gladiator/melee3.wav");
	sound_idle = gi.soundindex ("gladiator/gldidle1.wav");
	sound_search = gi.soundindex ("gladiator/gldsrch1.wav");
	sound_sight = gi.soundindex ("gladiator/sight.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	// Lazarus: special purpose skins
	if ( self->style )
	{
		PatchMonsterModel("models/monsters/gladiatr/tris.md2");
		self->s.skinnum = self->style * 2;
	}

	self->s.modelindex = gi.modelindex ("models/monsters/gladiatr/tris.md2");
	VectorSet (self->mins, -32, -32, -24);
	VectorSet (self->maxs, 32, 32, 40);

	if (!self->health)
		self->health = 400;
	if (!self->gib_health)
		self->gib_health = -175;
	if (!self->mass)
		self->mass = 400;

	self->pain = gladiator_pain;
	self->die = gladiator_die;

	self->monsterinfo.stand = gladiator_stand;
	self->monsterinfo.walk = gladiator_walk;
	self->monsterinfo.run = gladiator_run;
	self->monsterinfo.dodge = NULL;
	self->monsterinfo.attack = gladiator_attack;
	self->monsterinfo.melee = gladiator_melee;
	self->monsterinfo.sight = gladiator_sight;
	self->monsterinfo.idle = gladiator_idle;
	self->monsterinfo.search = gladiator_search;
	self->monsterinfo.blocked = gladiator_blocked;		// PGM

	if (!self->blood_type)
		self->blood_type = 3; //sparks and blood

	// Lazarus
	if (self->powerarmor)
	{
		if (self->powerarmortype == 1)
			self->monsterinfo.power_armor_type = POWER_ARMOR_SCREEN;
		else
			self->monsterinfo.power_armor_type = POWER_ARMOR_SHIELD;
		self->monsterinfo.power_armor_power = self->powerarmor;
	}
	if (!self->monsterinfo.flies)
		self->monsterinfo.flies = 0.05;

	self->common_name = "Gladiator";
	self->class_id = ENTITY_MONSTER_GLADIATOR;

	gi.linkentity (self);

	self->monsterinfo.currentmove = &gladiator_move_stand;
	if (self->health < 0)
	{
		mmove_t	*deathmoves[] = {&gladiator_move_death,
								 NULL};
		M_SetDeath (self, (mmove_t **)&deathmoves);
	}
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start (self);
}

/**
*
* NOTE(gnemeth): Em3raldTig3r's monster_bfgladiator for Q25
*
*/

/*QUAKED monster_gladiator (1 .5 0) (-32 -32 -24) (32 32 40) Ambush Trigger_Spawn Sight GoodGuy NoGib
*/
void SP_monster_bfgladiator(edict_t* self)
{
	if (deathmatch->value)
	{
		G_FreeEdict(self);
		return;
	}

	bfglad_sound_pain1 = gi.soundindex("bfgladiator/pain.wav");
	bfglad_sound_pain2 = gi.soundindex("bfgladiator/pain2.wav");
	bfglad_die = gi.soundindex("bfgladiator/glddeth2.wav");
	bfglad_sound_gun = gi.soundindex("weapons/Bfg__f1y.wav");
	bfglad_idle = gi.soundindex("bfgladiator/gldidle1.wav");
	bfglad_sight = gi.soundindex("bfgladiator/sight.wav");

	sound_pain1 = gi.soundindex("gladiator/pain.wav");
	sound_pain2 = gi.soundindex("gladiator/gldpain2.wav");
	sound_die = gi.soundindex("gladiator/glddeth2.wav");
	sound_gun = gi.soundindex("gladiator/railgun.wav");
	sound_cleaver_swing = gi.soundindex("gladiator/melee1.wav");
	sound_cleaver_hit = gi.soundindex("gladiator/melee2.wav");
	sound_cleaver_miss = gi.soundindex("gladiator/melee3.wav");
	sound_idle = gi.soundindex("gladiator/gldidle1.wav");
	sound_search = gi.soundindex("gladiator/gldsrch1.wav");
	sound_sight = gi.soundindex("gladiator/sight.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	// Lazarus: special purpose skins
	if (self->style)
	{
		PatchMonsterModel("models/monsters/bfgladiatr/tris.md2");
		self->s.skinnum = self->style * 2;
	}

	self->s.modelindex = gi.modelindex("models/monsters/bfgladiatr/tris.md2");
	VectorSet(self->mins, -32, -32, -24);
	VectorSet(self->maxs, 32, 32, 40);

	if (!self->health)
		self->health = 500;
	if (!self->gib_health)
		self->gib_health = -175;
	if (!self->mass)
		self->mass = 400;
	if (!self->powerarmor)
		self->powerarmor = 100;

	self->pain = gladiator_pain;
	self->die = gladiator_die;

	self->monsterinfo.stand = gladiator_stand;
	self->monsterinfo.walk = gladiator_walk;
	self->monsterinfo.run = gladiator_run;
	self->monsterinfo.dodge = NULL;
	self->monsterinfo.attack = gladiator_attack;
	self->monsterinfo.melee = gladiator_melee;
	self->monsterinfo.sight = gladiator_sight;
	self->monsterinfo.idle = gladiator_idle;
	self->monsterinfo.search = gladiator_search;
	self->monsterinfo.blocked = gladiator_blocked;		// PGM

	if (!self->blood_type)
		self->blood_type = 3; //sparks and blood

	// Lazarus
	if (self->powerarmor)
	{
		if (self->powerarmortype == 1)
			self->monsterinfo.power_armor_type = POWER_ARMOR_SCREEN;
		else
			self->monsterinfo.power_armor_type = POWER_ARMOR_SHIELD;
		self->monsterinfo.power_armor_power = self->powerarmor;
	}
	if (!self->monsterinfo.flies)
		self->monsterinfo.flies = 0.05;

	self->common_name = "BFGladiator";
	self->class_id = ENTITY_MONSTER_BFGLADIATOR;

	gi.linkentity(self);

	self->monsterinfo.currentmove = &gladiator_move_stand;
	if (self->health < 0)
	{
		mmove_t* deathmoves[] = { &gladiator_move_death,
								 NULL };
		M_SetDeath(self, (mmove_t**)&deathmoves);
	}
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start(self);
}
