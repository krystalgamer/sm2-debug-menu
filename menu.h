#define DWORD int
#define MAX_CHARS_SAFE 63
#define MAX_CHARS MAX_CHARS_SAFE+1
#define EXTEND_NEW_ENTRIES 20
#define MAX_ELEMENTS_PAGE 18

typedef enum{
	NORMAL,
	BOOLEAN_E,
	CUSTOM
}debug_menu_entry_type;


typedef enum {
	/*
	LEFT,
	RIGHT,
	*/
	ENTER
}custom_key_type;


struct _debug_menu_entry;
typedef char* (*custom_string_generator_ptr)(struct _debug_menu_entry* entry);

typedef struct _debug_menu_entry {

	char text[MAX_CHARS];
	debug_menu_entry_type entry_type;
	void* data;
	void* data1;
	custom_string_generator_ptr custom_string_generator;
}debug_menu_entry;

typedef void (*menu_handler_function)(debug_menu_entry*, custom_key_type key_type);
typedef void (*go_back_function)();

typedef struct {
	char title[MAX_CHARS];
	DWORD capacity;
	DWORD used_slots;
	DWORD window_start;
	DWORD cur_index;
	go_back_function go_back;
	menu_handler_function handler;
	debug_menu_entry* entries;
}debug_menu;

