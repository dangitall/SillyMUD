/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
*/
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "protos.h"
#include "utility.h"

#define MAIN_MENU           0
#define CHANGE_NAME         1
#define CHANGE_DESC         2
#define CHANGE_FLAGS        3
#define CHANGE_TYPE         4
#define CHANGE_TYPE2        5
#define CHANGE_TYPE3        6
#define CHANGE_EXIT         7
#define CHANGE_EXIT_NORTH   8
#define CHANGE_EXIT_EAST    9
#define CHANGE_EXIT_SOUTH   10
#define CHANGE_EXIT_WEST    11
#define CHANGE_EXIT_UP      12
#define CHANGE_EXIT_DOWN    13
#define CHANGE_EXIT_DELETE  14
#define CHANGE_NUMBER_NORTH 15
#define CHANGE_NUMBER_EAST  16
#define CHANGE_NUMBER_SOUTH 17
#define CHANGE_NUMBER_WEST  18
#define CHANGE_NUMBER_UP    19
#define CHANGE_NUMBER_DOWN  20
#define CHANGE_KEY_NORTH    21
#define CHANGE_KEY_EAST     22
#define CHANGE_KEY_SOUTH    23
#define CHANGE_KEY_WEST     24
#define CHANGE_KEY_UP       25
#define CHANGE_KEY_DOWN     26

#define ENTER_CHECK        1

extern const char *room_bits[];
extern const char *exit_bits[];
extern const char *sector_types[];


char *edit_menu = "    1) Name                       2) Description\n\r"
  "    3) Flags                      4) Sector Type\n\r"
  "    5) Exits\n\r\n\r";

char *exit_menu = "    1) North                      2) East\n\r"
  "    3) South                      4) West\n\r"
  "    5) Up                         6) Down\n\r" "\n\r";


void change_room_flags(struct room_data *rp, struct char_data *ch, char *arg,
                       int type) {
  int i, row, update;

  if (type != ENTER_CHECK)
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_room_menu(ch);
      return;
    }

  update = atoi(arg);
  update--;
  if (type != ENTER_CHECK) {
    if (update < 0 || update > 13)
      return;
    i = 1 << update;

    if (IS_SET(rp->room_flags, i))
      REMOVE_BIT(rp->room_flags, i);
    else
      SET_BIT(rp->room_flags, i);
  }

  send_to_charf(ch, VT_HOMECLR);
  send_to_charf(ch, "Room Flags:");

  row = 0;
  for (i = 0; i < 14; i++) {
    send_to_charf(ch, VT_CURSPOS, row + 4, ((i & 1) ? 45 : 5));
    if (i & 1)
      row++;
    send_to_charf(ch, "%-2d [%s] %s", i + 1,
                  ((rp->room_flags & (1 << i)) ? "X" : " "), room_bits[i]);
  }

  send_to_charf(ch, VT_CURSPOS, 20, 1);
  send_to_char
    ("Select the number to toggle, <C/R> to return to main menu.\n\r--> ", ch);
}

void do_redit(struct char_data *ch, char *UNUSED(arg),
              const char * UNUSED(cmd)) {
#ifndef TEST_SERVER
  struct room_data *rp;

  rp = real_roomp(ch->in_room);
#endif

  if (IS_NPC(ch))
    return;

  if ((IS_NPC(ch)) || (get_max_level(ch) < LOW_IMMORTAL))
    return;

  if (!ch->desc)                /* someone is forced to do something. can be bad! */
    return;                     /* the ch->desc->str field will cause problems... */

#ifndef TEST_SERVER
  if ((get_max_level(ch) < 56) && (rp->zone != GET_ZONE(ch))) {
    send_to_char("Sorry, you are not authorized to edit this zone.\n\r", ch);
    return;
  }
#endif

  ch->specials.edit = MAIN_MENU;
  ch->desc->connected = CON_EDITING;

  act("$n has begun editing.", FALSE, ch, 0, 0, TO_ROOM);

  update_room_menu(ch);
}


void update_room_menu(struct char_data *ch) {
  struct room_data *rp;

  rp = real_roomp(ch->in_room);

  send_to_char(VT_HOMECLR, ch);
  send_to_charf(ch, VT_CURSPOS, 1, 1);
  send_to_charf(ch, "Room Name: %s", rp->name);
  send_to_charf(ch, VT_CURSPOS, 1, 40);
  send_to_charf(ch, "Number: %d", rp->number);
  send_to_charf(ch, VT_CURSPOS, 1, 60);
  send_to_charf(ch, "Sector Type: %s", sector_types[rp->sector_type]);
  send_to_charf(ch, VT_CURSPOS, 3, 1);
  send_to_char("Menu:\n\r", ch);
  send_to_char(edit_menu, ch);
  send_to_char("--> ", ch);
}


void room_edit(struct char_data *ch, char *arg) {
  if (ch->specials.edit == MAIN_MENU) {
    if (!*arg || *arg == '\n') {
      ch->desc->connected = CON_PLYNG;
      act("$n has returned from editing.", FALSE, ch, 0, 0, TO_ROOM);
      return;
    }
    switch (atoi(arg)) {
    case 0:
      update_room_menu(ch);
      return;
    case CHANGE_NAME:
      ch->specials.edit = CHANGE_NAME;
      change_room_name(real_roomp(ch->in_room), ch, "", ENTER_CHECK);
      return;
    case CHANGE_DESC:
      ch->specials.edit = CHANGE_DESC;
      change_room_desc(real_roomp(ch->in_room), ch, "", ENTER_CHECK);
      return;
    case CHANGE_FLAGS:
      ch->specials.edit = CHANGE_FLAGS;
      change_room_flags(real_roomp(ch->in_room), ch, "", ENTER_CHECK);
      return;
    case CHANGE_TYPE:
      ch->specials.edit = CHANGE_TYPE;
      change_room_type(real_roomp(ch->in_room), ch, "", ENTER_CHECK);
      return;
    case 5:
      ch->specials.edit = CHANGE_EXIT;
      change_exit_dir(real_roomp(ch->in_room), ch, "", ENTER_CHECK);
      return;
    default:
      update_room_menu(ch);
      return;
    }
  }

  switch (ch->specials.edit) {
  case CHANGE_NAME:
    change_room_name(real_roomp(ch->in_room), ch, arg, 0);
    return;
  case CHANGE_DESC:
    change_room_desc(real_roomp(ch->in_room), ch, arg, 0);
    return;
  case CHANGE_FLAGS:
    change_room_flags(real_roomp(ch->in_room), ch, arg, 0);
    return;
  case CHANGE_TYPE:
  case CHANGE_TYPE2:
  case CHANGE_TYPE3:
    change_room_type(real_roomp(ch->in_room), ch, arg, 0);
    return;
  case CHANGE_EXIT:
    change_exit_dir(real_roomp(ch->in_room), ch, arg, 0);
    return;
  case CHANGE_EXIT_NORTH:
  case CHANGE_EXIT_EAST:
  case CHANGE_EXIT_SOUTH:
  case CHANGE_EXIT_WEST:
  case CHANGE_EXIT_UP:
  case CHANGE_EXIT_DOWN:
    add_exit_to_room(real_roomp(ch->in_room), ch, arg, 0);
    return;
  case CHANGE_KEY_NORTH:
  case CHANGE_KEY_EAST:
  case CHANGE_KEY_SOUTH:
  case CHANGE_KEY_WEST:
  case CHANGE_KEY_UP:
  case CHANGE_KEY_DOWN:
    change_key_number(real_roomp(ch->in_room), ch, arg, 0);
    return;
  case CHANGE_NUMBER_NORTH:
  case CHANGE_NUMBER_EAST:
  case CHANGE_NUMBER_SOUTH:
  case CHANGE_NUMBER_WEST:
  case CHANGE_NUMBER_UP:
  case CHANGE_NUMBER_DOWN:
    change_exit_number(real_roomp(ch->in_room), ch, arg, 0);
    return;
  default:
    log_msg("Got to bad spot in room_edit");
    return;
  }
}


void change_room_name(struct room_data *rp, struct char_data *ch, char *arg,
                      int type) {
  if (type != ENTER_CHECK)
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_room_menu(ch);
      return;
    }

  if (type != ENTER_CHECK) {
    if (rp->name)
      free(rp->name);
    rp->name = (char *)strdup(arg);
    ch->specials.edit = MAIN_MENU;
    update_room_menu(ch);
    return;
  }

  send_to_charf(ch, VT_HOMECLR);

  send_to_charf(ch, "Current Room Name: %s", rp->name);
  send_to_char("\n\r\n\rNew Room Name: ", ch);

  return;
}

void change_room_desc(struct room_data *rp, struct char_data *ch,
                      char *UNUSED(arg), int type) {

  if (type != ENTER_CHECK) {
    ch->specials.edit = MAIN_MENU;
    update_room_menu(ch);
    return;
  }

  send_to_charf(ch, VT_HOMECLR);

  send_to_charf(ch, "Current Room Description:\n\r");
  send_to_char(rp->description, ch);
  send_to_char("\n\r\n\rNew Room Description:\n\r", ch);
  send_to_char("(Terminate with a @. Press <C/R> again to continue)\n\r", ch);
  free(rp->description);
  rp->description = NULL;
  ch->desc->str = &rp->description;
  ch->desc->max_str = MAX_STRING_LENGTH;
  return;
}


void change_room_type(struct room_data *rp, struct char_data *ch, char *arg,
                      int type) {
  int i, row, update;

  if (type != ENTER_CHECK)
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_room_menu(ch);
      return;
    }

  update = atoi(arg);
  update--;

  if (type != ENTER_CHECK) {
    switch (ch->specials.edit) {
    case CHANGE_TYPE:
      if (update < 0 || update > 11)
        return;
      else {
        rp->sector_type = update;
        if (rp->sector_type == SECT_WATER_NOSWIM) {
          send_to_char("\n\rRiver Speed: ", ch);
          ch->specials.edit = CHANGE_TYPE2;
        }
        else {
          ch->specials.edit = MAIN_MENU;
          update_room_menu(ch);
        }
        return;
      }
    case CHANGE_TYPE2:
      rp->river_speed = update;
      send_to_char("\n\rRiver Direction (0 - 5): ", ch);
      ch->specials.edit = CHANGE_TYPE3;
      return;
    case CHANGE_TYPE3:
      update++;
      if (update < 0 || update > 5) {
        send_to_char("Direction must be between 0 and 5.\n\r", ch);
        return;
      }
      rp->river_dir = update;
      ch->specials.edit = MAIN_MENU;
      update_room_menu(ch);
      return;
    }
  }

  send_to_charf(ch, VT_HOMECLR);
  send_to_charf(ch, "Sector Type: %s", sector_types[rp->sector_type]);

  row = 0;
  for (i = 0; i < 12; i++) {
    send_to_charf(ch, VT_CURSPOS, row + 4, ((i & 1) ? 45 : 5));
    if (i & 1)
      row++;
    send_to_charf(ch, "%-2d %s", i + 1, sector_types[i]);
  }

  send_to_charf(ch, VT_CURSPOS, 20, 1);
  send_to_char
    ("Select the number to set to, <C/R> to return to main menu.\n\r--> ", ch);
}


void change_exit_dir(struct room_data *rp, struct char_data *ch, char *arg,
                     int type) {
  int update;

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = MAIN_MENU;
      update_room_menu(ch);
      return;
    }

    update = atoi(arg) - 1;
    if (update == -1) {
      change_exit_dir(rp, ch, "", ENTER_CHECK);
      return;
    }

    switch (update) {
    case 0:
      ch->specials.edit = CHANGE_EXIT_NORTH;
      add_exit_to_room(rp, ch, "", ENTER_CHECK);
      return;
      break;
    case 1:
      ch->specials.edit = CHANGE_EXIT_EAST;
      add_exit_to_room(rp, ch, "", ENTER_CHECK);
      return;
      break;
    case 2:
      ch->specials.edit = CHANGE_EXIT_SOUTH;
      add_exit_to_room(rp, ch, "", ENTER_CHECK);
      return;
      break;
    case 3:
      ch->specials.edit = CHANGE_EXIT_WEST;
      add_exit_to_room(rp, ch, "", ENTER_CHECK);
      return;
      break;
    case 4:
      ch->specials.edit = CHANGE_EXIT_UP;
      add_exit_to_room(rp, ch, "", ENTER_CHECK);
      return;
      break;
    case 5:
      ch->specials.edit = CHANGE_EXIT_DOWN;
      add_exit_to_room(rp, ch, "", ENTER_CHECK);
      return;
      break;
    case 6:
      ch->specials.edit = CHANGE_EXIT_DELETE;
      delete_exit(rp, ch, "", ENTER_CHECK);
      return;
      break;
    default:
      change_exit_dir(rp, ch, "", ENTER_CHECK);
      return;
    }

  }


  send_to_char(VT_HOMECLR, ch);
  send_to_charf(ch, "Room Name: %s", rp->name);
  send_to_charf(ch, VT_CURSPOS, 1, 40);
  send_to_charf(ch, "Room Number: %d", rp->number);
  send_to_charf(ch, VT_CURSPOS, 4, 1);
  send_to_char(exit_menu, ch);
  send_to_char("--> ", ch);
  return;
}


void add_exit_to_room(struct room_data *rp, struct char_data *ch, char *arg,
                      int type) {
  int update, dir, row, i = 0;

  extern const char *exit_bits[];

  switch (ch->specials.edit) {
  case CHANGE_EXIT_NORTH:
    dir = 0;
    break;
  case CHANGE_EXIT_EAST:
    dir = 1;
    break;
  case CHANGE_EXIT_SOUTH:
    dir = 2;
    break;
  case CHANGE_EXIT_WEST:
    dir = 3;
    break;
  case CHANGE_EXIT_UP:
    dir = 4;
    break;
  case CHANGE_EXIT_DOWN:
    dir = 5;
    break;
  }

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      switch (dir) {
      case 0:
        ch->specials.edit = CHANGE_NUMBER_NORTH;
        break;
      case 1:
        ch->specials.edit = CHANGE_NUMBER_EAST;
        break;
      case 2:
        ch->specials.edit = CHANGE_NUMBER_SOUTH;
        break;
      case 3:
        ch->specials.edit = CHANGE_NUMBER_WEST;
        break;
      case 4:
        ch->specials.edit = CHANGE_NUMBER_UP;
        break;
      case 5:
        ch->specials.edit = CHANGE_NUMBER_DOWN;
        break;
      }
      send_to_char("\n\r\n\rExit to Room: ", ch);
      change_exit_number(rp, ch, "", ENTER_CHECK);
      return;
    }

    update = atoi(arg) - 1;

    if (update < 0 || update > 6)
      return;
    i = 1 << update;

    if (IS_SET(rp->dir_option[dir]->exit_info, i))
      REMOVE_BIT(rp->dir_option[dir]->exit_info, i);
    else
      SET_BIT(rp->dir_option[dir]->exit_info, i);
  }

  else if (!rp->dir_option[dir]) {
    CREATE(rp->dir_option[dir], struct room_direction_data, 1);
    rp->dir_option[dir]->exit_info = 0;
  }

  send_to_charf(ch, VT_HOMECLR);
  send_to_charf(ch, "Exit Flags:");

  row = 0;
  for (i = 0; i < 7; i++) {
    send_to_charf(ch, VT_CURSPOS, row + 4, ((i & 1) ? 45 : 5));
    if (i & 1)
      row++;
    send_to_charf(ch, "%-2d [%s] %s", i + 1,
                  ((rp->dir_option[dir]->exit_info & (1 << i)) ? "X" : " "),
                  exit_bits[i]);
  }

  send_to_charf(ch, VT_CURSPOS, 20, 1);
  send_to_char
    ("Select the number to toggle, <C/R> to return to continue.\n\r--> ", ch);
}


void change_exit_number(struct room_data *rp, struct char_data *ch, char *arg,
                        int type) {
  int dir, update;

  switch (ch->specials.edit) {
  case CHANGE_NUMBER_NORTH:
    dir = 0;
    break;
  case CHANGE_NUMBER_EAST:
    dir = 1;
    break;
  case CHANGE_NUMBER_SOUTH:
    dir = 2;
    break;
  case CHANGE_NUMBER_WEST:
    dir = 3;
    break;
  case CHANGE_NUMBER_UP:
    dir = 4;
    break;
  case CHANGE_NUMBER_DOWN:
    dir = 5;
    break;
  }

  if (type == ENTER_CHECK)
    return;

  update = atoi(arg);

  if (update < 0 || update > 29000) {
    send_to_char("\n\rRoom number must be between 0 and 29000.\n\r", ch);
    send_to_char("\n\rExit to Room: ", ch);
    return;
  }

  rp->dir_option[dir]->to_room = update;

  switch (dir) {
  case 0:
    ch->specials.edit = CHANGE_KEY_NORTH;
    break;
  case 1:
    ch->specials.edit = CHANGE_KEY_EAST;
    break;
  case 2:
    ch->specials.edit = CHANGE_KEY_SOUTH;
    break;
  case 3:
    ch->specials.edit = CHANGE_KEY_WEST;
    break;
  case 4:
    ch->specials.edit = CHANGE_KEY_UP;
    break;
  case 5:
    ch->specials.edit = CHANGE_KEY_DOWN;
    break;
  }

  send_to_char("\n\rKey Number (0 for none): ", ch);

  change_key_number(rp, ch, "", ENTER_CHECK);
}


void change_key_number(struct room_data *rp, struct char_data *ch, char *arg,
                       int type) {
  int dir, update;

  switch (ch->specials.edit) {
  case CHANGE_KEY_NORTH:
    dir = 0;
    break;
  case CHANGE_KEY_EAST:
    dir = 1;
    break;
  case CHANGE_KEY_SOUTH:
    dir = 2;
    break;
  case CHANGE_KEY_WEST:
    dir = 3;
    break;
  case CHANGE_KEY_UP:
    dir = 4;
    break;
  case CHANGE_KEY_DOWN:
    dir = 5;
    break;
  }

  if (type == ENTER_CHECK)
    return;

  update = atoi(arg);

  if (!rp->dir_option[dir]->keyword)
    rp->dir_option[dir]->keyword = (char *)strdup("door");

  if (update < 0) {
    send_to_char("\n\rKey number must be greater than 0.\n\r", ch);
    send_to_char("\n\rKey Number (0 for none): ", ch);
    return;
  }

  rp->dir_option[dir]->key = update;

  ch->specials.edit = CHANGE_EXIT;
  change_exit_dir(rp, ch, "", ENTER_CHECK);
}

void delete_exit(struct room_data *UNUSED(rp), struct char_data *ch,
                 char *UNUSED(arg), int UNUSED(type)) {
  ch->specials.edit = MAIN_MENU;
  update_room_menu(ch);
}
