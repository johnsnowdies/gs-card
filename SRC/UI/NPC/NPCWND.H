
#define E_NPC_WND_TYPE enum npc_wnd_type

#define WND_NPC_DEFAULT_HEIGHT 220

E_NPC_WND_TYPE {
    NPC_QUEST_WND,
    NPC_DIALOG_WND
};

int gui_npc_wnd(WND* ptr_parent, NPC* ptr_npc, int wnd_type, char lines[][100], int lines_count);
