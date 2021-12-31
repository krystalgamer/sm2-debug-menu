#include "menu.h"

#define TEXT_VARIABLE __attribute__((section(".text")))
#define START_FUNC __attribute__((alias("aqui_menu")))

static int menu_enabled = 0;
static int menus_inited = 0;
static char buffer[1024];
static volatile char *alias_buffer;
static debug_menu* current_menu;
static debug_menu* start_menu;




#define my_printf(format, ...) {\
	sprintf_orig(buffer, format, __VA_ARGS__);\
	actual_printer(5);\
	}

#define my_print printf_syscall




int printf_syscall(const char* passedInA0, ...){
	    asm __volatile__(
		    "li $v1,0x75 \n"
		    "syscall    \n"
		    );
}

void __attribute__((noinline)) actual_printer(int i) {
	printf_syscall(buffer);
}

void* memset(void *ptr,  int b, long int size){

	unsigned char* cptr = ptr;
	for(unsigned int i = 0; i<size; i++)
		cptr[i] = b;
	return ptr;
}

void* memcpy(void *dest, const void *src, unsigned int n){

	unsigned char* d = dest;
	const unsigned char* s = src;
	for(unsigned int i =0; i<n; i++)
		d[i] = s[i];

	return dest;
}

void *realloc(void*ptr, unsigned int size){
//todo
}


char *strncpy(char *dest, const char *src, unsigned int n){

	for(unsigned int i = 0; i<n; i++){
		dest[i] = src[i];

		if(!dest[i])
			break;
	}

	return dest;
}

typedef int (*pause_menu_key_handler_ptr)(int *a1, int pad, int dualshock);
static const pause_menu_key_handler_ptr pause_menu_key_handler_orig = (void*)0x00353F5C;


typedef int (*sprintf_ptr)(char* dst, char *format, ...);
static const sprintf_ptr sprintf_orig = (void*)0x0058C908;


typedef void* (*malloc_ptr)(unsigned int size);
static const malloc_ptr malloc = (void*)0x0058CC00;

typedef void (*free_ptr)(void *ptr);
static const free_ptr free = (void*)0x0058CC50;


typedef int (*game_get_cur_state_ptr)(void *game_ptr);
static const game_get_cur_state_ptr game_get_cur_state = (void*)0x00395A18;

typedef void (*game_pause_ptr)(void *game_ptr);
static const game_pause_ptr game_pause = (void*)0x003E9408;

typedef void (*game_unpause_ptr)(void *game_ptr, int a2);
static const game_unpause_ptr game_unpause = (void*)0x003E95E8;

//NGL STUFF
typedef struct nglQuad{
	char unk[0x64];
}nglQuad;



/*
typedef int (*nglGetScreenHeight_ptr)(void);
static const nglGetScreenHeight_ptr nglGetScreenHeight = (void*)0x00631AFC;
*/

typedef int (*nglGetScreenWidth_ptr)(void);
static const nglGetScreenWidth_ptr nglGetScreenWidth = (void*)0x0050CA58;

typedef int (*nglInitQuad_ptr)(nglQuad *a1);
static const nglInitQuad_ptr nglInitQuad = (void*)0x005136D4;


typedef void (*nglSetQuadColor_ptr)(nglQuad* quad, int color);
static const nglSetQuadColor_ptr nglSetQuadColor = (void*)0x00513794;


typedef void (*nglListAddQuad_ptr)(nglQuad* quad);
static const nglListAddQuad_ptr nglListAddQuad = (void*)0x00513464;

/*
typedef void (*nglSetQuadRect_ptr)(nglQuad* quad, float x, float y, float x_end, float y_end);
static const nglSetQuadRect_ptr nglSetQuadRect = (void*)0x005137FC;

typedef void(*nglListAddString_ptr)(void *font, char *str, float x, float y, float z, int color, float x_scale, float y_scale);
static const nglListAddString_ptr nglListAddString = (void*)0x00511858;

typedef void (*nglSetQuadZ_ptr)(nglQuad* quad, float z);
static const nglSetQuadZ_ptr nglSetQuadZ = (void*)0x00513820;
*/


/*
 * Ignore this one
typedef void(*nglListAddStringF_ptr)(void *font, float x, float y, float z, int color, float x_scale, float y_scale, char *format, ...);
static const nglListAddStringF_ptr nglListAddStringF = (void*)0x005117B0;
*/



//GCC has bad calling convetion definition which borks the registers

void nglSetQuadZ(nglQuad *quad, int z){

	    asm __volatile__(
		"subu $sp, $sp, 4\n"
		"sw $ra, 0($sp)\n"

		 "mtc1 $a1, $f12\n"

		 "li $t9, 0x00513820\n"
		 "jal $t9\n"

		 "nop\n"
		 "ld $ra, 0($sp)\n"
		 "add $sp, $sp, 4\n"
		);
}

void nglSetQuadRect(nglQuad *quad, int x, int y, int x_end, int y_end){

	    asm __volatile__(
		"subu $sp, $sp, 4\n"
		"sw $ra, 0($sp)\n"

		 "mtc1 $a1, $f12\n"
		 "mtc1 $a2, $f13\n"
		 "mtc1 $a3, $f14\n"
		 "mtc1 $a4, $f15\n"

		 "li $t9, 0x005137FC\n"
		 "jal $t9\n"

		 "nop\n"
		 "ld $ra, 0($sp)\n"
		 "add $sp, $sp, 4\n"
		);

}

void nglListAddString(void *font, const char *str, int color, int x, int y, int z,  int x_scale, int y_scale){

	asm __volatile__(
		"subu $sp, $sp, 4\n"
		"sw $ra, 0($sp)\n"

		"mtc1 $a3, $f12\n"
		"mtc1 $a4, $f13\n"
		"mtc1 $a5, $f14\n"
		"mtc1 $a6, $f15\n"
		"mtc1 $a7, $f16\n"

		 "li $t9, 0x00511858\n"
		 "jal $t9\n"

		 "nop\n"
		 "ld $ra, 0($sp)\n"
		 "add $sp, $sp, 4\n"

		);
}

void nglGetStringDimensions(void *font, const char *str, int *width, int *height, int x_scale, int y_scale){
	asm __volatile__ (
		"subu $sp, $sp, 8\n"
		"sw $ra, 0($sp)\n"
		//"sw $v1, 4($sp)\n"

		"mtc1 $a4, $f12\n"
		"mtc1 $a5, $f13\n"

		 "li $t9, 0x00511BC0\n"
		 "jal $t9\n"

		 "nop\n"
		 "ld $ra, 0($sp)\n"
		 //"ld $v1, 4($sp)\n"
		 "add $sp, $sp, 8\n"
	       );

}

static void **nglSysFont = (void**)0x00611080;
static void **g_game_ptr = (void**)0x00608314;


typedef void (*sub_3E9A90_ptr)(int a1);
static const sub_3E9A90_ptr sub_3E9A90_orig = (void*)0x3E9A90;
void sub_3E9A90(int a1){

	if(menu_enabled)
		return;

	return sub_3E9A90_orig(a1);
}

enum key {
	SELECT=0,
	START,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	CROSS,
	TRIANGLE,
	SQUARE,
	CIRCLE,
	L1,
	R1,
	L2,
	R2
};


#define ENUM_TO_STRING(x) [x] = #x

static const char *key_names[14] = {
	ENUM_TO_STRING(SELECT),
	ENUM_TO_STRING(START),
	ENUM_TO_STRING(UP),
	ENUM_TO_STRING(DOWN),
	ENUM_TO_STRING(LEFT),
	ENUM_TO_STRING(RIGHT),
	ENUM_TO_STRING(CROSS),
	ENUM_TO_STRING(TRIANGLE),
	ENUM_TO_STRING(SQUARE),
	ENUM_TO_STRING(CIRCLE),
	ENUM_TO_STRING(L1),
	ENUM_TO_STRING(R1),
	ENUM_TO_STRING(L2),
	ENUM_TO_STRING(R2)
};




void game_unpauser_unpause_hook(void *a1, int a2){

	if(menu_enabled)
		return;
	game_unpause(a1, a2);
}

void __start() START_FUNC;


void* add_debug_menu_entry(debug_menu* menu, debug_menu_entry* entry) {

	if (menu->used_slots < menu->capacity) {
		void* ret = &menu->entries[menu->used_slots];
		memcpy(ret, entry, sizeof(debug_menu_entry));
		menu->used_slots++;
		return ret;
	}
	else {
		DWORD current_entries_size = sizeof(debug_menu_entry) * menu->capacity;
		DWORD new_entries_size = sizeof(debug_menu_entry) * EXTEND_NEW_ENTRIES;


		void* new_ptr = realloc(menu->entries, current_entries_size + new_entries_size);

		if (!new_ptr) {
			my_print("RIP");
		}
		else {

			menu->capacity += EXTEND_NEW_ENTRIES;
			menu->entries = new_ptr;
			memset(&menu->entries[menu->used_slots], 0, new_entries_size);

			return add_debug_menu_entry(menu, entry);
		}
	}
}

debug_menu* create_menu(const char* title, go_back_function go_back, menu_handler_function function, DWORD capacity) {

	debug_menu* menu = malloc(sizeof(debug_menu));
	memset(menu, 0, sizeof(debug_menu));

	strncpy(menu->title, title, MAX_CHARS_SAFE);

	menu->capacity = capacity;
	menu->handler = function;
	menu->go_back = go_back;

	DWORD total_entries_size = sizeof(debug_menu_entry) * capacity;
	menu->entries = malloc(total_entries_size);
	memset(menu->entries, 0, total_entries_size);

	return menu;

}



typedef union Converter{
	int i;
	float f;
}Converter;

int __inline cfi(float f){
	Converter c;
	c.f = f;
	return c.i;
}


/*
 * Menus
 */


#define max(a,b) ((a) < (b) ? (b) : (a))
#define min(a,b) ((a) < (b) ? (a) : (b))
static const char *UP_ARROW = " ^ ^ ^ ";
static const char *DOWN_ARROW = " v v v ";


char* getRealText(debug_menu_entry* entry, char* str) {

	if (entry->entry_type == BOOLEAN_E) {
		unsigned char* val = entry->data;
		sprintf_orig(str, "%s: %s", entry->text, *val ? "True" : "False");
		return str;
	}

	if (entry->entry_type == CUSTOM) {
		return entry->custom_string_generator(entry);
	}

	return entry->text;
}

void getStringDimensions(const char* str, int* width, int* height) {
	nglGetStringDimensions(*nglSysFont, str, width, height, cfi(1.0), cfi(1.0));
}

int getStringHeight(const char* str) {
	int height;
	nglGetStringDimensions(*nglSysFont, str, (void*)0, &height, cfi(1.0), cfi(1.0));
	return height;
}


typedef void (*merda_ptr)(const char*);
static const merda_ptr merda = (void*)0x90909090;
void __attribute__((noinline)) first(const char* f){
	my_print(f);
}

void __attribute__((noinline)) second(const char* f, ...){
	
	my_print(f);
}

void exemplo(){
	const char *c = "CONA";
	first(c);
	first(c);
}

void render_current_debug_menu() {


	char text_buffer[128];


	int num_elements = min(MAX_ELEMENTS_PAGE, current_menu->used_slots - current_menu->window_start);
	int needs_down_arrow = ((current_menu->window_start + MAX_ELEMENTS_PAGE) < current_menu->used_slots) ? 1 : 0;


	int cur_width, cur_height;
	int debug_width = 0;
	int debug_height = 0;

	#define get_and_update(x) {\
		 getStringDimensions(x, &cur_width, &cur_height); \
		 debug_height += cur_height; \
		 debug_width = max(debug_width, cur_width);\
	}


	get_and_update(current_menu->title);
	get_and_update(UP_ARROW);



	int total_elements_page = needs_down_arrow ? MAX_ELEMENTS_PAGE : current_menu->used_slots - current_menu->window_start;

	for (int i = 0; i < total_elements_page; i++) {

		debug_menu_entry *entry = &current_menu->entries[current_menu->window_start + i];
		char* cur = getRealText(entry, text_buffer);
		get_and_update(cur);
	}


	if (needs_down_arrow) {
		get_and_update(DOWN_ARROW);
	}

	nglQuad quad;


	int menu_x_start = 20, menu_y_start = 40;
	int menu_x_pad = 24, menu_y_pad = 18;

	nglInitQuad(&quad);
	nglSetQuadRect(&quad, cfi(menu_x_start), cfi(menu_y_start), cfi(menu_x_start + debug_width + menu_x_pad), cfi(menu_y_start + debug_height + menu_y_pad));
	nglSetQuadColor(&quad, 0x60000000);
	nglSetQuadZ(&quad, cfi(0.5f));
	nglListAddQuad(&quad);


	int white_color = nglColor(0x80, 0x80, 0x80, 0x80);
	int yellow_color = nglColor(0x80, 0x80, 0, 0x80);
	int green_color = nglColor(0, 0x80, 0, 0x80);
	int pink_color = nglColor(0x80, 0, 0x80, 0x80);


	int render_height = menu_y_start;
	render_height += 12;
	int render_x = menu_x_start;
	render_x += 8;
	//nglListAddString(*nglSysFont, current_menu->title, green_color, cfi(render_x * 1.0f), cfi(render_height * 1.0f), cfi(0.2f), cfi(1.f), cfi(1.f));
	//nglListAddString(*nglSysFont, current_menu->title, green_color, cfi(10.f), cfi(10.f), cfi(-0.2f), cfi(1.f), cfi(1.f));
	nglListAddString(*nglSysFont, current_menu->title, green_color, cfi(render_x*1.0f), cfi(render_height * 1.0f), cfi(0.f), cfi(1.f), cfi(1.f));
	render_height += getStringHeight(current_menu->title);


	if (current_menu->window_start) {
		nglListAddString(*nglSysFont, UP_ARROW, pink_color, cfi(render_x * 1.0f), cfi(render_height * 1.0f), cfi(0.2f), cfi(1.f), cfi(1.f));
	}
	render_height += getStringHeight(UP_ARROW);



	for (int i = 0; i < total_elements_page; i++) {

		int current_color = current_menu->cur_index == i ? yellow_color : white_color;

		debug_menu_entry* entry = &current_menu->entries[current_menu->window_start + i];
		char* cur = getRealText(entry, text_buffer);
		nglListAddString(*nglSysFont, cur, current_color, cfi(render_x*1.0f), cfi(render_height*1.0f), cfi(0.2f), cfi(1.f), cfi(1.f));
		render_height += getStringHeight(cur);
	}

	if (needs_down_arrow) {
		nglListAddString(*nglSysFont, DOWN_ARROW, pink_color, cfi(render_x*1.0f), cfi(render_height*1.0f), cfi(0.2f), cfi(1.f), cfi(1.f));
		render_height += getStringHeight(DOWN_ARROW);
	}
}

void debug_menu_render(){

	if (menu_enabled){
		render_current_debug_menu();
		//my_printf("CONA %08X", current_menu);
	}
}

void close_debug(){
	menu_enabled = 0;
	game_unpause(*g_game_ptr, 1);
}

void handle_start_menu_entry(debug_menu_entry *entry, custom_key_type k){
	//my_print("SEXOOOO");
}

void init_start_menu(){
	start_menu = create_menu("Debug Menu", close_debug, handle_start_menu_entry, 2);

	debug_menu_entry warp_entry = {"Warp", NORMAL, 0};
	debug_menu_entry options_entry = {"Options", NORMAL, 0};

	add_debug_menu_entry(start_menu, &warp_entry);
	add_debug_menu_entry(start_menu, &options_entry);
}

void init_menus(){
	init_start_menu();
	menus_inited = 1;
	my_print("menus have inited");
}

int pause_menu_key_handler(int *a1, int pad, int dualshock){

	my_printf("pad %s %d", key_names[pad], sizeof(long long));
	if(!dualshock){

		if(pad == SELECT){

			if(!menus_inited)
				init_menus();

			menu_enabled = !menu_enabled;
			my_print("PIROCA menu_enabled");

			if(!menu_enabled){
				close_debug();
				return pause_menu_key_handler_orig(a1, 0, 0);
			}
			else{
				current_menu = start_menu;
				return pause_menu_key_handler_orig(a1, START, dualshock);
			}

		}
	}
	return pause_menu_key_handler_orig(a1, pad, dualshock);
}

int aqui_menu(int a1, int a2, int a3){
	int res = a1+2*a2+3*a3;
	return res;
}

int sexo(int a1, int a2, int a3){
	return a1+2*a2;
}
