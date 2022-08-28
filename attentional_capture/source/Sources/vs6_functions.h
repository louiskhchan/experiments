// vs6_framework: a cross-platform general purpose psychophysical experiment creation tool
// by: louis
// on:19jan2010
// last modify: 19jan2010
// version: 6.0
// dependency: sdl(vs6), sdl-image, cairo and pango

namespace vsutil{
	//extended vector class
	template<typename T> class ev:public vector<T>{
	public:
		//constructors
		ev():vector<T>(){};
		ev(const ev& _Right):vector<T>(_Right){}
		ev(size_t _Count):vector<T>(_Count){}
		ev(size_t _Count,const T& _Val):vector<T>(_Count,_Val){}
		template<class InputIterator> ev(InputIterator _First,InputIterator _Last):vector<T>(_First,_Last){}

		//functions
		ev<T> operator-(T val){
			ev<T> ret=*this;
			ret.push_back(val);
			return ret;
		}
		ev<T> operator-(const char* val){
			ev<T> ret=*this;
			ret.push_back(val);
			return ret;
		}
		ev<T>& operator+(T val){
			(*this).push_back(val);
			return *this;
		}
		ev<T>& operator+(const char* val){
			(*this).push_back(val);
			return *this;
		}
		void print_r(){
			for (size_t i=0;i<(*this).size();i++) cout << (*this)[i] << endl;
			return;
		}
		//simple shuffle function with mapping returned
		map<size_t,size_t> shuffle(){
			map<unsigned int,size_t> tmp_map;
			ev<T> tmp_vector=(*this);
			for (size_t i=0;i<(*this).size();i++) tmp_map[genrand_int32()]=i;
			size_t tmp_map_i=0;
			map<size_t,size_t> shufflemap;
			for (map<unsigned int,size_t>::iterator iter=tmp_map.begin();iter!=tmp_map.end();iter++){
				(*this)[tmp_map_i]=tmp_vector[iter->second];
				shufflemap[iter->second]=tmp_map_i;
				tmp_map_i++;
			}
			return shufflemap;
		}
		//shuffle with offset and range support, with mapping returned
		map<size_t,size_t> shuffle(size_t offset, size_t size){
			map<unsigned int,size_t> tmp_map;
			ev<T> tmp_vector=(*this);
			for (size_t i=offset;i<(offset+size);i++) tmp_map[genrand_int32()]=i;
			size_t tmp_map_i=offset;
			map<size_t,size_t> shufflemap;
			for (map<unsigned int,size_t>::iterator iter=tmp_map.begin();iter!=tmp_map.end();iter++){
				(*this)[tmp_map_i]=tmp_vector[iter->second];
				shufflemap[iter->second]=tmp_map_i;
				tmp_map_i++;
			}
			return shufflemap;
		}
		//randomly pack an item out of a ev
		T rand_item(){
			return (*this)[genrand_int32()%(*this).size()];
		}
		//pop_back while removing the back item for ev
		T pop_back(){
			T tmpout=(*this).back();
			(*this).vector<T>::pop_back();
			return tmpout;
		}
		//return sum of numeric ev
		T sum(){
			if ((*this).size()<1) return 0;
			T sumout=0;
			for (size_t i=0;i<(*this).size();i++) sumout+=(*this)[i];
			return sumout;
		}
		//return min of numeric ev
		T min(){
			if ((*this).size()<1) return 0;
			T minout=(*this)[0];
			for (size_t i=1;i<(*this).size();i++) minout=_min(minout,(*this)[i]);
			return minout;
		}
		//return max of numeric ev
		T max(){
			if ((*this).size()<1) return 0;
			T maxout=(*this)[0];
			for (size_t i=1;i<(*this).size();i++) maxout=_max(maxout,(*this)[i]);
			return maxout;
		}
		//end of easy vector
	};

	template<typename T> class es:public set<T>{
	public:
		//constructors
		es():set<T>(){};
		es(const es& _Right):set<T>(_Right){}
		template<class InputIterator> es(InputIterator _First,InputIterator _Last):set<T>(_First,_Last){}

		//functions
		es<T> operator-(T val){
			es<T> ret=*this;
			ret.insert(val);
			return ret;
		}
		es<T> operator-(const char* val){
			es<T> ret=*this;
			ret.insert(val);
			return ret;
		}
		es<T>& operator+(T val){
			(*this).insert(val);
			return *this;
		}
		es<T>& operator+(const char* val){
			(*this).insert(val);
			return *this;
		}
		bool exist(T& val){
			return ((*this).find(val)!=(*this).end());
		}
		//end of easy set
	};
	//shortcuts for creating easy vector and set
	ev<int> evi(){ return ev<int>();}
	ev<double> evd(){ return ev<double>();}
	ev<string> evs(){ return ev<string>();}
	ev<Button> evb(){ return ev<Button>();}
	ev<ev<int>* > evevi(){ return ev<ev<int>* >();}
	ev<ev<string>* > evevs(){ return ev<ev<string>* >();}
	es<int> esi(){ return es<int>();}
	es<string> ess(){ return es<string>();}
	es<Button> esb(){ return es<Button>();}

	//color struct
	struct color{
		double r,g,b;
		color(){r=g=b=0;}
		color(double ri,double gi,double bi){r=ri;g=gi;b=bi;}
	};

	//staircase struct
	struct Staircase{
		int nup,ndown;
		double eratio;
		double level;
		int intensity;
		double sumlevel;
		int sumtrial;
		int ncorrect,nwrong;
		int nrev,nstartrev;
		bool goingdown;
		bool finished;
		void start(int nup_,int ndown_,double eratio_,double startlevel_,int nstartrev_=6){
			nup=nup_;
			ndown=ndown_;
			eratio=eratio_;
			level=startlevel_;
			nstartrev=nstartrev_;
			intensity=(int)(0.5+exp(eratio*level));
			sumlevel=0;
			sumtrial=ncorrect=nwrong=nrev=0;
			goingdown=true;
			finished=false;
			return;
		}
		bool finish(){
			if (sumtrial<=0) return false;
			level=sumlevel/sumtrial;
			intensity=(int)(0.5+exp(eratio*level));
			finished=true;
			return true;
		}
		void next(bool correct){
			if (finished) return;
			if (nrev>=nstartrev){
				sumlevel+=log((double)intensity)/eratio;
				sumtrial++;
			}
			if (correct){
				ncorrect++;
				if (ncorrect>=((nrev>0)?ndown:1)){
					if (!goingdown) nrev++;
					goingdown=true;
					ncorrect=nwrong=0;
					level-=1;
					intensity=(int)(0.5+exp(eratio*level));
				}
			} else {
				nwrong++;
				if (nwrong>=nup){
					if (goingdown) nrev++;
					goingdown=false;
					ncorrect=nwrong=0;
					level+=1;
					intensity=(int)(0.5+exp(eratio*level));
				}
			}
			return;
		}
		//constructor
		Staircase(){ finished=true;}
		Staircase(int nup_,int ndown_,double eratio_,double startlevel_,int nstartrev_=6){
			(*this).start(nup_,ndown_,eratio_,startlevel_,nstartrev_);
		}
	};

	//check if a file exists
	bool fileexists(string fn){
		struct stat buf;
		return (0==stat(fn.c_str(),&buf));
	}

	//convert an integer to a string
	string itos(int i,int width=-1){
		char wstr[5]="";
		if (width>=0) sprintf(wstr,"%d",width);
		char tmpstr[30];
		string querystr=string("%")+((width>=0)?string("0"):string(""))+string(wstr)+string("d");
		sprintf(tmpstr,querystr.c_str(),i);
		return string(tmpstr);
	}

	//convert a double to a string
	string dtos(double i){
		char tmpstr[30];
		sprintf(tmpstr,string("%f").c_str(),i);
		return string(tmpstr);
	}

	//convert a character to a string
	string ctos(char inputchar){
		char tmpstr[2];
		tmpstr[0]=inputchar;
		tmpstr[1]='\0';
		return string(tmpstr);
	}

	//simple trim function
	string trim(string str){
		size_t startpos,endpos;
		startpos=str.find_first_not_of(" \t");
		if (startpos==npos) return string("");
		endpos=str.find_last_not_of(" \t");
		str=str.substr(startpos,endpos-startpos+1);
		return str;
	}

	//decompose a string into a ev
	ev<string> explode(string delim,string str){
		ev<string> output;
		char* instr=new char[str.length()+1];
		strcpy(instr,str.c_str());
		char* token=strtok(instr,delim.c_str());
		while(token!=NULL){
			output.push_back(token);
			token=strtok(NULL,delim.c_str());
		}
		return output;
	}

	//compose a string from a ev
	string implode(string delim,ev<string> tokens){
		string outstr;
		if (tokens.size()>0) outstr.append(tokens[0]);
		for(size_t i=1;i<tokens.size();i++){
			outstr.append(delim);
			outstr.append(tokens[i]);
		}
		return outstr;
	}

	//temp file management
	string createtemp(string suffix="."){
		int i=0;
		while (fileexists("_vs6_tmp_"+itos(i)+suffix)) i++;
		return ("_vs6_tmp_"+itos(i)+suffix);
	}
	void freetemp(string fn){
		if (fileexists(fn)) unlink(fn.c_str());
		return;
	}
	//end of vsutil
}

namespace vsstructs{
	using namespace vsutil;
	//positioning structs
	struct point{
		double x,y;
		point(){ x=0; y=0; }
		point(const pair<int,int>& pxy){
			x=pxy.first; y=pxy.second;
		}
		point(double xi, double yi){
			x=xi;
			y=yi;
		}
		double dist_from_point(point center, point pt){
			return py(pt.x-(x+center.x),pt.y-(y+center.y));
		}
		double dist_from_point(point pt){
			return py(pt.x-x,pt.y-y);
		}
	};
	point operator+(point p1, point p2){
		point out;
		out.x=p1.x+p2.x;
		out.y=p1.y+p2.y;
		return out;
	}
	struct line{
		double a,b,c;
		line(point p1,point p2){
			a=p2.y-p1.y;
			b=p1.x-p2.x;
			c=-a*p1.x-b*p1.y;
		}
		double dist_from_point(point center, point pt){
			return (abs(a*(pt.x-center.x)+b*(pt.y-center.y)+c)/py(a,b));
		}
		bool sign_from_point(point center, point pt){
			return ((a*(pt.x-center.x)+b*(pt.y-center.y)+c)>0);
		}
	};
	struct shape{
		double ori;
		double w,h;
		double radius;
		double boundw,boundh;
		bool isrect;
		ev<point> points;
		ev<line> lines;
		shape(){}
		void setrect(double orii, double wi, double hi){
			isrect=true;
			ori=orii; w=wi; h=hi;
			//calculate the coordinate of the four points, and the four lines
			for (int i=0;i<2;i++) for (int j=-1;j<2;j+=2) points.push_back(point(h/2*cos(ori+i*pi)+w/2*cos(ori+i*pi+j*pi/2),h/2*sin(ori+i*pi)+w/2*sin(ori+i*pi+j*pi/2)));
			for (int i=0;i<4;i++) lines.push_back(line(points[i],points[(i+1)%4]));
			boundw=h*abs(cos(ori))+w*abs(cos(ori+pi/2));
			boundh=h*abs(sin(ori))+w*abs(sin(ori+pi/2));
			radius=py(w,h)/2;
			return;
		}
		void setcircle(double radiusi){
			isrect=false;
			radius=radiusi;
			boundw=radius*2;
			boundh=radius*2;
			return;
		}
		shape(double orii, double wi, double hi){setrect(orii,wi,hi);}
		shape(double radiusi){setcircle(radiusi);}
	};
	double shape_dist(shape shape1, point center1, shape shape2, point center2){
		ev<shape> shapes;
		ev<point> centers;
		shapes.push_back(shape1); shapes.push_back(shape2);
		centers.push_back(center1); centers.push_back(center2);

		double mindist=sw+sh;
		if (shape1.isrect && shape2.isrect){
			for (int r1=0;r1<2;r1++){
				int r2=1-r1;
				//first registers all the point-line-center relations
				ev<ev<bool> > ssc;
				for (int i=0;i<4;i++){
					ev<bool> ssci;
					bool rect1centersign=shapes[r1].lines[i].sign_from_point(centers[r1],centers[r1]);
					for (int j=0;j<4;j++) ssci.push_back(shapes[r1].lines[i].sign_from_point(centers[r1],centers[r2]+shapes[r2].points[j])==rect1centersign);
					ssc.push_back(ssci);
				}
				//scan for all-four-lines
				ev<int> a4lines;
				for (int i=0;i<4;i++){
					int count=0;
					for (int j=0;j<4;j++) if (!ssc[i][j]) count++;
					if (count==4) a4lines.push_back(i);
				}
				//calculate distance
				//if (a4lines.size()>2) dielog("shape_dist: rect dist calc error"); //impossible unless rect edge is curved
				if (a4lines.size()==2){ //this should be the most common case
					int closepoint=_max(a4lines[0],a4lines[1]);
					if (closepoint==3 && _min(a4lines[0],a4lines[1])==0) closepoint=0;
					for (int j=0;j<4;j++) mindist=_min1(mindist,shapes[r1].points[closepoint].dist_from_point(centers[r1],centers[r2]+shapes[r2].points[j]));
				}
				else if (a4lines.size()==1){ //some points are "shined" only by one edge
					for (int j=0;j<4;j++){
						int i1=(a4lines[0]+3)%4;
						int i2=(a4lines[0]+1)%4;
						if (!ssc[i1][j]){ //near the left corner
							int closepoint=_max(a4lines[0],i1);
							if (closepoint==3 && _min(a4lines[0],i1)==0) closepoint=0;
							mindist=_min1(mindist,shapes[r1].points[closepoint].dist_from_point(centers[r1],centers[r2]+shapes[r2].points[j]));
						} else if (!ssc[i2][j]){ //near the right corner
							int closepoint=_max(a4lines[0],i2);
							if (closepoint==3 && _min(a4lines[0],i2)==0) closepoint=0;
							mindist=_min1(mindist,shapes[r1].points[closepoint].dist_from_point(centers[r1],centers[r2]+shapes[r2].points[j]));
						} else mindist=_min1(mindist,shapes[r1].lines[a4lines[0]].dist_from_point(centers[r1],centers[r2]+shapes[r2].points[j])); //only with one edge
					}
				} //else is a4lines.size()==0. either the a4line is on the other rect or the two intersect.
			}
		} else if (shape1.isrect || shape2.isrect){
			int r1,c1;
			if (shape1.isrect) r1=0; else r1=1;
			c1=1-r1;
			//first registers all the line-center relations
			ev<bool> ssc;
			for (int i=0;i<4;i++) ssc.push_back(shapes[r1].lines[i].sign_from_point(centers[r1],centers[c1])==shapes[r1].lines[i].sign_from_point(centers[r1],centers[r1]));
			//scan for outside lines
			ev<int> a4lines;
			for (int i=0;i<4;i++) if (!ssc[i]) a4lines.push_back(i);
			//calculate distance
			//if (a4lines.size()>2) dielog("shape_dist: rect dist calc error"); //impossible unless rect edge is curved
			if (a4lines.size()==2){ //this should be the most common case
				int closepoint=_max(a4lines[0],a4lines[1]);
				if (closepoint==3 && _min(a4lines[0],a4lines[1])==0) closepoint=0;
				mindist=shapes[r1].points[closepoint].dist_from_point(centers[r1],centers[c1])-shapes[c1].radius;
			}
			else if (a4lines.size()==1){ //circle center is "shined" only by one edge
				mindist=shapes[r1].lines[a4lines[0]].dist_from_point(centers[r1],centers[c1])-shapes[c1].radius; //only with one edge
			} //else is a4lines.size()==0. either the a4line is on the other shape or the two intersect.
		} else {
			//case for two circle
			mindist=centers[0].dist_from_point(centers[1])-shapes[0].radius-shapes[1].radius;
		}

		if (mindist<0) mindist=0;
		if (mindist==(sw+sh)) mindist=0; //no a4line => intersecting rectangles.
		return mindist;
	}

//end of vsstructs
}

// vs6 framework functions
using namespace vsutil;
using namespace vsstructs;
namespace vs{
	//logging functions
	void freeallbuf();
	void freeallimg();
	void closedisplay();
	void dielog(string logstr){
		cout << "Error:\t" << logstr << endl;
		freeallbuf();
		freeallimg();
		closedisplay();
		exit(0);
		return;
	}
	void infolog(string logstr){
		cout << "Info:\t" << logstr << endl;
		return;
	}

	//basic types
	//Buffer structs
	//Buffer -- fast for blitting
	struct Buffer{
		SDL_Surface* hdc;
		unsigned int width, height;
		Buffer(){
			hdc=NULL;
			width=0;
			height=0;
		}
		Buffer(string srcbmp){
			SDL_Surface* srchdc=IMG_Load(srcbmp.c_str());
			if (srchdc==NULL) dielog("Buffer: cannot load buffer "+srcbmp);
			hdc=SDL_CreateRGBSurface(SDL_SWSURFACE,srchdc->w,srchdc->h,bb->format->BitsPerPixel,bb->format->Rmask,bb->format->Gmask,bb->format->Bmask,bb->format->Amask);
			if (!hdc) dielog("Buffer: fail to create surface for buffer");
			SDL_BlitSurface(srchdc,NULL,hdc,NULL);
			SDL_FreeSurface(srchdc);		
			width=hdc->w;
			height=hdc->h;
		}
		Buffer(SDL_Surface* srchdc){
			hdc=SDL_CreateRGBSurface(SDL_SWSURFACE,srchdc->w,srchdc->h,bb->format->BitsPerPixel,bb->format->Rmask,bb->format->Gmask,bb->format->Bmask,bb->format->Amask);
			if (!hdc) dielog("Buffer: fail to create surface from surface");
			SDL_BlitSurface(srchdc,NULL,hdc,NULL);
			SDL_FreeSurface(srchdc);		
			width=hdc->w;
			height=hdc->h;
		}
		Buffer(unsigned int wi,unsigned int hi){
			hdc=SDL_CreateRGBSurface(SDL_SWSURFACE,wi,hi,bb->format->BitsPerPixel,bb->format->Rmask,bb->format->Gmask,bb->format->Bmask,bb->format->Amask);
			if (!hdc) dielog("Buffer: fail to create surface");
			width=hdc->w;
			height=hdc->h;
		}
		void clean(){
			if (hdc!=NULL) SDL_FreeSurface(hdc);
			hdc=NULL;
		}
	};
	//Image -- good for cairo drawing
	struct Image{ 
		shape rect;
		int diameter;
		cairo_surface_t* hdc;
		string fn;
		Image(){
			hdc=0;
			diameter=0;
		}
		Image(int diameteri){
			diameter=diameteri+diameteri%2;
			fn=createtemp();
			hdc=cairo_svg_surface_create(fn.c_str(),diameter,diameter);
		}
		Image(int wi,int hi){
			diameter=0;
			fn=createtemp();
			hdc=cairo_svg_surface_create(fn.c_str(),wi,hi);
		}
		void clean(){
			if (hdc!=0) cairo_surface_destroy(hdc);
			hdc=0;
			if (!fn.empty()) freetemp(fn);
			fn.clear();
			return;
		}
	};

	//global vars (config)
	int imgmargin=10;
	bool imgdebugmode=false;

	//global vars (status)
	bool devicestarted=false;
	bool ssstarted=false;

	//global containers
	Image ssimage;
	map<string,Buffer> allBuffers;
	map<string,Image> allImages;
	ev<ofstream*> allFiles;

	//event functions
	void clearevent(int starttime=SDL_GetTicks()){
		//clear events that happens before waiting
		SDL_PumpEvents();
		if (starttime>0) for (list<Event>::iterator iter=eventqueue.begin();iter!=eventqueue.end();) if (iter->time<starttime) iter=eventqueue.erase(iter); else iter++;
		return;
	}
	list<Event>::iterator _peepevent(es<Button>& acceptbuttons){
		SDL_PumpEvents();
		for (list<Event>::iterator iter=eventqueue.begin();iter!=eventqueue.end();iter++){
			if (iter->button==Button(SDL_KEYDOWN,SDLK_F10)) dielog("getevent: F10 Quit");
			if (acceptbuttons.empty() || acceptbuttons.exist(iter->button)){
				return iter;
			}
		}
		return eventqueue.end();
	}
	Event peepevent(es<Button> acceptbuttons=es<Button>()){
		list<Event>::iterator iter=_peepevent(acceptbuttons);
		if (iter!=eventqueue.end()) return (*iter);
		return Event();
	}
	Event getevent(es<Button> acceptbuttons=es<Button>()){
		list<Event>::iterator iter=_peepevent(acceptbuttons);
		if (iter!=eventqueue.end()){
			Event ret=(*iter);
			eventqueue.erase(iter);
			return ret;
		}
		return Event();
	}
	list<Event>::iterator _waitevent(es<Button>& acceptbuttons,int starttime,int timeout){
		//search for any response in the acceptbuttons set
		int endtime=timeout+SDL_GetTicks(); //endtime will be rendered meaningless when timeout<0
		//start capturing events
		list<Event>::iterator iter;
		list<Event>::iterator iter2;
		bool iterinit=false;
		bool pleasecheck=false;
		while (timeout<0 || endtime>(int)SDL_GetTicks()){
			clearevent(starttime);
			if (!eventqueue.empty()){
				if (!iterinit){
					iter=eventqueue.begin();
					pleasecheck=true;
					iterinit=true;
				}
				if (iter==eventqueue.end()){
					iter=iter2;
					if ((++iter)!=eventqueue.end()) pleasecheck=true;
					else pleasecheck=false;
				}
				if (pleasecheck) do{
					if (iter->button==Button(SDL_KEYDOWN,SDLK_F10)) dielog("waitevent: F10 Quit");
					if (acceptbuttons.empty() || acceptbuttons.exist(iter->button)) return iter;
					iter2=iter;
				}while ((++iter)!=eventqueue.end());
			}
			if (timeout>0 && (endtime-SDL_GetTicks())<10) SDL_Delay(endtime-SDL_GetTicks());
			else SDL_Delay(10);
		}
		return eventqueue.end();
	}
	Event waitevent(es<Button> acceptbuttons=es<Button>(),int starttime=SDL_GetTicks(),int timeout=-1){
		list<Event>::iterator iter=_waitevent(acceptbuttons,starttime,timeout);
		if (iter!=eventqueue.end()) return (*iter);
		return Event();
	}
	Event waitevent(int starttime,int timeout){
		return waitevent(es<Button>(),starttime,timeout);
	}
	Event waitgetevent(es<Button> acceptbuttons=es<Button>(),int starttime=SDL_GetTicks(),int timeout=-1){
		list<Event>::iterator iter=_waitevent(acceptbuttons,starttime,timeout);
		if (iter!=eventqueue.end()){
			Event ret=(*iter);
			eventqueue.erase(iter);
			return ret;
		}
		return Event();
	}
	Event waitgetevent(int starttime,int timeout){
		return waitgetevent(es<Button>(),starttime,timeout);
	}
	Event waitgetevent_enter(es<Button> acceptbuttons=es<Button>(),int starttime=SDL_GetTicks()){
		if (!acceptbuttons.empty()){
			acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_RETURN));
			acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_KP_ENTER));
		}
		Event lastevent, tmpevent;
		while (1){
			tmpevent=waitgetevent(acceptbuttons,starttime);
			if ((tmpevent.button==Button(SDL_KEYDOWN,SDLK_RETURN) || tmpevent.button==Button(SDL_KEYDOWN,SDLK_KP_ENTER))){
				if (!lastevent.empty()) break;
				tmpevent=Event();
			}
			lastevent=tmpevent;
		}
		return lastevent;
	}

	//basic hardware functions
	void beep(int freq=1000,int duration=200){
		//fill the audio_chunk;
		int nsample=duration*22050/1000;
		audio_chunk=new Uint8[nsample];
		for (int i=0;i<nsample;i++) audio_chunk[i]=127*sin(freq*i*2*pi/22050);
		audio_pos=audio_chunk;
		audio_len=nsample;
		//play
		SDL_PauseAudio(0);
		while (audio_len>0) SDL_Delay(1);
		SDL_PauseAudio(1);
		//clean up
		delete [] audio_chunk;
		return;
	}
	int getticks(){
		return (int)SDL_GetTicks();
	}
	int elapsed(int starttime,int endtime=getticks()){
		if (endtime<0) return -1;
		return endtime-starttime;
	}
	void delay(int ms,int starttime=getticks()){
		int endtime=starttime+ms;
		if ((endtime-getticks())>300){
			list<Event>::iterator iter=_waitevent(es<Button>()+Button(SDL_KEYDOWN,SDLK_F9),-1,endtime-getticks()-300);
			if (iter!=eventqueue.end() && iter->button==Button(SDL_KEYDOWN,SDLK_F9)){
				infolog("delay: F9 skip");
				return;
			}
		}
		ms=endtime-getticks();
		if (ms>0) SDL_Delay(ms);
		return;
	}

	//file functions
	void freealloutput(){
		for (unsigned int i=0;i<allFiles.size();i++) allFiles[i]->close();
		return;
	}
	ostream* createoutput(string filename){
		ofstream* vsoff=new ofstream();
		vsoff->open(filename.c_str(),ios::app);
		if (!vsoff->is_open()) dielog("Cannot open "+filename);
		allFiles.push_back(vsoff);
		atexit(freealloutput);
		return (ostream*) vsoff;
	}
	ostream* createoutput(){
		return &cout;
	}


	//color function
	void setbgcolor(double r,double g,double b){
		bgcolor.r=255*r;
		bgcolor.g=255*g;
		bgcolor.b=255*b;
		return;
	}
	void setbgcolor(color col){
		setbgcolor(col.r,col.g,col.b);
		return;
	}
	void setbggrey(double i){
		setbgcolor(i,i,i);
		return;
	}
	void setfgcolor(double r,double g,double b){
		cairo_set_source_rgb(ss,r,g,b);
		return;
	}
	void setfgcolor(color col){
		cairo_set_source_rgb(ss,col.r,col.g,col.b);
		return;
	}
	void setfggrey(double i){
		setfgcolor(i,i,i);
		return;
	}

	//bb (back Buffer) functions
	int flip(){
		SDL_Flip(bb);
		return getticks();
	}
	void clsbb(){
		SDL_FillRect(bb,&bb->clip_rect,SDL_MapRGB(bb->format,bgcolor.r,bgcolor.g,bgcolor.b));
		return;
	}
	void blitbuf(string bufferid){
		if (bufferid=="cls") clsbb();
		else if (allBuffers.find(bufferid)!=allBuffers.end()) SDL_BlitSurface(allBuffers[bufferid].hdc,NULL,bb,NULL);
		else dielog("blitbuf: no such Buffer: "+bufferid);
		return;
	}
	int showbuf(string bufferid,int numflip=1){
		if (numflip<1) dielog("showbuf: numflip must be >0");
		blitbuf(bufferid);
		int stt=flip();
		if (numflip>1) blitbuf(bufferid);
		for (int i=1;i<numflip;i++) flip();
		return stt;
	}
	ev<int> showbuf(ev<string> imgbufarr,ev<int> numfliparr){
		if (numfliparr.sum()<1) dielog("showbuf: numfliparr sum must be >0");
		if (numfliparr.min()<1) dielog("showbuf: all item in numfliparr must > 0");
		if (imgbufarr.size()!=numfliparr.size()) dielog("showbuf: imgbufarr.size!=numfliparr.size");
		ev<int> stts;
		for (size_t i=0;i<imgbufarr.size();i++){
			blitbuf(imgbufarr[i]);
			stts.push_back(flip());
			if (numfliparr[i]>1) blitbuf(imgbufarr[i]);
			for (int j=1;j<numfliparr[i];j++) flip();
		}
		return stts;
	}
	int showbuf_time(string bufferid,int ms){
		if (ms<1) dielog("showbuf_time: ms must be >0");
		if (ms<100) infolog("showbuf_time: ms<100, use of showbuf() recommended");
		blitbuf(bufferid);
		int stt=flip();
		blitbuf(bufferid);
		if (ms>300) delay(ms-300,stt);
		while (getticks()<(stt+ms)) flip();
		return stt;
	}
	void freebuf(string bufferid){
		if (allBuffers.find(bufferid)==allBuffers.end()) return;
		allBuffers[bufferid].clean();
		allBuffers.erase(bufferid);
		return;
	}
	void freeallbuf(){
		for (map<string,Buffer>::iterator iter=allBuffers.begin();iter!=allBuffers.end();iter++) iter->second.clean();
		allBuffers.clear();
		return;
	}

	//ss (soft surface) functions
	void freess(){
		if (ss==0) return;
		cairo_destroy(ss);
		ssimage.clean();
		ss=0;
		sshdc=0;
		return;
	}
	void initss(){
		if (ss!=0) freess();
		ssimage=Image(sw,sh);
		sshdc=ssimage.hdc;
		ss=cairo_create(sshdc);
		cairo_set_source_rgb(ss,1,1,1);
		return;
	}
	void clsss(){
		cairo_save(ss);
		cairo_set_operator(ss,CAIRO_OPERATOR_CLEAR);
		cairo_paint(ss);
		cairo_restore(ss);
		return;
	}
	int showss(bool clsss_as_well=true){
		Buffer tmpbuf(sw,sh);
		cairo_surface_t* crhdc=cairo_image_surface_create_for_data((unsigned char*)tmpbuf.hdc->pixels,CAIRO_FORMAT_RGB24,sw,sh,tmpbuf.hdc->pitch);
		cairo_t* cr=cairo_create(crhdc);
		cairo_set_source_rgb(cr,bgcolor.r/255.0,bgcolor.g/255.0,bgcolor.b/255.0);
		cairo_paint(cr);
		cairo_set_source_surface(cr,sshdc,0,0);
		cairo_paint(cr);
		cairo_destroy(cr);
		cairo_surface_flush(crhdc);
		cairo_surface_destroy(crhdc);
		SDL_BlitSurface(tmpbuf.hdc,NULL,bb,NULL);
		tmpbuf.clean();
		if (clsss_as_well) clsss();
		return flip();
	}
	void savess(string bufferid,bool clsss_as_well=true){
		if (allBuffers.find(bufferid)!=allBuffers.end()) freebuf(bufferid);
		Buffer tmpbuf(sw,sh);
		cairo_surface_t* crhdc=cairo_image_surface_create_for_data((unsigned char*)tmpbuf.hdc->pixels,CAIRO_FORMAT_RGB24,sw,sh,tmpbuf.hdc->pitch);
		cairo_t* cr=cairo_create(crhdc);
		cairo_set_source_rgb(cr,bgcolor.r/255.0,bgcolor.g/255.0,bgcolor.b/255.0);
		cairo_paint(cr);
		cairo_set_source_surface(cr,sshdc,0,0);
		cairo_paint(cr);
		cairo_destroy(cr);
		cairo_surface_flush(crhdc);
		cairo_surface_destroy(crhdc);
		allBuffers[bufferid]=tmpbuf;
		if (clsss_as_well) clsss();
		return;
	}
	void savess_as_file(string filename,bool clsss_as_well=false){
		cairo_surface_t* crhdc=cairo_svg_surface_create(filename.c_str(),sw,sh);
		cairo_t* cr=cairo_create(crhdc);
		cairo_set_source_rgb(cr,bgcolor.r/255.0,bgcolor.g/255.0,bgcolor.b/255.0);
		cairo_paint(cr);
		cairo_set_source_surface(cr,sshdc,0,0);
		cairo_paint(cr);
		cairo_destroy(cr);
		cairo_surface_flush(crhdc);
		cairo_surface_destroy(crhdc);
		if (clsss_as_well) clsss();
		return;
	}

	//cairo shortcuts
	void drawline(double x1,double y1,double x2,double y2){
		cairo_save(ss);
		cairo_move_to(ss,x1,y1);
		cairo_line_to(ss,x2,y2);
		cairo_stroke(ss);
		cairo_restore(ss);
		return;
	}
	void drawline(point xy,double angle,double len){
		drawline(xy.x-len/2*cos(angle),xy.y-len/2*sin(angle),xy.x+len/2*cos(angle),xy.y+len/2*sin(angle));
		return;
	}
	void drawshape(ev<point> xys){
		if (xys.size()<3) dielog("drawshape: no. of node less than 3");
		cairo_save(ss);
		cairo_move_to(ss,xys[0].x,xys[0].y);
		for (int i=1;i<(int)xys.size();i++) cairo_line_to(ss,xys[i].x,xys[i].y);
		cairo_close_path(ss);
		cairo_stroke(ss);
		cairo_restore(ss);
		return;
	}
	void drawcircle(double x, double y, double radius){
		cairo_save(ss);
		cairo_move_to(ss,x+radius,y);
		cairo_arc(ss,x,y,radius,0,2*pi);
		cairo_close_path(ss);
		cairo_stroke(ss);
		cairo_restore(ss);
		return;
	}
	void fillshape(ev<point> xys){
		if (xys.size()<3) dielog("fillshape: no. of node less than 3");
		cairo_save(ss);
		cairo_move_to(ss,xys[0].x,xys[0].y);
		for (int i=1;i<(int)xys.size();i++) cairo_line_to(ss,xys[i].x,xys[i].y);
		cairo_close_path(ss);
		cairo_fill(ss);
		cairo_restore(ss);
		return;
	}
	void fillcircle(double x, double y, double radius){
		cairo_save(ss);
		cairo_move_to(ss,x+radius,y);
		cairo_arc(ss,x,y,radius,0,2*pi);
		cairo_close_path(ss);
		cairo_fill(ss);
		cairo_restore(ss);
		return;
	}

	//image functions
	void freeimg(string imgfn){
		allImages[imgfn].clean();
		allImages.erase(imgfn);
		return;
	}
	void freeallimg(){
		for (map<string,Image>::iterator iter=allImages.begin();iter!=allImages.end();iter++) iter->second.clean();
		allImages.clear();
		return;
	}
	void saveimg(string imgname,shape shapei,double xtrans=0,double ytrans=0,bool clsss_as_well=true){
		if (allImages.find(imgname)!=allImages.end()) freeimg(imgname);
		Image img(2*imgmargin+2*shapei.radius);
		img.rect=shapei;
		cairo_t* cr=cairo_create(img.hdc);
		cairo_set_source_surface(cr,sshdc,img.diameter/2-cx-xtrans,img.diameter/2-cy-ytrans);
		cairo_paint(cr);
		cairo_destroy(cr);
		cairo_surface_flush(img.hdc);
		if (clsss_as_well) clsss();
		allImages[imgname]=img;
	}
	void saveimg_rect(string imgname, double w, double h, double ori=-pi/2, double xtrans=0, double ytrans=0, bool translatefollowori=true, bool clsss_as_well=true){
		if (translatefollowori){
			double ox=xtrans;
			double oy=ytrans;
			xtrans=(int)(-oy*cos(ori)-ox*cos(pi/2-ori));
			ytrans=(int)(-oy*sin(ori)+ox*sin(pi/2-ori));
		}
		saveimg(imgname,shape(ori,w,h),xtrans,ytrans,clsss_as_well);
		return;
	}
	void saveimg_circle(string imgname, double radius, double xtrans=0, double ytrans=0, bool clsss_as_well=true){
		saveimg(imgname,shape(radius),xtrans,ytrans,clsss_as_well);
		return;
	}
	void transformimg(string srcimg,string destimg,double rotate,double scale){
		Image dimg(allImages[srcimg].diameter*scale);
		if (allImages[srcimg].rect.isrect) dimg.rect.setrect(allImages[srcimg].rect.ori+rotate,allImages[srcimg].rect.w*scale,allImages[srcimg].rect.h*scale);
		else dimg.rect.setcircle(allImages[srcimg].rect.radius*scale);
		cairo_t* cr=cairo_create(dimg.hdc);
		cairo_translate(cr,dimg.diameter/2,dimg.diameter/2);
		cairo_scale(cr,scale,scale);
		cairo_rotate(cr,rotate);
		cairo_set_source_surface(cr,allImages[srcimg].hdc,-allImages[srcimg].diameter/2,-allImages[srcimg].diameter/2);
		cairo_paint(cr);
		cairo_destroy(cr);
		cairo_surface_flush(dimg.hdc);
		if (srcimg==destimg) freeimg(srcimg);
		allImages[destimg]=dimg;
		return;
	}
	void showimg(string imgname,int x, int y){
		if (allImages.find(imgname)==allImages.end()) dielog("showimg: no such Image: "+imgname);
		cairo_save(ss);
		cairo_set_source_surface(ss,allImages[imgname].hdc,x-allImages[imgname].diameter/2,y-allImages[imgname].diameter/2);
		cairo_paint(ss);
		if (imgdebugmode){
			if (allImages[imgname].rect.isrect){
				cairo_move_to(ss,allImages[imgname].rect.points[3].x+x,allImages[imgname].rect.points[3].y+y);
				for (int i=0;i<4;i++) cairo_line_to(ss,allImages[imgname].rect.points[i].x+x,allImages[imgname].rect.points[i].y+y);
				cairo_set_line_width(ss,1);
				cairo_set_source_rgb(ss,1,0,0);
				cairo_stroke(ss);
			} else {
				cairo_move_to(ss,x+allImages[imgname].rect.radius,y);
				cairo_arc(ss,x,y,allImages[imgname].rect.radius,0,2*pi);
				cairo_set_line_width(ss,1);
				cairo_set_source_rgb(ss,1,0,0);
				cairo_close_path(ss);
				cairo_stroke(ss);
			}
		}
		cairo_restore(ss);
		return;
	}
	void loadpic_as_img(string imgid,string imgfn=""){
		if (imgfn.empty()) imgfn=imgid;
		Buffer cimg(imgfn);
		cairo_surface_t* cimghdc=cairo_image_surface_create_for_data((unsigned char*)cimg.hdc->pixels,CAIRO_FORMAT_RGB24,cimg.hdc->w,cimg.hdc->h,cimg.hdc->pitch);
		Image simg(py(cimg.width,cimg.height));
		simg.rect.setrect(-pi/2,cimg.width,cimg.height);
		cairo_t* cr=cairo_create(simg.hdc);
		cairo_set_source_surface(cr,cimghdc,(simg.diameter-cimg.width)/2,(simg.diameter-cimg.height)/2);
		cairo_paint(cr);
		cairo_destroy(cr);
		cairo_surface_flush(simg.hdc);
		allImages[imgid]=simg;
		cairo_surface_destroy(cimghdc);
		cimg.clean();
		return;
	}
	void showpic(string imgfn,int x,int y){
		Buffer cimg(imgfn);
		cairo_save(ss);
		cairo_surface_t* cimghdc=cairo_image_surface_create_for_data((unsigned char*)cimg.hdc->pixels,CAIRO_FORMAT_RGB24,cimg.hdc->w,cimg.hdc->h,cimg.hdc->pitch);
		cairo_set_source_surface(ss,cimghdc,x-cimg.width/2,y-cimg.height/2);
		cairo_paint(ss);
		cairo_surface_destroy(cimghdc);
		cimg.clean();
		cairo_restore(ss);
		return;
	}

	pair<int,int> showtext(string in_str, int x, int y,int size=20,string fontdesc="Verdana",bool pixelposition=false,int w=sh,PangoAlignment palign=PANGO_ALIGN_LEFT,double pspacing=1.0,PangoWrapMode pwrap=PANGO_WRAP_WORD_CHAR,int pindent=0){
		string font=fontdesc+", "+itos(size)+"px";
		PangoLayout* layout=pango_cairo_create_layout(ss);
		pango_layout_set_width(layout,w*PANGO_SCALE);
		pango_layout_set_alignment(layout,palign);
		pango_layout_set_spacing(layout,(pspacing-1)*size*PANGO_SCALE);
		pango_layout_set_wrap(layout,pwrap);
		pango_layout_set_indent(layout,pindent*PANGO_SCALE);
		pango_layout_set_text(layout,in_str.c_str(),-1);
		PangoFontDescription* desc=pango_font_description_from_string(font.c_str());
		pango_layout_set_font_description(layout,desc);
		pango_font_description_free(desc);
		cairo_save(ss);
		pango_cairo_update_layout(ss,layout);
		PangoRectangle ink_rect,logical_rect;
		pango_layout_get_pixel_extents(layout,&ink_rect,&logical_rect);
		int tw,th,tx,ty;
		if (pixelposition){
			tw=ink_rect.width;
			th=ink_rect.height;
			tx=ink_rect.x;
			ty=ink_rect.y;
		} else {
			tw=logical_rect.width;
			th=logical_rect.height;
			tx=logical_rect.x;
			ty=logical_rect.y;
		}

		cairo_move_to(ss,x-tw/2-tx,y-th/2-ty);
		pango_cairo_show_layout(ss,layout);
		//pango_cairo_layout_path(ss,layout);
		//cairo_fill(ss);
		cairo_restore(ss);
		g_object_unref(layout);
		return pair<int,int>(tw,th);
	}

	//input functions
	string inputstring(string prompt,string defaultval=string(),int changecase=0,int timeout=-1,bool* timeoutflag=NULL){
		int starttime=getticks();
		if (timeoutflag!=NULL) *timeoutflag=false;
		string input=defaultval;
		bool firsttimepress=true;
		bool clear=true;
		bool finish=false;
		clearevent();
		do{
			Event keypress;
			while (!(keypress=getevent()).empty()){
				if (keypress.button==Button(SDL_KEYDOWN,SDLK_RETURN) || keypress.button==Button(SDL_KEYDOWN,SDLK_KP_ENTER)){
					finish=true;
					break;
				} else if (keypress.button==Button(SDL_KEYDOWN,SDLK_ESCAPE)){
					input=defaultval;
					clear=true;
				} else if (keypress.button==Button(SDL_KEYDOWN,SDLK_BACKSPACE)){
					input=input.substr(0,_max(0,input.length()-1));
					firsttimepress=false;
				} else if (keypress.keyname.length()==1){
					if (clear){
						input.clear();
						clear=false;
					}
					input.append(keypress.keyname);
					firsttimepress=false;
				}
			}
			if (changecase==1) transform(input.begin(),input.end(),input.begin(),(int(*)(int))toupper);
			if (changecase==2) transform(input.begin(),input.end(),input.begin(),(int(*)(int))tolower);
			if (firsttimepress && timeout>0 && (getticks()-starttime)>timeout){
				if (timeoutflag!=NULL) *timeoutflag=true;
				finish=true;
			}
			clsss();
			showtext(prompt,cx,cy-50);
			showtext("[ "+input+" ]",cx,cy+50);
			showss();
		} while(!finish);
		return input;
	}
	int inputint(string prompt,int defaultval=0,int timeout=-1,bool* timeoutflag=NULL){
		int starttime=getticks();
		if (timeoutflag!=NULL) *timeoutflag=false;
		string input=itos(defaultval);
		bool firsttimepress=true;
		bool clear=true;
		bool finish=false;
		es<Button> acceptbuttons;
		for (int i=0;i<10;i++) acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_0+i));
		for (int i=0;i<10;i++) acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_KP0+i));
		acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_MINUS));
		acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_KP_MINUS));
		acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_RETURN));
		acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_KP_ENTER));
		acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_ESCAPE));
		acceptbuttons.insert(Button(SDL_KEYDOWN,SDLK_BACKSPACE));
		clearevent();
		do{
			Event keypress;
			while (!(keypress=getevent(acceptbuttons)).empty()){
				if (keypress.button==Button(SDL_KEYDOWN,SDLK_RETURN) || keypress.button==Button(SDL_KEYDOWN,SDLK_KP_ENTER)){
					finish=true;
					break;
				} else if (keypress.button==Button(SDL_KEYDOWN,SDLK_ESCAPE)){
					input=itos(defaultval);
					clear=true;
				} else if (keypress.button==Button(SDL_KEYDOWN,SDLK_BACKSPACE)){
					input=input.substr(0,_max(0,input.length()-1));
					firsttimepress=false;
				} else if (keypress.button==Button(SDL_KEYDOWN,SDLK_MINUS) || keypress.button==Button(SDL_KEYDOWN,SDLK_KP_MINUS)){
					if (input.empty()) input="-";
					else input=itos(-atoi(input.c_str()));
					firsttimepress=false;
				} else if (keypress.keyname.length()==1){
					if (clear){
						input.clear();
						clear=false;
					}
					input.append(keypress.keyname);
					firsttimepress=false;
				}
			}
			if (firsttimepress && timeout>0 && (getticks()-starttime)>timeout){
				if (timeoutflag!=NULL) *timeoutflag=true;
				finish=true;
			}
			clsss();
			showtext(prompt,cx,cy-50);
			showtext("[ "+input+" ]",cx,cy+50);
			showss();
		} while(!finish);
		return atoi(input.c_str());
	}

	//positioning tools
	double min_dist(const ev<string>& items, const ev<pair<int,int> >& points){
		double mindist=sw+sh;
		for (unsigned int a=0;a<items.size();a++) for (unsigned int b=a+1;b<items.size();b++) mindist=_min1(mindist,shape_dist(allImages[items[a]].rect,point(points[a]),allImages[items[b]].rect,point(points[b])));
		return mindist;
	}
	double min_dist(const ev<string>& items, const ev<point>& points){
		double mindist=sw+sh;
		for (unsigned int a=0;a<items.size();a++) for (unsigned int b=a+1;b<items.size();b++) mindist=_min1(mindist,shape_dist(allImages[items[a]].rect,points[a],allImages[items[b]].rect,points[b]));
		return mindist;
	}
	double min_dist(const ev<shape>& items, const ev<point>& points){
		double mindist=sw+sh;
		for (unsigned int a=0;a<items.size();a++) for (unsigned int b=a+1;b<items.size();b++) mindist=_min1(mindist,shape_dist(items[a],points[a],items[b],points[b]));
		return mindist;
	}
	pair<int,int> randpos_rect(string itemstr,int l, int r, int t, int b){
		if (allImages.find(itemstr)==allImages.end()) dielog("randpos_rect: no such cairo Image: "+itemstr);
		if (imgdebugmode){
			cairo_save(ss);
			cairo_set_source_rgb(ss,0,1,1);
			cairo_set_line_width(ss,1);
			cairo_move_to(ss,l,t);
			cairo_line_to(ss,r,t);
			cairo_line_to(ss,r,b);
			cairo_line_to(ss,l,b);
			cairo_close_path(ss);
			cairo_stroke(ss);
			cairo_restore(ss);
		}
		//get the box boundary
		//adjust for edge boxes
		l+=allImages[itemstr].rect.boundw/2;
		r-=allImages[itemstr].rect.boundw/2;
		t+=allImages[itemstr].rect.boundh/2;
		b-=allImages[itemstr].rect.boundh/2;
		//get the tmpX and tmpY
		if (l>r || t>b) dielog("randpos_rect: padding overflow in table search");
		int tmpx=l+genrand_int32()%(r-l);
		int tmpy=t+genrand_int32()%(b-t);
		return pair<int,int>(tmpx,tmpy);
	}
	bool checkpos_rect(string itemstr, int x, int y, int l, int r, int t, int b){
		if (allImages.find(itemstr)==allImages.end()) dielog("checkpos_rect: no such Image: "+itemstr);
		int iw2=allImages[itemstr].rect.boundw/2;
		int ih2=allImages[itemstr].rect.boundh/2;
		return ((((x-iw2)>=l) && ((x+iw2)<r)) && (((y-ih2)>=t) && ((y+ih2)<b)));
	}
	void search(ev<string> items, ev<pair<int,int> > xyin, string fimg=string()){
		if (!fimg.empty() && allImages.find(fimg)==allImages.end()) dielog("search: no such fixation Image: "+fimg);
		if (items.size()!=xyin.size()) dielog("search: item size and pos size not match");
		if (!fimg.empty()) showimg(fimg,cx,cy);
		for (size_t i=0;i<items.size();i++) showimg(items[i],xyin[i].first,xyin[i].second);
		return;
	}
	ev<ev<pair<int,int> > > search_table(ev<ev<string> > items, int x, int y, int ncol, int nrow, int width, int height, double mindist, string fimg=string(), bool shuffleorder=true){
		//first determine dynamic parameters
		if (mindist<=0) dielog("search_table: mindist should be > 0");
		ev<shape> serializeditems;
		if (!fimg.empty()){
			if (allImages.find(fimg)==allImages.end()) dielog("search_table: no such fixation Image: "+fimg);
			serializeditems.push_back(allImages[fimg].rect);
		}
		for (size_t i=0;i<items.size();i++) for (size_t j=0;j<items[i].size();j++){
			if (allImages.find(items[i][j])==allImages.end()) dielog("search_table: no such Image: "+items[i][j]);
			serializeditems.push_back(allImages[items[i][j]].rect);
		}

		int holdersize=ncol*nrow;
		if (holdersize<(int)items.size()) dielog("search_table: too few place holder");
		ev<int> ikeymap;
		ev<ev<point> > points;
		ev<point> pointss;
		for (int i=0;i<holdersize;i++) ikeymap.push_back(i);
		if (shuffleorder) ikeymap.shuffle();

		double mindistt=0;
		while (mindistt<mindist){
			//initialize
			points.clear();
			pointss.clear();
			if (!fimg.empty()) pointss.push_back(point(cx,cy));
			//generate candidate xys
			for (size_t i=0;i<items.size();i++){
				int tmpc=ikeymap[i]%ncol;
				int tmpr=ikeymap[i]/ncol;
				double mindistti=0;
				ev<point> pointsi;
				while (mindistti<mindist){
					pointsi.clear();
					for (size_t j=0;j<items[i].size();j++) pointsi.push_back(point(randpos_rect(items[i][j],x-width/2+tmpc*width/ncol,x-width/2+((tmpc+1)*width)/ncol,y-height/2+(tmpr*height)/nrow,y-height/2+((tmpr+1)*height)/nrow)));
					mindistti=min_dist(items[i],pointsi);
				}
				points.push_back(pointsi);
				for (size_t ii=0;ii<pointsi.size();ii++) pointss.push_back(pointsi[ii]);
			}
			mindistt=min_dist(serializeditems,pointss);
		}
		//for debug
		//showtext(itos(mindistt),cx,30);

		ev<string> serializeditemsstr;
		ev<pair<int,int> > serializedxys;

		ev<ev<pair<int,int> > > xyout;
		for (size_t i=0;i<items.size();i++){
			ev<pair<int,int> > xyouti;
			for (size_t j=0;j<items[i].size();j++){
				xyouti.push_back(pair<int,int>(points[i][j].x,points[i][j].y));
				serializeditemsstr.push_back(items[i][j]);
				serializedxys.push_back(xyouti.back());
			}
			xyout.push_back(xyouti);
		}

		search(serializeditemsstr,serializedxys,fimg);

		return xyout;
	}
	ev<pair<int,int> > search_table(ev<string> items, int x, int y, int ncol, int nrow, int width, int height, double mindist, string fimg=string(), bool shuffleorder=true){
		ev<ev<string> > multiitems;
		for (size_t i=0;i<items.size();i++){
			ev<string> itemsi;
			itemsi.push_back(items[i]);
			multiitems.push_back(itemsi);
		}
		ev<ev<pair<int,int> > > xyouts=search_table(multiitems,x,y,ncol,nrow,width,height,mindist,fimg,shuffleorder);
		ev<pair<int,int> > xyout;
		for (size_t i=0;i<xyouts.size();i++) for (size_t j=0;j<xyouts[i].size();j++) xyout.push_back(xyouts[i][j]);
		return xyout;
	}
	ev<pair<int,int> > search_ring(ev<string> items, int x, int y, int holdersize, int radiusx, int radiusy, double phase=0, string fimg=string(), bool shuffleorder=true){
		if (holdersize<(int)items.size()) dielog("search_ring: too few place holder");

		double tmpA;
		int tmpX, tmpY;
		ev<int> ikeymap;
		ev<pair<int,int> > xyout;
		for (int i=0;i<holdersize;i++) ikeymap.push_back(i);
		if (shuffleorder) ikeymap.shuffle();
		
		for (size_t i=0;i<items.size();i++){
			tmpA=phase+ikeymap[i]*2*pi/holdersize;
			tmpX=x+radiusx*sin(tmpA);
			tmpY=y-radiusy*cos(tmpA);
			xyout.push_back(pair<int,int>(tmpX,tmpY));
		}

		search(items,xyout,fimg);
		return xyout;
	}
	ev<pair<int,int> > search_radio(ev<string> items, int x, int y, ev<int> numpiece, ev<int> ringwidth, double mindist, string fimg=string(), bool shuffleorder=true, bool randomphase=true){

		int holdersize=numpiece.sum();
		if (holdersize<(int)items.size()) dielog("search_radio: too many items");
		if (numpiece.size()!=ringwidth.size()) dielog("search_radio: ring number unmatch");

		double tmpA,tmpZ;
		int tmpX, tmpY;
		ev<int> ikeymap;
		ev<pair<int,int> > xyout;
		for (int i=0;i<holdersize;i++) ikeymap.push_back(i);
		if (shuffleorder) ikeymap.shuffle();

		map<int, pair<int,int> > ikeyposmap;
		for (int i=0;i<(int)numpiece.size();i++) for (int j=0;j<numpiece[i];j++){
			ikeyposmap[ikeymap[ikeyposmap.size()]]=pair<int,int>(i,j);
		}
		ev<int> ringstart;
		int accradius=0;
		for (size_t i=0;i<ringwidth.size();i++){
			ringstart.push_back(accradius);
			accradius+=ringwidth[i];
		}
		ev<double> phase;
		for (size_t i=0;i<numpiece.size();i++) phase.push_back(randomphase?(genrand_real2()*2*pi):0);
		
		double curmindist;
		do{
			xyout.clear();
			for (int i=0;i<(int)items.size();i++){
				tmpZ=ringstart[ikeyposmap[i].first]+genrand_real2()*ringwidth[ikeyposmap[i].first];
				tmpA=phase[ikeyposmap[i].first]+(ikeyposmap[i].second+genrand_real2())*2*pi/numpiece[ikeyposmap[i].first];
				tmpX=x+tmpZ*sin(tmpA);
				tmpY=y-tmpZ*cos(tmpA);
				xyout.push_back(pair<int,int>(tmpX,tmpY));
			}
			if (fimg.empty()) curmindist=min_dist(items,xyout);
			else curmindist=min_dist(items-fimg,xyout-pair<int,int>(cx,cy));
		} while(curmindist<mindist);
		search(items,xyout,fimg);

		return xyout;
	}

	//factor functions
	int getlatin(int subject, int blocktotal, int blocki){
		if (blocki<0) dielog("getlatin: blocki<0");
		subject=_max(0,subject);
		if (blocki>=blocktotal) dielog("getlatin: blocki > blocktotal");
		int numrow=(blocktotal%2)?(blocktotal*2):(blocktotal);
		ev<ev<int> > latin;
		latin.resize(numrow);
		//generate the first subject
		for (int i=1;i<=blocktotal;i++) latin[0].push_back((i%2)?(1+((blocktotal-(i-3)/2)-1)%blocktotal):(i/2+1));
		//generate other rows
		for (int r=1;r<blocktotal;r++) for (int i=0;i<blocktotal;i++) latin[r].push_back(1+((latin[r-1][i]+1)-1)%blocktotal);
		//for odd number
		if (blocktotal%2) for (int r=0;r<blocktotal;r++){
			latin[blocktotal+r]=latin[r];
			reverse(latin[blocktotal+r].begin(),latin[blocktotal+r].end());
		}
		ev<int> map;
		map.resize(blocktotal);
		for (int i=0;i<blocktotal;i++) map[latin[0][i]-1]=i;
		return (map[latin[subject%numrow][blocki]-1]);
	}
	template<typename T> int shufflefactor(ev<ev<T>* >& lists, int offset, int size){
		int num=(int)lists.size();
		for (int i=0;i<num;i++) if ((offset+size)>(int)lists[i]->size()) dielog("shufflefactor: range exceeded");
		ev<int> index;
		for (int i=0;i<size;i++) index.push_back(i);
		index.shuffle();
		for (int i=0;i<num;i++){
			ev<T> shadow;
			shadow.resize(size);
			for (int j=0;j<size;j++) shadow[j]=(*lists[i])[offset+index[j]];
			for (int j=0;j<size;j++) (*lists[i])[offset+j]=shadow[j];
		}
		return 0;
	}
	template<typename T> void _genfactor(int numfactor, int numpretrial, int numtrial, ev<ev<T>* >& lists, ev<ev<T>* >& values){
		if (numfactor==0) return;
		numfactor--;
		int numvalue=(int)values[numfactor]->size();
		for (int vi=0;vi<numvalue;vi++){
			int numpretrial2=numpretrial+vi*numtrial/numvalue;
			int numtrial2=(vi+1)*numtrial/numvalue-vi*numtrial/numvalue;
			for (int ti=numpretrial2;ti<(numpretrial2+numtrial2);ti++) (*lists[numfactor])[ti]=(*values[numfactor])[vi];
			_genfactor(numfactor,numpretrial2,numtrial2,lists,values);
		}
		return;
	}
	template<typename T> void genfactor(int numtrial, int numpretrial, ev<ev<T>* > lists, ev<ev<T>* > values){
		if (lists.size()!=values.size()) dielog("genfactor: number of factors unmatch");
		int numfac=(int)lists.size();
		int ttnumcond=1;
		for (int i=0;i<numfac;i++) ttnumcond*=(int)values[i]->size();
		if ((numtrial%ttnumcond)>0) dielog("genfactor: Number of trials not a multiple of number of conditions");
		for (int i=0;i<numfac;i++) lists[i]->resize(numtrial+numpretrial);
		_genfactor(numfac,numpretrial,numtrial,lists,values);
		for (int i=0;i<numpretrial;i++){
			int copyfromtrial=numpretrial+genrand_int32()%numtrial;
			for (int j=0;j<numfac;j++) (*lists[j])[i]=(*lists[j])[copyfromtrial];
		}
		shufflefactor(lists,numpretrial,numtrial);
		return;
	}

	//init display script
	void initdisplay(int width=800, int height=600){
		//initiate devices
		_createdevices(width,height);
		devicestarted=true;

		//initiate soft surface
		initss();
		ssstarted=true;

		return;
	}
	void closedisplay(){
		//free soft surface
		if (ssstarted){
			ssstarted=false;
			freess();
		}
		//close devices
		if (devicestarted){
			devicestarted=false;
			_cleandevices();
		}
		return;
	}

	//end of vs6 framework functions
}

