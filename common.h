

enum collisiontypes{
	COL_NOTHING = 0,
	COL_BOX = 1,
	COL_GROUND = 2,
	COL_BODY = 4,
	COL_LIMB = 8,
	COL_TEAMA = 16,
	COL_TEAMB = 32,
	COL_COLLIDABLE = COL_BOX | COL_GROUND | COL_BODY | COL_LIMB,
	COL_ALL = 0xffffffff
};