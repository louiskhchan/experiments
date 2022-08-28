// vs6_framework: sample experiment configuration file
// 19jan2010 louis chan


void exptmain(){

	string experimentid=vs::inputstring("Please input the experiment code:","accuracy1");
	int subjectid=vs::inputint("Please input subject number",-1);
	ostream* vsof;
	if (subjectid<0) vsof=&cout;
	else vsof=vs::createoutput(experimentid+"_"+itos(subjectid)+".dat");

	int nb=3; //total number of block, including all conditions
	int npb=1; //now npb is for measuring the threshold no. of frames
	int nt=112;
	int npt=5;
//	vs::imgdebugmode=true;

	//stimuli parameters
	double linethick=5; //line thickness
	double linelen=40;
	double minsep=20;
	int ncol=3;
	int nrow=4;
	int pw=300;
	int ph=400;
	int psep=40;
	double maskjitter=10;
	double masknum=6;

	int ssize=24;

	cairo_set_source_rgb(ss,1,1,1);

	//prepare fixation cross
	cairo_move_to(ss,cx+4,cy);
	cairo_arc(ss,cx,cy,4,0,2*pi);
	cairo_fill(ss);
	vs::savess("fixscr",false);
	vs::saveimg_circle("cross",4);
	vs::saveimg_circle("crossdummy",4);

	//prepare the lines
	ev<double> angles=ev<double>()+(-pi/2)+(-pi/4)+0.0;
	ev<string> anglelabels=ev<string>()+"p0"+"p45"+"p90"; 
	ev<color> colors=ev<color>()+color(.5,.5,.5)+color(1,0,0);
	ev<string> colorlabels=ev<string>()+"grey"+"red";
	cairo_set_line_width(ss,linethick);
	cairo_set_line_cap(ss,CAIRO_LINE_CAP_ROUND);
	for (size_t j=0;j<colors.size();j++) for (size_t i=0;i<angles.size();i++){
		vs::setfgcolor(colors[j]);
		cairo_move_to(ss,cx-linelen/2*cos(angles[i]),cy-linelen/2*sin(angles[i]));
		cairo_line_to(ss,cx+linelen/2*cos(angles[i]),cy+linelen/2*sin(angles[i]));
		cairo_stroke(ss);
		vs::saveimg_rect(colorlabels[j]+anglelabels[i],linethick,linelen,angles[i]);
	}
	vs::setfgcolor(1,1,1);

	//definitions
	ev<string> target=evs()+"greyp0"+"greyp45";
	string distractor="greyp0";
	string singleton="redp0";
	string fixation="cross";
	string fixationdummy="crossdummy";

	//prepare response es
	es<Button> responseset=esb()+Button(SDL_KEYDOWN,SDLK_LSHIFT)+Button(SDL_KEYDOWN,SDLK_RSHIFT)+Button(SDL_KEYDOWN,SDLK_F9);

	Staircase staircase(1,3,.25,17.1);
	
	//experiment
	for (int rbi=0;rbi<(npb+nb);rbi++){
		int bi=rbi-npb;

		if (bi==0){
			if (!staircase.finish()){
				vs::showtext("Cannot find threshold. Please find your experimenter.",cx,cy);
				vs::showss();
				while (1) vs::waitevent();
			}
		}

		//display instruction
		vs::showtext("Press any key to start block "+itos(bi+npb+1),cx,cy);
		vs::showss();
		vs::waitevent();
		vs::showbuf("cls");

		//generate trial sequence
		ev<int> ans;
		ev<int> sing;
		ev<int> tarlr;
		ev<int> ansvals=evi()+0+1; //0: p30, 1:p90 
		ev<int> singvals=evi()+0+0+1+2; //0: no sing, 1: sing diff side, 2: sing same side
		ev<int> tarlrvals=evi()+0+1; //0: tar left, 1: tar right

		//always modify this line according to the experiment design
		vs::genfactor(nt,npt,evevi()+&ans+&sing+&tarlr,evevi()+&ansvals+&singvals+&tarlrvals);

		//start running trials
		for (int ti=0;ti<(npt+nt);ti++){
			//prepare displays before really running the trial

			//measure the trial start time so as to compensate for stimulus preparation time
			int starttrialtime=vs::getticks();
			vs::showbuf("cls");

			//generate search display
			//expt1 is discrimination
			ev<ev<string> > searchitems;
			searchitems.resize(2);
			searchitems[tarlr[ti]].push_back(target[ans[ti]]);
			if (sing[ti]==1) searchitems[neg(tarlr[ti])].push_back(singleton);
			if (sing[ti]==2) searchitems[tarlr[ti]].push_back(singleton);
			for (size_t i=0;i<2;i++) while (searchitems[i].size()<(size_t)(ssize/2)) searchitems[i].push_back(distractor);

			//finish stimuli, create search screen
			ev<ev<pair<int,int> > > tablepos;
			tablepos.resize(2);
			tablepos[0]=vs::search_table(searchitems[0],cx-(pw+psep)/2,cy,ncol,nrow,pw,ph,minsep,fixation);
			tablepos[1]=vs::search_table(searchitems[1],cx+(pw+psep)/2,cy,ncol,nrow,pw,ph,minsep,fixationdummy);
			vs::savess("searchscr");

			vs::setfgcolor(1,1,1);
			for (size_t i=0;i<2;i++) for (size_t j=0;j<tablepos[i].size();j++) for (int k=0;k<masknum;k++){
				double x=tablepos[i][j].first+genrand_real2()*maskjitter-maskjitter/2;
				double y=tablepos[i][j].second+genrand_real2()*maskjitter-maskjitter/2;
				double angle=genrand_real2()*pi;
				cairo_move_to(ss,x-linelen/2*cos(angle),y-linelen/2*sin(angle));
				cairo_line_to(ss,x+linelen/2*cos(angle),y+linelen/2*sin(angle));
				cairo_stroke(ss);
			}
			vs::showimg(fixation,cx,cy);
			vs::savess("maskscr");

			//start trial
			vs::delay(500,starttrialtime);
			vs::showbuf_time("fixscr",500);
			int starttime=vs::showbuf("searchscr",_max(1,staircase.intensity));
			vs::showbuf_time("maskscr",1000);
			vs::showbuf("cls");
			Event response=vs::waitgetevent(responseset);
			int rt=vs::elapsed(starttime,response.time);
			if (response.button==Button(SDL_KEYDOWN,SDLK_F9)) break;

			//judge correctness
			int correct=0;
			if (ans[ti] && response.button==Button(SDL_KEYDOWN,SDLK_LSHIFT)) correct=1;
			if (!ans[ti] && response.button==Button(SDL_KEYDOWN,SDLK_RSHIFT)) correct=1;
			if (!correct) vs::beep();
			
			//output information
			*vsof << experimentid << "\t";
			*vsof << subjectid << "\t";
			*vsof << (bi+1) << "\t";
			*vsof << (ti-npt) << "\t";
			*vsof << ans[ti] << "\t";
			*vsof << sing[ti] << "\t";
			*vsof << tarlr[ti] << "\t";
			*vsof << correct << "\t";
			*vsof << rt << "\t";
			*vsof << _max(1,staircase.intensity) << "\t";
			*vsof << endl;

			//change nframe
			if (bi<0) staircase.next(correct!=0);

		}//finish trials

	}//finish blocks

	//say thank you
	vs::showtext("Finished! Thank you very much.",cx,cy);
	vs::showss();
	vs::waitevent();

	return;
}

