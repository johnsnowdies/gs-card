#include "ui/npc/npcwnd.h"

#include <graphics.h>

#include "ui/locale.h"
#include "music.h"

int gui_npc_wnd(WND* ptr_parent, NPC* ptr_npc, int wnd_type,
                char title[100], char lines[][100], int lines_count,
                char buttons[][100], int buttons_count, int buttons_orient)
{
    char *photo = (ptr_npc->photo && ptr_npc->photo[0] != '\0') ? ptr_npc->photo : 0;
    int btn_count = (wnd_type == NPC_CHOICE_WND) ? buttons_count : 0;
    char (*btn_array)[100] = (wnd_type == NPC_CHOICE_WND) ? buttons : 0;

    return gui_dialog_wnd(ptr_parent, ptr_npc->name, title, photo,
                          lines, lines_count, btn_array, btn_count, NO_SOUND, buttons_orient);
}
