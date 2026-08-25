
#include <graphics.h>
#include "ui/gui.h"
#include "data/structs.h"
#include "ui/npc/npcwnd.h"
#include "ui/locale.h"

int gui_npc_wnd(WND* ptr_parent, NPC* ptr_npc, int wnd_type, char lines[][100], int lines_count)
{
    WND npc_wnd;
    int i, line_width = 0, max_line = 0;
    char photo[30];
    char *genders[] = {
        "M", "F"
    };

    settextstyle(SMALL_FONT, HORIZ_DIR, 4);
    for( i = 0; i < lines_count; i++ )
    {
        line_width = textwidth(lines[i]);
        if (line_width > max_line)
            max_line = line_width;
    }

    /* NPC Name in header */
    npc_wnd.header = ptr_npc->name;
    
    /* NPC Window height based on parent window */
    npc_wnd.height = WND_NPC_DEFAULT_HEIGHT;
    npc_wnd.y = (ptr_parent->height - npc_wnd.height) / 2;

    /* NPC Windows width based on max line width */
    npc_wnd.width = 10 + 160 + max_line + 10;

    npc_wnd.x = (ptr_parent->width - npc_wnd.width) / 2;

    /* Draw window frame */
    gui_draw_wnd_proto(&npc_wnd);

    /* Draw photo */
    setcolor(4);
    rectangle(npc_wnd.x + 9, npc_wnd.y + 29, npc_wnd.x + 150, npc_wnd.y + 200);
    settextstyle(SMALL_FONT, HORIZ_DIR, 6);
    outtextxy(npc_wnd.x + 40, npc_wnd.y + 100, "NO PHOTO");

    switch(ptr_npc->faction){
        case 1:
        case 3:
            sprintf(photo, "NPC/R%s%d.BMP", genders[ptr_npc->gender], ptr_npc->portrait + 1);
        break;

        case 0:
            sprintf(photo, "NPC/A%s%d.BMP", genders[ptr_npc->gender], ptr_npc->portrait + 1);
        break;

        default:
            sprintf(photo, "NPC/S%s%d.BMP", genders[ptr_npc->gender], ptr_npc->portrait + 1);
        break;
    }

    data_reader_draw_bmp(photo, npc_wnd.x + 10, npc_wnd.y + 30);
      
    /* Draw Title Text from lines[0] */
    settextstyle(SMALL_FONT, HORIZ_DIR, 6);
    setcolor(15);
    outtextxy(npc_wnd.x + 160, npc_wnd.y + 30, lines[0]);

    /* Draw NPC Name from ptr-npc-name */
    setcolor(4);
    outtextxy(npc_wnd.x + 160, npc_wnd.y + 50, ptr_npc->name);
    setcolor(15);

    /* Draw other lines */
    settextstyle(SMALL_FONT, HORIZ_DIR, 4);
    
    for ( i = 1; i < lines_count; i++ )
    {
        outtextxy(npc_wnd.x + 160, npc_wnd.y + 65 + (i * 15), lines[i]);
    }

    /* This is NPC Quest window - YES / NO buttons */
    if (wnd_type == NPC_CHOICE_WND){
        WND btn_holder;
        BTN btn_yes, btn_no;
        int choice = 0;   

        int btn_width = 80, btn_height = 20, btn_gap = 15;
        int btn_y;
        int total = 2 * btn_width + btn_gap;

        btn_holder.y = npc_wnd.y + 180;
        btn_holder.x = npc_wnd.x + 150;

        btn_holder.width = 10 + max_line + 10;
        btn_holder.height = 20;

        btn_yes.text = LC_GUI_BOOL_YES;
        btn_yes.x = btn_holder.x + (btn_holder.width - total) / 2;
        btn_yes.y = btn_holder.y;
        btn_yes.width = btn_width;
        btn_yes.height = btn_height;
        btn_yes.selected = (choice == 0) ? 1 : 0;
        btn_yes.enabled = 1;
        btn_yes.visible = 1;

        btn_no.text = LC_GUI_BOOL_NO;
        btn_no.x = btn_yes.x + btn_yes.width + btn_gap;
        btn_no.y = btn_holder.y;
        btn_no.width = btn_width;
        btn_no.height = btn_height;
        btn_no.selected = (choice == 1) ? 1 : 0;
        btn_no.enabled = 1;
        btn_no.visible = 1;

        while (1) {
            if (choice == 0)
            {
                btn_yes.selected = 1;
                btn_no.selected = 0;
            }

            if (choice == 1)
            {
                btn_yes.selected = 0;
                btn_no.selected = 1;
            }

            gui_draw_btn(&btn_yes);
            gui_draw_btn(&btn_no);

            if (gui_handle_btn_keys(2, &choice) == 1)
            {
                return choice;
            }
        }
    }

    return -1;
}
