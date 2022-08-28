// vs6_framework: a cross-platform general purpose psychophysical experiment creation tool
// by: louis
// on:19jan2010
// last modify: 19jan2010
// version: 6.0
// dependency: sdl(vs6), sdl-image, cairo and pango

#ifdef _WIN32  
#include "windows.h"
#endif

#include "SDL.h"
#include "SDL_image.h"

#include "cairo/cairo-svg.h"
#include <pango/pangocairo.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include "stdio.h"
#include "stdarg.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include "mt19937ar.h"
using namespace std;
#define npos string::npos

#undef max
#undef min
#define _max(a,b) (((a)>(b))?(a):(b))
#define _min(a,b) (((a)<(b))?(a):(b))

template<typename t1> t1 _max1(t1 a, t1 b){ return _max(a,b); }
template<typename t1> t1 _min1(t1 a, t1 b){ return _min(a,b); }

double pi=2*asin(1.0);
#define deg2rad(x) ((x)*pi/180)
#define rad2deg(x) ((x)*180/pi)
#define py(a,b) (sqrt(pow((double)(a),2)+pow((double)(b),2)))
#define neg(a) ((a+1)%2)

//global variables
SDL_Color bgcolor={0,0,0,0};
int sw;
int sh;
int cx;
int cy;

//devices pointers -- this is supposed to be transparent to users
SDL_Surface* bb=0;

//cairo pointers
cairo_surface_t* sshdc=0;
cairo_t* ss=0; //soft surface

//device status
bool sdlstarted=false;
bool videostarted=false;
bool audiostarted=false;

//functions
void _cleandevices();
void _dielog(string logstr){
	cout << "Error:\t" << logstr << endl;
	_cleandevices();
	exit(0);
	return;
}
void _infolog(string logstr){
	cout << "Info:\t" << logstr << endl;
	return;
}

//hard functions
//sound
static Uint8 *audio_chunk;
static Uint32 audio_len;
static Uint8 *audio_pos;
void fill_audio(void *udata, Uint8 *stream, int len){
	//stop when nothing left
	if (audio_len==0) return;
	//mix all left
	len=_min((int)audio_len,len);
	SDL_MixAudio(stream,audio_pos,len,SDL_MIX_MAXVOLUME);
	audio_pos+=len;
	audio_len-=len;
}

//event
struct Button{
	int type;
	int key;
	Button(){type=-1;key=-1;}
	Button(int type_i,int key_i){
		type=type_i;
		key=key_i;
	}
};
bool operator==(const Button& a,const Button& b){
	return (a.type==b.type && a.key==b.key);
}
bool operator!=(const Button& a,const Button& b){
	return !(a==b);
}
bool operator<(const Button& a,const Button& b){
	if (a.type==b.type) return (a.key<b.key);
	else return (a.type<b.key);
}
struct Event{
	int time;
	Button button;
	string keyname;
	int x,y;
	Event(){time=-1;}
	bool empty(){
		return (time<0);
	}
};
list<Event> eventqueue;
int eventfilter(const SDL_Event *eventi){
	Event vsei;
	int isupper;
	switch(eventi->type){
	case SDL_KEYDOWN:
//	case SDL_KEYUP:
		vsei.time=eventi->key.timestamp;
		vsei.button=Button((int)eventi->type,(int)eventi->key.keysym.sym);
		vsei.keyname=SDL_GetKeyName(eventi->key.keysym.sym);
		isupper=eventi->key.keysym.mod & KMOD_CAPS;
		if (eventi->key.keysym.mod & KMOD_SHIFT) isupper=!isupper;
		if (isupper) transform(vsei.keyname.begin(),vsei.keyname.end(),vsei.keyname.begin(),(int(*)(int))toupper);
		vsei.x=vsei.y=0;
		break;
	case SDL_MOUSEBUTTONDOWN:
//	case SDL_MOUSEBUTTONUP:
		vsei.time=eventi->button.timestamp;
		vsei.button=Button((int)eventi->type,(int)eventi->button.button);
		vsei.keyname="";
		vsei.x=eventi->button.x;
		vsei.y=eventi->button.y;
		break;
	default:
		return 1;
	}
	eventqueue.push_back(vsei);
	while (eventqueue.size()>10000) eventqueue.pop_front();//avoid eventqueue grow too big
	return 0;
}

//devices init
void _createdevices(int width, int height){
	//initiate SDL
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)!=0) _dielog("SDL initiation failure");
	sdlstarted=true;

	sw=width;
	sh=height;
	cx=width/2;
	cy=height/2;

	//check, determine and display device mode
	char vn[20];
	SDL_VideoDriverName(vn,20);
	_infolog(string("Using ")+string(vn)+string(" video driver"));
	int scrcolordepth=SDL_VideoModeOK(sw,sh,SDL_GetVideoInfo()->vfmt->BitsPerPixel, SDL_ANYFORMAT|SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
	char swstr[5];
	char shstr[5];
	char sdstr[5];
	sprintf(swstr,"%d",sw);
	sprintf(shstr,"%d",sh);
	sprintf(sdstr,"%d",scrcolordepth);
	if (scrcolordepth==0) _dielog("Required device mode not supported");
	if (scrcolordepth<24) _dielog("Video must support at least 24 bpp");
	_infolog(string("Using video mode ")+swstr+string("x")+shstr+string(" at ")+sdstr+string("bpp"));
	
	//initialize device
	SDL_ShowCursor(SDL_DISABLE);
	bb=SDL_SetVideoMode(sw,sh,scrcolordepth, SDL_ANYFORMAT|SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);

	if (bb==0 || (~bb->flags & (SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN))){
		_dielog("Required video mode not supported");
		return;
	}
	videostarted=true;

	//clear both sides
	SDL_FillRect(bb,&bb->clip_rect,SDL_MapRGB(bb->format,bgcolor.r,bgcolor.g,bgcolor.b));
	SDL_Flip(bb);
	SDL_FillRect(bb,&bb->clip_rect,SDL_MapRGB(bb->format,bgcolor.r,bgcolor.g,bgcolor.b));

	//initialize audio
	SDL_AudioSpec audiospec;
	audiospec.freq=22050;
	audiospec.format=AUDIO_S8;
	audiospec.channels=1;
	audiospec.samples=1024;
	audiospec.callback=fill_audio;
	audiospec.userdata=NULL;
	if (SDL_OpenAudio(&audiospec,NULL)<0) _dielog("error opening audio");
	audiostarted=true;

	//initialize event
	SDL_SetEventFilter(eventfilter);

	return;
}

//close devices
void _cleandevices(){
	if (audiostarted){
		audiostarted=false;
		SDL_CloseAudio();
	}
	if (videostarted){
		videostarted=false;
		SDL_FreeSurface(bb);
		SDL_ShowCursor(SDL_ENABLE);
	}
	if (sdlstarted){
		sdlstarted=false;
		SDL_Quit();
	}
	return;
}
