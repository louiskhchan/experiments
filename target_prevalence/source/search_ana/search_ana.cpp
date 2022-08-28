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
	//generated members
	bool valid;
	bool rtin; //rt within range
	int total_ti;
	int seq_subjid; //a sequential subjid that starts from 0 to nsubj-1
};


int main()
{
	try {
		app.init();
		wstring exptid = L"prev7";
		path data_path = L"../Data";
		path output_path = L"../Outputs";

		//get data
		auto all_data = get_data_from_file<trial_data_t>(data_path, exptid + L"_(\\d+)_contrast_data\\.dat");

		{
			if (false) {//generate fake data
//				boost::math::inverse_gaussian_distribution<float> igd(0,1);
				normal_distribution<float> nd(0, 1);
				for (auto& subj_data : all_data) {


					float rt_offset = nd(app.MTRand);

					boost::math::inverse_gaussian_distribution<double> igd1(1625 + rt_offset * 288, 10839);
					boost::math::inverse_gaussian_distribution<double> igd2(958 + rt_offset * 188, 6027);
					boost::math::inverse_gaussian_distribution<double> igd3(1117 + rt_offset * 283, 6992);
					boost::math::inverse_gaussian_distribution<double> igd4(973 + rt_offset * 201, 10277);

					auto gen_rand = [&](auto igd) {
						float out;
						do {
							out = (float)boost::math::quantile(igd, app.rand_float_exclusive(0, 1));
						} while (!isfinite(out));
						return out;
					};


					float acc_offset = nd(app.MTRand)*1.2f;
					float cond1acc = inverse_logit(max(0.f, 2.93f + acc_offset));
					float cond2acc = inverse_logit(max(0.f, 3.62f + acc_offset));
					float cond3acc = inverse_logit(max(0.f, 4.77f + acc_offset));
					float cond4acc = inverse_logit(max(0.f, 1.96f + acc_offset));
					for (auto& this_trial : subj_data) {
						if (this_trial.is_rare == 0 && this_trial.pa == 0) this_trial.rt = gen_rand(igd1);
						if (this_trial.is_rare == 0 && this_trial.pa == 1) this_trial.rt = gen_rand(igd2);
						if (this_trial.is_rare == 1 && this_trial.pa == 0) this_trial.rt = gen_rand(igd3);
						if (this_trial.is_rare == 1 && this_trial.pa == 1) this_trial.rt = gen_rand(igd4);

						if (this_trial.is_rare == 0 && this_trial.pa == 0) this_trial.correct = (app.rand_float(0, 1) < cond1acc);
						if (this_trial.is_rare == 0 && this_trial.pa == 1) this_trial.correct = (app.rand_float(0, 1) < cond2acc);
						if (this_trial.is_rare == 1 && this_trial.pa == 0) this_trial.correct = (app.rand_float(0, 1) < cond3acc);
						if (this_trial.is_rare == 1 && this_trial.pa == 1) this_trial.correct = (app.rand_float(0, 1) < cond4acc);

					}
				}
			}
			//sort data into conditions
			auto get_cond_func = [](const vector<trial_data_t>& subj_data) {
				map<int, vector<trial_data_t>> subj_conds_data;
				//sort data into conditions
				for (auto this_trial : subj_data) {
					int cond = this_trial.is_rare + 1;
					if (cond > 0) subj_conds_data[cond].push_back(this_trial);
				}
				return subj_conds_data;
			};
			auto including_conds = seq_by(1, 2);
			auto get_btw_cond_func = [](const vector<trial_data_t>& subj_data) {
				return vector<string>{to_string(subj_data.front().rare_first)};
			};

			//categorize data
			categorized_data_t<trial_data_t> categorized_data(all_data, get_cond_func, including_conds, get_btw_cond_func, output_path, exptid);

			//analyze
			//categorized_data.analyze_rtmean();
			//categorized_data.analyze_accuracy();
			//categorized_data.analyze_n();
			//categorized_data.analyze_setsize();
			categorized_data.analyze_sdt<sdt_extreme_value_handle_option_t::clamp>();
			//pa categorized analysis
			auto pa_categorized_data = categorized_data.get_pa_categorized_data();
			pa_categorized_data.analyze_rtmean();
			pa_categorized_data.analyze_zrtmean();
			pa_categorized_data.analyze_rankmean();
			pa_categorized_data.analyze_accuracy();
			pa_categorized_data.analyze_logit<sdt_extreme_value_handle_option_t::loglinear>();
			pa_categorized_data.analyze_n();
			pa_categorized_data.analyze_ssize();
			//ssize categorized analysis
			auto pa_ssize_categorized_data = pa_categorized_data.get_ssize_categorized_data();
			pa_ssize_categorized_data.analyze_rtmean();
			pa_ssize_categorized_data.analyze_accuracy();
		}

	}
	catch (quitapp_t) {
		cout << "Goodbye." << endl;
	}
	catch (exception e) {
		messagebox::notify(e.what());
	}
	return 0;
}

