/********************
Arduino generic menu system
Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

    prompt: class representing a text and an associated function pointer
      menu: prompt derived holding a list of prompts (options and submenus)
   menuOut: print derived with special virtual functions needed for menu output, derive this to have the menu in other devices
 menuPrint: menuOut implementation for generic print (as Serial)
   menuLCD: menuOut implementation for standard LiquidCrystal LCD

the menu system will read provided stream for input, it works for Serial
for encoders, joysticks, keyboards or touch a stream must be made out of them
*/
#ifndef RSITE_ARDUINOP_MENU_SYSTEM
  #define RSITE_ARDUINOP_MENU_SYSTEM
  
	#include <Stream.h>
	
  class prompt;
  class menu;
  class menuOut;
  
  #define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
  #define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
  #define CONCATENATE2(arg1, arg2)  arg1##arg2
  
  #define FOR_EACH_1(what, x, ...) what(x)
  #define FOR_EACH_2(what, x, ...)\
    what(x)\
    FOR_EACH_1(what,  __VA_ARGS__)
  #define FOR_EACH_3(what, x, ...)\
    what(x)\
    FOR_EACH_2(what, __VA_ARGS__)
  #define FOR_EACH_4(what, x, ...)\
    what(x)\
    FOR_EACH_3(what,  __VA_ARGS__)
  #define FOR_EACH_5(what, x, ...)\
    what(x)\
   FOR_EACH_4(what,  __VA_ARGS__)
  #define FOR_EACH_6(what, x, ...)\
    what(x)\
    FOR_EACH_5(what,  __VA_ARGS__)
  #define FOR_EACH_7(what, x, ...)\
    what(x)\
    FOR_EACH_6(what,  __VA_ARGS__)
  #define FOR_EACH_8(what, x, ...)\
    what(x)\
    FOR_EACH_7(what,  __VA_ARGS__)
  #define FOR_EACH_9(what, x, ...)\
    what(x)\
    FOR_EACH_8(what,  __VA_ARGS__)
  #define FOR_EACH_10(what, x, ...)\
    what(x)\
    FOR_EACH_9(what,  __VA_ARGS__)
  #define FOR_EACH_11(what, x, ...)\
    what(x)\
    FOR_EACH_10(what,  __VA_ARGS__)
  #define FOR_EACH_12(what, x, ...)\
    what(x)\
    FOR_EACH_11(what,  __VA_ARGS__)
  #define FOR_EACH_13(what, x, ...)\
    what(x)\
    FOR_EACH_12(what,  __VA_ARGS__)
  #define FOR_EACH_14(what, x, ...)\
    what(x)\
    FOR_EACH_13(what,  __VA_ARGS__)
  #define FOR_EACH_15(what, x, ...)\
    what(x)\
    FOR_EACH_14(what,  __VA_ARGS__)
  #define FOR_EACH_16(what, x, ...)\
    what(x)\
    FOR_EACH_15(what,  __VA_ARGS__)
  
  #define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
  #define FOR_EACH_NARG_(...) FOR_EACH_ARG_N(__VA_ARGS__)
  #define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
  #define FOR_EACH_RSEQ_N() 16,15,14,13,12,11,10,9,8, 7, 6, 5, 4, 3, 2, 1, 0
  
  #define FOR_EACH_(N, what, x, ...) CONCATENATE(FOR_EACH_, N)(what, x, __VA_ARGS__)
  #define FOR_EACH(what, x, ...) FOR_EACH_(FOR_EACH_NARG(x, __VA_ARGS__), what, x, __VA_ARGS__)
  
  #define DECL(x) DECL_##x
  #define DEF(x) DEF_##x,
  
  #define MENU(id,text,...)\
    FOR_EACH(DECL,__VA_ARGS__)\
    prompt* const id##_data[]={\
      FOR_EACH(DEF,__VA_ARGS__)\
    };\
    menu id (text,sizeof(id##_data)/sizeof(prompt*),id##_data);
  
  #define OP(...) OP_(__COUNTER__,__VA_ARGS__)
  #define DECL_OP_(cnt,...) prompt op##cnt(__VA_ARGS__);
  #define DECL_SUBMENU(id)
  #define DEF_OP_(cnt,...) &op##cnt
  #define DEF_SUBMENU(id) &id
  
  /////////////////////////////////////////////////////////
  // menu pure virtual output device, use derived
  // this base class represents the output device either derived to serial, LCD or other
  class menuOut {
    public:
    int top;//top for this device
    //device size
    int maxX;
    int maxY;
    //device resolution
    int resX;
    int resY;
    enum styles {
      enumerated, //print numbers or references for options, adequated keyboard or file input
      cursor,//print a cursor at selected option, tracking device (encoder)
      point,//pointing device (not implemented)
    } style;
    //menuOut(menuOut::styles style=menuOut::enumerated):maxX(0),maxY(0),style(style),top(0) {}
    menuOut(menuOut::styles style=menuOut::enumerated,int x=0x7F,int y=0x7F,int resX=1,int resY=1):maxX(x),maxY(y),style(style),top(0),resX(resX),resY(resY) {}
    virtual void clear()=0;
    virtual void setCursor(int x,int y)=0;
    virtual void print(char ch)=0;
    virtual void print(const char *text)=0;
    virtual void println(const char *text)=0;
    virtual void print(int)=0;
    virtual void println(int)=0;
    virtual void print(prompt &o,bool selected);
  };
  
  ////////////////////////////////////////////////////////////////////
  // menu structure
  
  //an associated function to be called on menu selection
  //associated functions can accept no parameters 
  // or accept some of the standard parameters preserving the order
  // standard parameters (for this menu lib) are:
  // prompt -> the associated prompt object that trigged the call
  // menuOut -> the device we were using to display the menu.. you migh want to draw on it
  // Stream -> the input stream we are using to play the menu, can be a serial or an encoder or keyboard stream
	class promptAction {
	public:
		typedef void (*callback)(prompt &p, menuOut &o, Stream &i);//callback fynction type
		callback hFn;//the hooked callback function
		//cast no arguments or partial arguments to be accepted as promptActions
		inline promptAction() {}
		inline promptAction(void (*f)()):hFn((callback)f) {}
		inline promptAction(void (*f)(prompt&)):hFn((callback)f) {}
		inline promptAction(void (*f)(prompt&,menuOut&)):hFn((callback)f) {}
		inline promptAction(callback f):hFn(f) {}
		//use this objects as a function (replacing functions)
		inline void operator()(prompt &p, menuOut &o, Stream &i) {hFn(p,o,i	);}
	};

	//holds a menu option
	//a menu is also a prompt so we can have sub-menus
  class prompt {
    public:
    const char *text;
    promptAction action;
    bool enabled;
    prompt(const char * text):text(text),enabled(true) {}
    prompt(const char * text,promptAction action)
    	:text(text),action(action),enabled(true) {}
    virtual size_t printTo(Print& p) {p.print(text);return strlen(text);}
    virtual void activate(menuOut& p,Stream&c) {action(*this,p,c);}
  };
  
  //a menu or sub-menu
  class menu:public prompt {
    public:
    static const char *exit;//text used for exit option
    static char enabledCursor;//character to be used as navigation cursor
    static char disabledCursor;//to be used when navigating over disabled options
    static prompt exitOption;//option tro append to every menu allowing exit when no escape button/key is available
    const int sz;
    int sel;//selection
    prompt* const* data;
    menu(const char * text,int sz,prompt* const data[]):prompt(text),sz(sz),data(data),sel(0) {}
    
    int menuKeys(menuOut &p,Stream& c);
    void printMenu(menuOut& p);
    
    void activate(menuOut& p,Stream& c);
  };

#endif