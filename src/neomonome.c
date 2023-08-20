#include "neomonome.h"

// lua
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>


// extern "C" {

// NB: needed by device_monome
#include <pthread.h>

// NB: provides `write`
#include <unistd.h>

    // matron
#include "lua_eval.h"
#include "device/device_monome.h"
    // monome
#include <monome.h>
    // local copies of private weaver types/methods
// #include <weaver_image.h>

#include <internal.h>
// }


// ------------------------------------------------------------------------
// impl

// see:
// - libmonome/src/proto/mext.h
// - libmonome/src/proto/mext.c

typedef enum {
	SS_LED_COLOR_NM_GRID    = 0xF,
} nm_subsystem_t;

typedef enum {
    /* outgoing */
    CMD_LED_COLOR = 0x9
} nm_cmd_t;

static size_t outgoing_payload_lengths[16][16] = {
    // [0 ... 15][0 ... 15] = 0,

    [SS_LED_COLOR_NM_GRID] = {
        [CMD_LED_COLOR] = 3
    }
};

// copy-pasta from libmonome/src/platform/posix.c
ssize_t monome_platform_write(monome_t *monome, const uint8_t *buf, size_t nbyte) {
	ssize_t ret = write(monome->fd, buf, nbyte);

	if( ret < nbyte )
		perror("libmonome: write is missing bytes");

	if( ret < 0 )
		perror("libmonome: error in write");

	return ret;
}

// inspired by `mext_led_set` -> `mext_write_msg`
static int nm_led_color(monome_t *monome, uint8_t r, uint8_t g, uint8_t b) {
    size_t payload_length;

    const uint8_t addr = SS_LED_COLOR_NM_GRID;
    const uint8_t cmd = CMD_LED_COLOR;
    const uint8_t header = ((addr & 0xF ) << 4) | (cmd & 0xF);

    const uint8_t buff[] = {
        // header = 0xF9 = 249
        header,
        // payload
        r, g, b
    };

    payload_length = outgoing_payload_lengths[addr][cmd];
    // payload_length = 3;

    // fprintf(stderr, "sending %#02x (%#01x, %#01x): %s\n", header, addr, cmd, buff);
    // fprintf(stderr, "sending stuff\n");
    fprintf(stderr, "sending 0x%x (0x%x, 0x%x): %d %d %d \n", header, addr, cmd, r, g, b);


    return monome_platform_write(monome, buff, 1 + payload_length);
}

void dev_monome_grid_set_color(struct dev_monome *md, uint8_t r, uint8_t g, uint8_t b) {
    nm_led_color(md->m, r, g, b);
}

static int _nm_grid_set_color(lua_State *l);

/***
 * neomonome: set color
 * @function grid_set_led
 * @param dev grid device
 * @param r r
 * @param g g
 * @param g b
 */
int _nm_grid_set_color(lua_State *l) {
    lua_check_num_args(4);
    luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
    struct dev_monome *md = lua_touserdata(l, 1);
    int r = (int)luaL_checkinteger(l, 2);
    int g = (int)luaL_checkinteger(l, 3);
    int b = (int)luaL_checkinteger(l, 4);
    dev_monome_grid_set_color(md, r, g, b);
    lua_settop(l, 0);
    return 0;
}


// ------------------------------------------------------------------------
// exposition as lua module

static const luaL_Reg mod[] = {
    {NULL, NULL}
};

static luaL_Reg func[] = {
    {"set_color", _nm_grid_set_color},
    {NULL, NULL}
};


NEOMONOME_API int luaopen_neomonome(lua_State *L) {
    lua_newtable(L);

    for (int i = 0; mod[i].name; i++) {
        mod[i].func(L);
    }

    luaL_setfuncs(L, func, 0);

    lua_pushstring(L, "VERSION");
    lua_pushstring(L, NEOMONOME_VERSION);
    lua_rawset(L, -3);

    return 1;
}
