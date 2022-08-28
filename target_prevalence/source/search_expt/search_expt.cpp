// vs73.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vs73.h>

CAppModule _Module;
namespace vs { app_t app; }
using namespace vs;

struct trial_data_t {
	string exptid;
	int subjid;
	int bi; //bi-npb
	int ti; //ti-npt
	int rare_first;
	int is_rare;
	int ssize;
	int pa;
	int response;
	bool correct;
	float rt;
	vector<D2D1_POINT_2F> item_pos;
	vector<float> item_ori;
	template<class Archive>	void serialize(Archive & ar, const unsigned int version) {
		ar & exptid;
		ar & subjid;
		ar &  bi;
		ar &  ti;
		ar &  rare_first;
		ar &  is_rare;
		ar &  ssize;
		ar &  pa;
		ar &  response;
		ar &  correct;
		ar &  rt;
		ar & item_pos;
		ar & item_ori;
	}
};

//design
constexpr int expt_npt = 5;
constexpr int expt_nt = 120;
constexpr int nbb = 2;
constexpr int nb = 2;
constexpr int practice_nt = 50;

int main()
{
	try {
		app.init();

		//set experiment params
		vector<int> is_rares = { 0,1 };

		//experiment
		string exptid = "prev7";
		int subjid = messagebox::getnumber(L"Enter subject no.: ", 0);

		ofstream outf(exptid + "_" + subjid + ".txt");

		//log contrast data
		ofstream outaf(exptid + "_" + subjid + "_contrast_data.dat");
		text_oarchive outa(outaf);
		vector<trial_data_t> trial_datas;
		raii_t datalogger([] {}, [&] {
			outa << trial_datas;
		});

		//stimulus
		D2D1_COLOR_F bgcolor = ColorF(ColorF::Black);
		D2D1_COLOR_F fgcolor = ColorF(ColorF::White);
		auto fixation = make_cross(20, 3, fgcolor);
		//		auto beep = make_beep(1000, 200ms);

		//float bar_length = 30.f;
		//float bar_thickness = 10.f;
		//D2D1_COLOR_F bar_color = ColorF(.4f, .4f, .4f);

		float landot_diameter = 25.f;
		float landot_gapsize = .8f; //in radians
		stroke_t landot_stroke(
			ColorF(.5f, .5f, .5f), // color
			5.f, //thickness
			app.draw.CreateStrokeStyle(StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND)));

		float center_margin_from_cell = landot_diameter*.5f;//
		D2D1_SIZE_F margin_size = SizeF(center_margin_from_cell, center_margin_from_cell);

		//float distractor_sd = 4.5f / 2;
		//float distractor_max = 4.5f;
		float distractor_ori = 0.f;
		float target_ori = 90.f;
		
		//prepare experiment window
#ifdef VS_USE_PRINT
		win_t win(WMT_REGULAR);
		app.rawinput_win.RegisterMice();
#else
		win_t win(WMT_EXPERIMENT);
		hide_cursor_t hct;
#endif
		//pre gen screens
		sketch_t iti_scr(bgcolor);
		sketch_t fixation_scr(bgcolor);
		fixation.Draw1(fixation_scr, align_origin, win.center);

		//error screen
		duration error_scr_duration = 500ms;
		sketch_t fa_scr(bgcolor);
		label_t1(L"False Alarm", brush_t(fgcolor), nullptr).Draw1(fa_scr, align_bound_center, win.center);
		sketch_t miss_scr(bgcolor);
		label_t1(L"Miss", brush_t(fgcolor), nullptr).Draw1(miss_scr, align_bound_center, win.center);

		//start experiment
		//between subj var
		int rare_first = subjid % 2;
		if (rare_first) reverse(is_rares.begin(), is_rares.end());

		//block loop
		for (int bbi = 0; bbi < nbb; bbi++) {
			int is_rare = is_rares[bbi];

			//prepare block
			//gen trials
			auto ssize = make_iv(4, 9, 16); //set size type
			auto pa = is_rare ? make_iv(0, 0, 0, 0, 1) : make_iv(0, 1, 1, 1, 1); //0: absent, 1: present

			//run block
			auto run_trial = [&](int bi, int nt, int npt) {
				//gen trials
				gen_trials<expt_nt, expt_npt>(ssize, pa);
				//trial loop
				for (int ti = 0; ti < nt + npt; ti++) {
					auto trial_start_time = clock::now();

					//prepare screens
					//gen stimulus

					//vector<float> item_ori;
					//for (int i = 0; i < ssize[ti]; i++) {
					//	float cand;
					//	do {
					//		cand = rnorm(0.f, distractor_sd);
					//	} while (abs(cand) > distractor_max);
					//	item_ori.push_back(cand);
					//}
					//if (pa[ti]) item_ori[0] = app.rand_sign()*target_ori;
					vector<float> item_ori(ssize[ti], distractor_ori);
					if (pa[ti]) item_ori[0] = target_ori;

					//prepare items
					vector<drawable_holder_t> items;
					for (int i = 0; i < ssize[ti]; i++) {
						items.push_back(landotc_t(landot_diameter, landot_gapsize, landot_stroke, nullptr, Matrix3x2F::Rotation(item_ori[i])));
					}

					//gen search screen
					sketch_t search_scr(bgcolor);
					vector<D2D1_POINT_2F> item_pos = search_table(search_scr, 6, 4, SizeF(600, 400), win.center, margin_size, drawable_holder_vec_to_ptr_vec(items));

					//show screens
					win.PresentScreensFor(iti_scr, 500ms - (clock::now() - trial_start_time));
					win.PresentScreensFor(fixation_scr, 500ms);
					auto start_time = win.PresentScreens(search_scr);

#ifdef VS_USE_PRINT
					save_screen_as_file(search_scr, to_wstring(exptid + L".xps"), win.d2d_rect++.size());
#endif

					auto response = wait_press(win, { VK_LBUTTON,VK_RBUTTON,VK_F9 });
					if (response.index == 2) break;

					bool correct = 0;
					if (pa[ti] != response.index) correct = 1;
					if (!correct) {
						if (pa[ti]) win.PresentScreensFor(miss_scr, error_scr_duration);
						else win.PresentScreensFor(fa_scr, error_scr_duration);
					}

					//save trial data
					trial_data_t trial_data{
						exptid,
						subjid,
						bi,
						ti - npt,
						rare_first,
						is_rare,
						ssize[ti],
						pa[ti],
						response.index,
						correct,
						static_cast<float>(to_ms(response.receive_time - start_time)),
						item_pos,
						item_ori,
					};
					trial_datas.push_back(trial_data);

					//output
					outf << exptid << "\t";
					outf << subjid << "\t";
					outf << bi << "\t";
					outf << ti - npt << "\t";
					outf << rare_first << "\t";
					outf << is_rare << "\t";
					outf << ssize[ti] << "\t";
					outf << pa[ti] << "\t";
					outf << response.index << "\t";
					outf << correct << "\t";
					outf << to_ms(response.receive_time - start_time) << "\t";
					outf << endl;
				} //end trial loop
			};

			auto show_instruction_with_stim = [&](win_t& win, wstring instruction_str, text_format_t text_format = text_format_t().SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER), brush_t text_brush = ColorF(ColorF::White), D2D1_COLOR_F bg_color = ColorF(ColorF::Black)) {
				wait_press_form_t instruction_form(win, theme_t::dark);
				drawable_control_t<label_t1> instruction_control(instruction_form, control_position_t(align_bound_center, align_bound_center), label_t1(instruction_str, text_brush, text_format));
				//aligned_control_group_t bar_group(instruction_form, control_position_t(align_top_center, align_bottom_center, instruction_control, SizeF(0, 20)), align_middle, landot_diameter);
				//drawable_control_t<landotc_t> left_landot(bar_group, nullptr, landotc_t(landot_diameter, landot_gapsize, landot_stroke, nullptr, Matrix3x2F::Rotation(-target_ori)));
				//drawable_control_t<landotc_t> right_landot(bar_group, nullptr, landotc_t(landot_diameter, landot_gapsize, landot_stroke, nullptr, Matrix3x2F::Rotation(target_ori)));
				app.Run();
			};


			//run practice
			wstring rare_msg = (is_rare ? L"TARGET IS RARE\n\n" : L"TARGET IS FREQUENT\n\n");
			show_instruction_with_stim(win, rare_msg + L"Press any key to start PRACTICE ");
			run_trial(bbi - nbb, practice_nt, 0);

			//run actual
			for (int bi = 0; bi < nb; bi++) {
				show_instruction_with_stim(win, rare_msg + L"Press any key to start BLOCK " + to_wstring(bi + 1));
				run_trial(bbi*nb + bi, expt_nt, expt_npt);
			}

		} // end block loop

		//say goodbye
		show_instruction(win, L"Finish! Thank you very much for your participation.");

	}
	catch (quitapp_t) {
		cout << "Goodbye." << endl;
	}
	catch (exception e) {
		messagebox::notify(e.what());
	}
	return 0;
}

